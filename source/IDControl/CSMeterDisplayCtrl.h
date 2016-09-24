#pragma once
#include "CCustomImageWnd.h"

// CSMeterDisplayCtrl

class CSMeterDisplayCtrl : public CCustomImageWnd
{
	DECLARE_DYNAMIC(CSMeterDisplayCtrl)

public:
	CSMeterDisplayCtrl();
	virtual ~CSMeterDisplayCtrl();

	VOID			SetLevel (LONG Level);
	VOID			SelectBand (ULONG Sel);
	VOID			SetMonitor (ULONG Monitor);
	VOID			SetNoiseSql (ULONG Sql);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

private:
	LONG		m_MaxLevel;
	LONG		m_Level;
	ULONG		m_BandSel;
	ULONG		m_Monitor;
	ULONG		m_NoiseSql;

	enum
	{
		PNG_ENABLE,
		PNG_DISABLE,
		PNG_PW_OFF,

		PNG_MAX
	};
	CImage	m_Image[PNG_MAX];
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


