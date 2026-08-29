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
#include "TitledMenu.h"
#include "emule.h"
#include "preferences.h"
#include "otherfunctions.h"
#include "NextTheme.h"
#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MP_TITLE	0xFFFEu
#define ICONSIZE	18

CTitledMenu::CTitledMenu()
	: m_clRight(::GetSysColor(COLOR_GRADIENTACTIVECAPTION))
	, m_clLeft(::GetSysColor(COLOR_ACTIVECAPTION))
	, m_clText(::GetSysColor(COLOR_CAPTIONTEXT))
	, m_uEdgeFlags(BDR_SUNKENINNER)
	, m_bDrawEdge()
	, m_bIconMenu()
{
	const EEmuleNextTheme theme = CNextTheme::GetTheme();
	if (theme == entModernLight) {
		m_clLeft = RGB(51, 145, 210);
		m_clRight = RGB(96, 187, 219);
		m_clText = RGB(255, 255, 255);
		m_uEdgeFlags = BDR_RAISEDINNER;
	} else if (theme == entModernAurora) {
		m_clLeft = RGB(48, 167, 173);
		m_clRight = RGB(111, 198, 186);
		m_clText = RGB(255, 255, 255);
		m_uEdgeFlags = BDR_RAISEDINNER;
	}
	m_mapMenuIdToIconIdx.InitHashTable(29);
}

CTitledMenu::~CTitledMenu()
{
	DeleteIcons();
}

void CTitledMenu::DeleteIcons()
{
	m_ImageList.DeleteImageList();
	m_mapIconNameToIconIdx.RemoveAll();
	m_mapMenuIdToIconIdx.RemoveAll();

	CString strKey;
	for (POSITION pos = m_mapIconNameToBitmap.GetStartPosition(); pos != NULL;) {
		HBITMAP hBmp;
		m_mapIconNameToBitmap.GetNextAssoc(pos, strKey, hBmp);
		VERIFY(::DeleteObject(hBmp));
	}
	m_mapIconNameToBitmap.RemoveAll();
}

BOOL CTitledMenu::CreateMenu()
{
	ASSERT(m_mapIconNameToIconIdx.IsEmpty());
	ASSERT(m_mapMenuIdToIconIdx.IsEmpty());
	ASSERT(m_ImageList.m_hImageList == NULL || m_ImageList.GetImageCount() == 0);
	ASSERT(m_mapIconNameToBitmap.IsEmpty());
	return __super::CreateMenu();
}

BOOL CTitledMenu::DestroyMenu()
{
	BOOL bResult = __super::DestroyMenu();
	DeleteIcons();
	return bResult;
}

void CTitledMenu::AddMenuTitle(LPCTSTR lpszTitle, bool bIsIconMenu)
{
	// insert an empty owner-draw item at top to serve as the title
	// note: item is not selectable (disabled) but not grayed
	//
	// Vista: Adding at least one MF_OWNERDRAW item would render the entire menu in owner drawn mode,
	// and it would be quite expensive to get the native Vista menu styles back. We would need to draw
	// the entire menu with the Vista theme API -- no way. Thus, there is no title for context menus
	// under Vista - the title doesn't fit to the native Vista menu style anyway.
	if (lpszTitle != NULL && !theApp.IsVistaThemeActive()) {
		m_strTitle = lpszTitle;
		m_strTitle.Remove(_T('&'));
		CMenu::InsertMenu(0, MF_BYPOSITION | MF_OWNERDRAW | MF_STRING | MF_DISABLED, MP_TITLE);
	}
	if (bIsIconMenu)
		EnableIcons();
}

void CTitledMenu::EnableIcons()
{
	m_bIconMenu = true;
	m_ImageList.DeleteImageList();
	m_ImageList.Create(ICONSIZE, ICONSIZE, theApp.m_iDfltImageListColorFlags | ILC_MASK, 0, 1);

	MENUINFO mi;
	mi.cbSize = (DWORD)sizeof mi;
	mi.fMask = MIM_STYLE;
	GetMenuInfo(&mi);
	mi.dwStyle |= MNS_CHECKORBMP;
	SetMenuInfo(&mi);
}

// NOTE: This function is no longer used for Vista!
void CTitledMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	if (lpMIS->itemID == MP_TITLE) {
		CDC dc;
		dc.Attach(::GetDC(HWND_DESKTOP));
		HFONT hfontOld = (HFONT)::SelectObject(dc.m_hDC, (HFONT)theApp.m_fontDefaultBold);
		CSize size = dc.GetTextExtent(m_strTitle);
		::SelectObject(dc.m_hDC, hfontOld);
		size.cx += ::GetSystemMetrics(SM_CXMENUCHECK) + 8;
		::ReleaseDC(NULL, dc.Detach());

		static const int nBorderSize = 2;
		lpMIS->itemWidth = size.cx + nBorderSize;
		lpMIS->itemHeight = size.cy + nBorderSize;
	} else {
		CMenu::MeasureItem(lpMIS);
		if (m_bIconMenu) {
			lpMIS->itemHeight = max(lpMIS->itemHeight, ICONSIZE + 2);
			lpMIS->itemWidth += ICONSIZE + 4;
		}
	}
}

// NOTE: This function is no longer used for Vista!
void CTitledMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (lpDIS->itemID == MP_TITLE) {
		COLORREF crOldBk = ::SetBkColor(lpDIS->hDC, m_clLeft);

		if (!g_bLowColorDesktop &&/* m_pfnGradientFill &&*/ m_clLeft != m_clRight) {
			TRIVERTEX rcVertex[2];
			--lpDIS->rcItem.right; // exclude this point, like FillRect does
			--lpDIS->rcItem.bottom;
			rcVertex[0].x = lpDIS->rcItem.left;
			rcVertex[0].y = lpDIS->rcItem.top;
			rcVertex[0].Red = GetRValue(m_clLeft) << 8;	// color values from 0x0000 to 0xff00 !!!!
			rcVertex[0].Green = GetGValue(m_clLeft) << 8;
			rcVertex[0].Blue = GetBValue(m_clLeft) << 8;
			rcVertex[0].Alpha = 0;
			rcVertex[1].x = lpDIS->rcItem.right;
			rcVertex[1].y = lpDIS->rcItem.bottom;
			rcVertex[1].Red = GetRValue(m_clRight) << 8;
			rcVertex[1].Green = GetGValue(m_clRight) << 8;
			rcVertex[1].Blue = GetBValue(m_clRight) << 8;
			rcVertex[1].Alpha = 0;
			static const GRADIENT_RECT rect{0, 1};
			::GradientFill(lpDIS->hDC, rcVertex, 2, (PVOID)&rect, 1, GRADIENT_FILL_RECT_H);
		} else
			::ExtTextOut(lpDIS->hDC, 0, 0, ETO_OPAQUE, &lpDIS->rcItem, NULL, 0, NULL);

		if (m_bDrawEdge)
			::DrawEdge(lpDIS->hDC, &lpDIS->rcItem, m_uEdgeFlags, BF_RECT);

		int iOldBkMode = ::SetBkMode(lpDIS->hDC, TRANSPARENT);
		COLORREF crOld = ::SetTextColor(lpDIS->hDC, m_clText);
		HFONT hfontOld = (HFONT)::SelectObject(lpDIS->hDC, (HFONT)theApp.m_fontDefaultBold);
		lpDIS->rcItem.left += ::GetSystemMetrics(SM_CXMENUCHECK) + 8;
		::DrawText(lpDIS->hDC, m_strTitle, -1, &lpDIS->rcItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
		::SelectObject(lpDIS->hDC, hfontOld);
		::SetTextColor(lpDIS->hDC, crOld);
		::SetBkMode(lpDIS->hDC, iOldBkMode);
		::SetBkColor(lpDIS->hDC, crOldBk);
	} else {
		int nIconPos;
		if (m_mapMenuIdToIconIdx.Lookup(lpDIS->itemID, nIconPos)) {
			int posY = lpDIS->rcItem.top + ((lpDIS->rcItem.bottom - lpDIS->rcItem.top) - ICONSIZE) / 2;
			CDC *dc = CDC::FromHandle(lpDIS->hDC);
			HICON hIcon = (lpDIS->itemState & ODS_GRAYED) ? ReplaceIconGreyedInImageList(m_ImageList, nIconPos) : 0;
			// Draw the bitmap on the menu.
			m_ImageList.Draw(dc, nIconPos, CPoint(lpDIS->rcItem.left, posY), ILD_TRANSPARENT);
			if (hIcon) { //restore coloured icon
				m_ImageList.Replace(nIconPos, hIcon);
				::DestroyIcon(hIcon);
			}
		}
	}
}

BOOL CTitledMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, LPCTSTR lpszIconName)
{
	BOOL bResult = CMenu::AppendMenu(nFlags, nIDNewItem, lpszNewItem);
	if (bResult)
		SetMenuBitmap(nFlags, (UINT)nIDNewItem, lpszNewItem, lpszIconName);
	return bResult;
}

BOOL CTitledMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, LPCTSTR lpszIconName)
{
	BOOL bResult = CMenu::InsertMenu(nPosition, nFlags, nIDNewItem, lpszNewItem);
	if (bResult)
		SetMenuBitmap(nFlags, (UINT)nIDNewItem, lpszNewItem, lpszIconName);
	return bResult;
}

BOOL CTitledMenu::RenameMenu(UINT_PTR nIDNewItem, UINT nFlags, LPCTSTR lpszNewItem, LPCTSTR lpszIconName)
{
	MENUITEMINFO mi = {};
	mi.cbSize = (UINT)sizeof(MENUITEMINFO);
	mi.fMask = MIIM_TYPE;
	mi.fType = MFT_STRING;
	mi.dwTypeData = const_cast<LPTSTR>(lpszNewItem);
	BOOL bResult = SetMenuItemInfo((UINT)nIDNewItem, &mi, nFlags == MF_BYPOSITION);
	if (bResult)
		SetMenuBitmap(0, (UINT)nIDNewItem, lpszNewItem, lpszIconName);
	return bResult;
}

static HBITMAP Create32BitHBITMAP(HDC hdc, int cx, int cy, void **ppvBits = NULL)
{
	HBITMAP hBmp = NULL;
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hdcUsed = hdc ? hdc : GetDC(NULL);
	if (hdcUsed) {
		hBmp = ::CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
		if (hdc != hdcUsed)
			::ReleaseDC(NULL, hdcUsed);
	}
	return hBmp;
}

static HBITMAP IconToBitmap32(HICON hIcon, int cx, int cy)
{
	HBITMAP hBmp = NULL;
	HDC hdcDest = ::CreateCompatibleDC(NULL);
	if (hdcDest) {
		hBmp = ::Create32BitHBITMAP(hdcDest, cx, cy);
		if (hBmp) {
			HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcDest, hBmp);
			if (hbmpOld) {
				// "DrawIconEx" works well only for icons which do also have an XP version specified.
				// For 256 color icons the icons drawn by "DrawIconEx" are way too "bright"?
				//::DrawIconEx(hdcDest, 0, 0, hIcon, cx, cy, 0, NULL, DI_NORMAL);

				// Not as efficient as "DrawIconEx", but using an image list works for XP icons
				// as well as for 256 color icons.
				HIMAGELIST himl = ::ImageList_Create(cx, cy, theApp.m_iDfltImageListColorFlags | ILC_MASK, 1, 0);
				if (himl) {
					::ImageList_AddIcon(himl, hIcon);
					::ImageList_Draw(himl, 0, hdcDest, 0, 0, ILD_NORMAL);
					::ImageList_Destroy(himl);
				}

				::SelectObject(hdcDest, hbmpOld);
			}
		}
		::DeleteDC(hdcDest);
	}
	return hBmp;
}

// The historic 16-pixel menu resources are difficult to read on today's
// displays.  These compact, alpha-aware glyphs are deliberately limited to
// the eMule Next Tools menu so that custom icon packs keep working elsewhere.
static HICON CreateNextToolsMenuIcon(const CString &strIconName)
{
	ULONG_PTR gdiplusToken = 0;
	Gdiplus::GdiplusStartupInput startupInput;
	if (Gdiplus::GdiplusStartup(&gdiplusToken, &startupInput, NULL) != Gdiplus::Ok)
		return NULL;

	HICON hIcon = NULL;
	{
	Gdiplus::Bitmap bitmap(64, 64, PixelFormat32bppARGB);
	Gdiplus::Graphics graphics(&bitmap);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

	Gdiplus::Color color(255, 88, 205, 239); // eMule Next cyan
	if (strIconName == _T("next_convert"))
		color = Gdiplus::Color(255, 246, 190, 74);
	else if (strIconName == _T("next_wizard"))
		color = Gdiplus::Color(255, 170, 141, 245);
	else if (strIconName == _T("next_ipfilter"))
		color = Gdiplus::Color(255, 94, 208, 157);
	else if (strIconName == _T("next_link"))
		color = Gdiplus::Color(255, 245, 153, 92);
	else if (strIconName == _T("next_scheduler"))
		color = Gdiplus::Color(255, 235, 101, 101);

	Gdiplus::Pen pen(color, 7.0f);
	pen.SetLineJoin(Gdiplus::LineJoinRound);
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);
	Gdiplus::SolidBrush brush(color);
	const auto Rect = [](float left, float top, float right, float bottom) {
		return Gdiplus::RectF(left, top, right - left, bottom - top);
	};

	if (strIconName == _T("next_incoming")) {
		Gdiplus::PointF points[] = { { 8, 21 }, { 25, 21 }, { 31, 15 }, { 55, 15 }, { 55, 49 }, { 8, 49 } };
		graphics.DrawPolygon(&pen, points, _countof(points));
		graphics.DrawLine(&pen, 8, 25, 55, 25);
	} else if (strIconName == _T("next_convert")) {
		graphics.DrawLine(&pen, 12, 22, 50, 22);
		graphics.DrawLine(&pen, 43, 15, 50, 22);
		graphics.DrawLine(&pen, 50, 22, 43, 29);
		graphics.DrawLine(&pen, 52, 42, 14, 42);
		graphics.DrawLine(&pen, 21, 35, 14, 42);
		graphics.DrawLine(&pen, 14, 42, 21, 49);
	} else if (strIconName == _T("next_wizard")) {
		graphics.DrawLine(&pen, 15, 50, 43, 22);
		graphics.DrawLine(&pen, 12, 18, 12, 27);
		graphics.DrawLine(&pen, 8, 22, 17, 22);
		graphics.DrawLine(&pen, 47, 34, 47, 45);
		graphics.DrawLine(&pen, 42, 39, 52, 39);
		graphics.FillEllipse(&brush, Rect(39, 13, 46, 20));
	} else if (strIconName == _T("next_ipfilter")) {
		Gdiplus::PointF shield[] = { { 32, 8 }, { 52, 15 }, { 52, 31 }, { 32, 53 }, { 12, 31 }, { 12, 15 } };
		graphics.DrawPolygon(&pen, shield, _countof(shield));
		graphics.DrawLine(&pen, 22, 30, 29, 37);
		graphics.DrawLine(&pen, 29, 37, 43, 22);
	} else if (strIconName == _T("next_link")) {
		graphics.DrawArc(&pen, Rect(7, 25, 35, 53), 225, 180);
		graphics.DrawArc(&pen, Rect(29, 7, 57, 35), 45, 180);
		graphics.DrawLine(&pen, 24, 40, 40, 24);
	} else if (strIconName == _T("next_web")) {
		graphics.DrawEllipse(&pen, Rect(8, 8, 56, 56));
		graphics.DrawLine(&pen, 8, 32, 56, 32);
		graphics.DrawLine(&pen, 32, 8, 32, 56);
		graphics.DrawArc(&pen, Rect(18, 8, 46, 56), 90, 180);
		graphics.DrawArc(&pen, Rect(18, 8, 46, 56), 270, 180);
	} else if (strIconName == _T("next_scheduler")) {
		graphics.DrawRectangle(&pen, Rect(10, 13, 54, 53));
		graphics.DrawLine(&pen, 10, 26, 54, 26);
		graphics.DrawLine(&pen, 21, 8, 21, 19);
		graphics.DrawLine(&pen, 43, 8, 43, 19);
		graphics.FillEllipse(&brush, Rect(22, 34, 28, 40));
		graphics.FillEllipse(&brush, Rect(36, 34, 42, 40));
	}

	if (bitmap.GetHICON(&hIcon) != Gdiplus::Ok)
		hIcon = NULL;
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return hIcon;
}

static HICON LoadTitledMenuIcon(const CString &strIconName)
{
	if (strIconName.Left(5) == _T("next_"))
		return CreateNextToolsMenuIcon(strIconName);
	return theApp.LoadIcon(strIconName);
}

void CTitledMenu::SetMenuBitmap(UINT nFlags, UINT nIDNewItem, LPCTSTR /*lpszNewItem*/, LPCTSTR lpszIconName)
{
	if (!m_bIconMenu || (nFlags & MF_SEPARATOR) != 0) {
		if (m_bIconMenu && lpszIconName != NULL)
			ASSERT(0);
		return;
	}

	// Those MFC warnings which are thrown when one opens certain context menus
	// are because of sub menu items. All the IDs shown in the warnings are sub
	// menu handles! Seems to be a bug in MFC. Look at '_AfxFindPopupMenuFromID'.
	// ---
	// Warning: unknown WM_MEASUREITEM for menu item 0x530601.
	// Warning: unknown WM_MEASUREITEM for menu item 0x4305E7.
	// ---
	//if (nFlags & MF_POPUP)
	//	TRACE(_T("TitledMenu: adding popup menu item id=%x  str=%s\n"), nIDNewItem, lpszNewItem);

	CString strIconLower(lpszIconName);
	if (strIconLower.MakeLower().IsEmpty())
		return;
	if (thePrefs.GetWindowsVersion() >= _WINVER_VISTA_) {
		// Vista+: Use the Windows built-in feature for 32-bit menu item bitmaps.
		// 'MeasureItem', 'DrawItem' will not get called any longer and Vista
		// cares properly about grayed/selected menu item bitmaps.
		HBITMAP hBmp = NULL;
		if (!m_mapIconNameToBitmap.Lookup(strIconLower, hBmp)) {
			HICON hIcon = LoadTitledMenuIcon(strIconLower);
			if (hIcon) {
				hBmp = IconToBitmap32(hIcon, ICONSIZE, ICONSIZE);
				VERIFY(::DestroyIcon(hIcon));
			} else
				hBmp = NULL;
		}

		if (hBmp) {
			MENUITEMINFO info = {};
			info.cbSize = (UINT)sizeof info;
			info.fMask = MIIM_BITMAP;
			info.hbmpItem = hBmp;
			VERIFY(SetMenuItemInfo(nIDNewItem, &info, FALSE));
			m_mapIconNameToBitmap[strIconLower] = hBmp;
		}
	} else {
		// pre-Vista: Use owner drawn menu items which are handled in 'MeasureItem' and 'DrawItem'
		int nPos;
		if (m_mapIconNameToIconIdx.Lookup(strIconLower, nPos)) {
			m_mapMenuIdToIconIdx[nIDNewItem] = nPos;
		} else {
			HICON hIcon = LoadTitledMenuIcon(strIconLower);
			if (!hIcon)
				return;
			nPos = m_ImageList.Add(hIcon);
			if (nPos >= 0) {
				m_mapIconNameToIconIdx[strIconLower] = nPos;
				m_mapMenuIdToIconIdx[nIDNewItem] = nPos;
			}
			VERIFY(::DestroyIcon(hIcon));
		}
		if (nPos != -1) {
			MENUITEMINFO info = {};
			info.cbSize = (UINT)sizeof info;
			info.fMask = MIIM_BITMAP;
			info.hbmpItem = HBMMENU_CALLBACK;
			VERIFY(SetMenuItemInfo(nIDNewItem, &info, FALSE));
		}
	}
}

bool CTitledMenu::HasEnabledItems() const
{
	for (int i = GetMenuItemCount(); --i >= 0;)
		if ((GetMenuState((UINT)i, MF_BYPOSITION) & (MF_DISABLED | MF_SEPARATOR | MF_GRAYED)) == 0)
			return true;
	return false;
}

int CTitledMenu::FindSubMenuPosition(HMENU hSubMenu) const
{
	for (int position = 0; position < GetMenuItemCount(); ++position) {
		const CMenu *subMenu = GetSubMenu(position);
		if (subMenu != NULL && subMenu->GetSafeHmenu() == hSubMenu)
			return position;
	}
	return -1;
}

BOOL CTitledMenu::EnableSubMenu(HMENU hSubMenu, UINT nEnable)
{
	const int position = FindSubMenuPosition(hSubMenu);
	ASSERT(position >= 0);
	return position >= 0 && EnableMenuItem(static_cast<UINT>(position), nEnable | MF_BYPOSITION);
}

BOOL CTitledMenu::RemoveSubMenu(HMENU hSubMenu)
{
	const int position = FindSubMenuPosition(hSubMenu);
	ASSERT(position >= 0);
	return position >= 0 && RemoveMenu(static_cast<UINT>(position), MF_BYPOSITION);
}
