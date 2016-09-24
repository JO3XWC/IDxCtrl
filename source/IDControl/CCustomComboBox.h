#pragma once
#include "CCustomDC.h"

// CCustomComboBox


class CCustomComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCustomComboBox)

public:
	CCustomComboBox();
	virtual ~CCustomComboBox();
	VOID		Initialize		();


public:
	VOID SetCustomDC (CCustomDC* pDC);
	VOID SetCustomCursor (HCURSOR hCursor);

	VOID SetCustomBkIndex (INT Index);
	INT GetCustomBkIndex ();
	VOID SelectValue (DWORD_PTR Data);
	DWORD_PTR GetSelectedItemData ();

	VOID	SetTitle (LPCTSTR pszText);
	LPCTSTR	GetTitle ();
	VOID	SetTextFormat (UINT TitleFormat, UINT TextFormat);
	
protected:
	VOID UpdateDropDownWidth ();


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

private:
	INT				m_BkIndex;
	CCustomDC*		m_pDC;
	HCURSOR			m_hCustomCursor;
	HWND			m_hListbox;
	CString			m_strTitle;
	UINT			m_TitleFormat;
	UINT			m_TextFormat;

public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnCbnDropdown();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


