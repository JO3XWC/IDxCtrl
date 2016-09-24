#pragma once
#include "CSerialPort.h"

class CID92SerialPort : public CSerialPort
{
public:
	CID92SerialPort();
	virtual ~CID92SerialPort();

protected:
virtual ULONG	OnCommand		(ULONG Type, PVOID pBuffer, ULONG Length);
virtual ULONG	OnRead			(ULONG Type, PUCHAR pBuffer = NULL, ULONG Length = 0);
virtual ULONG	OnReadChar		(ULONG Type, UCHAR Data);


	
	enum
	{
		CMD_OFFSET	= 0x04,
#if 1
		TRX1		= 0x01,
		TRX2		= 0x02,
		CTL1		= 0x80,
		CTL2		= 0x81,
#else
		TRX1		= 0x84,
		TRX2		= 0x84,
		CTL1		= 0xE0,
		CTL2		= 0xE0,
#endif
	};
};

