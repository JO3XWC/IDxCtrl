// CFreqDisplayCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CFreqDisplayCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AUTO_REPEATE_TIME_ID	123456

// CFreqDisplayCtrl

IMPLEMENT_DYNAMIC(CFreqDisplayCtrl, CCustomImageWnd)

CFreqDisplayCtrl::CFreqDisplayCtrl()
{
	m_Freq				= 433000000;
	m_DupOffset			= 0;
	m_DupMode			= 0;
	m_PttStatus			= 0;
	m_Monitor			= 0;
	m_BandSel			= 0;

	m_NotifyCode		= 0;
	m_nRepeatTime		= 60;
	m_nRepeatTimeDelay	= 500;
	m_hCustomCursor		= NULL;
}

CFreqDisplayCtrl::~CFreqDisplayCtrl()
{
}
	
VOID CFreqDisplayCtrl::SetCustomCursor (HCURSOR hCursor)
{
	m_hCustomCursor = hCursor;
}

VOID CFreqDisplayCtrl::SetFreq (ULONGLONG Freq)
{
	m_Freq = Freq;
	Invalidate ();
	UpdateWindow ();
}

ULONGLONG CFreqDisplayCtrl::GetFreq ()
{
	return m_Freq;
}

VOID CFreqDisplayCtrl::SetDupOffset (ULONGLONG DupOffset)
{
	m_DupOffset = DupOffset;
	Invalidate ();
	UpdateWindow ();
}

VOID CFreqDisplayCtrl::SetDupMode (ULONG DupMode)
{
	m_DupMode = DupMode;
	Invalidate ();
	UpdateWindow ();
}

VOID CFreqDisplayCtrl::SetPttStatus (ULONG Status)
{
	m_PttStatus = Status;
	Invalidate ();
	UpdateWindow ();
}

VOID CFreqDisplayCtrl::SetMonitor (ULONG Monitor)
{
	m_Monitor = Monitor;
	Invalidate ();
	UpdateWindow ();
}

VOID CFreqDisplayCtrl::SelectBand (ULONG Sel)
{
	m_BandSel = Sel;
	Invalidate ();
	UpdateWindow ();
}




BEGIN_MESSAGE_MAP(CFreqDisplayCtrl, CCustomImageWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CFreqDisplayCtrl メッセージ ハンドラー




int CFreqDisplayCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CCustomImageWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	LoadPng (IDB_DIGIT_ENABLE_PNG	, &m_Image[PNG_ENABLE]);
	LoadPng (IDB_DIGIT_DISABLE_PNG	, &m_Image[PNG_DISABLE]);
	LoadPng (IDB_DIGIT_PTT_ON_OK_PNG, &m_Image[PNG_PTT_ON]);
	LoadPng (IDB_DIGIT_PTT_ON_NG_PNG, &m_Image[PNG_PTT_NG]);
	LoadPng (IDB_DIGIT_PW_OFF_PNG	, &m_Image[PNG_PW_OFF]);

	return 0;
}


void CFreqDisplayCtrl::OnPaint()
{
	CPaintDC	dc(this); // device context for painting
	CRect		Rect;
	BYTE		SrcAlpha	= 255;
	ULONGLONG	Freq		= m_Freq;
	ULONG		Value;
	CImage*		pImage		= &m_Image[PNG_DISABLE];
	INT			ImageWidth	= pImage->GetWidth () / 11;
	INT			ImageHeight	= pImage->GetHeight ();
	INT			TargetWidth;
	INT			TargetHeight;
	INT			Left		= ImageWidth * 9;
	BOOL		bDraw		= FALSE;
	INT			BltMode		= dc.SetStretchBltMode (HALFTONE);

	if (m_Monitor)
	{
		switch (m_DupMode)
		{
		case DUP_MODE_MINUS:
			{
				Freq -= m_DupOffset;
			}
			break;

		case DUP_MODE_PLUS:
			{
				Freq += m_DupOffset;
			}
			break;
		}
	}

	if (GetStyle () & WS_DISABLED)
	{
		pImage = &m_Image[PNG_PW_OFF];
	}
	else if (m_PttStatus)
	{
		pImage = &m_Image[PNG_PTT_ON];
	}
	else if (m_BandSel)
	{
		pImage = &m_Image[PNG_ENABLE];
	}

	GetClientRect (&Rect);

	TargetWidth = Rect.Width () / 10;
	TargetHeight = Rect.Height ();

	dc.FillSolidRect (&Rect, CUSTOM_DC_BK_NORMAL_COLOR);
	//dc.FillSolidRect (&Rect, RGB (30, 30, 30));
	
	Freq /= 10;

	for (int i=0;i<11;i++)
	{
		Value = Freq % 10;
	
		if ((i==2) || (i==6))
		{
			pImage->AlphaBlend (dc.GetSafeHdc (), Left, 0, TargetWidth, TargetHeight, 10 * ImageWidth, 0, ImageWidth, ImageHeight, SrcAlpha); 
			Left -= ImageWidth;
			continue;
		}

		pImage->AlphaBlend (dc.GetSafeHdc (), Left, 0, TargetWidth, TargetHeight, Value * ImageWidth, 0, ImageWidth, ImageHeight, SrcAlpha); 
		Left -= ImageWidth;
		
		Freq /= 10;
		if (Freq == 0)
		{	break;
		}
	}	

	dc.SetStretchBltMode (BltMode);
}


void CFreqDisplayCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	SendParentNotify (NM_RCLICK);

	if (!m_NotifyCode)
	{
		SetCapture();
		m_NotifyCode = NM_RCLICK;

		if (m_nRepeatTime > 0)
		{
			SetTimer (AUTO_REPEATE_TIME_ID, m_nRepeatTimeDelay, NULL);
		}
	}

	CCustomImageWnd::OnRButtonDown(nFlags, point);
}

void CFreqDisplayCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	SendParentNotify (NM_RCLICK);

	if (!m_NotifyCode)
	{
		SetCapture();
		m_NotifyCode = NM_RCLICK;

		if (m_nRepeatTime > 0)
		{
			SetTimer (AUTO_REPEATE_TIME_ID, m_nRepeatTimeDelay, NULL);
		}
	}

	CCustomImageWnd::OnRButtonDblClk(nFlags, point);
}

void CFreqDisplayCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_NotifyCode)
	{
		ReleaseCapture();
		m_NotifyCode = FALSE;
		KillTimer(AUTO_REPEATE_TIME_ID);
	}

	CCustomImageWnd::OnRButtonUp(nFlags, point);
}





void CFreqDisplayCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendParentNotify (NM_CLICK);
	
	if (!m_NotifyCode)
	{
		SetCapture();
		m_NotifyCode = NM_CLICK;

		if (m_nRepeatTime > 0)
		{
			SetTimer (AUTO_REPEATE_TIME_ID, m_nRepeatTimeDelay, NULL);
		}
	}

	CCustomImageWnd::OnLButtonDown(nFlags, point);
}

void CFreqDisplayCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SendParentNotify (NM_CLICK);
	
	if (!m_NotifyCode)
	{
		SetCapture();
		m_NotifyCode = NM_CLICK;

		if (m_nRepeatTime > 0)
		{
			SetTimer (AUTO_REPEATE_TIME_ID, m_nRepeatTimeDelay, NULL);
		}
	}

	CCustomImageWnd::OnLButtonDblClk(nFlags, point);
}

void CFreqDisplayCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_NotifyCode)
	{
		ReleaseCapture();
		m_NotifyCode = 0;
		KillTimer(AUTO_REPEATE_TIME_ID);
	}

	CCustomImageWnd::OnLButtonUp(nFlags, point);
}



void CFreqDisplayCtrl::OnCancelMode()
{
	CCustomImageWnd::OnCancelMode();

	if (m_NotifyCode)
	{
		ReleaseCapture();
		m_NotifyCode = 0;
		KillTimer(AUTO_REPEATE_TIME_ID);
	}
}


void CFreqDisplayCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == AUTO_REPEATE_TIME_ID)
	{
		KillTimer (AUTO_REPEATE_TIME_ID);
		SendParentNotify (m_NotifyCode);
		SetTimer (AUTO_REPEATE_TIME_ID, m_nRepeatTime, NULL);
	}

	CCustomImageWnd::OnTimer(nIDEvent);
}

LRESULT CFreqDisplayCtrl::SendParentNotify (UINT Code)
{
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		NMHDR nmh;
		nmh.code = Code;
		nmh.idFrom = GetDlgCtrlID();
		nmh.hwndFrom = m_hWnd;

		return pParent->SendMessage (WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
	}

	return 0;
}



BOOL CFreqDisplayCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		NMHDR nmh;
		nmh.idFrom = GetDlgCtrlID();
		nmh.hwndFrom = m_hWnd;

		if (zDelta >= WHEEL_DELTA)
		{
			nmh.code = NM_RCLICK;
			pParent->SendMessage (WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
		}
		else if (zDelta <= -WHEEL_DELTA)
		{
			nmh.code = NM_CLICK;
			pParent->SendMessage (WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
		}

	}

	return CCustomImageWnd::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CFreqDisplayCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCustomCursor != NULL)
	{
		SetCursor (m_hCustomCursor);
		return TRUE;
	}

	return CCustomImageWnd::OnSetCursor(pWnd, nHitTest, message);
}
