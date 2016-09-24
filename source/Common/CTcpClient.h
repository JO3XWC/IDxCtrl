#pragma once
#include <winsock2.h>
#include "CTcpCommon.h"

class CTcpClient
{
public:
	CTcpClient(void);
	~CTcpClient(void);

public:
	static ULONG	GetAddress		(LPCTSTR pszAddress);

public:
	virtual ULONG	Open			(LPCTSTR pszAddress, USHORT Port);
	virtual ULONG	Open			(ULONG Address, USHORT Port);
	virtual ULONG	Close			();
	virtual ULONG	DeleteSocket	();

	VOID	Send			(PVOID pBuffer, ULONG Length);

protected:
	ULONG	CreateSocket	();

	ULONG	ReConnect		();
	ULONG	Connect			(ULONG Address, USHORT Port);

	UINT	ClientThread	();
static UINT ClientThread	(LPVOID pParam);

virtual VOID	OnConnect		();
virtual VOID	OnDisconnect	();
virtual VOID	OnWaitTimeout	(ULONG TimeoutCount);
virtual VOID	OnReceive		(PVOID pBuffer, ULONG Length);


private:
	SOCKET				m_hSocket;
	ULONG				m_Address;
	USHORT				m_Port;
	HANDLE				m_hSocketEvent;
	HANDLE				m_hStopEvent;
	HANDLE				m_hSocketThread;
	INT					m_Timeout;
	ULONG				m_ThreadID;

	HANDLE				m_hSendEvent;
	CTcpPacket			m_SendPacket;
};


