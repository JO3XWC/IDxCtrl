#pragma once

class CMd5
{
public:
	CMd5(void);
	~CMd5(void);

	void	Starts		();
	void	Update		(const void* pInput, ULONG Length );
	void	Finish		(UCHAR* pDigest = NULL, INT DigestLen = 0);

private:
	void	Process		(UCHAR data[64]);
	VOID	Process1	(PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t);
	VOID	Process2	(PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t);
	VOID	Process3	(PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t);
	VOID	Process4	(PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t);

private:
	ULONG	m_Total[2];
	ULONG	m_State[4];
	UCHAR	m_pBuffer[64];
	UCHAR	m_Digest[16];

static	UCHAR	m_Padding[64];

};



inline VOID CMd5::Process1 (PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t)
{
	ULONG a,b,c,d;

	a = pValue[(Start + 0) % 4];
	b = pValue[(Start + 1) % 4];
	c = pValue[(Start + 2) % 4];
	d = pValue[(Start + 3) % 4];

	a += (d ^ (b & (c ^ d))) + k + t;
	a = ((a << s) | ((a & 0xFFFFFFFF) >> (32 - s))) + b;

	pValue [Start] = a;
}


inline VOID CMd5::Process2 (PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t)
{
	ULONG a,b,c,d;

	a = pValue[(Start + 0) % 4];
	b = pValue[(Start + 1) % 4];
	c = pValue[(Start + 2) % 4];
	d = pValue[(Start + 3) % 4];

	a += (c ^ (d & (b ^ c))) + k + t;
	a = ((a << s) | ((a & 0xFFFFFFFF) >> (32 - s))) + b;

	pValue [Start] = a;
}

inline VOID CMd5::Process3 (PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t)
{
	ULONG a,b,c,d;

	a = pValue[(Start + 0) % 4];
	b = pValue[(Start + 1) % 4];
	c = pValue[(Start + 2) % 4];
	d = pValue[(Start + 3) % 4];

	a += (b ^ c ^ d) + k + t;
	a = ((a << s) | ((a & 0xFFFFFFFF) >> (32 - s))) + b;

	pValue [Start] = a;
}

inline VOID CMd5::Process4 (PULONG pValue, ULONG Start, ULONG k, ULONG s, ULONG t)
{
	ULONG a,b,c,d;

	a = pValue[(Start + 0) % 4];
	b = pValue[(Start + 1) % 4];
	c = pValue[(Start + 2) % 4];
	d = pValue[(Start + 3) % 4];

	a += (c ^ (b | ~d)) + k + t;
	a = ((a << s) | ((a & 0xFFFFFFFF) >> (32 - s))) + b;

	pValue [Start] = a;
}


inline void CMd5::Process (UCHAR data[64])
{
	ULONG	pValue[4];
    ULONG	ValueX[16];

	for (int i=0;i<16;i++)
	{
		ValueX[i] =  reinterpret_cast<PULONG>(data)[i];
	}

    pValue[0] = m_State[0];
    pValue[1] = m_State[1];
    pValue[2] = m_State[2];
    pValue[3] = m_State[3];

    Process1 (pValue, 0, ValueX[ 0],  7, 0xD76AA478);
    Process1 (pValue, 3, ValueX[ 1], 12, 0xE8C7B756);
    Process1 (pValue, 2, ValueX[ 2], 17, 0x242070DB);
    Process1 (pValue, 1, ValueX[ 3], 22, 0xC1BDCEEE);
    Process1 (pValue, 0, ValueX[ 4],  7, 0xF57C0FAF);
    Process1 (pValue, 3, ValueX[ 5], 12, 0x4787C62A);
    Process1 (pValue, 2, ValueX[ 6], 17, 0xA8304613);
    Process1 (pValue, 1, ValueX[ 7], 22, 0xFD469501);
    Process1 (pValue, 0, ValueX[ 8],  7, 0x698098D8);
    Process1 (pValue, 3, ValueX[ 9], 12, 0x8B44F7AF);
    Process1 (pValue, 2, ValueX[10], 17, 0xFFFF5BB1);
    Process1 (pValue, 1, ValueX[11], 22, 0x895CD7BE);
    Process1 (pValue, 0, ValueX[12],  7, 0x6B901122);
    Process1 (pValue, 3, ValueX[13], 12, 0xFD987193);
    Process1 (pValue, 2, ValueX[14], 17, 0xA679438E);
    Process1 (pValue, 1, ValueX[15], 22, 0x49B40821);

    Process2 (pValue, 0, ValueX[ 1],  5, 0xF61E2562);
    Process2 (pValue, 3, ValueX[ 6],  9, 0xC040B340);
    Process2 (pValue, 2, ValueX[11], 14, 0x265E5A51);
    Process2 (pValue, 1, ValueX[ 0], 20, 0xE9B6C7AA);
    Process2 (pValue, 0, ValueX[ 5],  5, 0xD62F105D);
    Process2 (pValue, 3, ValueX[10],  9, 0x02441453);
    Process2 (pValue, 2, ValueX[15], 14, 0xD8A1E681);
    Process2 (pValue, 1, ValueX[ 4], 20, 0xE7D3FBC8);
    Process2 (pValue, 0, ValueX[ 9],  5, 0x21E1CDE6);
    Process2 (pValue, 3, ValueX[14],  9, 0xC33707D6);
    Process2 (pValue, 2, ValueX[ 3], 14, 0xF4D50D87);
    Process2 (pValue, 1, ValueX[ 8], 20, 0x455A14ED);
    Process2 (pValue, 0, ValueX[13],  5, 0xA9E3E905);
    Process2 (pValue, 3, ValueX[ 2],  9, 0xFCEFA3F8);
    Process2 (pValue, 2, ValueX[ 7], 14, 0x676F02D9);
	Process2 (pValue, 1, ValueX[12], 20, 0x8D2A4C8A);

    Process3 (pValue, 0, ValueX[ 5],  4, 0xFFFA3942);
    Process3 (pValue, 3, ValueX[ 8], 11, 0x8771F681);
    Process3 (pValue, 2, ValueX[11], 16, 0x6D9D6122);
    Process3 (pValue, 1, ValueX[14], 23, 0xFDE5380C);
    Process3 (pValue, 0, ValueX[ 1],  4, 0xA4BEEA44);
    Process3 (pValue, 3, ValueX[ 4], 11, 0x4BDECFA9);
    Process3 (pValue, 2, ValueX[ 7], 16, 0xF6BB4B60);
    Process3 (pValue, 1, ValueX[10], 23, 0xBEBFBC70);
    Process3 (pValue, 0, ValueX[13],  4, 0x289B7EC6);
    Process3 (pValue, 3, ValueX[ 0], 11, 0xEAA127FA);
    Process3 (pValue, 2, ValueX[ 3], 16, 0xD4EF3085);
    Process3 (pValue, 1, ValueX[ 6], 23, 0x04881D05);
    Process3 (pValue, 0, ValueX[ 9],  4, 0xD9D4D039);
    Process3 (pValue, 3, ValueX[12], 11, 0xE6DB99E5);
    Process3 (pValue, 2, ValueX[15], 16, 0x1FA27CF8);
    Process3 (pValue, 1, ValueX[ 2], 23, 0xC4AC5665);

    Process4 (pValue, 0, ValueX[ 0],  6, 0xF4292244);
    Process4 (pValue, 3, ValueX[ 7], 10, 0x432AFF97);
    Process4 (pValue, 2, ValueX[14], 15, 0xAB9423A7);
    Process4 (pValue, 1, ValueX[ 5], 21, 0xFC93A039);
    Process4 (pValue, 0, ValueX[12],  6, 0x655B59C3);
    Process4 (pValue, 3, ValueX[ 3], 10, 0x8F0CCC92);
    Process4 (pValue, 2, ValueX[10], 15, 0xFFEFF47D);
    Process4 (pValue, 1, ValueX[ 1], 21, 0x85845DD1);
    Process4 (pValue, 0, ValueX[ 8],  6, 0x6FA87E4F);
    Process4 (pValue, 3, ValueX[15], 10, 0xFE2CE6E0);
    Process4 (pValue, 2, ValueX[ 6], 15, 0xA3014314);
    Process4 (pValue, 1, ValueX[13], 21, 0x4E0811A1);
    Process4 (pValue, 0, ValueX[ 4],  6, 0xF7537E82);
    Process4 (pValue, 3, ValueX[11], 10, 0xBD3AF235);
    Process4 (pValue, 2, ValueX[ 2], 15, 0x2AD7D2BB);
    Process4 (pValue, 1, ValueX[ 9], 21, 0xEB86D391);

    m_State[0] += pValue[0];
    m_State[1] += pValue[1];
    m_State[2] += pValue[2];
    m_State[3] += pValue[3];
}
