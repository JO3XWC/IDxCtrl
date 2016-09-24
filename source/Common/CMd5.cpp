#include "StdAfx.h"
#include "Cmd5.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UCHAR CMd5::m_Padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


CMd5::CMd5(void)
{
	ZeroMemory (this, sizeof (CMd5));
	Starts ();
}

CMd5::~CMd5(void)
{
}


void CMd5::Starts ()
{
    m_Total[0] = 0;
    m_Total[1] = 0;
    m_State[0] = 0x67452301;
    m_State[1] = 0xEFCDAB89;
    m_State[2] = 0x98BADCFE;
    m_State[3] = 0x10325476;
}

void CMd5::Update (const void* pInput, ULONG Length)
{
    ULONG	Left;
	ULONG	Fill;
	UCHAR*	Input = reinterpret_cast<PUCHAR>(const_cast<void*>(pInput));

    if( ! Length )
	{	return;
	}

    Left = ( m_Total[0] >> 3 ) & 0x3F;
    Fill = 64 - Left;

    m_Total[0] += Length <<  3;
    m_Total[1] += Length >> 29;

    m_Total[0] &= 0xFFFFFFFF;
    m_Total[1] += m_Total[0] < ( Length << 3 );

    if( Left && Length >= Fill )
    {
        memcpy (m_pBuffer + Left, Input, Fill );
        Process (m_pBuffer );
        Length -= Fill;
        Input  += Fill;
        Left = 0;
    }

    while( Length >= 64 )
    {
        Process (Input);
        Length -= 64;
        Input  += 64;
    }

    if( Length )
    {	memcpy(m_pBuffer + Left, Input, Length );
    }
}

void CMd5::Finish (UCHAR* pDigest, INT DigestLen)
{
    ULONG	Last;
	ULONG	Padn;
    ULONG	MsgLen[16];

	MsgLen[14] = m_Total[0];
	MsgLen[15] = m_Total[1];

    Last = ( m_Total[0] >> 3 ) & 0x3F;
    Padn = ( Last < 56 ) ? ( 56 - Last ) : ( 120 - Last );

    Update (m_Padding, Padn);

	for (ULONG i = 0; i < 14; i++)
	{	MsgLen [i] = reinterpret_cast<PULONG>(m_pBuffer) [i];
	}
	Process (reinterpret_cast<PUCHAR>(MsgLen));

	for (int i=0;i<4;i++)
	{	reinterpret_cast<PULONG>(&m_Digest)[i] = m_State[i];
	}

	if (pDigest != NULL)
	{
		memcpy (pDigest, m_Digest, min (DigestLen, sizeof (m_Digest)));
	}
}


 
