// Network.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "Network.h"
#include "CTcpClientCmd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
//TODO: この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされたどの関数も
//		関数の最初に追加される AFX_MANAGE_STATE マクロを
//		持たなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな 
//		らないことを意味します、コンストラクターが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

// CNetworkApp

BEGIN_MESSAGE_MAP(CNetworkApp, CWinApp)
END_MESSAGE_MAP()


// CNetworkApp コンストラクション

CNetworkApp::CNetworkApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CNetworkApp オブジェクトです。

CNetworkApp theApp;


// CNetworkApp 初期化

BOOL CNetworkApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}



CTcpClientCmd g_Client;


ULONG WINAPI Serial_Open (PLUGIN_PARAM* pPluginParam, DATA_CALLBACK_FUNC pfnCallback, PVOID pParam)
{
	g_Client.SetAuthMd5 (pPluginParam->AuthMd5);
	g_Client.SetCallback (pfnCallback, pParam);
	return g_Client.Open (pPluginParam->szRemoteAddr, pPluginParam->RemotePort);
}

ULONG WINAPI Serial_Close()
{
	g_Client.SetCallback (NULL, NULL);
	return g_Client.Close ();
}

ULONG WINAPI Serial_Command(ULONG Type, PVOID pBuffer, ULONG Length)
{
	UCHAR Buff[4 * 1024] = {};
	PACKET_CMD_HEADER* pHeader = reinterpret_cast<PACKET_CMD_HEADER*>(Buff);

	pHeader->m_Length	= sizeof (PACKET_CMD_HEADER) + Length;
	pHeader->m_Type		= Type;

	memcpy (pHeader + 1, pBuffer, Length);

	g_Client.Send (pHeader, pHeader->m_Length);

	return ERROR_SUCCESS;
}

PLUGIN g_Plugin = 
{
	sizeof (PLUGIN),
	FLAGS_NETWORK,
	NULL,
	L"Network Control",
	L"Network Control",
	L"(C) 2012-2016 JO3XWC (k.yamada)",	
	Serial_Open,
	Serial_Close,
	Serial_Command
};

// エクスポート関数
ULONG WINAPI IDCtrlInfo (PLUGIN* pPlugin)
{
	ULONG	Result		= ERROR_SUCCESS;

	do
	{
		if (pPlugin == NULL)
		{	
			Result = ERROR_INVALID_PARAMETER;
			break;
		}

		if (pPlugin->Size != sizeof (PLUGIN))
		{
			Result = ERROR_INVALID_PARAMETER;
			break;
		}

		g_Plugin.hModule = theApp.m_hInstance;

		memcpy (pPlugin, &g_Plugin, sizeof (PLUGIN));
	}
	while (0);

	return Result;
}

