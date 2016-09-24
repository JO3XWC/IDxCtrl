
#pragma once

class CMessageHistoryView : public CDockablePane
{
public:
	CMessageHistoryView();
	virtual ~CMessageHistoryView();

	void AdjustLayout();
	VOID AddHistory (CStringArray* pArray);
	VOID FreeHistory ();
	VOID LoadHistory ();

protected:
	CListCtrlEx m_wndListCtrl;

// オーバーライド
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	VOID AddRxMessage (PVOID pBuffer, ULONG Length);

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
		COLUMN_MESSAGE,
		
		COLUMN_MAX,

	};
};

