#include "stdafx.h"
#include "CID92SerialPort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CMD_WAIT_TIME	(500)

CID92SerialPort::CID92SerialPort()
{
	SetTrxAddr (TRX1, TRX2);
	SetCtlAddr (CTL1, CTL2);
}


CID92SerialPort::~CID92SerialPort()
{
}

ULONG CID92SerialPort::OnCommand (ULONG Type, PVOID pInBuffer, ULONG Length)
{
	ULONG			Result		= ERROR_SUCCESS;
	DCB				Dcb			= {sizeof (Dcb)};
	UCHAR			Trx;
	UCHAR			Cmd;
	UCHAR			SubCmd;
	UCHAR			SubCmd2;
	PUCHAR			pBuffer		= reinterpret_cast<PUCHAR>(pInBuffer);

	//DBG_MSG((_T("CID92SerialPort::Command - START(Type=%08X)\n"), Type));
	Trx = m_TrxAddr[static_cast<UCHAR>((Type & CIV_TRX_MASK) >> 24)];
	Cmd = static_cast<UCHAR>((Type >> 16) & 0xFF);
	SubCmd = (Type >> 8) & 0xFF;
	SubCmd2 = (Type >> 0) & 0xFF;

	switch (Type)
	{
	case CIV_RESYNC:
		{
			UCHAR	p4800[]		= { 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, TRX1, CTL1, 0x19, 0xFD}; 
			UCHAR	p38400[406]	= { 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 
									0xFE, 0xFE, TRX1, CTL1, 0x19, 0xFD};
		
			Cmd = (CIV_READ_ID >> 16) & 0xFF;
#if 0
			SetBaudRate (CBR_19200);
			Sleep (100);
			
			Cmd = (CIV_READ_OPERATING_MODE1 >> 16) & 0xFF;
			Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			break;
#endif

			SetBaudRate (CBR_38400);
			
			//Sleep (100);
			SetWaitResp (Trx, CTL1, Cmd);
			Write (p38400, sizeof (p38400));
			Result = WaitResp (CMD_WAIT_TIME);
			if (Result == ERROR_SUCCESS)
			{	break;
			}
		
			SetBaudRate (CBR_4800);
			
			//Sleep (100);

			SetWaitResp (Trx, CTL1, Cmd);
			Write (p4800, sizeof (p4800));
			WaitResp (CMD_WAIT_TIME);
		
			SetBaudRate (CBR_38400);
			
			//Sleep (100);
			SetWaitResp (Trx, CTL1, Cmd);
			Write (p38400, sizeof (p38400));
			Result = WaitResp (CMD_WAIT_TIME);
		}
		break;

	case CIV_READ_ID:
		{
			Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
		}
		break;

	case CIV_READ_FREQUENCY1:
	case CIV_READ_FREQUENCY2:
	case CIV_READ_OPERATING_MODE1:
	case CIV_READ_OPERATING_MODE2:
	case CIV_READ_DUPLEX_OFFSET1:
	case CIV_READ_DUPLEX_OFFSET2:
		{
			Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
		}
		break;

	case CIV_WRITE_FREQUENCY1:
	case CIV_WRITE_FREQUENCY2:
		{
			Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			//if (Result == ERROR_SUCCESS)
			{
				Result = WriteCmd (Trx, CTL1, (CIV_READ_FREQUENCY1 >> 16) & 0xFF, NULL, 0, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_WRITE_OPERATING_MODE1:
	case CIV_WRITE_OPERATING_MODE2:
		{
			Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			//if (Result == ERROR_SUCCESS)
			{
				Result = WriteCmd (Trx, CTL1, (CIV_READ_OPERATING_MODE1 >> 16) & 0xFF, NULL, 0, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_WRITE_DUPLEX_OFFSET1:
	case CIV_WRITE_DUPLEX_OFFSET2:
		{
			Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			//if (Result == ERROR_SUCCESS)
			{
				Result = WriteCmd (Trx, CTL1, (CIV_READ_DUPLEX_OFFSET1 >> 16) & 0xFF, NULL, 0, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_DUPLEX_MODE1:
	case CIV_DUPLEX_MODE2:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_ATTENUATOR:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_MAIN_POWER:
		{
			if (Length > 0)
			{
				if (pBuffer[0] == 0x00)//OFF
				{
					Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
					
					if (Result == ERROR_SUCCESS)
					{
						//Power OFF
						UCHAR pOFF[] = { 0xFE, 0xFE, 0xE0, 0x00, 0x18, 0x00, 0xFD};
						OnRead (TYPE_CHAR, pOFF, sizeof (pOFF));
					}
				}
				else//ON
				{
					Result = OnCommand (CIV_RESYNC, NULL, 0);
					if (Result != ERROR_SUCCESS)
					{	break;
					}

					Result = WriteCmdFB (Trx, CTL1, Cmd, pBuffer, Length, 2000);
					if (Result == ERROR_SUCCESS)
					{
						Result = OnCommand (CIV_ALL_STATUS, NULL, 0);
					}
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_SIGNAL_LEVEL1:
	case CIV_SIGNAL_LEVEL2:
	case CIV_NOISE_SQL1:
	case CIV_NOISE_SQL2:
		{
			Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
		}
		break;

	case CIV_AF_VOLUME1:
	case CIV_AF_VOLUME2:
	case CIV_SQL_LEVEL1:
	case CIV_SQL_LEVEL2:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_RF_POWER1:
	case CIV_RF_POWER2:
	case CIV_PTT_STATUS:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	case CIV_MONITOR_MODE:
	case CIV_DUAL_WATCH_MODE:
	case CIV_MAIN_BAND_SELECTION:
	case CIV_VFO_MR_CALL_SELECTION1:
	case CIV_VFO_MR_CALL_SELECTION2:
	case CIV_DV_MY_CALLSIGN1:
	case CIV_DV_MY_CALLSIGN2:
	case CIV_DV_MY_CALLSIGN3:
	case CIV_DV_MY_CALLSIGN4:
	case CIV_DV_MY_CALLSIGN5:
	case CIV_DV_MY_CALLSIGN6:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, SubCmd, SubCmd2, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, SubCmd2, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, SubCmd2, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;


	case CIV_DV_MY_CALLSIGN_MEMCH:
	case CIV_DV_TX_CALLSIGN:
	case CIV_DV_BREAK_IN_MODE:
	case CIV_DV_TX_MESSAGE_MEMCH:
	case CIV_DV_TX_MESSAGE_ENABLE:
	case CIV_DV_EMERGENCY_MODE:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, CMD_WAIT_TIME);
			}
		}
		break;

	//case CIV_DV_MY_CALLSIGN_ALL:
	//	{
	//		SetWaitResp (Trx, CTL1, Cmd, SubCmd, 0x06);
	//		WriteCmd (Trx, CTL1, Cmd, SubCmd, pBuffer, Length, 0);
	//		Result = WaitResp (CMD_WAIT_TIME);
	//	}
	//	break;

	case CIV_DV_TX_MESSAGE1:
	case CIV_DV_TX_MESSAGE2:
	case CIV_DV_TX_MESSAGE3:
	case CIV_DV_TX_MESSAGE4:
	case CIV_DV_TX_MESSAGE5:
		{
			if (Length > 0)
			{
				Result = WriteCmdFB (Trx, CTL1, Cmd, SubCmd, SubCmd2, pBuffer, Length, CMD_WAIT_TIME);
				//if (Result == ERROR_SUCCESS)
				{
					Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, SubCmd2, NULL, 0, CMD_WAIT_TIME);
				}
			}
			else
			{
				if ((Result = WriteCmd (Trx, CTL1, Cmd, SubCmd, SubCmd2, pBuffer, Length, CMD_WAIT_TIME)) != ERROR_SUCCESS)
				{	break;
				}
			}
		}
		break;

	case CIV_ALL_STATUS:
		{
			Result = Command (CIV_RESYNC);
			if (Result != ERROR_SUCCESS)
			{	break;
			}

			for (ULONG i=0;i<g_nCiv_All_Status;i++)
			{
				if ((Result = Command (g_Civ_All_Status[i])) != ERROR_SUCCESS)
				{	break;
				}
			}
		}
		break;

	case CIV_RECV_FREQUENCY1:
	case CIV_RECV_FREQUENCY2:
		{
			ASSERT (FALSE);
		}
		break;
	default:
		{
			ASSERT (FALSE);
		}
		break;
	}

	//DBG_MSG((_T("CID92SerialPort::Command - END(Type=%08X)\n"), Type));

	return Result;
}


ULONG CID92SerialPort::OnRead (ULONG Type, PUCHAR pBuffer, ULONG Length)
{
	USHORT	CIV8;
	USHORT	CIV16;
	ULONG	Result = ERROR_NOT_READY;

	DBG_MSG((_T("CID92SerialPort::OnRead\n")));
	DBG_BUF (pBuffer, Length);

	do
	{
		//CIV Convert
		//pBuffer[CMD_OFFSET] = CIV_READ_ID;


		//
		Result = CSerialPort::OnRead (Type, pBuffer, Length);
		if (Result == ERROR_SUCCESS)
		{	break;
		}

		CIV8 = pBuffer[4] << 8;
		CIV16 = (pBuffer[4] << 8) | pBuffer[5];

		//switch (CIV8)
		//{
		//default:
		//	{
		//		DBG_MSG((_T("CID92SerialPort::OnRead - UNKNOWN CIV8(%04X)!! \n"), CIV8));
		//		Result = ERROR_NOT_READY;
		//	}
		//	break;
		//}
	}
	while (0);

	return Result;
}

ULONG CID92SerialPort::OnReadChar (ULONG Type, UCHAR Char)
{
	//DBG_MSG((_T("CID92SerialPort::OnReadChar - START\n")));
	ULONG	Result = ERROR_NOT_READY;

	do
	{
		Result = CSerialPort::OnReadChar (Type, Char);
		if (Result == ERROR_SUCCESS)
		{	break;
		}

		//先頭バイト
		if (m_MemFile.GetLength () == 0)
		{
			if (Char == 0x11)//パワーon
			{
				DBG_MSG((_T("EID9xSerialPort::OnReadChar - START CHAR 0x%02X(POWER ON)\n"), Char));
				
				UCHAR p[] = { 0xFE, 0xFE, CTL1, TRX1, 0x18, 0x01, 0xFD};
				OnRead (TYPE_CHAR, p, sizeof (p));

				Result = ERROR_SUCCESS;
				break;
			}

			if (Char == 0x00)//パワーoFF
			{
				DBG_MSG((_T("EID9xSerialPort::OnReadChar - START CHAR 0x%02X(POWER OFF)\n"), Char));
			
				UCHAR p[] = { 0xFE, 0xFE, CTL1, TRX1, 0x18, 0x00, 0xFD};
				OnRead (TYPE_CHAR, p, sizeof (p));

				Result = ERROR_SUCCESS;
				break;
			}


			DBG_MSG((_T("EID9xSerialPort::OnReadChar - START CHAR 0x%02X(UNKNOWN)\n"), Char));

			//たまに出る?
			if (Char == 0x13)
			{
				//::ResetEvent (m_hReadEvent);

				Result = ERROR_SUCCESS;
				break;
			}

			//ASSERT (FALSE);
		}


		m_MemFile.Write (&Char, sizeof (Char));
	}
	while (0);

	//DBG_MSG((_T("CID92SerialPort::OnReadChar - END\n")));
	return Result;
}
