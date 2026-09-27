#include "stdafx.h"
#include "PrivateDiagnostics.h"

#ifdef EMULENEXT_PRIVATE_DIAGNOSTICS

#include <Psapi.h>
#include "emule.h"
#include "Preferences.h"
#include "Statistics.h"
#include "UploadQueue.h"
#include "MD4.h"
#include "DownloadQueue.h"
#include "SharedFileList.h"
#include "ServerConnect.h"
#include "Kademlia/Kademlia/Kademlia.h"
#include "TickCountHelpers.h"
#include "Opcodes.h"

#pragma comment(lib, "psapi.lib")

namespace
{
	const ULONGLONG kMaximumDiagnosticFileSize = 50ui64 * 1024ui64 * 1024ui64;
	CCriticalSection s_diagnosticLock;
	CString s_diagnosticPath;
	uint64 s_sessionId;
	DWORD s_sessionStartTick;
	DWORD s_mainThreadId;
	DWORD s_lastSampleTick;
	DWORD s_lastUploadDecisionTick;
	CStringA s_lastUploadDecision;
	bool s_initialized;

	uint64 FileTimeToMilliseconds(const FILETIME& value)
	{
		ULARGE_INTEGER integerValue = {};
		integerValue.LowPart = value.dwLowDateTime;
		integerValue.HighPart = value.dwHighDateTime;
		return integerValue.QuadPart / 10000ui64;
	}

	void SelectRotatedPath(const SYSTEMTIME& utc)
	{
		s_diagnosticPath.Format(_T("%sprivate-general-diagnostics-%04u%02u%02u-%02u%02u%02u-%lu.csv"),
			(LPCTSTR)thePrefs.GetMuleDirectory(EMULE_CONFIGDIR), utc.wYear, utc.wMonth,
			utc.wDay, utc.wHour, utc.wMinute, utc.wSecond, ::GetCurrentProcessId());
	}

	void WriteRowLocked(const char* category, const char* action,
		uint64 value1, uint64 value2, uint64 value3)
	{
		if (!s_initialized)
			return;

		PROCESS_MEMORY_COUNTERS_EX memory = {};
		memory.cb = sizeof(memory);
		(void)::GetProcessMemoryInfo(::GetCurrentProcess(),
			reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory), sizeof(memory));
		DWORD handleCount = 0;
		(void)::GetProcessHandleCount(::GetCurrentProcess(), &handleCount);
		FILETIME creationTime = {}, exitTime = {}, kernelTime = {}, userTime = {};
		(void)::GetProcessTimes(::GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);

		uint32 uploadRate = 0;
		uint32 downloadRate = 0;
		INT_PTR waitingUploads = 0;
		INT_PTR uploadSlots = 0;
		INT_PTR downloadFiles = 0;
		INT_PTR sharedFiles = 0;
		INT_PTR hashingFiles = 0;
		bool ed2kConnected = false;
		bool kadConnected = false;
		bool kadFirewalled = false;
		// The hashing worker also emits events.  Only the UI thread may inspect the
		// MFC containers owned by the application; worker rows still contain their
		// event values and process metrics, while periodic UI-thread snapshots carry
		// the complete queue, library and connection state.
		if (::GetCurrentThreadId() == s_mainThreadId) {
			uploadRate = theApp.uploadqueue != NULL ? theApp.uploadqueue->GetDatarate() : 0;
			downloadRate = theApp.downloadqueue != NULL ? theApp.downloadqueue->GetDatarate() : 0;
			waitingUploads = theApp.uploadqueue != NULL ? theApp.uploadqueue->GetWaitingUserCount() : 0;
			uploadSlots = theApp.uploadqueue != NULL ? theApp.uploadqueue->GetUploadQueueLength() : 0;
			downloadFiles = theApp.downloadqueue != NULL ? theApp.downloadqueue->GetFileCount() : 0;
			sharedFiles = theApp.sharedfiles != NULL ? theApp.sharedfiles->GetCount() : 0;
			hashingFiles = theApp.sharedfiles != NULL ? theApp.sharedfiles->GetHashingCount() : 0;
			ed2kConnected = theApp.serverconnect != NULL && theApp.serverconnect->IsConnected();
			kadConnected = Kademlia::CKademlia::IsConnected();
			kadFirewalled = kadConnected && Kademlia::CKademlia::IsFirewalled();
		}

		SYSTEMTIME utc = {};
		::GetSystemTime(&utc);
		CFile file;
		CFileException error;
		if (!file.Open(s_diagnosticPath, CFile::modeCreate | CFile::modeWrite
			| CFile::modeNoTruncate | CFile::shareDenyWrite, &error))
		{
			return;
		}
		if (file.GetLength() >= kMaximumDiagnosticFileSize) {
			file.Close();
			SelectRotatedPath(utc);
			if (!file.Open(s_diagnosticPath, CFile::modeCreate | CFile::modeWrite
				| CFile::modeNoTruncate | CFile::shareDenyWrite, &error))
			{
				return;
			}
		}

		const bool writeHeader = file.GetLength() == 0;
		file.SeekToEnd();
		if (writeHeader) {
			static const char header[] =
				"timestamp_utc,session_id,architecture,thread_id,uptime_ms,category,action,"
				"value_1,value_2,value_3,working_set_bytes,private_bytes,handle_count,"
				"kernel_cpu_ms,user_cpu_ms,upload_bytes_per_sec,download_bytes_per_sec,"
				"waiting_upload_users,open_upload_slots,download_files,shared_files,hashing_files,"
				"session_uploaded_bytes,session_downloaded_bytes,max_upload_kib_per_sec,"
				"max_download_kib_per_sec,uss_enabled,ed2k_connected,kad_connected,kad_firewalled\r\n";
			file.Write(header, sizeof(header) - 1);
		}

#ifdef _WIN64
		static const char architecture[] = "x64";
#else
		static const char architecture[] = "x86";
#endif
		CStringA line;
		line.Format(
			"%04u-%02u-%02uT%02u:%02u:%02uZ,%I64u,%s,%lu,%lu,%s,%s,"
			"%I64u,%I64u,%I64u,%I64u,%I64u,%lu,%I64u,%I64u,%u,%u,"
			"%Id,%Id,%Id,%Id,%Id,%I64u,%I64u,%u,%u,%u,%u,%u,%u\r\n",
			utc.wYear, utc.wMonth, utc.wDay, utc.wHour, utc.wMinute, utc.wSecond,
			s_sessionId, architecture, ::GetCurrentThreadId(),
			static_cast<DWORD>(::GetTickCount() - s_sessionStartTick), category, action,
			value1, value2, value3, static_cast<uint64>(memory.WorkingSetSize),
			static_cast<uint64>(memory.PrivateUsage), handleCount,
			FileTimeToMilliseconds(kernelTime), FileTimeToMilliseconds(userTime), uploadRate, downloadRate,
			waitingUploads, uploadSlots, downloadFiles, sharedFiles, hashingFiles,
			CStatistics::sessionSentBytes, CStatistics::sessionReceivedBytes,
			thePrefs.GetMaxUpload(), thePrefs.GetMaxDownload(), thePrefs.IsDynUpEnabled() ? 1u : 0u,
			ed2kConnected ? 1u : 0u, kadConnected ? 1u : 0u, kadFirewalled ? 1u : 0u);
		file.Write(line.GetString(), line.GetLength());
	}
}

void PrivateDiagnostics::Initialize()
{
	CSingleLock lock(&s_diagnosticLock, TRUE);
	if (s_initialized)
		return;
	s_diagnosticPath = thePrefs.GetMuleDirectory(EMULE_CONFIGDIR) + _T("private-general-diagnostics.csv");
	s_sessionStartTick = ::GetTickCount();
	s_sessionId = (static_cast<uint64>(::GetCurrentProcessId()) << 32)
		| s_sessionStartTick;
	s_mainThreadId = ::GetCurrentThreadId();
	s_lastSampleTick = 0;
	s_lastUploadDecisionTick = 0;
	s_lastUploadDecision.Empty();
	s_initialized = true;
	WriteRowLocked("lifecycle", "startup", 0, 0, 0);
}

void PrivateDiagnostics::Shutdown()
{
	CSingleLock lock(&s_diagnosticLock, TRUE);
	if (!s_initialized)
		return;
	WriteRowLocked("lifecycle", "clean-shutdown", 0, 0, 0);
	s_initialized = false;
}

void PrivateDiagnostics::Sample()
{
	const DWORD now = ::GetTickCount();
	CSingleLock lock(&s_diagnosticLock, TRUE);
	if (!s_initialized || (s_lastSampleTick != 0
		&& !HasTickCountElapsed(now, s_lastSampleTick, SEC2MS(30))))
	{
		return;
	}
	s_lastSampleTick = now;
	WriteRowLocked("periodic", "snapshot", 0, 0, 0);
}

void PrivateDiagnostics::LogEvent(const char* category, const char* action,
	uint64 value1, uint64 value2, uint64 value3)
{
	CSingleLock lock(&s_diagnosticLock, TRUE);
	WriteRowLocked(category, action, value1, value2, value3);
}

void PrivateDiagnostics::LogUiCommand(UINT commandId, UINT notificationCode, bool hasControl)
{
	LogEvent("ui", "command", commandId, notificationCode, hasControl ? 1 : 0);
}

void PrivateDiagnostics::LogUiInteraction(UINT message, UINT controlId)
{
	LogEvent("ui", "mouse", message, controlId, 0);
}

void PrivateDiagnostics::LogUploadDecision(const char* decision, INT_PTR waitingUsers,
	INT_PTR openSlots, INT_PTR activeSlots)
{
	const DWORD now = ::GetTickCount();
	CSingleLock lock(&s_diagnosticLock, TRUE);
	if (!s_initialized)
		return;
	if (s_lastUploadDecision == decision && s_lastUploadDecisionTick != 0
		&& !HasTickCountElapsed(now, s_lastUploadDecisionTick, SEC2MS(30)))
	{
		return;
	}
	s_lastUploadDecision = decision;
	s_lastUploadDecisionTick = now;
	WriteRowLocked("upload-manager", decision, static_cast<uint64>(waitingUsers),
		static_cast<uint64>(openSlots), static_cast<uint64>(activeSlots));
}

#endif // EMULENEXT_PRIVATE_DIAGNOSTICS
