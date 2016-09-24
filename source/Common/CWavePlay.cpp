#include "StdAfx.h"
#include "CWavePlay.h"
#include <Mmsystem.h>
#pragma comment (lib, "winmm.lib") 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWavePlay::CWavePlay(void)
{
	m_pWaveBuffer = reinterpret_cast<PWAVE_HEADER>(new UCHAR[WAVE_BUFFER_SIZE]);
	::ZeroMemory (m_pWaveBuffer, WAVE_BUFFER_SIZE);
	m_Level = 10;
}

CWavePlay::~CWavePlay(void)
{
	POSITION Pos;
	ULONG Index;
	PWAVE_HEADER pWave;

	Pos = m_Map.GetStartPosition ();
	while (Pos != NULL)
	{
		m_Map.GetNextAssoc (Pos, Index, pWave);

		delete pWave;
	}

	delete m_pWaveBuffer;
}


VOID CWavePlay::Load (HINSTANCE hInstance, ULONG Index, ULONG ResID, ULONG Count)
{
	PWAVE_HEADER pWave = NULL;
	PWAVE_HEADER pResWave = NULL;

	for (ULONG i=0;i<Count;i++)
	{
		do
		{
			pResWave = LoadWave (hInstance, ResID + i);
			ASSERT (pResWave != NULL);
			if (pResWave == NULL)
			{
				break;
			}

			pWave = reinterpret_cast<PWAVE_HEADER>(new UCHAR[pResWave->Length + 8]);
			if (pWave == NULL)
			{
				break;
			}

			memcpy (pWave, pResWave, pResWave->Length + 8);

			m_Map.SetAt (Index + i, pWave);
		}
		while (0);
	}

}

PWAVE_HEADER CWavePlay::LoadWave (HINSTANCE hInstance, UINT Id)
{
	HRSRC			hrsrc	= NULL;
	HGLOBAL			hRData	= NULL;
	PWAVE_HEADER	pWave = NULL;

	do
	{
		hrsrc = ::FindResource (hInstance, MAKEINTRESOURCE (Id), _T("WAVE"));
		ASSERT (hrsrc != NULL);
		if (hrsrc == NULL)
		{
			break;
		}
		
		hRData = ::LoadResource (hInstance, hrsrc);
		ASSERT (hRData != NULL);
		if (hRData == NULL)
		{
			ASSERT (FALSE);
			break;
		}

		pWave = (PWAVE_HEADER)::LockResource(hRData);
		ASSERT (pWave != NULL);
		if (pWave == NULL)
		{
			break;
		}
	}
	while (0);

	return pWave;
}


PWAVE_HEADER CWavePlay::Init (ULONG Index)
{
	PWAVE_HEADER pWave = NULL;

	::ZeroMemory (m_pWaveBuffer, WAVE_BUFFER_SIZE);

	do
	{
		if (!m_Map.Lookup (Index, pWave))
		{	break;
		}

		memcpy (m_pWaveBuffer, pWave, sizeof (WAVE_HEADER) + pWave->DataChunkLength);
	}
	while (0);

	return m_pWaveBuffer;
}

PWAVE_HEADER CWavePlay::Append (ULONG Index)
{
	PWAVE_HEADER pWave = NULL;

	if (m_pWaveBuffer->Length == 0)
	{
		return Init (Index);
	}

	do
	{
		if (!m_Map.Lookup (Index, pWave))
		{	break;
		}

		if ((m_pWaveBuffer->DataChunkLength + sizeof (WAVE_HEADER) + pWave->DataChunkLength) > WAVE_BUFFER_SIZE)
		{	break;
		}

		memcpy ((PUCHAR)m_pWaveBuffer + sizeof (WAVE_HEADER) + m_pWaveBuffer->DataChunkLength, pWave + 1, pWave->DataChunkLength);

		m_pWaveBuffer->Length += pWave->DataChunkLength;
		m_pWaveBuffer->DataChunkLength += pWave->DataChunkLength;
	}
	while (0);

	return m_pWaveBuffer;
}

VOID CWavePlay::Stop ()
{
	::PlaySound (NULL, NULL, SND_PURGE);
}

VOID CWavePlay::Play (LPCSTR pszString)
{
	do
	{
		Stop ();

		Init (pszString[0]);
		for (int i=1;i<lstrlenA (pszString);i++)
		{
			Append (pszString[i]);
		}

		VolumeApply ();

		if (m_Level == 0)
		{	break;
		}

		::PlaySound ((LPCTSTR)m_pWaveBuffer, NULL, SND_MEMORY | SND_ASYNC);
	}
	while (0);
}

// 0 - 20 - 39
VOID CWavePlay::SetVolume (ULONG Level)
{
	m_Level = Level;
	if (m_Level > 39)
	{	m_Level = 39;
	}
}
ULONG CWavePlay::GetVolume ()
{
	return m_Level;
}

PWAVE_HEADER CWavePlay::VolumeApply ()
{
	double Level = (double)m_Level / 20;
	PSHORT pBuffer = (PSHORT)(m_pWaveBuffer + 1);

	do
	{
		if (m_pWaveBuffer == NULL)
		{	break;
		}

		if (m_pWaveBuffer->DataChunkLength == 0)
		{	break;
		}

		if (m_pWaveBuffer->wChannels == 0)
		{	break;
		}

		for (ULONG i=0;i<(m_pWaveBuffer->DataChunkLength/m_pWaveBuffer->wChannels);i++)
		{
			for (int ch=0;ch<m_pWaveBuffer->wChannels;ch++)
			{
				pBuffer[i * m_pWaveBuffer->wChannels + ch] = (SHORT)(pBuffer[i * m_pWaveBuffer->wChannels + ch] * Level);
			}
		}
	}
	while (0);

	return m_pWaveBuffer;
}
