
#pragma once
#include "CScrollWnd.h"
#include "CCustomDC.h"
#include "CCustomButton.h"
#include "CCustomComboBox.h"
#include "CFreqDisplayCtrl.h"
#include "CSMeterDisplayCtrl.h"


class CDigitalFreqScrollView : public CScrollWnd
{
    DECLARE_DYNCREATE(CDigitalFreqScrollView)

public:
    CDigitalFreqScrollView();
    virtual ~CDigitalFreqScrollView();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);
	VOID OnRepeaterListChange ();
	VOID UpdateRxCs ();


protected:
    DECLARE_MESSAGE_MAP()

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnBtnClicked (UINT nID);
	afx_msg void OnCmbSelChange (UINT nID);
	afx_msg void OnCsCmbSelChange (UINT nID);
	afx_msg void OnCsMenuCmbSelChange (UINT nID);
	afx_msg void OnFreqRClick(NMHDR* pNmHdr, LRESULT* pResult);
	afx_msg void OnFreqLClick(NMHDR* pNmHdr, LRESULT* pResult);
	
	VOID ResetTxMsg ();
	VOID UpdateTxMsg ();
	VOID SetTxMsg (INT Index, LPCTSTR pszMsg);
	
	VOID ResetMyCallsign ();
	VOID UpdateMyCallsign ();
	VOID SetMyCallsign (INT Index, LPCTSTR pszCall);
	VOID SetTxCallSign (CRepeater* pRepeater, CString strUr, CString strR1, CString strR2);

	VOID RepeaterSelectChange (CRepeater* pRepeater);
public:
	CCustomDC			m_DC;
	CCustomDC			m_DC2;

	CCustomButton		m_Band;

	CCustomComboBox		m_Sql;
	CCustomComboBox		m_Vol;
	CCustomComboBox		m_Vfo;
	CCustomComboBox		m_Dup;
	CCustomComboBox		m_Mode;

	CFreqDisplayCtrl	m_FreqCtrl;
	CSMeterDisplayCtrl	m_SMeterCtrl;

	CCustomComboBox		m_UrMenu;
	CCustomComboBox		m_R1Menu;
	CCustomComboBox		m_R2Menu;
	CCustomComboBox		m_MyMenu;
	CCustomComboBox		m_TxMsgMenu;

	CCustomComboBox		m_UrCombo;
	CCustomComboBox		m_R1Combo;
	CCustomComboBox		m_R2Combo;
	CCustomComboBox		m_MyCombo;
	CCustomComboBox		m_TxMsgCombo;

	INT					m_LastTxMsg;
	INT					m_LastMyCallsign;
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

		IDC_CS_BTN_START,
		IDC_CS_BTN_MAX,

		IDC_CS_CMB_START,
		IDC_CS_CMB_UR,
		IDC_CS_CMB_R1,
		IDC_CS_CMB_R2,
		IDC_CS_CMB_MY_CALLSIGN_MEMCH,
		IDC_CS_CMB_TX_MSG_MEMCH,
		IDC_CS_CMB_MAX,

		IDC_CS_CMB_MENU_START,
		IDC_CS_CMB_UR_MENU,
		IDC_CS_CMB_R1_MENU,
		IDC_CS_CMB_R2_MENU,
		IDC_CS_CMB_MY_MENU,
		IDC_CS_CMB_MY_MENU_CALL1,
		IDC_CS_CMB_MY_MENU_CALL2,
		IDC_CS_CMB_MY_MENU_CALL3,
		IDC_CS_CMB_MY_MENU_CALL4,
		IDC_CS_CMB_MY_MENU_CALL5,
		IDC_CS_CMB_MY_MENU_CALL6,
		IDC_CS_CMB_MY_MENU_MEMO1,
		IDC_CS_CMB_MY_MENU_MEMO2,
		IDC_CS_CMB_MY_MENU_MEMO3,
		IDC_CS_CMB_MY_MENU_MEMO4,
		IDC_CS_CMB_MY_MENU_MEMO5,
		IDC_CS_CMB_MY_MENU_MEMO6,
		IDC_CS_CMB_TX_MSG_MENU,
		IDC_CS_CMB_TX_MSG_MENU_TXMSG1,//TX MSG
		IDC_CS_CMB_TX_MSG_MENU_TXMSG2,//TX MSG
		IDC_CS_CMB_TX_MSG_MENU_TXMSG3,//TX MSG
		IDC_CS_CMB_TX_MSG_MENU_TXMSG4,//TX MSG
		IDC_CS_CMB_TX_MSG_MENU_TXMSG5,//TX MSG
		IDC_CS_CMB_MENU_MAX,

	};

	CStringArray	m_TxMsgArray;
	CStringArray	m_MyCallsignArray;

	CArray<CRepeater*> m_RepeaterArray[100];

	ULONG			m_LastR1ComboGroup;

};


class CDigitalFreqView : public CDockablePane
{
public:
	CDigitalFreqView();
	virtual ~CDigitalFreqView();

	void AdjustLayout();

	VOID OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length);
	VOID OnRepeaterListChange ();
	VOID UpdateRxCs ();

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
	CDigitalFreqScrollView	m_View;
};

