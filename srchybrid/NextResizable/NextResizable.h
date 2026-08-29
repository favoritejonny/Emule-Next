// This file is part of eMule Next.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Independent, minimal MFC anchor-layout implementation.  It intentionally
// exposes the legacy class names used by the client while containing only
// eMule Next code, so the former Artistic-1.0 ResizableLib can be retired.

#pragma once

#include <vector>

const CSize NOANCHOR(-1, -1);
const CSize TOP_LEFT(0, 0);
const CSize TOP_CENTER(50, 0);
const CSize TOP_RIGHT(100, 0);
const CSize MIDDLE_LEFT(0, 50);
const CSize MIDDLE_CENTER(50, 50);
const CSize MIDDLE_RIGHT(100, 50);
const CSize BOTTOM_LEFT(0, 100);
const CSize BOTTOM_CENTER(50, 100);
const CSize BOTTOM_RIGHT(100, 100);

#ifndef ANCHOR
#define ANCHOR(x, y) CSize((x), (y))
#endif

class CResizableLayout
{
protected:
	struct LayoutInfo
	{
		HWND hWnd = NULL;
		CSize topLeftType = NOANCHOR;
		CSize topLeftMargin = CSize(0, 0);
		CSize bottomRightType = NOANCHOR;
		CSize bottomRightMargin = CSize(0, 0);
	};

	void AddAnchor(HWND hWnd, CSize topLeftType, CSize bottomRightType = NOANCHOR);
	void AddAnchor(UINT controlId, CSize topLeftType, CSize bottomRightType = NOANCHOR);
	void AddAllOtherAnchors(CSize anchor = TOP_LEFT);
	BOOL RemoveAnchor(HWND hWnd);
	BOOL RemoveAnchor(UINT controlId);
	void RemoveAllAnchors();
	void ArrangeLayout();
	void EraseBackground(CDC* dc);
	BOOL GetAnchorPosition(HWND hWnd, const CRect& parentRect, CRect& childRect, UINT* flags = NULL) const;
	BOOL GetAnchorPosition(UINT controlId, const CRect& parentRect, CRect& childRect, UINT* flags = NULL) const;

	virtual void InitResizeProperties(LayoutInfo& layout);
	virtual BOOL LikesClipping(const LayoutInfo& layout);
	virtual BOOL NeedsRefresh(const LayoutInfo& layout, const CRect& oldRect, const CRect& newRect);
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);
	virtual void GetTotalClientRect(LPRECT rect);
	virtual CWnd* GetResizableWnd() = 0;

private:
	LayoutInfo* FindAnchor(HWND hWnd);
	const LayoutInfo* FindAnchor(HWND hWnd) const;
	BOOL CalculateAnchorPosition(const LayoutInfo& layout, const CRect& parentRect, CRect& childRect) const;

	std::vector<LayoutInfo> m_layouts;
	bool m_isArranging = false;
};

class CResizableMinMax
{
protected:
	void MinMaxInfo(LPMINMAXINFO minMaxInfo) const;
	void SetMaximizedRect(const CRect& rect);
	void ResetMaximizedRect();
	void SetMinTrackSize(const CSize& size);
	void ResetMinTrackSize();
	void SetMaxTrackSize(const CSize& size);
	void ResetMaxTrackSize();

private:
	bool m_hasMinTrackSize = false;
	bool m_hasMaxTrackSize = false;
	bool m_hasMaximizedRect = false;
	CSize m_minTrackSize = CSize(0, 0);
	CSize m_maxTrackSize = CSize(0, 0);
	CRect m_maximizedRect = CRect(0, 0, 0, 0);
};

class CResizableState
{
protected:
	virtual ~CResizableState() = default;
	BOOL LoadWindowRect(LPCTSTR section, BOOL rectOnly);
	BOOL SaveWindowRect(LPCTSTR section, BOOL rectOnly) const;
	virtual CWnd* GetResizableWnd() = 0;
};

class CResizableDialog : public CDialog, public CResizableLayout, public CResizableMinMax, public CResizableState
{
public:
	CResizableDialog();
	explicit CResizableDialog(UINT templateId, CWnd* parent = NULL);
	explicit CResizableDialog(LPCTSTR templateName, CWnd* parent = NULL);
	virtual ~CResizableDialog();

protected:
	void EnableSaveRestore(LPCTSTR section, BOOL rectOnly = FALSE);
	virtual CWnd* GetResizableWnd();

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* minMaxInfo);
	afx_msg void OnSize(UINT type, int width, int height);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	DECLARE_MESSAGE_MAP()

private:
	bool m_saveRestoreEnabled = false;
	bool m_rectOnly = false;
	CString m_stateSection;
};

class CResizablePage : public CPropertyPage, public CResizableLayout
{
	DECLARE_DYNCREATE(CResizablePage)

public:
	CResizablePage();
	explicit CResizablePage(UINT templateId, UINT captionId = 0);
	explicit CResizablePage(LPCTSTR templateName, UINT captionId = 0);
	virtual ~CResizablePage();

protected:
	virtual CWnd* GetResizableWnd();
	afx_msg void OnSize(UINT type, int width, int height);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor);
	DECLARE_MESSAGE_MAP()
};

class CResizableSheet : public CPropertySheet, public CResizableLayout, public CResizableMinMax, public CResizableState
{
	DECLARE_DYNAMIC(CResizableSheet)

public:
	CResizableSheet();
	explicit CResizableSheet(UINT captionId, CWnd* parent = NULL, UINT selectedPage = 0);
	explicit CResizableSheet(LPCTSTR caption, CWnd* parent = NULL, UINT selectedPage = 0);
	virtual ~CResizableSheet();

	virtual BOOL OnInitDialog();
	void RefreshLayout();

protected:
	void EnableSaveRestore(LPCTSTR section, BOOL rectOnly = FALSE, BOOL savePage = FALSE);
	int GetMinWidth() const;
	virtual CWnd* GetResizableWnd();
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* minMaxInfo);
	afx_msg void OnSize(UINT type, int width, int height);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	DECLARE_MESSAGE_MAP()

private:
	void AddStandardAnchors();
	void RestoreSelectedPage();
	void SaveSelectedPage() const;

	bool m_saveRestoreEnabled = false;
	bool m_rectOnly = false;
	bool m_savePage = false;
	CString m_stateSection;
};

class CResizableFormView : public CFormView, public CResizableLayout, public CResizableMinMax
{
	DECLARE_DYNAMIC(CResizableFormView)

protected:
	explicit CResizableFormView(UINT templateId);
	explicit CResizableFormView(LPCTSTR templateName);
	virtual ~CResizableFormView();

	virtual CWnd* GetResizableWnd();
	virtual void GetTotalClientRect(LPRECT rect) override;
	virtual void OnInitialUpdate();

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* minMaxInfo);
	afx_msg void OnSize(UINT type, int width, int height);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlColor);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	bool m_initialUpdateComplete = false;
};
