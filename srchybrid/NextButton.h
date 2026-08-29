// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
// See ../LICENSE and ../CHANGES.md.
#pragma once

// Flat, accessible action button used by the eMule Next interface.  It keeps
// the native keyboard and focus behaviour of CButton while replacing the
// legacy 3D bevel with a clear primary/secondary visual hierarchy.
class CNextButton : public CButton
{
	DECLARE_DYNAMIC(CNextButton)

public:
	CNextButton();

	void SetPrimary(bool bPrimary = true);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	bool m_bHot;
	bool m_bTrackingMouse;
	bool m_bPrimary;

	void UpdateHotState(bool bHot);

	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
};
