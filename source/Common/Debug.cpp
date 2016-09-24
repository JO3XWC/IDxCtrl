#include "stdafx.h"
#include "Debug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

VOID _DebugBuf (PVOID pBuffer, ULONG Length)
{
	PUCHAR	pBuf	= reinterpret_cast<PUCHAR>(pBuffer);
	CString String;
	CString HexString;
	CString AscString;
	TCHAR* Ascii	=	_T("................")\
						_T("................")\
						_T(" !\"#$.&\'()*+,-./")\
						_T("0123456789:;<=>?")\
						_T("@ABCDEFGHIJKLMNO")\
						_T("PQRSTUVWXYZ[\\]^_")\
						_T("`abcdefghijklmno")\
						_T("pqrstuvwxyz{|}~.")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................")\
						_T("................");


	for (ULONG x=0;x<((Length/16)+1);x++)
	{
		HexString.Format (_T("%04X : "), x * 16);
		AscString = _T("");

		for (ULONG y=0;y<16;y++)
		{
			if (Length > (x*16+y))
			{
				HexString.AppendFormat (_T("%02X "), pBuf[x*16+y]);
				AscString+= Ascii[pBuf[x*16+y]];
			}else
			{
				HexString.Append (_T("   "));
				AscString.Append (_T(" "));
			}
		}
		String += HexString + AscString + _T("\r\n");
	}

	OutputDebugString (String);
}


CString _DebugTime ()
{
	SYSTEMTIME	Time;
	CString		strTime;

	::GetLocalTime (&Time);

	strTime.Format (_T("%02u:%02u:%02u:%03u"), Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);

	return strTime;
}

