#include "StdAfx.h"
#include "CTcpServer.h"
#include <Mstcpip.h>
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define TCP_TIMEOUT			(30 * 1000)

CTcpServer::CTcpServer (void)
{
	m_hStopEvent		= ::CreateEvent (NULL, TRUE, FALSE, NULL);
	m_hServerThread		= NULL;
	m_ClientCount		= 0;
	m_MaxClientCount	= 0;
	m_Timeout			= TCP_TIMEOUT;
}

CTcpServer::~CTcpServer (void)
{
	Close ();

	if (m_hStopEvent != NULL)
	{
		::CloseHandle (m_hStopEvent);
		m_hStopEvent = NULL;
	}
}


ULONG CTcpServer::GetAddress (LPCTSTR pszAddress)
{
	ULONG		dwIP		= 0;
	IN_ADDR		Addr		= {};
	ADDRINFOT*	pAddInfo	= NULL;
    ADDRINFOT*	ptr			= NULL;
	ADDRINFOT	Hints		= {};

	DBG_MSG((_T("CTcpServer::GetAddress - %s\n"), pszAddress));
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



ULONG CTcpServer::Open (USHORT Port)
{
	ULONG			Result		= ERROR_SUCCESS;
	CWinThread*		pThread		= NULL;

	Close ();

	do
	{
		::ResetEvent (m_hStopEvent);

		m_ServerPort = Port;
		if (m_ServerPort == 0)
		{	break;
		}

		pThread = AfxBeginThread (ListenThread, this, 0, 0, CREATE_SUSPENDED);
		if (pThread != NULL)
		{
			pThread->ResumeThread ();

			if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hServerThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{
				Result = ::GetLastError ();
				break;
			}
		}
	}
	while (0);

	if (Result != ERROR_SUCCESS)
	{
		Close ();
	}

	return Result;
}

ULONG CTcpServer::Close ()
{
	::SetEvent (m_hStopEvent);

	if (m_hServerThread != NULL)
	{
		::WaitForSingleObject (m_hServerThread, INFINITE);
		m_hServerThread = NULL;
	}

	while (m_ClientCount > 0)
	{
		Sleep (50);
	}

	return ERROR_SUCCESS;
}


CTcpServer::CTcpServerClient* CTcpServer::OnAllocClient ()
{
	return new CTcpServerClient;
}


UINT CTcpServer::ListenThread (PVOID pParam)
{
	CTcpServer*			pThis			= reinterpret_cast<CTcpServer*>(pParam);
	SOCKET				hSocket			= INVALID_SOCKET;
	ULONG				Result			= ERROR_SUCCESS;
	BOOL				KeepAliveOn		= TRUE; 
	tcp_keepalive		KeepAlive		= {0};
	tcp_keepalive		KeepAliveRet	= {0};
	BOOL				Nodelay			= 1;
	HANDLE				hSocketEvent	= WSA_INVALID_EVENT;
	SOCKADDR_IN			Sa				= {0};
	ULONG				Timeout			= TCP_TIMEOUT;
	DWORD				dwBytes			= 0; 
    HANDLE              Handles[2]      = {0};
    SOCKADDR_IN         SockAddr        = {0};
    INT                 SockAddrLen     = 0;
    WSANETWORKEVENTS    NetworkEvents   = {0};

	do
	{
        hSocketEvent = ::WSACreateEvent ();
        if (hSocketEvent == WSA_INVALID_EVENT)
        {
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ListenThread - WSACreateEvent(hSocketEvent) error %d\n"), Result));
            break;
        }

        hSocket = ::WSASocket (AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (hSocket == INVALID_SOCKET)
        {   
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ListenThread - WSASocket error %d\n"), Result));
            break;
        }

        //// Set socket option -------------------------------------------------------------
        //if (::setsockopt (hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen) == SOCKET_ERROR)
        //{
        //    Status = ::WSAGetLastError ();
        //    DBG_MSG((_T("CTcp::Open - sockopt error %d\n"), Status));
        //    break;
        //}

		if (::setsockopt (hSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&Timeout), sizeof (Timeout)) == SOCKET_ERROR)
		{
			DBG_MSG((_T("CTcpServer::ListenThread - setsockopt SO_RCVTIMEO ERROR %d\n"), ::WSAGetLastError ()));
		}

		if (::setsockopt (hSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&Timeout), sizeof (Timeout)) == SOCKET_ERROR)
		{
			DBG_MSG((_T("CTcpServer::ListenThread - setsockopt SO_SNDTIMEO ERROR %d\n"), ::WSAGetLastError ()));
		}

		// No Delay
		if (::setsockopt (hSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&Nodelay), sizeof (Nodelay)) == SOCKET_ERROR)
		{ 
			DBG_MSG((_T("CTcpServer::ListenThread - setsockopt(TCP_NODELAY) error %d\n"), ::WSAGetLastError ()));
		} 

		// KeepAlive 
		if (::setsockopt (hSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&KeepAliveOn), sizeof (KeepAliveOn)) == SOCKET_ERROR)
		{ 
			DBG_MSG((_T("CTcpServer::ListenThread - setsockopt(SO_KEEPALIVE) error %d\n"), ::WSAGetLastError ()));
		} 

		KeepAlive.onoff				= KeepAliveOn; 
		KeepAlive.keepalivetime		= 30 * 1000 ;	// Keep Alive in 30 sec. 
		KeepAlive.keepaliveinterval	= 500;			// Resend if No-Reply 
		if (::WSAIoctl (hSocket, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof(KeepAlive), &KeepAliveRet, sizeof(KeepAliveRet), &dwBytes, NULL, NULL) == SOCKET_ERROR) 
		{ 
			DBG_MSG((_T("CTcpServer::ListenThread - WSAIoctl(SIO_KEEPALIVE_VALS) error %d\n"), ::WSAGetLastError ()));
		} 

        // Bind the socket -------------------------------------------------------------
        Sa.sin_family       = AF_INET;
        Sa.sin_addr.s_addr  = 0;
		Sa.sin_port         = ::htons (pThis->m_ServerPort);

        if (::bind (hSocket, (PSOCKADDR) &Sa, sizeof (Sa)) == SOCKET_ERROR)
        {
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ListenThread - bind error %d\n"), Result));
            break;
        }

		if (::listen (hSocket, SOMAXCONN) == SOCKET_ERROR)
		{
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ListenThread - listen error %d\n"), Result));
            break;
		}

        if (::WSAEventSelect (hSocket, hSocketEvent, FD_ALL_EVENTS) == SOCKET_ERROR)
        {
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ListenThread - WSAEventSelect error %d\n"), Result));
            break;
        }

	    Handles[0] = pThis->m_hStopEvent;
	    Handles[1] = hSocketEvent;
       
		while (1)
        {
            Result = ::WSAWaitForMultipleEvents (_countof (Handles), Handles, FALSE, INFINITE, FALSE);

			if (Result == (WAIT_OBJECT_0 + 0))
            {
				//STOP Event
				break;
			}

			ASSERT (Result == (WAIT_OBJECT_0 + 1));
			if (Result == (WAIT_OBJECT_0 + 1))
            {
                if (::WSAEnumNetworkEvents (hSocket, hSocketEvent, &NetworkEvents) == SOCKET_ERROR)
                {
                    DBG_MSG ((_T("CTcpServer::ListenThread -  WSAEnumNetworkEvents failure err=%u\n"), ::WSAGetLastError ()));
                    continue;
                }

                if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
                {   
					do
					{
						if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
						{
		                    DBG_MSG ((_T("CTcpServer::ListenThread -  FD_ACCEPT_BIT=%u\n"), NetworkEvents.iErrorCode[FD_ACCEPT_BIT]));
							break;
						}

                        ::ZeroMemory (&SockAddr, sizeof (SockAddr));
                        SockAddrLen = sizeof (SockAddr);

                        SOCKET  hClientSocket = ::WSAAccept (hSocket, reinterpret_cast<SOCKADDR*>(&SockAddr), &SockAddrLen, NULL, 0);
                        if (hClientSocket == INVALID_SOCKET)
                        {	break;
						}

						if ((pThis->m_MaxClientCount > 0) && (pThis->m_ClientCount >= pThis->m_MaxClientCount))
						{
							::closesocket (hClientSocket);
							DBG_MSG ((_T("CTcpServer::ListenThread - REJECT \n")));
							break;
						}

                        DBG_MSG ((_T("CTcpServer::ListenThread - FD_ACCEPT(ClientCount=%u) \n"), pThis->m_ClientCount));

						CTcpServerClient* pClient = pThis->OnAllocClient ();

						if (pClient == NULL)
						{	break;
						}

						pClient->m_hSocket		= hClientSocket;
						pClient->m_pServer		= pThis;

						AfxBeginThread (ClientThread, pClient);
                    }
					while (0);

                }
			}
		}
	}
	while (0);

	if (hSocket != INVALID_SOCKET)
	{
		::closesocket (hSocket);
	}

	if (hSocketEvent != WSA_INVALID_EVENT)
	{
		::WSACloseEvent (hSocketEvent);
	}

	return 0;
}



UINT CTcpServer::ClientThread (PVOID pParam)
{
	CTcpServerClient*	pClient			= reinterpret_cast<CTcpServerClient*>(pParam);
	CTcpServer*			pThis			= reinterpret_cast<CTcpServer*>(pClient->m_pServer);
	SOCKET				hSocket			= pClient->m_hSocket;
	HANDLE				hSocketEvent	= WSA_INVALID_EVENT;
	ULONG				Result			= 0;
    HANDLE              Handles[3]      = {0};
    WSANETWORKEVENTS    NetworkEvents   = {0};
    WSABUF              RecvDesc        = {0};
    WSAOVERLAPPED       WsaOverlap      = {0};
	ULONG				BytesRecv		= 0;
	ULONG				Flags			= 0;
	BOOL				bLoop			= TRUE;
	ULONG				TimeoutCount	= 0;
    struct tcp_keepalive ka_tmp1;
    DWORD				ka_tmp2			= 0;
	INT					Timeout			= pThis->m_Timeout;
	ULONG				BytesSent;
	WSABUF				SendBufs;

	DBG_MSG((_T("CTcpServer::ClientThread - START\n")));

	::InterlockedIncrement (&pThis->m_ClientCount);

	pThis->m_ClientMapLock.Lock ();
	pThis->m_ClientMap.SetAt (pClient, 0);
	pThis->m_ClientMapLock.Unlock ();

	pThis->OnConnect (pClient);

	do
	{
		RecvDesc.len  = 64 * 1024;
		RecvDesc.buf  = new CHAR[RecvDesc.len];
		if (RecvDesc.buf == NULL)
		{
			break;
		}

        WsaOverlap.hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
        if (WsaOverlap.hEvent == WSA_INVALID_EVENT)
        {   
            break;
        }


        // keepalive設定
        ka_tmp1.onoff				= 1;			//有効
        ka_tmp1.keepalivetime		= 30 * 1000;	//30秒
        ka_tmp1.keepaliveinterval	= 1000;			//1秒

        if (::WSAIoctl(hSocket, SIO_KEEPALIVE_VALS, (LPVOID)&ka_tmp1, sizeof(ka_tmp1), NULL, 0, &ka_tmp2, NULL, NULL) == SOCKET_ERROR)
        {
            Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpServer::ClientThread - keepalive failure %d\n"), Result));
            break;
        }

		if (::setsockopt (hSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&Timeout, sizeof (Timeout)) == SOCKET_ERROR)
		{
			Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpServer::ClientThread - SO_RCVTIMEO failure %d\n"), Result));
			break;
		}

		if (::setsockopt (hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&Timeout, sizeof (Timeout)) == SOCKET_ERROR)
		{
			Result = ::WSAGetLastError();
			DBG_MSG((_T("CTcpServer::ClientThread - SO_SNDTIMEO failure %d\n"), Result));
			break;
		}



        hSocketEvent = ::WSACreateEvent ();
        if (hSocketEvent == WSA_INVALID_EVENT)
        {
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ClientThread - WSACreateEvent(hSocketEvent) error %d\n"), Result));
            break;
        }

        if (::WSAEventSelect (hSocket, hSocketEvent, FD_ALL_EVENTS) == SOCKET_ERROR)
        {
            Result = ::WSAGetLastError ();
            DBG_MSG((_T("CTcpServer::ClientThread - WSAEventSelect error %d\n"), Result));
            break;
        }

	    Handles[0] = pThis->m_hStopEvent;
	    Handles[1] = hSocketEvent;
	    Handles[2] = pClient->m_hSendEvent;

		while (1)
        {
            Result = ::WSAWaitForMultipleEvents (_countof (Handles), Handles, FALSE, 1000, FALSE);

			if (Result == WSA_WAIT_TIMEOUT)
			{
				TimeoutCount++;
				pThis->OnWaitTimeout (pClient, TimeoutCount);
			}
			else if (Result == (WSA_WAIT_EVENT_0 + 0))
            {
				//STOP Event
				break;
			}

			if (Result == (WSA_WAIT_EVENT_0 + 1))
            {
                if (::WSAEnumNetworkEvents (hSocket, hSocketEvent, &NetworkEvents) == SOCKET_ERROR)
                {
                    DBG_MSG ((_T("CTcpServer::ClientThread -  WSAEnumNetworkEvents failure err=%u\n"), ::WSAGetLastError ()));
                    break;
                }

                if (NetworkEvents.lNetworkEvents & FD_CLOSE)
                {   
					DBG_MSG ((_T("CTcpServer::ClientThread -  WSAEnumNetworkEvents FD_CLOSE (%u)\n"), NetworkEvents.iErrorCode[FD_CLOSE_BIT]));
					break;
				}

				if (NetworkEvents.lNetworkEvents & FD_READ)
				{
					//受信したのでタイムアウトカウントリセット
					TimeoutCount = 0;

					DBG_MSG ((_T("CTcpServer::ClientThread -  WSAEnumNetworkEvents FD_READ (%u)\n"), NetworkEvents.iErrorCode[FD_READ_BIT]));
					do
					{
						BytesRecv = 0;

						if (::WSARecv (hSocket, &RecvDesc, 1, &BytesRecv, &Flags, &WsaOverlap, NULL) != SOCKET_ERROR)
						{   
							break;
						}

						if (::WSAGetLastError () != ERROR_IO_PENDING)
						{	break;
						}

						if (::WSAGetOverlappedResult (hSocket, &WsaOverlap, &BytesRecv, TRUE, &Flags))
						{
						}
					}
					while (0);

					pThis->OnReceive (pClient, RecvDesc.buf, BytesRecv);
				}
			}

			if (Result == (WSA_WAIT_EVENT_0 + 2))
            {
				CSingleLock		Lock (pClient->m_SendPacket.GetLock (), TRUE);
			
				BytesSent = 0;
				SendBufs.len = static_cast<ULONG>(pClient->m_SendPacket.GetLength ());
				SendBufs.buf = reinterpret_cast<CHAR*>(pClient->m_SendPacket.GetBuffer ());
				
				do
				{
					if (SendBufs.len == 0)
					{	break;
					}

					if (::WSASend (pClient->m_hSocket, &SendBufs, 1, &BytesSent, 0, &WsaOverlap, NULL) != SOCKET_ERROR)
					{   
						Result = ::WSAGetLastError ();
						break;
					}

					if (::WSAGetLastError () != ERROR_IO_PENDING)
					{
						Result = ::WSAGetLastError ();
						break;
					}

					if (::WSAGetOverlappedResult (pClient->m_hSocket, &WsaOverlap, &BytesSent, TRUE, &Flags))
					{
					}

				}
				while (0);

				pClient->m_SendPacket.SetLength (0);

				DBG_MSG ((_T("CTcpServer::ClientThread - WSASend complete(Len=%u, BytesSent=%u)\n"), SendBufs.len, BytesSent));

			}
		}
	}
	while (0);

	::closesocket (hSocket);

	if (hSocketEvent != WSA_INVALID_EVENT)
	{
		::WSACloseEvent (hSocketEvent);
	}

	if (WsaOverlap.hEvent != WSA_INVALID_EVENT)
	{
		::WSACloseEvent (WsaOverlap.hEvent);
	}

	if (RecvDesc.buf != NULL)
	{	
		delete[] RecvDesc.buf;
	}

	DBG_MSG((_T("CTcpServer::ClientThread - END\n")));

	pThis->OnDisconnect (pClient);

	pThis->m_ClientMapLock.Lock ();
	pThis->m_ClientMap.RemoveKey (pClient);
	pThis->m_ClientMapLock.Unlock ();

	delete pClient;

	return ::InterlockedDecrement (&pThis->m_ClientCount);
}

VOID CTcpServer::OnConnect (CTcpServerClient* pClient)
{
	DBG_MSG((_T("CTcpServer::OnConnect\n")));
}

VOID CTcpServer::OnDisconnect (CTcpServerClient* pClient)
{
	DBG_MSG((_T("CTcpServer::OnDisconnect\n")));
}

VOID CTcpServer::OnWaitTimeout (CTcpServerClient* pClient, ULONG TimeoutCount)
{
	DBG_MSG((_T("CTcpServer::OnWaitTimeout\n")));
}

VOID CTcpServer::OnReceive (CTcpServerClient* pClient, PVOID pBuffer, ULONG Length)
{
	DBG_MSG((_T("CTcpServer::OnReceive\n")));
}
