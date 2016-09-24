#pragma once

VOID _DebugBuf (PVOID pBuffer, ULONG Length);
CString _DebugTime ();

#ifdef _DEBUG
#define DBG_MSG(_msg_)		{CString _dbgmsg_;_dbgmsg_.Format(_T("%s "), _DebugTime ().GetString ());_dbgmsg_.AppendFormat##_msg_;OutputDebugString (_dbgmsg_);}
#define DBG_BUF(_p_, _l_)	_DebugBuf (_p_, _l_)
#else
#define DBG_MSG(_msg_)
#define DBG_BUF(_p_, _l_)
#endif
