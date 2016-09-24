// CMFCListCtrlEx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CListCtrlEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define LIST_TEXT_COLOR		RGB (222, 222, 222)
//#define LIST_BK_COLOR		RGB (40, 40, 40)

/*
CListCtrlExDC::CListCtrlExDC ()
{
}
	
CListCtrlExDC::~CListCtrlExDC ()
{
}

VOID CListCtrlExDC::Initialize (CWnd* pWnd, INT cx, INT cy, COLORREF colorStart, COLORREF colorFinish, COLORREF colorText, COLORREF colorLine, BOOL bHorz, int nStartFlatPercentage, int nEndFlatPercentage)
{
	VOID*	pBits	= NULL;
	CDC*	pDC		= pWnd->GetDC ();
	CRect	sRect;
	CRect	eRect;
	CFont*	pFont	= pWnd->GetFont ();
	LOGFONT	LogFont	= {};

	pFont->GetLogFont (&LogFont);
	LogFont.lfWeight = FW_BOLD;
	m_Font.CreateFontIndirect (&LogFont);


	m_ClrText	= colorText;
	m_ClrLine	= colorLine;
	m_Size		= CSize(cx, cy);

	sRect = CRect (0, 0, m_Size.cx, m_Size.cy / 2);
	eRect = CRect (0, m_Size.cy / 2, m_Size.cx, m_Size.cy);

	m_pBmp = CBitmap::FromHandle (CDrawingManager::CreateBitmap_32 (m_Size, &pBits));

	m_MemDC.CreateCompatibleDC (pDC);
	m_pOldBmp = m_MemDC.SelectObject (m_pBmp);
	
	CDrawingManager dm(m_MemDC);
	dm.FillGradient(sRect, colorStart, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);
	dm.FillGradient(eRect, colorFinish, colorStart, bHorz, nStartFlatPercentage, nEndFlatPercentage);

	pWnd->ReleaseDC (pDC);
}

VOID CListCtrlExDC::Deinitialize ()
{
	m_MemDC.SelectObject (m_pOldBmp);
	delete m_pBmp;

	m_pOldBmp = NULL;
	m_pBmp = NULL;
}

CDC* CListCtrlExDC::GetDC ()
{
	return &m_MemDC;
}

CSize CListCtrlExDC::GetSize ()
{
	return m_Size;
}

COLORREF CListCtrlExDC::GetLineColor ()
{
	return m_ClrLine;
}

COLORREF CListCtrlExDC::GetTextColor ()
{
	return m_ClrText;
}

CFont* CListCtrlExDC::GetFont ()
{
	return &m_Font;
}
*/


IMPLEMENT_DYNAMIC(CListCtrlExHeader, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CListCtrlExHeader, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

VOID CListCtrlExHeader::Initialize ()
{
	m_DC.Initialize (this, GetFont (), 100, 50, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);
	SetBkColor (CUSTOM_DC_BK_NORMAL_COLOR);
}

void CListCtrlExHeader::OnPaint()
{
	CPaintDC		dc(this); // device context for painting
	CRect			ClientRect;
	CRect			ItemRect;
	CArray<CRect>	RectArray;
	CDC*			pDC			= NULL;
	COLORREF		ClrText		= 0;
	COLORREF		ClrLine		= 0;
	COLORREF		ClrBk		= 0;
	INT				OldMode		= 0;
	CFont*			pOldFont	= NULL;
	INT				Count;
	int				BltMode;
	CSize			BmpSize		= m_DC.GetSize ();
	HDITEM			hdi			= {};
	TCHAR			szText[512]	= {};
	

	do
	{
		pDC = &dc;//GetDC ();
		if (pDC == NULL)
		{	break;
		}

		BltMode = pDC->SetStretchBltMode (HALFTONE);
		ClrText	= pDC->SetTextColor (m_DC.GetTextColor ());
		ClrBk	= pDC->GetBkColor ();
		OldMode	= pDC->SetBkMode (TRANSPARENT);
		pOldFont= pDC->SelectObject (m_DC.GetFont ());
		
		ClrLine	= m_DC.GetLineColor ();

		//GetClientRect (ClientRect);
		//pDC->FillSolidRect (&ClientRect, GetSysColor (COLOR_WINDOW));
		Count = GetItemCount ();

		for (int nColumn=0;nColumn<Count;nColumn++)
		{
			::ZeroMemory (szText, sizeof (szText));

			hdi.mask = HDI_TEXT | HDI_FORMAT;
			hdi.pszText = szText;
			hdi.cchTextMax = _countof (szText);
			GetItem (nColumn, &hdi);

			GetItemRect (nColumn, &ItemRect);
			//DBG_MSG((_T("i=%d, %d, %d, %d, %d\n"), i, HeaderRect.left, HeaderRect.top, HeaderRect.right, HeaderRect.bottom));

			pDC->StretchBlt (ItemRect.left, ItemRect.top, ItemRect.Width (), ItemRect.Height (), m_DC.GetDC (), 0, 0, BmpSize.cx, BmpSize.cy, SRCCOPY);
			
			//ItemRect.DeflateRect (1, 1, 1, 1);
			pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom - 1, ItemRect.right, ItemRect.bottom), ClrLine);
			pDC->FillSolidRect (CRect (ItemRect.right - 1, ItemRect.top, ItemRect.right, ItemRect.bottom), ClrLine);

			ItemRect.DeflateRect (2, 0, 2, 0);
			pDC->DrawText (szText, ItemRect, DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		}
	}
	while (0);

	if (pDC != NULL)
	{
		pDC->SetStretchBltMode (BltMode);
		pDC->SetBkColor (ClrBk);
		pDC->SetBkMode (OldMode);
		pDC->SelectObject (pOldFont);
		//ReleaseDC (pDC);
	}
}

BOOL CListCtrlExHeader::OnEraseBkgnd(CDC* pDC)
{
	CRect	Rect;

	GetClientRect (&Rect);

	pDC->FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
	return TRUE;
	//return CHeaderCtrl::OnEraseBkgnd(pDC);
}







// CMFCListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{

}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
END_MESSAGE_MAP()



// CMFCListCtrlEx メッセージ ハンドラー

VOID CListCtrlEx::Initialize ()
{
	ULONG Style = GetStyle ();

	Style &= ~LVS_TYPEMASK;
	Style |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_NOSORTHEADER;

	ModifyStyle (-1, Style);

	if (m_HeaderCtrl.m_hWnd == NULL)
	{
		HWND hWnd = GetHeaderCtrl ()->GetSafeHwnd ();
		
		if (hWnd != NULL)
		{
			//INT Height;

			m_HeaderCtrl.SubclassWindow (hWnd);
			m_HeaderCtrl.Initialize ();
			m_HeaderCtrl.SetBkColor (RGB (30, 30, 30));
			//Height = m_Header.SetupHeight ();
			
			//m_ImageList.Create (1, Height, ILC_COLORDDB, 1, 0);

			//SetImageList (&m_ImageList, LVSIL_SMALL);

			//EnableExtDiv (TRUE);
		}
	}

	//SetFont (&m_CellFont[1]);
	m_DC.Initialize (this, GetFont (), 100, 50, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);
	SetBkColor (CUSTOM_DC_BK_NORMAL_COLOR);

	OnInitialize ();
}

VOID CListCtrlEx::OnInitialize ()
{
}




void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC*			pDC			= CDC::FromHandle (lpDrawItemStruct->hDC);
	CDC*			pGetDC		= NULL;
	int 			nItem		= lpDrawItemStruct->itemID;
	COLORREF		ClrText		= 0;
	COLORREF		ClrLine		= 0;
	COLORREF		ClrBk		= 0;
	INT				OldMode		= 0;
	LV_ITEM 		lvi			= {};
	CFont*			pOldFont	= NULL;
	CArray<CRect>	RectArray;
	CRect			ClientRect;
	CRect			ItemRectAll;
	CRect			ItemRect;
	CRect			HeaderRect;
	CHeaderCtrl*	pHeaderCtrl	= GetHeaderCtrl ();
	INT				BltMode;
	CSize			BmpSize		= m_DC.GetSize ();
	INT				Count;
	CString			strText;
	INT				StartY		= 0;

	do
	{
		//---------------------------------------------------------------------------------------------
		// get item data ------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------
		lvi.mask		= LVIF_IMAGE | LVIF_STATE;
		lvi.iItem		= nItem;
		lvi.iSubItem	= 0;
		lvi.stateMask	= LVIS_SELECTED | LVIS_FOCUSED;		// get all state flags

		GetItem (&lvi);

		if (lvi.state & LVIS_SELECTED)
		{
			StartY = BmpSize.cy;
		}
	
		ClrText	= pDC->SetTextColor (m_DC.GetTextColor ());
		ClrBk	= pDC->GetBkColor ();
		OldMode	= pDC->SetBkMode (TRANSPARENT);
		pOldFont= pDC->SelectObject (m_DC.GetFont ());
		BltMode = pDC->SetStretchBltMode (HALFTONE);
		
		ClrLine	= m_DC.GetLineColor ();
		
		GetClientRect (ClientRect);
		Count = pHeaderCtrl->GetItemCount ();
		
		if (!GetItemRect (nItem, &ItemRectAll, LVIR_BOUNDS))
		{	ASSERT (FALSE);
			break;
		}

		ItemRect.top	= ItemRectAll.top;
		ItemRect.bottom	= ItemRectAll.bottom;

		for (int nColumn=0;nColumn<Count;nColumn++)
		{
			pHeaderCtrl->GetItemRect (nColumn, &HeaderRect);
			
			ItemRect.left	= HeaderRect.left;
			ItemRect.right	= HeaderRect.right;

			pDC->StretchBlt (ItemRect.left, ItemRect.top, ItemRect.Width (), ItemRect.Height (), m_DC.GetDC (), 0, StartY, BmpSize.cx, BmpSize.cy, SRCCOPY);
			
			//ItemRect.DeflateRect (1, 1, 1, 1);
			pDC->FillSolidRect (CRect (ItemRect.left, ItemRect.bottom - 1, ItemRect.right, ItemRect.bottom), m_DC.GetLineColor ());
			pDC->FillSolidRect (CRect (ItemRect.right - 1, ItemRect.top, ItemRect.right, ItemRect.bottom), m_DC.GetLineColor ());

			ItemRect.DeflateRect (2, 0, 2, 0);
			pDC->DrawText (GetItemText (nItem, nColumn), ItemRect, DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		}
	}
	while (0);

	if (pDC != NULL)
	{
		pDC->SetTextColor (ClrText);
		pDC->SetBkColor (ClrBk);
		pDC->SetBkMode (OldMode);
		pDC->SelectObject (pOldFont);
		pDC->SetStretchBltMode (BltMode);
	}

}



