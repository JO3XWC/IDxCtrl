
#include "stdafx.h"
#include "MainFrm.h"
#include "CDigitalFreqView.h"
#include "Resource.h"
#include "IDControl.h"
#include "CDlgEdit.h"
#include "CDlgMyCallsign.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDigitalFreqScrollView, CScrollWnd)

CDigitalFreqScrollView::CDigitalFreqScrollView()
{
	m_LastTxMsg = -1;
	m_LastR1ComboGroup = -1;

}

CDigitalFreqScrollView::~CDigitalFreqScrollView()
{
}

BEGIN_MESSAGE_MAP(CDigitalFreqScrollView, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_START, IDC_BTN_MAX, &CDigitalFreqScrollView::OnBtnClicked)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CMB_START, IDC_CMB_MAX, &CDigitalFreqScrollView::OnCmbSelChange)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CS_CMB_START, IDC_CS_CMB_MAX, &CDigitalFreqScrollView::OnCsCmbSelChange)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CS_CMB_MENU_START, IDC_CS_CMB_MENU_MAX, &CDigitalFreqScrollView::OnCsMenuCmbSelChange)
	ON_NOTIFY(NM_CLICK, IDC_FREQ, &CDigitalFreqScrollView::OnFreqLClick)
	ON_NOTIFY(NM_RCLICK, IDC_FREQ, &CDigitalFreqScrollView::OnFreqRClick)
END_MESSAGE_MAP()


int CDigitalFreqScrollView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CScrollWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	INT		Height	= 20;
	INT		Height2	= 22;
	INT		Width	= 70;
	INT		LeftOffset;
	INT		TopOffset;
	INT		Space	= 10;
	CPoint	Pos;
	CRect	Rect;
	CString	strTemp;
	INT		TmpIdx;

	SetScrollSizes (MM_TEXT, CSize ((Width + Space) * 6, Height + Space));
	
	m_DC.Initialize (this, NULL, 100, Height, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR2, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);
	m_DC2.Initialize (this, NULL, 100, Height2, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR2, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);

	Pos.SetPoint (0, 0);	Rect.SetRect (Space + (Width + Space) * Pos.x, Space + (Height + Space) * Pos.y, Space + (Width + Space) * Pos.x + Width, Space + (Height + Space) * Pos.y + Height);
	m_Band.SetCustomDC (&m_DC);
	m_Band.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_Band.Create (_T("B BAND"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_BAND);

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
	//TmpIdx = m_Mode.AddString (_T("W-FM"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_WFM);
	TmpIdx = m_Mode.AddString (_T("FM-N"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_FMN);
	TmpIdx = m_Mode.AddString (_T("DV"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_DV);
	//TmpIdx = m_Mode.AddString (_T("DD"));	m_Mode.SetItemData (TmpIdx, OPERATING_MODE_DD);
	
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


	//コールサイン
	Height		= m_DC2.GetSize ().cy;
	Width		= 60;
	Space		= 10;
	TopOffset	= Rect.bottom;
	LeftOffset	= 0;

	Pos.SetPoint (0, 0);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_UrMenu.SetCustomDC (&m_DC2);
	m_UrMenu.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_UrMenu.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_UR_MENU);
	m_UrMenu.SetTextFormat (DT_RIGHT, DT_LEFT);
	m_UrMenu.SetTitle (_T("UR:"));
	m_UrMenu.AddString (_T("[EDIT]"));
	m_UrMenu.AddString (_T("[CLEAR]"));
	m_UrMenu.AddString (_T("CQCQCQ  "));

	Pos.SetPoint (0, 1);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_R1Menu.SetCustomDC (&m_DC2);
	m_R1Menu.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_R1Menu.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_R1_MENU);
	m_R1Menu.SetTextFormat (DT_RIGHT, DT_LEFT);
	m_R1Menu.SetTitle (_T("R1:"));
	m_R1Menu.AddString (_T("[EDIT]"));
	m_R1Menu.AddString (_T("[NOT USED]"));

	Pos.SetPoint (0, 2);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_R2Menu.SetCustomDC (&m_DC2);
	m_R2Menu.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_R2Menu.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_R2_MENU);
	m_R2Menu.SetTextFormat (DT_RIGHT, DT_LEFT);
	m_R2Menu.SetTitle (_T("R2:"));
	m_R2Menu.AddString (_T("[EDIT]"));
	m_R2Menu.AddString (_T("[NOT USED]"));

	Pos.SetPoint (0, 3);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_MyMenu.SetCustomDC (&m_DC2);
	m_MyMenu.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_MyMenu.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_MY_MENU);
	m_MyMenu.SetTextFormat (DT_RIGHT, DT_LEFT);
	m_MyMenu.SetTitle (_T("MY:"));

	Pos.SetPoint (0, 4);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_TxMsgMenu.SetCustomDC (&m_DC2);
	m_TxMsgMenu.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_TxMsgMenu.Create (CBS_DROPDOWNLIST	, Rect, this, IDC_CS_CMB_TX_MSG_MENU);
	m_TxMsgMenu.SetTextFormat (DT_RIGHT, DT_LEFT);
	m_TxMsgMenu.SetTitle (_T("MSG:"));


	//コールサイン
	Width	= 240;
	Space	= 10;
	LeftOffset = Rect.right;

	//CCustomComboBox		m_UrCombo;
	//CCustomComboBox		m_R1Combo;
	//CCustomComboBox		m_R2Combo;
	//CCustomComboBox		m_MyCombo;
	//CCustomComboBox		m_TxMsgCombo;

	Pos.SetPoint (0, 0);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_UrCombo.SetCustomDC (&m_DC2);
	m_UrCombo.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_UrCombo.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_UR);
	m_UrCombo.SetTextFormat (DT_LEFT, DT_LEFT);

	Pos.SetPoint (0, 1);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_R1Combo.SetCustomDC (&m_DC2);
	m_R1Combo.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_R1Combo.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_R1);
	m_R1Combo.SetTextFormat (DT_LEFT, DT_LEFT);

	Pos.SetPoint (0, 2);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_R2Combo.SetCustomDC (&m_DC2);
	m_R2Combo.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_R2Combo.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_R2);
	m_R2Combo.SetTextFormat (DT_LEFT, DT_LEFT);

	Pos.SetPoint (0, 3);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_MyCombo.SetCustomDC (&m_DC2);
	m_MyCombo.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_MyCombo.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_MY_CALLSIGN_MEMCH);
	m_MyCombo.SetTextFormat (DT_LEFT, DT_LEFT);
	m_MyCombo.AddString (_T("1 : "));
	m_MyCombo.AddString (_T("2 : "));
	m_MyCombo.AddString (_T("3 : "));
	m_MyCombo.AddString (_T("4 : "));
	m_MyCombo.AddString (_T("5 : "));
	m_MyCombo.AddString (_T("6 : "));

	Pos.SetPoint (0, 4);	Rect.SetRect (LeftOffset + Space + (Width + Space) * Pos.x, TopOffset + Space + (Height + Space) * Pos.y, LeftOffset + Space + (Width + Space) * Pos.x + Width, TopOffset + Space + (Height + Space) * Pos.y + Height);
	m_TxMsgCombo.SetCustomDC (&m_DC2);
	m_TxMsgCombo.SetCustomCursor (::LoadCursor (NULL, IDC_HAND));
	m_TxMsgCombo.Create (CBS_DROPDOWNLIST		, Rect, this, IDC_CS_CMB_TX_MSG_MEMCH);
	m_TxMsgCombo.SetTextFormat (DT_LEFT, DT_LEFT);

	ResetTxMsg ();
	ResetMyCallsign ();

	return 0;
}

void CDigitalFreqScrollView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect Rect;

	GetClientRect (&Rect);

	dc.FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
}



void CDigitalFreqScrollView::ResetTxMsg ()
{
	CString strMsg;

	for (int i=0;i<5;i++)
	{
		m_TxMsgArray.SetAtGrow (i, _T(""));
	}

	UpdateTxMsg();
}

VOID CDigitalFreqScrollView::SetTxMsg (INT Index, LPCTSTR pszMsg)
{
	m_TxMsgArray.SetAtGrow (Index, pszMsg);

	UpdateTxMsg ();
}

void CDigitalFreqScrollView::UpdateTxMsg ()
{
	INT	SelIndex = m_TxMsgCombo.GetCurSel ();
	CString strMsg;


	m_TxMsgCombo.ResetContent ();
	
	for (int i=0;i<m_TxMsgArray.GetCount ();i++)
	{
		strMsg.Format (_T("%u : %s"), i + 1, m_TxMsgArray.GetAt (i).GetString ());
		m_TxMsgCombo.AddString (strMsg);
	}

	m_TxMsgCombo.SetCurSel (SelIndex);


	//----------------------------
	SelIndex = m_TxMsgMenu.GetCurSel ();

	m_TxMsgMenu.ResetContent ();
	m_TxMsgMenu.AddString (_T("OFF"));
	m_TxMsgMenu.AddString (_T("ON"));
	
	for (int i=0;i<m_TxMsgArray.GetCount ();i++)
	{
		strMsg.Format (_T("[EDIT] %u : %s"), i + 1, m_TxMsgArray.GetAt (i).GetString ());
		m_TxMsgMenu.AddString (strMsg);
	}

	m_TxMsgMenu.SetCurSel (SelIndex);
}


void CDigitalFreqScrollView::ResetMyCallsign ()
{
	CString strMsg;

	for (int i=0;i<6;i++)
	{
		m_MyCallsignArray.SetAtGrow (i, _T(""));
	}

	UpdateMyCallsign();
}

VOID CDigitalFreqScrollView::SetMyCallsign (INT Index, LPCTSTR pszMsg)
{
	m_MyCallsignArray.SetAtGrow (Index, pszMsg);

	UpdateMyCallsign ();
}

void CDigitalFreqScrollView::UpdateMyCallsign()
{
	INT	SelIndex = m_MyCombo.GetCurSel ();
	CString strCall;


	m_MyCombo.ResetContent ();
	
	for (int i=0;i<m_MyCallsignArray.GetCount ();i++)
	{
		strCall.Format (_T("%u : %s"), i + 1, m_MyCallsignArray.GetAt (i).GetString ());
		m_MyCombo.AddString (strCall);
	}

	m_MyCombo.SetCurSel (SelIndex);


	//------------------------------------------
	SelIndex = m_MyMenu.GetCurSel ();

	m_MyMenu.ResetContent ();
	
	for (int i=0;i<m_MyCallsignArray.GetCount ();i++)
	{
		strCall.Format (_T("[EDIT] %u : %s"), i + 1, m_MyCallsignArray.GetAt (i).GetString ());
		m_MyMenu.AddString (strCall);
	}

	m_MyMenu.SetCurSel (SelIndex);
}

void CDigitalFreqScrollView::UpdateRxCs()
{
	CStringArray Array;

	m_UrCombo.ResetContent ();

	theApp.GetRxCs (&Array);

	for (int i=0;i<Array.GetCount ();i++)
	{
		m_UrCombo.AddString (Array.GetAt (i));
	}
}

void CDigitalFreqScrollView::OnRepeaterListChange()
{
	POSITION			Pos;
	CRepeater*			pRepeater;
	CList<CRepeater*>*	pList = &theApp.GetRepeaterList ()->m_List;
	//CArray<CArray<CRepeater*>> m_RepeaterArray;
	CArray<CRepeater*>	Array;


	for (int i=0;i<_countof (m_RepeaterArray);i++)
	{
		m_RepeaterArray[i].RemoveAll ();
	}

	Pos = pList->GetHeadPosition ();
	while (Pos != NULL)
	{
		pRepeater = pList->GetNext (Pos);

		m_RepeaterArray[pRepeater->GetGroup ()].Add (pRepeater);
	}



	m_R1Menu.ResetContent ();
	m_R1Menu.AddString (_T("[EDIT]"));
	m_R1Menu.AddString (_T("[NOT USED]"));

	for (int i=0;i<_countof (m_RepeaterArray);i++)
	{
		if (m_RepeaterArray[i].IsEmpty ())
		{	continue;
		}

		pRepeater = m_RepeaterArray[i].GetAt (0);
		INT ItemIdx = m_R1Menu.AddString (pRepeater->GetGroupName ());
		m_R1Menu.SetItemData (ItemIdx, i);
	}

	m_R1Combo.ResetContent ();
	m_LastR1ComboGroup = -1;
}
	
VOID CDigitalFreqScrollView::RepeaterSelectChange (CRepeater* pRepeater)
{
	CRepeater*	pRepeater2;
	CRepeater*	pRepeater3;
	INT			RptIndex;
	CString		strTemp;

	//FIND R1COMBO
	for(int i=0;i<m_R1Combo.GetCount ();i++)
	{
		pRepeater2 = reinterpret_cast<CRepeater*>(m_R1Combo.GetItemData (i));
		if (lstrcmp (pRepeater2->GetCallsign (), pRepeater->GetCallsign ()) == 0)
		{
			m_R1Combo.SetCurSel (i);
			return;
		}
	}

	m_LastR1ComboGroup = -1;

	//FIND R1MENU
	for(int i=1;i<m_R1Menu.GetCount ();i++)//0=EDIT skip
	{
		INT ArrayIdx = static_cast<INT>(m_R1Menu.GetItemData (i));

		if(m_RepeaterArray[ArrayIdx].IsEmpty ())
		{
			continue;
		}

		pRepeater2 = m_RepeaterArray[ArrayIdx].GetAt (0);

		if(pRepeater2->GetGroup () == pRepeater->GetGroup ())
		{
			if(m_R1Menu.GetCurSel () != i)
			{
				m_R1Menu.SetCurSel (i);
				m_LastR1ComboGroup = i;

				RptIndex = static_cast<INT>(m_R1Menu.GetItemData (i));

				m_R1Combo.ResetContent ();

				for (int j=0;j<m_RepeaterArray[RptIndex].GetCount ();j++)
				{
					pRepeater3 = m_RepeaterArray[RptIndex].GetAt (j);

					if (!pRepeater3->GetRpt1Use ())
					{	continue;
					}

					strTemp.Format (_T("%s (%s)"), pRepeater3->GetCallsign (), pRepeater3->GetName ());
					INT ItemIdx = m_R1Combo.AddString (strTemp);
					m_R1Combo.SetItemData(ItemIdx, reinterpret_cast<DWORD_PTR>(pRepeater3));

					if (pRepeater == pRepeater3)
					{
						m_R1Combo.SetCurSel (ItemIdx);
					}
				}
			}

			break;
		}
	}


	

}

VOID CDigitalFreqScrollView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	PUCHAR	p		= reinterpret_cast<PUCHAR>(pBuffer);
	PCHAR	pC		= reinterpret_cast<PCHAR>(pBuffer);

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

			m_UrMenu.EnableWindow (p[5] != 0);
			m_R1Menu.EnableWindow (p[5] != 0);
			m_R2Menu.EnableWindow (p[5] != 0);
			m_MyMenu.EnableWindow (p[5] != 0);
			m_TxMsgMenu.EnableWindow (p[5] != 0);
			
			m_UrCombo.EnableWindow (p[5] != 0);
			m_R1Combo.EnableWindow (p[5] != 0);
			m_R2Combo.EnableWindow (p[5] != 0);
			m_MyCombo.EnableWindow (p[5] != 0);
			m_TxMsgCombo.EnableWindow (p[5] != 0);
		}
		break;

	case CIV_MAIN_BAND_SELECTION:
		{
			m_Band.SetCustomBkIndex (p[7] != 0 ? 1 : 0);
			m_FreqCtrl.SelectBand (p[7] != 0 ? 1 : 0);
			m_SMeterCtrl.SelectBand (p[7] != 0 ? 1 : 0);
		}
		break;

	case CIV_SQL_LEVEL2:
		{
			m_Sql.SelectValue (p[6]);
		}
		break;

	case CIV_AF_VOLUME2:
		{
			m_Vol.SelectValue (p[6] << 8 | p[7]);
		}
		break;

	case CIV_VFO_MR_CALL_SELECTION2:
		{
			m_Vfo.SelectValue (p[7]);
		}
		break;

	case CIV_READ_FREQUENCY2:
		{
			ULONGLONG Freq = SWAPBCD2ULONGLONG (p + 5, 5);
			m_FreqCtrl.SetFreq (Freq);
		}
		break;

	case CIV_DUPLEX_MODE2:
		{
			m_Dup.SelectValue (p[5]);
			m_FreqCtrl.SetDupMode (p[5]);
		}
		break;

	case CIV_READ_DUPLEX_OFFSET2:
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

	case CIV_READ_OPERATING_MODE2:
		{
			m_Mode.SelectValue (p[5] | p[6] << 8);
		}
		break;

	case CIV_SIGNAL_LEVEL2:
		{
			ULONG Level = (ULONG)BCD2ULONGLONG (p + 6, 2);
			m_SMeterCtrl.SetLevel (Level);
		}
		break;

	case CIV_NOISE_SQL2:
		{
			m_SMeterCtrl.SetNoiseSql ( p[6]);
		}
		break;

		//TX MESSAGE
	case CIV_DV_TX_MESSAGE1:
	case CIV_DV_TX_MESSAGE2:
	case CIV_DV_TX_MESSAGE3:
	case CIV_DV_TX_MESSAGE4:
	case CIV_DV_TX_MESSAGE5:
		{
			CString strMsg = DATA2STR (p + 7, 20); 
			SetTxMsg (p[6] - 1, strMsg);
		}
		break;

	case CIV_DV_TX_MESSAGE_MEMCH:
		{
			m_TxMsgCombo.SetCurSel (p[6] - 1);
		}
		break;

	case CIV_DV_TX_MESSAGE_ENABLE:
		{
			m_TxMsgCombo.EnableWindow(p[6] != 0x00);

			m_LastTxMsg = (p[6] == 0) ? 0 : 1;
			m_TxMsgMenu.SetCurSel (m_LastTxMsg);
		}
		break;

	case CIV_DV_MY_CALLSIGN1:
	case CIV_DV_MY_CALLSIGN2:
	case CIV_DV_MY_CALLSIGN3:
	case CIV_DV_MY_CALLSIGN4:
	case CIV_DV_MY_CALLSIGN5:
	case CIV_DV_MY_CALLSIGN6:
		{
			CString strCall = DATA2STR (p + 7, 8);
			CString strMemo = DATA2STR (p + 15, 4);
			
			SetMyCallsign (p[6] - 1, strCall + _T(" / ") + strMemo);
		}
		break;

	case CIV_DV_MY_CALLSIGN_MEMCH:
		{
			m_MyCombo.SetCurSel (p[6] - 1);
		}
		break;

	case CIV_DV_TX_CALLSIGN:
		{
			CString				strRpt2;
			CString				strRpt1;
			CString				strUr;
			CRepeater*			pRepeater;
			CMapStringToPtr*	pMap = &theApp.GetRepeaterList ()->m_Map;
			CMapStringToPtr*	pToMap = &theApp.GetRepeaterList ()->m_ToMap;

			strRpt2	= DATA2STR (pC + 6, 8);
			strRpt1 = DATA2STR (pC + 14, 8);
			strUr	= DATA2STR (pC + 22, 8);
			
			if (pMap->Lookup (strUr, (void*&)pRepeater))
			{
				strUr.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
			}
			else if (pToMap->Lookup (strUr, (void*&)pRepeater))
			{
				strUr.Format (_T("%s (%s)"), pRepeater->GetToCallsign (), pRepeater->GetName ());
			}
			
			if (pMap->Lookup (strRpt1, (void*&)pRepeater))
			{
				strRpt1.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
			
				RepeaterSelectChange (pRepeater);
	
				//CString strFreq;
				//strFreq.Format (_T("%I64u"), pRepeater->GetFrequency ());
				//if (pMap->Lookup (strFreq, (void*&)pRepeater))
				//{
				//	RepeaterSelectChange (pRepeater);
				//}
			}

			
			if (pMap->Lookup (strRpt2, (void*&)pRepeater))
			{
				strRpt2.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
			}


			m_UrCombo.SetTitle (strUr);
			m_R1Combo.SetTitle (strRpt1);
			m_R2Combo.SetTitle (strRpt2);
		}
		break;


	default:
		{
		}
		break;
	}
}

void CDigitalFreqScrollView::OnBtnClicked (UINT nID)
{
	DBG_MSG((_T("CDigitalFreqScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_BTN_BAND:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_BTN_BAND, 1);
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

void CDigitalFreqScrollView::OnCmbSelChange (UINT nID)
{
	DBG_MSG((_T("CDigitalFreqScrollView::OnBtnClicked - nID=%u\n"), nID));
	switch (nID)
	{
	case IDC_CMB_SQL:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CMB_SQL, m_Sql.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_VOL:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CMB_VOL, m_Vol.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_VFO:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CMB_VFO, m_Vfo.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_DUP:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CMB_DUP, m_Dup.GetSelectedItemData ());
		}
		break;

	case IDC_CMB_MODE:
		{
			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CMB_MODE, m_Mode.GetSelectedItemData ());
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

void CDigitalFreqScrollView::OnCsCmbSelChange (UINT nID)
{
	INT				Index;
	CRepeater*		pRepeater;
	CString			strCallSign;

	DBG_MSG((_T("CDigitalFreqScrollView::OnCsCmbSelChange - nID=%u\n"), nID));
	
	switch (nID)
	{
	case IDC_CS_CMB_UR:
		{
			Index = m_UrCombo.GetCurSel ();
			if (Index == CB_ERR)
			{	break;
			}
			m_UrCombo.SetCurSel (-1);
			m_UrCombo.GetLBText (Index, strCallSign);
			AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_UR_CALLSIGN, reinterpret_cast<WPARAM>(strCallSign.GetString ()));
		}
		break;

	case IDC_CS_CMB_R1:
		{
			Index = m_R1Combo.GetCurSel ();
			if (Index == CB_ERR)
			{	break;
			}

			pRepeater = reinterpret_cast<CRepeater*>(m_R1Combo.GetItemData (Index));
		
			AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_TOFROM_CALLSIGN, reinterpret_cast<WPARAM>(pRepeater));
		}
		break;

	case IDC_CS_CMB_R2:
		{
			ASSERT (FALSE);
		}
		break;

	case IDC_CS_CMB_MY_CALLSIGN_MEMCH:
		{
			Index = m_MyCombo.GetCurSel ();
			switch (Index)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				{
					AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CS_CMB_MY_CALLSIGN_MEMCH, Index + 1);
				}
				break;
			}
		}
		break;

	case IDC_CS_CMB_TX_MSG_MEMCH:
		{
			Index = m_TxMsgCombo.GetCurSel ();
			switch (Index)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				{
					AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CS_CMB_TX_MSG_MEMCH, Index + 1);
				}
				break;
			}
		}
		break;



	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}


void CDigitalFreqScrollView::OnCsMenuCmbSelChange (UINT nID)
{
	INT				Index;
	CRepeater*		pRepeater = NULL;
	CString			strTemp;
	CString			strTemp2;
	CString			strCall;
	CString			strUr;
	CString			strR1;
	CString			strR2;
	CString			strMemo;
	CDlgEdit		Dlg;

	//DBG_MSG((_T("CDigitalFreqScrollView::OnCsMenuCmbSelChange - nID=%u\n")));
	Index = m_R1Combo.GetCurSel ();
	if (Index != CB_ERR)
	{
		pRepeater = reinterpret_cast<CRepeater*>(m_R1Combo.GetItemData (Index));
	}


	switch (nID)
	{
	case IDC_CS_CMB_UR_MENU:
		{
			Index = m_UrMenu.GetCurSel ();
			if (Index == CB_ERR)
			{	break;
			}

			m_UrMenu.SetCurSel (-1);

			strUr = m_UrCombo.GetTitle ();
			strR1 = m_R1Combo.GetTitle ();
			strR2 = m_R2Combo.GetTitle ();

			strUr = strUr.Left (8);
			strR1 = strR1.Left (8);
			strR2 = strR2.Left (8);

			switch (Index)
			{
			case 0://EDIT
				{
					Dlg.SetText (strUr, 8);
					if (Dlg.DoModal () != IDOK)
					{	break;
					}
					strUr.Format (_T("%- 8s"), Dlg.GetText ());
					strUr = strUr.Left (8);

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_UR_CALLSIGN, reinterpret_cast<WPARAM>(strUr.GetString ()));
				}
				break;

			case 1:
				{
					strUr = _T("        ");

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_UR_CALLSIGN, reinterpret_cast<WPARAM>(strUr.GetString ()));
				}
				break;

			case 2:
				{
					strUr = _T("CQCQCQ  ");

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_UR_CALLSIGN, reinterpret_cast<WPARAM>(strUr.GetString ()));
				}
				break;

			default:
				{
					ASSERT (FALSE);
				}
				break;
			}
		}
		break;

	case IDC_CS_CMB_R1_MENU:
		{
			Index = m_R1Menu.GetCurSel ();
			if (Index == CB_ERR)
			{	break;
			}

			switch (Index)
			{
			case 0://EDIT
				{
					m_R1Menu.SetCurSel (m_LastR1ComboGroup);

					strR1 = m_R1Combo.GetTitle ();
					strR1 = strR1.Left (8);
					Dlg.SetText (strR1, 8);
					if (Dlg.DoModal () != IDOK)
					{	break;
					}
					strR1.Format (_T("%- 8s"), Dlg.GetText ());
					strR1 = strR1.Left (8);

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_R1_CALLSIGN, reinterpret_cast<LPARAM>(strR1.GetString ()));
				}
				break;

			case 1:
				{
					m_R1Menu.SetCurSel (m_LastR1ComboGroup);

					strR1 = _T("        ");

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_R1_CALLSIGN, reinterpret_cast<LPARAM>(strR1.GetString ()));
				}
				break;

			default:
				{
					m_R1Combo.ResetContent ();
					m_LastR1ComboGroup = Index;

					Index = static_cast<INT>(m_R1Menu.GetItemData (Index));
					for (int i=0;i<m_RepeaterArray[Index].GetCount ();i++)
					{
						pRepeater = m_RepeaterArray[Index].GetAt (i);

						if (!pRepeater->GetRpt1Use ())
						{	continue;
						}

						strTemp.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
						INT ItemIdx = m_R1Combo.AddString (strTemp);
						m_R1Combo.SetItemData(ItemIdx, reinterpret_cast<DWORD_PTR>(pRepeater));
					}
				}
				break;
			}
		}
		break;

	case IDC_CS_CMB_R2_MENU:
		{
			Index = m_R2Menu.GetCurSel ();
			if (Index == CB_ERR)
			{	break;
			}

			m_R2Menu.SetCurSel (-1);

			switch (Index)
			{
			case 0://EDIT
				{
					strR2 = m_R2Combo.GetTitle ();
					strR2 = strR2.Left (8);

					Dlg.SetText (strR2, 8);
					if (Dlg.DoModal () != IDOK)
					{	break;
					}
					strR2.Format (_T("%- 8s"), Dlg.GetText ());
					strR2 = strR2.Left (8);

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_R2_CALLSIGN, reinterpret_cast<LPARAM>(strR2.GetString ()));
				}
				break;

			case 1:
				{
					strR2 = _T("        ");

					AfxGetMainWnd ()->SendMessage (WM_SET_CALLSIGN, CMainFrame::SET_R2_CALLSIGN, reinterpret_cast<LPARAM>(strR2.GetString ()));
				}
				break;

			default:
				{
					ASSERT (FALSE);
				}
				break;
			}
		}
		break;

	case IDC_CS_CMB_MY_MENU:
		{
			//EDIT
			Index = m_MyMenu.GetCurSel ();

			m_MyMenu.SetCurSel (-1);


			if ((Index >= 0) && (Index <= 5))
			{
				m_MyCombo.GetLBText (Index, strTemp);
				strTemp = strTemp.Mid (4);
				strCall = strTemp.Left (8);
				strMemo = strTemp.Mid (8 + 3, 4);

				CDlgMyCallsign DlgCall;

				DlgCall.SetCallsign (strCall, 8);
				DlgCall.SetMemo (strMemo, 4);

				if (DlgCall.DoModal () != IDOK)
				{	break;
				}
				strCall.Format (_T("%- 8s"), DlgCall.GetCallsign ());
				strCall = strCall.Left (8);
				strMemo.Format (_T("%- 4s"), DlgCall.GetMemo ());
				strMemo = strMemo.Left (4);
			}

			strTemp.Format(_T("%s%s"), strCall.GetString (), strMemo.GetString ());

			AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CS_CMB_MY_MENU_CALL1 + Index, reinterpret_cast<LPARAM>(strTemp.GetString ()));
		}
		break;

	case IDC_CS_CMB_TX_MSG_MENU:
		{
			Index = m_TxMsgMenu.GetCurSel ();

			switch (Index)
			{
			case 0://OFF
			case 1://ON
				{
					AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CS_CMB_TX_MSG_MENU, Index);
				}
				break;

			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				{
					//EDIT
					Index -= 2;

					m_TxMsgMenu.SetCurSel (m_LastTxMsg);

					m_TxMsgCombo.GetLBText (Index, strTemp);
					strTemp = strTemp.Mid (4);

					Dlg.SetText (strTemp, 20);
					if (Dlg.DoModal () != IDOK)
					{	break;
					}
					strTemp.Format (_T("%- 20s"), Dlg.GetText ());
					strTemp = strTemp.Left (20);

					AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_CS_CMB_TX_MSG_MENU_TXMSG1 + Index, reinterpret_cast<LPARAM>(strTemp.GetString ()));

				}
				break;

			default:
				{
					ASSERT (FALSE);
				}
				break;
			}
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}


void CDigitalFreqScrollView::OnFreqLClick(NMHDR* pNmHdr, LRESULT* pResult)
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
		AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_FREQ, reinterpret_cast<LPARAM>(&Freq));
	}

	*pResult = 0;
}
void CDigitalFreqScrollView::OnFreqRClick(NMHDR* pNmHdr, LRESULT* pResult)
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
		AfxGetMainWnd ()->SendMessage (WM_DIGITAL_FREQ_VIEW, IDC_FREQ, reinterpret_cast<LPARAM>(&Freq));
	}

	*pResult = 0;
}












//////////////////////////////////////////////////////////////////////
// コンストラクション/デストラクション
//////////////////////////////////////////////////////////////////////

CDigitalFreqView::CDigitalFreqView()
{
}

CDigitalFreqView::~CDigitalFreqView()
{
}

BEGIN_MESSAGE_MAP(CDigitalFreqView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitalFreqView メッセージ ハンドラー

int CDigitalFreqView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	return 0;
}

void CDigitalFreqView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CDigitalFreqView::OnContextMenu(CWnd* pWnd, CPoint point)
{
}

void CDigitalFreqView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect(rectClient);

	m_View.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CDigitalFreqView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CDigitalFreqView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_View.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CDigitalFreqView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_View.SetFocus();
}



VOID CDigitalFreqView::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	m_View.OnCIV (Trx, CIV, pBuffer, Length);
}



void CDigitalFreqView::OnRepeaterListChange()
{
	m_View.OnRepeaterListChange ();
}

void CDigitalFreqView::UpdateRxCs()
{
	m_View.UpdateRxCs ();
}
