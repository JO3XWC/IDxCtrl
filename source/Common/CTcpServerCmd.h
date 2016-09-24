#pragma once
#include "CTcpServer.h"
#include "plugin.h"


class CTcpServerCmd : public CTcpServer
{
public:
	class CTcpServerCmdClient : public CTcpServerClient
	{
	public:
		CTcpServerCmdClient ()
		{
			m_Flags = 0;
		}

		virtual ~CTcpServerCmdClient ()
		{
		}

	public:
		BOOL IsFlags (ULONG Flags)
		{
			return (m_Flags & Flags) != 0;
		}

		VOID SetFlags (ULONG Flags)
		{
			m_Flags |= Flags;
		}

		VOID DelFlags (ULONG Flags)
		{
			m_Flags &= ~Flags;
		}

	public:

		CTcpPacket			m_RecvPacket;
	private:
		ULONG				m_Flags;

	public:
		enum
		{
			FLAGS_AUTH16_COMPLETE		= 0x00000002,

			FLAGS_ALL					= 0xffffffff,
		};
	};






public:
	CTcpServerCmd(void);
	virtual ~CTcpServerCmd(void);

public:
	virtual ULONG		Open			(USHORT Port);
	virtual ULONG		Close			();

	VOID	SetAuthGuid (GUID* pReq, GUID* pResp);
	VOID	SetAuthMd5 (UCHAR Md5[16]);
	VOID	SetCallback (DATA_CALLBACK_FUNC pfnCallback, PVOID pCallbackParam);

	VOID	SendClients	(PVOID pBuffer, ULONG Length);
protected:
	virtual CTcpServerClient*	OnAllocClient ();
	virtual VOID	OnConnect		(CTcpServerClient* pClient);
	virtual VOID	OnDisconnect	(CTcpServerClient* pClient);
	virtual VOID	OnWaitTimeout	(CTcpServerClient* pClient, ULONG TimeoutCount);
	virtual VOID	OnReceive		(CTcpServerClient* pClient, PVOID pBuffer, ULONG Length);

	virtual VOID	OnPacket		(CTcpServerCmdClient* pClient, PACKET_BASE_HEADER* pPacket);
	virtual VOID	OnAuthCompleted	(CTcpServerClient* pClient);

protected:
	//ULONG	Callback	(CTcpServerClient* pClient, PVOID pBuffer, ULONG Length);
	ULONG	Callback	(ULONG Type, PVOID pBuffer, ULONG Length);

private:
	DATA_CALLBACK_FUNC	m_pfnCallback;
	PVOID				m_pCallbackParam;

private:
	GUID			m_AuthResp;
	GUID			m_AuthReq;
	UCHAR			m_AuthMd5[16];
};
