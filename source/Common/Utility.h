#pragma once

CString MlLoadString (UINT nID);
CString GetMyDir ();

PVOID		SWAPBCD				(PVOID pDest, PVOID pSrc, ULONG Length);
ULONGLONG	BCD2ULONGLONG		(PVOID pBuffer, ULONG BufferLength);
ULONGLONG	SWAPBCD2ULONGLONG	(PVOID pBuffer, ULONG BufferLength);
VOID		ULONGLONG2BCD		(ULONGLONG Value, PVOID pBuffer, ULONG BufferLength);
VOID		ULONGLONG2SWAPBCD	(ULONGLONG Value, PVOID pBuffer, ULONG BufferLength);

//ULONG		BCD2ULONG		(PVOID pBuffer, ULONG BufferLength);
//USHORT		BCD2USHORT		(PVOID pBuffer, ULONG BufferLength);
//VOID		ULONG2BCD		(ULONG Value, PVOID pBuffer, ULONG BufferLength);
//VOID		USHORT2BCD		(USHORT Value, PVOID pBuffer, ULONG BufferLength);

CString		DATA2STR		(PVOID pBuffer, ULONG BufferLength);
PVOID		STR2DATA		(LPCTSTR pszString, PVOID pBuffer, ULONG BufferLength);
INT_PTR CSV_Split (CString String, CStringArray* pArray);
BOOL FileReadString (LPCTSTR pszFileName, CString* pstrString);

USHORT GPS_Crc (LPCSTR pszGPS, ULONG Length);
