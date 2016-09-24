// ID92.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "ID92.h"
#include "CID92SerialPort.h"

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

// CID92App

BEGIN_MESSAGE_MAP(CID92App, CWinApp)
END_MESSAGE_MAP()


// CID92App コンストラクション

CID92App::CID92App()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CID92App オブジェクトです。

CID92App theApp;


// CID92App 初期化

BOOL CID92App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}










CID92SerialPort g_Serial;

ULONG WINAPI Serial_Open (PLUGIN_PARAM* pPluginParam, DATA_CALLBACK_FUNC pfnCallback, PVOID pParam)
{
	g_Serial.SetCallback (pfnCallback, pParam);
	return g_Serial.Open (pPluginParam->ComNo);
}

ULONG WINAPI Serial_Close()
{
	g_Serial.SetCallback (NULL, NULL);
	return g_Serial.Close ();
}

ULONG WINAPI Serial_Command(ULONG Type, PVOID pBuffer, ULONG Length)
{
	return g_Serial.AddCommand (Type, pBuffer, Length);
}

PLUGIN g_Plugin = 
{
	sizeof (PLUGIN),
	FLAGS_SERIAL,
	NULL,
	L"ID-92",
	L"ID-92 Control",
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

