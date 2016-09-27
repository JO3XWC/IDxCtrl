
#include "stdafx.h"
#include "mainfrm.h"
#include "CRepeaterView.h"
#include "Resource.h"
#include "IDControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree メッセージ ハンドラー

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}




/////////////////////////////////////////////////////////////////////////////
// CRepeaterView

CRepeaterView::CRepeaterView()
{
}

CRepeaterView::~CRepeaterView()
{
}

BEGIN_MESSAGE_MAP(CRepeaterView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SET_TO_CALLSIGN, OnSetToCallsign)
	ON_UPDATE_COMMAND_UI(ID_SET_TO_CALLSIGN, &CRepeaterView::OnUpdateSetToCallsign)
	ON_COMMAND(ID_SET_FROM_CALLSIGN, OnSetFromCallsign)
	ON_UPDATE_COMMAND_UI(ID_SET_FROM_CALLSIGN, &CRepeaterView::OnUpdateSetFromCallsign)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(NM_DBLCLK, 4, &CRepeaterView::OnNMDblclkTree)
	ON_NOTIFY(TVN_DELETEITEM, 4, &CRepeaterView::OnTvnDeleteitemTree)
	ON_NOTIFY(TVN_ITEMEXPANDED, 4, &CRepeaterView::OnTvnExpanded)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar メッセージ ハンドラー

int CRepeaterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// ビューの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndTreeView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("ファイル ビューを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	m_wndTreeView.SetBkColor (RGB (40, 40, 40));
	m_wndTreeView.SetTextColor (RGB (222, 222, 222));

	// ビューのイメージの読み込み:
	m_TreeViewImages.Create(IDB_TREE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndTreeView.SetImageList(&m_TreeViewImages, TVSIL_NORMAL);

	OnChangeVisualStyle();

	LoadRepeaterList (NULL);

	AdjustLayout();

	return 0;
}

void CRepeaterView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void CRepeaterView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndTreeView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}


	CRepeater*	pRepeater;
	CPoint		ptTree = point;
	UINT		flags = 0;
	HTREEITEM	hTreeItem;

	do
	{
		if (point == CPoint(-1, -1))
		{	break;
		}

		// クリックされた項目の選択:
		pWndTree->ScreenToClient (&ptTree);

		hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem == NULL)
		{	break;
		}

		pWndTree->SelectItem (hTreeItem);
		pWndTree->SetFocus();

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hTreeItem));
		if (pRepeater == NULL)
		{	break;
		}

		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_REPEATER, point.x, point.y, this, TRUE);
	}
	while (0);


}

void CRepeaterView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = 0;

	m_wndTreeView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CRepeaterView::OnTvnDeleteitemTree (NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW	pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CRepeater*		pRepeater;

	do
	{
		if (!(pNMTreeView->itemOld.mask & TVIF_PARAM))
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(pNMTreeView->itemOld.lParam);
		if (pRepeater == NULL)
		{	break;
		}

		theApp.GetRepeaterList ()->RemoveAt (pRepeater);
	}
	while (0);

	*pResult = 0;
}

void CRepeaterView::OnTvnExpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW	pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	do
	{
		if (pNMTreeView->itemNew.state & TVIS_EXPANDED)
		{
			m_wndTreeView.SetItemImage (pNMTreeView->itemNew.hItem, 1, 1);
		}
		else
		{
			m_wndTreeView.SetItemImage (pNMTreeView->itemNew.hItem, 0, 0);
		}

	}
	while (0);

	*pResult = 0;
}







void CRepeaterView::OnUpdateSetToCallsign(CCmdUI *pCmdUI)
{
	HTREEITEM	hItem;
	CRepeater*	pRepeater;

	do
	{
		hItem = m_wndTreeView.GetSelectedItem ();
		if (hItem == NULL)
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hItem));
		if (pRepeater == NULL)
		{	break;
		}

		pCmdUI->Enable ();
	}
	while (0);
}


void CRepeaterView::OnSetToCallsign()
{
	HTREEITEM	hItem;
	CRepeater*	pRepeater;

	do
	{
		hItem = m_wndTreeView.GetSelectedItem ();
		if (hItem == NULL)
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hItem));
		if (pRepeater == NULL)
		{	break;
		}

		AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_TO_CALLSIGN, reinterpret_cast<WPARAM>(pRepeater));
	}
	while (0);
}

void CRepeaterView::OnUpdateSetFromCallsign(CCmdUI *pCmdUI)
{
	HTREEITEM	hItem;
	CRepeater*	pRepeater;

	do
	{
		hItem = m_wndTreeView.GetSelectedItem ();
		if (hItem == NULL)
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hItem));
		if (pRepeater == NULL)
		{	break;
		}

		if (!pRepeater->GetRpt1Use ())
		{
			break;
		}

		pCmdUI->Enable ();
	}
	while (0);
}

void CRepeaterView::OnSetFromCallsign()
{
	HTREEITEM	hItem;
	CRepeater*	pRepeater;

	do
	{
		hItem = m_wndTreeView.GetSelectedItem ();
		if (hItem == NULL)
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hItem));
		if (pRepeater == NULL)
		{	break;
		}

		if (!pRepeater->GetRpt1Use ())
		{	break;
		}

		AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_TOFROM_CALLSIGN, reinterpret_cast<WPARAM>(pRepeater));
	}
	while (0);
}







void CRepeaterView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_wndTreeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CRepeaterView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndTreeView.SetFocus();
}

void CRepeaterView::OnChangeVisualStyle()
{
	m_TreeViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_TREE_VIEW_24 : IDB_TREE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("ビットマップを読み込めませんでした: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_TreeViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_TreeViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndTreeView.SetImageList(&m_TreeViewImages, TVSIL_NORMAL);
}


VOID CRepeaterView::ClearRepeaterList ()
{
	m_wndTreeView.DeleteAllItems ();
}

VOID CRepeaterView::LoadRepeaterList (LPCTSTR pszFileName)
{
	CString			strDisplay;
	HTREEITEM		hRoot;
	HTREEITEM		hItem;
	CRepeater*		pRepeater;
	CMap<ULONG, ULONG, HTREEITEM, HTREEITEM> Map;
	CList<CRepeater*>*	pList	= &theApp.GetRepeaterList ()->m_List;
	POSITION		Pos;

	do
	{
		ClearRepeaterList ();

		if (pszFileName != NULL)
		{	
			theApp.GetRepeaterList ()->Load (pszFileName);
		}
		
		Pos = pList->GetHeadPosition ();

		while (Pos != NULL)
		{
			pRepeater = pList->GetNext (Pos);

			if (!Map.Lookup (pRepeater->GetGroup (), hRoot))
			{
				strDisplay.Format (_T("%02u : %s"), pRepeater->GetGroup (), pRepeater->GetGroupName ());
				hRoot = m_wndTreeView.InsertItem (TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, strDisplay, 0, 0, 0, 0, NULL, TVI_ROOT, TVI_LAST);

				Map.SetAt (pRepeater->GetGroup (), hRoot); 
			}

			strDisplay = pRepeater->GetName ();
			if (lstrlen (pRepeater->GetCallsign ()) > 0)
			{
				strDisplay.AppendFormat (_T(" (%s)"), pRepeater->GetCallsign ());
			}

			hItem = m_wndTreeView.InsertItem (TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, 
				strDisplay, 2, 2, 0, 0, reinterpret_cast<LPARAM>(pRepeater), hRoot, TVI_LAST);
		}
	}
	while (0);
}


void CRepeaterView::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW	pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CRepeater*		pRepeater;
	HTREEITEM		hItem;
	UINT			uFlag = 0;
	CPoint			pt = ::GetMessagePos();

	do
	{
		
		m_wndTreeView.ScreenToClient(&pt);
		
		hItem = m_wndTreeView.HitTest (pt, &uFlag);
		if(hItem == NULL || !(uFlag & TVHT_ONITEM))
		{	break;
		}

		pRepeater = reinterpret_cast<CRepeater*>(m_wndTreeView.GetItemData (hItem));
		if (pRepeater == NULL)
		{	break;
		}

		if (!pRepeater->GetRpt1Use ())
		{	break;
		}

		AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_TOFROM_CALLSIGN, reinterpret_cast<WPARAM>(pRepeater));
	}
	while (0);

	*pResult = 0;
}
