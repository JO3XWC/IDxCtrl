
#include "stdafx.h"
#include "MainFrm.h"
#include "CCallsignHistoryView.h"
#include "Resource.h"
#include "IDControl.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static UINT WM_ON_HISTORY = ::RegisterWindowMessage (_T("WM_ON_HISTORY"));

//////////////////////////////////////////////////////////////////////
// コンストラクション/デストラクション
//////////////////////////////////////////////////////////////////////

CCallsignHistoryView::CCallsignHistoryView()
{
}

CCallsignHistoryView::~CCallsignHistoryView()
{
	FreeHistory ();
}

BEGIN_MESSAGE_MAP(CCallsignHistoryView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_ON_HISTORY, &CCallsignHistoryView::OnHistory)
	ON_NOTIFY(LVN_GETDISPINFO, 2, &CCallsignHistoryView::OnLvnGetdispinfoList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallsignHistoryView メッセージ ハンドラー

int CCallsignHistoryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	CString strLabel;

	rectDummy.SetRectEmpty();

	// ビューの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOSORTHEADER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndListCtrl.Create (dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("クラス ビューを作成できませんでした\n");
		return -1;      // 作成できない場合
	}
	m_wndListCtrl.Initialize ();
	//m_wndListCtrl.SetExtendedStyle (m_wndListCtrl.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_DOUBLEBUFFER);
	//m_wndListCtrl.SetExtendedStyle (m_wndListCtrl.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_DOUBLEBUFFER);

	if (!strLabel.LoadString (IDS_HISTORY_VIEW_DATETIME)){}		m_wndListCtrl.InsertColumn (COLUMN_DATETIME	, strLabel, 0, 130);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_CALLER)){}		m_wndListCtrl.InsertColumn (COLUMN_CALLER	, strLabel, 0, 140);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_MEMO)){}			m_wndListCtrl.InsertColumn (COLUMN_MEMO		, strLabel, 0, 50);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_CALLED)){}		m_wndListCtrl.InsertColumn (COLUMN_CALLED	, strLabel, 0, 140);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_RX_RPT1)){}		m_wndListCtrl.InsertColumn (COLUMN_RX_RPT1	, strLabel, 0, 140);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_RX_RPT2)){}		m_wndListCtrl.InsertColumn (COLUMN_RX_RPT2	, strLabel, 0, 140);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_STATUS)){}		m_wndListCtrl.InsertColumn (COLUMN_STATUS	, strLabel, 0, 100);

	LoadHistory ();

	return 0;
}

void CCallsignHistoryView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CCallsignHistoryView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
}

void CCallsignHistoryView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;

	GetClientRect(rectClient);

	m_wndListCtrl.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CCallsignHistoryView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CCallsignHistoryView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_wndListCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CCallsignHistoryView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndListCtrl.SetFocus();
}


VOID CCallsignHistoryView::AddHistory (CStringArray* pArray)
{
	CSingleLock		Lock (&m_HistoryArrayLock, TRUE);
	CStringArray*	pNewArray = new CStringArray;

	pNewArray->Copy (*pArray);

	OnAddHistory (pNewArray);

	m_HistoryAddArray.Add (pNewArray);
	PostMessage (WM_ON_HISTORY);
}

VOID CCallsignHistoryView::OnAddHistory (CStringArray* pArray)
{
	CString				strTemp;
	CRepeater*			pRepeater;
	CMapStringToPtr*	pMap = &theApp.GetRepeaterList ()->m_Map;


	if (pMap->Lookup (pArray->GetAt (COLUMN_CALLER), (void*&)pRepeater))
	{
		strTemp.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
		pArray->SetAt (COLUMN_CALLER, strTemp);
	}

	if (pMap->Lookup (pArray->GetAt (COLUMN_CALLED), (void*&)pRepeater))
	{
		strTemp.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
		pArray->SetAt (COLUMN_CALLED, strTemp);
	}

	if (pMap->Lookup (pArray->GetAt (COLUMN_RX_RPT1), (void*&)pRepeater))
	{
		strTemp.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
		pArray->SetAt (COLUMN_RX_RPT1, strTemp);
	}

	if (pMap->Lookup (pArray->GetAt (COLUMN_RX_RPT2), (void*&)pRepeater))
	{
		strTemp.Format (_T("%s (%s)"), pRepeater->GetCallsign (), pRepeater->GetName ());
		pArray->SetAt (COLUMN_RX_RPT2, strTemp);
	}
}


VOID CCallsignHistoryView::FreeHistory ()
{
	CSingleLock		Lock (&m_HistoryArrayLock, TRUE);

	for (int i=0;i<m_HistoryArray.GetCount ();i++)
	{
		delete m_HistoryArray.GetAt (i);
	}

	m_HistoryArray.RemoveAll ();

	if (::IsWindow (GetSafeHwnd ()))
	{
		PostMessage (WM_ON_HISTORY);
	}
}

VOID CCallsignHistoryView::LoadHistory ()
{
	INT				Days	= 7;
	CSingleLock		Lock (&m_HistoryArrayLock, TRUE);
	CString			strFileName;
	CString			strLogDir	 = GetMyDir () + _T("LOG\\History\\");
	CString			strLine;
	CTimeSpan		TimeSpan (1, 0, 0, 0);
	CTime			Time	= CTime::GetCurrentTime () - CTimeSpan (Days - 1, 0, 0, 0);
	CStringArray*	pNewArray;
	CString			strText;
	INT				sTextIndex	= 0;
	INT				eTextIndex;
	INT				sIndex	= 0;
	INT				eIndex;



	for (int i=0;i<Days;i++)
	{
		strFileName.Format (_T("%s%s"), strLogDir.GetString (), Time.Format (_T("%Y\\%m%d\\History_%Y%m%d.txt")).GetString ());
		DBG_MSG((_T("CCallsignHistoryView::LoadHistory - %s\n"), strFileName.GetString ()));

		do
		{
			strText = _T("");

			if (!FileReadString (strFileName, &strText))
			{	break;
			}

			sTextIndex = 0;

			while (1)
			{
				eTextIndex = strText.Find (_T("\n"), sTextIndex);
				if (eTextIndex == -1)
				{	break;
				}

				strLine = strText.Mid (sTextIndex, eTextIndex - sTextIndex);
				sTextIndex = eTextIndex + 1;


				pNewArray = new CStringArray;

				sIndex = 0;
				while (1)
				{
					eIndex = strLine.Find (_T("\t"), sIndex);
					if (eIndex == -1)
					{	break;
					}

					pNewArray->Add (strLine.Mid (sIndex, eIndex - sIndex));
					sIndex = eIndex += 1;//\t
				}

				if (pNewArray->GetCount () == COLUMN_MAX)
				{
					OnAddHistory (pNewArray);
					m_HistoryAddArray.Add (pNewArray);
				}
				else
				{
					delete pNewArray;
				}
			}
		}
		while (0);

		Time += TimeSpan;
	}

	PostMessage (WM_ON_HISTORY);
}

LRESULT CCallsignHistoryView::OnHistory (WPARAM wParam, LPARAM lParam)
{
	CSingleLock Lock (&m_HistoryArrayLock, TRUE);
	BOOL		bRedraw = FALSE;

	for (int i=0;i<m_HistoryAddArray.GetCount ();i++)
	{
		m_HistoryArray.Add (m_HistoryAddArray.GetAt (i));
	}
	m_HistoryAddArray.RemoveAll ();

	while (m_HistoryArray.GetCount () > 10000)
	{
		delete m_HistoryArray.GetAt (0);
		m_HistoryArray.RemoveAt (0);
		bRedraw = TRUE;
	}

	m_wndListCtrl.SetItemCountEx (static_cast<INT>(m_HistoryArray.GetCount ()));
	m_wndListCtrl.EnsureVisible (static_cast<INT>(m_HistoryArray.GetCount ()) - 1, FALSE);

	if (bRedraw)
	{
		m_wndListCtrl.Invalidate ();
		m_wndListCtrl.UpdateWindow ();
	}

	return 0;
}

void CCallsignHistoryView::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO*	pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CStringArray*	pNewArray;

	do
	{
		if (!(pDispInfo->item.mask & LVIF_TEXT))
		{	break;
		}

		if (pDispInfo->item.iItem >= m_HistoryArray.GetCount ())
		{	break;
		}

		pNewArray = m_HistoryArray.GetAt (pDispInfo->item.iItem);
		ASSERT (pNewArray != NULL);
		if (pNewArray == NULL)
		{	break;
		}

		if (pDispInfo->item.iSubItem >= pNewArray->GetCount ())
		{	break;
		}

		StringCchCopy (pDispInfo->item.pszText, pDispInfo->item.cchTextMax, pNewArray->GetAt (pDispInfo->item.iSubItem));
	}
	while (0);

	*pResult = 0;
}


BOOL CCallsignHistoryView::AddRxCall (PVOID pBuffer, ULONG Length, CString* pstrCaller, CString* pstrCalled)
{
	PUCHAR			p		= reinterpret_cast<PUCHAR>(pBuffer);
	PCHAR			pC		= reinterpret_cast<PCHAR>(pBuffer);
	DV_FLAG*		pFlag	= reinterpret_cast<DV_FLAG*>(p + 7);
	CTime			Time	= CTime::GetCurrentTime ();
	CStringA		strRpt2A;
	CStringA		strRpt1A;
	CStringA		strCalledA;
	CStringA		strCallerA;
	CStringA		strMemoA;
	CString			strTime;
	CString			strStatus;
	CString			strFileName;
	CStringArray	NewArray;
	CString			strLogDir	 = GetMyDir () + _T("LOG\\History\\");
	CStringA		strLogTextA;
	CStringA		strSpace = "        ";

	DBG_MSG((_T("CCallsignHistoryView::AddRxCall")));

	strTime.Format (_T("%s"), Time.Format (_T("%Y/%m/%d %H:%M:%S")).GetString ());

	strRpt2A.SetString	(pC +  8, 8);
	strRpt1A.SetString	(pC + 16, 8);
	strCalledA.SetString(pC + 24, 8);
	strCallerA.SetString(pC + 32, 8);
	strMemoA.SetString	(pC + 40, 4);

	if ((strRpt2A == strSpace) && (strRpt1A == strSpace) && (strCalledA == strSpace) && (strCallerA == strSpace) && (strMemoA == "    "))
	{
		return FALSE;
	}

	if ((pFlag->CtrlFlag != DV_CTRL_FLAG_RELAY_UNAVAILABLE) && (pFlag->CtrlFlag != DV_CTRL_FLAG_NO_RESPONSE))
	{
		if (pstrCaller != NULL)
		{
			pstrCaller->Format (_T("%s %s"), CString (strCallerA).GetString (), CString (strMemoA).GetString ());
		}
	
		if (pstrCalled != NULL)
		{
			pstrCalled->Format (_T("%s"), CString (strCalledA).GetString ());
		}
	}

	switch (pFlag->CtrlFlag)
	{
	case DV_CTRL_FLAG_NULL:				{	strStatus = _T("RX");			}	break;
	case DV_CTRL_FLAG_RELAY_UNAVAILABLE:{	strStatus = _T("RPT?");		}	break;
	case DV_CTRL_FLAG_NO_RESPONSE:		{	strStatus = _T("UR?");		}	break;
	case DV_CTRL_FLAG_ACK:				{	strStatus = _T("ACK");		}	break;
	case DV_CTRL_FLAG_RESENT:			{	strStatus = _T("RESENT");	}	break;
	case DV_CTRL_FLAG_UNUSED:			{	strStatus = _T("UNUSED");	}	break;
	case DV_CTRL_FLAG_AUTO_REPLY:		{	strStatus = _T("AT-RESP");	}	break;
	case DV_CTRL_FLAG_REPEATER_CTRL:	{	strStatus = _T("RPT(CTRL)");}	break;
	}

	if (pFlag->Urgent)		{	strStatus += _T(", EMR");	}
	if (pFlag->Control)		{	strStatus += _T(", CTRL");	}
	if (pFlag->Interruption){	strStatus += _T(", BK");	}
	if (pFlag->Relay)		{	strStatus += _T(", RPT");	}
	if (pFlag->Data)		{	strStatus += _T(", DATA");	}

	NewArray.SetAtGrow (COLUMN_DATETIME, strTime);
	NewArray.SetAtGrow (COLUMN_CALLER, CString (strCallerA));
	NewArray.SetAtGrow (COLUMN_MEMO, CString (strMemoA));
	NewArray.SetAtGrow (COLUMN_CALLED, CString (strCalledA));
	NewArray.SetAtGrow (COLUMN_RX_RPT1, CString (strRpt1A));
	NewArray.SetAtGrow (COLUMN_RX_RPT2, CString (strRpt2A));
	NewArray.SetAtGrow (COLUMN_STATUS, strStatus);

	strFileName.Format (_T("%s%s"), strLogDir.GetString (), Time.Format (_T("%Y\\%m%d\\History_%Y%m%d.txt")).GetString ());
	strLogDir = strFileName.Left (strFileName.ReverseFind ('\\') + 1);
	
	if (::GetFileAttributes (strLogDir) == INVALID_FILE_ATTRIBUTES)
	{	::SHCreateDirectory (NULL, strLogDir);
	}
	
	//LOG CLOSE?
	if (m_LogFile.GetFilePath () != strFileName)
	{
		if (m_LogFile.m_hFile != INVALID_HANDLE_VALUE)
		{
			m_LogFile.Close ();
		}
	}

	//LOG OPEN
	if (m_LogFile.m_hFile == INVALID_HANDLE_VALUE)
	{
		CFileException Excep;
		if (m_LogFile.Open (strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite, &Excep))
		{
			m_LogFile.SeekToEnd ();
		}
	}

	//LOG WRITE
	if (m_LogFile.m_hFile != INVALID_HANDLE_VALUE)
	{
		strLogTextA = "";
		for (int i=0;i<COLUMN_MAX;i++)
		{
			strLogTextA.AppendFormat("%s\t", CStringA (NewArray.GetAt (i)).GetString ());
		}
		strLogTextA += "\r\n";

		try
		{
			m_LogFile.Write (strLogTextA.GetString (), strLogTextA.GetLength ());
		}
		catch (...)
		{
		}
	}

	//UPDATE DISPLAY
	AddHistory (&NewArray);

	return TRUE;
}

