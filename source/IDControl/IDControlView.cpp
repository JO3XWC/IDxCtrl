
// IDControlView.cpp : CIDControlView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "IDControl.h"
#endif

#include "IDControlDoc.h"
#include "IDControlView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CIDControlView

IMPLEMENT_DYNCREATE(CIDControlView, CView)

BEGIN_MESSAGE_MAP(CIDControlView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CIDControlView コンストラクション/デストラクション

CIDControlView::CIDControlView()
{
	// TODO: 構築コードをここに追加します。

}

CIDControlView::~CIDControlView()
{
}

BOOL CIDControlView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CIDControlView 描画

void CIDControlView::OnDraw(CDC* pDC)
{
	CIDControlDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect Rect;

	GetClientRect (&Rect);

	pDC->FillSolidRect (&Rect, RGB (40, 40, 40));
}

void CIDControlView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIDControlView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
}


// CIDControlView 診断

#ifdef _DEBUG
void CIDControlView::AssertValid() const
{
	CView::AssertValid();
}

void CIDControlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIDControlDoc* CIDControlView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIDControlDoc)));
	return (CIDControlDoc*)m_pDocument;
}
#endif //_DEBUG


// CIDControlView メッセージ ハンドラー
