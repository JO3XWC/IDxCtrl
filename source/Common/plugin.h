#pragma once

typedef ULONG (CALLBACK* DATA_CALLBACK_FUNC)(ULONG Type, PVOID pBuffer, ULONG Length, PVOID pParam);

extern ULONG *g_Civ_All_Status;
extern ULONG g_nCiv_All_Status;

enum
{
	FLAGS_SERIAL	= 0x00000001,
	FLAGS_NETWORK	= 0x00000002,
};

#pragma pack (push, 1)

typedef struct _PLUGIN_PARAM
{
	INT			ComNo;
	WCHAR		szRemoteAddr[256];
	INT			RemotePort;
	UCHAR		AuthMd5[16];
}PLUGIN_PARAM, *PPLUGIN_PARAM;

typedef struct _PLUGIN
{
	ULONG		Size;
	ULONG		Flags;			//	SERIAL / NETWORK
	HMODULE		hModule;		//	
	LPCWSTR		pszPluginName;	//	
	LPCWSTR		pszDescription;	//	
	LPCWSTR		pszCopyright;	//	

	ULONG		(WINAPI *Open)(PLUGIN_PARAM* pPluginParam, DATA_CALLBACK_FUNC pfnCallback, PVOID pParam);
	ULONG		(WINAPI *Close)();
	ULONG		(WINAPI *Command)(ULONG Type, PVOID pBuffer, ULONG Length);
}PLUGIN, *PPLUGIN;

#pragma pack (pop)

enum _TYPE
{
	TYPE_SERIAL,	//From Serial
	TYPE_VOICE,		//From Mic
	TYPE_VOICE_G711,//From Network
	TYPE_COMMAND,	//To Serial

	TYPE_CACHE_CLEAR,
	TYPE_CACHE_GET,
};

