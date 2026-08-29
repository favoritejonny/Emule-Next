#include "stdafx.h"
#include "TrayMenuBtn.h"
#include "NextTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTrayMenuBtn

BEGIN_MESSAGE_MAP(CTrayMenuBtn, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CTrayMenuBtn::CTrayMenuBtn()
	: m_sIcon()
	, m_hIcon()
	, m_nBtnID(rand())
	, m_bBold()
	, m_bMouseOver()
	, m_bNoHover()
	, m_bUseIcon()
	, m_bParentCapture()
{
}

CTrayMenuBtn::~CTrayMenuBtn()
{
	if (m_hIcon)
		::DestroyIcon(m_hIcon);
}

void CTrayMenuBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rClient;
	GetClientRect(rClient);

	if (rClient.PtInRect(point)) {
		SetCapture();

		if (!m_bNoHover)
			m_bMouseOver = true;
	} else {
		if (m_bParentCapture) {
			CWnd *pParent = GetParent();
			if (pParent)
				pParent->SetCapture();
			else
				::ReleaseCapture();
		} else
			::ReleaseCapture();

		m_bMouseOver = false;
	}
	Invalidate();

	CWnd::OnMouseMove(nFlags, point);
}

void CTrayMenuBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rClient;
	GetClientRect(rClient);

	if (rClient.PtInRect(point)) {
		CWnd *pParent = GetParent();
		if (pParent)
			pParent->PostMessage(WM_COMMAND, MAKEWPARAM(m_nBtnID, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
	} else {
		::ReleaseCapture();
		m_bMouseOver = false;
		Invalidate();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CTrayMenuBtn::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rClient;
	GetClientRect(rClient);

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap MemBMP, *pOldBMP;
	MemBMP.CreateCompatibleBitmap(&dc, rClient.Width(), rClient.Height());
	pOldBMP = MemDC.SelectObject(&MemBMP);
	CFont *pOldFONT = m_cfFont.GetSafeHandle() ? MemDC.SelectObject(&m_cfFont) : NULL;

	BOOL bEnabled = IsWindowEnabled();

	const bool bHover = m_bMouseOver && bEnabled;
	const EEmuleNextTheme eTheme = CNextTheme::GetTheme();
	COLORREF crBackground = ::GetSysColor(COLOR_BTNFACE);
	COLORREF crText = ::GetSysColor(COLOR_BTNTEXT);
	COLORREF crAccent = ::GetSysColor(COLOR_HIGHLIGHT);
	if (eTheme == entModernLight) {
		crBackground = bHover ? RGB(226, 240, 252) : RGB(248, 250, 252);
		crText = bEnabled ? RGB(35, 49, 64) : RGB(145, 153, 162);
		crAccent = RGB(51, 145, 210);
	}
	else if (eTheme == entModernAurora) {
		crBackground = bHover ? RGB(218, 241, 244) : RGB(244, 249, 250);
		crText = bEnabled ? RGB(31, 58, 67) : RGB(139, 157, 161);
		crAccent = RGB(48, 167, 173);
	}

	MemDC.FillSolidRect(rClient, crBackground);
	if (bHover && eTheme != entClassic) {
		CPen borderPen(PS_SOLID, 1, crAccent);
		CBrush backgroundBrush(crBackground);
		CPen *pOldPen = MemDC.SelectObject(&borderPen);
		CBrush *pOldBrush = MemDC.SelectObject(&backgroundBrush);
		CRect rcHighlight(rClient);
		rcHighlight.DeflateRect(1, 1);
		MemDC.RoundRect(rcHighlight, CPoint(7, 7));
		MemDC.SelectObject(pOldBrush);
		MemDC.SelectObject(pOldPen);
	}
	MemDC.SetTextColor(eTheme == entClassic && bHover ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : crText);

	int iLeftOffset = 0;
	if (m_bUseIcon) {
		MemDC.DrawState(CPoint(7, rClient.Height() / 2 - m_sIcon.cy / 2), m_sIcon, m_hIcon,
			DST_ICON | (bEnabled ? DSS_NORMAL : DSS_DISABLED), (CBrush*)NULL);
		iLeftOffset = m_sIcon.cx + 9;
	}

	MemDC.SetBkMode(TRANSPARENT);
	CRect rText;
	MemDC.DrawText(m_strText, rText, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
	CPoint pt(rClient.left + 7 + iLeftOffset, rClient.Height() / 2 - rText.Height() / 2);
	CPoint sz(rText.Width(), rText.Height());
	MemDC.DrawState(pt, sz, m_strText, DST_TEXT | (bEnabled ? DSS_NORMAL : DSS_DISABLED)
				, FALSE, m_strText.GetLength(), (CBrush*)NULL);
	dc.BitBlt(0, 0, rClient.Width(), rClient.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBMP);
	if (pOldFONT)
		MemDC.SelectObject(pOldFONT);
}
