#include "StdAfx.h"
#include "CTcpClientCmd.h"
//#include "ENoDebug.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CTcpClientCmd::CTcpClientCmd(void)
{
	m_Flags				= 0;
	GUID App2Serv = AUTH_APP_TO_SERVER;
	GUID Serv2App = AUTH_SERVER_TO_APP;

	SetAuthGuid (&App2Serv, &Serv2App);

	::ZeroMemory (m_AuthMd5, sizeof (m_AuthMd5));
}

CTcpClientCmd::~CTcpClientCmd(void)
{
	Close ();
}

BOOL CTcpClientCmd::IsFlags (ULONG Flags)
{
	return (m_Flags & Flags) != 0;
}

VOID CTcpClientCmd::SetFlags (ULONG Flags)
{
	m_Flags |= Flags;
}

VOID CTcpClientCmd::DelFlags (ULONG Flags)
{
	m_Flags &= ~Flags;
}

VOID CTcpClientCmd::SetAuthGuid (GUID* pReq, GUID* pResp)
{
	memcpy (&m_AuthReq, pReq, sizeof (m_AuthReq));
	memcpy (&m_AuthResp, pResp, sizeof (m_AuthResp));
}

VOID CTcpClientCmd::SetAuthMd5 (UCHAR Md5[16])
{
	memcpy (m_AuthMd5, Md5, sizeof (m_AuthMd5));
}

VOID CTcpClientCmd::SetCallback (DATA_CALLBACK_FUNC pfnCallback, PVOID pCallbackParam)
{
	m_pfnCallback		= pfnCallback;
	m_pCallbackParam	= pCallbackParam;
}

VOID CTcpClientCmd::Callback (ULONG Type, PVOID pBuffer, ULONG Length)
{
	if (m_pfnCallback != NULL)
	{
		m_pfnCallback (Type, pBuffer, Length, m_pCallbackParam);
	}
}



ULONG CTcpClientCmd::Disconnect ()
{
	DeleteSocket ();
	m_RecvPacket.SetLength (0);

	return ERROR_SUCCESS;
}


VOID CTcpClientCmd::OnConnect ()
{
	DBG_MSG((_T("CTcpClientCmd::OnConnect\n")));
	SetFlags (FLAGS_CONNECTED);
	Send (&m_AuthReq, sizeof (m_AuthReq));
	Send (&m_AuthMd5, sizeof (m_AuthMd5));
}

VOID CTcpClientCmd::OnDisconnect ()
{
	DBG_MSG((_T("CTcpClientCmd::OnDisconnect\n")));
	DelFlags (FLAGS_ALL);

	Callback (TYPE_CACHE_CLEAR, NULL, 0);
}

VOID CTcpClientCmd::OnWaitTimeout (ULONG TimeoutCount)
{
	//DBG_MSG((_T("CTcpClientCmd::OnWaitTimeout\n")));
}

VOID CTcpClientCmd::OnReceive (PVOID pBuffer, ULONG Length)
{
	DBG_MSG((_T("CTcpClientCmd::OnReceive\n")));
	CSingleLock	Lock (m_RecvPacket.GetLock (), TRUE);

	m_RecvPacket.Write (pBuffer, Length);
	
	if (IsFlags (CTcpClientCmd::FLAGS_AUTH16_COMPLETE))
	{
		PACKET_BASE_HEADER* pPacket = m_RecvPacket.GetPacket ();

		while (pPacket != NULL)
		{
			OnPacket (pPacket);

			pPacket = m_RecvPacket.RemovePacket ();
		}
	}
	else
	{
		if (m_RecvPacket.GetLength () == (sizeof (m_AuthResp) + sizeof (m_AuthMd5)))
		{
			if ((memcmp (m_RecvPacket.GetBuffer (), &m_AuthResp, sizeof (m_AuthResp)) == 0) &&
				(memcmp (m_RecvPacket.GetBuffer () + sizeof (m_AuthResp), m_AuthMd5, sizeof (m_AuthMd5)) == 0))
			{
				DBG_MSG((_T("CTcpClientCmd::OnReceive - AUTH COMPLETE\n")));
				m_RecvPacket.SetLength (0);
				SetFlags (CTcpClientCmd::FLAGS_AUTH16_COMPLETE);

				OnAuthComplete ();
			}
			else
			{
				DBG_MSG((_T("CTcpClientCmd::OnReceive - AUTH failure\n")));
				Disconnect ();
			}
		}
	}
}

VOID CTcpClientCmd::OnPacket (PACKET_BASE_HEADER* pPacket)
{
	PACKET_CMD_HEADER* pCmdHeader = reinterpret_cast<PACKET_CMD_HEADER*>(pPacket);

	Callback (pCmdHeader->m_Type, pCmdHeader->GetData (), pCmdHeader->GetDatatLength ());
}

VOID CTcpClientCmd::OnAuthComplete ()
{
	PACKET_CMD_HEADER Header;

	//認証が完了したので現在のコマンド一覧を取得する
	Header.m_Length = sizeof (Header);
	Header.m_Type = TYPE_CACHE_GET;

	Send (&Header, sizeof (Header));
}
