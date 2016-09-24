// CSMeterDisplayCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CSMeterDisplayCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define REFRESH_TIME_ID		123456
#define REFRESH_TIME_TIME	20

// CSMeterDisplayCtrl

IMPLEMENT_DYNAMIC(CSMeterDisplayCtrl, CCustomImageWnd)

CSMeterDisplayCtrl::CSMeterDisplayCtrl()
{
	m_Level				= 0;
	m_BandSel			= 0;
	m_Monitor			= 0;
	m_NoiseSql			= 0;
}

CSMeterDisplayCtrl::~CSMeterDisplayCtrl()
{
}
	
VOID CSMeterDisplayCtrl::SetLevel (LONG Level)
{
	m_Level = Level;
	m_MaxLevel = max (m_MaxLevel, m_Level);
	Invalidate ();
	UpdateWindow ();
}

VOID CSMeterDisplayCtrl::SelectBand (ULONG Sel)
{
	m_BandSel = Sel;
	Invalidate ();
	UpdateWindow ();
}

VOID CSMeterDisplayCtrl::SetMonitor (ULONG Monitor)
{
	m_Monitor = Monitor;
	Invalidate ();
	UpdateWindow ();
}

VOID CSMeterDisplayCtrl::SetNoiseSql (ULONG Sql)
{
	m_NoiseSql = Sql;
	Invalidate ();
	UpdateWindow ();
}




BEGIN_MESSAGE_MAP(CSMeterDisplayCtrl, CCustomImageWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CSMeterDisplayCtrl メッセージ ハンドラー




int CSMeterDisplayCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CCustomImageWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	LoadPng (IDB_SMETER_ENABLE_PNG	, &m_Image[PNG_ENABLE]);
	LoadPng (IDB_SMETER_DISABLE_PNG	, &m_Image[PNG_DISABLE]);
	LoadPng (IDB_SMETER_PW_OFF_PNG	, &m_Image[PNG_PW_OFF]);

	SetTimer (REFRESH_TIME_ID, REFRESH_TIME_TIME, NULL);

	return 0;
}


void CSMeterDisplayCtrl::OnPaint()
{
	CPaintDC	dc(this); // device context for painting
	CRect		Rect;
	BYTE		SrcAlpha	= 255;
	ULONG		Level		= m_Level;
	CImage*		pImage		= &m_Image[PNG_DISABLE];
	INT			ImageWidth	= pImage->GetWidth ();
	INT			ImageHeight	= pImage->GetHeight ();
	BOOL		bDraw		= FALSE;
	INT			BltMode		= dc.SetStretchBltMode (HALFTONE);
	COLORREF	ColorLevelMax;
	COLORREF	ColorLevelMax2;
	COLORREF	ColorLevel;
	COLORREF	ColorLevel2;
	CMemDC		_MemDC (dc, this);
	CDC&		MemDC		= _MemDC.GetDC ();

	if (GetStyle () & WS_DISABLED)
	{
		pImage = &m_Image[PNG_PW_OFF];
	}
	else if (m_BandSel)
	{
		pImage = &m_Image[PNG_ENABLE];
	}

	GetClientRect (&Rect);

	MemDC.FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
	//dc.FillSolidRect (&Rect, RGB (30, 30, 30));
	

	ColorLevelMax = RGB (10, 80, 200);
	ColorLevelMax2 = RGB (170, 32, 32);
	ColorLevel = RGB (10, 80, 255);
	ColorLevel2 = RGB (200, 32, 32);

	if (m_MaxLevel > m_Level)
	{
		if (m_MaxLevel > 154)
		{
			MemDC.FillSolidRect (&CRect(Rect.left + m_Level, Rect.top + 20, Rect.left + m_MaxLevel, Rect.top + 24), ColorLevelMax2);
		}
		MemDC.FillSolidRect (&CRect(Rect.left + m_Level, Rect.top + 20, Rect.left + min (154, m_MaxLevel), Rect.top + 24), ColorLevelMax);
	}
	
	if (m_Level > 0)
	{
		if (m_Level > 154)
		{
			MemDC.FillSolidRect (&CRect(Rect.left, Rect.top + 20, Rect.left + m_Level, Rect.top + 24), ColorLevel2);
		}
		MemDC.FillSolidRect (&CRect(Rect.left, Rect.top + 20, Rect.left + min (154, m_Level), Rect.top + 24), ColorLevel);
	}
	else if (m_NoiseSql | m_Monitor)
	{
		MemDC.FillSolidRect (&CRect(Rect.left, Rect.top + 20, Rect.left + 10, Rect.top + 24), ColorLevel);
	}


	pImage->AlphaBlend (MemDC.GetSafeHdc (), Rect.left, Rect.top, Rect.Width (), Rect.Height (), 0, 0, ImageWidth, ImageHeight, SrcAlpha); 
	
	dc.BitBlt (0, 0, Rect.Width (), Rect.Height (), &MemDC, 0, 0, SRCCOPY);

	//for (int i=0;i<11;i++)
	//{
	//	Value = Freq % 10;
	//
	//	if ((i==2) || (i==6))
	//	{
	//		pImage->AlphaBlend (dc.GetSafeHdc (), Left, 0, ImageWidth, ImageHeight, 10 * ImageWidth, 0, ImageWidth, ImageHeight, SrcAlpha); 
	//		Left -= ImageWidth;
	//		continue;
	//	}

	//	pImage->AlphaBlend (dc.GetSafeHdc (), Left, 0, ImageWidth, ImageHeight, Value * ImageWidth, 0, ImageWidth, ImageHeight, SrcAlpha); 
	//	Left -= ImageWidth;
	//	
	//	Freq /= 10;
	//	if (Freq == 0)
	//	{	break;
	//	}
	//}	

	dc.SetStretchBltMode (BltMode);
}

void CSMeterDisplayCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == REFRESH_TIME_ID)
	{
		if (m_MaxLevel > 0)
		{
			m_MaxLevel -= 7;
			if (m_MaxLevel < 0)
			{	m_MaxLevel = 0;
			}
			m_MaxLevel = max (m_MaxLevel, m_Level);

			Invalidate ();
			UpdateWindow ();
		}

		return;
	}

	CCustomImageWnd::OnTimer(nIDEvent);
}

