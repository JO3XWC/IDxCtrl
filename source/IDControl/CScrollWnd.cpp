#include "stdafx.h"
#include "CScrollWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CScrollWnd, CWnd)

BEGIN_MESSAGE_MAP(CScrollWnd, CWnd)
	//{{AFX_MSG_MAP(CScrollWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CScrollWnd::CScrollWnd()
{
	m_DisplaySize = CSize(0,0);
}

CScrollWnd::~CScrollWnd()
{
}


void CScrollWnd::SetScrollSizes (int nMapMode, SIZE sizeTotal, const SIZE& sizePage, const SIZE& sizeLine)
{
	ASSERT (nMapMode == MM_TEXT);

	m_DisplaySize = sizeTotal;

	if (::IsWindow (m_hWnd))
	{
		UpdateScrollInfo();
	}
}

void CScrollWnd::UpdateScrollInfo ()
{
	CRect		Rect(0, 0, 0, 0);
	CRect		Rect2;
	CSize		DeltaPos(0,0);

	GetClientRect (&Rect2);

	while (Rect != Rect2)
	{
		Rect = Rect2;
		UpdateScrollBar (SB_HORZ, Rect.Width(), m_DisplaySize.cx, DeltaPos.cx);
		UpdateScrollBar (SB_VERT, Rect.Height(), m_DisplaySize.cy, DeltaPos.cy);
		
		GetClientRect (&Rect2);
		
		if ((DeltaPos.cx != 0) || (DeltaPos.cy != 0))
		{
			ScrollWindow (DeltaPos.cx, DeltaPos.cy);
		}
	}
}

void CScrollWnd::UpdateScrollBar (int Bar, int WindowSize, int DisplaySize, LONG& DeltaPos)
{
	int			ScrollMax	= 0;
	SCROLLINFO	ScrollInfo	= {};

	ScrollInfo.cbSize	= sizeof (SCROLLINFO);
	ScrollInfo.fMask	= SIF_ALL;

	GetScrollInfo (Bar, &ScrollInfo);

	DeltaPos = 0;

	if (WindowSize < DisplaySize)
	{
		ScrollMax = DisplaySize - 1;
		if ((ScrollInfo.nPage > 0) && (ScrollInfo.nPos > 0))
		{
			ScrollInfo.nPos = (INT)(ScrollInfo.nPos * WindowSize / ScrollInfo.nPage);
		}

		ScrollInfo.nPage	= WindowSize;
		ScrollInfo.nPos		= min (ScrollInfo.nPos, DisplaySize - (INT)ScrollInfo.nPage);
		DeltaPos			= GetScrollPos (Bar) - ScrollInfo.nPos;
	}
	else
	{
		ScrollInfo.nPage	= 0;
		ScrollInfo.nPos		= 0;
		DeltaPos			= GetScrollPos (Bar);
	}

	ScrollInfo.nMin	= 0;
	ScrollInfo.nMax	= ScrollMax;

	SetScrollInfo (Bar, &ScrollInfo);
}



int CScrollWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int status = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

	SetFocus();

	return status;
}

void CScrollWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	const int	LineOffset	= 60;
	int			DeltaPos	= 0;
	SCROLLINFO	ScrollInfo	= {};
	INT			nPage;

	DBG_MSG((_T("CScrollWnd::OnHScroll - nSBCode=%u, nPos=%u, pScrollBar=%p\n"), nSBCode, nPos, pScrollBar));

	if (pScrollBar != NULL)
	{	return;
	}


	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask  = SIF_ALL;

	GetScrollInfo (SB_HORZ, &ScrollInfo);
	nPage = static_cast<INT>(ScrollInfo.nPage);

	switch( nSBCode )
	{
	case SB_LINELEFT:		{	DeltaPos = -LineOffset;				}	break;
	case SB_LINERIGHT:		{	DeltaPos =  LineOffset;				}	break;
	case SB_PAGELEFT:		{	DeltaPos = -nPage;					}	break;
	case SB_PAGERIGHT:		{	DeltaPos =  nPage;					}	break;
	case SB_THUMBTRACK:		{	DeltaPos = nPos - ScrollInfo.nPos;	}	break;
	case SB_THUMBPOSITION:	{	DeltaPos = nPos - ScrollInfo.nPos;	}	break;
	case SB_ENDSCROLL:		{	return;								}	break;
	default:				{	ASSERT (FALSE);	return;				}	break;
	}

	int newScrollPos = ScrollInfo.nPos + DeltaPos;

	if (newScrollPos < 0)
	{	DeltaPos = -ScrollInfo.nPos;
	}

	int maxScrollPos = m_DisplaySize.cx - nPage;
	if (newScrollPos > maxScrollPos)
	{	DeltaPos = maxScrollPos - ScrollInfo.nPos;
	}

	if (DeltaPos != 0)
	{
		SetScrollPos (SB_HORZ, ScrollInfo.nPos + DeltaPos);
		ScrollWindow (-DeltaPos, 0);
	}
}

void CScrollWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	const int	LineOffset	= 60;
	int			DeltaPos	= 0;
	SCROLLINFO	ScrollInfo	= {};
	INT			nPage;
	DBG_MSG((_T("CScrollWnd::OnVScroll - nSBCode=%u, nPos=%u, pScrollBar=%p\n"), nSBCode, nPos, pScrollBar));

	if (pScrollBar != NULL)
	{	return;
	}

	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask  = SIF_ALL;

	GetScrollInfo (SB_VERT, &ScrollInfo);
	nPage = static_cast<INT>(ScrollInfo.nPage);

	switch (nSBCode)
	{
	case SB_LINEUP:			{	DeltaPos = -LineOffset;				}	break;
	case SB_LINEDOWN:		{	DeltaPos =  LineOffset;				}	break;
	case SB_PAGEUP:			{	DeltaPos = -nPage;					}	break;
	case SB_PAGEDOWN:		{	DeltaPos =  nPage;					}	break;
	case SB_THUMBTRACK:		{	DeltaPos = nPos - ScrollInfo.nPos;	}	break;
	case SB_THUMBPOSITION:	{	DeltaPos = nPos - ScrollInfo.nPos;	}	break;
	case SB_ENDSCROLL:		{	return;								}	break;
	default:				{	ASSERT (FALSE); return;				}	break;
	}

	int newScrollPos = ScrollInfo.nPos + DeltaPos;

	if (newScrollPos < 0)
	{	DeltaPos = -ScrollInfo.nPos;
	}

	int maxScrollPos = m_DisplaySize.cy - nPage;
	if ( newScrollPos > maxScrollPos )
	{	DeltaPos = maxScrollPos - ScrollInfo.nPos;
	}

	if (DeltaPos != 0)
	{
		SetScrollPos (SB_VERT, ScrollInfo.nPos + DeltaPos);
		ScrollWindow (0, -DeltaPos);
	}
}

BOOL CScrollWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int		ScrollMin = 0;
	int		ScrollMax = 0;
	int		NumScrollLinesPerIncrement = 0;
	int		NumScrollIncrements = abs(zDelta) / WHEEL_DELTA;

	GetScrollRange (SB_VERT, &ScrollMin, &ScrollMax);
	
	if (ScrollMin == ScrollMax)
	{	return FALSE;
	}

	::SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &NumScrollLinesPerIncrement, 0);

	if (NumScrollLinesPerIncrement == WHEEL_PAGESCROLL)
	{
		OnVScroll ((zDelta > 0) ? SB_PAGEUP : SB_PAGEDOWN, 0, NULL);
		return TRUE;
	}

	int NumScrollLines = NumScrollIncrements * NumScrollLinesPerIncrement;

	NumScrollLines = max (NumScrollLines/3, 1);

	for(int i = 0; i < NumScrollLines; ++i)
	{
		OnVScroll ((zDelta > 0) ? SB_LINEUP : SB_LINEDOWN, 0, NULL);
	}

	return TRUE;
}

void CScrollWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	UpdateScrollInfo();
}


BOOL CScrollWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect Rect;

	pDC->GetClipBox (&Rect);

	pDC->FillSolidRect (&Rect, GetSysColor (COLOR_WINDOW));

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}
