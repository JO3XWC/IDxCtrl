
#include "stdafx.h"
#include "MainFrm.h"
#include "CDebugView.h"
#include "Resource.h"
#include "IDControl.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT WM_ON_DEBUG_MSG = ::RegisterWindowMessage (_T("WM_ON_DEBUG_MSG"));



//////////////////////////////////////////////////////////////////////
// コンストラクション/デストラクション
//////////////////////////////////////////////////////////////////////

CDebugView::CDebugView()
{
}

CDebugView::~CDebugView()
{
}

BEGIN_MESSAGE_MAP(CDebugView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_ON_DEBUG_MSG, &CDebugView::OnDebugMsg)
	ON_NOTIFY(LVN_GETDISPINFO, 2, &CDebugView::OnLvnGetdispinfoList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugView メッセージ ハンドラー

int CDebugView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// ビューの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOCOLUMNHEADER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndListCtrl.Create (dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("クラス ビューを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	m_wndListCtrl.Initialize ();
	//m_wndListCtrl.SetExtendedStyle (m_wndListCtrl.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_DOUBLEBUFFER);

	m_wndListCtrl.InsertColumn (0, _T(""), 0, 800);

	return 0;
}

void CDebugView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CDebugView::OnContextMenu(CWnd* pWnd, CPoint point)
{

}

void CDebugView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	CHeaderCtrl* pHeader = m_wndListCtrl.GetHeaderCtrl ();
	HDITEM HdItem = {};

	GetClientRect(rectClient);
	HdItem.mask = HDI_WIDTH;
	HdItem.cxy = rectClient.Width() - 20;
	
	pHeader->SetItem (0, &HdItem);

	m_wndListCtrl.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CDebugView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CDebugView::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_wndListCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CDebugView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndListCtrl.SetFocus();
}

VOID CDebugView::AddFormat (LPCTSTR pszFormat, ...)
{
	CSingleLock Lock (&m_DebugMsgArrayLock, TRUE);
	CString		String;
	va_list		args;
	SYSTEMTIME	Time;

	::GetLocalTime (&Time);

	String.Format (_T("%02u:%02u:%02u:%03u : "), Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);

	va_start (args, pszFormat);
	String.AppendFormatV (pszFormat, args);
	va_end (args);

	m_DebugMsgAddArray.Add (String);

	PostMessage (WM_ON_DEBUG_MSG);
}

VOID CDebugView::AddDump (LPCTSTR pszMsg, PVOID pBuffer, ULONG Length)
{
	CSingleLock Lock (&m_DebugMsgArrayLock, TRUE);
	CString		strHex;
	CString		strAscii = _T(" : ");
	PUCHAR		p = reinterpret_cast<PUCHAR>(pBuffer);
	TCHAR* Ascii	=	_T("................")\
						_T("................")\
						_T(" !\"#$.&\'()*+,-./")\
						_T("0123456789:;<=>?")\
						_T("@ABCDEFGHIJKLMNO")\
						_T("PQRSTUVWXYZ[\\]^_")\
						_T("`abcdefghijklmno")\
						_T("pqrstuvwxyz{|}~.")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................");
	
	strHex.Format (_T("%s"), pszMsg);
	for (ULONG i=0;i<Length;i++)
	{
		strHex.AppendFormat (_T("%02X "), p[i]);
		strAscii += Ascii[p[i]];
	}
	strHex += strAscii;

	AddFormat (strHex);
}


LRESULT CDebugView::OnDebugMsg (WPARAM wParam, LPARAM lParam)
{
	CSingleLock Lock (&m_DebugMsgArrayLock, TRUE);

	for (int i=0;i<m_DebugMsgAddArray.GetCount ();i++)
	{
		m_DebugMsgArray.Add (m_DebugMsgAddArray.GetAt (i));
	}
	m_DebugMsgAddArray.RemoveAll ();

	while (m_DebugMsgArray.GetCount () > 10000)
	{
		m_DebugMsgArray.RemoveAt (0);
	}

	m_wndListCtrl.SetItemCountEx (static_cast<INT>(m_DebugMsgArray.GetCount ()));
	m_wndListCtrl.EnsureVisible (static_cast<INT>(m_DebugMsgArray.GetCount ()) - 1, FALSE);

	return 0;
}

void CDebugView::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	do
	{
		if (!(pDispInfo->item.mask & LVIF_TEXT))
		{	break;
		}

		if (pDispInfo->item.iItem >= m_DebugMsgArray.GetCount ())
		{	break;
		}

		StringCchCopy (pDispInfo->item.pszText, pDispInfo->item.cchTextMax, m_DebugMsgArray.GetAt (pDispInfo->item.iItem));
	}
	while (0);

	*pResult = 0;
}

