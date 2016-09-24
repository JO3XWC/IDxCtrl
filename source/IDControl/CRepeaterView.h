
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree ウィンドウ

class CViewTree : public CTreeCtrl
{
// コンストラクション
public:
	CViewTree();

// オーバーライド
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 実装
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};


class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CRepeaterView : public CDockablePane
{
// コンストラクション
public:
	CRepeaterView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	VOID ClearRepeaterList ();
	VOID LoadRepeaterList (LPCTSTR pszFileName);

// 属性
protected:

	CViewTree m_wndTreeView;
	CImageList m_TreeViewImages;

protected:

// 実装
public:
	virtual ~CRepeaterView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnSetToCallsign();
	afx_msg void OnUpdateSetToCallsign(CCmdUI *pCmdUI);

	afx_msg void OnSetFromCallsign();
	afx_msg void OnUpdateSetFromCallsign(CCmdUI *pCmdUI);

	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitemTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnExpanded(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

