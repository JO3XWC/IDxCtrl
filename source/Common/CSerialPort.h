#pragma once
#include "plugin.h"
#include "CMemFileEx.h"

typedef struct _SERIAL_COMMAND
{
	ULONG	Type;
}SERIAL_COMMAND, *PSERIAL_COMMAND;

class CSerialPort
{
public:
	CSerialPort();
	virtual ~CSerialPort();

	VOID		SetCallback		(DATA_CALLBACK_FUNC pfnCallbackFunc, PVOID pCallbackParam);

virtual ULONG	Open			(ULONG ComNo);
virtual ULONG	Close			();

	ULONG		SetBaudRate		(ULONG BaudRate);

	ULONG		AddCommand		(ULONG Type, PVOID pBuffer = NULL, ULONG Length = 0);
	ULONG		Command			(ULONG Type, PVOID pBuffer = NULL, ULONG Length = 0);

protected:
virtual ULONG	OnCommand		(ULONG Type, PVOID pBuffer, ULONG Length);
virtual ULONG	OnRead			(ULONG Type, PUCHAR pBuffer = NULL, ULONG Length = 0);
virtual ULONG	OnReadChar		(ULONG Type, UCHAR Data);

	ULONG		Write			(PUCHAR pBuffer, ULONG Length);
	ULONG		WriteCmd		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2, PUCHAR pBuffer, ULONG Length, ULONG Time);
	ULONG		WriteCmdFB		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2, PUCHAR pBuffer, ULONG Length, ULONG Time);
	ULONG		WriteCmd		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, PUCHAR pBuffer, ULONG Length, ULONG Time);
	ULONG		WriteCmdFB		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, PUCHAR pBuffer, ULONG Length, ULONG Time);
	ULONG		WriteCmd		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, PUCHAR pBuffer, ULONG Length, ULONG Time);
	ULONG		WriteCmdFB		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, PUCHAR pBuffer, ULONG Length, ULONG Time);

	ULONG		WaitResp		(ULONG Time = 1000);
	VOID		SetWaitResp		(PUCHAR pBuffer, ULONG Length);
	VOID		SetWaitResp		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2);
	VOID		SetWaitResp		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd);
	VOID		SetWaitResp		(UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd);


	VOID		Callback		(PUCHAR pBuffer, ULONG Length);

protected:
	VOID		SetTrxAddr		(UCHAR Addr1, UCHAR Addr2 = 0);
	VOID		SetCtlAddr		(UCHAR Addr1, UCHAR Addr2 = 0);

private:
	static UINT	ReadThread		(PVOID pParam);
	VOID		ReadThread		();
	static UINT	WorkerThread	(PVOID pParam);
	VOID		WorkerThread	();

protected:
	ULONG		m_ComNo;
	HANDLE		m_hCom;
	HANDLE		m_hReadThread;
	HANDLE		m_hWorkerThread;
	HANDLE		m_hStopEvent;
	HANDLE		m_hWriteEvent;
	ULONGLONG	m_LastReadTickCount;
	ULONGLONG	m_RespWaitStartTime;

	ULONG		m_PrevSignalLevel[2];
	ULONG		m_PrevPowerState;

	DATA_CALLBACK_FUNC	m_pfnCallbackFunc;
	PVOID				m_pCallbackParam;

	CCriticalSection	m_CommandWriteLock;

	CCriticalSection	m_MemFileLock;
	CMemFileEx			m_MemFile;
	HANDLE				m_MemFileEvent;

	UCHAR				m_RespBuff[16];
	ULONG				m_RespLen;
	HANDLE				m_hRespEvent;
	UCHAR				m_TrxAddr[2];
	UCHAR				m_CtlAddr[2];

	HANDLE				m_hCommand;
	CMemFileExList		m_CommandList;
public:
	enum
	{
		TYPE_CHAR,
		TYPE_ERR,
	};
};

