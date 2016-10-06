#pragma once


class CRepeater
{
public:
	CRepeater();
	~CRepeater();
public:
	VOID Set (CStringArray* pArray);

	ULONG		GetGroup ()			{	return m_Group;			}
	LPCTSTR		GetGroupName ()		{	return m_strGroupName;	}
	LPCTSTR		GetName ()			{	return m_strName;		}
	LPCTSTR		GetSubName ()		{	return m_strSubName;	}
	LPCTSTR		GetCallsign ()		{	return m_strCallsign;	}
	LPCTSTR		GetToCallsign ()	{	return m_strToCallsign;	}
	LPCTSTR		GetGateway ()		{	return m_strGateway;	}
	ULONGLONG	GetFrequency ()		{	return m_Frequency;		}
	LPCTSTR		GetFrequencyStr ()	{	return m_strFrequency;	}
	UCHAR		GetDup ()			{	return m_Dup;			}
	ULONGLONG	GetOffset ()		{	return m_Offset;		}
	LPCTSTR		GetOffsetStr ()		{	return m_strOffset;		}
	ULONG		GetDvMode ()		{	return m_DvMode;		}
	ULONG		GetToneEnable ()	{	return m_ToneEnable;	}
	ULONG		GetRepeaterTone ()	{	return m_RepeaterTone;	}
	LPCTSTR		GetRepeaterToneStr (){	return m_strRepeaterTone;}
	ULONG		GetRpt1Use ()		{	return m_Rpt1Use;		}

	static CString 	GetToCallsign (CString strCallsign);

private:
//Group No,Group Name,Name,Sub Name,Repeater Call Sign,Gateway Call Sign,Frequency,Dup,Offset,Mode,TONE,Repeater Tone,RPT1USE,Position,Latitude,Longitude,UTC Offset
//1,ä÷ìå,èHótå¥430,ìåãûìs,JP1YLA A,JP1YLA G,434.320000,DUP+,5.000000,DV,OFF,88.5Hz,YES,Approximate,35.697750,139.772150,+9:00

	ULONG		m_Group;
	CString		m_strGroupName;
	CString		m_strName;
	CString		m_strSubName;
	CString		m_strCallsign;
	CString		m_strToCallsign;
	CString		m_strGateway;
	ULONGLONG	m_Frequency;
	CString		m_strFrequency;
	UCHAR		m_Dup;
	ULONGLONG	m_Offset;
	CString		m_strOffset;
	ULONG		m_DvMode;
	ULONG		m_ToneEnable;
	ULONG		m_RepeaterTone;
	CString		m_strRepeaterTone;
	ULONG		m_Rpt1Use;
};

class CRepeaterList
{
public:
	CRepeaterList ();
	~CRepeaterList ();

	VOID	Clear		();
	VOID	Load		(LPCTSTR pszFileName);
	VOID	Save		(LPCTSTR pszFileName);
	VOID	RemoveAt	(CRepeater* pResultRepeater);

	BOOL	Lookup		(LPCTSTR pszCallSign, CRepeater*& pRepeater);
	BOOL	LookupTo	(LPCTSTR pszCallSign, CRepeater*& pRepeater);
public:
	CList<CRepeater*>	m_List;
	CMapStringToPtr		m_Map;
	CMapStringToPtr		m_ToMap;
};
