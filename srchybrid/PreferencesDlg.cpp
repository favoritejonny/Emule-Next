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
#include "PreferencesDlg.h"
#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
enum ENextPreferencesPageIcon
{
	enppiGeneral,
	enppiDisplay,
	enppiConnection,
	enppiProxy,
	enppiServer,
	enppiDirectories,
	enppiFiles,
	enppiNotify,
	enppiStatistics,
	enppiIrc,
	enppiSecurity,
	enppiScheduler,
	enppiWeb,
	enppiTweaks,
	enppiMessages
};

static Gdiplus::Color GetNextPreferencesIconColor(ENextPreferencesPageIcon eIcon)
{
	COLORREF color = RGB(55, 150, 205);
	switch (eIcon) {
	case enppiDirectories:
	case enppiTweaks:
		color = RGB(243, 155, 80);
		break;
	case enppiFiles:
	case enppiStatistics:
		color = RGB(73, 176, 150);
		break;
	case enppiNotify:
	case enppiScheduler:
		color = RGB(142, 108, 220);
		break;
	case enppiSecurity:
		color = RGB(70, 106, 196);
		break;
	default:
		break;
	}
	return Gdiplus::Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
}

static void DrawNextPreferencesPageIcon(Gdiplus::Graphics& graphics, int iIconSize, ENextPreferencesPageIcon eIcon)
{
	const float scale = static_cast<float>(iIconSize) / 32.0f;
	const auto P = [scale](float value) { return value * scale; };
	const auto Rect = [&P](float left, float top, float right, float bottom) {
		return Gdiplus::RectF(P(left), P(top), P(right) - P(left), P(bottom) - P(top));
	};
	const Gdiplus::Color color = GetNextPreferencesIconColor(eIcon);
	const Gdiplus::Color fillColor(48, color.GetR(), color.GetG(), color.GetB());
	const float stroke = 2.35f * scale;
	Gdiplus::Pen pen(color, stroke);
	pen.SetLineJoin(Gdiplus::LineJoinRound);
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);
	Gdiplus::SolidBrush brush(color);
	Gdiplus::SolidBrush fillBrush(fillColor);

	switch (eIcon) {
	case enppiGeneral: // compact sliders
		for (int i = 0; i < 3; ++i)
			graphics.DrawLine(&pen, P(5), P(8.0f + static_cast<float>(i) * 8.0f), P(27), P(8.0f + static_cast<float>(i) * 8.0f));
		graphics.FillEllipse(&brush, Rect(11, 5, 17, 11));
		graphics.FillEllipse(&brush, Rect(19, 13, 25, 19));
		graphics.FillEllipse(&brush, Rect(8, 21, 14, 27));
		break;
	case enppiDisplay: // monitor
		graphics.DrawRectangle(&pen, Rect(5, 6, 27, 21));
		graphics.DrawLine(&pen, P(12), P(26), P(20), P(26));
		graphics.DrawLine(&pen, P(16), P(21), P(16), P(26));
		graphics.FillRectangle(&fillBrush, Rect(8, 9, 24, 18));
		break;
	case enppiConnection: // connected peers
		graphics.DrawLine(&pen, P(10), P(10), P(22), P(10));
		graphics.DrawLine(&pen, P(10), P(10), P(16), P(23));
		graphics.DrawLine(&pen, P(22), P(10), P(16), P(23));
		graphics.FillEllipse(&brush, Rect(6, 6, 14, 14));
		graphics.FillEllipse(&brush, Rect(18, 6, 26, 14));
		graphics.FillEllipse(&brush, Rect(12, 19, 20, 27));
		break;
	case enppiProxy: // protected route
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(5), P(16), P(27), P(16));
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 270, 180);
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 90, 180);
		break;
	case enppiServer: // server rack
		graphics.DrawRectangle(&pen, Rect(6, 5, 26, 13));
		graphics.DrawRectangle(&pen, Rect(6, 18, 26, 26));
		graphics.FillEllipse(&brush, Rect(9, 8, 12, 11));
		graphics.FillEllipse(&brush, Rect(9, 21, 12, 24));
		graphics.DrawLine(&pen, P(16), P(9), P(23), P(9));
		graphics.DrawLine(&pen, P(16), P(22), P(23), P(22));
		break;
	case enppiDirectories: // folder
		graphics.DrawLine(&pen, P(4), P(11), P(12), P(11));
		graphics.DrawLine(&pen, P(12), P(11), P(15), P(8));
		graphics.DrawLine(&pen, P(15), P(8), P(27), P(8));
		graphics.DrawLine(&pen, P(27), P(8), P(27), P(25));
		graphics.DrawLine(&pen, P(27), P(25), P(4), P(25));
		graphics.DrawLine(&pen, P(4), P(25), P(4), P(11));
		graphics.FillRectangle(&fillBrush, Rect(6, 14, 25, 23));
		break;
	case enppiFiles: // document
		graphics.DrawRectangle(&pen, Rect(7, 4, 24, 28));
		graphics.DrawLine(&pen, P(11), P(11), P(20), P(11));
		graphics.DrawLine(&pen, P(11), P(16), P(20), P(16));
		graphics.DrawLine(&pen, P(11), P(21), P(17), P(21));
		break;
	case enppiNotify: // bell
		graphics.DrawArc(&pen, Rect(9, 6, 14, 17), 180, 180);
		graphics.DrawLine(&pen, P(9), P(15), P(7), P(22));
		graphics.DrawLine(&pen, P(23), P(15), P(25), P(22));
		graphics.DrawLine(&pen, P(7), P(22), P(25), P(22));
		graphics.FillEllipse(&brush, Rect(14, 24, 18, 28));
		break;
	case enppiStatistics: // bars
		graphics.DrawLine(&pen, P(5), P(27), P(27), P(27));
		graphics.FillRectangle(&brush, Rect(7, 18, 11, 27));
		graphics.FillRectangle(&brush, Rect(14, 12, 18, 27));
		graphics.FillRectangle(&brush, Rect(21, 6, 25, 27));
		break;
	case enppiIrc: // conversation
		{
			Gdiplus::PointF points[] = { { P(5), P(7) }, { P(27), P(7) }, { P(27), P(21) }, { P(16), P(21) }, { P(10), P(26) }, { P(11), P(21) }, { P(5), P(21) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.DrawLine(&pen, P(10), P(13), P(22), P(13));
			graphics.DrawLine(&pen, P(10), P(17), P(18), P(17));
		}
		break;
	case enppiSecurity: // shield with lock
		{
			Gdiplus::PointF points[] = { { P(16), P(4) }, { P(25), P(8) }, { P(23), P(19) }, { P(16), P(28) }, { P(9), P(19) }, { P(7), P(8) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.FillRectangle(&brush, Rect(12, 15, 20, 22));
			graphics.DrawArc(&pen, Rect(13, 11, 6, 8), 180, 180);
		}
		break;
	case enppiScheduler: // clock
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(16), P(16), P(16), P(9));
		graphics.DrawLine(&pen, P(16), P(16), P(22), P(19));
		graphics.FillEllipse(&brush, Rect(14, 14, 18, 18));
		break;
	case enppiWeb: // globe
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(5), P(16), P(27), P(16));
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 270, 180);
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 90, 180);
		break;
	case enppiTweaks: // adjustable tool
		graphics.DrawLine(&pen, P(9), P(23), P(23), P(9));
		graphics.DrawEllipse(&pen, Rect(18, 4, 28, 14));
		graphics.FillEllipse(&brush, Rect(6, 20, 12, 26));
		break;
	case enppiMessages: // message bubbles
		graphics.DrawRectangle(&pen, Rect(5, 6, 23, 19));
		graphics.DrawLine(&pen, P(10), P(19), P(8), P(23));
		graphics.DrawRectangle(&pen, Rect(10, 13, 27, 25));
		graphics.FillRectangle(&fillBrush, Rect(13, 16, 24, 22));
		break;
	}
}

class CNextPreferencesIconFactory
{
public:
	CNextPreferencesIconFactory()
		: m_gdiplusToken()
		, m_bReady(false)
	{
		Gdiplus::GdiplusStartupInput startupInput;
		m_bReady = Gdiplus::GdiplusStartup(&m_gdiplusToken, &startupInput, NULL) == Gdiplus::Ok;
	}

	~CNextPreferencesIconFactory()
	{
		if (m_bReady)
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}

	HICON Create(ENextPreferencesPageIcon eIcon) const
	{
		if (!m_bReady)
			return NULL;

		const int iOutputSize = 32;
		const int iScale = 4;
		Gdiplus::Bitmap source(iOutputSize * iScale, iOutputSize * iScale, PixelFormat32bppPARGB);
		Gdiplus::Bitmap output(iOutputSize, iOutputSize, PixelFormat32bppPARGB);
		if (source.GetLastStatus() != Gdiplus::Ok || output.GetLastStatus() != Gdiplus::Ok)
			return NULL;

		Gdiplus::Graphics sourceGraphics(&source);
		sourceGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		sourceGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		sourceGraphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		sourceGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
		DrawNextPreferencesPageIcon(sourceGraphics, iOutputSize * iScale, eIcon);

		Gdiplus::Graphics outputGraphics(&output);
		outputGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		outputGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		outputGraphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
		outputGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
		outputGraphics.DrawImage(&source, Gdiplus::Rect(0, 0, iOutputSize, iOutputSize), 0, 0,
			source.GetWidth(), source.GetHeight(), Gdiplus::UnitPixel);

		HICON hIcon = NULL;
		return output.GetHICON(&hIcon) == Gdiplus::Ok ? hIcon : NULL;
	}

private:
	ULONG_PTR m_gdiplusToken;
	bool m_bReady;
};

static bool CreateNextPreferencesTreeIcons(CImageList& imageList)
{
	static const ENextPreferencesPageIcon aIcons[] = {
		enppiGeneral, enppiDisplay, enppiConnection, enppiProxy, enppiServer,
		enppiDirectories, enppiFiles, enppiNotify, enppiStatistics, enppiIrc,
		enppiMessages, enppiSecurity, enppiScheduler, enppiWeb, enppiTweaks
	};

	imageList.DeleteImageList();
	if (!imageList.Create(20, 20, ILC_COLOR32 | ILC_MASK, _countof(aIcons), 1))
		return false;

	CNextPreferencesIconFactory iconFactory;
	for (int i = 0; i < static_cast<int>(_countof(aIcons)); ++i) {
		HICON hIcon = iconFactory.Create(aIcons[i]);
		if (hIcon == NULL || imageList.Add(hIcon) < 0) {
			if (hIcon != NULL)
				::DestroyIcon(hIcon);
			imageList.DeleteImageList();
			return false;
		}
		::DestroyIcon(hIcon);
	}
	return true;
}
}


IMPLEMENT_DYNAMIC(CPreferencesDlg, CTreePropSheet)

BEGIN_MESSAGE_MAP(CPreferencesDlg, CTreePropSheet)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

CPreferencesDlg::CPreferencesDlg()
{
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_wndGeneral.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndDisplay.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndConnection.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndServer.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndDirectories.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndFiles.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndStats.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndIRC.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndWebServer.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndTweaks.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndSecurity.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndScheduler.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndProxy.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndMessages.m_psp.dwFlags &= ~PSH_HASHELP;
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	m_wndDebug.m_psp.dwFlags &= ~PSH_HASHELP;
#endif

	CTreePropSheet::SetPageIcon(&m_wndGeneral, _T("Preferences"));
	CTreePropSheet::SetPageIcon(&m_wndDisplay, _T("DISPLAY"));
	CTreePropSheet::SetPageIcon(&m_wndConnection, _T("CONNECTION"));
	CTreePropSheet::SetPageIcon(&m_wndProxy, _T("PROXY"));
	CTreePropSheet::SetPageIcon(&m_wndServer, _T("SERVER"));
	CTreePropSheet::SetPageIcon(&m_wndDirectories, _T("FOLDERS"));
	CTreePropSheet::SetPageIcon(&m_wndFiles, _T("Transfer"));
	CTreePropSheet::SetPageIcon(&m_wndNotify, _T("NOTIFICATIONS"));
	CTreePropSheet::SetPageIcon(&m_wndStats, _T("STATISTICS"));
	CTreePropSheet::SetPageIcon(&m_wndIRC, _T("IRC"));
	CTreePropSheet::SetPageIcon(&m_wndSecurity, _T("SECURITY"));
	CTreePropSheet::SetPageIcon(&m_wndScheduler, _T("SCHEDULER"));
	CTreePropSheet::SetPageIcon(&m_wndWebServer, _T("WEB"));
	CTreePropSheet::SetPageIcon(&m_wndTweaks, _T("TWEAK"));
	CTreePropSheet::SetPageIcon(&m_wndMessages, _T("MESSAGES"));
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	CTreePropSheet::SetPageIcon(&m_wndDebug, _T("Preferences"));
#endif

	AddPage(&m_wndGeneral);
	AddPage(&m_wndDisplay);
	AddPage(&m_wndConnection);
	AddPage(&m_wndProxy);
	AddPage(&m_wndServer);
	AddPage(&m_wndDirectories);
	AddPage(&m_wndFiles);
	AddPage(&m_wndNotify);
	AddPage(&m_wndStats);
	AddPage(&m_wndIRC);
	AddPage(&m_wndMessages);
	AddPage(&m_wndSecurity);
	AddPage(&m_wndScheduler);
	AddPage(&m_wndWebServer);
	AddPage(&m_wndTweaks);
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	AddPage(&m_wndDebug);
#endif

	// The height of the option dialog is already too large for 640x480. To show as much as
	// possible we do not show a page caption (which is a decorative element only anyway).
	SetTreeViewMode(TRUE, ::GetSystemMetrics(SM_CYSCREEN) >= 600, TRUE);
	SetTreeWidth(170);

	m_pPshStartPage = NULL;
	m_bSaveIniFile = false;
}
void CPreferencesDlg::OnDestroy()
{
	CTreePropSheet::OnDestroy();
	if (m_bSaveIniFile) {
		thePrefs.Save();
		m_bSaveIniFile = false;
	}
	m_pPshStartPage = GetPage(GetActiveIndex())->m_psp.pszTemplate;
}

BOOL CPreferencesDlg::OnInitDialog()
{
	ASSERT(!m_bSaveIniFile);
	BOOL bResult = CTreePropSheet::OnInitDialog();
	InitWindowStyles(this);
	if (CreateNextPreferencesTreeIcons(m_ilNextPageIcons)) {
		CTreeCtrl *pPageTree = GetPageTreeControl();
		pPageTree->SetImageList(&m_ilNextPageIcons, TVSIL_NORMAL);
		for (int i = 0; i < m_ilNextPageIcons.GetImageCount(); ++i) {
			HTREEITEM hItem = GetPageTreeItem(i);
			if (hItem != NULL)
				pPageTree->SetItemImage(hItem, i, i);
		}
	}

	for (int i = (int)m_pages.GetCount(); --i >= 0;)
		if (GetPage(i)->m_psp.pszTemplate == m_pPshStartPage) {
			SetActivePage(i);
			break;
		}

	Localize();
	return bResult;
}

void CPreferencesDlg::LocalizeItemText(int i, UINT strid)
{
	GetPageTreeControl()->SetItemText(GetPageTreeItem(i), GetResNoAmp(strid));
}

void CPreferencesDlg::Localize()
{
	SetTitle(GetResNoAmp(IDS_EM_PREFS));

	m_wndGeneral.Localize();
	m_wndDisplay.Localize();
	m_wndConnection.Localize();
	m_wndServer.Localize();
	m_wndDirectories.Localize();
	m_wndFiles.Localize();
	m_wndStats.Localize();
	m_wndNotify.Localize();
	m_wndIRC.Localize();
	m_wndSecurity.Localize();
	m_wndTweaks.Localize();
	m_wndWebServer.Localize();
	m_wndScheduler.Localize();
	m_wndProxy.Localize();
	m_wndMessages.Localize();

	if (GetPageTreeControl()) {
		static const UINT uids[15] =
		{
			IDS_PW_GENERAL, IDS_PW_DISPLAY, IDS_CONNECTION, IDS_PW_PROXY, IDS_PW_SERVER,
			IDS_PW_DIR, IDS_PW_FILES, IDS_PW_EKDEV_OPTIONS, IDS_STATSSETUPINFO, IDS_IRC,
			IDS_MESSAGESCOMMENTS, IDS_SECURITY, IDS_SCHEDULER, IDS_PW_WS, IDS_PW_TWEAK
		};

		int c;
		for (c = 0; c < (int)_countof(uids); ++c)
			LocalizeItemText(c, uids[c]);
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
		GetPageTreeControl()->SetItemText(GetPageTreeItem(c), _T("Debug"));
#endif
	}

	UpdateCaption();
}

void CPreferencesDlg::OnHelp()
{
	int iCurSel = GetActiveIndex();
	if (iCurSel >= 0) {
		CPropertyPage *pPage = GetPage(iCurSel);
		if (pPage) {
			HELPINFO hi = {};
			hi.cbSize = (UINT)sizeof hi;
			hi.iContextType = HELPINFO_WINDOW;
			//hi.iCtrlId = 0;
			hi.hItemHandle = pPage->m_hWnd;
			//hi.dwContextId = 0;
			pPage->SendMessage(WM_HELP, 0, (LPARAM)&hi);
			return;
		}
	}

	theApp.ShowHelp(0, HELP_CONTENTS);
}

BOOL CPreferencesDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case ID_HELP:
		return OnHelpInfo(NULL);
	case IDOK:
	case ID_APPLY_NOW:
		m_bSaveIniFile = true;
	}
	return __super::OnCommand(wParam, lParam);
}

BOOL CPreferencesDlg::OnHelpInfo(HELPINFO*)
{
	OnHelp();
	return TRUE;
}
