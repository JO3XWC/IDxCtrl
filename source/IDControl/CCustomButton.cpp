// CCustomButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CCustomButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CCustomButton

IMPLEMENT_DYNAMIC(CCustomButton, CButton)

CCustomButton::CCustomButton()
{
	m_pDC			= NULL;
	m_hCustomCursor	= NULL;
	m_BkIndex		= 0;

}

CCustomButton::~CCustomButton()
{
}


BEGIN_MESSAGE_MAP(CCustomButton, CButton)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


VOID CCustomButton::Initialize ()
{
}

// CCustomButton メッセージ ハンドラー


VOID CCustomButton::SetCustomDC (CCustomDC* pDC)
{
	m_pDC = pDC;
}
	
VOID CCustomButton::SetCustomCursor (HCURSOR hCursor)
{
	m_hCustomCursor = hCursor;
}

VOID CCustomButton::SetCustomBkIndex (INT Index)
{
	m_BkIndex = Index;
	Invalidate ();
	UpdateWindow ();
}

INT CCustomButton::GetCustomBkIndex ()
{
	return m_BkIndex;
}


void CCustomButton::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CDC*			pDC			= CDC::FromHandle (pDrawItemStruct->hDC);
	CRect			ItemRect (pDrawItemStruct->rcItem);
	int 			nItem		= pDrawItemStruct->itemID;
	COLORREF		ClrText		= 0;
	COLORREF		ClrLine		= m_pDC->GetLineColor ();
	COLORREF		ClrBk		= 0;
	INT				OldMode		= 0;
	CFont*			pOldFont	= NULL;
	CRect			ClientRect;
	INT				BltMode;
	CSize			BmpSize		= m_pDC->GetSize ();
	CString			strText;
	INT				BkIndex		= (GetStyle () & WS_DISABLED) ? 2 : m_BkIndex;

	do
	{
		DBG_MSG((_T("CCustomButton::DrawItem - itemState=%08X\n"), pDrawItemStruct->itemState));

		ClrText	= pDC->SetTextColor (m_pDC->GetTextColor ());
		ClrBk	= pDC->GetBkColor ();
		OldMode	= pDC->SetBkMode (TRANSPARENT);
		pOldFont= pDC->SelectObject (m_pDC->GetFont ());
		BltMode = pDC->SetStretchBltMode (HALFTONE);

		pDC->StretchBlt (ItemRect.left, ItemRect.top, ItemRect.Width (), ItemRect.Height (), m_pDC->GetDC (), 0, BkIndex * BmpSize.cy, BmpSize.cx, BmpSize.cy, SRCCOPY);
	
		if (pDrawItemStruct->itemState & ODS_SELECTED)
		{
			pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom, ItemRect.left + 1, ItemRect.top), ClrLine);
			pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.top, ItemRect.right, ItemRect.top + 1), ClrLine);
			ItemRect.OffsetRect (1, 1);
		}
		else
		{
			pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom - 1, ItemRect.right, ItemRect.bottom), ClrLine);
			pDC->FillSolidRect (CRect (ItemRect.right - 1, ItemRect.top, ItemRect.right, ItemRect.bottom), ClrLine);
		}

		GetWindowText (strText);
		pDC->DrawText (strText, ItemRect, DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER);

		pDC->SetTextColor (ClrText);
		pDC->SetBkColor (ClrBk);
		pDC->SetBkMode (OldMode);
		pDC->SelectObject (pOldFont);
		pDC->SetStretchBltMode (BltMode);
	}
	while (0);

}


BOOL CCustomButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//DBG_MSG((_T("CCustomButton::OnSetCursor - pWnd=%p, nHitTest=%X, message=%X\n"), pWnd, nHitTest, message));
	
	if (m_hCustomCursor != NULL)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		if (rectClient.PtInRect(ptCursor))
		{
			::SetCursor(m_hCustomCursor);
			return TRUE;
		}
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
