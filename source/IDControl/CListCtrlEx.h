#pragma once
#include "CCustomDC.h"

//class CListCtrlExDC
//{
//public:
//	CListCtrlExDC ();
//	~CListCtrlExDC ();
//
//	VOID	Initialize		(CWnd* pWnd, INT cx, INT cy, COLORREF colorStart, COLORREF colorFinish, COLORREF colorText, COLORREF colorLine, BOOL bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);
//	VOID	Deinitialize	();
//	CDC*	GetDC			();
//	CSize	GetSize			();
//	COLORREF	GetLineColor ();
//	COLORREF	GetTextColor ();
//	CFont*		GetFont ();
//
//public:
//	CSize		m_Size;
//	CBitmap*	m_pBmp;
//	CBitmap*	m_pOldBmp;
//	CDC			m_MemDC;
//	COLORREF	m_ClrLine;
//	COLORREF	m_ClrText;
//	CFont		m_Font;
//};

class CListCtrlExHeader : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CListCtrlExHeader)
public:
		VOID		Initialize		();
	afx_msg void OnPaint();
	
	COLORREF	GetBkColor	() const { return m_BkColor; }
	VOID		SetBkColor	(_In_ COLORREF cr) { m_BkColor=cr;}

protected:
	DECLARE_MESSAGE_MAP()

private:
	COLORREF		m_BkColor;
	CCustomDC		m_DC;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

// CListCtrlEx

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

		VOID		Initialize		();
virtual VOID		OnInitialize	();

CListCtrlExHeader* GetHeaderCtrlEx () { return &m_HeaderCtrl;}
	

protected:
	DECLARE_MESSAGE_MAP()

private:
	CListCtrlExHeader	m_HeaderCtrl;
	CCustomDC			m_DC;

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


