
#include "stdafx.h"
#include "MainFrm.h"
#include "CGpsHistoryView.h"
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

CGpsHistoryView::CGpsHistoryView()
{
}

CGpsHistoryView::~CGpsHistoryView()
{
	FreeHistory ();
}

BEGIN_MESSAGE_MAP(CGpsHistoryView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_ON_HISTORY, &CGpsHistoryView::OnHistory)
	ON_NOTIFY(LVN_GETDISPINFO, 2, &CGpsHistoryView::OnLvnGetdispinfoList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsHistoryView メッセージ ハンドラー

int CGpsHistoryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if (!strLabel.LoadString (IDS_HISTORY_VIEW_DATETIME)){}		m_wndListCtrl.InsertColumn (COLUMN_DATETIME	, strLabel, 0, 130);
	if (!strLabel.LoadString (IDS_HISTORY_VIEW_GPS)){}			m_wndListCtrl.InsertColumn (COLUMN_GPS		, strLabel, 0, 600);

	LoadHistory ();

	return 0;
}

void CGpsHistoryView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CGpsHistoryView::OnContextMenu(CWnd* pWnd, CPoint point)
{

}

void CGpsHistoryView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;

	GetClientRect(rectClient);

	m_wndListCtrl.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CGpsHistoryView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CGpsHistoryView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_wndListCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CGpsHistoryView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndListCtrl.SetFocus();
}


VOID CGpsHistoryView::AddHistory (CStringArray* pArray)
{
	CSingleLock		Lock (&m_HistoryArrayLock, TRUE);
	CStringArray*	pNewArray = new CStringArray;

	pNewArray->Copy (*pArray);

	m_HistoryAddArray.Add (pNewArray);

	PostMessage (WM_ON_HISTORY);
}


VOID CGpsHistoryView::FreeHistory ()
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

VOID CGpsHistoryView::LoadHistory ()
{
	INT				Days	= 7;
	CSingleLock		Lock (&m_HistoryArrayLock, TRUE);
	CString			strFileName;
	CString			strLogDir	 = GetMyDir () + _T("LOG\\Gps\\");
	CString			strLine;
	CTimeSpan		TimeSpan (1, 0, 0, 0);
	CTime			Time	= CTime::GetCurrentTime () - CTimeSpan (Days - 1, 0, 0, 0);
	CStringArray*	pNewArray;
	CString			strText;
	INT				sTextIndex	= 0;
	INT				eTextIndex;
	INT				sIndex;
	INT				eIndex;



	for (int i=0;i<Days;i++)
	{
		strFileName.Format (_T("%s%s"), strLogDir.GetString (), Time.Format (_T("%Y\\%m%d\\Gps_%Y%m%d.txt")).GetString ());
		DBG_MSG((_T("CGpsHistoryView::LoadHistory - %s\n"), strFileName.GetString ()));

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

				m_HistoryAddArray.Add (pNewArray);
			}
		}
		while (0);

		Time += TimeSpan;
	}

	PostMessage (WM_ON_HISTORY);
}

LRESULT CGpsHistoryView::OnHistory (WPARAM wParam, LPARAM lParam)
{
	CSingleLock Lock (&m_HistoryArrayLock, TRUE);
	BOOL bRefresh = FALSE;

	for (int i=0;i<m_HistoryAddArray.GetCount ();i++)
	{
		m_HistoryArray.Add (m_HistoryAddArray.GetAt (i));
	}
	m_HistoryAddArray.RemoveAll ();

	while (m_HistoryArray.GetCount () > 10000)
	{
		delete m_HistoryArray.GetAt (0);
		m_HistoryArray.RemoveAt (0);

		bRefresh = TRUE;
	}

	m_wndListCtrl.SetItemCountEx (static_cast<INT>(m_HistoryArray.GetCount ()));
	m_wndListCtrl.EnsureVisible (static_cast<INT>(m_HistoryArray.GetCount ()) - 1, FALSE);

	if (bRefresh)
	{
		m_wndListCtrl.RedrawWindow ();
	}

	return 0;
}

void CGpsHistoryView::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
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

VOID CGpsHistoryView::AddGps (LPCTSTR pszGps)
{
	CTime			Time	= CTime::GetCurrentTime ();
	CString			strTime;
	CString			strStatus;
	CString			strFileName;
	CStringArray	NewArray;
	CString			strLogDir	 = GetMyDir () + _T("LOG\\Gps\\");
	CStringA		strLogTextA;

	strTime.Format (_T("%s"), Time.Format (_T("%Y/%m/%d %H:%M:%S")).GetString ());


	NewArray.SetAtGrow (COLUMN_DATETIME, strTime);
	NewArray.SetAtGrow (COLUMN_GPS, pszGps);

	strFileName.Format (_T("%s%s"), strLogDir.GetString (), Time.Format (_T("%Y\\%m%d\\Gps_%Y%m%d.txt")).GetString ());
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
		m_LogFile.Open (strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite, &Excep);
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
}

