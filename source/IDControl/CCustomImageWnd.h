#pragma once


// CCustomImageWnd

class CCustomImageWnd : public CWnd
{
	DECLARE_DYNAMIC(CCustomImageWnd)

public:
	CCustomImageWnd();
	virtual ~CCustomImageWnd();

	HRESULT LoadPng (UINT nID, CImage* pImage);

private:
	VOID ConvertAlpha (CImage* pImage);

protected:
	DECLARE_MESSAGE_MAP()
};


