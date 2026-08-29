//this file is part of eMule
//Copyright (C)2002-2026 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "emule.h"
#include "emuleDlg.h"
#include "ServerWnd.h"
#include "PPgServer.h"
#include "OtherFunctions.h"
#include "Preferences.h"
#include "HelpIDs.h"
#include "InputBox.h"
#include "Opcodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


class CServerListSourcesDlg : public CDialog
{
public:
	explicit CServerListSourcesDlg(CWnd *pParent)
		: CDialog(IDD_SERVERLIST_SOURCES, pParent)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUpdateNow();
	afx_msg void OnSelectionChanged();

	bool SaveSources();
	void UpdateButtons();
	void UpdateHorizontalExtent();

	CListBox m_wndSources;

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CServerListSourcesDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_UPDATESERVERMETFROMURL, OnUpdateNow)
	ON_LBN_SELCHANGE(IDC_SERVER_SOURCE_LIST, OnSelectionChanged)
END_MESSAGE_MAP()

void CServerListSourcesDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_SOURCE_LIST, m_wndSources);
}

BOOL CServerListSourcesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitWindowStyles(this);

	SetWindowText(GetResString(IDS_SV_SERVERLIST));
	SetDlgItemText(IDC_LBL_UPDATE_SERVERS, GetResString(IDS_SV_MET));
	SetDlgItemText(IDC_ADD, GetResString(IDS_ADD));
	SetDlgItemText(IDC_REMOVE, GetResString(IDS_REMOVE));
	SetDlgItemText(IDC_UPDATESERVERMETFROMURL, GetResString(IDS_SV_UPDATE));
	SetDlgItemText(IDOK, GetResString(IDS_SAVE));
	SetDlgItemText(IDCANCEL, GetResString(IDS_CANCEL));

	for (POSITION pos = thePrefs.addresses_list.GetHeadPosition(); pos != NULL;)
		m_wndSources.AddString(thePrefs.addresses_list.GetNext(pos));

	if (m_wndSources.GetCount() > 0)
		m_wndSources.SetCurSel(0);

	UpdateHorizontalExtent();
	UpdateButtons();
	return TRUE;
}

void CServerListSourcesDlg::OnAdd()
{
	InputBox input(this);
	input.SetLabels(GetResString(IDS_SV_SERVERLIST), GetResString(IDS_SV_MET), CString());
	if (input.DoModal() != IDOK)
		return;

	CString strURL(input.GetInput());
	strURL.Trim();
	if (strURL.IsEmpty() || strURL.Find(_T("://")) < 1) {
		AfxMessageBox(GetResString(IDS_INVALIDURL), MB_OK | MB_ICONWARNING);
		return;
	}

	for (int i = 0; i < m_wndSources.GetCount(); ++i) {
		CString strExisting;
		m_wndSources.GetText(i, strExisting);
		if (strExisting.CompareNoCase(strURL) == 0) {
			m_wndSources.SetCurSel(i);
			UpdateButtons();
			return;
		}
	}

	const int iAdded = m_wndSources.AddString(strURL);
	if (iAdded != LB_ERR && iAdded != LB_ERRSPACE)
		m_wndSources.SetCurSel(iAdded);

	UpdateHorizontalExtent();
	UpdateButtons();
}

void CServerListSourcesDlg::OnRemove()
{
	const int iSelected = m_wndSources.GetCurSel();
	if (iSelected == LB_ERR)
		return;

	m_wndSources.DeleteString(iSelected);
	const int iCount = m_wndSources.GetCount();
	if (iCount > 0)
		m_wndSources.SetCurSel(min(iSelected, iCount - 1));

	UpdateHorizontalExtent();
	UpdateButtons();
}

void CServerListSourcesDlg::OnUpdateNow()
{
	if (!SaveSources())
		return;

	if (m_wndSources.GetCount() == 0) {
		AfxMessageBox(GetResString(IDS_SRV_NOURLAV), MB_OK | MB_ICONINFORMATION);
		return;
	}

	for (int i = 0; i < m_wndSources.GetCount(); ++i) {
		CString strURL;
		m_wndSources.GetText(i, strURL);
		if (theApp.emuledlg->serverwnd->UpdateServerMetFromURL(strURL)) {
			CDialog::OnOK();
			return;
		}
	}
}

void CServerListSourcesDlg::OnSelectionChanged()
{
	UpdateButtons();
}

void CServerListSourcesDlg::OnOK()
{
	if (SaveSources())
		CDialog::OnOK();
}

bool CServerListSourcesDlg::SaveSources()
{
	const CString strPath(thePrefs.GetMuleDirectory(EMULE_CONFIGDIR) + _T("addresses.dat"));
	const CString strTempPath(strPath + _T(".tmp"));

	CStdioFile file;
	CFileException fileException;
	if (!file.Open(strTempPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeBinary, &fileException)) {
		AfxMessageBox(GetResString(IDS_ERROR_SAVEFILE), MB_OK | MB_ICONERROR);
		return false;
	}

	bool bFileOpen = true;
	try {
		static const WORD wBOM = u'\xFEFF';
		file.Write(&wBOM, sizeof wBOM);
		for (int i = 0; i < m_wndSources.GetCount(); ++i) {
			CString strURL;
			m_wndSources.GetText(i, strURL);
			file.WriteString(strURL);
			file.Write(_T("\r\n"), 2 * sizeof(TCHAR));
		}
		CommitAndClose(file);
		bFileOpen = false;
	}
	catch (CFileException *pException) {
		if (bFileOpen)
			file.Abort();
		pException->Delete();
		_tremove(strTempPath);
		AfxMessageBox(GetResString(IDS_ERROR_SAVEFILE), MB_OK | MB_ICONERROR);
		return false;
	}

	if (!MoveFileEx(strTempPath, strPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
		_tremove(strTempPath);
		AfxMessageBox(GetResString(IDS_ERROR_SAVEFILE), MB_OK | MB_ICONERROR);
		return false;
	}

	thePrefs.addresses_list.RemoveAll();
	for (int i = 0; i < m_wndSources.GetCount(); ++i) {
		CString strURL;
		m_wndSources.GetText(i, strURL);
		thePrefs.addresses_list.AddTail(strURL);
	}
	return true;
}

void CServerListSourcesDlg::UpdateButtons()
{
	GetDlgItem(IDC_REMOVE)->EnableWindow(m_wndSources.GetCurSel() != LB_ERR);
	GetDlgItem(IDC_UPDATESERVERMETFROMURL)->EnableWindow(m_wndSources.GetCount() > 0);
}

void CServerListSourcesDlg::UpdateHorizontalExtent()
{
	int iExtent = 0;
	CDC *pDC = m_wndSources.GetDC();
	if (pDC != NULL) {
		for (int i = 0; i < m_wndSources.GetCount(); ++i) {
			CString strURL;
			m_wndSources.GetText(i, strURL);
			iExtent = max(iExtent, pDC->GetTextExtent(strURL).cx);
		}
		m_wndSources.ReleaseDC(pDC);
	}
	m_wndSources.SetHorizontalExtent(iExtent + GetSystemMetrics(SM_CXVSCROLL));
}

IMPLEMENT_DYNAMIC(CPPgServer, CPropertyPage)

BEGIN_MESSAGE_MAP(CPPgServer, CPropertyPage)
	ON_EN_CHANGE(IDC_SERVERRETRIES, OnSettingsChange)
	ON_BN_CLICKED(IDC_AUTOSERVER, OnSettingsChange)
	ON_BN_CLICKED(IDC_UPDATESERVERCONNECT, OnSettingsChange)
	ON_BN_CLICKED(IDC_UPDATESERVERCLIENT, OnSettingsChange)
	ON_BN_CLICKED(IDC_SCORE, OnSettingsChange)
	ON_BN_CLICKED(IDC_SMARTIDCHECK, OnSettingsChange)
	ON_BN_CLICKED(IDC_SAFESERVERCONNECT, OnSettingsChange)
	ON_BN_CLICKED(IDC_AUTOCONNECTSTATICONLY, OnSettingsChange)
	ON_BN_CLICKED(IDC_MANUALSERVERHIGHPRIO, OnSettingsChange)
	ON_BN_CLICKED(IDC_EDITADR, OnBnClickedEditadr)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

CPPgServer::CPPgServer()
	: CPropertyPage(CPPgServer::IDD)
{
}

void CPPgServer::DoDataExchange(CDataExchange *pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BOOL CPPgServer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE;  // return TRUE unless you set the focus to the control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPPgServer::LoadSettings()
{
	SetDlgItemInt(IDC_SERVERRETRIES, thePrefs.m_uDeadServerRetries, FALSE);
	CheckDlgButton(IDC_AUTOSERVER, thePrefs.m_bAutoUpdateServerList);
	CheckDlgButton(IDC_UPDATESERVERCONNECT, thePrefs.m_bAddServersFromServer);
	CheckDlgButton(IDC_UPDATESERVERCLIENT, thePrefs.m_bAddServersFromClients);
	CheckDlgButton(IDC_SCORE, thePrefs.m_bUseServerPriorities);
	CheckDlgButton(IDC_SMARTIDCHECK, thePrefs.m_bSmartServerIdCheck);
	CheckDlgButton(IDC_SAFESERVERCONNECT, thePrefs.m_bSafeServerConnect);
	CheckDlgButton(IDC_AUTOCONNECTSTATICONLY, thePrefs.m_bAutoConnectToStaticServersOnly);
	CheckDlgButton(IDC_MANUALSERVERHIGHPRIO, thePrefs.m_bManualAddedServersHighPriority);
}

BOOL CPPgServer::OnApply()
{
	UINT uCurDeadServerRetries = thePrefs.m_uDeadServerRetries;
	thePrefs.m_uDeadServerRetries = GetDlgItemInt(IDC_SERVERRETRIES, NULL, FALSE);
	if (thePrefs.m_uDeadServerRetries < 1)
		thePrefs.m_uDeadServerRetries = 1;
	else if (thePrefs.m_uDeadServerRetries > MAX_SERVERFAILCOUNT)
		thePrefs.m_uDeadServerRetries = MAX_SERVERFAILCOUNT;
	if (uCurDeadServerRetries != thePrefs.m_uDeadServerRetries) {
		theApp.emuledlg->serverwnd->serverlistctrl.Invalidate();
		theApp.emuledlg->serverwnd->serverlistctrl.UpdateWindow();
	}
	thePrefs.m_bAutoUpdateServerList = IsDlgButtonChecked(IDC_AUTOSERVER) != 0;
	thePrefs.m_bAddServersFromServer = IsDlgButtonChecked(IDC_UPDATESERVERCONNECT) != 0;
	thePrefs.m_bAddServersFromClients = IsDlgButtonChecked(IDC_UPDATESERVERCLIENT) != 0;
	thePrefs.m_bUseServerPriorities = IsDlgButtonChecked(IDC_SCORE) != 0;
	thePrefs.m_bSmartServerIdCheck = IsDlgButtonChecked(IDC_SMARTIDCHECK) != 0;
	thePrefs.SetSafeServerConnectEnabled(IsDlgButtonChecked(IDC_SAFESERVERCONNECT) != 0);
	thePrefs.m_bAutoConnectToStaticServersOnly = IsDlgButtonChecked(IDC_AUTOCONNECTSTATICONLY) != 0;
	thePrefs.m_bManualAddedServersHighPriority = IsDlgButtonChecked(IDC_MANUALSERVERHIGHPRIO) != 0;

	LoadSettings();

	SetModified();
	return CPropertyPage::OnApply();
}

void CPPgServer::Localize()
{
	if (m_hWnd) {
		SetWindowText(GetResString(IDS_PW_SERVER));
		SetDlgItemText(IDC_LBL_UPDATE_SERVERS, GetResString(IDS_SV_UPDATE));
		SetDlgItemText(IDC_LBL_MISC, GetResString(IDS_PW_MISC));
		SetDlgItemText(IDC_REMOVEDEAD, GetResString(IDS_PW_RDEAD));
		SetDlgItemText(IDC_RETRIES_LBL, GetResString(IDS_PW_RETRIES));
		SetDlgItemText(IDC_UPDATESERVERCONNECT, GetResString(IDS_PW_USC));
		SetDlgItemText(IDC_UPDATESERVERCLIENT, GetResString(IDS_PW_UCC));
		SetDlgItemText(IDC_AUTOSERVER, GetResString(IDS_PW_USS));
		SetDlgItemText(IDC_SMARTIDCHECK, GetResString(IDS_SMARTLOWIDCHECK));
		SetDlgItemText(IDC_SAFESERVERCONNECT, GetResString(IDS_PW_FASTSRVCON));
		SetDlgItemText(IDC_SCORE, GetResString(IDS_PW_SCORE));
		SetDlgItemText(IDC_MANUALSERVERHIGHPRIO, GetResString(IDS_MANUALSERVERHIGHPRIO));
		SetDlgItemText(IDC_EDITADR, GetResString(IDS_EDITLIST));
		SetDlgItemText(IDC_AUTOCONNECTSTATICONLY, GetResString(IDS_PW_AUTOCONNECTSTATICONLY));
	}
}

void CPPgServer::OnBnClickedEditadr()
{
	CServerListSourcesDlg dlg(this);
	dlg.DoModal();
}

void CPPgServer::OnHelp()
{
	theApp.ShowHelp(eMule_FAQ_Preferences_Server);
}

BOOL CPPgServer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return (wParam == ID_HELP) ? OnHelpInfo(NULL) : __super::OnCommand(wParam, lParam);
}

BOOL CPPgServer::OnHelpInfo(HELPINFO*)
{
	OnHelp();
	return TRUE;
}
