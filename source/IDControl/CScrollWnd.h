#pragma once


class CScrollWnd : public CWnd
{
	DECLARE_DYNCREATE(CScrollWnd)
public:
	CScrollWnd();
	virtual ~CScrollWnd();

protected:
	// ClassWizard generated virtual function overrides.
	//{{AFX_VIRTUAL(CScrollWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions.
	//{{AFX_MSG(CScrollWnd)
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetScrollSizes (int nMapMode, SIZE sizeTotal, const SIZE& sizePage = CSize (0, 0), const SIZE& sizeLine = CSize (0, 0));

private:
	void	UpdateScrollInfo	();
	void	UpdateScrollBar		(int Bar, int WindowSize, int DisplaySize, LONG& DeltaPos);

private:
	CSize	m_DisplaySize;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

