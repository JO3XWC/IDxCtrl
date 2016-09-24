#pragma once
#include "CMemFileEx.h"

#define AUTH_APP_TO_SERVER	{0xf2d4f43d, 0xef51, 0x43b1, 0x8e, 0xcb, 0x12, 0x13, 0xd7, 0x46, 0x35, 0x4c}
#define AUTH_SERVER_TO_APP	{0x58c9aef6, 0x9ab2, 0x46e3, 0xb8, 0x88, 0x53, 0x18, 0xa6, 0x10, 0x15, 0xc1}



#pragma pack (push, 1)

typedef struct _PACKET_BASE_HEADER
{
	ULONG GetPacketLength ()
	{
		return m_Length;
	}

	_PACKET_BASE_HEADER* GetNext ()
	{
		return reinterpret_cast<_PACKET_BASE_HEADER*>(reinterpret_cast<PUCHAR>(this) + GetPacketLength ());
	}
//-------------------
	ULONG	m_Length;
}PACKET_BASE_HEADER, *PPACKET_BASE_HEADER;


typedef struct _PACKET_CMD_HEADER : public PACKET_BASE_HEADER
{
	ULONG GetDatatLength ()
	{
		return m_Length - sizeof (PACKET_CMD_HEADER);
	}

	PVOID GetData ()
	{
		return (this + 1);
	}

//-------------------
	ULONG	m_Type;
}PACKET_CMD_HEADER, *PPACKET_CMD_HEADER;





#pragma pack(pop)


class CTcpPacket : public CMemFileEx
{
public:
	PACKET_BASE_HEADER* GetPacket ()
	{
		PACKET_BASE_HEADER* pHeader = NULL;

		do
		{
			if (GetLength () < sizeof (PACKET_BASE_HEADER))
			{	break;
			}

			pHeader = reinterpret_cast<PACKET_BASE_HEADER*>(GetBuffer ());
			if (pHeader->m_Length > GetLength ())
			{
				pHeader = NULL;
				break;
			}
		}
		while (0);

		return pHeader;
	}

	PACKET_BASE_HEADER* RemovePacket ()
	{
		PACKET_BASE_HEADER* pHeader = NULL;
		CSingleLock			Lock (&m_Lock, TRUE);
		ULONGLONG			PacketLength;

		do
		{
			pHeader = GetPacket ();
			if (pHeader == NULL)
			{	break;
			}

			PacketLength = pHeader->GetPacketLength ();
			memcpy (GetBuffer (), pHeader->GetNext (), static_cast<size_t>(GetLength () - PacketLength));
			SetLength (GetLength () - PacketLength);

			pHeader = GetPacket ();
		}
		while (0);

		return pHeader;
	}

};


