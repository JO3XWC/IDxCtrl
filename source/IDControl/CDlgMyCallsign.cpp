// CDlgMyCallsign.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CDlgMyCallsign.h"
#include "afxdialogex.h"


// CDlgMyCallsign ダイアログ

IMPLEMENT_DYNAMIC(CDlgMyCallsign, CDialogEx)

CDlgMyCallsign::CDlgMyCallsign(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT_MY_CALLSIGN, pParent)
{

}

CDlgMyCallsign::~CDlgMyCallsign()
{
}

VOID CDlgMyCallsign::SetCallsign (LPCTSTR pszText, INT MaxLen)
{
	m_strCallsign = pszText;
	m_CallsignLen = MaxLen;
}

LPCTSTR CDlgMyCallsign::GetCallsign ()
{
	return m_strCallsign;
}
	
VOID CDlgMyCallsign::SetMemo (LPCTSTR pszText, INT MaxLen)
{
	m_strMemo = pszText;
	m_MemoLen = MaxLen;
}

LPCTSTR CDlgMyCallsign::GetMemo ()
{
	return m_strMemo;
}


void CDlgMyCallsign::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALLSIGN_EDIT, m_CallsignEditBox);
	DDX_Control(pDX, IDC_MEMO_EDIT, m_MemoEditBox);
	DDX_Control(pDX, IDOK, m_OkButton);
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
}


BEGIN_MESSAGE_MAP(CDlgMyCallsign, CDialogEx)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CDlgMyCallsign メッセージ ハンドラー
BOOL CDlgMyCallsign::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	INT		Height	= 20;

	m_DC.Initialize (this, NULL, 100, Height, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR2, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);

	// TODO: ここに初期化を追加してください
	SetBackgroundColor (CUSTOM_DC_BK_NORMAL_COLOR);

	m_CallsignEditBox.SetLimitText (m_CallsignLen);
	m_MemoEditBox.SetLimitText (m_MemoLen);

	m_strCallsign.TrimRight ();
	m_CallsignEditBox.SetWindowText (m_strCallsign);
	m_CallsignEditBox.SetSel (0, -1);

	m_strMemo.TrimRight ();
	m_MemoEditBox.SetWindowText (m_strMemo);
	m_MemoEditBox.SetSel (0, -1);
	
	m_OkButton.SetCustomDC (&m_DC);
	m_CancelButton.SetCustomDC (&m_DC);
	
	//m_OkButton.Create (_T("OK"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_BAND);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgMyCallsign::OnOK()
{
	m_CallsignEditBox.GetWindowText (m_strCallsign);
	m_MemoEditBox.GetWindowText (m_strMemo);

	CDialogEx::OnOK();
}


LRESULT CDlgMyCallsign::OnNcHitTest(CPoint point)
{
    LRESULT nHit = CDialogEx::OnNcHitTest(point);
    return (nHit == HTCLIENT)? HTCAPTION : nHit;
	//return CDialogEx::OnNcHitTest(point);
}
