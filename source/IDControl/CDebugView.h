
#pragma once


class CDebugView : public CDockablePane
{
public:
	CDebugView();
	virtual ~CDebugView();

	void AdjustLayout();
	VOID AddFormat (LPCTSTR pszFormat, ...);
	VOID AddDump (LPCTSTR pszMsg, PVOID pBuffer, ULONG Length);

protected:
	CListCtrlEx m_wndListCtrl;

// オーバーライド
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnDebugMsg (WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:
	CCriticalSection	m_DebugMsgArrayLock;
	CStringArray		m_DebugMsgArray;
	CStringArray		m_DebugMsgAddArray;
};

