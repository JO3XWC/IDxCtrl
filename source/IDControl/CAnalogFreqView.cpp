
#include "stdafx.h"
#include "MainFrm.h"
#include "CAnalogFreqView.h"
#include "Resource.h"
#include "IDControl.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAnalogFreqScrollView, CScrollWnd)

CAnalogFreqScrollView::CAnalogFreqScrollView()
{

}

CAnalogFreqScrollView::~CAnalogFreqScrollView()
{
}

BEGIN_MESSAGE_MAP(CAnalogFreqScrollView, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_START, IDC_BTN_MAX, &CAnalogFreqScrollView::OnBtnClicked)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CMB_START, IDC_CMB_MAX, &CAnalogFreqScrollView::OnCmbSelChange)
	ON_NOTIFY(NM_CLICK, IDC_FREQ, &CAnalogFreqScrollView::OnFreqLClick)
	ON_NOTIFY(NM_RCLICK, IDC_FREQ, &CAnalogFreqScrollView::OnFreqRClick)
END_MESSAGE_MAP()


int CAnalogFreqScrollView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	m_Band.SetCustomDC (&m_DC);
	m_Band.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Band.Create (_T("A BAND"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_BAND);

	Pos.SetPoint (1, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Sql.SetCustomDC (&m_DC);
	m_Sql.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_Sql.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_SQL);
	m_Sql.SetTextFormat (DT_CENTER, DT_CENTER);
	for (int i=SQL_LEVEL_MAX;i>=SQL_LEVEL1;i--)
	{
		strTemp.Format (_T("SQL LV%u"), i - 1);
		TmpIdx = m_Sql.AddString (strTemp);	m_Sql.SetItemData (TmpIdx, i);
	}
	TmpIdx = m_Sql.AddString (_T("SQL AT"));	m_Sql.SetItemData (TmpIdx, SQL_AUTO);
	TmpIdx = m_Sql.AddString (_T("SQL OP"));	m_Sql.SetItemData (TmpIdx, SQL_OPEN);

	Pos.SetPoint (2, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Vol.SetCustomDC (&m_DC);
	m_Vol.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_Vol.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_VOL);
	m_Vol.SetTextFormat (DT_CENTER, DT_CENTER);
	for (int i=VOL_LEVEL_MAX;i>=VOL_LEVEL1;i--)
	{
		USHORT Level;
		strTemp.Format (_T("VOL %u"), i);
		ULONGLONG2SWAPBCD (i, &Level, sizeof (Level));
		TmpIdx = m_Vol.AddString (strTemp);	m_Vol.SetItemData (TmpIdx, Level);
	}

	Pos.SetPoint (3, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Vfo.SetCustomDC (&m_DC);
	m_Vfo.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_Vfo.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_VFO);
	m_Vfo.SetTextFormat (DT_CENTER, DT_CENTER);
	TmpIdx = m_Vfo.AddString (_T("VFO"));	m_Vfo.SetItemData (TmpIdx, SELECT_VFO);
	TmpIdx = m_Vfo.AddString (_T("MR1"));	m_Vfo.SetItemData (TmpIdx, SELECT_MR1);
	TmpIdx = m_Vfo.AddString (_T("MR2"));	m_Vfo.SetItemData (TmpIdx, SELECT_MR2);
	TmpIdx = m_Vfo.AddString (_T("CALL"));	m_Vfo.SetItemData (TmpIdx, SELECT_CALL);

	Pos.SetPoint (4, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Dup.SetCustomDC (&m_DC);
	m_Dup.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_Dup.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_DUP);
	m_Dup.SetTextFormat (DT_CENTER, DT_CENTER);
	TmpIdx = m_Dup.AddString (_T("SMPL"));	m_Dup.SetItemData (TmpIdx, DUP_MODE_SIMPLEXT);
	TmpIdx = m_Dup.AddString (_T("DUP-"));	m_Dup.SetItemData (TmpIdx, DUP_MODE_MINUS);
	TmpIdx = m_Dup.AddString (_T("DUP+"));	m_Dup.SetItemData (TmpIdx, DUP_MODE_PLUS);

	Pos.SetPoint (5, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Mode.SetCustomDC (&m_DC);
	m_Mode.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_Mode.Create (CBS_DROPDOWNLIST, Rect, this, IDC_CMB_MODE);
	m_Mode.SetTextFormat (DT_CENTER, DT_CENTER);
	TmpIdx = m_Mode.AddString (_T("AUTO"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_AUTO);
	TmpIdx = m_Mode.AddString (_T("AM"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_AM);
	TmpIdx = m_Mode.AddString (_T("FM"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_FM);
	TmpIdx = m_Mode.AddString (_T("W-FM"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_WFM);
	//TmpIdx = m_Mode.AddString (_T("FM-N"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_FMN);
	//TmpIdx = m_Mode.AddString (_T("OPMODE DV"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_DV);
	//TmpIdx = m_Mode.AddString (_T("OPMODE DD"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_DD);
	
	Pos.SetPoint (0, 1);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	Rect.bottom = Rect.top + 27;
	Rect.right = Rect.left + 272;

	m_SMeterCtrl.Create (NULL, NULL, WS_CHILD | WS_VISIBLE, Rect, this, IDC_S_METER);

	Pos.SetPoint (0, 2);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	Rect.bottom = Rect.top + 31;
	Rect.left += 16;
	Rect.right = Rect.left + 22*10;

	m_FreqCtrl.SetCustomCursor (theApp.LoadCursor (IDC_TOP_AND_BOTTOM_CURSOR));
	m_FreqCtrl.Create (NULL, NULL, WS_CHILD | WS_VISIBLE, Rect, this, IDC_FREQ);


	return 0;
}

void CAnalogFreqScrollView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect Rect;

	GetClientRect (&Rect);

	dc.FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
}


VOID CAnalogFreqScrollView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	PUCHAR	p		= reinterpret_cast<PUCHAR>(pBuffer);

	switch (CIV)
	{
	case CIV_MAIN_POWER:
		{
			m_Band.EnableWindow (p[5] != 0);
			m_Vol.EnableWindow (p[5] != 0);
			m_Sql.EnableWindow (p[5] != 0);
			m_Vfo.EnableWindow (p[5] != 0);
			m_Dup.EnableWindow (p[5] != 0);
			m_Mode.EnableWindow (p[5] != 0);
			m_FreqCtrl.EnableWindow (p[5] != 0);
			m_FreqCtrl.RedrawWindow ();
			m_SMeterCtrl.EnableWindow (p[5] != 0);
			m_SMeterCtrl.RedrawWindow ();
		}
		break;

	case CIV_MAIN_BAND_SELECTION:
		{
			m_Band.SetCustomBkIndex (p[7] == 0 ? 1 : 0);
			m_FreqCtrl.SelectBand (p[7] == 0 ? 1 : 0);
			m_SMeterCtrl.SelectBand (p[7] == 0 ? 1 : 0);
		}
		break;

	case CIV_SQL_LEVEL1:
		{
			m_Sql.SelectValue (p[6]);
		}
		break;

	case CIV_AF_VOLUME1:
		{
			m_Vol.SelectValue (p[6] << 8 | p[7]);
		}
		break;

	case CIV_VFO_MR_CALL_SELECTION1:
		{
			m_Vfo.SelectValue (p[7]);
		}
		break;

	case CIV_READ_FREQUENCY1:
		{
			ULONGLONG Freq = SWAPBCD2ULONGLONG (p + 5, 5);
			m_FreqCtrl.SetFreq (Freq);
		}
		break;

	case CIV_DUPLEX_MODE1:
		{
			m_Dup.SelectValue (p[5]);
			m_FreqCtrl.SetDupMode (p[5]);
		}
		break;

	case CIV_READ_DUPLEX_OFFSET1:
		{
			ULONGLONG Offset = SWAPBCD2ULONGLONG (p + 5, 5);
			m_FreqCtrl.SetDupOffset (Offset);
		}
		break;

	case CIV_PTT_STATUS:
		{
			m_FreqCtrl.SetPttStatus (p[6] != 0x00);
		}
		break;

	case CIV_MONITOR_MODE:
		{
			if (m_Band.GetCustomBkIndex () == 1)
			{
				m_FreqCtrl.SetMonitor (p[7] != 0x00);
				m_SMeterCtrl.SetMonitor (p[7] != 0x00);
			}
		}
		break;

	case CIV_READ_OPERATING_MODE1:
		{
			m_Mode.SelectValue (p[5] | p[6] << 8);
		}
		break;

	case CIV_SIGNAL_LEVEL1:
		{
			ULONG Level = (ULONG)BCD2ULONGLONG (p + 6, 2);
			m_SMeterCtrl.SetLevel (Level);
		}
		break;

	case CIV_NOISE_SQL1:
		{
			m_SMeterCtrl.SetNoiseSql ( p[6]);
		}
		break;

	default:
		{
		}
		break;
	}
}

void CAnalogFreqScrollView::OnBtnClicked (UINT nID)
{
	DBG_MSG((_T("CAnalogFreqScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_BTN_BAND:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_BTN_BAND, 0);
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

void CAnalogFreqScrollView::OnCmbSelChange (UINT nID)
{
	DBG_MSG((_T("CAnalogFreqScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_CMB_SQL:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_CMB_SQL, m_Sql.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_VOL:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_CMB_VOL, m_Vol.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_VFO:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_CMB_VFO, m_Vfo.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_DUP:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_CMB_DUP, m_Dup.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_MODE:
		{
			AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_CMB_MODE, m_Mode.GetSelectedItemData ());
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

void CAnalogFreqScrollView::OnFreqLClick(NMHDR* pNmHdr, LRESULT* pResult)
{
	CPoint		Pos		= GetMessagePos ();
	ULONGLONG	Freq	= m_FreqCtrl.GetFreq ();
	INT			Index;	

	m_FreqCtrl.ScreenToClient (&Pos);
	
	Index = Pos.x / 22;	

	switch (Index)
	{
	case 0:	{	Freq -= 100000000ll;	}	break;	//100MHz
	case 1:	{	Freq -=  10000000ll;	}	break;	// 10MHz
	case 2:	{	Freq -=   1000000ll;	}	break;	// 1MHz
	case 4:	{	Freq -=    100000ll;	}	break;	//100KHz
	case 5:	{	Freq -=     10000ll;	}	break;	// 10KHz
	case 6:	{	Freq -=      5000ll;	}	break;	//  1KHz
//	case 8:	{	Freq -=       100ll;	}	break;	//100Hz
//	case 9:	{	Freq -=        10ll;	}	break;	// 10Hz
//	case 10:{	Freq -=         1ll;	}	break;	//  1Hz
	}

	if (Freq != m_FreqCtrl.GetFreq ())
	{
		AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_FREQ, reinterpret_cast<LPARAM>(&Freq));
	}

	*pResult = 0;
}
void CAnalogFreqScrollView::OnFreqRClick(NMHDR* pNmHdr, LRESULT* pResult)
{
	CPoint		Pos		= GetMessagePos ();
	ULONGLONG	Freq	= m_FreqCtrl.GetFreq ();
	INT			Index;	

	m_FreqCtrl.ScreenToClient (&Pos);
	
	Index = Pos.x / 22;	

	switch (Index)
	{
	case 0:	{	Freq += 100000000ll;	}	break;	//100MHz
	case 1:	{	Freq +=  10000000ll;	}	break;	// 10MHz
	case 2:	{	Freq +=   1000000ll;	}	break;	// 1MHz
	case 4:	{	Freq +=    100000ll;	}	break;	//100KHz
	case 5:	{	Freq +=     10000ll;	}	break;	// 10KHz
	case 6:	{	Freq +=      5000ll;	}	break;	//  1KHz
//	case 8:	{	Freq +=       100ll;	}	break;	//100Hz
//	case 9:	{	Freq +=        10ll;	}	break;	// 10Hz
//	case 10:{	Freq +=         1ll;	}	break;	//  1Hz
	}

	if (Freq != m_FreqCtrl.GetFreq ())
	{
		AfxGetMainWnd ()->SendMessage (WM_ANALOG_FREQ_VIEW, IDC_FREQ, reinterpret_cast<LPARAM>(&Freq));
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////
// コンストラクション/デストラクション
//////////////////////////////////////////////////////////////////////

CAnalogFreqView::CAnalogFreqView()
{
}

CAnalogFreqView::~CAnalogFreqView()
{
}

BEGIN_MESSAGE_MAP(CAnalogFreqView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalogFreqView メッセージ ハンドラー

int CAnalogFreqView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CAnalogFreqView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CAnalogFreqView::OnContextMenu(CWnd* pWnd, CPoint point)
{
}

void CAnalogFreqView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect(rectClient);

	m_View.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CAnalogFreqView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CAnalogFreqView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_View.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CAnalogFreqView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_View.SetFocus();
}



VOID CAnalogFreqView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	m_View.OnCIV (Trx, CIV, pBuffer, Length);
}


