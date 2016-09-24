
#pragma once

class CCallsignHistoryView : public CDockablePane
{
public:
	CCallsignHistoryView();
	virtual ~CCallsignHistoryView();

	void AdjustLayout();
	VOID AddHistory (CStringArray* pArray);
	VOID FreeHistory ();
	VOID LoadHistory ();

protected:
	CListCtrlEx m_wndListCtrl;

	VOID OnAddHistory (CStringArray* pArray);
// オーバーライド
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL AddRxCall (PVOID pBuffer, ULONG Length, CString* pstrCaller, CString* pstrCalled);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnHistory (WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:
	CCriticalSection		m_HistoryArrayLock;
	CArray<CStringArray*>	m_HistoryArray;
	CArray<CStringArray*>	m_HistoryAddArray;

	CFile					m_LogFile;
public:
	enum
	{
		COLUMN_DATETIME,
		COLUMN_CALLER,
		COLUMN_MEMO,
		COLUMN_CALLED,
		COLUMN_RX_RPT1,
		COLUMN_RX_RPT2,
		COLUMN_STATUS,
		
		COLUMN_MAX,

	};
};

