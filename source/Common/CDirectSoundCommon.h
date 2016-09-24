#pragma once


typedef VOID (CALLBACK* ESOUND_CALLBACK)(PUCHAR pBuffer, ULONG Length, PVOID pParam);

class ESoundDevice
{
public:
	ESoundDevice ()
	{
		::ZeroMemory (&m_Guid, sizeof (m_Guid));
	}
public:
	GUID		m_Guid;
	CString		m_strID;
	CString		m_strName;
};
