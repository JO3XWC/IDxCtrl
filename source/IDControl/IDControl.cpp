
// IDControl.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IDControl.h"
#include "MainFrm.h"

#include "IDControlDoc.h"
#include "IDControlView.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CIDControlApp

BEGIN_MESSAGE_MAP(CIDControlApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIDControlApp::OnAppAbout)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CIDControlApp コンストラクション

CIDControlApp::CIDControlApp()
{
	m_bHiColorIcons = TRUE;

	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// アプリケーションが共通言語ランタイム サポート (/clr) を使用して作成されている場合:
	//     1) この追加設定は、再起動マネージャー サポートが正常に機能するために必要です。
	//     2) 作成するには、プロジェクトに System.Windows.Forms への参照を追加する必要があります。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 下のアプリケーション ID 文字列を一意の ID 文字列で置換します。推奨される
	// 文字列の形式は CompanyName.ProductName.SubProduct.VersionInformation です
	CString strAppID;
	
	strAppID.Format (_T("JO3XWC.IDControl.%s.NoVersion"), GetMyDir ().GetString ());
	SetAppID(strAppID);

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

// 唯一の CIDControlApp オブジェクトです。

CIDControlApp theApp;


// CIDControlApp 初期化

BOOL CIDControlApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// OLE ライブラリを初期化します。
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit コントロールを使用するには AfxInitRichEdit2() が必要です	
	// AfxInitRichEdit2();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	CString strRegKey;
	
	strRegKey.Format (_T("JO3XWC\\IDCtrl\\%s"), GetMyDir ().GetString ());
	SetRegistryKey(strRegKey);
	LoadStdProfileSettings(4);  // 標準の INI ファイルのオプションをロードします (MRU を含む)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIDControlDoc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CIDControlView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE、file open など標準のシェル コマンドのコマンド ラインを解析します。
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// DDE Execute open を使用可能にします。
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);

	LoadSetting ();
	GetRepeaterList ()->Load (GetMyDir () + _T("RptList.csv"));

	// コマンド ラインで指定されたディスパッチ コマンドです。アプリケーションが
	// /RegServer、/Register、/Unregserver または /Unregister で起動された場合、False を返します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出します
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	// ドラッグ/ドロップ オープンを許可します。
	//m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CIDControlApp::ExitInstance()
{
	SaveSetting ();

	//TODO: 追加したリソースがある場合にはそれらも処理してください
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}


#define SECTION_PLUGIN			_T("Plugin")
#define PLUGIN_PATH				_T("Path")
#define PLUGIN_FLAGS			_T("Flags")
#define PLUGIN_COM				_T("ComNo")


#define SECTION_NETWORK			_T("Network")
#define NETWORK_SERVER_PORT		_T("ServerPort")
#define NETWORK_SERVER_ENABLE	_T("ServerEnable")
#define NETWORK_REMOTE_ADDR		_T("RemoteAddr")
#define NETWORK_REMOTE_PORT		_T("RemotePort")
#define NETWORK_USER_NAME		_T("UserName")
#define NETWORK_USER_PASS		_T("UserPass")

#define SECTION_MIC				_T("Mic")
#define MIC_ENABLE				_T("Enable")
#define MIC_PATH				_T("Path")
#define MIC_DEVICE				_T("Device")

#define SECTION_SPEAKER			_T("Speaker")
#define SPEAKER_ENABLE			_T("Enable")
#define SPEAKER_PATH			_T("Path")
#define SPEAKER_DEVICE			_T("Device")

#define SECTION_CONFIG			_T("Config")
#define CONFIG_SPEECH_LEVEL		_T("SPEECH_LEVEL")

VOID CIDControlApp::SaveSetting ()
{
	WriteProfileString (SECTION_PLUGIN, PLUGIN_PATH				, m_Setting.m_Trx.m_strPluginPath);
	WriteProfileInt    (SECTION_PLUGIN, PLUGIN_COM				, m_Setting.m_Trx.m_ComNo);

	WriteProfileInt    (SECTION_NETWORK, NETWORK_SERVER_ENABLE	, m_Setting.m_Network.m_ServerEnable);
	WriteProfileString (SECTION_NETWORK, NETWORK_USER_NAME		, m_Setting.m_Network.m_strUserName);
	WriteProfileString (SECTION_NETWORK, NETWORK_USER_PASS		, m_Setting.m_Network.m_strUserPass);
	WriteProfileInt    (SECTION_NETWORK, NETWORK_SERVER_PORT	, m_Setting.m_Network.m_ServerPort);
	WriteProfileString (SECTION_NETWORK, NETWORK_REMOTE_ADDR	, m_Setting.m_Network.m_strRemoteAddr);
	WriteProfileInt    (SECTION_NETWORK, NETWORK_REMOTE_PORT	, m_Setting.m_Network.m_RemotePort);

	WriteProfileInt    (SECTION_MIC, MIC_ENABLE					, m_Setting.m_Mic.m_Enable);
	WriteProfileString (SECTION_MIC, MIC_DEVICE					, m_Setting.m_Mic.m_strID);

	WriteProfileInt    (SECTION_SPEAKER, SPEAKER_ENABLE			, m_Setting.m_Speaker.m_Enable);
	WriteProfileString (SECTION_SPEAKER, SPEAKER_DEVICE			, m_Setting.m_Speaker.m_strID);

	WriteProfileInt    (SECTION_CONFIG, CONFIG_SPEECH_LEVEL		, m_Setting.m_SpeechLevel);
}

VOID CIDControlApp::LoadSetting ()
{
	CString strText;

	m_Setting.m_Trx.m_ComNo				= GetProfileInt    (SECTION_PLUGIN, PLUGIN_COM				, 1); 
	m_Setting.m_Trx.m_strPluginPath		= GetProfileString (SECTION_PLUGIN, PLUGIN_PATH				, _T(""));

	m_Setting.m_Network.m_ServerEnable	= GetProfileInt    (SECTION_NETWORK, NETWORK_SERVER_ENABLE	, 0);
	m_Setting.m_Network.m_strUserName	= GetProfileString (SECTION_NETWORK, NETWORK_USER_NAME		, _T(""));
	m_Setting.m_Network.m_strUserPass	= GetProfileString (SECTION_NETWORK, NETWORK_USER_PASS		, _T(""));
	m_Setting.m_Network.m_ServerPort	= GetProfileInt    (SECTION_NETWORK, NETWORK_SERVER_PORT	, 20000);
	m_Setting.m_Network.m_strRemoteAddr	= GetProfileString (SECTION_NETWORK, NETWORK_REMOTE_ADDR	, _T(""));
	m_Setting.m_Network.m_RemotePort	= GetProfileInt    (SECTION_NETWORK, NETWORK_REMOTE_PORT	, 20000);
	
	m_Setting.m_Mic.m_Enable			= GetProfileInt    (SECTION_MIC, MIC_ENABLE					, 0);
	m_Setting.m_Mic.m_strID				= GetProfileString (SECTION_MIC, MIC_DEVICE					, _T(""));
	
	m_Setting.m_Speaker.m_Enable		= GetProfileInt    (SECTION_SPEAKER, SPEAKER_ENABLE			, 0);
	m_Setting.m_Speaker.m_strID			= GetProfileString (SECTION_SPEAKER, SPEAKER_DEVICE			, _T(""));

	m_Setting.m_SpeechLevel				= GetProfileInt    (SECTION_CONFIG, CONFIG_SPEECH_LEVEL		, 20);

}



// CIDControlApp メッセージ ハンドラー


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CIDControlApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIDControlApp のカスタマイズされた読み込みメソッドと保存メソッド

void CIDControlApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_REPEATER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_REPEATER);
}

void CIDControlApp::LoadCustomState()
{
}

void CIDControlApp::SaveCustomState()
{
}

// CIDControlApp メッセージ ハンドラー



