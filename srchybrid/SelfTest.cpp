#include "stdafx.h"
#include "MD4.h"
#include "UploadPolicy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
static UINT g_uResNumber;
static UINT g_uTotalSize;

static BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR) noexcept
{
	++g_uResNumber;
	UINT uSize = 0;
	HRSRC hResInfo = FindResource(hModule, lpszName, lpszType);
	if (hResInfo) {
		uSize = SizeofResource(hModule, hResInfo);
		g_uTotalSize += uSize;
	}
#if 0
	TRACE(_T("%3u: "), g_uResNumber);
	if (IS_INTRESOURCE(lpszType)) {
		if ((DWORD)lpszType == (DWORD)RT_GROUP_ICON)
			TRACE(_T("RT_GROUP_ICON"));
		else if ((DWORD)lpszType == (DWORD)RT_ICON)
			TRACE(_T("RT_ICON"));
		else if ((DWORD)lpszType == (DWORD)RT_BITMAP)
			TRACE(_T("RT_BITMAP"));
		else
			TRACE(_T("type=%u"), (UINT)lpszType);
	} else
		TRACE(_T("type=\"%s\""), lpszType);
	TRACE(_T("  size=%5u"), uSize);
	if (IS_INTRESOURCE(lpszName))
		TRACE(_T("  name=*%u"), (UINT)lpszName);
	else
		TRACE(_T("  name=\"%s\""), lpszName);
	TRACE(_T("\n"));
#endif
	return TRUE;
}

bool CheckResources()
{
	g_uTotalSize = 0;
	g_uResNumber = 0;
	EnumResourceNames(AfxGetInstanceHandle(), RT_GROUP_ICON, EnumResNameProc, 0);
	TRACE("RT_GROUP_ICON resources: %u (%u bytes)\n", g_uResNumber, g_uTotalSize);

	g_uTotalSize = 0;
	g_uResNumber = 0;
	EnumResourceNames(AfxGetInstanceHandle(), RT_ICON, EnumResNameProc, 0);
	TRACE("RT_ICON resources: %u (%u bytes)\n", g_uResNumber, g_uTotalSize);

	g_uTotalSize = 0;
	g_uResNumber = 0;
	EnumResourceNames(AfxGetInstanceHandle(), RT_BITMAP, EnumResNameProc, 0);
	TRACE("RT_BITMAP resources: %u (%u bytes)\n", g_uResNumber, g_uTotalSize);

	return true;
}
#endif

/*
int fooAsCode(int a)
{
	return a + 30;
00401000 8B 44 24 04      mov         eax,dword ptr [esp+4]
00401004 83 C0 1E         add         eax,1Eh
00401007 C3               ret
}
*/
unsigned char fooAsData[] = {
	0x8B,0x44,0x24,0x04,
	0x83,0xC0,0x1E,
	0xC3
};

extern "C" int(*convertDataAddrToCodeAddr(void *p))(int)
{
	return (int(*)(int))p;
}

int g_fooResult;

static bool CheckMD4Vector(const char *input, const byte (&expected)[MD4_DIGEST_SIZE])
{
	CMD4 md4;
	md4.Add(input, strlen(input));
	md4.Finish();
	return memcmp(md4.GetHash(), expected, MD4_DIGEST_SIZE) == 0;
}

static bool CheckHashing()
{
	static const byte emptyHash[MD4_DIGEST_SIZE] =
		{0x31, 0xd6, 0xcf, 0xe0, 0xd1, 0x6a, 0xe9, 0x31, 0xb7, 0x3c, 0x59, 0xd7, 0xe0, 0xc0, 0x89, 0xc0};
	static const byte aHash[MD4_DIGEST_SIZE] =
		{0xbd, 0xe5, 0x2c, 0xb3, 0x1d, 0xe3, 0x3e, 0x46, 0x24, 0x5e, 0x05, 0xfb, 0xdb, 0xd6, 0xfb, 0x24};
	static const byte abcHash[MD4_DIGEST_SIZE] =
		{0xa4, 0x48, 0x01, 0x7a, 0xaf, 0x21, 0xd8, 0x52, 0x5f, 0xc1, 0x0a, 0xe8, 0x7a, 0xa6, 0x72, 0x9d};
	return CheckMD4Vector("", emptyHash)
		&& CheckMD4Vector("a", aHash)
		&& CheckMD4Vector("abc", abcHash);
}

static bool CheckUploadPolicy()
{
	const uint32 targetRate = 3u * 1024u;
	return UploadPolicy::GetTargetClientDataRate(0, false) == targetRate
		&& UploadPolicy::GetTargetClientDataRate(3, true) == targetRate * 3u / 4u
		&& UploadPolicy::GetTargetClientDataRate(30, false) == UPLOAD_CLIENT_MAXDATARATE
		&& UploadPolicy::GetSlotLimit(0, targetRate) == MIN_UP_CLIENTS_ALLOWED
		&& UploadPolicy::GetSlotLimit(10u * 1024u, targetRate) == MIN_UP_CLIENTS_ALLOWED + 1u
		&& UploadPolicy::GetSlotLimit(17u * 1024u, targetRate) == MIN_UP_CLIENTS_ALLOWED + 2u
		&& UploadPolicy::GetSlotLimit(26u * 1024u, targetRate) >= MIN_UP_CLIENTS_ALLOWED + 3u
		&& UploadPolicy::ShouldOpenUnlimitedSlot(MIN_UP_CLIENTS_ALLOWED + 2u, 0, targetRate)
		&& !UploadPolicy::ShouldOpenUnlimitedSlot(MIN_UP_CLIENTS_ALLOWED + 3u, 0, targetRate)
		&& UploadPolicy::ShouldOpenUnlimitedSlot(MIN_UP_CLIENTS_ALLOWED + 3u, 30u * 1024u, targetRate);
}

static bool CheckLanguageModules()
{
	TCHAR modulePath[MAX_PATH];
	DWORD length = GetModuleFileName(NULL, modulePath, _countof(modulePath));
	if (length == 0 || length >= _countof(modulePath))
		return false;

	CString languageDirectory(modulePath);
	int separator = languageDirectory.ReverseFind(_T('\\'));
	if (separator < 0)
		return false;
	languageDirectory = languageDirectory.Left(separator + 1) + _T("lang\\");

	WIN32_FIND_DATA findData = {};
	HANDLE findHandle = FindFirstFile(languageDirectory + _T("*.dll"), &findData);
	if (findHandle == INVALID_HANDLE_VALUE)
		return false;

	UINT languageCount = 0;
	bool modulesValid = true;
	do {
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
			|| _tcsicmp(findData.cFileName, _T("eMuleNext-GraphicsTest.dll")) == 0)
		{
			continue;
		}

		HMODULE module = LoadLibraryEx(languageDirectory + findData.cFileName, NULL,
			LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
		if (module == NULL) {
			modulesValid = false;
			break;
		}
		FreeLibrary(module);
		++languageCount;
	} while (FindNextFile(findHandle, &findData));
	FindClose(findHandle);

	return modulesValid && languageCount == 43;
}

bool SelfTest(bool extended)
{
	// Win98/WinME PROBLEM: Those Windows version have some icon resource limit.
	// That limit seems to be a combination of the total amount of icon (image)
	// data which is used by all icons as well as the total number of icon resources
	// which are listed here in that section. We already exceeded that limit and we
	// should take care about the order of the icons in that list. We would need to
	// place icons which are never used under Win98/WinME at the end of the list so
	// that all other icons have a chance to get loaded. It is though not easy to
	// find that kind of icons. So, for now, as a quick fix, the smiley icons are
	// placed at the end of the list - as they are for sure the least important ones.
	//
	// However, note also that it leads to quite serious problems if some particular
	// icons can not get loaded (under Win98/ME). If those icons are used within an
	// image list and can not get loaded, the remaining icons in that list will
	// change their position within the list which leads to the situation that the
	// user will see semantically *wrong* icons (e.g. seeing the 'connected' state
	// icon although the 'disconnect' state should be shown) - and this is actually
	// even worse than showing no icons at all.
	//
	// It seems that the total amount of icon (image) must not exceed sharp 1.0 MB.
	// All icons which are placed in the icon section of the resource file above
	// that 1.0 MB limit can not get loaded by Win98/ME. Note also, if the icon
	// resource section exceeds some other certain limit, the EXE file itself can
	// not get loaded by Win98/ME any longer.
	//
	// (See also "CheckResources" in SelfTest.cpp)
	//
	// TODO: Maybe we can put the icons in different language sections in the
	// rc file to avoid that Win98 restriction.
	//
#ifdef _DEBUG
	//CheckResources();
#endif

	// Test DEP
	//int (* volatile pfnFooAsData)(int) = (int (*)(int))convertDataAddrToCodeAddr(fooAsData);
	//g_fooResult = (*pfnFooAsData)(5);

	// Test a crash
	//*(int*)0=0;

	if (!extended)
		return true;

	return CheckHashing() && CheckUploadPolicy() && CheckLanguageModules();
}
