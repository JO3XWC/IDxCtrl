#pragma once
#include "CCustomDC.h"


// CCustomButton

class CCustomButton : public CButton
{
	DECLARE_DYNAMIC(CCustomButton)

public:
	CCustomButton();
	virtual ~CCustomButton();

	VOID		Initialize		();

protected:
	DECLARE_MESSAGE_MAP()
public:
	VOID SetCustomDC (CCustomDC* pDC);
	VOID SetCustomCursor (HCURSOR hCursor);
	VOID SetCustomBkIndex (INT Index);
	INT GetCustomBkIndex ();

private:
	INT				m_BkIndex;
	CCustomDC*		m_pDC;
	HCURSOR			m_hCustomCursor;

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


