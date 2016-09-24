#pragma once

class CCustomDC
{
public:
	CCustomDC ();
	~CCustomDC ();

	VOID		Initialize		(CWnd* pWnd, CFont* pFont, INT cx, INT cy, COLORREF colorNormal, COLORREF colorPush, COLORREF colorGlay, COLORREF colorText, COLORREF colorLine, BOOL bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);
	VOID		Deinitialize	();
	CDC*		GetDC			();
	CSize		GetSize			();
	COLORREF	GetLineColor	();
	COLORREF	GetTextColor	();
	CFont*		GetFont			();

public:
	CSize		m_Size;
	CBitmap*	m_pBmp;
	CBitmap*	m_pOldBmp;
	CDC			m_MemDC;
	COLORREF	m_ClrLine;
	COLORREF	m_ClrText;
	CFont		m_Font;
};

#define CUSTOM_DC_TEXT_COLOR			RGB (220, 220, 220)
#define CUSTOM_DC_BK_NORMAL_COLOR		RGB (40, 40, 40)
#define CUSTOM_DC_BK_PUSH_COLOR			RGB (50, 20, 0)
#define CUSTOM_DC_BK_PUSH_COLOR2		RGB (50*2, 20*2, 0)
#define CUSTOM_DC_BK_GLAY_COLOR			RGB (60, 60, 60)
#define CUSTOM_DC_LINE_COLOR			RGB (70, 70, 70)
