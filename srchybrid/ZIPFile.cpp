//
// ZIPFile.cpp
//
// Copyright (c) Shareaza Development Team, 2002-2004.
// This file is part of SHAREAZA (www.shareaza.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "StdAfx.h"
#include "ZIPFile.h"
#include "zlib/zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CZIPFile construction

CZIPFile::CZIPFile(HANDLE hAttach)
	: m_hFile(INVALID_HANDLE_VALUE)
	, m_pFile()
	, m_nFile()
	, m_bAttach()
{
	if (hAttach != INVALID_HANDLE_VALUE)
		Attach(hAttach);
}

CZIPFile::~CZIPFile()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile open

bool CZIPFile::Open(LPCTSTR pszFile)
{
	ASSERT(pszFile != NULL);

	Close();

	m_bAttach = false;
	m_hFile = ::CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL
						, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	if (LocateCentralDirectory())
		return true;

	Close();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile attach

bool CZIPFile::Attach(HANDLE hFile)
{
	ASSERT(hFile != INVALID_HANDLE_VALUE);

	Close();

	m_bAttach = true;
	m_hFile = hFile;

	if (LocateCentralDirectory())
		return true;

	Close();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile open test

bool CZIPFile::IsOpen() const
{
	return m_hFile != INVALID_HANDLE_VALUE;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile close

void CZIPFile::Close()
{
	if (IsOpen()) {
		if (!m_bAttach)
			::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	delete[] m_pFile;
	m_pFile = NULL;
	m_nFile = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile get a particular file

CZIPFile::File* CZIPFile::GetFile(int nFile) const
{
	return (nFile < 0 || nFile >= m_nFile) ? NULL : m_pFile + nFile;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile lookup a file by name

CZIPFile::File* CZIPFile::GetFile(LPCTSTR pszFile, BOOL bPartial) const
{
	File *pFile = m_pFile;
	for (int nFile = m_nFile; --nFile >= 0;) {
		LPCTSTR pszName = bPartial ? _tcsrchr(pFile->m_sName, '/') : NULL;
		pszName = pszName ? pszName + 1 : (LPCTSTR)pFile->m_sName;
		if (_tcsicoll(pszName, pszFile) == 0)
			return pFile;
		++pFile;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile locate the central directory

#pragma pack(push, 1)
typedef struct
{
	DWORD	nSignature;			// 0x06054b50
	WORD	nThisDisk;
	WORD	nDirectoryDisk;
	WORD	nFilesThisDisk;
	WORD	nTotalFiles;
	DWORD	nDirectorySize;
	DWORD	nDirectoryOffset;
	WORD	nCommentLen;
} ZIP_DIRECTORY_LOC;
#pragma pack(pop)

bool CZIPFile::LocateCentralDirectory()
{
	BYTE pBuffer[4096];
	const DWORD fileSize = GetFileSize(m_hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE || fileSize < sizeof(ZIP_DIRECTORY_LOC))
		return false;

	const DWORD bytesToRead = min(fileSize, static_cast<DWORD>(sizeof pBuffer));
	DWORD nBuffer = 0;

	if (SetFilePointer(m_hFile, -static_cast<LONG>(bytesToRead), NULL, FILE_END) == INVALID_SET_FILE_POINTER
		&& GetLastError() != NO_ERROR)
	{
		return false;
	}
	if (!::ReadFile(m_hFile, pBuffer, bytesToRead, &nBuffer, NULL) || nBuffer != bytesToRead)
		return false;

	ZIP_DIRECTORY_LOC directory = {};
	bool foundDirectory = false;
	for (INT_PTR nScan = static_cast<INT_PTR>(nBuffer) - sizeof(ZIP_DIRECTORY_LOC); nScan >= 0; --nScan) {
		ZIP_DIRECTORY_LOC candidate;
		memcpy(&candidate, &pBuffer[nScan], sizeof candidate);
		if (candidate.nSignature == 0x06054b50
			&& static_cast<DWORD>(nScan) + sizeof candidate + candidate.nCommentLen == nBuffer)
		{
			directory = candidate;
			foundDirectory = true;
			break;
		}
	}
	if (!foundDirectory || directory.nDirectorySize == 0 || directory.nTotalFiles == 0)
		return false;

	if (directory.nDirectoryOffset > fileSize || directory.nDirectorySize > fileSize - directory.nDirectoryOffset)
		return false;

	if (SetFilePointer(m_hFile, directory.nDirectoryOffset, NULL, FILE_BEGIN) != directory.nDirectoryOffset)
		return false;

	BYTE *pDirectory = new BYTE[directory.nDirectorySize];
	if (!::ReadFile(m_hFile, pDirectory, directory.nDirectorySize, &nBuffer, NULL)) {
		delete[] pDirectory;
		return false;
	}

	if (nBuffer == directory.nDirectorySize) {
		m_nFile = static_cast<int>(directory.nTotalFiles);
		m_pFile = new File[m_nFile];
		if (!ParseCentralDirectory(pDirectory, nBuffer)) {
			m_nFile = 0;
			delete[] m_pFile;
			m_pFile = NULL;
		}
	}
	delete[] pDirectory;

	return m_nFile > 0;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile parse the central directory

#pragma pack(push, 1)
typedef struct
{
	DWORD	nSignature;		// 0x02014b50
	WORD	nWriteVersion;
	WORD	nReadVersion;
	WORD	nFlags;
	WORD	nCompression;
	WORD	nFileTime;
	WORD	nFileDate;
	DWORD	nCRC;
	DWORD	nCompressedSize;
	DWORD	nActualSize;
	WORD	nNameLen;
	WORD	nExtraLen;
	WORD	nCommentLen;
	WORD	nStartDisk;
	WORD	nInternalAttr;
	DWORD	nExternalAttr;
	DWORD	nLocalOffset;
} ZIP_CENTRAL_FILE;
#pragma pack(pop)

bool CZIPFile::ParseCentralDirectory(BYTE *pDirectory, DWORD nDirectory)
{
	for (int nFile = 0; nFile < m_nFile; ++nFile) {
		if (nDirectory < sizeof(ZIP_CENTRAL_FILE))
			return false;

		ZIP_CENTRAL_FILE record;
		memcpy(&record, pDirectory, sizeof record);
		if (record.nSignature != 0x02014b50)
			return false;

		pDirectory += sizeof record;
		nDirectory -= sizeof record;

		const DWORD tailLength = static_cast<DWORD>(record.nNameLen) + record.nExtraLen + record.nCommentLen;
		if (nDirectory < tailLength)
			return false;

		m_pFile[nFile].m_pZIP = this;
		m_pFile[nFile].m_nSize = record.nActualSize;
		m_pFile[nFile].m_nLocalOffset = record.nLocalOffset;
		m_pFile[nFile].m_nCompressedSize = record.nCompressedSize;
		m_pFile[nFile].m_nCompression = record.nCompression;

		LPTSTR pszName = m_pFile[nFile].m_sName.GetBuffer(record.nNameLen);

		for (WORD nChar = 0; nChar < record.nNameLen; ++nChar) {
			pszName[nChar] = (TCHAR)pDirectory[nChar];
			if (pszName[nChar] == '\\')
				pszName[nChar] = '/';
		}

		m_pFile[nFile].m_sName.ReleaseBuffer(record.nNameLen);

		pDirectory += tailLength;
		nDirectory -= tailLength;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile::File seek to a file

#pragma pack(push, 1)
typedef struct
{
	DWORD	nSignature;		// 0x04034b50
	WORD	nVersion;
	WORD	nFlags;
	WORD	nCompression;
	WORD	nFileTime;
	WORD	nFileDate;
	DWORD	nCRC;
	DWORD	nCompressedSize;
	DWORD	nActualSize;
	WORD	nNameLen;
	WORD	nExtraLen;
} ZIP_LOCAL_FILE;
#pragma pack(pop)

bool CZIPFile::SeekToFile(const File *pFile)
{
	ASSERT(pFile != NULL && pFile->m_pZIP == this);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;
	const DWORD fileSize = GetFileSize(m_hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE || pFile->m_nLocalOffset > fileSize
		|| sizeof(ZIP_LOCAL_FILE) > fileSize - pFile->m_nLocalOffset)
	{
		return false;
	}

	if (SetFilePointer(m_hFile, (DWORD)pFile->m_nLocalOffset, NULL, FILE_BEGIN) != pFile->m_nLocalOffset)
		return false;

	ZIP_LOCAL_FILE pLocal;
	DWORD nRead = 0;
	if (!::ReadFile(m_hFile, &pLocal, sizeof pLocal, &nRead, NULL) || nRead != sizeof pLocal)
		return false;

	if (pLocal.nSignature != 0x04034b50)
		return false;
	if (pLocal.nCompression != Z_DEFLATED && pLocal.nCompression != 0)
		return false;

	const uint64 dataOffset = pFile->m_nLocalOffset + sizeof pLocal + pLocal.nNameLen + pLocal.nExtraLen;
	if (dataOffset > fileSize || pFile->m_nCompressedSize > fileSize - dataOffset)
		return false;
	if (SetFilePointer(m_hFile, pLocal.nNameLen + pLocal.nExtraLen, NULL, FILE_CURRENT) != dataOffset)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile::File prepare to decompress

bool CZIPFile::File::PrepareToDecompress(LPVOID pStream)
{
	memset(pStream, 0, sizeof z_stream);

	if (m_pZIP->SeekToFile(this)) {
		if (m_nCompression == 0)
			return m_nSize == m_nCompressedSize;

		if (m_nCompression == Z_DEFLATED)
			return Z_OK == inflateInit2((z_stream*)pStream, -MAX_WBITS);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile::File decompress to memory

/*CBuffer* CZIPFile::File::Decompress()
{
	z_stream pStream;

	if (m_nSize > 32 * 1024 * 1024)
		return NULL;
	if (!PrepareToDecompress(&pStream))
		return NULL;

	if (m_nCompression == 0) {
		CBuffer *pTarget = new CBuffer();
		pTarget->EnsureBuffer((DWORD)m_nSize);
		::ReadFile(m_pZIP->m_hFile, pTarget->m_pBuffer, (DWORD)m_nSize, &pTarget->m_nLength, NULL);
		if (pTarget->m_nLength == (DWORD)m_nSize)
			return pTarget;
		delete pTarget;
		return NULL;
	}

	DWORD nSource = (DWORD)m_nCompressedSize;
	DWORD rSource;
	BYTE *pSource = new BYTE[nSource];
	::ReadFile(m_pZIP->m_hFile, pSource, nSource, &rSource, NULL);

	if (nSource != rSource) {
		inflateEnd(&pStream);
		delete[] pSource;
		return NULL;
	}

	CBuffer *pTarget = new CBuffer();
	pTarget->EnsureBuffer((DWORD)m_nSize);
	pTarget->m_nLength = (DWORD)m_nSize;

	pStream.next_in = pSource;
	pStream.avail_in = (DWORD)m_nCompressedSize;
	pStream.next_out = pTarget->m_pBuffer;
	pStream.avail_out = pTarget->m_nLength;

	inflate(&pStream, Z_FINISH);

	delete[] pSource;

	if (pStream.avail_out != 0) {
		delete pTarget;
		pTarget = NULL;
	}

	inflateEnd(&pStream);

	return pTarget;
}*/

/////////////////////////////////////////////////////////////////////////////
// CZIPFile::File decompress to disk

#define BUFFER_IN_SIZE		(64*1024)
#define BUFFER_OUT_SIZE		(128*1024)

bool CZIPFile::File::Extract(LPCTSTR pszFile)
{
	z_stream pStream;
	HANDLE hFile;

	hFile = ::CreateFile(pszFile, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	if (!PrepareToDecompress(&pStream)) {
		::CloseHandle(hFile);
		::DeleteFile(pszFile);
		return false;
	}

	uint64 nUncompressed = 0;
	BYTE *pBufferOut = new BYTE[BUFFER_OUT_SIZE];

	if (m_nCompression == Z_DEFLATED) {
		BYTE *pBufferIn = new BYTE[BUFFER_IN_SIZE];

		for (uint64 nCompressed = 0; nCompressed < m_nCompressedSize || nUncompressed < m_nSize;) {
			if (pStream.avail_in == 0) {
				const uint64 remainingCompressed = m_nCompressedSize - nCompressed;
				if (remainingCompressed == 0)
					break;
				pStream.avail_in = (DWORD)min(remainingCompressed, BUFFER_IN_SIZE);
				pStream.next_in = pBufferIn;

				DWORD nRead = 0;
				if (!::ReadFile(m_pZIP->m_hFile, pBufferIn, pStream.avail_in, &nRead, NULL)
					|| nRead != pStream.avail_in)
					break;
				nCompressed += nRead;
			}

			pStream.avail_out = BUFFER_OUT_SIZE;
			pStream.next_out = pBufferOut;

			const int inflateResult = inflate(&pStream, Z_SYNC_FLUSH);
			if (inflateResult != Z_OK && inflateResult != Z_STREAM_END)
				break;

			if (pStream.avail_out < BUFFER_OUT_SIZE) {
				DWORD nWrite = BUFFER_OUT_SIZE - pStream.avail_out;
				if (nUncompressed > m_nSize || nWrite > m_nSize - nUncompressed)
					break;
				DWORD nWritten;
				if (!::WriteFile(hFile, pBufferOut, nWrite, &nWritten, NULL) || nWritten != nWrite)
					break;
				nUncompressed += nWrite;
			}
			if (inflateResult == Z_STREAM_END)
				break;
		}

		delete[] pBufferIn;

		inflateEnd(&pStream);
	} else
		while (nUncompressed < m_nSize) {
			DWORD nChunk = (DWORD)min(m_nSize - nUncompressed, BUFFER_OUT_SIZE);
			DWORD nProcess;

			VERIFY(::ReadFile(m_pZIP->m_hFile, pBufferOut, nChunk, &nProcess, NULL));
			if (nChunk != nProcess)
				break;
			::WriteFile(hFile, pBufferOut, nChunk, &nProcess, NULL);
			if (nChunk != nProcess)
				break;
			nUncompressed += nChunk;
		}

	delete[] pBufferOut;
	::CloseHandle(hFile);

	if (nUncompressed >= m_nSize)
		return true;

	::DeleteFile(pszFile);
	return false;
}
