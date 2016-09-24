
#pragma once
#include "CScrollWnd.h"
#include "CCustomDC.h"
#include "CCustomButton.h"
#include "CCustomComboBox.h"


class CCommonScrollView : public CScrollWnd
{
    DECLARE_DYNCREATE(CCommonScrollView)

public:
    CCommonScrollView();
    virtual ~CCommonScrollView();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);
	VOID SetSpeeachLevel (ULONG Level);

protected:
    DECLARE_MESSAGE_MAP()

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnBtnClicked (UINT nID);
	afx_msg void OnCmbSelChange (UINT nID);

public:
	CCustomDC			m_DC;

	CCustomButton		m_Power;
	CCustomButton		m_Monitor;
	CCustomButton		m_Dual;
	CCustomButton		m_Att;

	CCustomComboBox		m_RfPower;
	CCustomComboBox		m_SpeeachLv;

public:
	enum
	{
		IDC_NONE,

		IDC_BTN_START,
		IDC_BTN_MAIN_POWER,
		IDC_BTN_MONITOR,
		IDC_BTN_DUAL,
		IDC_BTN_ATT,
		IDC_BTN_MAX,

		IDC_CMB_START,
		IDC_CMB_RF_POWER,
		IDC_CMB_SPEEACH_LEVEL,
		IDC_CMB_MAX,
	};
};


class CCommonView : public CDockablePane
{
public:
	CCommonView();
	virtual ~CCommonView();

	void AdjustLayout();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);
	VOID SetSpeeachLevel (ULONG Level);

// オーバーライド
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnCommon (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CCommonScrollView	m_View;
};

