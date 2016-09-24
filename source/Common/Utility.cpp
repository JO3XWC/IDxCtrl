#include "stdafx.h"
#include "utility.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CString MlLoadString (UINT nID)
{
	CString strText;

	if (!strText.LoadString (nID))
	{
	}

	return strText;
}

CString GetMyDir ()
{
	CString strMyDir;
	CString strTemp;

	strMyDir.ReleaseBuffer (::GetModuleFileName (NULL, strMyDir.GetBuffer (MAX_PATH), MAX_PATH));
	strTemp.ReleaseBuffer (::GetFullPathName (strMyDir, MAX_PATH, strTemp.GetBuffer (MAX_PATH), NULL));
	strMyDir = strTemp.Left (strTemp.ReverseFind ('\\') + 1);

	return strMyDir;
}

PVOID SWAPBCD (PVOID pDest, PVOID pSrc, ULONG Length)
{
	PUCHAR pS = reinterpret_cast<PUCHAR>(pSrc);
	PUCHAR pD = reinterpret_cast<PUCHAR>(pDest);

	for (ULONG i=0;i<Length;i++)
	{
		pD[Length - i - 1] = pS[i];
	}

	return pD;
}

ULONGLONG BCD2ULONGLONG (PVOID pBuffer, ULONG BufferLength)
{
	ULONGLONG Value = 0;
	PUCHAR p = reinterpret_cast<PUCHAR>(pBuffer);

	for (ULONG i=0;i<BufferLength;i++)
	{
		Value *= 100;
		Value += ((p[i] & 0xF0) >> 4) * 10;
		Value += ((p[i] & 0x0F) >> 0) * 1;
	}

	return Value;
}

ULONGLONG SWAPBCD2ULONGLONG (PVOID pBuffer, ULONG BufferLength)
{
	UCHAR Buff[1024];

	return BCD2ULONGLONG (SWAPBCD (Buff, pBuffer, BufferLength), BufferLength);
	
}

VOID ULONGLONG2BCD (ULONGLONG Value, PVOID pBuffer, ULONG BufferLength)
{
	PUCHAR p = reinterpret_cast<PUCHAR>(pBuffer);
	for (ULONG i=0;i<BufferLength;i++)
	{
		p[BufferLength - i - 1] = (Value % 10) ;
		Value /= 10;
		p[BufferLength - i - 1] |= (Value % 10) << 4; 
		Value /= 10;
	}
}

VOID ULONGLONG2SWAPBCD (ULONGLONG Value, PVOID pBuffer, ULONG BufferLength)
{
	UCHAR Buff[1024];

	ULONGLONG2BCD (Value, Buff, BufferLength);
	SWAPBCD (pBuffer, Buff, BufferLength);
}

CString DATA2STR (PVOID pBuffer, ULONG BufferLength)
{
	CStringA strA;
	strA.SetString (reinterpret_cast<CHAR*>(pBuffer), BufferLength);
	return CString (strA);
}

PVOID STR2DATA (LPCTSTR pszString, PVOID pBuffer, ULONG BufferLength)
{
	CStringA strA (pszString);
	memcpy (pBuffer, strA.GetString (), min (strA.GetLength (), (INT)BufferLength));
	return pBuffer;
}

//
//ULONG BCD2ULONG (PVOID pBuffer, ULONG BufferLength)
//{
//	return static_cast<ULONG>(BCD2ULONGLONG (pBuffer, BufferLength));
//}
//
//USHORT BCD2USHORT (PVOID pBuffer, ULONG BufferLength)
//{
//	return static_cast<USHORT>(BCD2ULONGLONG (pBuffer, BufferLength));
//}
//
//VOID ULONGLONG2BCD (ULONGLONG Value, PVOID pBuffer, ULONG BufferLength)
//{
//	PUCHAR p = reinterpret_cast<PUCHAR>(pBuffer);
//	for (ULONG i=0;i<BufferLength;i++)
//	{
//		p[BufferLength - i - 1] = (Value % 10) ;
//		Value /= 10;
//		p[BufferLength - i - 1] |= (Value % 10) << 4; 
//		Value /= 10;
//	}
//}
//
//VOID ULONG2BCD (ULONG Value, PVOID pBuffer, ULONG BufferLength)
//{
//	ULONGLONG2BCD (Value, pBuffer, BufferLength);
//}
//
//VOID USHORT2BCD (USHORT Value, PVOID pBuffer, ULONG BufferLength)
//{
//	ULONGLONG2BCD (Value, pBuffer, BufferLength);
//}
//

BOOL FileReadString (LPCTSTR pszFileName, CString* pstrString)
{
	BOOL			Result = FALSE;
	PUCHAR			pBuffer		= NULL;
	ULONG			BuffLen;
	CStringA		strA;
	CFile			File;
	CFileException	Excep;

	do
	{
		if (!File.Open (pszFileName, CFile::modeRead, &Excep))
		{	break;
		}

		BuffLen = static_cast<ULONG>(File.GetLength ());
		pBuffer = new UCHAR[BuffLen + 1];
		pBuffer[BuffLen] = 0;

		if (File.Read (pBuffer, BuffLen) != BuffLen)
		{	break;
		}

		if ((pBuffer[0] == 0xFF) && (pBuffer[1] == 0xFE))
		{
			pstrString->SetString (reinterpret_cast<PWCHAR>(pBuffer + 2), ((BuffLen - 2) / sizeof (WCHAR)));
		}
		else
		{
			strA.SetString (reinterpret_cast<PCHAR>(pBuffer), BuffLen);
			*pstrString = strA;
		}

		Result = TRUE;
	}
	while (0);

	if (pBuffer != NULL)
	{
		delete[] pBuffer;
	}

	return Result;
}


INT_PTR CSV_Split (CString String, CStringArray* pArray)
{
	CString		strValue;
	INT			Index;

	if (pArray == NULL)
	{	return 0;	
	}
	
	pArray->RemoveAll ();

	do
	{
		String.Trim ();

		if (String.Left (1) == _T("\""))
		{
			String = String.Mid (1);
			Index = 0;
			while (1)
			{
				Index = String.Find (_T("\""), Index);
				if (Index == -1)
				{
					Index = String.GetLength ();
					break;
				}
				if (Index < (String.GetLength () - 1))
				{
					if (String.GetAt (Index + 1) == '\"')
					{	Index+=2;
						continue;
					}
				}
				break;
			}

			strValue = String.Left (Index);
			strValue.Replace (_T("\"\""), _T("\""));
			String = String.Mid (Index + 1);

			Index = String.Find (_T(","));
			if (Index != -1)
			{	String = String.Mid (Index + 1);
			}
		}
		else
		{
			Index = String.Find (_T(","));
			if (Index != -1)
			{
				strValue = String.Left (Index);
				String = String.Mid (Index + 1);
			}
			else
			{	strValue = String;
			}
		}

		strValue.Trim ();
		
		pArray->Add (strValue);

	}
	while (Index != -1);

	return 	pArray->GetCount ();
}

USHORT GPS_Crc (LPCSTR pszGPS, ULONG Length)
{
	USHORT	Crc = 0xFFFF;
	UCHAR	Data;

	for (ULONG i=0;i<Length;i++)
	{
		Data = pszGPS[i];

		for(int j=0;j<8;j++)
		{
			if(((Crc ^ Data) & 0x01) == 0x01)
			{
				Crc = (Crc >> 1) & 0x7FFF;
				Crc ^= 0x8408;
			}
			else
			{
				Crc = (Crc >> 1) & 0x7FFF;
			}

			Data = (Data >> 1) & 0x7F;
		}
	}

    return ~Crc & 0xFFFF;
}