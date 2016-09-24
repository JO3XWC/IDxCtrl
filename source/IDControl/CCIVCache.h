#pragma once
#include "CTcpCommon.h"

class CCIVCache
{
public:
	CCIVCache();
	~CCIVCache();

	VOID	Clear		();
	VOID	Set			(ULONG Civ, PVOID pBuffer, ULONG Length);
	ULONG	GetCache	(PVOID pBuffer, ULONG Length);
	ULONG	Lookup		(ULONG CIV, PVOID pBuffer, ULONG Length);

private:
	CCriticalSection	m_MapLock;
	CMap<ULONG, ULONG, CMemFileEx*, CMemFileEx*>	m_Map;
};

