#include "stdafx.h"
#include "CSerialPort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TIMEOUT_RESYNC	(10 * 1000)
#define TIMEOUT_SYGNAL	(3 * 1000)

CSerialPort::CSerialPort()
{
	m_hCom				= INVALID_HANDLE_VALUE;
	m_hReadThread		= NULL;
	m_hWorkerThread		= NULL;
	m_hStopEvent		= ::CreateEvent (NULL, TRUE, FALSE, NULL);
	m_hWriteEvent		= ::CreateEvent (NULL, FALSE, FALSE, NULL);
	m_hCommand			= ::CreateEvent (NULL, FALSE, FALSE, NULL);
	
	m_pfnCallbackFunc	= NULL;
	m_pCallbackParam	= NULL;
	m_LastReadTickCount	= 0;

	m_RespLen			= 0;
	m_hRespEvent		= ::CreateEvent (NULL, FALSE, FALSE, NULL);

	ZeroMemory (m_TrxAddr, sizeof (m_TrxAddr));
	ZeroMemory (m_CtlAddr, sizeof (m_CtlAddr));
}


CSerialPort::~CSerialPort()
{
	ASSERT (m_hCom == INVALID_HANDLE_VALUE);
	Close ();

	if (m_hStopEvent != NULL)
	{
		::CloseHandle (m_hStopEvent);
		m_hStopEvent = NULL;
	}

	if (m_hWriteEvent != NULL)
	{
		::CloseHandle (m_hWriteEvent);
		m_hWriteEvent = NULL;
	}

	if (m_hCommand != NULL)
	{
		::CloseHandle (m_hCommand);
		m_hCommand = NULL;
	}

	if (m_hRespEvent != NULL)
	{
		::CloseHandle (m_hRespEvent);
		m_hRespEvent = NULL;
	}
}

VOID CSerialPort::SetCallback (DATA_CALLBACK_FUNC pfnCallbackFunc, PVOID pCallbackParam)
{
	m_pfnCallbackFunc	= pfnCallbackFunc;
	m_pCallbackParam	= pCallbackParam;
}

VOID CSerialPort::Callback (PUCHAR pBuffer, ULONG Length)
{
	if (m_pfnCallbackFunc != NULL)
	{
		m_pfnCallbackFunc (TYPE_SERIAL, pBuffer, Length, m_pCallbackParam);
	}
}

VOID CSerialPort::SetTrxAddr (UCHAR Addr1, UCHAR Addr2)
{
	m_TrxAddr[0] = Addr1;
	m_TrxAddr[1] = Addr2;
}

VOID CSerialPort::SetCtlAddr (UCHAR Addr1, UCHAR Addr2)
{
	m_CtlAddr[0] = Addr1;
	m_CtlAddr[1] = Addr2;
}



ULONG CSerialPort::Open (ULONG ComNo)
{
	ULONG			Result		= ERROR_SUCCESS;
	DCB				Dcb			= {sizeof (Dcb)};
	COMMTIMEOUTS	ComTimeouts	= {};
	CString			strCom;
	CWinThread*		pThread;

	DBG_MSG((_T("CSerialPort::Open - COM%u \n"), ComNo));

	do
	{
		if (m_hCom != INVALID_HANDLE_VALUE)
		{
			Close ();
			Sleep (100);
		}

		ASSERT ((ComNo >= 1) && (ComNo <= 256));
		if ((ComNo < 1) || (ComNo > 256))
		{
			Result = ERROR_INVALID_PARAMETER;
			break;
		}

		DBG_MSG((_T("CSerialPort::Open - @@@@@ 38400 @@@@@ \n")));
		m_ComNo = ComNo;

		strCom.Format (_T("\\\\.\\COM%u"), ComNo);
		
		m_hCom = ::CreateFile (strCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (m_hCom == INVALID_HANDLE_VALUE)
		{	
			Result = ::GetLastError ();
			DBG_MSG((_T("CSerialPort::Open - CreateFile failure %d\n"), Result));
			break;
		}

		if (!::SetupComm (m_hCom, 1024, 1024))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - SetupComm failure %d\n"), Result));
			break;
		}

		if (!::PurgeComm (m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - PurgeComm failure %d\n"), Result));
			break;
		}

		if (!::GetCommState (m_hCom, &Dcb))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - GetCommState failure %d\n"), Result));
			break;
		}

		Dcb.DCBlength				= sizeof (Dcb);			/* sizeof(DCB)                     */
		Dcb.BaudRate				= CBR_19200;			/* Baudrate at which running       */
		Dcb.fBinary					= TRUE;					/* Binary Mode (skip EOF check)    */
		//Dcb.fParity: 1;									/* Enable parity checking          */
		//Dcb.fOutxCtsFlow:1;								/* CTS handshaking on output       */
		//Dcb.fOutxDsrFlow:1;								/* DSR handshaking on output       */
		Dcb.fDtrControl				= DTR_CONTROL_ENABLE;	/* DTR Flow control                */
		//Dcb.fDsrSensitivity:1;							/* DSR Sensitivity              */
		//Dcb.fTXContinueOnXoff: 1;							/* Continue TX when Xoff sent */
		//Dcb.fOutX: 1;										/* Enable output X-ON/X-OFF        */
		//Dcb.fInX: 1;										/* Enable input X-ON/X-OFF         */
		Dcb.fErrorChar				= 0;					/* Enable Err Replacement          */
		//Dcb.fNull: 1;										/* Enable Null stripping           */
		Dcb.fRtsControl				= RTS_CONTROL_ENABLE;	/* Rts Flow control                */
		//Dcb.fAbortOnError:1;								/* Abort all reads and writes on Error */
		//Dcb.fDummy2:17;									/* Reserved                        */
		//Dcb.wReserved;									/* Not currently used              */
		//Dcb.XonLim;										/* Transmit X-ON threshold         */
		//Dcb.XoffLim;										/* Transmit X-OFF threshold        */
		Dcb.ByteSize				= 8;					/* Number of bits/byte, 4-8        */
		Dcb.Parity					= NOPARITY;				/* 0-4=None,Odd,Even,Mark,Space    */
		Dcb.StopBits				= ONESTOPBIT;			/* 0,1,2 = 1, 1.5, 2               */
		//Dcb.XonChar;										/* Tx and Rx X-ON character        */
		//Dcb.XoffChar;										/* Tx and Rx X-OFF character       */
		Dcb.ErrorChar				= 0x00;					/* Error replacement char          */
		Dcb.EofChar					= 0x00;					/* End of Input character          */
		Dcb.EvtChar					= (char)0xFD;			/* Received Event character        */
		//Dcb.wReserved1;									/* Fill for now.                   */

		if (!::SetCommState (m_hCom, &Dcb))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - SetCommState failure %d\n"), Result));
			break;
		}

		if (!::EscapeCommFunction (m_hCom, SETDTR))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - EscapeCommFunction(SETDTR) failure %d\n"), Result));
			break;
		}

		if (!::EscapeCommFunction (m_hCom, SETRTS))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::Open - EscapeCommFunction(SETRTS) failure %d\n"), Result));
			break;
		}

		ComTimeouts.ReadIntervalTimeout			= 20;
		ComTimeouts.ReadTotalTimeoutMultiplier	= 10;
		ComTimeouts.ReadTotalTimeoutConstant	= 100;
		ComTimeouts.WriteTotalTimeoutMultiplier	= 10;
		ComTimeouts.WriteTotalTimeoutConstant	= 100;
		
		if (!::SetCommTimeouts (m_hCom, &ComTimeouts))
		{
			Result = ::GetLastError ();
			break;
		}

		//Read Thread
		pThread = AfxBeginThread (ReadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hReadThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			Result = ::GetLastError ();
		}

		pThread->ResumeThread ();

		//Worker Thread
		pThread = AfxBeginThread (WorkerThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	
		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hWorkerThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			Result = ::GetLastError ();
		}

		pThread->ResumeThread ();

		if (Result != ERROR_SUCCESS)
		{	break;
		}

	}
	while (0);

	if (Result != ERROR_SUCCESS)
	{
		Close ();
	}
	DBG_MSG((_T("CSerialPort::Open - Result=%08X\n"), Result));

	return Result;
}

ULONG CSerialPort::Close ()
{
	ULONG			Result		= ERROR_SUCCESS;

	do
	{
		if (m_hCom == INVALID_HANDLE_VALUE)
		{	break;
		}

		if (!::EscapeCommFunction (m_hCom, CLRDTR))
		{
			Result = ::GetLastError ();
			//break;
		}

		if (!::EscapeCommFunction (m_hCom, CLRRTS))
		{
			Result = ::GetLastError ();
			//break;
		}
		
		::CancelIo (m_hCom);
		::FlushFileBuffers(m_hCom);
		::CloseHandle (m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
	while (0);

	::SetEvent (m_hStopEvent);
	
	if (m_hWorkerThread != NULL)
	{
		::WaitForSingleObject (m_hWorkerThread, INFINITE);
		::CloseHandle (m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
	
	if (m_hReadThread != NULL)
	{
		::WaitForSingleObject (m_hReadThread, INFINITE);
		::CloseHandle (m_hReadThread);
		m_hReadThread = NULL;
	}

	::ResetEvent (m_hStopEvent);
	m_MemFile.SetLength (0);
	
	m_CommandList.RemoveAll ();

	Sleep (100);

	return Result;
}

ULONG CSerialPort::SetBaudRate (ULONG BaudRate)
{
	DCB		Dcb			= {sizeof (Dcb)};
	ULONG	Result		= ERROR_SUCCESS;
	
	DBG_MSG((_T("ESerialPort::SetBaudRate - BaudRate=%d\n"), BaudRate));

	do
	{
		if (!::GetCommState (m_hCom, &Dcb))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::SetBaudRate - GetCommState failure %d\n"), Result));
			break;
		}
		
		Dcb.BaudRate = BaudRate;
			
		if (!::SetCommState (m_hCom, &Dcb))
		{
			Result = ::GetLastError ();
			DBG_MSG((_T("ESerialPort::SetBaudRate - SetCommState failure %d\n"), Result));
			break;
		}
	}
	while (0);

	return Result;
}







UINT CSerialPort::WorkerThread (PVOID pParam)
{
	reinterpret_cast<CSerialPort*>(pParam)->WorkerThread ();
	return 0;
}

VOID CSerialPort::WorkerThread ()
{
	HANDLE				Handles[2]		= {};
	ULONG				Result			= 0;
	CString				strMsg;
	ULONGLONG			TickCount;
	CList<CMemFileEx*>	CommandList;
	CMemFileEx*			pMemFile;
	SERIAL_COMMAND*		pCmdHdr;

	DBG_MSG((_T("CSerialPort::WorkerThread - START\n")));

	Handles[0] = m_hStopEvent;
	Handles[1] = m_hCommand;

	m_PrevPowerState = -1;
	for (int i=0;i<_countof (m_PrevSignalLevel);i++)
	{
		m_PrevSignalLevel[i] = -1;
	}

	Callback (NULL, 0);
	
	if (Command (CIV_ALL_STATUS) == ERROR_SUCCESS)
	{
	}

	while (::WaitForSingleObject (m_hStopEvent, 0) == WAIT_TIMEOUT)
	{
		Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, 3000);
			
		if (Result == WAIT_TIMEOUT)
		{
			TickCount = ::GetTickCount64 ();
			if ((TickCount - m_LastReadTickCount) > TIMEOUT_RESYNC)
			{
				for (int i=0;i<_countof (m_PrevSignalLevel);i++)
				{
					m_PrevSignalLevel[i] = -1;
				}
				
				Callback (NULL, 0);

				AddCommand (CIV_ALL_STATUS);
			}
			else
			{
				AddCommand (CIV_SIGNAL_LEVEL1);
				AddCommand (CIV_SIGNAL_LEVEL2);
			}
		}
		else if (Result == (WAIT_OBJECT_0 + 0))
		{
			//STOP Event
			break;
		}
		else if (Result == (WAIT_OBJECT_0 + 1))
		{
			//Command Event
			m_CommandList.GetUsedMemFiles (&CommandList);

			while (!CommandList.IsEmpty ())
			{
				pMemFile = CommandList.RemoveHead ();
				pCmdHdr = reinterpret_cast<SERIAL_COMMAND*>(pMemFile->GetBuffer ());

				Command (pCmdHdr->Type, pCmdHdr + 1, pMemFile->GetLength () - sizeof (SERIAL_COMMAND));

				m_CommandList.FreeMemFile (pMemFile);
			}
		}
		else
		{
			ASSERT (FALSE);
		}
		
		//DBG_MSG((_T("CSerialPort::WorkerThread - WaitResult=%08X, Event=%08X\n"), Result, Event));

	}
	
	DBG_MSG((_T("CSerialPort::WorkerThread - END\n")));
}

UINT CSerialPort::ReadThread (PVOID pParam)
{
	reinterpret_cast<CSerialPort*>(pParam)->ReadThread ();
	return 0;
}

VOID CSerialPort::ReadThread ()
{
	OVERLAPPED		Commvlap		= {};
	OVERLAPPED		ReadOvlap		= {};
	HANDLE			Handles[2]		= {};
	ULONG			tmp				= 0;
	ULONG			Result			= 0;
	ULONG			Event			= 0;
	ULONG			Errors			= 0;
	ULONG			BytesRead		= 0;
	ULONG			BytesWritten	= 0;
	COMSTAT			ComStat			= {};
	UCHAR			pRecvBuf[256];
	CString			strMsg;
	// Create an event object for use by WaitCommEvent. 
	
	DBG_MSG((_T("CSerialPort::ReadThread - START\n")));

	Commvlap.hEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);
	ReadOvlap.hEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);

	Handles[0] = m_hStopEvent;
	Handles[1] = Commvlap.hEvent;

	::SetCommMask (m_hCom, EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY | EV_CTS | EV_DSR | EV_RLSD | EV_BREAK | EV_ERR | EV_RING);

	while (::WaitForSingleObject (m_hStopEvent, 0) == WAIT_TIMEOUT)
	{
		if (!::WaitCommEvent (m_hCom, &Event, &Commvlap))
		{
			Result = ::GetLastError ();
			if (Result != ERROR_IO_PENDING)
			{	
				DBG_MSG((_T("CSerialPort::ReadThread - WaitCommEvent failure Result = %d\n"), Result));
				break;
			}

			Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, INFINITE);
			
			if (Result == (WAIT_OBJECT_0 + 0))
			{
				//STOP Event
				break;
			}
			else if (Result == (WAIT_OBJECT_0 + 1))
			{
				//Comm Event
				if (!::GetOverlappedResult (m_hCom, &Commvlap, &tmp, FALSE))
				{
					Result = ::GetLastError ();
					DBG_MSG((_T("CSerialPort::ReadThread - GetOverlappedResult(WaitCommEvent) failure Result = %d\n"), Result));
					continue;
				}
			}
			else
			{
				ASSERT (FALSE);
			}
		}
		
		//DBG_MSG((_T("CSerialPort::ReadThread - WaitResult=%08X, Event=%08X\n"), Result, Event));

		if (Event & EV_RXCHAR)
		{
			//DBG_MSG((_T("CSerialPort::ReadThread - EV_RXCHAR \n")));
			::ZeroMemory (&Errors, sizeof (Errors));
			::ZeroMemory (&ComStat, sizeof (ComStat));
			::ZeroMemory (&pRecvBuf, sizeof (pRecvBuf));
			BytesRead = 0;

			do
			{
				if (!::ClearCommError (m_hCom, &Errors, &ComStat))
				{
					Result = ::GetLastError ();
					DBG_MSG((_T("CSerialPort::ReadThread - ClearCommError failure Result = %d\n"), Result));
					break;
				}

				if(ComStat.cbInQue == 0)
				{	break;
				}

				//受信データサイズ分データをRecBufに読み出す
				BytesRead = 0;
				if (!::ReadFile (m_hCom, pRecvBuf, min (ComStat.cbInQue, sizeof (pRecvBuf)), &BytesRead, &ReadOvlap))
				{
					Result = ::GetLastError ();
					if (Result != ERROR_IO_PENDING)
					{
						DBG_MSG((_T("CSerialPort::ReadThread - ReadFile failure Result = %d\n"), Result));
						break;
					}

					Handles[0] = ReadOvlap.hEvent;
					Handles[1] = m_hStopEvent;
					Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, INFINITE);
					if (Result != WAIT_OBJECT_0)
					{
						DBG_MSG((_T("CSerialPort::ReadThread - WaitForSingleObject(ReadFile) failure Result = %d\n"), Result));
						continue;
					}

					if (::GetOverlappedResult (m_hCom, &ReadOvlap, &BytesRead, FALSE))
					{
						Result = ERROR_SUCCESS;
					}
					else
					{
						Result = ::GetLastError ();
						DBG_MSG((_T("CSerialPort::ReadThread - GetOverlappedResult(ReadFile) failure Result = %d\n"), Result));
					}
				}

				if (BytesRead == 0)
				{	break;
				}

				m_LastReadTickCount = ::GetTickCount64 ();

				for (ULONG i=0;i<BytesRead;i++)
				{
					OnReadChar (TYPE_CHAR, pRecvBuf[i]);
				}
			}
			while (0);


		}

		if (Event & EV_RXFLAG)
		{
			//DBG_MSG((_T("CSerialPort::ReadThread - EV_RXFLAG \n")));
		}

		if (Event & EV_TXEMPTY)
		{
			//DBG_MSG((_T("CSerialPort::ReadThread - EV_TXEMPTY \n")));
			//::SetEvent (m_hTxEmpty);
		}

		if (Event & EV_CTS)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_CTS \n")));
		}

		if (Event & EV_DSR)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_DSR \n")));
		}

		if (Event & EV_RLSD)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_RLSD \n")));
			::ZeroMemory (&ComStat, sizeof (ComStat));
			::ZeroMemory (&Errors, sizeof (Errors));
			::ClearCommError (m_hCom, &Errors, &ComStat);

		}

		if (Event & EV_BREAK)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_BREAK \n")));
			::ZeroMemory (&ComStat, sizeof (ComStat));
			::ZeroMemory (&Errors, sizeof (Errors));
			::ClearCommError (m_hCom, &Errors, &ComStat);
		}

		if (Event & EV_ERR)
		{
			::ZeroMemory (&ComStat, sizeof (ComStat));
			::ZeroMemory (&Errors, sizeof (Errors));
			::ClearCommError (m_hCom, &Errors, &ComStat);
			
			OnReadChar (TYPE_ERR, static_cast<UCHAR>(Errors));

			::PurgeComm (m_hCom, PURGE_RXCLEAR);
#ifdef _DEBUG
			CString strErr;
			if (Errors & CE_RXOVER)		{	strErr += _T(" RXOVER");	}
			if (Errors & CE_OVERRUN)	{	strErr += _T(" OVERRUN");	}
			if (Errors & CE_RXPARITY)	{	strErr += _T(" RXPARITY");	}
			if (Errors & CE_FRAME)		{	strErr += _T(" FRAME");		}
			if (Errors & CE_BREAK)		{	strErr += _T(" BREAK");		}
			if (Errors & CE_TXFULL)		{	strErr += _T(" TXFULL");	}
			DBG_MSG((_T("CSerialPort::ReadThread - EV_ERR (%08X) %s\n"), Errors, strErr.GetString ()));
#endif
		}

		if (Event & EV_RING)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_RING \n")));
		}

		if (Event & EV_PERR)
		{
			DBG_MSG((_T("CSerialPort::ReadThread - EV_PERR \n")));
		}

		if (Event & EV_RX80FULL)
		{
			DBG_MSG((_T("CSerialPort::WorkerThread - EV_RX80FULL \n")));
		}
	}

	if (Commvlap.hEvent != NULL)
	{
		::CloseHandle (Commvlap.hEvent);
	}

	if (ReadOvlap.hEvent != NULL)
	{
		::CloseHandle (ReadOvlap.hEvent);
	}

	DBG_MSG((_T("CSerialPort::ReadThread - END\n")));
}


ULONG CSerialPort::Write (PUCHAR pBuffer, ULONG Length)
{
	//DBG_MSG((_T("CSerialPort::Write - START\n")));
	OVERLAPPED		WriteOvlap		= {};
	ULONG			BytesWritten	= 0;
	ULONG			Result			= ERROR_SUCCESS;
	
	DBG_MSG((_T("CSerialPort::Write\n")));
	DBG_BUF (pBuffer, Length);

	WriteOvlap.hEvent = m_hWriteEvent;
	
	do
	{
		if (::WriteFile (m_hCom, pBuffer, Length, &BytesWritten, &WriteOvlap))
		{	break;
		}

		if (GetLastError () != ERROR_IO_PENDING)
		{	break;
		}

		if (::GetOverlappedResult (m_hCom, &WriteOvlap, &BytesWritten, TRUE))
		{
		}
	}
	while (0);

	return Result;
}

ULONG CSerialPort::WriteCmd (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X, SubCmd=%02X, SubCmd2=%02X\n"), TrIndex, CtrlIndex, Cmd, SubCmd, SubCmd2));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	p[6] = SubCmd2;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[7], pBuffer, Length);
	}
	
	p[Length + 7] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 7;
		memcpy (m_RespBuff, p, m_RespLen);
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
	}

	Write (p, Length + 8);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}

ULONG CSerialPort::WriteCmdFB (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X, SubCmd=%02X, SubCmd2=%02X\n"), TrIndex, CtrlIndex, Cmd, SubCmd, SubCmd2));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	p[6] = SubCmd2;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[7], pBuffer, Length);
	}
	
	p[Length + 7] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 5;
		m_RespBuff[0] = p[0];
		m_RespBuff[1] = p[1];
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
		m_RespBuff[4] = 0xFB;
	}

	Write (p, Length + 8);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}

ULONG CSerialPort::WriteCmd (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X, SubCmd=%02X\n"), TrIndex, CtrlIndex, Cmd, SubCmd));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[6], pBuffer, Length);
	}
	
	p[Length + 6] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 6;
		memcpy (m_RespBuff, p, m_RespLen);
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
	}

	Write (p, Length + 7);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}

ULONG CSerialPort::WriteCmdFB (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X, SubCmd=%02X\n"), TrIndex, CtrlIndex, Cmd, SubCmd));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[6], pBuffer, Length);
	}
	
	p[Length + 6] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 5;
		m_RespBuff[0] = p[0];
		m_RespBuff[1] = p[1];
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
		m_RespBuff[4] = 0xFB;
	}

	Write (p, Length + 7);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}

ULONG CSerialPort::WriteCmd (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X\n"), TrIndex, CtrlIndex, Cmd));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[5], pBuffer, Length);
	}
	
	p[Length + 5] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 5;
		memcpy (m_RespBuff, p, m_RespLen);
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
	}

	Write (p, Length + 6);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}
ULONG CSerialPort::WriteCmdFB (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, PUCHAR pBuffer, ULONG Length, ULONG Time)
{
	DBG_MSG((_T("CSerialPort::WriteCmd - TrIndex=%02X, CtrlIndex=%02X, Cmd=%02X\n"), TrIndex, CtrlIndex, Cmd));
	UCHAR	p[1024] = {};
	ULONG	Result = ERROR_SUCCESS;

	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = TrIndex;
	p[3] = CtrlIndex;
	p[4] = Cmd;
	
	if ((pBuffer != NULL) && (Length > 0))
	{
		memcpy (&p[5], pBuffer, Length);
	}
	
	p[Length + 5] = 0xFD;

	if (Time)
	{
		m_RespWaitStartTime = ::GetTickCount64 ();
		m_RespLen = 5;
		m_RespBuff[0] = p[0];
		m_RespBuff[1] = p[1];
		m_RespBuff[2] = p[3];
		m_RespBuff[3] = p[2];
		m_RespBuff[4] = 0xFB;
	}

	Write (p, Length + 6);

	if (Time)
	{
		Result = WaitResp (Time);
	}

	//DBG_MSG((_T("CSerialPort::WriteCmd - END\n")));

	return Result;
}

VOID CSerialPort::SetWaitResp (PUCHAR pBuffer, ULONG Length)
{
	m_RespWaitStartTime = ::GetTickCount64 ();
	m_RespLen = Length;
	memcpy (m_RespBuff, pBuffer, m_RespLen);
}

VOID CSerialPort::SetWaitResp (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd)
{
	UCHAR	p[5] = {};
	
	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = CtrlIndex;
	p[3] = TrIndex;
	p[4] = Cmd;
	
	SetWaitResp (p , sizeof (p));
}

VOID CSerialPort::SetWaitResp (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd)
{
	UCHAR	p[6] = {};
	
	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = CtrlIndex;
	p[3] = TrIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	
	SetWaitResp (p , sizeof (p));
}

VOID CSerialPort::SetWaitResp (UCHAR TrIndex, UCHAR CtrlIndex, UCHAR Cmd, UCHAR SubCmd, UCHAR SubCmd2)
{
	UCHAR	p[7] = {};
	
	p[0] = 0xFE;
	p[1] = 0xFE;
	p[2] = CtrlIndex;
	p[3] = TrIndex;
	p[4] = Cmd;
	p[5] = SubCmd;
	p[6] = SubCmd2;
	
	SetWaitResp (p , sizeof (p));
}

ULONG CSerialPort::WaitResp (ULONG Time)
{
	HANDLE	Handles[2];
	ULONG	Result;

	Handles[0] = m_hRespEvent;
	Handles[1] = m_hStopEvent;

	Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, Time);

	ULONG WaitTime = static_cast<ULONG>(GetTickCount64 () - m_RespWaitStartTime);
	if (Result == WAIT_OBJECT_0)
	{
		DBG_MSG((_T("CSerialPort::WaitResp - COMPLETE(Time=%u)\n"), WaitTime));
	}
	else
	{
		DBG_MSG((_T("CSerialPort::WaitResp - FAILURE(Time=%u)\n"), WaitTime));
	}

	return Result;
}

ULONG CSerialPort::AddCommand (ULONG Type, PVOID pBuffer, ULONG Length)
{
	CList<CMemFileEx*>	CommandList;
	CMemFileEx*			pMemFile;
	SERIAL_COMMAND*		pCmdHdr;

	do
	{
		pMemFile = m_CommandList.GetFreeMemFile ();
		ASSERT (pMemFile != NULL);
		if (pMemFile == NULL)
		{	break;
		}

		pMemFile->SetLength (sizeof (SERIAL_COMMAND) + Length);
		pCmdHdr = reinterpret_cast<SERIAL_COMMAND*>(pMemFile->GetBuffer ());

		pCmdHdr->Type = Type;
		memcpy (pCmdHdr + 1, pBuffer, Length);

		m_CommandList.UsedMemFile (pMemFile);
		::SetEvent (m_hCommand);
	}
	while (0);

	return ERROR_SUCCESS;
}

ULONG CSerialPort::Command (ULONG Type, PVOID pBuffer, ULONG Length)
{
	CSingleLock	Lock (&m_CommandWriteLock, TRUE);

	return OnCommand (Type, pBuffer, Length);
}

ULONG CSerialPort::OnCommand (ULONG Type, PVOID pBuffer, ULONG Length)
{
	return ERROR_NOT_SUPPORTED;
}

ULONG CSerialPort::OnReadChar (ULONG Type, UCHAR Char)
{
	ULONG	Result = ERROR_NOT_READY;

	//DBG_MSG((_T("CSerialPort::OnReadChar - START\n")));

	do
	{
		if (Type == TYPE_ERR)
		{
			DBG_MSG((_T("CSerialPort::OnReadChar - ERR DETECTED\n")));
			
			//clear
			m_MemFile.SetLength (0);

			Result = ERROR_SUCCESS;
			break;
		}

		//デリミタ
		if (Char == 0xFD)
		{
			m_MemFile.Write (&Char, sizeof (Char));
		
			OnRead (TYPE_CHAR, m_MemFile.GetBuffer (), static_cast<ULONG>(m_MemFile.GetLength ()));

			m_MemFile.SetLength (0);

			Result = ERROR_SUCCESS;
			break;
		}

		//プリアンブル
		if (Char == 0xFE)
		{
			//0xFEが来たら先頭バイトとして処理する(0xFE -> 0xFE, 0xFEになる)
			m_MemFile.SetLength (0);
			m_MemFile.Write (&Char, sizeof (Char));
			m_MemFile.Write (&Char, sizeof (Char));

			Result = ERROR_SUCCESS;
			break;
		}

	}
	while (0);

	//DBG_MSG((_T("CSerialPort::OnReadChar - END\n")));
	return Result;
}


ULONG CSerialPort::OnRead (ULONG Type, PUCHAR pBuffer, ULONG Length)
{
	ULONG	CIV8;
	ULONG	CIV16;
	ULONG	CIV24;
	UCHAR	Trx		= 0;
	ULONG	Result	= ERROR_NOT_READY;

	//DBG_MSG((_T("CSerialPort::OnRead\n")));

	do
	{
		if (Type == TYPE_ERR)
		{

			Result = ERROR_SUCCESS;
			break;
		}

		ASSERT (m_TrxAddr[0] != 0x00);
		ASSERT (m_TrxAddr[1] != 0x00);
		ASSERT (m_CtlAddr[0] != 0x00);
		ASSERT (m_CtlAddr[1] != 0x00);

		//エコーバックかチェック
		if (((m_TrxAddr[0] != 0x00) && (pBuffer[2] == m_TrxAddr[0])) && 
			((m_CtlAddr[0] != 0x00) && (pBuffer[3] == m_CtlAddr[0])))
		{
			Result = ERROR_INVALID_DATA;
			break;
		}

		//エコーバックかチェック
		if (((m_TrxAddr[1] != 0x00) && (pBuffer[2] == m_TrxAddr[1])) && 
			((m_CtlAddr[1] != 0x00) && (pBuffer[3] == m_CtlAddr[1])))
		{
			Result = ERROR_INVALID_DATA;
			break;
		}

		//レスポンスかチェック
		if ((m_RespLen > 0) && (m_RespLen <= Length))
		{
			if (memcmp (pBuffer, m_RespBuff, m_RespLen) == 0)
			{
				DBG_MSG((_T("CSerialPort::OnRead - RespEvent!! \n")));
				DBG_BUF(m_RespBuff, m_RespLen);
				::SetEvent (m_hRespEvent);
				m_RespLen = 0;
			}
		}

		//機種依存番号を統一
		if ((m_TrxAddr[0] != 0x00) && (pBuffer[3] == m_TrxAddr[0]))
		{
			Trx = 0;
		}
		else if ((m_TrxAddr[1] != 0x00) && (pBuffer[3] == m_TrxAddr[1]))
		{
			Trx = 1;
		}

		pBuffer[3] = Trx;
		pBuffer[2] = 0xE0;



		CIV8  = (pBuffer[3] << 24) | (pBuffer[4] << 16);
		CIV16 = (pBuffer[3] << 24) | (pBuffer[4] << 16) | (pBuffer[5] << 8);
		CIV24 = (pBuffer[3] << 24) | (pBuffer[4] << 16) | (pBuffer[5] << 8) | (pBuffer[6] << 0);
		
		Result = ERROR_SUCCESS;

		switch (CIV8)
		{
		case CIV_RECV_FREQUENCY1:
		case CIV_RECV_FREQUENCY2:
			{
				//CIV_RECV_FREQUENCY -> CIV_READ_FREQUENCY
				pBuffer[4] = (CIV_READ_FREQUENCY1 >> 16) & 0xFF;
				Callback (pBuffer, Length);
			}
			break;

		case CIV_RECV_OPERATING_MODE1:
		case CIV_RECV_OPERATING_MODE2:
			{
				//CIV_RECV_OPERATING_MODE -> CIV_READ_OPERATING_MODE
				pBuffer[4] = (CIV_READ_OPERATING_MODE1 >> 16) & 0xFF;
				Callback (pBuffer, Length);
			}
			break;

		case CIV_READ_FREQUENCY1:
		case CIV_READ_FREQUENCY2:
		case CIV_READ_OPERATING_MODE1:
		case CIV_READ_OPERATING_MODE2:
		case CIV_READ_DUPLEX_OFFSET1:
		case CIV_READ_DUPLEX_OFFSET2:
		case CIV_DUPLEX_MODE1:
		case CIV_DUPLEX_MODE2:
		case CIV_ATTENUATOR:
		case CIV_READ_ID:
		case CIV_PTT_STATUS:
			{
				Callback (pBuffer, Length);
			}
			break;

		case CIV_MAIN_POWER:
			{
				if (m_PrevPowerState != pBuffer[5])
				{
					m_PrevPowerState = pBuffer[5];
					Callback (pBuffer, Length);
				}
			}
			break;

		case CIV_ACK_OK1:
		case CIV_ACK_OK2:
			{
			}
			break;

		case CIV_ACK_NG1:
		case CIV_ACK_NG2:
			{
				ASSERT (FALSE);
			}
			break;

		default:
			{
				switch (CIV16)
				{
				case CIV_AF_VOLUME1:
				case CIV_AF_VOLUME2:
				case CIV_SQL_LEVEL1:
				case CIV_SQL_LEVEL2:
				case CIV_RF_POWER1:
				case CIV_RF_POWER2:
				case CIV_NOISE_SQL1:
				case CIV_NOISE_SQL2:
				case CIV_DV_MY_CALLSIGN_MEMCH:
				case CIV_DV_RX_CALLSIGN:
				case CIV_DV_TX_CALLSIGN:
				case CIV_DV_BREAK_IN_MODE:
				case CIV_DV_FM_DETECTED:
				case CIV_DV_DATA:
				case CIV_DV_RX_MESSAGE:
				case CIV_DV_TX_MESSAGE_MEMCH:
				case CIV_DV_TX_MESSAGE_ENABLE:
				case CIV_DV_EMERGENCY_MODE:
					{
						Callback (pBuffer, Length);
					}
					break;

				case CIV_SIGNAL_LEVEL1:
				case CIV_SIGNAL_LEVEL2:
					{
						ULONG SignalLevel = *reinterpret_cast<PUSHORT>(&pBuffer[6]);

						if (m_PrevSignalLevel[Trx] != SignalLevel)
						{
							m_PrevSignalLevel[Trx] = SignalLevel;
							Callback (pBuffer, Length);
						}
					}
					break;

				default:
					{
						switch (CIV24)
						{
						case CIV_MONITOR_MODE:
						case CIV_DUAL_WATCH_MODE:
						case CIV_MAIN_BAND_SELECTION:
						case CIV_VFO_MR_CALL_SELECTION1:
						case CIV_VFO_MR_CALL_SELECTION2:
						case CIV_DV_TX_MESSAGE1:
						case CIV_DV_TX_MESSAGE2:
						case CIV_DV_TX_MESSAGE3:
						case CIV_DV_TX_MESSAGE4:
						case CIV_DV_TX_MESSAGE5:
						case CIV_DV_MY_CALLSIGN1:
						case CIV_DV_MY_CALLSIGN2:
						case CIV_DV_MY_CALLSIGN3:
						case CIV_DV_MY_CALLSIGN4:
						case CIV_DV_MY_CALLSIGN5:
						case CIV_DV_MY_CALLSIGN6:
							{
								Callback (pBuffer, Length);
							}
							break;

						default:
							{
								DBG_MSG((_T("CSerialPort::OnRead - UNKNOWN CIV(%06X or %06X or %06X)!! \n"), CIV8, CIV16, CIV24));
								Result = ERROR_NOT_READY;
							}
							break;
						}
					}
					break;
				}
			}
			break;
		}

	}
	while (0);

	return Result;
}
