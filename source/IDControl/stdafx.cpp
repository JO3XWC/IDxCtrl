
// stdafx.cpp : 標準インクルード IDControl.pch のみを
// 含むソース ファイルは、プリコンパイル済みヘッダーになります。
// stdafx.obj にはプリコンパイル済み型情報が含まれます。

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


UINT WM_SET_CALLSIGN = ::RegisterWindowMessage (_T("WM_SET_CALLSIGN"));
UINT WM_COMMON_VIEW = ::RegisterWindowMessage (_T("WM_COMMON_VIEW"));
UINT WM_ANALOG_FREQ_VIEW = ::RegisterWindowMessage (_T("WM_ANALOG_FREQ_VIEW"));
UINT WM_DIGITAL_FREQ_VIEW = ::RegisterWindowMessage (_T("WM_DIGITAL_FREQ_VIEW"));
