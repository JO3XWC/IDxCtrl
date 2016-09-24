#pragma once
#include "CTcpClient.h"
#include "Plugin.h"

class CTcpClientCmd : public CTcpClient
{
public:
	CTcpClientCmd(void);
	~CTcpClientCmd(void);

	ULONG	Disconnect		();
	VOID	SetCallback		(DATA_CALLBACK_FUNC pfnCallback, PVOID pCallbackParam);

	VOID	SetAuthMd5		(UCHAR Md5[16]);
	VOID	SetAuthGuid		(GUID* pReq, GUID* pResp);
protected:
	BOOL	IsFlags			(ULONG Flags);
	VOID	SetFlags		(ULONG Flags);
	VOID	DelFlags		(ULONG Flags);

protected:
virtual VOID	OnConnect		();
virtual VOID	OnDisconnect	();
virtual VOID	OnWaitTimeout	(ULONG TimeoutCount);
virtual VOID	OnReceive		(PVOID pBuffer, ULONG Length);

virtual VOID	OnPacket		(PACKET_BASE_HEADER* pPacket);
virtual VOID	OnAuthComplete	();

private:
	ULONG			m_Flags;

	CTcpPacket		m_RecvPacket;
private:

	GUID			m_AuthResp;
	GUID			m_AuthReq;
	UCHAR			m_AuthMd5[16];

	//Callback
protected:
	VOID	Callback	(ULONG Type, PVOID pBuffer, ULONG Length);

private:
	DATA_CALLBACK_FUNC		m_pfnCallback;
	PVOID					m_pCallbackParam;

public:
	enum
	{
		FLAGS_CONNECTED				= 0x00000001,
		FLAGS_AUTH16_COMPLETE		= 0x00000002,

		FLAGS_ALL					= 0xffffffff,
	};
};
