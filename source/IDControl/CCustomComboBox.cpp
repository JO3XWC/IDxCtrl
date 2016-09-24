// CSplitExButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CCustomComboBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSplitExButton

IMPLEMENT_DYNAMIC(CCustomComboBox, CComboBox)

CCustomComboBox::CCustomComboBox()
{
	m_pDC			= NULL;
	m_hCustomCursor	= NULL;
	m_BkIndex		= 0;
	m_hListbox		= NULL;
	m_TitleFormat	= DT_LEFT;
	m_TextFormat	= DT_LEFT;
}

CCustomComboBox::~CCustomComboBox()
{
}


BEGIN_MESSAGE_MAP(CCustomComboBox, CComboBox)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CCustomComboBox::OnCbnDropdown)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// CSplitExButton メッセージ ハンドラー
VOID CCustomComboBox::Initialize ()
{
	//m_DC.Initialize (this, GetFont (), 100, 50, COMBOBOX_BK_START_COLOR, COMBOBOX_BK_FINISH_COLOR, RGB (10, 10, 10), COMBOBOX_TEXT_COLOR, COMBOBOX_LINE_COLOR);
}

BOOL CCustomComboBox::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CHILD | WS_VISIBLE | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL;

	return CComboBox::PreCreateWindow(cs);
}

int CCustomComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: ここに特定な作成コードを追加してください。

	return 0;
}

VOID CCustomComboBox::SetCustomDC (CCustomDC* pDC)
{
	m_pDC = pDC;
}
	
VOID CCustomComboBox::SetCustomCursor (HCURSOR hCursor)
{
	m_hCustomCursor = hCursor;
}

VOID CCustomComboBox::SetCustomBkIndex (INT Index)
{
	m_BkIndex = Index;
	Invalidate ();
	UpdateWindow ();
}

INT CCustomComboBox::GetCustomBkIndex ()
{
	return m_BkIndex;
}

VOID CCustomComboBox::SetTitle (LPCTSTR pszText)
{
	m_strTitle = pszText;
	Invalidate ();
	UpdateWindow ();
}

LPCTSTR	CCustomComboBox::GetTitle ()
{
	return m_strTitle;
}

VOID CCustomComboBox::SetTextFormat (UINT TitleFormat, UINT TextFormat)
{
	m_TextFormat = TextFormat;
	m_TitleFormat = TitleFormat;
	Invalidate ();
	UpdateWindow ();
}

VOID CCustomComboBox::SelectValue (DWORD_PTR Data)
{

	for (int i=0;i<GetCount ();i++)
	{
		if (GetItemData (i) == Data)
		{
			SetCurSel (i);
			break;
		}
	}
}

DWORD_PTR CCustomComboBox::GetSelectedItemData ()
{
	INT			Index;
	DWORD_PTR	Result	= -1;

	do
	{
		Index = GetCurSel ();
		if (Index == CB_ERR)
		{	break;
		}

		Result = GetItemData (Index);
	}
	while (0);

	return Result;
}


void CCustomComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC*			pDC			= CDC::FromHandle (lpDrawItemStruct->hDC);
	CRect			ItemRect (lpDrawItemStruct->rcItem);
	int 			nItem		= lpDrawItemStruct->itemID;
	COLORREF		ClrText		= 0;
	COLORREF		ClrLine		= m_pDC->GetLineColor ();
	COLORREF		ClrBk		= 0;
	INT				OldMode		= 0;
	CFont*			pOldFont	= NULL;
	CRect			ClientRect;
	INT				BltMode;
	CSize			BmpSize		= m_pDC->GetSize ();
	CString			strText		= m_strTitle;
	INT				BkIndex		= m_BkIndex;
	UINT			TextFormat	= m_TextFormat;

	//DBG_MSG((_T("######### State=%08X, CtlType=%u, CtlID=%u, itemID=%u, itemAction=%08X\n"), lpDrawItemStruct->itemState, lpDrawItemStruct->CtlType, lpDrawItemStruct->CtlID, lpDrawItemStruct->itemID, lpDrawItemStruct->itemAction));

	do
	{
		GetClientRect (&ClientRect);

		ClrText	= pDC->SetTextColor (m_pDC->GetTextColor ());
		ClrBk	= pDC->GetBkColor ();
		OldMode	= pDC->SetBkMode (TRANSPARENT);
		pOldFont= pDC->SelectObject (m_pDC->GetFont ());
		BltMode = pDC->SetStretchBltMode (HALFTONE);

		if (m_strTitle.IsEmpty ())
		{
			GetWindowText (strText);
		}

		do
		{
			if (lpDrawItemStruct->itemState & ODS_COMBOBOXEDIT)
			{
				BkIndex = 0;
				TextFormat	= m_TitleFormat;
				break;
			}

			if (lpDrawItemStruct->itemID == -1)
			{
				strText = _T("");
				break;
			}

			GetLBText (lpDrawItemStruct->itemID, strText);
			if (lpDrawItemStruct->itemState & (ODS_FOCUS | ODS_SELECTED))
			{	
				BkIndex = 1;
			}
		}
		while (0);

		BkIndex = (GetStyle () & WS_DISABLED) ? 2 : BkIndex;

		//pDC->FillSolidRect (&Rect, RGB (255, 255, 255));
		pDC->StretchBlt (ItemRect.left, ItemRect.top, ItemRect.Width (), ItemRect.Height (), m_pDC->GetDC (), 0, BkIndex * BmpSize.cy, BmpSize.cx, BmpSize.cy, SRCCOPY);
		//pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom - 1, ItemRect.right, ItemRect.bottom), ClrLine);
		//pDC->FillSolidRect (CRect (ItemRect.right - 1, ItemRect.top, ItemRect.right, ItemRect.bottom), ClrLine);

		//if (!(lpDrawItemStruct->itemState & ODS_COMBOBOXEDIT))
		//{
		//	ItemRect.left = ClientRect.left;
		//	ItemRect.right = ClientRect.right;

		//	//ItemRect.DeflateRect (	0, 0, GetSystemMetrics (SM_CXVSCROLL), 0);
		//}
	
		switch (TextFormat & 0xF)
		{
		case DT_LEFT:
			{
				ItemRect.left += (GetSystemMetrics (SM_CYEDGE) + GetSystemMetrics (SM_CXBORDER));
			}
			break;
		case DT_RIGHT:
			{
				ItemRect.right -= (GetSystemMetrics (SM_CYEDGE) + GetSystemMetrics (SM_CXBORDER));
			}
			break;
		}

		pDC->DrawText (strText, ItemRect, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | TextFormat);

		pDC->SetTextColor (ClrText);
		pDC->SetBkColor (ClrBk);
		pDC->SetBkMode (OldMode);
		pDC->SelectObject (pOldFont);
		pDC->SetStretchBltMode (BltMode);
	}
	while (0);




}

void CCustomComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CRect Rect;
	GetWindowRect (&Rect);
	
	lpMeasureItemStruct->itemHeight = m_pDC->GetSize ().cy;
	
	if (lpMeasureItemStruct->itemID == -1)
	{
		lpMeasureItemStruct->itemHeight -= (GetSystemMetrics (SM_CYEDGE) + GetSystemMetrics (SM_CXBORDER)) * 2;
	}
}


HBRUSH CCustomComboBox::CtlColor(CDC* pDC, UINT nCtlColor)
{
	//DBG_MSG((_T("nCtlColor=%X\n"), nCtlColor));

	CRect			ItemRect;
	CRect			ClipRect;
	CSize			BmpSize		= m_pDC->GetSize ();
	INT				BltMode		= pDC->SetStretchBltMode (HALFTONE);
	COLORREF		ClrLine		= m_pDC->GetLineColor ();
	INT				BkIndex		= (GetStyle () & WS_DISABLED) ? 2 : m_BkIndex;
	COLORREF		ClrTri		= RGB (110, 110, 110);
	INT				Width		= GetSystemMetrics (SM_CXVSCROLL) + GetSystemMetrics (SM_CYEDGE) + GetSystemMetrics (SM_CXBORDER);

	pDC->GetClipBox (&ClipRect);

	GetClientRect (&ItemRect);
	//pDC->FillSolidRect (&Rect, RGB (70, 70, 70));
	// TODO: ここで DC の属性を変更してください。
	pDC->StretchBlt (ItemRect.left, ItemRect.top, ItemRect.Width (), ItemRect.Height (), m_pDC->GetDC (), 0, BkIndex * BmpSize.cy, BmpSize.cx, BmpSize.cy, SRCCOPY);


	// Create an array of TRIVERTEX structures that describe
	// positional and color values for each vertex.
	TRIVERTEX Vertex[3];
	Vertex[0].x     = ItemRect.right - (Width - 6);
	Vertex[0].y     = ItemRect.top + static_cast<LONG>(ItemRect.Height () * 0.3f);
	Vertex[0].Red   = GetRValue (ClrTri) << 8;
	Vertex[0].Green = GetGValue (ClrTri) << 8;
	Vertex[0].Blue  = GetBValue (ClrTri) << 8;
	Vertex[0].Alpha = 0x0000;

	Vertex[1].x     = ItemRect.right - 6;
	Vertex[1].y     = ItemRect.top + static_cast<LONG>(ItemRect.Height () * 0.3f);
	Vertex[1].Red   = GetRValue (ClrTri) << 8;
	Vertex[1].Green = GetGValue (ClrTri) << 8;
	Vertex[1].Blue  = GetBValue (ClrTri) << 8;
	Vertex[1].Alpha = 0x0000;

	Vertex[2].x     = ItemRect.right - (Width / 2);
	Vertex[2].y     = ItemRect.bottom - static_cast<LONG>(ItemRect.Height () * 0.3f);
	Vertex[2].Red   = GetRValue (ClrTri) << 8;
	Vertex[2].Green = GetGValue (ClrTri) << 8;
	Vertex[2].Blue  = GetBValue (ClrTri) << 8;
	Vertex[2].Alpha = 0x0000;

	// Create a GRADIENT_TRIANGLE structure that
	// references the TRIVERTEX vertices.
	GRADIENT_TRIANGLE gTriangle[1];
	gTriangle[0].Vertex1 = 0;
	gTriangle[0].Vertex2 = 1;
	gTriangle[0].Vertex3 = 2;

	// Draw a shaded triangle.
	pDC->GradientFill(Vertex, _countof (Vertex), gTriangle, _countof (gTriangle), GRADIENT_FILL_TRIANGLE);


	pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom - 1, ItemRect.right, ItemRect.bottom), ClrLine);
	pDC->FillSolidRect (CRect (ItemRect.right - 1, ItemRect.top, ItemRect.right, ItemRect.bottom), ClrLine);
	pDC->SetStretchBltMode (BltMode);
	// TODO: 親のハンドラーを呼び出せない場合は NULL 以外のブラシを返します。
	//return NULL;
	return (HBRUSH)GetStockObject (NULL_BRUSH);
}


BOOL CCustomComboBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//DBG_MSG((_T("CCustomComboBox::OnSetCursor - pWnd=%p, nHitTest=%X, message=%X\n"), pWnd, nHitTest, message));
	if (m_hCustomCursor != NULL)
	{
		SetCursor (m_hCustomCursor);
		return TRUE;
	}

	return CComboBox::OnSetCursor(pWnd, nHitTest, message);
}




void CCustomComboBox::OnCbnDropdown()
{
	UpdateDropDownWidth ();
}




HBRUSH CCustomComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
	case CTLCOLOR_LISTBOX:
		{
			m_hListbox = pWnd->m_hWnd;
		}
		break;

	case CTLCOLOR_EDIT:
		{
		}
		break;
	}

	// TODO: 既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}

VOID CCustomComboBox::UpdateDropDownWidth ()
{
	CString		str;
	CSize		sz;
	int			dx			= 0;
	TEXTMETRIC	tm			= {};
	CDC*		pDC			= GetDC();
	CFont*		pFont		= m_pDC->GetFont();
	CFont*		pOldFont	= NULL;

	ASSERT (pFont != NULL);
	ASSERT (pDC != NULL);

	do
	{
		ASSERT (pDC != NULL);
		if (pDC == NULL)
		{	break;
		}

		// Select the listbox font, save the old font
		pOldFont = pDC->SelectObject (pFont);
		
		// Get the text metrics for avg char width
		pDC->GetTextMetrics (&tm);

		for (int i = 0; i < GetCount (); i++)
		{
			GetLBText (i, str);
			sz = pDC->GetTextExtent (str);

			// Add the avg width to prevent clipping
			sz.cx += tm.tmAveCharWidth;

			if (sz.cx > dx)
			{	dx = sz.cx;
			}
		}

		// Select the old font back into the DC
		pDC->SelectObject (pOldFont);
		ReleaseDC (pDC);

		// Adjust the width for the vertical scroll bar and the left and right border.
		dx += ::GetSystemMetrics (SM_CXVSCROLL) + 2 * ::GetSystemMetrics (SM_CXEDGE);
	}
	while (0);

	SetDroppedWidth (dx);
}
