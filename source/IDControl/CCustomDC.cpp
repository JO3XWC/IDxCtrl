#include "stdafx.h"
#include "CCustomDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCustomDC::CCustomDC()
{
}


CCustomDC::~CCustomDC()
{
}

VOID CCustomDC::Initialize (CWnd* pWnd, CFont* pFont, INT cx, INT cy, COLORREF colorNormal, COLORREF colorPush, COLORREF colorGlay, COLORREF colorText, COLORREF colorLine, BOOL bHorz, int nStartFlatPercentage, int nEndFlatPercentage)
{
	VOID*		pBits	= NULL;
	CDC*		pDC		= pWnd->GetDC ();
	CRect		sRect;
	CRect		eRect;
	CRect		TextRect(0, 0, 1, 1);
	COLORREF	colorFinish;
	LOGFONT		LogFont	= {};
	CSize		BmpSize(cx, cy * 3);
	double		clrX = 1.5;

	if (pFont != NULL)
	{
		pFont->GetLogFont (&LogFont);
		LogFont.lfWeight = FW_BOLD;
		m_Font.CreateFontIndirect (&LogFont);
	}
	else
	{
		m_Font.CreateFont ((INT)(cy * 0.7f),
			0,							//文字幅
			0,							//テキストの角度
			0,							//ベースラインとｘ軸との角度
			FW_BOLD,					//フォントの重さ（太さ）
			FALSE,						//イタリック体
			FALSE,						//アンダーライン
			FALSE,						//打ち消し線
			ANSI_CHARSET,				//文字セット
			OUT_DEFAULT_PRECIS,			//出力精度
			CLIP_DEFAULT_PRECIS,		//クリッピング精度
			CLEARTYPE_NATURAL_QUALITY,	//出力品質
			FIXED_PITCH | FF_MODERN,	//ピッチとファミリー
			_T("Arial"));				//書体名
	}

	m_ClrText	= colorText;
	m_ClrLine	= colorLine;
	m_Size		= CSize(cx, cy);

	sRect = CRect (0, 0, m_Size.cx, m_Size.cy / 2);
	eRect = CRect (0, m_Size.cy / 2, m_Size.cx, m_Size.cy);

	m_pBmp = CBitmap::FromHandle (CDrawingManager::CreateBitmap_32 (BmpSize, &pBits));

	m_MemDC.CreateCompatibleDC (pDC);
	m_pOldBmp = m_MemDC.SelectObject (m_pBmp);
	
	CDrawingManager dm(m_MemDC);

	colorFinish = RGB (min (255, GetRValue (colorNormal) * clrX), min (255, GetGValue (colorNormal) * clrX), min (255, GetBValue (colorNormal) * clrX));
	dm.FillGradient(sRect, colorNormal, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);
	dm.FillGradient(eRect, colorFinish, colorNormal, bHorz, nStartFlatPercentage, nEndFlatPercentage);

	sRect.OffsetRect (0, cy);
	eRect.OffsetRect (0, cy);
	colorFinish = RGB (min (255, GetRValue (colorPush) * clrX), min (255, GetGValue (colorPush) * clrX), min (255, GetBValue (colorPush) * clrX));
	dm.FillGradient(sRect, colorPush, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);
	dm.FillGradient(eRect, colorFinish, colorPush, bHorz, nStartFlatPercentage, nEndFlatPercentage);

	sRect.OffsetRect (0, cy);
	eRect.OffsetRect (0, cy);
	colorFinish = RGB (min (255, GetRValue (colorGlay) * clrX), min (255, GetGValue (colorGlay) * clrX), min (255, GetBValue (colorGlay) * clrX));
	dm.FillGradient(sRect, colorGlay, colorFinish, bHorz, nStartFlatPercentage, nEndFlatPercentage);
	dm.FillGradient(eRect, colorFinish, colorGlay, bHorz, nStartFlatPercentage, nEndFlatPercentage);


	pWnd->ReleaseDC (pDC);
}

VOID CCustomDC::Deinitialize ()
{
	m_MemDC.SelectObject (m_pOldBmp);
	delete m_pBmp;

	m_pOldBmp = NULL;
	m_pBmp = NULL;
}

CDC* CCustomDC::GetDC ()
{
	return &m_MemDC;
}

CSize CCustomDC::GetSize ()
{
	return m_Size;
}

COLORREF CCustomDC::GetLineColor ()
{
	return m_ClrLine;
}

COLORREF CCustomDC::GetTextColor ()
{
	return m_ClrText;
}

CFont* CCustomDC::GetFont ()
{
	return &m_Font;
}

