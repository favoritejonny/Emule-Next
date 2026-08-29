//this file is part of eMule
//Copyright (C)2002-2026 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.

#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "NextButton.h"
#include "NextTheme.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct NextButtonColors
{
	COLORREF back;
	COLORREF border;
	COLORREF text;
};

static NextButtonColors GetNextButtonColors(bool bPrimary, bool bDisabled, bool bPressed, bool bHot)
{
	const EEmuleNextTheme eTheme = CNextTheme::GetTheme();
	if (eTheme == entClassic)
		return {
			::GetSysColor(bPressed ? COLOR_3DSHADOW : COLOR_BTNFACE),
			::GetSysColor(COLOR_BTNSHADOW),
			::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT)
		};
	if (bDisabled)
		return eTheme == entModernAurora
			? NextButtonColors{ RGB(230, 226, 248), RGB(187, 191, 232), RGB(132, 132, 170) }
			: NextButtonColors{ ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_GRAYTEXT) };
	if (eTheme == entModernAurora) {
		if (bPrimary)
			return {
				bPressed ? RGB(77, 61, 170) : (bHot ? RGB(126, 105, 224) : RGB(102, 82, 203)),
				bPressed ? RGB(61, 48, 143) : RGB(81, 65, 177),
				RGB(255, 255, 255)
			};
		return {
			bPressed ? RGB(197, 226, 250) : (bHot ? RGB(211, 239, 247) : RGB(239, 235, 255)),
			bPressed ? RGB(86, 169, 195) : RGB(151, 126, 218),
			RGB(66, 55, 139)
		};
	}
	if (bPrimary)
		return {
			bPressed ? RGB(15, 96, 134) : (bHot ? RGB(34, 151, 196) : RGB(21, 126, 171)),
			bPressed ? RGB(8, 68, 99) : RGB(10, 103, 145),
			RGB(255, 255, 255)
		};
	return {
		bPressed ? RGB(211, 235, 244) : (bHot ? RGB(228, 244, 249) : RGB(244, 249, 251)),
		bPressed ? RGB(77, 165, 199) : RGB(126, 187, 209),
		RGB(27, 64, 80)
	};
}

static D2D1_COLOR_F ToD2DColor(COLORREF color)
{
	return D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f);
}

static bool DrawNextButtonWithDirect2D(CDC *pDC, const CRect &rcButton, const CString &strText, const NextButtonColors &colors, bool bPressed)
{
	ID2D1Factory *pD2DFactory = NULL;
	ID2D1DCRenderTarget *pRenderTarget = NULL;
	IDWriteFactory *pWriteFactory = NULL;
	IDWriteTextFormat *pTextFormat = NULL;
	ID2D1SolidColorBrush *pBackgroundBrush = NULL;
	ID2D1SolidColorBrush *pBorderBrush = NULL;
	ID2D1SolidColorBrush *pTextBrush = NULL;

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	if (SUCCEEDED(hr)) {
		D2D1_RENDER_TARGET_PROPERTIES targetProperties = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
		hr = pD2DFactory->CreateDCRenderTarget(&targetProperties, &pRenderTarget);
	}
	if (SUCCEEDED(hr))
		hr = pRenderTarget->BindDC(pDC->m_hDC, &rcButton);
	if (SUCCEEDED(hr))
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&pWriteFactory));
	if (SUCCEEDED(hr)) {
		hr = pWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"", &pTextFormat);
	}

	if (SUCCEEDED(hr)) {
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pRenderTarget->BeginDraw();
		pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		hr = pRenderTarget->CreateSolidColorBrush(ToD2DColor(colors.back), &pBackgroundBrush);
		if (SUCCEEDED(hr))
			hr = pRenderTarget->CreateSolidColorBrush(ToD2DColor(colors.border), &pBorderBrush);
		if (SUCCEEDED(hr))
			hr = pRenderTarget->CreateSolidColorBrush(ToD2DColor(colors.text), &pTextBrush);
		if (SUCCEEDED(hr)) {
			const float width = (float)rcButton.Width();
			const float height = (float)rcButton.Height();
			const float offset = bPressed ? 1.0f : 0.0f;
			const D2D1_ROUNDED_RECT button = D2D1::RoundedRect(
				D2D1::RectF(0.5f, 0.5f, width - 0.5f, height - 0.5f), 4.5f, 4.5f);
			pRenderTarget->FillRoundedRectangle(button, pBackgroundBrush);
			pRenderTarget->DrawRoundedRectangle(button, pBorderBrush, 1.0f);
			pRenderTarget->DrawText((LPCWSTR)CStringW(strText), (UINT32)strText.GetLength(), pTextFormat,
				D2D1::RectF(6.0f + offset, 1.0f + offset, width - 6.0f + offset, height - 1.0f + offset), pTextBrush);
		}
		HRESULT drawResult = pRenderTarget->EndDraw();
		if (SUCCEEDED(hr))
			hr = drawResult;
	}

	if (pTextBrush)
		pTextBrush->Release();
	if (pBorderBrush)
		pBorderBrush->Release();
	if (pBackgroundBrush)
		pBackgroundBrush->Release();
	if (pTextFormat)
		pTextFormat->Release();
	if (pWriteFactory)
		pWriteFactory->Release();
	if (pRenderTarget)
		pRenderTarget->Release();
	if (pD2DFactory)
		pD2DFactory->Release();
	return SUCCEEDED(hr);
}

IMPLEMENT_DYNAMIC(CNextButton, CButton)

BEGIN_MESSAGE_MAP(CNextButton, CButton)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_MOUSEMOVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

CNextButton::CNextButton()
	: m_bHot(false)
	, m_bTrackingMouse(false)
	, m_bPrimary(false)
{
}

void CNextButton::SetPrimary(bool bPrimary)
{
	if (m_bPrimary != bPrimary) {
		m_bPrimary = bPrimary;
		if (m_hWnd)
			Invalidate();
	}
}

void CNextButton::PreSubclassWindow()
{
	// Preserve the original default-button intent before changing the Windows
	// control type to owner-drawn. BS_OWNERDRAW shares bit flags with other
	// button styles, so testing the style later would be unreliable.
	m_bPrimary = (GetStyle() & BS_TYPEMASK) == BS_DEFPUSHBUTTON;
	ModifyStyle(BS_TYPEMASK, BS_OWNERDRAW | BS_NOTIFY);
	CButton::PreSubclassWindow();
}

void CNextButton::UpdateHotState(bool bHot)
{
	if (m_bHot != bHot) {
		m_bHot = bHot;
		Invalidate();
	}
}

void CNextButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackingMouse) {
		TRACKMOUSEEVENT tracking = {};
		tracking.cbSize = sizeof tracking;
		tracking.dwFlags = TME_LEAVE;
		tracking.hwndTrack = m_hWnd;
		if (::TrackMouseEvent(&tracking))
			m_bTrackingMouse = true;
	}
	UpdateHotState(true);
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CNextButton::OnMouseLeave(WPARAM, LPARAM)
{
	m_bTrackingMouse = false;
	UpdateHotState(false);
	return 0;
}

void CNextButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	if (!bEnable)
		m_bHot = false;
	Invalidate();
}

void CNextButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (pDC == NULL)
		return;

	const UINT state = lpDrawItemStruct->itemState;
	const bool bDisabled = (state & ODS_DISABLED) != 0;
	const bool bPressed = (state & ODS_SELECTED) != 0;
	const bool bPrimary = m_bPrimary;
	CRect rcButton(lpDrawItemStruct->rcItem);
	const NextButtonColors colors = GetNextButtonColors(bPrimary, bDisabled, bPressed, m_bHot);
	CString strText;
	GetWindowText(strText);

	if (DrawNextButtonWithDirect2D(pDC, rcButton, strText, colors, bPressed)) {
		if ((state & ODS_FOCUS) != 0) {
			CRect rcFocus(rcButton);
			rcFocus.DeflateRect(4, 3);
			pDC->DrawFocusRect(&rcFocus);
		}
		return;
	}

	// Direct2D is available on supported Windows versions. Keep the previous GDI
	// implementation as an automatic fallback for unusual or remote sessions.
	CPen borderPen(PS_SOLID, 1, colors.border);
	CBrush backBrush(colors.back);
	CPen *pOldPen = pDC->SelectObject(&borderPen);
	CBrush *pOldBrush = pDC->SelectObject(&backBrush);
	pDC->RoundRect(&rcButton, CPoint(5, 5));
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	CRect rcText(rcButton);
	rcText.DeflateRect(6, 1);
	if (bPressed)
		rcText.OffsetRect(1, 1);

	CFont *pOldFont = pDC->SelectObject(GetFont());
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF crOldText = pDC->SetTextColor(colors.text);
	pDC->DrawText(strText, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(crOldText);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(pOldFont);

	if ((state & ODS_FOCUS) != 0) {
		CRect rcFocus(rcButton);
		rcFocus.DeflateRect(4, 3);
		pDC->DrawFocusRect(&rcFocus);
	}
}
