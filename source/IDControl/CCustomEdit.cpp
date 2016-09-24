// CCustomEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CCustomEdit.h"


// CCustomEdit

IMPLEMENT_DYNAMIC(CCustomEdit, CEdit)

CCustomEdit::CCustomEdit()
{

}

CCustomEdit::~CCustomEdit()
{
}


BEGIN_MESSAGE_MAP(CCustomEdit, CEdit)
	//ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CCustomEdit メッセージ ハンドラー




void CCustomEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ここにメッセージ ハンドラー コードを追加します。
					   // 描画メッセージで CEdit::OnPaint() を呼び出さないでください。
}


HBRUSH CCustomEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CRect Rect;

	pDC->SetBkColor (RGB (10, 10, 10));
	pDC->SetTextColor (CUSTOM_DC_TEXT_COLOR);
	
	GetClientRect (&Rect);
	pDC->FillSolidRect (&Rect, RGB (10, 10, 10));

	return (HBRUSH)GetStockObject (NULL_BRUSH);
	//return NULL;
}
