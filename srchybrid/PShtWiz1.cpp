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
#include "langids.h"
#include "emule.h"
#include "enbitmap.h"
#include "OtherFunctions.h"
#include "Preferences.h"
#include "emuledlg.h"
#include "ListenSocket.h"
#include "ClientUDPSocket.h"
#include "ConnectionSetupPolicy.h"
#include "ConnectionSpeedTest.h"
#include "KadBootstrap.h"
#include "KademliaWnd.h"
#include "MenuCmds.h"
#include "ServerList.h"
#include "StatisticsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CDlgPageWizard dialog

class CDlgPageWizard : public CPropertyPageEx
{
	DECLARE_DYNCREATE(CDlgPageWizard)

public:
	CDlgPageWizard();

	explicit CDlgPageWizard(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CPropertyPageEx(nIDTemplate)
	{
		if (pszCaption) {
			m_strCaption = pszCaption; // "convenience storage"
			m_psp.pszTitle = m_strCaption;
			m_psp.dwFlags |= PSP_USETITLE;
		}
		if (pszHeaderTitle && pszHeaderTitle[0] != _T('\0')) {
			m_strHeaderTitle = pszHeaderTitle;
			m_psp.dwSize = (DWORD)sizeof m_psp;
		}
		if (pszHeaderSubTitle && pszHeaderSubTitle[0] != _T('\0')) {
			m_strHeaderSubTitle = pszHeaderSubTitle;
			m_psp.dwSize = (DWORD)sizeof m_psp;
		}
	}

protected:

	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE(CDlgPageWizard, CPropertyPageEx)

BEGIN_MESSAGE_MAP(CDlgPageWizard, CPropertyPageEx)
END_MESSAGE_MAP()

CDlgPageWizard::CDlgPageWizard()
	: CPropertyPageEx()
{
}

static void CreateWizardTitleFont(CFont& font)
{
	CFont fontSegoeUi;
	CreatePointFont(fontSegoeUi, 13 * 10, _T("Segoe UI"));

	LOGFONT lf;
	fontSegoeUi.GetLogFont(&lf);
	lf.lfWeight = FW_SEMIBOLD;
	lf.lfQuality = CLEARTYPE_QUALITY;
	font.CreateFontIndirect(&lf);
}

static CString GetWizardText(UINT resourceID)
{
	CString text(GetResString(resourceID));
	const CString oldName(_T("eMule"));
	const CString productName(EMULE_NEXT_PRODUCT_NAME);
	for (int position = 0; (position = text.Find(oldName, position)) >= 0; ) {
		if (text.Mid(position, productName.GetLength()).CompareNoCase(productName) != 0) {
			text.Delete(position, oldName.GetLength());
			text.Insert(position, productName);
		}
		position += productName.GetLength();
	}
	return text;
}

static CString RemoveTrailingChoiceNote(CString text)
{
	text.TrimRight();
	int opening = text.ReverseFind(_T('('));
	if (opening >= 0 && text.Right(1) == _T(")")) {
		text = text.Left(opening);
		text.TrimRight();
		return text;
	}
	opening = text.ReverseFind(static_cast<TCHAR>(0xFF08));
	if (opening >= 0 && !text.IsEmpty() && text[text.GetLength() - 1] == static_cast<TCHAR>(0xFF09)) {
		text = text.Left(opening);
		text.TrimRight();
	}
	return text;
}

struct WizardButtonText
{
	LANGID languageID;
	LPCTSTR back;
	LPCTSTR next;
	LPCTSTR finish;
};

static void LocalizeWizardButtons(CPropertySheetEx *pSheet)
{
	static const WizardButtonText labels[] = {
		{ LANGID_AR_AE, _T("\u0627\u0644\u0633\u0627\u0628\u0642"), _T("\u0627\u0644\u062a\u0627\u0644\u064a"), _T("\u0625\u0646\u0647\u0627\u0621") },
		{ LANGID_BA_BA, _T("Atzera"), _T("Hurrengoa"), _T("Amaitu") },
		{ LANGID_BG_BG, _T("\u041d\u0430\u0437\u0430\u0434"), _T("\u041d\u0430\u043f\u0440\u0435\u0434"), _T("\u0413\u043e\u0442\u043e\u0432\u043e") },
		{ LANGID_CA_ES, _T("Enrere"), _T("Seg\u00fcent"), _T("Finalitza") },
		{ LANGID_CZ_CZ, _T("Zp\u011bt"), _T("Dal\u0161\u00ed"), _T("Dokon\u010dit") },
		{ LANGID_DA_DK, _T("Tilbage"), _T("N\u00e6ste"), _T("Udf\u00f8r") },
		{ LANGID_DE_DE, _T("Zur\u00fcck"), _T("Weiter"), _T("Fertig stellen") },
		{ LANGID_EL_GR, _T("\u03a0\u03af\u03c3\u03c9"), _T("\u0395\u03c0\u03cc\u03bc\u03b5\u03bd\u03bf"), _T("\u03a4\u03ad\u03bb\u03bf\u03c2") },
		{ LANGID_ES_AS, _T("Atr\u00e1s"), _T("Siguiente"), _T("Finar") },
		{ LANGID_ES_ES_T, _T("Atr\u00e1s"), _T("Siguiente"), _T("Finalizar") },
		{ LANGID_ET_EE, _T("Tagasi"), _T("Edasi"), _T("L\u00f5peta") },
		{ LANGID_FA_IR, _T("\u0642\u0628\u0644\u06cc"), _T("\u0628\u0639\u062f\u06cc"), _T("\u067e\u0627\u06cc\u0627\u0646") },
		{ LANGID_FI_FI, _T("Edellinen"), _T("Seuraava"), _T("Valmis") },
		{ LANGID_FR_BR, _T("Kent"), _T("War-lerc'h"), _T("Echui\u00f1") },
		{ LANGID_FR_FR, _T("Pr\u00e9c\u00e9dent"), _T("Suivant"), _T("Terminer") },
		{ LANGID_GL_ES, _T("Atr\u00e1s"), _T("Seguinte"), _T("Rematar") },
		{ LANGID_HE_IL, _T("\u05d4\u05e7\u05d5\u05d3\u05dd"), _T("\u05d4\u05d1\u05d0"), _T("\u05e1\u05d9\u05d5\u05dd") },
		{ LANGID_HU_HU, _T("Vissza"), _T("Tov\u00e1bb"), _T("Befejez\u00e9s") },
		{ LANGID_IT_IT, _T("Indietro"), _T("Avanti"), _T("Fine") },
		{ LANGID_JP_JP, _T("\u623b\u308b"), _T("\u6b21\u3078"), _T("\u5b8c\u4e86") },
		{ LANGID_KO_KR, _T("\ub4a4\ub85c"), _T("\ub2e4\uc74c"), _T("\ub9c8\uce68") },
		{ LANGID_LT_LT, _T("Atgal"), _T("Toliau"), _T("Baigti") },
		{ LANGID_LV_LV, _T("Atpaka\u013c"), _T("T\u0101l\u0101k"), _T("Pabeigt") },
		{ LANGID_MT_MT, _T("Lura"), _T("Li jmiss"), _T("Spi\u010b\u010ba") },
		{ LANGID_NB_NO, _T("Tilbake"), _T("Neste"), _T("Fullf\u00f8r") },
		{ LANGID_NN_NO, _T("Tilbake"), _T("Neste"), _T("Fullf\u00f8r") },
		{ LANGID_NL_NL, _T("Vorige"), _T("Volgende"), _T("Voltooien") },
		{ LANGID_PL_PL, _T("Wstecz"), _T("Dalej"), _T("Zako\u0144cz") },
		{ LANGID_PT_BR, _T("Voltar"), _T("Avan\u00e7ar"), _T("Concluir") },
		{ LANGID_PT_PT, _T("Voltar"), _T("Seguinte"), _T("Concluir") },
		{ LANGID_RO_RO, _T("\u00cenapoi"), _T("\u00cenainte"), _T("Finalizare") },
		{ LANGID_RU_RU, _T("\u041d\u0430\u0437\u0430\u0434"), _T("\u0414\u0430\u043b\u0435\u0435"), _T("\u0413\u043e\u0442\u043e\u0432\u043e") },
		{ LANGID_SL_SI, _T("Nazaj"), _T("Naprej"), _T("Dokon\u010daj") },
		{ LANGID_SQ_AL, _T("Prapa"), _T("Tjetra"), _T("P\u00ebrfundo") },
		{ LANGID_SV_SE, _T("Tillbaka"), _T("N\u00e4sta"), _T("Slutf\u00f6r") },
		{ LANGID_TR_TR, _T("Geri"), _T("\u0130leri"), _T("Bitir") },
		{ LANGID_UA_UA, _T("\u041d\u0430\u0437\u0430\u0434"), _T("\u0414\u0430\u043b\u0456"), _T("\u0413\u043e\u0442\u043e\u0432\u043e") },
		{ LANGID_UG_CN, _T("\u0626\u0627\u0644\u062f\u0649\u0646\u0642\u0649"), _T("\u0643\u06d0\u064a\u0649\u0646\u0643\u0649"), _T("\u062a\u0627\u0645\u0627\u0645") },
		{ LANGID_VA_ES, _T("Arrere"), _T("Seg\u00fcent"), _T("Finalitzar") },
		{ LANGID_VA_ES_RACV, _T("Arrere"), _T("Seg\u00fcent"), _T("Finalisar") },
		{ LANGID_VI_VN, _T("Quay l\u1ea1i"), _T("Ti\u1ebfp theo"), _T("Ho\u00e0n t\u1ea5t") },
		{ LANGID_ZH_CN, _T("\u4e0a\u4e00\u6b65"), _T("\u4e0b\u4e00\u6b65"), _T("\u5b8c\u6210") },
		{ LANGID_ZH_TW, _T("\u4e0a\u4e00\u6b65"), _T("\u4e0b\u4e00\u6b65"), _T("\u5b8c\u6210") }
	};

	const WizardButtonText defaultLabels = { LANGID_EN_US, _T("Back"), _T("Next"), _T("Finish") };
	const WizardButtonText *selected = &defaultLabels;
	for (size_t index = 0; index < _countof(labels); ++index) {
		if (labels[index].languageID == thePrefs.GetLanguageID()) {
			selected = &labels[index];
			break;
		}
	}

	CWnd *button = pSheet->GetDlgItem(ID_WIZBACK);
	if (button != NULL)
		button->SetWindowText(CString(_T("< ")) + selected->back);
	button = pSheet->GetDlgItem(ID_WIZNEXT);
	if (button != NULL)
		button->SetWindowText(CString(selected->next) + _T(" >"));
	button = pSheet->GetDlgItem(ID_WIZFINISH);
	if (button != NULL)
		button->SetWindowText(selected->finish);
	button = pSheet->GetDlgItem(IDCANCEL);
	if (button != NULL)
		button->SetWindowText(GetResString(IDS_CANCEL));
}

void CDlgPageWizard::DoDataExchange(CDataExchange *pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
}

BOOL CDlgPageWizard::OnSetActive()
{
	CPropertySheetEx *pSheet = (CPropertySheetEx*)GetParent();
	if (pSheet->IsWizard()) {
		int iPages = pSheet->GetPageCount();
		int iActPage = pSheet->GetActiveIndex();
		DWORD dwButtons = 0;
		if (iActPage > 0)
			dwButtons |= PSWIZB_BACK;
		if (iActPage < iPages)
			dwButtons |= PSWIZB_NEXT;
		if (iActPage == iPages - 1) {
			if (pSheet->m_psh.dwFlags & PSH_WIZARDHASFINISH)
				dwButtons &= ~PSWIZB_NEXT;
			dwButtons |= PSWIZB_FINISH;
		}
		pSheet->SetWizardButtons(dwButtons);
		LocalizeWizardButtons(pSheet);
	}
	return CPropertyPageEx::OnSetActive();
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1Welcome dialog

class CPPgWiz1Welcome : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1Welcome)

	enum
	{
		IDD = IDD_WIZ1_WELCOME
	};

public:
	CPPgWiz1Welcome();
	explicit CPPgWiz1Welcome(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle)
	{
	}
	virtual BOOL OnInitDialog();

protected:
	CFont m_FontTitle;

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1Welcome, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1Welcome, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1Welcome::CPPgWiz1Welcome()
	: CDlgPageWizard(CPPgWiz1Welcome::IDD)
{
}

BOOL CPPgWiz1Welcome::OnInitDialog()
{
	CreateWizardTitleFont(m_FontTitle);

	CStatic *pStatic = static_cast<CStatic*>(GetDlgItem(IDC_WIZ1_TITLE));
	pStatic->SetFont(&m_FontTitle);

	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	SetDlgItemText(IDC_WIZ1_TITLE, GetWizardText(IDS_WIZ1_WELCOME_TITLE));
	SetDlgItemText(IDC_WIZ1_ACTIONS, GetWizardText(IDS_WIZ1_WELCOME_ACTIONS));
	SetDlgItemText(IDC_WIZ1_BTN_HINT, GetWizardText(IDS_WIZ1_WELCOME_BTN_HINT));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1General dialog

class CPPgWiz1General : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1General)

	enum
	{
		IDD = IDD_WIZ1_GENERAL
	};

public:
	CPPgWiz1General();
	explicit CPPgWiz1General(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle), m_iAutoConnectAtStart(), m_iAutoStart()
	{
	}
	virtual BOOL OnInitDialog();

	CString m_strNick;
	int m_iAutoConnectAtStart;
	int m_iAutoStart;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1General, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1General, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1General::CPPgWiz1General()
	: CDlgPageWizard(CPPgWiz1General::IDD)
	, m_iAutoConnectAtStart()
	, m_iAutoStart()
{
}

void CPPgWiz1General::DoDataExchange(CDataExchange *pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NICK, m_strNick);
	DDX_Check(pDX, IDC_AUTOCONNECT, m_iAutoConnectAtStart);
	DDX_Check(pDX, IDC_AUTOSTART, m_iAutoStart);
}

BOOL CPPgWiz1General::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	CEdit* nickEdit = static_cast<CEdit*>(GetDlgItem(IDC_NICK));
	nickEdit->SetLimitText(thePrefs.GetMaxUserNickLength());
	// Long repository-based aliases must open from their beginning instead of
	// leaving the user at the visually ambiguous tail of the edit control.
	nickEdit->SetSel(0, 0);
	SetDlgItemText(IDC_NICK_FRM, GetWizardText(IDS_ENTERUSERNAME));
	SetDlgItemText(IDC_AUTOCONNECT, GetWizardText(IDS_FIRSTAUTOCON));
	SetDlgItemText(IDC_AUTOSTART, GetWizardText(IDS_WIZ_STARTWITHWINDOWS));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1Ports and automatic router setup

class CPPgWiz1Ports : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1Ports)

	enum
	{
		IDD = IDD_WIZ1_PORTS
	};
public:
	CPPgWiz1Ports();
	explicit CPPgWiz1Ports(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle)
		, m_uTCP()
		, m_uUDP()
		, m_iAutomaticPortSetup()
	{
	}

	virtual BOOL OnInitDialog();
	UINT	m_uTCP;
	UINT	m_uUDP;
	int		m_iAutomaticPortSetup;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnKillActive();
	bool	ValidatePorts();

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1Ports, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1Ports, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1Ports::CPPgWiz1Ports()
	: CDlgPageWizard(CPPgWiz1Ports::IDD)
	, m_uTCP()
	, m_uUDP()
	, m_iAutomaticPortSetup()
{
}

void CPPgWiz1Ports::DoDataExchange(CDataExchange *pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TCP, m_uTCP);
	DDV_MinMaxUInt(pDX, m_uTCP, 1, 65535);
	DDX_Text(pDX, IDC_UDP, m_uUDP);
	DDV_MinMaxUInt(pDX, m_uUDP, 1, 65535);
	DDX_Check(pDX, IDC_WIZ_AUTO_PORTS, m_iAutomaticPortSetup);
}

bool CPPgWiz1Ports::ValidatePorts()
{
	if (!ConnectionSetupPolicy::ValidPorts(GetDlgItemInt(IDC_TCP, NULL, FALSE),
		GetDlgItemInt(IDC_UDP, NULL, FALSE), false))
	{
		LocMessageBox(IDS_CONNSETUP_BADPORT, MB_OK | MB_ICONWARNING);
		return false;
	}
	return UpdateData(TRUE) != FALSE;
}

BOOL CPPgWiz1Ports::OnKillActive()
{
	return ValidatePorts() && CDlgPageWizard::OnKillActive();
}

BOOL CPPgWiz1Ports::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);

	static_cast<CEdit*>(GetDlgItem(IDC_TCP))->SetLimitText(5);
	static_cast<CEdit*>(GetDlgItem(IDC_UDP))->SetLimitText(5);

	SetDlgItemText(IDC_PORTINFO, GetWizardText(IDS_CONNSETUP_INFO));
	SetDlgItemText(IDC_WIZ_AUTO_PORTS, GetWizardText(IDS_CONNSETUP_BUTTON));
	SetDlgItemText(IDC_WIZ_SETUP_INFO, GetWizardText(IDS_WIZSETUP_PROTOCOLS));

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1UlPrio dialog

class CPPgWiz1UlPrio : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1UlPrio)

	enum
	{
		IDD = IDD_WIZ1_ULDL_PRIO
	};

public:
	CPPgWiz1UlPrio();
	explicit CPPgWiz1UlPrio(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle), m_iUAP(1), m_iDAP(1)
	{
	}
	virtual BOOL OnInitDialog();

	int m_iUAP;
	int m_iDAP;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1UlPrio, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1UlPrio, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1UlPrio::CPPgWiz1UlPrio()
	: CDlgPageWizard(CPPgWiz1UlPrio::IDD)
	, m_iUAP(1)
	, m_iDAP(1)
{
}

void CPPgWiz1UlPrio::DoDataExchange(CDataExchange *pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_UAP, m_iUAP);
	DDX_Check(pDX, IDC_DAP, m_iDAP);
}

BOOL CPPgWiz1UlPrio::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	SetDlgItemText(IDC_UAP, GetResString(IDS_FIRSTAUTOUP));
	SetDlgItemText(IDC_DAP, GetResString(IDS_FIRSTAUTODOWN));

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1Upload dialog

class CPPgWiz1Upload : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1Upload)

	enum
	{
		IDD = IDD_WIZ1_UPLOAD
	};

public:
	CPPgWiz1Upload();
	explicit CPPgWiz1Upload(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle), m_iObfuscation()
	{
	}
	virtual BOOL OnInitDialog();

	int m_iObfuscation;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1Upload, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1Upload, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1Upload::CPPgWiz1Upload()
	: CDlgPageWizard(CPPgWiz1Upload::IDD)
	, m_iObfuscation()
{
}

void CPPgWiz1Upload::DoDataExchange(CDataExchange *pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_WIZZARDOBFUSCATION, m_iObfuscation);
}

BOOL CPPgWiz1Upload::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	SetDlgItemText(IDC_WIZZARDOBFUSCATION, GetResString(IDS_WIZZARDOBFUSCATION));
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1Speed dialog

namespace
{
const UINT WM_WIZARD_SPEEDTEST_FINISHED = WM_APP + 201;

struct SpeedTestThreadContext
{
	HWND destination;
};

UINT AFX_CDECL RunWizardSpeedTest(LPVOID parameter)
{
	std::unique_ptr<SpeedTestThreadContext> context(static_cast<SpeedTestThreadContext*>(parameter));
	std::unique_ptr<ConnectionSpeedTest::Result> result(new ConnectionSpeedTest::Result(ConnectionSpeedTest::Run()));
	if (!::PostMessage(context->destination, WM_WIZARD_SPEEDTEST_FINISHED, 0,
		reinterpret_cast<LPARAM>(result.get())))
		return 0;
	result.release();
	return 0;
}
}

class CPPgWiz1Speed : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1Speed)

	enum { IDD = IDD_WIZ1_SPEEDTEST };

public:
	CPPgWiz1Speed();
	explicit CPPgWiz1Speed(UINT nIDTemplate, LPCTSTR pszCaption = NULL,
		LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle)
		, m_uDownloadCapacity(), m_uUploadCapacity(), m_iApplyResults(), m_testRunning(false)
	{
	}
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnQueryCancel();

	UINT m_uDownloadCapacity;
	UINT m_uUploadCapacity;
	int m_iApplyResults;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnStartSpeedTest();
	afx_msg LRESULT OnSpeedTestFinished(WPARAM, LPARAM resultPointer);
	void SetRunning(bool running);

	bool m_testRunning;
	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1Speed, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1Speed, CDlgPageWizard)
	ON_BN_CLICKED(IDC_WIZ_SPEED_START, OnStartSpeedTest)
	ON_MESSAGE(WM_WIZARD_SPEEDTEST_FINISHED, OnSpeedTestFinished)
END_MESSAGE_MAP()

CPPgWiz1Speed::CPPgWiz1Speed()
	: CDlgPageWizard(CPPgWiz1Speed::IDD)
	, m_uDownloadCapacity()
	, m_uUploadCapacity()
	, m_iApplyResults()
	, m_testRunning(false)
{
}

void CPPgWiz1Speed::DoDataExchange(CDataExchange* pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WIZ_SPEED_DOWNLOAD, m_uDownloadCapacity);
	DDX_Text(pDX, IDC_WIZ_SPEED_UPLOAD, m_uUploadCapacity);
	DDX_Check(pDX, IDC_WIZ_SPEED_APPLY, m_iApplyResults);
}

BOOL CPPgWiz1Speed::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	static_cast<CEdit*>(GetDlgItem(IDC_WIZ_SPEED_DOWNLOAD))->SetLimitText(9);
	static_cast<CEdit*>(GetDlgItem(IDC_WIZ_SPEED_UPLOAD))->SetLimitText(9);
	SetDlgItemText(IDC_WIZ_SPEED_INFO, GetWizardText(IDS_WIZSETUP_SPEEDTEST_INFO));
	SetDlgItemText(IDC_WIZ_SPEED_START, GetWizardText(IDS_CONNECTIONTEST));
	SetDlgItemText(IDC_WIZ_SPEED_DOWNLOAD_LABEL, GetWizardText(IDS_DOWNLOAD));
	SetDlgItemText(IDC_WIZ_SPEED_UPLOAD_LABEL, GetWizardText(IDS_ST_UPLOAD));
	CString applyText;
	applyText.Format(_T("%s: %s"), (LPCTSTR)GetWizardText(IDS_PW_APPLY),
		(LPCTSTR)GetWizardText(IDS_SPEED_LIMITS));
	SetDlgItemText(IDC_WIZ_SPEED_APPLY, applyText);
	return TRUE;
}

BOOL CPPgWiz1Speed::OnSetActive()
{
	SetRunning(m_testRunning);
	return CDlgPageWizard::OnSetActive();
}

BOOL CPPgWiz1Speed::OnKillActive()
{
	if (m_testRunning) {
		MessageBeep(MB_ICONINFORMATION);
		return FALSE;
	}
	if (!UpdateData(TRUE))
		return FALSE;
	if (m_iApplyResults != 0 && (m_uDownloadCapacity == 0 || m_uUploadCapacity == 0
		|| m_uDownloadCapacity >= UNLIMITED || m_uUploadCapacity >= UNLIMITED)) {
		LocMessageBox(IDS_WIZSETUP_SPEEDTEST_INVALID, MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	return CDlgPageWizard::OnKillActive();
}

BOOL CPPgWiz1Speed::OnQueryCancel()
{
	if (m_testRunning) {
		MessageBeep(MB_ICONINFORMATION);
		return FALSE;
	}
	return CDlgPageWizard::OnQueryCancel();
}

void CPPgWiz1Speed::SetRunning(bool running)
{
	m_testRunning = running;
	GetDlgItem(IDC_WIZ_SPEED_START)->EnableWindow(!running);
	GetDlgItem(IDC_WIZ_SPEED_DOWNLOAD)->EnableWindow(!running);
	GetDlgItem(IDC_WIZ_SPEED_UPLOAD)->EnableWindow(!running);
	GetDlgItem(IDC_WIZ_SPEED_APPLY)->EnableWindow(!running);
	CPropertySheet* sheet = static_cast<CPropertySheet*>(GetParent());
	if (sheet != NULL)
		sheet->SetWizardButtons(running ? 0 : PSWIZB_BACK | PSWIZB_NEXT);
}

void CPPgWiz1Speed::OnStartSpeedTest()
{
	if (m_testRunning)
		return;
	m_iApplyResults = 0;
	UpdateData(FALSE);
	SetDlgItemText(IDC_WIZ_SPEED_STATUS, GetWizardText(IDS_CONNECTIONTEST) + _T("..."));
	SetRunning(true);
	std::unique_ptr<SpeedTestThreadContext> context(new SpeedTestThreadContext{GetSafeHwnd()});
	if (AfxBeginThread(RunWizardSpeedTest, context.get(), THREAD_PRIORITY_NORMAL) == NULL) {
		SetRunning(false);
		SetDlgItemText(IDC_WIZ_SPEED_STATUS,
			GetWizardText(IDS_CONNECTIONTEST) + _T(": ") + GetWizardText(IDS_FAILED));
		return;
	}
	context.release();
}

LRESULT CPPgWiz1Speed::OnSpeedTestFinished(WPARAM, LPARAM resultPointer)
{
	std::unique_ptr<ConnectionSpeedTest::Result> result(
		reinterpret_cast<ConnectionSpeedTest::Result*>(resultPointer));
	if (result != nullptr && result->Succeeded()) {
		m_uDownloadCapacity = result->downloadKBps;
		m_uUploadCapacity = result->uploadKBps;
		m_iApplyResults = 1;
		UpdateData(FALSE);
		CString status;
		status.Format(_T("%s: %u %s | %s: %u %s"),
			(LPCTSTR)GetWizardText(IDS_DOWNLOAD), m_uDownloadCapacity,
			(LPCTSTR)GetWizardText(IDS_KBYTESPERSEC),
			(LPCTSTR)GetWizardText(IDS_ST_UPLOAD), m_uUploadCapacity,
			(LPCTSTR)GetWizardText(IDS_KBYTESPERSEC));
		SetDlgItemText(IDC_WIZ_SPEED_STATUS, status);
	} else {
		CString status;
		status.Format(_T("%s: %s (%lu)"), (LPCTSTR)GetWizardText(IDS_CONNECTIONTEST),
			(LPCTSTR)GetWizardText(IDS_FAILED), result != nullptr ? result->error : ERROR_GEN_FAILURE);
		SetDlgItemText(IDC_WIZ_SPEED_STATUS, status);
	}
	SetRunning(false);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1Server dialog

class CPPgWiz1Server : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1Server)

	enum
	{
		IDD = IDD_WIZ1_SERVER
	};

public:
	CPPgWiz1Server();
	explicit CPPgWiz1Server(UINT nIDTemplate, LPCTSTR pszCaption = NULL, LPCTSTR pszHeaderTitle = NULL, LPCTSTR pszHeaderSubTitle = NULL)
		: CDlgPageWizard(nIDTemplate, pszCaption, pszHeaderTitle, pszHeaderSubTitle)
		, m_iSafeServerConnect(), m_iKademlia(1), m_iED2K(1), m_iAddServers(1), m_iAddKadNodes(1)
	{
	}
	virtual BOOL OnInitDialog();

	int m_iSafeServerConnect;
	int m_iKademlia;
	int m_iED2K;
	int m_iAddServers;
	int m_iAddKadNodes;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	afx_msg void OnNetworkChanged();
	void UpdateDownloadChoices();

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1Server, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1Server, CDlgPageWizard)
	ON_BN_CLICKED(IDC_WIZARD_NETWORK_ED2K, OnNetworkChanged)
	ON_BN_CLICKED(IDC_WIZARD_NETWORK_KADEMLIA, OnNetworkChanged)
END_MESSAGE_MAP()

CPPgWiz1Server::CPPgWiz1Server()
	: CDlgPageWizard(CPPgWiz1Server::IDD)
	, m_iSafeServerConnect()
	, m_iKademlia(1)
	, m_iED2K(1)
	, m_iAddServers(1)
	, m_iAddKadNodes(1)
{
}

void CPPgWiz1Server::DoDataExchange(CDataExchange *pDX)
{
	CDlgPageWizard::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SAFESERVERCONNECT, m_iSafeServerConnect);
	DDX_Check(pDX, IDC_WIZARD_NETWORK_KADEMLIA, m_iKademlia);
	DDX_Check(pDX, IDC_WIZARD_NETWORK_ED2K, m_iED2K);
	DDX_Check(pDX, IDC_WIZ_SERVERS_ADD, m_iAddServers);
	DDX_Check(pDX, IDC_WIZ_NODES_ADD, m_iAddKadNodes);
}

BOOL CPPgWiz1Server::OnInitDialog()
{
	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	SetDlgItemText(IDC_SAFESERVERCONNECT, GetWizardText(IDS_FIRSTSAFECON));
	SetDlgItemText(IDC_WIZARD_NETWORK, GetWizardText(IDS_WIZARD_NETWORK));
	SetDlgItemText(IDC_WIZARD_ED2K, _T("eD2K / Kad"));
	SetDlgItemText(IDC_WIZ_SERVERS_ADD, RemoveTrailingChoiceNote(GetWizardText(IDS_WIZSETUP_SERVERS_ADD)));
	SetDlgItemText(IDC_WIZ_NODES_ADD, RemoveTrailingChoiceNote(GetWizardText(IDS_WIZSETUP_NODES_ADD)));
	return TRUE;
}

BOOL CPPgWiz1Server::OnSetActive()
{
	UpdateDownloadChoices();
	return CDlgPageWizard::OnSetActive();
}

void CPPgWiz1Server::OnNetworkChanged()
{
	UpdateDownloadChoices();
}

void CPPgWiz1Server::UpdateDownloadChoices()
{
	GetDlgItem(IDC_WIZ_SERVERS_ADD)->EnableWindow(IsDlgButtonChecked(IDC_WIZARD_NETWORK_ED2K) != 0);
	GetDlgItem(IDC_WIZ_NODES_ADD)->EnableWindow(IsDlgButtonChecked(IDC_WIZARD_NETWORK_KADEMLIA) != 0);
}


///////////////////////////////////////////////////////////////////////////////
// CPPgWiz1End dialog

class CPPgWiz1End : public CDlgPageWizard
{
	DECLARE_DYNAMIC(CPPgWiz1End)

	enum
	{
		IDD = IDD_WIZ1_END
	};

public:
	CPPgWiz1End();
	CPPgWiz1End(LPCTSTR caption, CPPgWiz1Ports& ports, CPPgWiz1Server& networks)
		: CDlgPageWizard(IDD_WIZ1_END, caption), m_ports(&ports), m_networks(&networks)
	{
	}
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

protected:
	CFont m_FontTitle;
	CPPgWiz1Ports* m_ports;
	CPPgWiz1Server* m_networks;

	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPPgWiz1End, CDlgPageWizard)

BEGIN_MESSAGE_MAP(CPPgWiz1End, CDlgPageWizard)
END_MESSAGE_MAP()

CPPgWiz1End::CPPgWiz1End()
	: CDlgPageWizard(CPPgWiz1End::IDD)
	, m_ports(NULL)
	, m_networks(NULL)
{
}

BOOL CPPgWiz1End::OnInitDialog()
{
	CreateWizardTitleFont(m_FontTitle);

	CStatic *pStatic = static_cast<CStatic*>(GetDlgItem(IDC_WIZ1_TITLE));
	pStatic->SetFont(&m_FontTitle);

	CDlgPageWizard::OnInitDialog();
	InitWindowStyles(this);
	CString readyText(GetWizardText(IDS_MAIN_READY));
	CString readyTitle;
	readyTitle.Format(readyText, EMULE_NEXT_VERSION_STRING);
	SetDlgItemText(IDC_WIZ1_TITLE, readyTitle);
	SetDlgItemText(IDC_WIZ1_BTN_HINT, GetWizardText(IDS_WIZ1_END_BTN_HINT));

	return TRUE;
}

BOOL CPPgWiz1End::OnSetActive()
{
	CString summary(GetWizardText(IDS_FIRSTCOMPLETE));
	int paragraphEnd = summary.Find(_T("\r\n\r\n"));
	if (paragraphEnd < 0)
		paragraphEnd = summary.Find(_T("\n\n"));
	if (paragraphEnd >= 0)
		summary = summary.Left(paragraphEnd);

	if (m_ports != NULL && m_networks != NULL) {
		const CString enabled(GetWizardText(IDS_ENABLED));
		const CString disabled(GetWizardText(IDS_DISABLED));
		CString networks;
		if (m_networks->m_iED2K != 0)
			networks = _T("eD2K");
		if (m_networks->m_iKademlia != 0) {
			if (!networks.IsEmpty())
				networks += _T(" + ");
			networks += _T("Kad");
		}
		if (networks.IsEmpty())
			networks = disabled;

		summary.AppendFormat(_T("\r\n\r\n%s: %s\r\n%s: %s\r\n%s: %s\r\n%s: %s"),
			(LPCTSTR)GetWizardText(IDS_CONNSETUP_TITLE),
			(LPCTSTR)(m_ports->m_iAutomaticPortSetup != 0 ? enabled : disabled),
			(LPCTSTR)GetWizardText(IDS_NETWORK), (LPCTSTR)networks,
			(LPCTSTR)RemoveTrailingChoiceNote(GetWizardText(IDS_WIZSETUP_SERVERS_ADD)),
			(LPCTSTR)(m_networks->m_iAddServers != 0 && m_networks->m_iED2K != 0 ? enabled : disabled),
			(LPCTSTR)RemoveTrailingChoiceNote(GetWizardText(IDS_WIZSETUP_NODES_ADD)),
			(LPCTSTR)(m_networks->m_iAddKadNodes != 0 && m_networks->m_iKademlia != 0 ? enabled : disabled));
	}
	SetDlgItemText(IDC_WIZ1_ACTIONS, summary);
	return CDlgPageWizard::OnSetActive();
}


///////////////////////////////////////////////////////////////////////////////
// CPShtWiz1

class CPShtWiz1 : public CPropertySheetEx
{
	DECLARE_DYNAMIC(CPShtWiz1)

public:
	explicit CPShtWiz1(UINT nIDCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

protected:
	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CPShtWiz1, CPropertySheetEx)

BEGIN_MESSAGE_MAP(CPShtWiz1, CPropertySheetEx)
END_MESSAGE_MAP()

CPShtWiz1::CPShtWiz1(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage)
	: CPropertySheetEx(nIDCaption, pParentWnd, iSelectPage)
{
}

BOOL FirstTimeWizard()
{
	static bool s_recommendedFirstRunDefaultsApplied = false;
	const bool firstRun = thePrefs.IsFirstStart() && !s_recommendedFirstRunDefaultsApplied;
	const bool automaticHomeSetupWasEnabled = thePrefs.IsUPnPEnabled() && thePrefs.IsUPnPHomeOnly();
	const CString sWiz1(GetWizardText(IDS_WIZ1));
	CEnBitmap bmWatermark;
	VERIFY(bmWatermark.LoadImage(IDR_WIZ1_WATERMARK, _T("GIF"), NULL, ::GetSysColor(COLOR_WINDOW)));
	CEnBitmap bmHeader;
	VERIFY(bmHeader.LoadImage(IDR_WIZ1_HEADER, _T("GIF"), NULL, ::GetSysColor(COLOR_WINDOW)));
	CPropertySheetEx sheet(sWiz1, NULL, 0, bmWatermark, NULL, bmHeader);
	sheet.m_psh.dwFlags |= PSH_WIZARD;
#ifdef _DEBUG
	sheet.m_psh.dwFlags |= PSH_WIZARDHASFINISH;
#endif
	sheet.m_psh.dwFlags |= PSH_WIZARD97;

	CPPgWiz1Welcome	page1(IDD_WIZ1_WELCOME, sWiz1);
	page1.m_psp.dwFlags |= PSP_HIDEHEADER;
	sheet.AddPage(&page1);

	CPPgWiz1General page2(IDD_WIZ1_GENERAL, sWiz1, GetWizardText(IDS_PW_GENERAL), GetWizardText(IDS_QL_USERNAME));
	sheet.AddPage(&page2);

	CPPgWiz1Ports page3(IDD_WIZ1_PORTS, sWiz1, GetWizardText(IDS_CONNECTION), _T("TCP / UDP - PCP / NAT-PMP / UPnP"));
	sheet.AddPage(&page3);
	CPPgWiz1Speed page4(IDD_WIZ1_SPEEDTEST, sWiz1, GetWizardText(IDS_SPEED_LIMITS), GetWizardText(IDS_CONNECTIONTEST));
	sheet.AddPage(&page4);
	CPPgWiz1Server page6(IDD_WIZ1_SERVER, sWiz1, GetWizardText(IDS_NETWORK), _T("eD2K / Kad"));
	sheet.AddPage(&page6);

	CPPgWiz1End page7(sWiz1, page3, page6);
	page7.m_psp.dwFlags |= PSP_HIDEHEADER;
	sheet.AddPage(&page7);

	page2.m_strNick = firstRun ? EMULE_NEXT_DEFAULT_ALIAS : thePrefs.GetUserNick();
	page2.m_iAutoConnectAtStart = firstRun ? 1 : thePrefs.DoAutoConnect();
	page2.m_iAutoStart = firstRun ? 0 : thePrefs.GetAutoStart();
	page3.m_uTCP = thePrefs.GetPort();
	page3.m_uUDP = thePrefs.GetUDPPort() != 0 ? thePrefs.GetUDPPort() : thePrefs.GetRandomUDPPort();
	page3.m_iAutomaticPortSetup = firstRun ? 1 : automaticHomeSetupWasEnabled;
	page4.m_uDownloadCapacity = firstRun ? 0 : thePrefs.GetMaxGraphDownloadRate();
	page4.m_uUploadCapacity = firstRun ? 0 : thePrefs.GetMaxGraphUploadRate(true);
	page4.m_iApplyResults = 0;
	page6.m_iSafeServerConnect = firstRun ? 0 : thePrefs.IsSafeServerConnectEnabled();
	page6.m_iKademlia = firstRun ? 1 : thePrefs.GetNetworkKademlia();
	page6.m_iED2K = firstRun ? 1 : thePrefs.GetNetworkED2K();
	page6.m_iAddServers = firstRun ? 1 : 0;
	page6.m_iAddKadNodes = firstRun ? 1 : 0;

	uint16 oldtcpport = thePrefs.GetPort();
	uint16 oldudpport = thePrefs.GetUDPPort();

	if (sheet.DoModal() != ID_WIZFINISH) {

		// restore port settings?
		thePrefs.port = oldtcpport;
		thePrefs.udpport = oldudpport;
		theApp.listensocket->Rebind();
		theApp.clientudp->Rebind();

		return FALSE;
	}

	page2.m_strNick.Trim();
	if (page2.m_strNick.IsEmpty())
		page2.m_strNick = EMULE_NEXT_PROJECT_URL;

	thePrefs.SetUserNick(page2.m_strNick);
	thePrefs.SetAutoConnect(page2.m_iAutoConnectAtStart != 0);
	thePrefs.SetAutoStart(page2.m_iAutoStart != 0);
	if (!thePrefs.IsPortableMode())
		SetAutoStart(thePrefs.GetAutoStart());

	if (firstRun) {
		thePrefs.SetNewAutoDown(true);
		thePrefs.SetNewAutoUp(true);
		thePrefs.m_bCryptLayerRequested = true;
		thePrefs.m_bCryptLayerSupported = true;
		thePrefs.m_bCryptLayerRequired = false;
		s_recommendedFirstRunDefaultsApplied = true;
	}
	if (page4.m_iApplyResults != 0) {
		thePrefs.SetMaxGraphDownloadRate(page4.m_uDownloadCapacity);
		thePrefs.SetMaxGraphUploadRate(page4.m_uUploadCapacity);
		thePrefs.SetMaxUpload(ConnectionSpeedTest::RecommendedUploadLimit(page4.m_uUploadCapacity));
		thePrefs.SetMaxDownload(UNLIMITED);
		theApp.emuledlg->statisticswnd->SetARange(false, page4.m_uUploadCapacity);
		theApp.emuledlg->statisticswnd->SetARange(true, page4.m_uDownloadCapacity);
	} else if (firstRun) {
		// A clean installation still carries the conservative legacy limits
		// loaded by Preferences. Do not silently apply those limits when the
		// user skips the optional speed test; keep both directions unlimited.
		thePrefs.SetMaxUpload(UNLIMITED);
		thePrefs.SetMaxDownload(UNLIMITED);
	}
	thePrefs.SetSafeServerConnectEnabled(page6.m_iSafeServerConnect != 0);
	thePrefs.SetNetworkKademlia(page6.m_iKademlia != 0);
	thePrefs.SetNetworkED2K(page6.m_iED2K != 0);
	theApp.serverlist->RequestBootstrapServers(true, page6.m_iAddServers != 0, page6.m_iED2K != 0);
	if (page6.m_iAddKadNodes != 0 && page6.m_iKademlia != 0)
		theApp.emuledlg->kademliawnd->UpdateNodesDatFromURL(KadBootstrap::OnlineNodesDatUrl, false);

	// set ports
	thePrefs.port = (uint16)page3.m_uTCP;
	thePrefs.udpport = (uint16)page3.m_uUDP;
	if ((thePrefs.port != theApp.listensocket->GetConnectedPort()) || (thePrefs.udpport != theApp.clientudp->GetConnectedPort()))
		if (!theApp.IsPortchangeAllowed())
			LocMessageBox(IDS_NOPORTCHANGEPOSSIBLE, MB_OK, 0);
		else {
			theApp.listensocket->Rebind();
			theApp.clientudp->Rebind();
		}

	if (thePrefs.GetPort() != theApp.listensocket->GetConnectedPort()
		|| thePrefs.GetUDPPort() != theApp.clientudp->GetConnectedPort())
	{
		thePrefs.port = oldtcpport;
		thePrefs.udpport = oldudpport;
		theApp.listensocket->Rebind();
		theApp.clientudp->Rebind();
		LocMessageBox(IDS_CONNSETUP_BINDFAILED, MB_OK | MB_ICONWARNING);
	} else if (page3.m_iAutomaticPortSetup != 0) {
		// Commit only after Finish and successful binding. StartUPnP rechecks
		// the network, since it may have changed while the wizard was open.
		thePrefs.EnableAutomaticHomeUPnP();
		if (!automaticHomeSetupWasEnabled)
			theApp.emuledlg->StartUPnP(true, 0, 0, true);
	} else if (automaticHomeSetupWasEnabled) {
		thePrefs.DisableAutomaticHomeUPnP();
	}

	// The modal wizard has its own message loop, so the regular startup timer
	// may already have checked the old auto-connect value. Complete the first
	// run hand-off here in that case. StartConnection also waits for an active
	// automatic router setup before contacting eD2K or Kad.
	if (ConnectionSetupPolicy::ShouldConnectAfterFirstRunFinish(firstRun,
		thePrefs.DoAutoConnect(), theApp.m_app_state == APP_STATE_RUNNING))
	{
		theApp.emuledlg->SendMessage(WM_COMMAND, MP_CONNECT);
	}

	return TRUE;
}
