//this file is part of eMule
//Copyright (C)2002-2026 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
// eMule Next modification notice: modified on 2026-08-19; see ../CHANGES.md.
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
#include "PreferencesDlg.h"
#include "Wizard.h"
#include "emuledlg.h"
#include "StatisticsDlg.h"
#include "opcodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
struct SConnectionProfile
{
	LPCTSTR name;
	UINT downloadKBps;
	UINT uploadKBps;
};

// Provider-neutral connection profiles expressed in KB/s, matching the rest
// of the connection preferences. The custom entry remains available for any
// plan which is not represented here.
const SConnectionProfile s_connectionProfiles[] =
{
	{ _T("DSL 1 / 0.5 Mbps"),          122u,      61u },
	{ _T("DSL 20 / 1 Mbps"),          2441u,     122u },
	{ _T("VDSL / FTTC 100 / 20 Mbps"),12207u,   2441u },
	{ _T("VDSL / FTTC 200 / 20 Mbps"),24414u,   2441u },
	{ _T("FWA 100 / 20 Mbps"),       12207u,   2441u },
	{ _T("Cable 100 / 20 Mbps"),     12207u,   2441u },
	{ _T("Cable 300 / 30 Mbps"),     36621u,   3662u },
	{ _T("Cable 1 Gbps / 50 Mbps"), 122070u,   6103u },
	{ _T("FTTH 100 / 100 Mbps"),     12207u,  12207u },
	{ _T("FTTH 300 / 300 Mbps"),     36621u,  36621u },
	{ _T("FTTH 1 Gbps / 200 Mbps"), 122070u,  24414u },
	{ _T("FTTH 1 Gbps / 1 Gbps"),   122070u, 122070u },
	{ _T("FTTH 2.5 Gbps / 1 Gbps"), 305175u, 122070u },
	{ _T("FTTH 5 Gbps / 2.5 Gbps"), 610351u, 305175u },
	{ _T("FTTH 10 Gbps / 10 Gbps"),1220703u,1220703u }
};

static CString GetWizardRateHeader(UINT uResourceID)
{
	CString header(GetResString(uResourceID));
	const int unitStart = header.Find(_T('('));
	if (unitStart >= 0) {
		header = header.Left(unitStart);
		header.TrimRight();
	}
	header.AppendFormat(_T(" (%s)"), (LPCTSTR)GetResString(IDS_KBYTESPERSEC));
	return header;
}
}

// CConnectionWizardDlg dialog

IMPLEMENT_DYNAMIC(CConnectionWizardDlg, CDialog)

BEGIN_MESSAGE_MAP(CConnectionWizardDlg, CDialog)
	ON_BN_CLICKED(IDC_WIZ_APPLY_BUTTON, OnBnClickedApply)
	ON_BN_CLICKED(IDC_WIZ_CANCEL_BUTTON, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_WIZ_LOWDOWN_RADIO, OnBnClickedWizLowdownloadRadio)
	ON_BN_CLICKED(IDC_WIZ_MEDIUMDOWN_RADIO, OnBnClickedWizMediumdownloadRadio)
	ON_BN_CLICKED(IDC_WIZ_HIGHDOWN_RADIO, OnBnClickedWizHighdownloadRadio)
	ON_NOTIFY(NM_CLICK, IDC_PROVIDERS, OnNmClickProviders)
END_MESSAGE_MAP()

CConnectionWizardDlg::CConnectionWizardDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CConnectionWizardDlg::IDD, pParent)
	, m_icoWnd()
	, m_iTotalDownload()
{
}

CConnectionWizardDlg::~CConnectionWizardDlg()
{
	if (m_icoWnd)
		VERIFY(::DestroyIcon(m_icoWnd));
}

void CConnectionWizardDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Radio(pDX, IDC_WIZ_XP_RADIO, m_iOS);
	DDX_Control(pDX, IDC_PROVIDERS, m_provider);
	DDX_Radio(pDX, IDC_WIZ_LOWDOWN_RADIO, m_iTotalDownload);
}

void CConnectionWizardDlg::OnBnClickedApply()
{
	if (m_provider.GetSelectionMark() == 0) {
		// change the upload/download to unlimited and don't touch other stuff, keep the default values
		thePrefs.maxGraphUploadRate = UNLIMITED;
		thePrefs.maxGraphDownloadRate = 96;
		thePrefs.m_maxupload = UNLIMITED;
		thePrefs.m_maxdownload = UNLIMITED;
		theApp.emuledlg->statisticswnd->SetARange(false, thePrefs.GetMaxGraphUploadRate(true));
		theApp.emuledlg->statisticswnd->SetARange(true, thePrefs.maxGraphDownloadRate);
		theApp.emuledlg->preferenceswnd->m_wndConnection.LoadSettings();
		CDialog::OnOK();
		return;
	}

	UINT download = GetDlgItemInt(IDC_WIZ_TRUEDOWNLOAD_BOX, NULL, TRUE);
	if (download <= 0) {
		GetDlgItem(IDC_WIZ_TRUEDOWNLOAD_BOX)->SetFocus();
		return;
	}

	UINT upload = GetDlgItemInt(IDC_WIZ_TRUEUPLOAD_BOX, NULL, TRUE);
	if (upload <= 0) {
		GetDlgItem(IDC_WIZ_TRUEUPLOAD_BOX)->SetFocus();
		return;
	}

	// The table and both "real bandwidth" fields are expressed in KB/s,
	// exactly like the rest of the connection preferences.

	thePrefs.maxGraphDownloadRate = download;
	thePrefs.maxGraphUploadRate = upload;

	if (upload > 0 && download > 0) {
		thePrefs.m_maxupload = upload * 4 / 5;
		if (upload < 4 && download > upload * 3) {
			thePrefs.m_maxdownload = thePrefs.m_maxupload * 3;
			download = upload * 3;
		} else if (upload < 10 && download > upload * 4) {
			thePrefs.m_maxdownload = thePrefs.m_maxupload * 4;
			download = upload * 4;
		} else if (upload < 20 && download > upload * 5) {
			thePrefs.m_maxdownload = thePrefs.m_maxupload * 5;
			download = upload * 5;
		} else
			thePrefs.m_maxdownload = download * 9 / 10;

		theApp.emuledlg->statisticswnd->SetARange(false, thePrefs.maxGraphUploadRate);
		theApp.emuledlg->statisticswnd->SetARange(true, thePrefs.maxGraphDownloadRate);

		if (upload <= 7)
			thePrefs.maxconnections = 80;
		else if (upload < 12)
			thePrefs.maxconnections = 200;
		else if (upload < 25)
			thePrefs.maxconnections = 400;
		else if (upload < 37)
			thePrefs.maxconnections = 600;
		else
			thePrefs.maxconnections = 800;

		static const UINT srcperfile[5][3] =
		{
			{ 100u,  60u,  40u},
			{ 300u, 200u, 100u},
			{ 500u, 400u, 350u},
			{ 800u, 600u, 400u},
			{1000u, 750u, 500u}
		};

		int i;
		if (download <= 7)
			i = 0;
		else if (download < 62)
			i = 1;
		else if (download < 187)
			i = 2;
		else if (download <= 312)
			i = 3;
		else
			i = 4;

		thePrefs.maxsourceperfile = srcperfile[i][m_iTotalDownload];
	}
	theApp.emuledlg->preferenceswnd->m_wndConnection.LoadSettings();
	CDialog::OnOK();
}

void CConnectionWizardDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CConnectionWizardDlg::OnBnClickedWizLowdownloadRadio()
{
	m_iTotalDownload = 0;
}

void CConnectionWizardDlg::OnBnClickedWizMediumdownloadRadio()
{
	m_iTotalDownload = 1;
}

void CConnectionWizardDlg::OnBnClickedWizHighdownloadRadio()
{
	m_iTotalDownload = 2;
}

void CConnectionWizardDlg::OnBnClickedWizResetButton()
{
	SetDlgItemInt(IDC_WIZ_TRUEDOWNLOAD_BOX, 0, FALSE);
	SetDlgItemInt(IDC_WIZ_TRUEUPLOAD_BOX, 0, FALSE);
}

BOOL CConnectionWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitWindowStyles(this);

	SetIcon(m_icoWnd = theApp.LoadIcon(_T("Wizard")), FALSE);

	CheckRadioButton(IDC_WIZ_LOWDOWN_RADIO, IDC_WIZ_HIGHDOWN_RADIO, IDC_WIZ_LOWDOWN_RADIO);
	SetDlgItemInt(IDC_WIZ_TRUEDOWNLOAD_BOX, 0, FALSE);
	SetDlgItemInt(IDC_WIZ_TRUEUPLOAD_BOX, 0, FALSE);

	m_provider.InsertColumn(0, GetResString(IDS_TYPE), LVCFMT_LEFT, 150);
	m_provider.InsertColumn(1, GetWizardRateHeader(IDS_WIZ_DOWN), LVCFMT_LEFT, 85);
	m_provider.InsertColumn(2, GetWizardRateHeader(IDS_WIZ_UP), LVCFMT_LEFT, 85);
	m_provider.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	m_provider.InsertItem(0, GetResString(IDS_UNKNOWN));
	m_provider.SetItemText(0, 1, _T(""));
	m_provider.SetItemText(0, 2, _T(""));
	m_provider.InsertItem(1, GetResString(IDS_WIZARD_CUSTOM));
	m_provider.SetItemText(1, 1, GetResString(IDS_WIZARD_ENTERBELOW));
	m_provider.SetItemText(1, 2, GetResString(IDS_WIZARD_ENTERBELOW));
	for (UINT i = 0; i < _countof(s_connectionProfiles); ++i) {
		const int item = m_provider.InsertItem(2 + static_cast<int>(i), s_connectionProfiles[i].name);
		CString rate;
		rate.Format(_T("%u"), s_connectionProfiles[i].downloadKBps);
		m_provider.SetItemText(item, 1, rate);
		rate.Format(_T("%u"), s_connectionProfiles[i].uploadKBps);
		m_provider.SetItemText(item, 2, rate);
	}

	m_provider.SetSelectionMark(0);
	m_provider.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	SetCustomItemsActivation();

	Localize();

	return TRUE;
}

void CConnectionWizardDlg::OnNmClickProviders(LPNMHDR, LRESULT *pResult)
{
	SetCustomItemsActivation();
	int i = m_provider.GetSelectionMark();
	if (i < 0 || i >= 2 + static_cast<int>(_countof(s_connectionProfiles)))
		return;
	UINT up = 0;
	UINT down = 0;
	if (i == 1) { // this is 'custom'; preferences already use KB/s
		down = thePrefs.maxGraphDownloadRate;
		up = thePrefs.GetMaxGraphUploadRate(true);
	} else if (i >= 2) {
		const SConnectionProfile &profile = s_connectionProfiles[i - 2];
		down = profile.downloadKBps;
		up = profile.uploadKBps;
	}

	SetDlgItemInt(IDC_WIZ_TRUEDOWNLOAD_BOX, down, FALSE);
	SetDlgItemInt(IDC_WIZ_TRUEUPLOAD_BOX, up, FALSE);
	*pResult = 0;
}

void CConnectionWizardDlg::Localize()
{
	SetWindowText(GetResString(IDS_WIZARD));
	SetDlgItemText(IDC_WIZ_OS_FRAME, GetResString(IDS_WIZ_OS_FRAME));
	SetDlgItemText(IDC_WIZ_CONCURENTDOWN_FRAME, GetResString(IDS_CONCURDWL));
	SetDlgItemText(IDC_WIZ_HOTBUTTON_FRAME, GetResString(IDS_WIZ_CTFRAME));
	SetDlgItemText(IDC_WIZ_TRUEUPLOAD_TEXT, GetResString(IDS_WIZ_TRUEUPLOAD_TEXT));
	SetDlgItemText(IDC_WIZ_TRUEDOWNLOAD_TEXT, GetResString(IDS_WIZ_TRUEDOWNLOAD_TEXT));
	SetDlgItemText(IDC_KBITS, GetResString(IDS_KBYTESPERSEC));
	SetDlgItemText(IDC_KBYTES, GetResString(IDS_KBYTESPERSEC));
	SetDlgItemText(IDC_WIZ_APPLY_BUTTON, GetResString(IDS_PW_APPLY));
	SetDlgItemText(IDC_WIZ_CANCEL_BUTTON, GetResString(IDS_CANCEL));
}

void CConnectionWizardDlg::SetCustomItemsActivation()
{
	BOOL bActive = (m_provider.GetSelectionMark() == 1);
	GetDlgItem(IDC_WIZ_TRUEUPLOAD_BOX)->EnableWindow(bActive);
	GetDlgItem(IDC_WIZ_TRUEDOWNLOAD_BOX)->EnableWindow(bActive);
	GetDlgItem(IDC_KBITS)->EnableWindow(bActive);
	GetDlgItem(IDC_KBYTES)->EnableWindow(bActive);
}
