#include "stdafx.h"
#include "CTcpServerCmd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CTcpServerCmd::CTcpServerCmd(void)
{
	GUID App2Serv = AUTH_APP_TO_SERVER;
	GUID Serv2App = AUTH_SERVER_TO_APP;

	SetAuthGuid (&App2Serv, &Serv2App);
	::ZeroMemory (m_AuthMd5, sizeof (m_AuthMd5));
	
	m_pfnCallback		= NULL;
	m_pCallbackParam	= NULL;
}

CTcpServerCmd::~CTcpServerCmd(void)
{
}


VOID CTcpServerCmd::SetAuthGuid (GUID* pReq, GUID* pResp)
{
	memcpy (&m_AuthReq, pReq, sizeof (m_AuthReq));
	memcpy (&m_AuthResp, pResp, sizeof (m_AuthResp));
}

VOID CTcpServerCmd::SetAuthMd5 (UCHAR Md5[16])
{
	memcpy (m_AuthMd5, Md5, sizeof (m_AuthMd5));
}

VOID CTcpServerCmd::SetCallback (DATA_CALLBACK_FUNC pfnCallback, PVOID pCallbackParam)
{
	m_pfnCallback		= pfnCallback;
	m_pCallbackParam	= pCallbackParam;
}

ULONG CTcpServerCmd::Callback (ULONG Type, PVOID pBuffer, ULONG Length)
{
	ULONG	Result = ERROR_SUCCESS;

	if (m_pfnCallback != NULL)
	{	
		Result = m_pfnCallback (Type, pBuffer, Length, m_pCallbackParam);
	}

	return Result;
}

//VOID CTcpServerCmd::Callback (CTcpServerClient* pClient, PVOID pBuffer, ULONG Length)
//{
//	CTcpServerCmdClient* pCmdClient = reinterpret_cast<CTcpServerCmdClient*>(pClient);
//
//	if (m_pfnCallback != NULL)
//	{	
//		m_pfnCallback (pBuffer, Length, m_pCallbackParam);
//	}
//}




ULONG CTcpServerCmd::Open (USHORT Port)
{
	return CTcpServer::Open (Port);
}

ULONG CTcpServerCmd::Close ()
{
	DBG_MSG((_T("CTcpServerCmd::Stop\n")));

	CTcpServer::Close ();

	return ERROR_SUCCESS;
}


CTcpServer::CTcpServerClient* CTcpServerCmd::OnAllocClient ()
{
	CTcpServerCmdClient* pClient = new CTcpServerCmdClient;

	return pClient;
}

VOID CTcpServerCmd::OnConnect (CTcpServerClient* pClient)
{
	DBG_MSG((_T("CTcpServerCmd::OnConnect\n")));
}

VOID CTcpServerCmd::OnDisconnect (CTcpServerClient* pClient)
{
	DBG_MSG((_T("CTcpServerCmd::OnDisconnect\n")));
}

VOID CTcpServerCmd::OnWaitTimeout (CTcpServerClient* pClient, ULONG TimeoutCount)
{
	CTcpServerCmdClient* pCmdClient = reinterpret_cast<CTcpServerCmdClient*>(pClient);

	//DBG_MSG((_T("CTcpServerCmd::OnWaitTimeout\n")));

	if (!pCmdClient->IsFlags (CTcpServerCmdClient::FLAGS_AUTH16_COMPLETE) && (TimeoutCount > 5))
	{	pCmdClient->Close ();
	}
}

VOID CTcpServerCmd::OnAuthCompleted (CTcpServerClient* pClient)
{
}

VOID CTcpServerCmd::OnReceive (CTcpServerClient* pClient, PVOID pBuffer, ULONG Length)
{
	CTcpServerCmdClient* pCmdClient = reinterpret_cast<CTcpServerCmdClient*>(pClient);

	DBG_MSG((_T("CTcpServerCmd::OnReceive\n")));

	CSingleLock	Lock (pCmdClient->m_RecvPacket.GetLock (), TRUE);

	pCmdClient->m_RecvPacket.Write (pBuffer, Length);

	if (pCmdClient->IsFlags (CTcpServerCmdClient::FLAGS_AUTH16_COMPLETE))
	{
		PACKET_BASE_HEADER* pPacket = pCmdClient->m_RecvPacket.GetPacket ();

		while (pPacket != NULL)
		{
			OnPacket (pCmdClient, pPacket);

			pPacket = pCmdClient->m_RecvPacket.RemovePacket ();
		}
	}
	else
	{
		if (pCmdClient->m_RecvPacket.GetLength () == (sizeof (m_AuthReq) + sizeof (m_AuthMd5)))
		{
			if ((memcmp (pCmdClient->m_RecvPacket.GetBuffer (), &m_AuthReq, sizeof (m_AuthReq)) == 0) &&
				(memcmp (pCmdClient->m_RecvPacket.GetBuffer () + sizeof (m_AuthReq), m_AuthMd5, sizeof (m_AuthMd5)) == 0))
			{
				DBG_MSG((_T("CTcpServerCmd::OnReceive - AUTH COMPLETE\n")));
				pCmdClient->SetFlags (CTcpServerCmdClient::FLAGS_AUTH16_COMPLETE);
				pCmdClient->m_RecvPacket.SetLength (0);
				pCmdClient->Send (&m_AuthResp, sizeof (m_AuthResp));
				pCmdClient->Send (m_AuthMd5, sizeof (m_AuthMd5));

				OnAuthCompleted (pCmdClient);
			}
			else
			{
				DBG_MSG((_T("CTcpServerCmd::OnReceive - AUTH failure\n")));
				pCmdClient->Close ();
			}
		}
	}
}

VOID CTcpServerCmd::OnPacket (CTcpServerCmdClient* pClient, PACKET_BASE_HEADER* pPacket)
{
	PACKET_CMD_HEADER* pCmdHeader = reinterpret_cast<PACKET_CMD_HEADER*>(pPacket);

	switch (pCmdHeader->m_Type)
	{
	case TYPE_CACHE_GET:
		{
			CTcpPacket	Packet;
			ULONG		Length = 64 * 1024;

			Packet.SetLength (Length);
			Length = Callback (TYPE_CACHE_GET, Packet.GetBuffer (), Length);
			DBG_MSG((_T("CTcpServerCmd::OnPacket - TYPE_CACHE_GET,  Length=%u \n"), Length));

			ASSERT (Length < (64 * 1024));
			if (Length == 0)
			{	break;
			}

			pClient->Send (Packet.GetBuffer (), Length);
		}
		break;

	default:
		{
			Callback (pCmdHeader->m_Type, pCmdHeader->GetData (), pCmdHeader->GetDatatLength ());
		}
		break;
	}
}

VOID CTcpServerCmd::SendClients (PVOID pBuffer, ULONG Length)
{
	POSITION			Pos;
	CTcpServerClient*	pClient;
	ULONG				Value;
	CSingleLock			Lock (&m_ClientMapLock, TRUE);

	Pos = m_ClientMap.GetStartPosition ();
	while (Pos != NULL)
	{
		m_ClientMap.GetNextAssoc (Pos, pClient, Value);
		
		pClient->Send (pBuffer, Length);
	}
}
