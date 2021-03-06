#pragma once

enum
{
	DV_CTRL_FLAG_NULL,
	DV_CTRL_FLAG_RELAY_UNAVAILABLE,
	DV_CTRL_FLAG_NO_RESPONSE,
	DV_CTRL_FLAG_ACK,
	DV_CTRL_FLAG_RESENT,
	DV_CTRL_FLAG_UNUSED,
	DV_CTRL_FLAG_AUTO_REPLY,
	DV_CTRL_FLAG_REPEATER_CTRL,
};

enum
{
	SELECT_VFO,
	SELECT_MR1,
	SELECT_MR2,
	SELECT_CALL,
};

enum
{
	DUP_MODE_SIMPLEXT	= 0x10,
	DUP_MODE_MINUS		= 0x11,
	DUP_MODE_PLUS		= 0x12,

	SQL_OPEN		= 0,
	SQL_AUTO		= 1,
	SQL_LEVEL1		= 2,
	SQL_LEVEL_MAX	= 10,

	VOL_LEVEL1		= 0,
	VOL_LEVEL_MAX	= 39,
};

typedef struct _DV_FLAG
{
	UCHAR	CtrlFlag	: 3,
			Urgent		: 1,
			Control		: 1,
			Interruption: 1,
			Relay		: 1,
			Data		: 1;
}DV_FLAG, *PDV_FLAG;

enum 
{
	OPERATING_MODE_AM		= 0x0102,
	OPERATING_MODE_FM		= 0x0105,
	OPERATING_MODE_FMN		= 0x0205,
	OPERATING_MODE_WFM		= 0x0106,
	OPERATING_MODE_DV		= 0x01D0,
	OPERATING_MODE_DD		= 0x01D1,
	OPERATING_MODE_AUTO		= 0x01A0,
};

enum
{
	CIV_TRX1					= 0x00000000,
	CIV_TRX2					= 0x01000000,
	CIV_TRX_MASK				= 0x01000000,
	CIV_CMD_MASK				= 0x00FFFFFF,
															//Ack
	CIV_RECV_FREQUENCY1			= 0x00000000 | CIV_TRX1,
	CIV_RECV_FREQUENCY2			= 0x00000000 | CIV_TRX2,

	CIV_RECV_OPERATING_MODE1	= 0x00010000 | CIV_TRX1,
	CIV_RECV_OPERATING_MODE2	= 0x00010000 | CIV_TRX2,

	CIV_READ_FREQUENCY1			= 0x00030000 | CIV_TRX1,
	CIV_READ_FREQUENCY2			= 0x00030000 | CIV_TRX2,

	CIV_READ_OPERATING_MODE1	= 0x00040000 | CIV_TRX1,
	CIV_READ_OPERATING_MODE2	= 0x00040000 | CIV_TRX2,

	CIV_WRITE_FREQUENCY1		= 0x00050000 | CIV_TRX1,	//FB
	CIV_WRITE_FREQUENCY2		= 0x00050000 | CIV_TRX2,	//FB

	CIV_WRITE_OPERATING_MODE1	= 0x00060000 | CIV_TRX1,	//FB
	CIV_WRITE_OPERATING_MODE2	= 0x00060000 | CIV_TRX2,	//FB

	CIV_READ_DUPLEX_OFFSET1		= 0x000C0000 | CIV_TRX1,
	CIV_READ_DUPLEX_OFFSET2		= 0x000C0000 | CIV_TRX2,
	
	CIV_WRITE_DUPLEX_OFFSET1	= 0x000D0000 | CIV_TRX1,	//FB
	CIV_WRITE_DUPLEX_OFFSET2	= 0x000D0000 | CIV_TRX2,	//FB

	CIV_DUPLEX_MODE1			= 0x000F0000 | CIV_TRX1,	//FB
	CIV_DUPLEX_MODE2			= 0x000F0000 | CIV_TRX2,	//FB

	CIV_ATTENUATOR				= 0x00110000,

	CIV_AF_VOLUME1				= 0x00140100 | CIV_TRX1,	//FB?
	CIV_AF_VOLUME2				= 0x00140100 | CIV_TRX2,	//FB?

	CIV_SQL_LEVEL1				= 0x00140300 | CIV_TRX1,	//FB?
	CIV_SQL_LEVEL2				= 0x00140300 | CIV_TRX2,	//FB?

	CIV_RF_POWER1				= 0x00140A00 | CIV_TRX1,
	CIV_RF_POWER2				= 0x00140A00 | CIV_TRX2,

	CIV_NOISE_SQL1				= 0x00150100 | CIV_TRX1,
	CIV_NOISE_SQL2				= 0x00150100 | CIV_TRX2,
	CIV_SIGNAL_LEVEL1			= 0x00150200 | CIV_TRX1,
	CIV_SIGNAL_LEVEL2			= 0x00150200 | CIV_TRX2,

	CIV_MAIN_POWER				= 0x00180000,
	CIV_READ_ID					= 0x00190000,

	CIV_MONITOR_MODE			= 0x001A0301,
	CIV_DUAL_WATCH_MODE			= 0x001A0400,
	CIV_MAIN_BAND_SELECTION		= 0x001A0401,

	CIV_VFO_MR_CALL_SELECTION1	= 0x001A0402 | CIV_TRX1,
	CIV_VFO_MR_CALL_SELECTION2	= 0x001A0402 | CIV_TRX2,

	CIV_TOT						= 0x001A054A,

	CIV_PTT_STATUS				= 0x001C0000 | CIV_TRX1,

	CIV_DV_MY_CALLSIGN_MEMCH	= 0x001D0200 | CIV_TRX2,	//TRX2
	CIV_DV_RX_CALLSIGN			= 0x001D0400 | CIV_TRX2,	//TRX2
	CIV_DV_TX_CALLSIGN			= 0x001D0500 | CIV_TRX2,	//TRX2
//	CIV_DV_MY_CALLSIGN_ALL		= 0x001D0800 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN1			= 0x001D0801 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN2			= 0x001D0802 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN3			= 0x001D0803 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN4			= 0x001D0804 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN5			= 0x001D0805 | CIV_TRX2,	//TRX2
	CIV_DV_MY_CALLSIGN6			= 0x001D0806 | CIV_TRX2,	//TRX2
	CIV_DV_BREAK_IN_MODE		= 0x001D1000 | CIV_TRX2,	//TRX2

	CIV_DV_FM_DETECTED			= 0x001D1B00 | CIV_TRX2,	//TRX2

	CIV_DV_DATA					= 0x001DC000 | CIV_TRX2,	//TRX2

	CIV_DV_RX_MESSAGE			= 0x001DDF00 | CIV_TRX2,	//TRX2

	CIV_DV_TX_MESSAGE1			= 0x001DE001 | CIV_TRX2,	//TRX2
	CIV_DV_TX_MESSAGE2			= 0x001DE002 | CIV_TRX2,	//TRX2
	CIV_DV_TX_MESSAGE3			= 0x001DE003 | CIV_TRX2,	//TRX2
	CIV_DV_TX_MESSAGE4			= 0x001DE004 | CIV_TRX2,	//TRX2
	CIV_DV_TX_MESSAGE5			= 0x001DE005 | CIV_TRX2,	//TRX2

	CIV_DV_TX_MESSAGE_MEMCH		= 0x001DE100 | CIV_TRX2,	//TRX2
	CIV_DV_TX_MESSAGE_ENABLE	= 0x001DE200 | CIV_TRX2,	//TRX2
	
	CIV_DV_EMERGENCY_MODE		= 0x001DEC00 | CIV_TRX2,

	CIV_ACK_OK1					= 0x00FB0000 | CIV_TRX1,
	CIV_ACK_OK2					= 0x00FB0000 | CIV_TRX2,
	CIV_ACK_NG1					= 0x00FA0000 | CIV_TRX1,
	CIV_ACK_NG2					= 0x00FA0000 | CIV_TRX2,

	CIV_RESYNC					= 0x00FFFFFE,
	CIV_ALL_STATUS				= 0x00FFFFFF,
};