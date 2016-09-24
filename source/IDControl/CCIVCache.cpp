#include "stdafx.h"
#include "CCIVCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCIVCache::CCIVCache()
{
}

CCIVCache::~CCIVCache()
{
	Clear ();
}


VOID CCIVCache::Clear ()
{
	CSingleLock		Lock (&m_MapLock, TRUE);
	POSITION		Pos;
	ULONG			Civ;
	CMemFileEx*		pData;

	Pos = m_Map.GetStartPosition ();
	while (Pos != NULL)
	{
		m_Map.GetNextAssoc (Pos, Civ, pData);

		delete pData;
	}

	m_Map.RemoveAll ();
}

VOID CCIVCache::Set (ULONG Civ, PVOID pBuffer, ULONG Length)
{
	CSingleLock		Lock (&m_MapLock, TRUE);
	CMemFileEx*		pData;

	if (!m_Map.Lookup (Civ, pData))
	{
		pData = new CMemFileEx;
		m_Map.SetAt (Civ, pData);
	}

	pData->SetLength (0);
	pData->Write (pBuffer, Length);
}

ULONG CCIVCache::GetCache (PVOID pBuffer, ULONG Length)
{
	CSingleLock		Lock (&m_MapLock, TRUE);
	CMemFileEx*		pData;
	ULONG			l	= 0;
	PUCHAR			p	= reinterpret_cast<PUCHAR>(pBuffer);
	PACKET_CMD_HEADER Header;

	DBG_MSG((_T("CCIVCache::GetCache \n")));

	for (ULONG i=0;i<g_nCiv_All_Status;i++)
	{
		if (m_Map.Lookup (g_Civ_All_Status[i], pData))
		{
			if (Length < (l + pData->GetLength () + sizeof (Header)))
			{	
				ASSERT (FALSE);
				break;
			}
			DBG_BUF (pData->GetBuffer (), static_cast<ULONG>(pData->GetLength ()));

			Header.m_Length = static_cast<ULONG>(pData->GetLength () + sizeof (Header));
			Header.m_Type = TYPE_SERIAL;

			memcpy (p, &Header, sizeof (Header));
			p += sizeof (Header);
			memcpy (p, pData->GetBuffer (), static_cast<ULONG>(pData->GetLength ()));
			p += pData->GetLength ();

			l += Header.m_Length;
		}
	}

	return l;
}

ULONG CCIVCache::Lookup (ULONG CIV, PVOID pBuffer, ULONG Length)
{
	CSingleLock		Lock (&m_MapLock, TRUE);
	CMemFileEx*		pData;
	ULONG			ResultLength	= 0;

	do
	{
		if (!m_Map.Lookup (CIV, pData))
		{	break;
		}

		if (Length < pData->GetLength ())
		{	
			ASSERT (FALSE);
			break;
		}

		ResultLength = static_cast<ULONG>(pData->GetLength ());
		DBG_BUF (pData->GetBuffer (), ResultLength);
		memcpy (pBuffer, pData->GetBuffer (), ResultLength);
	}
	while (0);

	return ResultLength;
}
