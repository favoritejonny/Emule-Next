#include "stdafx.h"
#include "MD4.h"
#include "KnownFile.h"
#include "UploadPolicy.h"
#include "ConnectionSetupPolicy.h"
#include "ConnectionSpeedTest.h"
#include "KadBootstrap.h"
#include "ServerBootstrap.h"
#include "LanguageSelection.h"
#include "Resource.h"
#include "Preferences.h"

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
	if (!CheckMD4Vector("", emptyHash)
		|| !CheckMD4Vector("a", aHash)
		|| !CheckMD4Vector("abc", abcHash))
	{
		return false;
	}

	// Exercise several reads across the production hashing buffer boundary.
	std::vector<byte> data(3 * 64 * 1024 + 17);
	for (size_t i = 0; i < data.size(); ++i)
		data[i] = static_cast<byte>((i * 131u + 17u) & 0xffu);
	CMD4 reference;
	reference.Add(data.data(), static_cast<uint32>(data.size()));
	reference.Finish();
	byte bufferedHash[MD4_DIGEST_SIZE] = {};
	return CKnownFile::CreateHash(data.data(), static_cast<uint32>(data.size()), bufferedHash)
		&& memcmp(bufferedHash, reference.GetHash(), MD4_DIGEST_SIZE) == 0;
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

static bool CheckLanguageSelection()
{
	const LANGID available[] = {LANGID_EN_US, LANGID_IT_IT, LANGID_DE_DE, LANGID_ES_ES_T,
		LANGID_PT_BR, LANGID_PT_PT, LANGID_ZH_CN, LANGID_ZH_TW, LANGID_AR_AE,
		LANGID_NB_NO, LANGID_NN_NO, LANGID_CA_ES, LANGID_VA_ES};
	const auto supported = [&available](LANGID language) {
		for (LANGID candidate : available)
			if (language == candidate)
				return true;
		return false;
	};
	const struct { LANGID saved; LANGID windows; LANGID expected; } cases[] = {
		{0, LANGID_IT_IT, LANGID_IT_IT},
		{0, MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN_SWISS), LANGID_IT_IT},
		{0, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN), LANGID_DE_DE},
		{0, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN), LANGID_ES_ES_T},
		{0, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), LANGID_EN_US},
		{0, LANGID_PT_BR, LANGID_PT_BR},
		{0, LANGID_PT_PT, LANGID_PT_PT},
		{0, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG), LANGID_ZH_TW},
		{0, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_MACAU), LANGID_ZH_TW},
		{0, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SINGAPORE), LANGID_ZH_CN},
		{0, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA), LANGID_AR_AE},
		{0, LANGID_NN_NO, LANGID_NN_NO},
		{0, MAKELANGID(LANG_CATALAN, 2), LANGID_VA_ES},
		{0, MAKELANGID(LANG_HINDI, SUBLANG_DEFAULT), LANGID_EN_US},
		{0, 0x1400, LANGID_EN_US}, // no usable Windows UI language identifier
		{0, 0, LANGID_EN_US},
		{LANGID_IT_IT, LANGID_EN_US, LANGID_IT_IT},
		{LANGID_EN_US, LANGID_IT_IT, LANGID_EN_US},
		{0xffff, LANGID_IT_IT, LANGID_EN_US}
	};
	for (const auto& test : cases)
		if (LanguageSelection::Resolve(test.saved, test.windows, supported) != test.expected)
			return false;
	const auto englishOnly = [](LANGID language) { return language == LANGID_EN_US; };
	const auto simplifiedOnly = [](LANGID language) { return language == LANGID_EN_US || language == LANGID_ZH_CN; };
	if (LanguageSelection::Resolve(0, LANGID_IT_IT, englishOnly) != LANGID_EN_US
		|| LanguageSelection::Resolve(LANGID_IT_IT, LANGID_DE_DE, englishOnly) != LANGID_EN_US
		|| LanguageSelection::Resolve(0, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG), simplifiedOnly) != LANGID_EN_US)
		return false;

	const LANGID originalLanguage = thePrefs.GetLanguageID();
	const LCID originalLocale = ::GetThreadLocale();
	const LANGID expected = LanguageSelection::Resolve(0, ::GetUserDefaultUILanguage(),
		[](LANGID language) { return CPreferences::IsLanguageSupported(language); });
	if (originalLanguage != expected)
		return false;
	// Exercise the production loader with regional formats deliberately set
	// to another language, without changing the Windows user's settings.
	bool valid = ::SetThreadLocale(MAKELCID(expected == LANGID_IT_IT ? LANGID_EN_US : LANGID_IT_IT, SORT_DEFAULT)) != FALSE;
	thePrefs.SetLanguageID(0);
	thePrefs.SetLanguage();
	valid = valid && thePrefs.GetLanguageID() == expected;
	const LANGID savedChoices[] = {LANGID_IT_IT, LANGID_EN_US, LANGID_FR_FR};
	for (LANGID language : savedChoices) {
		thePrefs.SetLanguageID(language);
		thePrefs.SetLanguage();
		thePrefs.SetLanguage(); // subsequent startup/change must keep the choice
		valid = valid && thePrefs.GetLanguageID() == language;
	}
	thePrefs.SetLanguageID(0xffff);
	thePrefs.SetLanguage();
	valid = valid && thePrefs.GetLanguageID() == LANGID_EN_US;
	thePrefs.SetLanguageID(originalLanguage);
	thePrefs.SetLanguage();
	valid = (::SetThreadLocale(originalLocale) != FALSE) && valid;
	return valid;
}

bool SelfTestSavedLanguage()
{
	// Paired with tests/fixtures/saved-language.ini in an isolated portable copy.
	// No sockets, UI, router discovery or preference saving is performed.
	return !thePrefs.IsFirstStart() && thePrefs.GetLanguageID() == LANGID_FR_FR
		&& thePrefs.GetUserNick() == _T("CI saved preferences")
		&& thePrefs.GetPort() == 24662 && thePrefs.GetUDPPort() == 24672
		&& !thePrefs.IsUPnPEnabled() && !thePrefs.IsUPnPHomeOnly();
}

static bool CheckConnectionSetupPolicy()
{
	using namespace ConnectionSetupPolicy;
	return ValidPorts(1, 1, false) && ValidPorts(65535, 65535, false)
		&& ValidPorts(4662, 0, true) && !ValidPorts(0, 4672, false)
		&& !ValidPorts(65536, 4672, false) && !ValidPorts(4662, 0, false)
		&& !ValidPorts(4662, 65536, false)
		&& MappingIsActive("192.168.1.2", "4662", "1", "192.168.1.2", 4662)
		&& !MappingIsActive("192.168.1.3", "4662", "1", "192.168.1.2", 4662)
		&& !MappingIsActive("192.168.1.2", "4663", "1", "192.168.1.2", 4662)
		&& !MappingIsActive("192.168.1.2", "4662", "0", "192.168.1.2", 4662)
		&& !MappingIsActive("192.168.1.2", "4662", "", "192.168.1.2", 4662)
		&& !MatchesEndpoint("192.168.1.2", "4662junk", "192.168.1.2", 4662)
		&& !MatchesEndpoint("192.168.1.2", "4294971958", "192.168.1.2", 4662)
		&& !MatchesEndpoint("192.168.1.2", "-1", "192.168.1.2", 65535)
		&& !MatchesEndpoint("", "4662", "", 4662)
		&& !MatchesEndpoint(nullptr, "4662", "192.168.1.2", 4662)
		&& AllowAutomaticSetup(true, false, true, 2)
		&& !AllowAutomaticSetup(false, false, true, 2)
		&& !AllowAutomaticSetup(true, true, true, 2)
		&& !AllowAutomaticSetup(true, false, false, 2)
		&& !AllowAutomaticSetup(true, false, true, 0)
		&& !AllowAutomaticSetup(true, false, true, 1)
		&& !AllowAutomaticSetup(true, false, true, 4)
		&& !AllowAutomaticSetup(true, false, true, 6)
		&& ShouldConnectAfterFirstRunFinish(true, true, true)
		&& !ShouldConnectAfterFirstRunFinish(false, true, true)
		&& !ShouldConnectAfterFirstRunFinish(true, false, true)
		&& !ShouldConnectAfterFirstRunFinish(true, true, false);
}

static bool CheckConnectionSetupDefaults()
{
	// This test runs only in a fresh, isolated portable directory. Merely
	// starting the client must not authorize router changes.
	return !thePrefs.IsUPnPEnabled() && !thePrefs.IsUPnPHomeOnly()
		&& !thePrefs.IsSafeServerConnectEnabled();
}

static bool CheckServerBootstrap()
{
	if (wcsncmp(ServerBootstrap::OnlineServerMetUrl, L"https://", 8) != 0
		|| wcsstr(ServerBootstrap::OnlineServerMetUrl, L"server.met") == nullptr)
		return false;
	for (unsigned mask = 0; mask < 8; ++mask) {
		const bool completed = (mask & 1) != 0, selected = (mask & 2) != 0, ed2k = (mask & 4) != 0;
		const bool expected = completed && selected && ed2k;
		ServerBootstrap::Selection beforeLoad;
		if (beforeLoad.Finish(completed, selected, ed2k) || beforeLoad.Loaded() != expected || beforeLoad.Loaded())
			return false;
		ServerBootstrap::Selection afterLoad;
		if (afterLoad.Loaded() || afterLoad.Finish(completed, selected, ed2k) != expected)
			return false;
	}
	bool present[_countof(ServerBootstrap::Entries)] = {true};
	unsigned mutations = 0;
	const auto exists = [&](const ServerBootstrap::Entry& entry) { return present[&entry - ServerBootstrap::Entries]; };
	const auto add = [&](const ServerBootstrap::Entry& entry) {
		++mutations;
		present[&entry - ServerBootstrap::Entries] = true;
		return true;
	};
	// A pre-existing entry is never handed to AddServer (which would reset
	// its failed count). Repeating the operation adds nothing.
	if (ServerBootstrap::MergeMissing(exists, add) != 7 || mutations != 7
		|| ServerBootstrap::MergeMissing(exists, add) != 0 || mutations != 7)
		return false;
	return ServerBootstrap::MergeMissing([](const ServerBootstrap::Entry&) { return false; },
		[](const ServerBootstrap::Entry&) { return false; }) == 0;
}

static bool CheckKadBootstrap()
{
	return wcsncmp(KadBootstrap::OnlineNodesDatUrl, L"https://", 8) == 0
		&& wcsstr(KadBootstrap::OnlineNodesDatUrl, L"nodes.dat") != nullptr;
}

static bool CheckConnectionSpeedPolicy()
{
	return ConnectionSpeedTest::RecommendedUploadLimit(0) == 0
		&& ConnectionSpeedTest::RecommendedUploadLimit(1) == 1
		&& ConnectionSpeedTest::RecommendedUploadLimit(10) == 8
		&& ConnectionSpeedTest::RecommendedUploadLimit(2500) == 2000;
}

static INT_PTR CALLBACK ConnectionLayoutTestProc(HWND, UINT, WPARAM, LPARAM) noexcept
{
	return FALSE;
}

static bool CheckConnectionSetupLayout()
{
	// Instantiate only the dialog resource, never the live wizard or sockets.
	// Check real font metrics for the embedded English resources and every
	// language module shipped beside the executable.
	TCHAR executable[MAX_PATH];
	const DWORD length = GetModuleFileName(NULL, executable, _countof(executable));
	if (length == 0 || length >= _countof(executable))
		return false;
	CString executableDirectory(executable);
	executableDirectory = executableDirectory.Left(executableDirectory.ReverseFind(_T('\\')) + 1);
	HWND host = CreateWindowEx(0, _T("STATIC"), _T(""), WS_POPUP,
		0, 0, 800, 600, NULL, NULL, AfxGetInstanceHandle(), NULL);
	const UINT templates[] = {IDD_WIZ1_PORTS, IDD_WIZ1_SPEEDTEST, IDD_WIZ1_SERVER};
	HWND pages[_countof(templates)] = {};
	bool valid = host != NULL;
	for (unsigned i = 0; valid && i < _countof(templates); ++i) {
		pages[i] = CreateDialogParam(AfxGetInstanceHandle(), MAKEINTRESOURCE(templates[i]), host, ConnectionLayoutTestProc, 0);
		valid = pages[i] != NULL;
	}
	// The wizard must not duplicate Windows' own firewall permission flow.
	valid = valid && ::GetDlgItem(pages[0], IDC_SETUP_FIREWALL) == NULL;
	const struct { unsigned page; int control; UINT text; bool multiline; int padding; } checks[] = {
		{0, IDC_PORTINFO, IDS_CONNSETUP_INFO, true, 0},
		{0, IDC_WIZ_AUTO_PORTS, IDS_CONNSETUP_BUTTON, true, 18},
		{0, IDC_WIZ_SETUP_INFO, IDS_WIZSETUP_PROTOCOLS, true, 0},
		{1, IDC_WIZ_SPEED_INFO, IDS_WIZSETUP_SPEEDTEST_INFO, true, 0},
		{2, IDC_SAFESERVERCONNECT, IDS_FIRSTSAFECON, true, 18},
		{2, IDC_WIZ_SERVERS_ADD, IDS_WIZSETUP_SERVERS_ADD, true, 18},
		{2, IDC_WIZ_NODES_ADD, IDS_WIZSETUP_NODES_ADD, true, 18}
	};
	CStringW failure;
	auto validateModule = [&](HMODULE module, LPCWSTR moduleName) {
		bool moduleValid = true;
		for (UINT id = IDS_CONNSETUP_TITLE; moduleValid && id <= IDS_WIZSETUP_SPEEDTEST_INVALID; ++id) {
			CString text;
			moduleValid = text.LoadString(module, id) != FALSE && !text.IsEmpty();
			if (!moduleValid && failure.IsEmpty())
				failure.Format(L"missing-string module=%s id=%u", moduleName, id);
		}
		for (const auto& check : checks) {
			if (!moduleValid)
				break;
			HWND control = ::GetDlgItem(pages[check.page], check.control);
			CString text;
			moduleValid = control != NULL && text.LoadString(module, check.text) != FALSE;
			if (!moduleValid) {
				if (failure.IsEmpty())
					failure.Format(L"missing-control-or-string module=%s id=%u", moduleName, check.text);
				break;
			}
			RECT available = {};
			::GetClientRect(control, &available);
			RECT measured = available;
			measured.right -= check.padding;
			HDC dc = ::GetDC(control);
			if (dc == NULL) {
				moduleValid = false;
				if (failure.IsEmpty())
					failure.Format(L"missing-device-context module=%s id=%u", moduleName, check.text);
				break;
			}
			HGDIOBJ oldFont = ::SelectObject(dc, reinterpret_cast<HFONT>(::SendMessage(control, WM_GETFONT, 0, 0)));
			::DrawText(dc, text, text.GetLength(), &measured,
				DT_CALCRECT | DT_NOPREFIX | (check.multiline ? DT_WORDBREAK : DT_SINGLELINE));
			moduleValid = measured.bottom <= available.bottom
				&& measured.right <= available.right - check.padding;
			if (!moduleValid && failure.IsEmpty())
				failure.Format(L"text-overflow module=%s id=%u required=%ldx%ld available=%ldx%ld",
					moduleName, check.text, measured.right, measured.bottom, available.right, available.bottom);
			::SelectObject(dc, oldFont);
			::ReleaseDC(control, dc);
		}
		return moduleValid;
	};

	if (!valid)
		failure = L"connection-dialog-create-failed";
	valid = valid && validateModule(AfxGetInstanceHandle(), L"embedded-English");
	unsigned languageModuleCount = 0;
	WIN32_FIND_DATA languageFile = {};
	HANDLE languageSearch = FindFirstFile(executableDirectory + _T("lang\\*.dll"), &languageFile);
	if (languageSearch == INVALID_HANDLE_VALUE) {
		valid = false;
		if (failure.IsEmpty())
			failure = L"language-directory-not-found";
	} else {
		do {
			if ((languageFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				continue;
			if (_tcsicmp(languageFile.cFileName, _T("eMuleNext-GraphicsTest.dll")) == 0)
				continue;
			HMODULE languageModule = LoadLibraryEx(executableDirectory + _T("lang\\") + languageFile.cFileName,
				NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			if (languageModule == NULL) {
				valid = false;
				if (failure.IsEmpty())
					failure.Format(L"language-module-load-failed module=%s", languageFile.cFileName);
				continue;
			}
			++languageModuleCount;
			valid = validateModule(languageModule, languageFile.cFileName) && valid;
			FreeLibrary(languageModule);
		} while (FindNextFile(languageSearch, &languageFile));
		FindClose(languageSearch);
	}
	// English is embedded in the executable; all 43 shipped translations are DLLs.
	valid = valid && languageModuleCount == 43;
	if (languageModuleCount != 43 && failure.IsEmpty())
		failure.Format(L"language-module-count expected=43 actual=%u", languageModuleCount);
	if (host != NULL)
		DestroyWindow(host);
	if (!valid && !failure.IsEmpty()) {
		CFile diagnostic;
		if (diagnostic.Open(_T("ci-connection-layout.log"), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
			const WCHAR bom = 0xFEFF;
			diagnostic.Write(&bom, sizeof(bom));
			diagnostic.Write(failure.GetString(), failure.GetLength() * sizeof(WCHAR));
		}
	}
	return valid;
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

	extern bool SelfTestPortMapping();
	return SelfTestPortMapping() && CheckHashing() && CheckUploadPolicy() && CheckLanguageModules() && CheckLanguageSelection()
		&& CheckConnectionSetupPolicy() && CheckConnectionSetupDefaults() && CheckServerBootstrap()
		&& CheckKadBootstrap()
		&& CheckConnectionSpeedPolicy() && CheckConnectionSetupLayout();
}
