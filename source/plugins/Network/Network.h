// Network.h : Network.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CNetworkApp
// このクラスの実装に関しては Network.cpp を参照してください。
//

class CNetworkApp : public CWinApp
{
public:
	CNetworkApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
