// CDlgEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CDlgEdit.h"
#include "afxdialogex.h"


// CDlgEdit ダイアログ

IMPLEMENT_DYNAMIC(CDlgEdit, CDialogEx)

CDlgEdit::CDlgEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT, pParent)
{

}

CDlgEdit::~CDlgEdit()
{
}

VOID CDlgEdit::SetText (LPCTSTR pszText, INT MaxLen)
{
	m_strText	= pszText;
	m_MaxLen	= MaxLen;
}

LPCTSTR CDlgEdit::GetText ()
{
	return m_strText;
}

void CDlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditBox);
	DDX_Control(pDX, IDOK, m_OkButton);
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
}


BEGIN_MESSAGE_MAP(CDlgEdit, CDialogEx)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CDlgEdit メッセージ ハンドラー


BOOL CDlgEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	INT		Height	= 20;

	m_DC.Initialize (this, NULL, 100, Height, CUSTOM_DC_BK_NORMAL_COLOR, CUSTOM_DC_BK_PUSH_COLOR2, CUSTOM_DC_BK_GLAY_COLOR, CUSTOM_DC_TEXT_COLOR, CUSTOM_DC_LINE_COLOR);

	// TODO: ここに初期化を追加してください
	SetBackgroundColor (CUSTOM_DC_BK_NORMAL_COLOR);

	m_EditBox.SetLimitText (m_MaxLen);

	m_strText.TrimRight ();
	m_EditBox.SetWindowText (m_strText);
	m_EditBox.SetSel (0, -1);
	
	m_OkButton.SetCustomDC (&m_DC);
	m_CancelButton.SetCustomDC (&m_DC);
	
	//m_OkButton.Create (_T("OK"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW		, Rect, this, IDC_BTN_BAND);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CDlgEdit::OnOK()
{
	m_EditBox.GetWindowText (m_strText);

	CDialogEx::OnOK();
}


LRESULT CDlgEdit::OnNcHitTest(CPoint point)
{
    LRESULT nHit = CDialogEx::OnNcHitTest(point);
    return (nHit == HTCLIENT)? HTCAPTION : nHit;
	//return CDialogEx::OnNcHitTest(point);
}
