#pragma once
#include "CCustomImageWnd.h"

// CFreqDisplayCtrl

class CFreqDisplayCtrl : public CCustomImageWnd
{
	DECLARE_DYNAMIC(CFreqDisplayCtrl)

public:
	CFreqDisplayCtrl();
	virtual ~CFreqDisplayCtrl();

	VOID			SetFreq (ULONGLONG Freq);
	ULONGLONG		GetFreq ();

	VOID			SetCustomCursor (HCURSOR hCursor);
	VOID			SetDupOffset (ULONGLONG DupOffset);
	VOID			SetDupMode (ULONG DupMode);
	VOID			SetPttStatus (ULONG Status);
	VOID			SetMonitor (ULONG Monitor);
	VOID			SelectBand (ULONG Sel);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

private:
	ULONGLONG	m_Freq;
	ULONGLONG	m_DupOffset;
	ULONG		m_DupMode;
	ULONG		m_PttStatus;
	ULONG		m_Monitor;
	ULONG		m_BandSel;

	UINT		m_NotifyCode;
	INT			m_nRepeatTime;
	INT			m_nRepeatTimeDelay;
	HCURSOR		m_hCustomCursor;

	enum
	{
		PNG_ENABLE,
		PNG_DISABLE,
		PNG_PTT_ON,
		PNG_PTT_NG,
		PNG_PW_OFF,

		PNG_MAX
	};
	CImage	m_Image[PNG_MAX];
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	LRESULT SendParentNotify (UINT Code);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


