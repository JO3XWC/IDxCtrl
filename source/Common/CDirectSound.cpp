#include "StdAfx.h"
#include "CDirectSound.h"
#include "NoDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dxguid.lib")

#define	SAMPLE_FREQ		8000				// サンプリング周波数.

CDirectSound::CDirectSound(void)
{
	m_CallbackFunc				= NULL;
	m_CallbackParam				= NULL;
	m_hThread					= NULL;
	m_hWnd						= NULL;
	m_hStopEvent				= ::CreateEvent (NULL, TRUE, FALSE, NULL);
}

CDirectSound::~CDirectSound(void)
{
	Stop ();

	if (m_hStopEvent != NULL)
	{
		::CloseHandle (m_hStopEvent);
		m_hStopEvent = NULL;
	}

	while (!m_RenderList.IsEmpty ())
	{
		delete m_RenderList.RemoveHead ();
	}

}

VOID CDirectSound::SetCallBack (ESOUND_CALLBACK Callback, PVOID Param)
{
	m_CallbackFunc = Callback;
	m_CallbackParam = Param;
}

ULONG CDirectSound::StartCapture (HWND hWnd, LPCTSTR pszDeviceID)
{
	CWinThread* pThread;
	ULONG		Result = ERROR_SUCCESS;

	do
	{
		Stop ();

		m_hWnd			= hWnd;
		m_strDeviceID	= pszDeviceID;

		::ResetEvent (m_hStopEvent);

		pThread = AfxBeginThread (CaptureThread, this, 0, 0, CREATE_SUSPENDED);

		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			pThread->ResumeThread ();
			Result = ::GetLastError ();
			break;
		}

		pThread->ResumeThread ();
	}
	while (0);

	return Result;
}

ULONG CDirectSound::StartRender (HWND hWnd, LPCTSTR pszDeviceID)
{
	CWinThread* pThread;
	ULONG		Result = ERROR_SUCCESS;

	do
	{
		Stop ();

		m_hWnd			= hWnd;
		m_strDeviceID	= pszDeviceID;

		::ResetEvent (m_hStopEvent);

		pThread = AfxBeginThread (RenderThread, this, 0, 0, CREATE_SUSPENDED);

		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			pThread->ResumeThread ();
			Result = ::GetLastError ();
			break;
		}

		pThread->ResumeThread ();
	}
	while (0);

	return Result;
}

ULONG CDirectSound::Stop ()
{
	::SetEvent (m_hStopEvent);

	if (m_hThread != NULL)
	{
		::WaitForSingleObject (m_hThread, INFINITE);
		::CloseHandle (m_hThread);
		m_hThread = NULL;
	}

	while (!m_RenderList.IsEmpty ())
	{
		delete m_RenderList.RemoveHead ();
	}

	return ERROR_SUCCESS;
}

ULONG CDirectSound::Play (PVOID pBuffer, ULONG Length)
{
	CDirectSoundBuffer*		pBuf	= NULL;
	ULONG			Result	= ERROR_SUCCESS;

	do
	{
		if (!IsRunning ())
		{	
			Result = ERROR_NOT_READY;
			break;
		}

		pBuf = m_BufferPool.Alloc (pBuffer, Length);
		if (pBuf == NULL)
		{	
			Result = ERROR_INSUFFICIENT_BUFFER;
			break;
		}

		m_RenderListLock.Lock ();
		m_RenderList.AddTail (pBuf);
		m_RenderListLock.Unlock ();
	}
	while (0);

	return Result;
}

BOOL CDirectSound::IsRunning ()
{
	return (::WaitForSingleObject (m_hThread, 0) == WAIT_TIMEOUT);
}

BOOL CDirectSound::EnumCallback (LPGUID pGuid, LPCTSTR pDesc, LPCTSTR pModule, LPVOID pParam)
{
	CArray<ESoundDevice>* pArray = reinterpret_cast<CArray<ESoundDevice>*>(pParam);
	ESoundDevice Device;

	if (pGuid != NULL)
	{
		Device.m_Guid = *pGuid;
	}
	Device.m_strName = pDesc;
	Device.m_strID = pModule;


	pArray->Add (Device);
	return TRUE;
}

HRESULT CDirectSound::EnumCaptureDevice (CArray<ESoundDevice>* pArray)
{
	return DirectSoundCaptureEnumerate (EnumCallback, pArray);
}

HRESULT CDirectSound::EnumRenderDevice (CArray<ESoundDevice>* pArray)
{
	return DirectSoundEnumerate (EnumCallback, pArray);
}
UINT CDirectSound::RenderThread (PVOID pParam)
{
	reinterpret_cast<CDirectSound*>(pParam)->RenderThread ();
	return 0;
}

VOID CDirectSound::RenderThread ()
{
	HRESULT						hr;
	IDirectSound8*				pDs				= NULL;
	IDirectSoundBuffer*			pDsBuff			= NULL;
	IDirectSoundNotify8*		pDsNotify		= NULL;
	DSBUFFERDESC				BuffDesc		= {};
	DSBCAPS						DsCaps			= {};
	DSBPOSITIONNOTIFY			DsNotifyPos[2]	= {};
	GUID						Guid			= {};
	GUID*						pGuid			= NULL;
	CArray<ESoundDevice>		Array;
	ESoundDevice				Device;
	WAVEFORMATEX				WaveFormatEx	= {};
	ULONG						NotifyBuffSize	= 0;
	ULONG						Result;
	void*						pBuff1			= NULL;
	void*						pBuff2			= NULL;
	ULONG						Len1			= 0;
	ULONG						Len2			= 0;
	ULONG						WritePos		= 0;
	PUCHAR						pSilent			= NULL;
	HANDLE						Handles[_countof (DsNotifyPos) + 1]		= {};

	ASSERT (m_hWnd != NULL);
	DBG_MSG((_T("CDirectSound::RenderThread - START \n")));

	WaveFormatEx.cbSize			= 0;
	WaveFormatEx.wFormatTag		= WAVE_FORMAT_PCM;
	WaveFormatEx.nChannels		= 1;
	WaveFormatEx.nSamplesPerSec	= SAMPLE_FREQ;
	WaveFormatEx.wBitsPerSample	= 16;
	WaveFormatEx.nBlockAlign	= WaveFormatEx.nChannels * WaveFormatEx.wBitsPerSample / 8;
	WaveFormatEx.nAvgBytesPerSec= WaveFormatEx.nSamplesPerSec * WaveFormatEx.nBlockAlign;

	NotifyBuffSize = (WaveFormatEx.nAvgBytesPerSec * 80) / 1000;//80ms

	do
	{
		pSilent = new UCHAR[NotifyBuffSize];
		if (pSilent == NULL)
		{	break;
		}

		::ZeroMemory (pSilent, NotifyBuffSize);


		hr = EnumRenderDevice (&Array);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - EnumCaptureDevice failure hr=%08X \n"), hr));
			break;
		}

		for (int i=0;i<Array.GetCount ();i++)
		{
			Device = Array.GetAt (i);

			if (Device.m_strID == m_strDeviceID)
			{
				if (!Device.m_strID.IsEmpty ())
				{
					Guid = Device.m_Guid;
					pGuid = &Guid;
				}
				break;
			}
		}

		hr = ::DirectSoundCreate8 (pGuid, &pDs, NULL);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - DirectSoundCreate8 failure hr=%08X \n"), hr));
			break;
		}

		hr = pDs->SetCooperativeLevel (m_hWnd, DSSCL_PRIORITY);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - SetCooperativeLevel failure hr=%08X \n"), hr));
			break;
		}

		BuffDesc.dwSize			= sizeof(BuffDesc);
		BuffDesc.dwFlags		= DSBCAPS_GLOBALFOCUS
			| DSBCAPS_CTRLPOSITIONNOTIFY
			| DSBCAPS_CTRLVOLUME
			| DSBCAPS_CTRLPAN;
		BuffDesc.dwBufferBytes	= NotifyBuffSize * _countof (DsNotifyPos);
		BuffDesc.lpwfxFormat	= &WaveFormatEx;

		hr = pDs->CreateSoundBuffer (&BuffDesc, &pDsBuff, NULL);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - CreateSoundBuffer failure hr=%08X \n"), hr));
			break;
		}

		DsCaps.dwSize = sizeof (DsCaps);
		hr = pDsBuff->GetCaps (&DsCaps);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - pDsBuff->GetCaps failure hr=%08X \n"), hr));
			break;
		}

		hr = pDsBuff->QueryInterface (IID_IDirectSoundNotify, (LPVOID*)&pDsNotify);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - pDsBuff->QueryInterface failure hr=%08X \n"), hr));
			break;
		}

		for (int i=0;i<_countof (DsNotifyPos);i++)
		{
			DsNotifyPos[i].dwOffset		= NotifyBuffSize * i + (NotifyBuffSize / 2);
			DsNotifyPos[i].hEventNotify	= ::CreateEvent (NULL, FALSE, FALSE, NULL);
		}


		hr = pDsNotify->SetNotificationPositions (_countof (DsNotifyPos), DsNotifyPos);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - pDsNotify->SetNotificationPositions failure hr=%08X \n"), hr));
			break;
		}

		hr = pDsBuff->SetCurrentPosition (0);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - pDsBuff->SetCurrentPosition failure hr=%08X \n"), hr));
			break;
		}

		hr = pDsBuff->Play (0, 0, DSBPLAY_LOOPING);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::RenderThread - pDsBuff->Start failure hr=%08X \n"), hr));
			break;
		}
		

		//音声リストクリア
		m_RenderListLock.Lock ();
		while (!m_RenderList.IsEmpty ())
		{
			m_BufferPool.Free (m_RenderList.RemoveHead ());
		}
		m_RenderListLock.Unlock ();

		Handles[0] = m_hStopEvent;
		for (int i=0;i<_countof (DsNotifyPos);i++)
		{
			Handles[i + 1] = DsNotifyPos[i].hEventNotify;
		}

		while (1)
		{
			Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, INFINITE);

			if (Result == (WAIT_OBJECT_0 + 0))
			{	
				DBG_MSG((_T("CDirectSound::RenderThread - STOP Event\n")));
				break;
			}

			WritePos = (Result % _countof (DsNotifyPos)) * NotifyBuffSize;

			hr = pDsBuff->Lock (WritePos, NotifyBuffSize, &pBuff1, &Len1, &pBuff2, &Len2, 0 );
			DBG_MSG((_T("CDirectSound::RenderThread - Lock (hr=%08X, WritePos=%d, pBuff1=%p, Len1=%d, pBuff2=%p, Len2=%d)\n"), hr, WritePos, pBuff1, Len1, pBuff2, Len2));
			ASSERT (SUCCEEDED (hr));
		
			do
			{
				CSingleLock Lock (&m_RenderListLock, TRUE);
				CDirectSoundBuffer*	pBuff = NULL;

				if (!m_RenderList.IsEmpty ())
				{	
					//ASSERT (m_RenderList.GetCount () < 3);
					if (m_RenderList.GetCount () > 4)
					{
						while (m_RenderList.GetCount () > 1)
						{
							DBG_MSG((_T("CDirectSound::RenderThread - Remove!!!!!!!!!!!!! count=%u\n"), m_RenderList.GetCount ()));
							m_BufferPool.Free (m_RenderList.RemoveHead ());
						}
					}

					pBuff = m_RenderList.RemoveHead ();

					memcpy (pBuff1, pBuff->GetBuffer (), pBuff->GetLength ());

					m_BufferPool.Free (pBuff);
				}
				else
				{
					memcpy (pBuff1, pSilent, NotifyBuffSize);
				}
			}
			while (0);

			//! 読み出しの完了したバッファをアンロックする.
			hr = pDsBuff->Unlock (pBuff1, Len1, pBuff2, Len2); 
			//DBG_MSG((_T("CDirectSound::RenderThread - Unlock (hr=%08X)\n"), hr));

			//WritePos = (WritePos + Len1 + Len2) % DsCaps.dwBufferBytes;

		}
	}
	while (0);

	if (pDsBuff != NULL)
	{
		pDsBuff->Stop ();
		pDsBuff->Release ();
	}

	if (pDsNotify != NULL)
	{
		pDsNotify->Release ();
	}

	if (pDs != NULL)
	{
		pDs->Release ();
	}

	for (int i=0;i<_countof (DsNotifyPos);i++)
	{
		if (DsNotifyPos[i].hEventNotify != NULL)
		{
			::CloseHandle (DsNotifyPos[i].hEventNotify);
		}
	}

	if (pSilent != NULL)
	{
		delete[] pSilent;
	}

	DBG_MSG((_T("CDirectSound::RenderThread - END \n")));
}

UINT CDirectSound::CaptureThread (PVOID pParam)
{
	reinterpret_cast<CDirectSound*>(pParam)->CaptureThread ();
	return 0;
}

VOID CDirectSound::CaptureThread ()
{
	HRESULT						hr;
	IDirectSoundCapture8*		pDs				= NULL;
	IDirectSoundCaptureBuffer*	pDsBuff			= NULL;
	IDirectSoundNotify8*		pDsNotify		= NULL;
	DSCBUFFERDESC				DsBuffDesc		= {};
	DSCBCAPS					DsCaps			= {};
	DSBPOSITIONNOTIFY			DsNotifyPos[2]	= {};
	GUID						Guid			= {};
	GUID*						pGuid			= NULL;
	CArray<ESoundDevice>		Array;
	ESoundDevice				Device;
	WAVEFORMATEX				WaveFormatEx	= {};
	HANDLE						Handles[_countof (DsNotifyPos) + 1]		= {};
	ULONG						Result;
	ULONG						NotifyBuffSize	= 0;
	ULONG						ReadPos			= 0;
	void*						pBuff1			= NULL;
	void*						pBuff2			= NULL;
	ULONG						Len1			= 0;
	ULONG						Len2			= 0;
	PUCHAR						pBuffer			= NULL;

	ASSERT (m_hWnd != NULL);
	DBG_MSG((_T("CDirectSound::CaptureThread - START \n")));

	WaveFormatEx.cbSize			= 0;
	WaveFormatEx.wFormatTag		= WAVE_FORMAT_PCM;
	WaveFormatEx.nChannels		= 1;
	WaveFormatEx.nSamplesPerSec	= SAMPLE_FREQ;
	WaveFormatEx.wBitsPerSample	= 16;
	WaveFormatEx.nBlockAlign	= WaveFormatEx.nChannels * WaveFormatEx.wBitsPerSample / 8;
	WaveFormatEx.nAvgBytesPerSec= WaveFormatEx.nSamplesPerSec * WaveFormatEx.nBlockAlign;

	NotifyBuffSize = (WaveFormatEx.nAvgBytesPerSec * 80) / 1000;//80ms

	do
	{
		hr = EnumCaptureDevice (&Array);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - EnumCaptureDevice failure hr=%08X \n"), hr));
			break;
		}

		for (int i=0;i<Array.GetCount ();i++)
		{
			Device = Array.GetAt (i);

			if (Device.m_strID == m_strDeviceID)
			{
				if (!Device.m_strID.IsEmpty ())
				{
					Guid = Device.m_Guid;
					pGuid = &Guid;
				}
				break;
			}
		}

		hr = ::DirectSoundCaptureCreate8 (pGuid, &pDs, NULL);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - DirectSoundCaptureCreate8 failure hr=%08X \n"), hr));
			break;
		}

		DsBuffDesc.dwSize		= sizeof (DsBuffDesc);
		DsBuffDesc.dwFlags		= 0;
		DsBuffDesc.dwBufferBytes= NotifyBuffSize * _countof (DsNotifyPos);
		DsBuffDesc.lpwfxFormat	= &WaveFormatEx;
		
		DBG_MSG((_T("CDirectSound::CaptureThread - dwBufferBytes=%u \n"), DsBuffDesc.dwBufferBytes));

		hr = pDs->CreateCaptureBuffer (&DsBuffDesc, &pDsBuff, NULL);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - CreateCaptureBuffer failure hr=%08X \n"), hr));
			break;
		}

		DsCaps.dwSize = sizeof (DsCaps);
		hr = pDsBuff->GetCaps (&DsCaps);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - pDsBuff->GetCaps failure hr=%08X \n"), hr));
			break;
		}

		hr = pDsBuff->QueryInterface (IID_IDirectSoundNotify, (LPVOID*)&pDsNotify);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - pDsBuff->QueryInterface failure hr=%08X \n"), hr));
			break;
		}

		for (int i=0;i<_countof (DsNotifyPos);i++)
		{
			DsNotifyPos[i].dwOffset		= (NotifyBuffSize * (i + 1)) - 1;
			DsNotifyPos[i].hEventNotify	= ::CreateEvent (NULL, FALSE, FALSE, NULL);
		}

		hr = pDsNotify->SetNotificationPositions (_countof (DsNotifyPos), DsNotifyPos);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - pDsNotify->SetNotificationPositions failure hr=%08X \n"), hr));
			break;
		}

		hr = pDsBuff->Start (DSCBSTART_LOOPING);
		if (FAILED(hr))
		{
			DBG_MSG((_T("CDirectSound::CaptureThread - pDsBuff->Start failure hr=%08X \n"), hr));
			break;
		}

		Handles[0] = m_hStopEvent;
		for (int i=0;i<_countof (DsNotifyPos);i++)
		{
			Handles[i + 1] = DsNotifyPos[i].hEventNotify;
		}

		pBuffer = new UCHAR[DsCaps.dwBufferBytes];

		while (1)
		{
			Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, INFINITE);

			if (Result == (WAIT_OBJECT_0 + 0))
			{	
				DBG_MSG((_T("CDirectSound::CaptureThread - STOP Event\n")));
				break;
			}

			ReadPos = (Result - 1) * NotifyBuffSize;
			hr = pDsBuff->Lock (ReadPos, NotifyBuffSize, &pBuff1, &Len1, &pBuff2, &Len2, 0 );
			//DBG_MSG((_T("CDirectSound::CaptureThread - Lock (hr=%08X, ReadPos=%d, pBuff1=%p, Len1=%d, pBuff2=%p, Len2=%d)\n"), hr, ReadPos, pBuff1, Len1, pBuff2, Len2));

			memcpy (pBuffer, pBuff1, Len1);
			if (pBuff2 != NULL)
			{
				memcpy (pBuffer + Len1, pBuff2, Len2);
			}

			//! 読み出しの完了したバッファをアンロックする.
			hr = pDsBuff->Unlock (pBuff1, Len1, pBuff2, Len2); 
			//DBG_MSG((_T("CDirectSound::CaptureThread - Unlock (hr=%08X)\n"), hr));

			ReadPos = (ReadPos + Len1 + Len2) % DsCaps.dwBufferBytes;

			if (m_CallbackFunc != NULL)
			{
				m_CallbackFunc (pBuffer, Len1 + Len2, m_CallbackParam);
			}
		}
	}
	while (0);

	if (pDsBuff != NULL)
	{
		pDsBuff->Stop ();
		pDsBuff->Release ();
	}

	if (pDsNotify != NULL)
	{
		pDsNotify->Release ();
	}

	if (pDs != NULL)
	{
		pDs->Release ();
	}

	for (int i=0;i<_countof (DsNotifyPos);i++)
	{
		if (DsNotifyPos[i].hEventNotify != NULL)
		{
			::CloseHandle (DsNotifyPos[i].hEventNotify);
		}
	}

	if (pBuffer != NULL)
	{
		delete[] pBuffer;
	}
	DBG_MSG((_T("CDirectSound::CaptureThread - END \n")));
}

