#include "stdafx.h"
#include "CTcpClient.h"
#include <MsTcpIp.h>
#include <ws2tcpip.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define TCP_TIMEOUT			(30 * 1000)

CTcpClient::CTcpClient(void)
{
	m_hSocket			= INVALID_SOCKET;
	m_Address			= 0;
	m_Port				= 0;
	m_hSocketEvent		= ::WSACreateEvent ();
	m_hSendEvent		= ::CreateEvent (NULL, FALSE, FALSE, NULL);
	m_hStopEvent		= ::CreateEvent (NULL, TRUE, FALSE, NULL);
	m_hSocketThread		= NULL;
	m_Timeout			= TCP_TIMEOUT;
	m_ThreadID			= 0;
}

CTcpClient::~CTcpClient(void)
{
	if (m_hSocketEvent != NULL)
	{
		WSACloseEvent (m_hSocketEvent);
		m_hSocketEvent = NULL;
	}

	if (m_hSendEvent != NULL)
	{
		::CloseHandle (m_hSendEvent);
		m_hSendEvent = NULL;
	}

	if (m_hStopEvent != NULL)
	{
		::CloseHandle (m_hStopEvent);
		m_hStopEvent = NULL;
	}
}


ULONG CTcpClient::GetAddress (LPCTSTR pszAddress)
{
	ULONG		dwIP		= 0;
	IN_ADDR		Addr		= {};
	ADDRINFOT*	pAddInfo	= NULL;
    ADDRINFOT*	ptr			= NULL;
	ADDRINFOT	Hints		= {};

	DBG_MSG((_T("CTcpClient::GetAddress - %s\n"), pszAddress));
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;

	do
	{
		if (lstrcmp (pszAddress, _T("255.255.255.255")) == 0)
		{	dwIP = 0xFFFFFFFF;
			break;
		}

		if (::InetPton(AF_INET, pszAddress, &Addr) == 1)
		{
			dwIP = Addr.S_un.S_addr;
			break;
		}

		if (GetAddrInfo (pszAddress, NULL, NULL, &pAddInfo) != ERROR_SUCCESS)
		{	
			ASSERT (FALSE);
			break;
		}

		for(ptr = pAddInfo; ptr != NULL ;ptr = ptr->ai_next)
		{
			switch(ptr->ai_family)
			{
			case AF_INET:
				{
					LPSOCKADDR_IN pAddrIn = reinterpret_cast<LPSOCKADDR_IN>(ptr->ai_addr);

					dwIP = pAddrIn->sin_addr.S_un.S_addr;
				}
				break;

			default:
				{
				}
				break;
			}
		}

	}
	while (0);
			
	if (pAddInfo != NULL)
	{
	    FreeAddrInfo (pAddInfo);
	}

	return ::ntohl (dwIP);
}




ULONG CTcpClient::Open (LPCTSTR pszAddress, USHORT Port)
{
	return CTcpClient::Open (GetAddress (pszAddress), Port);
}

ULONG CTcpClient::Open (ULONG Address, USHORT Port)
{
	INT			Result		= ERROR_SUCCESS;
	CWinThread*	pThread		= NULL;

	DBG_MSG((_T("CTcpClient::Start - this=%p Address=%u.%u.%u.%u, Port=%u \n")
		, this
		, ((PUCHAR)&Address)[3]
		, ((PUCHAR)&Address)[2]
		, ((PUCHAR)&Address)[1]
		, ((PUCHAR)&Address)[0]
		, Port));

	do
	{
		Close ();

		m_SendPacket.SetLength (0);

		m_Address	= Address;
		m_Port		= Port;

		::ResetEvent (m_hStopEvent);

		pThread = AfxBeginThread (ClientThread, this, 0, 0, CREATE_SUSPENDED);
		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hSocketThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			Result = ::GetLastError ();
			break;
		}

		if (pThread != NULL)
		{
			pThread->ResumeThread ();
			m_ThreadID = ::GetThreadId (pThread->m_hThread);
		}
	}
	while (0);

	return Result;
}

ULONG CTcpClient::Close ()
{
	DBG_MSG((_T("CTcpClient::Close - this=%p, m_hSocket=%p\n"), this, (void*)m_hSocket));

	::SetEvent (m_hStopEvent);
	if (m_hSocketThread != NULL)
	{
		if (::GetThreadId (::GetCurrentThread ()) != m_ThreadID)
		{
			::WaitForSingleObject (m_hSocketThread, INFINITE);
		}

		::CloseHandle (m_hSocketThread);
		m_hSocketThread = NULL;
	}

	return DeleteSocket ();
}


VOID CTcpClient::Send (PVOID pBuffer, ULONG Length)
{
	//CSingleLock Lock (m_SendPacket.GetLock (), TRUE);

	m_SendPacket.Write (pBuffer, Length);

	::SetEvent (m_hSendEvent);
}



ULONG CTcpClient::ReConnect ()
{
	INT					Result			= ERROR_SUCCESS;
	SOCKADDR_IN			SockAddr		= {0};
	WSANETWORKEVENTS	NetworkEvents	= {0};

	DBG_MSG((_T("CTcpClient::ReConnect - this=%p Address=%u.%u.%u.%u, Port=%u \n")
		, this
		, ((PUCHAR)&m_Address)[3]
		, ((PUCHAR)&m_Address)[2]
		, ((PUCHAR)&m_Address)[1]
		, ((PUCHAR)&m_Address)[0]
		, m_Port));

	do
	{
		DeleteSocket ();

		//ソケット作成
		Result = CreateSocket ();
		if (Result != ERROR_SUCCESS)
		{
			DBG_MSG((_T("CTcpClient::ReConnect - CreateSocket failure %d\n"), Result));
            break;
		}

		//接続開始
		Result = Connect (m_Address, m_Port);
		if (Result != ERROR_SUCCESS)
		{
			DBG_MSG((_T("CTcpClient::ReConnect - Connect failure %d\n"), Result));
            break;
		}

		//イベント待ち設定
		WSAEventSelect (m_hSocket, m_hSocketEvent, FD_READ | FD_CLOSE | FD_WRITE);

		DBG_MSG((_T("CTcpClient::ReConnect - WSAConnect success\n")));
	}
	while (0);

	if (Result != ERROR_SUCCESS)
	{
		DeleteSocket ();
	}

	return Result;
}

ULONG CTcpClient::CreateSocket ()
{
	ULONG				Result			= ERROR_SUCCESS;
	SOCKADDR_IN			Service			= {0};
    struct tcp_keepalive ka_tmp1;
    DWORD				ka_tmp2			= 0;

	do
	{
		m_hSocket = ::WSASocket (AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_hSocket == INVALID_SOCKET)
		{
            Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpClient::CreateSocket - Socket == INVALID_SOCKET\n")));
			break;
		}

		DBG_MSG((_T("CTcpClient::CreateSocket - this=%p, Socket=%p\n"), this, (void*)m_hSocket));

		Service.sin_family		= AF_INET;
		Service.sin_addr.s_addr	= INADDR_ANY;
		Service.sin_port		= 0;

		if (::bind (m_hSocket, reinterpret_cast<SOCKADDR*>(&Service), sizeof (Service)) == SOCKET_ERROR)
		{
			Result = ::WSAGetLastError ();
			DBG_MSG((_T("CTcpClient::CreateSocket - bind failure %d\n"), Result));
			break;
		}

        ka_tmp1.onoff				= 1;
        ka_tmp1.keepalivetime		= 30 * 1000;
        ka_tmp1.keepaliveinterval	= 1000;

        if (::WSAIoctl(m_hSocket, SIO_KEEPALIVE_VALS, (LPVOID)&ka_tmp1, sizeof(ka_tmp1), NULL, 0, &ka_tmp2, NULL, NULL) == SOCKET_ERROR)
        {
            Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpClient::CreateSocket - keepalive failure %d\n"), Result));
            break;
        }

		if (::setsockopt (m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&m_Timeout, sizeof (m_Timeout)) == SOCKET_ERROR)
		{
			Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpClient::CreateSocket - SO_RCVTIMEO failure %d\n"), Result));
			break;
		}

		if (::setsockopt (m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&m_Timeout, sizeof (m_Timeout)) == SOCKET_ERROR)
		{
			Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpClient::CreateSocket - SO_SNDTIMEO failure %d\n"), Result));
			break;
		}

	}
	while (0);

	return Result;
}

ULONG CTcpClient::DeleteSocket ()
{

	if (m_hSocket != INVALID_SOCKET)
	{
		::shutdown (m_hSocket, SD_BOTH);
		//::shutdown (m_hSocket, SD_SEND);

		WSAEventSelect (m_hSocket, m_hSocketEvent, 0);

		::closesocket (m_hSocket);
		
		m_hSocket = INVALID_SOCKET;
	}

	return ERROR_SUCCESS;
}



ULONG CTcpClient::Connect (ULONG Address, USHORT Port)
{
	ULONG				Result			= ERROR_SUCCESS;
	SOCKADDR_IN			SockAddr		= {0};
	WSANETWORKEVENTS	NetworkEvents	= {0};
	HANDLE				Handles[2]		= {};

	Handles[0] = m_hSocketEvent;
	Handles[1] = m_hStopEvent;

	do
	{
		//イベント待ち設定
		WSAEventSelect (m_hSocket, m_hSocketEvent, FD_CONNECT);
		::WSAResetEvent (m_hSocketEvent);

		ZeroMemory (&SockAddr, sizeof (SockAddr));
		SockAddr.sin_family				= AF_INET;
		SockAddr.sin_addr.S_un.S_addr	= ::htonl (Address);
		SockAddr.sin_port				= ::htons (Port);

		DBG_MSG((_T("CTcpClient::Connect - WSAConnect \n")));
		if (::WSAConnect (m_hSocket, (SOCKADDR*)&SockAddr, sizeof (SockAddr), NULL, NULL, NULL, NULL) != SOCKET_ERROR)
        {
            Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpClient::Connect - WSAConnect failure %d\n"), Result));
            break;
        }

loop:
		switch (::WSAWaitForMultipleEvents (_countof (Handles), Handles, FALSE, m_Timeout, FALSE))
		{
		case WSA_WAIT_EVENT_0:
			{
				if (::WSAEnumNetworkEvents (m_hSocket, m_hSocketEvent, &NetworkEvents) == SOCKET_ERROR)
				{
					Result = ::WSAGetLastError();
					DBG_MSG((_T("CTcpClient::Connect - WSAConnect WSAEnumNetworkEvents failure %d\n"), Result));
					break;
				}

				if (!(NetworkEvents.lNetworkEvents & FD_CONNECT))
				{
					Result = ERROR_INVALID_EVENTNAME;
					DBG_MSG((_T("CTcpClient::Connect - not FD_CONNECT failure %d, lNetworkEvents=0x%08X\n"), Result, NetworkEvents.lNetworkEvents));
					goto loop;
					break;
				}
		 
				if (NetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
				{
					Result = NetworkEvents.iErrorCode[FD_CONNECT_BIT];
					DBG_MSG((_T("CTcpClient::Connect - iErrorCode[FD_CONNECT_BIT]=%d\n"), NetworkEvents.iErrorCode[FD_CONNECT_BIT]));
					break;
				}

				Result = ERROR_SUCCESS;
				DBG_MSG((_T("CTcpClient::Connect - success \n")));

			}
			break;

		case (WSA_WAIT_EVENT_0 + 1):
			{
				Result = ERROR_INVALID_HANDLE;
				DBG_MSG((_T("CTcpClient::Connect - WSAConnect WSAWaitForMultipleEvents failure %d, m_Timeout=%d, Result=%d\n"), Result, m_Timeout, Result));
			}
			break;

		default:
			{
				Result = ::WSAGetLastError();
				DBG_MSG((_T("CTcpClient::Connect - WSAConnect WSAWaitForMultipleEvents failure %d, m_Timeout=%d, Result=%d\n"), Result, m_Timeout, Result));
			}
			break;
		}
	}
	while (0);

	return Result;
}


UINT CTcpClient::ClientThread(LPVOID pParam)
{
	return ((CTcpClient*)pParam)->ClientThread ();
}

UINT CTcpClient::ClientThread()
{
	ULONG				Result;
	PCHAR				pBuffer			= NULL;
	ULONG				BufferLen		= 64 * 1024;
	ULONG				RecvLen;
	HANDLE				Handles[3]		= {};
	WSANETWORKEVENTS	NetworkEvents	= {0};
    WSAOVERLAPPED       WsaOverlap      = {0};
	ULONG				TimeoutCount	= 0;
	ULONG				ReConnectWaitTime	= 0;
	ULONG				BytesSent;
	WSABUF				SendBufs;
	ULONG				Flags = 0;

	DBG_MSG((_T("CTcpClient::ClientThread - START, this=%p \n"), this));

	Handles[0] = m_hStopEvent;
	Handles[1] = m_hSocketEvent;
    Handles[2] = m_hSendEvent;

	pBuffer = new CHAR[BufferLen];
	WsaOverlap.hEvent = ::WSACreateEvent ();

	while (WaitForSingleObject (m_hStopEvent, 0) == WAIT_TIMEOUT)
	{
		if (m_hSocket == INVALID_SOCKET)
		{
			if (WaitForSingleObject (m_hStopEvent, ReConnectWaitTime) == WAIT_OBJECT_0)
			{	break;
			}

			ReConnectWaitTime = 5000;//5 SEC

			DBG_MSG((_T("CTcpClient::ClientThread - ReConnect \n")));
			Result = ReConnect ();

			if (Result == ERROR_SUCCESS)
			{
				OnConnect ();
			}
			else
			{
				DBG_MSG((_T("CTcpClient::ClientThread - Connect failure %d\n"), Result));
				continue;
			}
		}


		Result = ::WSAWaitForMultipleEvents (_countof (Handles), Handles, FALSE, 1000, FALSE);

		if (Result == WSA_WAIT_TIMEOUT)
		{
			TimeoutCount++;
			OnWaitTimeout (TimeoutCount);
		}
		else if (Result == WSA_WAIT_EVENT_0)
		{
			Result = ERROR_EVENT_DONE;
			DBG_MSG((_T("CTcpClient::ClientThread - CLOSE  \n")));
			break;
		}
		else if (Result == (WSA_WAIT_EVENT_0 + 1))//SocketEvent
		{
			do
			{
				if (::WSAEnumNetworkEvents (m_hSocket, m_hSocketEvent, &NetworkEvents) == SOCKET_ERROR)
				{
					Result = ::WSAGetLastError();
					DBG_MSG((_T("CTcpClient::ClientThread - WSAConnect WSAEnumNetworkEvents failure %d\n"), Result));
					break;
				}

				if ((NetworkEvents.lNetworkEvents & FD_READ))// && (NetworkEvents.iErrorCode[FD_READ_BIT] == 0))
				{
					//reset timecount
					TimeoutCount = 0;

					RecvLen = recv (m_hSocket, pBuffer, BufferLen, 0);
					if (RecvLen == SOCKET_ERROR)
					{
						DBG_MSG((_T("CTcpClient::ClientThread - recv SOCKET_ERROR \n")));
						break;
					}

					OnReceive (pBuffer, RecvLen);
				}

				if (NetworkEvents.lNetworkEvents & FD_WRITE)
				{
				}

				if (NetworkEvents.lNetworkEvents & FD_CLOSE)
				{
					DBG_MSG((_T("CTcpClient::ClientThread - FD_CLOSE \n")));
					
					OnDisconnect ();

					Result = WSAECONNABORTED;
					DeleteSocket ();

					break;
				}
			}
			while (0);
		}
		else if (Result == (WSA_WAIT_EVENT_0 + 2))//SendEvent
		{
			CSingleLock		Lock (m_SendPacket.GetLock (), TRUE);

			SendBufs.len = static_cast<ULONG>(m_SendPacket.GetLength ());
			SendBufs.buf = reinterpret_cast<CHAR*>(m_SendPacket.GetBuffer ());
				
			do
			{
				if (::WSASend (m_hSocket, &SendBufs, 1, &BytesSent, 0, &WsaOverlap, NULL) != SOCKET_ERROR)
				{   
					break;
				}

				if (::WSAGetLastError () != ERROR_IO_PENDING)
				{
					Result = ::WSAGetLastError ();
					break;
				}

				if (::WSAGetOverlappedResult (m_hSocket, &WsaOverlap, &BytesSent, TRUE, &Flags))
				{
				}
			}
			while (0);

			m_SendPacket.SetLength (0);

			DBG_MSG ((_T("CTcpClient::ClientThread - WSASend complete(BytesSent=%u)\n"), BytesSent));

		}
		else
		{
			ASSERT (FALSE);
		}
	}

	Close ();

	if (pBuffer != NULL)
	{
		delete[] pBuffer;
	}

	if (WsaOverlap.hEvent != WSA_INVALID_EVENT)
	{
		::WSACloseEvent (WsaOverlap.hEvent);
	}

	DBG_MSG((_T("CTcpClient::ClientThread - END, this=%p \n"), this));
	
	return 0;
}

VOID CTcpClient::OnConnect ()
{
	DBG_MSG((_T("CTcpClient::OnConnect\n")));
}

VOID CTcpClient::OnDisconnect ()
{
	DBG_MSG((_T("CTcpClient::OnDisconnect\n")));
}

VOID CTcpClient::OnWaitTimeout (ULONG TimeoutCount)
{
	DBG_MSG((_T("CTcpClient::OnWaitTimeout\n")));
}

VOID CTcpClient::OnReceive (PVOID pBuffer, ULONG Length)
{
	DBG_MSG((_T("CTcpClient::OnReceive\n")));
}

