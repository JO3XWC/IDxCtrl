
#pragma once
#include "CScrollWnd.h"
#include "CCustomDC.h"
#include "CCustomButton.h"
#include "CCustomComboBox.h"
#include "CFreqDisplayCtrl.h"
#include "CSMeterDisplayCtrl.h"


class CAnalogFreqScrollView : public CScrollWnd
{
    DECLARE_DYNCREATE(CAnalogFreqScrollView)

public:
    CAnalogFreqScrollView();
    virtual ~CAnalogFreqScrollView();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);

protected:
    DECLARE_MESSAGE_MAP()

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnBtnClicked (UINT nID);
	afx_msg void OnCmbSelChange (UINT nID);
	afx_msg void OnFreqRClick(NMHDR* pNmHdr, LRESULT* pResult);
	afx_msg void OnFreqLClick(NMHDR* pNmHdr, LRESULT* pResult);

public:
	CCustomDC			m_DC;

	CCustomButton		m_Band;

	CCustomComboBox		m_Sql;
	CCustomComboBox		m_Vol;
	CCustomComboBox		m_Vfo;
	CCustomComboBox		m_Dup;
	CCustomComboBox		m_Mode;

	CFreqDisplayCtrl	m_FreqCtrl;
	CSMeterDisplayCtrl	m_SMeterCtrl;
public:
	enum
	{
		IDC_NONE,

		IDC_BTN_START,
		IDC_BTN_BAND,
		IDC_BTN_MAX,

		IDC_CMB_START,
		IDC_CMB_SQL,
		IDC_CMB_VOL,
		IDC_CMB_VFO,
		IDC_CMB_DUP,
		IDC_CMB_MODE,
		IDC_CMB_MAX,

		IDC_FREQ,
		IDC_S_METER,
	};
};


class CAnalogFreqView : public CDockablePane
{
public:
	CAnalogFreqView();
	virtual ~CAnalogFreqView();

	void AdjustLayout();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);

// オーバーライド
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);


	DECLARE_MESSAGE_MAP()

private:
	CAnalogFreqScrollView	m_View;
};

