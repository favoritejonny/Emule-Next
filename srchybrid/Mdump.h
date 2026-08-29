// eMule Next modification notice: modified on 2026-08-19; see ../CHANGES.md.
#pragma once

struct _EXCEPTION_POINTERS;

class CMiniDumper
{
public:
	static void Enable(LPCTSTR pszAppName, bool bShowErrors, LPCTSTR pszDumpDir);
	// Offers a previously created local dump to the user on the next start.
	// The report is never uploaded automatically.
	static void ShowPendingReport();
	unsigned uCreateCrashDump; //0 - no dump; 1 - create dump if user agrees; 2 - create without asking
private:
	static TCHAR m_szAppName[MAX_PATH];
	static TCHAR m_szDumpDir[MAX_PATH];

	static HMODULE GetDebugHelperDll(FARPROC *ppfnMiniDumpWriteDump, bool bShowErrors);
	static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo) noexcept;
};

extern CMiniDumper theCrashDumper;
