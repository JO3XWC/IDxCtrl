#pragma once
#include <winsock2.h>
#include "CTcpCommon.h"



class CTcpServer
{
public:
	class CTcpServerClient
	{
	public:
		CTcpServerClient ()
		{
			m_hSocket		= INVALID_SOCKET;
			m_pServer		= NULL;
			m_hSendEvent	= ::CreateEvent (NULL, FALSE, FALSE, NULL);
		}

		virtual ~CTcpServerClient ()
		{
			Close ();

			if (m_hSendEvent != NULL)
			{
				::CloseHandle (m_hSendEvent);
				m_hSendEvent = NULL;
			}
		}

		VOID Close ()
		{
			if (m_hSocket != INVALID_SOCKET)
			{
				::closesocket (m_hSocket);
				m_hSocket = INVALID_SOCKET;
			}
		}

		VOID Send (PVOID pBuffer, ULONG Length)
		{
			//CSingleLock Lock (m_SendPacket.GetLock (), TRUE);

			m_SendPacket.Write (pBuffer, Length);
			::SetEvent (m_hSendEvent);
		}

	public:
		SOCKET				m_hSocket;
		CTcpServer*			m_pServer;
		HANDLE				m_hSendEvent;
		CTcpPacket			m_SendPacket;
	};


public:
	CTcpServer(void);
	virtual ~CTcpServer(void);
public:
	static ULONG	GetAddress		(LPCTSTR pszAddress);

	virtual ULONG	Open			(USHORT Port);
	virtual ULONG	Close			();

protected:
	virtual CTcpServerClient*	OnAllocClient ();

	virtual VOID	OnConnect		(CTcpServerClient* pClient);
	virtual VOID	OnDisconnect	(CTcpServerClient* pClient);
	virtual VOID	OnWaitTimeout	(CTcpServerClient* pClient, ULONG TimeoutCount);

	virtual VOID	OnReceive		(CTcpServerClient* pClient, PVOID pBuffer, ULONG Length);

private:
	static UINT ListenThread	(PVOID pParam);
	static UINT ClientThread	(PVOID pParam);

private:
	HANDLE				m_hStopEvent;
	HANDLE				m_hServerThread;
	SOCKET				m_hServerSocket;
	USHORT				m_ServerPort;

	LONG				m_ClientCount;
	LONG				m_MaxClientCount;
	INT					m_Timeout;

protected:
	CCriticalSection	m_ClientMapLock;
	CMap<CTcpServerClient*, CTcpServerClient*, ULONG, ULONG> m_ClientMap;
};
