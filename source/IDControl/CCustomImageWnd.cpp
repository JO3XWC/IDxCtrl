// CCustomImageWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CCustomImageWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CCustomImageWnd

IMPLEMENT_DYNAMIC(CCustomImageWnd, CWnd)

CCustomImageWnd::CCustomImageWnd()
{

}

CCustomImageWnd::~CCustomImageWnd()
{
}

VOID CCustomImageWnd::ConvertAlpha (CImage* pImage)
{
	//画像をそのまま使うと色がおかしくなるので補正する
	unsigned char * pCol = 0;
	long lW = pImage->GetWidth();
	long lH = pImage->GetHeight();
	for (long ixy = 0; ixy < lH; ixy ++)
	{
		for (long ixx = 0; ixx < lW; ixx ++)
		{
			pCol = (unsigned char *)pImage->GetPixelAddress(ixx,ixy);
			unsigned char alpha = pCol[3];
			if(alpha < 255)
			{
				pCol[0] = ((pCol[0] * alpha) + 127) / 255;
				pCol[1] = ((pCol[1] * alpha) + 127) / 255;
				pCol[2] = ((pCol[2] * alpha) + 127) / 255;
			}
		}
	}
}

HRESULT CCustomImageWnd::LoadPng (UINT nID, CImage* pImage)
{
	CImage		Image;
	HRESULT		Result			= S_OK;
	HRSRC		hResource		= NULL;
	DWORD		ImageSize		= 0;
	PVOID		pResourceData	= NULL;
	HGLOBAL		hLoadResource	= NULL;
	HGLOBAL		hBuffer			= NULL;
	PVOID		pBuffer			= NULL;
	IStream*	pStream			= NULL;

	do
	{
		hResource = ::FindResource (AfxGetResourceHandle (), MAKEINTRESOURCE (nID), _T("PNG"));
		if (hResource == NULL)
		{	break;
		}
	
		ImageSize = ::SizeofResource (AfxGetResourceHandle (), hResource);
		if (ImageSize == 0)
		{	break;
		}

		hLoadResource = ::LoadResource (AfxGetResourceHandle (), hResource);
		if (hLoadResource == NULL)
		{	break;
		}

		pResourceData = ::LockResource (hLoadResource);
		if (pResourceData == NULL)
		{	break;
		}

		hBuffer  = ::GlobalAlloc (GMEM_MOVEABLE, ImageSize);
		if (hBuffer == NULL)
		{	break;
		}

		pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer == NULL)
		{	break;
		}

		CopyMemory (pBuffer, pResourceData, ImageSize);

		if (::CreateStreamOnHGlobal (hBuffer, FALSE, &pStream) != S_OK)
		{	break;
		}

		Result = Image.Load (pStream);
		if (Result != S_OK)
		{	break;
		}

		ConvertAlpha (&Image);

		if (!pImage->Create (Image.GetWidth (), Image.GetHeight (), Image.GetBPP (), CImage::createAlphaChannel))
		{	
			Result = S_FALSE;
			break;
		}

		Image.BitBlt (pImage->GetDC (), 0, 0, SRCCOPY);
		pImage->ReleaseDC ();
	}
	while (0);

	Image.Destroy ();

	if (pBuffer != NULL)
	{
		::GlobalUnlock (hBuffer);
	}

	if (hBuffer != NULL)
	{
		::GlobalFree (hBuffer);
	}

	return Result;
}

BEGIN_MESSAGE_MAP(CCustomImageWnd, CWnd)
END_MESSAGE_MAP()



// CCustomImageWnd メッセージ ハンドラー


