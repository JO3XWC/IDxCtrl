
#include "stdafx.h"
#include "MainFrm.h"
#include "CCommonView.h"
#include "Resource.h"
#include "IDControl.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT WM_ON_COMMON_MSG = ::RegisterWindowMessage (_T("WM_ON_COMMON_MSG"));

IMPLEMENT_DYNCREATE(CCommonScrollView, CScrollWnd)

CCommonScrollView::CCommonScrollView()
{

}

CCommonScrollView::~CCommonScrollView()
{
}

BEGIN_MESSAGE_MAP(CCommonScrollView, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_START, IDC_BTN_MAX, &CCommonScrollView::OnBtnClicked)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CMB_START, IDC_CMB_MAX, &CCommonScrollView::OnCmbSelChange)
END_MESSAGE_MAP()


int CCommonScrollView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CScrollWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	INT		Height	= 20;
	INT		Width	= 70;
	INT		Space	= 10;
	CPoint	Pos;
	CRect	Rect;
	CString	strTemp;
	INT		TmpIdx;

	SetScrollSizes (MM_TEXT, CSize ((Width + Space) * 6, Height + Space));
	
	m_DC.Initialize (this, NULL, 100, Height, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR2, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);

	Pos.SetPoint (0, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Power.SetCustomDC (&m_DC);
	m_Power.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Power.Create (_T("POWER"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_MAIN_POWER);

	Pos.SetPoint (1, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Dual.SetCustomDC (&m_DC);
	m_Dual.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Dual.Create (_T("DUAL"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_DUAL);

	Pos.SetPoint (2, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Monitor.SetCustomDC (&m_DC);
	m_Monitor.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Monitor.Create (_T("MONITOR"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW	, Rect, this, IDC_BTN_MONITOR);

	Pos.SetPoint (3, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Att.SetCustomDC (&m_DC);
	m_Att.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Att.Create (_T("ATT"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW	, Rect, this, IDC_BTN_ATT);

	Pos.SetPoint (4, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_RfPower.SetCustomDC (&m_DC);
	m_RfPower.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_RfPower.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_RF_POWER);
	m_RfPower.SetTextFormat (DT_CENTER, DT_CENTER);
	TmpIdx = m_RfPower.AddString (_T("5.0W"));	m_RfPower.SetItemData (TmpIdx, 0x0255);
	TmpIdx = m_RfPower.AddString (_T("2.5W"));	m_RfPower.SetItemData (TmpIdx, 0x0128);
	TmpIdx = m_RfPower.AddString (_T("0.5W"));	m_RfPower.SetItemData (TmpIdx, 0x0064);
	TmpIdx = m_RfPower.AddString (_T("0.1W"));	m_RfPower.SetItemData (TmpIdx, 0x0000);

	Pos.SetPoint (5, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_SpeeachLv.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_SpeeachLv.SetCustomDC (&m_DC);
	m_SpeeachLv.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_SPEEACH_LEVEL);
	m_SpeeachLv.SetTextFormat (DT_CENTER, DT_CENTER);
	for (int i=39;i>=0;i--)
	{
		strTemp.Format (_T("SPC %02u"), i);
		TmpIdx = m_SpeeachLv.AddString (strTemp);
		m_SpeeachLv.SetItemData (TmpIdx, i);
	}

	return 0;
}

void CCommonScrollView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect Rect;

	GetClientRect (&Rect);

	dc.FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
}

VOID CCommonScrollView::SetSpeeachLevel (ULONG Level)
{
	m_SpeeachLv.SelectValue (Level);
}

VOID CCommonScrollView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	PUCHAR	p		= reinterpret_cast<PUCHAR>(pBuffer);

	switch (CIV)
	{
	case CIV_RF_POWER1:
		{
			DBG_MSG((_T("#################################################################\n")));
			DBG_MSG((_T("POWER%u %04X\n"), Trx, p[6] <<8 | p[7]));
		}
		break;

	case CIV_RF_POWER2:
		{
			DBG_MSG((_T("#################################################################\n")));
			DBG_MSG((_T("POWER%u %04X\n"), Trx, p[6] <<8 | p[7]));
			m_RfPower.SelectValue (p[6] <<8 | p[7]);
		}
		break;

	case CIV_ATTENUATOR:
		{
			m_Att.SetCustomBkIndex (p[5] != 0);
		}
		break;

	case CIV_DUAL_WATCH_MODE:
		{
			m_Dual.SetCustomBkIndex (p[7] != 0);
		}
		break;

	case CIV_MONITOR_MODE:
		{
			m_Monitor.SetCustomBkIndex (p[7] != 0);
		}
		break;

	case CIV_READ_ID:
		{
		}
		break;

	case CIV_MAIN_POWER:
		{
			m_Power.SetCustomBkIndex (p[5] != 0);
			
			m_Monitor.EnableWindow (p[5] != 0);
			m_Dual.EnableWindow (p[5] != 0);
			m_Att.EnableWindow (p[5] != 0);
			m_RfPower.EnableWindow (p[5] != 0);
			m_SpeeachLv.EnableWindow (p[5] != 0);
		}
		break;

	default:
		{
		}
		break;
	}
}

void CCommonScrollView::OnBtnClicked (UINT nID)
{
	DBG_MSG((_T("CCommonScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_BTN_MAIN_POWER:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_BTN_MAIN_POWER, !m_Power.GetCustomBkIndex ());
		}
		break;
	case IDC_BTN_MONITOR:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_BTN_MONITOR, !m_Monitor.GetCustomBkIndex ());
		}
		break;
	case IDC_BTN_DUAL:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_BTN_DUAL, !m_Dual.GetCustomBkIndex ());
		}
		break;
	case IDC_BTN_ATT:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_BTN_ATT, !m_Att.GetCustomBkIndex ());
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

void CCommonScrollView::OnCmbSelChange (UINT nID)
{
	DBG_MSG((_T("CCommonScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_CMB_RF_POWER:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_CMB_RF_POWER, m_RfPower.GetSelectedItemData ());
		}
		break;
	case IDC_CMB_SPEEACH_LEVEL:
		{
			AfxGetMainWnd ()->SendMessage (WM_COMMON_VIEW, IDC_CMB_SPEEACH_LEVEL, m_SpeeachLv.GetSelectedItemData ());
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////
// コンストラクション/デストラクション
//////////////////////////////////////////////////////////////////////

CCommonView::CCommonView()
{
}

CCommonView::~CCommonView()
{
}

BEGIN_MESSAGE_MAP(CCommonView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_ON_COMMON_MSG, &CCommonView::OnCommon)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonView メッセージ ハンドラー

int CCommonView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// ビューの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_View.Create (NULL, NULL, dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("クラス ビューを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	//m_wndListCtrl.Initialize ();
	////m_wndListCtrl.SetExtendedStyle (m_wndListCtrl.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_DOUBLEBUFFER);

	//m_wndListCtrl.InsertColumn (0, _T(""), 0, 800);

	return 0;
}

void CCommonView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CCommonView::OnContextMenu(CWnd* pWnd, CPoint point)
{
}

void CCommonView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect(rectClient);

	m_View.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CCommonView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CCommonView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_View.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CCommonView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_View.SetFocus();
}




LRESULT CCommonView::OnCommon (WPARAM wParam, LPARAM lParam)
{

	return 0;
}


VOID CCommonView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	m_View.OnCIV (Trx, CIV, pBuffer, Length);
}

VOID CCommonView::SetSpeeachLevel (ULONG Level)
{
	m_View.SetSpeeachLevel (Level);
}
