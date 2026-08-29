// This file is part of eMule Next.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stdafx.h"
#include "NextResizable/NextResizable.h"
#include "NextTheme.h"

#include <algorithm>

namespace
{
	const TCHAR kWindowPlacementKey[] = _T("EmuleNext.WindowPlacement");
	const TCHAR kSelectedPageKey[] = _T("EmuleNext.SelectedPage");

	COLORREF ModernWindowColor(EEmuleNextTheme theme)
	{
		return theme == entModernAurora ? RGB(239, 249, 248) : RGB(246, 248, 251);
	}

	COLORREF ModernTextColor(EEmuleNextTheme theme)
	{
		return theme == entModernAurora ? RGB(27, 63, 65) : RGB(35, 49, 66);
	}

	HBRUSH ModernDialogBrush(EEmuleNextTheme theme)
	{
		static CBrush lightBrush(RGB(246, 248, 251));
		static CBrush auroraBrush(RGB(239, 249, 248));
		return static_cast<HBRUSH>((theme == entModernAurora ? auroraBrush : lightBrush).GetSafeHandle());
	}

	HBRUSH ApplyModernControlColors(CDC* dc, UINT ctlColor)
	{
		const EEmuleNextTheme theme = CNextTheme::GetTheme();
		if (theme == entClassic || (ctlColor != CTLCOLOR_DLG && ctlColor != CTLCOLOR_STATIC))
			return NULL;

		dc->SetTextColor(ModernTextColor(theme));
		dc->SetBkColor(ModernWindowColor(theme));
		if (ctlColor == CTLCOLOR_STATIC)
			dc->SetBkMode(TRANSPARENT);
		return ModernDialogBrush(theme);
	}

	int AnchorOffset(int length, int percentage)
	{
		return MulDiv(length, percentage, 100);
	}

	bool ChildCanClipBackground(HWND child)
	{
		TCHAR className[64] = {};
		if (::GetClassName(child, className, _countof(className)) == 0)
			return true;

		const DWORD style = static_cast<DWORD>(::GetWindowLongPtr(child, GWL_STYLE));
		if (_tcsicmp(className, TOOLBARCLASSNAME) == 0 && (style & TBSTYLE_TRANSPARENT) != 0)
			return false;
		if (_tcsicmp(className, WC_BUTTON) == 0) {
			const DWORD buttonType = style & BS_TYPEMASK;
			if (buttonType == BS_GROUPBOX)
				return false;
			if (buttonType == BS_OWNERDRAW) {
				CRect windowRect;
				::GetWindowRect(child, &windowRect);
				if (::SendMessage(child, WM_NCHITTEST, 0, MAKELPARAM(windowRect.left, windowRect.top)) == HTTRANSPARENT)
					return false;
			}
			return true;
		}
		if (_tcsicmp(className, WC_STATIC) == 0) {
			const DWORD staticType = style & SS_TYPEMASK;
			if ((staticType == SS_ICON || staticType == SS_ENHMETAFILE) && (style & SS_CENTERIMAGE) != 0)
				return false;
			return staticType == SS_LEFT || staticType == SS_CENTER || staticType == SS_RIGHT
				|| staticType == SS_SIMPLE || staticType == SS_LEFTNOWORDWRAP
				|| staticType == SS_BLACKRECT || staticType == SS_GRAYRECT || staticType == SS_WHITERECT
				|| staticType == SS_ETCHEDHORZ || staticType == SS_ETCHEDVERT
				|| staticType == SS_BITMAP || staticType == SS_ICON || staticType == SS_ENHMETAFILE;
		}
		return true;
	}
}

void CResizableLayout::AddAnchor(HWND hWnd, CSize topLeftType, CSize bottomRightType)
{
	CWnd* const parent = GetResizableWnd();
	if (parent == NULL || !::IsWindow(parent->GetSafeHwnd()) || !::IsWindow(hWnd))
		return;

	if (bottomRightType == NOANCHOR)
		bottomRightType = topLeftType;

	RemoveAnchor(hWnd);

	CRect parentRect;
	GetTotalClientRect(&parentRect);
	CRect childRect;
	::GetWindowRect(hWnd, &childRect);
	parent->ScreenToClient(&childRect);

	LayoutInfo layout;
	layout.hWnd = hWnd;
	layout.topLeftType = topLeftType;
	layout.bottomRightType = bottomRightType;
	layout.topLeftMargin = CSize(
		childRect.left - AnchorOffset(parentRect.Width(), topLeftType.cx),
		childRect.top - AnchorOffset(parentRect.Height(), topLeftType.cy));
	layout.bottomRightMargin = CSize(
		childRect.right - AnchorOffset(parentRect.Width(), bottomRightType.cx),
		childRect.bottom - AnchorOffset(parentRect.Height(), bottomRightType.cy));
	m_layouts.push_back(layout);
}

void CResizableLayout::AddAnchor(UINT controlId, CSize topLeftType, CSize bottomRightType)
{
	CWnd* const parent = GetResizableWnd();
	if (parent != NULL)
		AddAnchor(::GetDlgItem(parent->GetSafeHwnd(), controlId), topLeftType, bottomRightType);
}

void CResizableLayout::AddAllOtherAnchors(CSize anchor)
{
	CWnd* const parent = GetResizableWnd();
	if (parent == NULL || !::IsWindow(parent->GetSafeHwnd()))
		return;

	for (HWND child = ::GetWindow(parent->GetSafeHwnd(), GW_CHILD); child != NULL; child = ::GetWindow(child, GW_HWNDNEXT)) {
		if (FindAnchor(child) == NULL)
			AddAnchor(child, anchor);
	}
}

BOOL CResizableLayout::RemoveAnchor(HWND hWnd)
{
	const std::vector<LayoutInfo>::iterator found = std::find_if(m_layouts.begin(), m_layouts.end(),
		[hWnd](const LayoutInfo& layout) { return layout.hWnd == hWnd; });
	if (found == m_layouts.end())
		return FALSE;

	m_layouts.erase(found);
	return TRUE;
}

BOOL CResizableLayout::RemoveAnchor(UINT controlId)
{
	CWnd* const parent = GetResizableWnd();
	return parent != NULL ? RemoveAnchor(::GetDlgItem(parent->GetSafeHwnd(), controlId)) : FALSE;
}

void CResizableLayout::RemoveAllAnchors()
{
	m_layouts.clear();
}

BOOL CResizableLayout::GetAnchorPosition(HWND hWnd, const CRect& parentRect, CRect& childRect, UINT* flags) const
{
	const LayoutInfo* const layout = FindAnchor(hWnd);
	if (layout == NULL)
		return FALSE;

	if (flags != NULL)
		*flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
	return CalculateAnchorPosition(*layout, parentRect, childRect);
}

BOOL CResizableLayout::GetAnchorPosition(UINT controlId, const CRect& parentRect, CRect& childRect, UINT* flags) const
{
	CWnd* const parent = const_cast<CResizableLayout*>(this)->GetResizableWnd();
	return parent != NULL ? GetAnchorPosition(::GetDlgItem(parent->GetSafeHwnd(), controlId), parentRect, childRect, flags) : FALSE;
}

void CResizableLayout::ArrangeLayout()
{
	CWnd* const parent = GetResizableWnd();
	if (m_isArranging || parent == NULL || !::IsWindow(parent->GetSafeHwnd()))
		return;

	m_isArranging = true;
	CRect parentRect;
	GetTotalClientRect(&parentRect);
	struct PendingPosition
	{
		HWND window;
		CRect rect;
		UINT flags;
	};
	std::vector<PendingPosition> pendingPositions;
	pendingPositions.reserve(m_layouts.size());
	for (const LayoutInfo& layout : m_layouts) {
		if (!::IsWindow(layout.hWnd))
			continue;

		CRect target;
		if (CalculateAnchorPosition(layout, parentRect, target)) {
			CRect current;
			::GetWindowRect(layout.hWnd, &current);
			parent->ScreenToClient(&current);
			UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOCOPYBITS;
			if (current.TopLeft() == target.TopLeft())
				flags |= SWP_NOMOVE;
			if (current.Size() == target.Size())
				flags |= SWP_NOSIZE;
			if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
				continue;

			pendingPositions.push_back({ layout.hWnd, target, flags });
		}
	}

	bool arrangedAsBatch = false;
	if (!pendingPositions.empty()) {
		HDWP deferred = ::BeginDeferWindowPos(static_cast<int>(pendingPositions.size()));
		for (const PendingPosition& pending : pendingPositions) {
			if (deferred == NULL)
				break;
			deferred = ::DeferWindowPos(deferred, pending.window, NULL, pending.rect.left, pending.rect.top,
				pending.rect.Width(), pending.rect.Height(), pending.flags);
		}
		arrangedAsBatch = deferred != NULL && ::EndDeferWindowPos(deferred) != FALSE;
		if (!arrangedAsBatch) {
			for (const PendingPosition& pending : pendingPositions) {
				::SetWindowPos(pending.window, NULL, pending.rect.left, pending.rect.top,
					pending.rect.Width(), pending.rect.Height(), pending.flags);
			}
		}
	}
	if (!pendingPositions.empty())
		parent->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	m_isArranging = false;
}

void CResizableLayout::EraseBackground(CDC* dc)
{
	CWnd* const parent = GetResizableWnd();
	if (dc == NULL || parent == NULL || !::IsWindow(parent->GetSafeHwnd()))
		return;

	const int savedDc = dc->SaveDC();
	for (HWND child = ::GetWindow(parent->GetSafeHwnd(), GW_CHILD); child != NULL;
		child = ::GetWindow(child, GW_HWNDNEXT))
	{
		if (!::IsWindowVisible(child) || !ChildCanClipBackground(child))
			continue;
		CRect childRect;
		::GetWindowRect(child, &childRect);
		parent->ScreenToClient(&childRect);
		dc->ExcludeClipRect(&childRect);
	}

	CRect clientRect;
	parent->GetClientRect(&clientRect);
	HBRUSH background = reinterpret_cast<HBRUSH>(parent->SendMessage(WM_CTLCOLORDLG,
		reinterpret_cast<WPARAM>(dc->GetSafeHdc()), reinterpret_cast<LPARAM>(parent->GetSafeHwnd())));
	if (background == NULL)
		background = reinterpret_cast<HBRUSH>(::GetClassLongPtr(parent->GetSafeHwnd(), GCLP_HBRBACKGROUND));
	if (background != NULL)
		::FillRect(dc->GetSafeHdc(), &clientRect, background);
	if (savedDc != 0)
		dc->RestoreDC(savedDc);
}

BOOL CResizableLayout::ArrangeLayoutCallback(LayoutInfo&)
{
	return FALSE;
}

void CResizableLayout::InitResizeProperties(LayoutInfo&)
{
}

BOOL CResizableLayout::LikesClipping(const LayoutInfo&)
{
	return FALSE;
}

BOOL CResizableLayout::NeedsRefresh(const LayoutInfo&, const CRect&, const CRect&)
{
	return FALSE;
}

void CResizableLayout::GetTotalClientRect(LPRECT rect)
{
	if (rect == NULL)
		return;

	CWnd* const parent = GetResizableWnd();
	if (parent != NULL && ::IsWindow(parent->GetSafeHwnd()))
		parent->GetClientRect(rect);
	else
		::SetRectEmpty(rect);
}

CResizableLayout::LayoutInfo* CResizableLayout::FindAnchor(HWND hWnd)
{
	const std::vector<LayoutInfo>::iterator found = std::find_if(m_layouts.begin(), m_layouts.end(),
		[hWnd](const LayoutInfo& layout) { return layout.hWnd == hWnd; });
	return found != m_layouts.end() ? &(*found) : NULL;
}

const CResizableLayout::LayoutInfo* CResizableLayout::FindAnchor(HWND hWnd) const
{
	const std::vector<LayoutInfo>::const_iterator found = std::find_if(m_layouts.begin(), m_layouts.end(),
		[hWnd](const LayoutInfo& layout) { return layout.hWnd == hWnd; });
	return found != m_layouts.end() ? &(*found) : NULL;
}

BOOL CResizableLayout::CalculateAnchorPosition(const LayoutInfo& layout, const CRect& parentRect, CRect& childRect) const
{
	childRect.left = AnchorOffset(parentRect.Width(), layout.topLeftType.cx) + layout.topLeftMargin.cx;
	childRect.top = AnchorOffset(parentRect.Height(), layout.topLeftType.cy) + layout.topLeftMargin.cy;
	childRect.right = AnchorOffset(parentRect.Width(), layout.bottomRightType.cx) + layout.bottomRightMargin.cx;
	childRect.bottom = AnchorOffset(parentRect.Height(), layout.bottomRightType.cy) + layout.bottomRightMargin.cy;
	return childRect.Width() >= 0 && childRect.Height() >= 0;
}

void CResizableMinMax::MinMaxInfo(LPMINMAXINFO minMaxInfo) const
{
	if (minMaxInfo == NULL)
		return;
	if (m_hasMinTrackSize) {
		minMaxInfo->ptMinTrackSize.x = m_minTrackSize.cx;
		minMaxInfo->ptMinTrackSize.y = m_minTrackSize.cy;
	}
	if (m_hasMaxTrackSize) {
		minMaxInfo->ptMaxTrackSize.x = m_maxTrackSize.cx;
		minMaxInfo->ptMaxTrackSize.y = m_maxTrackSize.cy;
	}
	if (m_hasMaximizedRect) {
		minMaxInfo->ptMaxPosition.x = m_maximizedRect.left;
		minMaxInfo->ptMaxPosition.y = m_maximizedRect.top;
		minMaxInfo->ptMaxSize.x = m_maximizedRect.Width();
		minMaxInfo->ptMaxSize.y = m_maximizedRect.Height();
	}
}

void CResizableMinMax::SetMaximizedRect(const CRect& rect)
{
	m_maximizedRect = rect;
	m_hasMaximizedRect = true;
}

void CResizableMinMax::ResetMaximizedRect()
{
	m_hasMaximizedRect = false;
}

void CResizableMinMax::SetMinTrackSize(const CSize& size)
{
	m_minTrackSize = size;
	m_hasMinTrackSize = true;
}

void CResizableMinMax::ResetMinTrackSize()
{
	m_hasMinTrackSize = false;
}

void CResizableMinMax::SetMaxTrackSize(const CSize& size)
{
	m_maxTrackSize = size;
	m_hasMaxTrackSize = true;
}

void CResizableMinMax::ResetMaxTrackSize()
{
	m_hasMaxTrackSize = false;
}

BOOL CResizableState::LoadWindowRect(LPCTSTR section, BOOL rectOnly)
{
	CWinApp* const application = AfxGetApp();
	CWnd* const window = GetResizableWnd();
	if (application == NULL || window == NULL || !::IsWindow(window->GetSafeHwnd()) || section == NULL)
		return FALSE;

	const CString stored = application->GetProfileString(section, kWindowPlacementKey, _T(""));
	int left = 0;
	int top = 0;
	int right = 0;
	int bottom = 0;
	int showCommand = SW_SHOWNORMAL;
	if (swscanf_s(stored, _T("%d,%d,%d,%d,%d"), &left, &top, &right, &bottom, &showCommand) != 5 || right <= left || bottom <= top)
		return FALSE;

	if (rectOnly) {
		return ::SetWindowPos(window->GetSafeHwnd(), NULL, left, top, right - left, bottom - top,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER) != FALSE;
	}

	WINDOWPLACEMENT placement = {};
	placement.length = sizeof(placement);
	placement.showCmd = showCommand;
	placement.rcNormalPosition = CRect(left, top, right, bottom);
	return window->SetWindowPlacement(&placement);
}

BOOL CResizableState::SaveWindowRect(LPCTSTR section, BOOL) const
{
	CWinApp* const application = AfxGetApp();
	CWnd* const window = const_cast<CResizableState*>(this)->GetResizableWnd();
	if (application == NULL || window == NULL || !::IsWindow(window->GetSafeHwnd()) || section == NULL)
		return FALSE;

	WINDOWPLACEMENT placement = {};
	placement.length = sizeof(placement);
	if (!window->GetWindowPlacement(&placement))
		return FALSE;

	CString stored;
	stored.Format(_T("%ld,%ld,%ld,%ld,%u"), placement.rcNormalPosition.left, placement.rcNormalPosition.top,
		placement.rcNormalPosition.right, placement.rcNormalPosition.bottom, placement.showCmd);
	return application->WriteProfileString(section, kWindowPlacementKey, stored);
}

BEGIN_MESSAGE_MAP(CResizableDialog, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CResizableDialog::CResizableDialog()
{
}

CResizableDialog::CResizableDialog(UINT templateId, CWnd* parent)
	: CDialog(templateId, parent)
{
}

CResizableDialog::CResizableDialog(LPCTSTR templateName, CWnd* parent)
	: CDialog(templateName, parent)
{
}

CResizableDialog::~CResizableDialog()
{
	RemoveAllAnchors();
}

void CResizableDialog::EnableSaveRestore(LPCTSTR section, BOOL rectOnly)
{
	m_saveRestoreEnabled = section != NULL;
	m_rectOnly = rectOnly != FALSE;
	m_stateSection = section != NULL ? section : _T("");
	if (m_saveRestoreEnabled && LoadWindowRect(m_stateSection, m_rectOnly))
		ArrangeLayout();
}

CWnd* CResizableDialog::GetResizableWnd()
{
	return this;
}

void CResizableDialog::OnGetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
	MinMaxInfo(minMaxInfo);
	CDialog::OnGetMinMaxInfo(minMaxInfo);
}

void CResizableDialog::OnSize(UINT type, int width, int height)
{
	CDialog::OnSize(type, width, height);
	ArrangeLayout();
}

BOOL CResizableDialog::OnEraseBkgnd(CDC* dc)
{
	EraseBackground(dc);
	return TRUE;
}

HBRUSH CResizableDialog::OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor)
{
	HBRUSH brush = ApplyModernControlColors(dc, ctlColor);
	return brush != NULL ? brush : CDialog::OnCtlColor(dc, wnd, ctlColor);
}

void CResizableDialog::OnDestroy()
{
	if (m_saveRestoreEnabled)
		SaveWindowRect(m_stateSection, m_rectOnly);
	CDialog::OnDestroy();
}

int CResizableDialog::OnCreate(LPCREATESTRUCT createStruct)
{
	return CDialog::OnCreate(createStruct);
}

IMPLEMENT_DYNCREATE(CResizablePage, CPropertyPage)

BEGIN_MESSAGE_MAP(CResizablePage, CPropertyPage)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CResizablePage::CResizablePage()
{
}

CResizablePage::CResizablePage(UINT templateId, UINT captionId)
	: CPropertyPage(templateId, captionId)
{
}

CResizablePage::CResizablePage(LPCTSTR templateName, UINT captionId)
	: CPropertyPage(templateName, captionId)
{
}

CResizablePage::~CResizablePage()
{
	RemoveAllAnchors();
}

CWnd* CResizablePage::GetResizableWnd()
{
	return this;
}

void CResizablePage::OnSize(UINT type, int width, int height)
{
	CPropertyPage::OnSize(type, width, height);
	ArrangeLayout();
}

BOOL CResizablePage::OnEraseBkgnd(CDC* dc)
{
	EraseBackground(dc);
	return TRUE;
}

HBRUSH CResizablePage::OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor)
{
	HBRUSH brush = ApplyModernControlColors(dc, ctlColor);
	return brush != NULL ? brush : CPropertyPage::OnCtlColor(dc, wnd, ctlColor);
}

IMPLEMENT_DYNAMIC(CResizableSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CResizableSheet, CPropertySheet)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CResizableSheet::CResizableSheet()
	: CPropertySheet(_T(""))
{
}

CResizableSheet::CResizableSheet(UINT captionId, CWnd* parent, UINT selectedPage)
	: CPropertySheet(captionId, parent, selectedPage)
{
}

CResizableSheet::CResizableSheet(LPCTSTR caption, CWnd* parent, UINT selectedPage)
	: CPropertySheet(caption, parent, selectedPage)
{
}

CResizableSheet::~CResizableSheet()
{
	RemoveAllAnchors();
}

BOOL CResizableSheet::OnInitDialog()
{
	const BOOL initialized = CPropertySheet::OnInitDialog();
	AddStandardAnchors();
	if (m_saveRestoreEnabled)
		LoadWindowRect(m_stateSection, m_rectOnly);
	RestoreSelectedPage();
	RefreshLayout();
	return initialized;
}

void CResizableSheet::RefreshLayout()
{
	CTabCtrl* const tab = GetTabControl();
	if (tab == NULL || !::IsWindow(tab->GetSafeHwnd()))
		return;

	CRect pageRect;
	tab->GetClientRect(&pageRect);
	tab->AdjustRect(FALSE, &pageRect);
	tab->ClientToScreen(&pageRect);
	ScreenToClient(&pageRect);

	for (int index = 0; index < GetPageCount(); ++index) {
		CPropertyPage* const page = GetPage(index);
		if (page != NULL && ::IsWindow(page->GetSafeHwnd())) {
			page->SetWindowPos(NULL, pageRect.left, pageRect.top, pageRect.Width(), pageRect.Height(),
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
		}
	}
}

void CResizableSheet::EnableSaveRestore(LPCTSTR section, BOOL rectOnly, BOOL savePage)
{
	m_saveRestoreEnabled = section != NULL;
	m_rectOnly = rectOnly != FALSE;
	m_savePage = savePage != FALSE;
	m_stateSection = section != NULL ? section : _T("");
	if (m_saveRestoreEnabled && ::IsWindow(GetSafeHwnd()) && LoadWindowRect(m_stateSection, m_rectOnly))
		RefreshLayout();
}

int CResizableSheet::GetMinWidth() const
{
	CRect rect;
	GetWindowRect(&rect);
	return rect.Width();
}

CWnd* CResizableSheet::GetResizableWnd()
{
	return this;
}

BOOL CResizableSheet::ArrangeLayoutCallback(LayoutInfo&)
{
	return FALSE;
}

void CResizableSheet::OnGetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
	MinMaxInfo(minMaxInfo);
	CPropertySheet::OnGetMinMaxInfo(minMaxInfo);
}

void CResizableSheet::OnSize(UINT type, int width, int height)
{
	CPropertySheet::OnSize(type, width, height);
	ArrangeLayout();
	RefreshLayout();
}

BOOL CResizableSheet::OnEraseBkgnd(CDC* dc)
{
	EraseBackground(dc);
	return TRUE;
}

HBRUSH CResizableSheet::OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor)
{
	HBRUSH brush = ApplyModernControlColors(dc, ctlColor);
	return brush != NULL ? brush : CPropertySheet::OnCtlColor(dc, wnd, ctlColor);
}

void CResizableSheet::OnDestroy()
{
	if (m_saveRestoreEnabled) {
		SaveWindowRect(m_stateSection, m_rectOnly);
		SaveSelectedPage();
	}
	CPropertySheet::OnDestroy();
}

int CResizableSheet::OnCreate(LPCREATESTRUCT createStruct)
{
	return CPropertySheet::OnCreate(createStruct);
}

void CResizableSheet::AddStandardAnchors()
{
	if (CTabCtrl* const tab = GetTabControl())
		AddAnchor(tab->GetSafeHwnd(), TOP_LEFT, BOTTOM_RIGHT);

	static const UINT buttons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP, ID_WIZBACK, ID_WIZNEXT, ID_WIZFINISH };
	for (const UINT buttonId : buttons) {
		if (CWnd* const button = GetDlgItem(buttonId))
			AddAnchor(button->GetSafeHwnd(), BOTTOM_RIGHT);
	}
}

void CResizableSheet::RestoreSelectedPage()
{
	if (!m_saveRestoreEnabled || !m_savePage)
		return;

	CWinApp* const application = AfxGetApp();
	if (application == NULL)
		return;

	const int pageIndex = application->GetProfileInt(m_stateSection, kSelectedPageKey, -1);
	if (pageIndex >= 0 && pageIndex < GetPageCount())
		SetActivePage(pageIndex);
}

void CResizableSheet::SaveSelectedPage() const
{
	if (!m_savePage)
		return;

	CWinApp* const application = AfxGetApp();
	if (application != NULL)
		application->WriteProfileInt(m_stateSection, kSelectedPageKey, GetActiveIndex());
}

IMPLEMENT_DYNAMIC(CResizableFormView, CFormView)

BEGIN_MESSAGE_MAP(CResizableFormView, CFormView)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CResizableFormView::CResizableFormView(UINT templateId)
	: CFormView(templateId)
{
}

CResizableFormView::CResizableFormView(LPCTSTR templateName)
	: CFormView(templateName)
{
}

CResizableFormView::~CResizableFormView()
{
	RemoveAllAnchors();
}

CWnd* CResizableFormView::GetResizableWnd()
{
	return this;
}

void CResizableFormView::GetTotalClientRect(LPRECT rect)
{
	if (rect == NULL)
		return;

	GetClientRect(rect);
	const CSize formSize = GetTotalSize();
	if (!m_initialUpdateComplete) {
		rect->right = rect->left + formSize.cx;
		rect->bottom = rect->top + formSize.cy;
		return;
	}

	// A CFormView can initially be created much smaller than its dialog
	// template. Keep anchors relative to the full form while scroll bars are
	// active, otherwise bottom-aligned controls can be placed outside the view.
	if (m_nMapMode >= 0) {
		if (rect->right - rect->left < formSize.cx)
			rect->right = rect->left + formSize.cx;
		if (rect->bottom - rect->top < formSize.cy)
			rect->bottom = rect->top + formSize.cy;
		const CPoint scrollPosition = GetScrollPosition();
		::OffsetRect(rect, -scrollPosition.x, -scrollPosition.y);
	}
}

void CResizableFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	m_initialUpdateComplete = true;
	ArrangeLayout();
}

void CResizableFormView::OnGetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
	MinMaxInfo(minMaxInfo);
	CFormView::OnGetMinMaxInfo(minMaxInfo);
}

void CResizableFormView::OnSize(UINT type, int width, int height)
{
	CFormView::OnSize(type, width, height);
	ArrangeLayout();
}

BOOL CResizableFormView::OnEraseBkgnd(CDC* dc)
{
	EraseBackground(dc);
	return TRUE;
}

HBRUSH CResizableFormView::OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor)
{
	HBRUSH brush = ApplyModernControlColors(dc, ctlColor);
	return brush != NULL ? brush : CFormView::OnCtlColor(dc, wnd, ctlColor);
}

void CResizableFormView::OnDestroy()
{
	RemoveAllAnchors();
	CFormView::OnDestroy();
}
