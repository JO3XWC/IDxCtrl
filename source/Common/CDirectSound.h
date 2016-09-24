#pragma once
#include <Mmsystem.h>
#include <dsound.h>
#include "CDirectSoundCommon.h"
#include "CDirectSoundBuffer.h"

class CDirectSound
{
public:
	CDirectSound(void);
	~CDirectSound(void);

	ULONG	StartCapture	(HWND hWnd, LPCTSTR pszDeviceID);
	ULONG	StartRender		(HWND hWnd, LPCTSTR pszDeviceID);
	ULONG	Stop			();

	ULONG	Play			(PVOID pBuffer, ULONG Length);

	BOOL	IsRunning		();
	
	VOID	SetCallBack		(ESOUND_CALLBACK Callback, PVOID Param);

	static HRESULT	EnumCaptureDevice	(CArray<ESoundDevice>* pArray);
	static HRESULT	EnumRenderDevice	(CArray<ESoundDevice>* pArray);

private:
	static BOOL CALLBACK EnumCallback (LPGUID pGuid, LPCTSTR pDesc, LPCTSTR pModule, LPVOID pParam);

	static UINT CaptureThread (PVOID pParam);
	VOID CaptureThread ();

	static UINT RenderThread (PVOID pParam);
	VOID RenderThread ();

private:
	ESOUND_CALLBACK		m_CallbackFunc;
	PVOID				m_CallbackParam;
	HANDLE				m_hThread;
	HANDLE				m_hStopEvent;

	CString				m_strDeviceID;
	HWND				m_hWnd;

	CDirectSoundBufferPool			m_BufferPool;

	CCriticalSection	m_RenderListLock;
	CList<CDirectSoundBuffer*>		m_RenderList;
};
