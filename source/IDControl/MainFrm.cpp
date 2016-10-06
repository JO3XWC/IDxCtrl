
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include <strsafe.h>
#include "IDControl.h"

#include "MainFrm.h"
#include "CDlgTrxSetting.h"
#include "CDlgSoundSetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()
	ON_UPDATE_COMMAND_UI(ID_CMD_TRX_SETTING, &CMainFrame::OnUpdateCmdTrxSetting)
	ON_COMMAND(ID_CMD_TRX_SETTING, &CMainFrame::OnCmdTrxSetting)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_REGISTERED_MESSAGE(WM_SET_CALLSIGN, &CMainFrame::OnSetCallsign)
	ON_REGISTERED_MESSAGE(WM_COMMON_VIEW, &CMainFrame::OnCommonView)
	ON_REGISTERED_MESSAGE(WM_ANALOG_FREQ_VIEW, &CMainFrame::OnAnalogFreqView)
	ON_REGISTERED_MESSAGE(WM_DIGITAL_FREQ_VIEW, &CMainFrame::OnDigitalFreqView)
	ON_COMMAND(ID_CMD_IMPORT_REPEATER_LIST, &CMainFrame::OnCmdImportRepeaterList)
	ON_UPDATE_COMMAND_UI(ID_CMD_SOUND_SETTING, &CMainFrame::OnUpdateCmdSoundSetting)
	ON_COMMAND(ID_CMD_SOUND_SETTING, &CMainFrame::OnCmdSoundSetting)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケーター
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame()
{
	m_hWorkerThread		= NULL;
	m_hStopEvent		= ::CreateEvent (NULL, TRUE, FALSE, NULL);
	m_hWorkEvent		= ::CreateEvent (NULL, FALSE, FALSE, NULL);
	m_VoiceEnable		= 0;
	m_MyCallsignIndex	= -1;

	::ZeroMemory (&m_Plugin, sizeof (m_Plugin));
}

CMainFrame::~CMainFrame()
{
	if (m_hStopEvent != NULL)
	{
		::CloseHandle (m_hStopEvent);
	}

	if (m_hWorkEvent != NULL)
	{
		::CloseHandle (m_hWorkEvent);
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("メニュー バーを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// アクティブになったときメニュー バーにフォーカスを移動しない
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ツール バーおよびメニュー バーをドッキング可能にしない場合は、この 5 つの行を削除します
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);


	// Visual Studio 2005 スタイルのドッキング ウィンドウ動作を有効にします
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 スタイルのドッキング ウィンドウの自動非表示動作を有効にします
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// メニュー項目イメージ (どの標準ツール バーにもないイメージ) を読み込みます:
	//CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// ドッキング ウィンドウを作成します
	if (!CreateDockingWindows())
	{
		TRACE0("ドッキング ウィンドウを作成できませんでした\n");
		return -1;
	}

	m_wndRepeaterView.EnableDocking(CBRS_ALIGN_ANY);
#ifdef _DEBUG
	m_wndDebugView.EnableDocking(CBRS_ALIGN_ANY);
#endif
	m_wndCallsignHistoryView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMessageHistoryView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndGpsHistoryView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCommonView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndAnalogFreqView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDigitalFreqView.EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndRepeaterView, AFX_IDW_DOCKBAR_LEFT);
	DockPane(&m_wndCommonView, AFX_IDW_DOCKBAR_TOP);
	//m_wndAnalogFreqView.DockToWindow(&m_wndCommonView, CBRS_ALIGN_BOTTOM);
	DockPane(&m_wndAnalogFreqView, AFX_IDW_DOCKBAR_TOP);
	//DockPane(&m_wndDigitalFreqView, AFX_IDW_DOCKBAR_TOP);
	m_wndDigitalFreqView.DockToWindow(&m_wndAnalogFreqView, CBRS_ALIGN_RIGHT);
	DockPane(&m_wndCallsignHistoryView, AFX_IDW_DOCKBAR_BOTTOM);

	CDockablePane* pTabbedBar = NULL;
	m_wndMessageHistoryView.AttachToTabWnd(&m_wndCallsignHistoryView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndGpsHistoryView.AttachToTabWnd(&m_wndCallsignHistoryView, DM_SHOW, TRUE, &pTabbedBar);
#ifdef _DEBUG
	m_wndDebugView.AttachToTabWnd(&m_wndCallsignHistoryView, DM_SHOW, TRUE, &pTabbedBar);
#endif
	


	// すべてのユーザー インターフェイス要素の描画に使用される、ビジュアル マネージャーを設定します
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// ツール バーとドッキング ウィンドウ メニューの配置変更を有効にします
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ツール バーのクイック (Alt キーを押しながらドラッグ) カスタマイズを有効にします
	CMFCToolBar::EnableQuickCustomization();

	//
	m_WaveSpeech.Load (AfxGetResourceHandle (), '/', IDR_WAVE_SLASH, 1);
	m_WaveSpeech.Load (AfxGetResourceHandle (), ' ', IDR_WAVE_SPACE, 1);
	m_WaveSpeech.Load (AfxGetResourceHandle (), '0', IDR_WAVE_0, 10);
	m_WaveSpeech.Load (AfxGetResourceHandle (), 'A', IDR_WAVE_A, ('Z' - 'A') + 1);
	m_WaveSpeech.Load (AfxGetResourceHandle (), 'a', IDR_WAVE_A, ('Z' - 'A') + 1);
	m_WaveSpeech.SetVolume (theApp.m_Setting.m_SpeechLevel);
	m_wndCommonView.SetSpeeachLevel (theApp.m_Setting.m_SpeechLevel);
	//CStringA strLebelA;
	//strLebelA.Format ("%u", theApp.m_Setting.m_SpeechLevel);
	//m_WaveSpeech.Play (strLebelA);


	m_wndDigitalFreqView.OnRepeaterListChange ();
	m_wndDigitalFreqView.UpdateRxCs ();



	CWinThread* pThread = AfxBeginThread (WorkerThread, this, 0, CREATE_SUSPENDED);
	
	if (pThread != NULL)
	{
		if (!::DuplicateHandle (::GetCurrentProcess (), pThread->m_hThread, ::GetCurrentProcess (), &m_hWorkerThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			ASSERT (FALSE);
		}

		pThread->ResumeThread ();
	}

	StartIDCtrl ();

	return 0;
}

void CMainFrame::OnClose()
{
	::SetEvent (m_hStopEvent);


	if (m_hWorkerThread != NULL)
	{
		::WaitForSingleObject (m_hWorkerThread, INFINITE);
		m_hWorkerThread = NULL;
	}

	StopIDCtrl ();

	CFrameWndEx::OnClose();
}

void CMainFrame::OnDestroy()
{
	CFrameWndEx::OnDestroy();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	
	cs.style &= ~FWS_ADDTOTITLE;	//ドキュメントの名称を表示しない(SetTitle()は使用できなくなる)

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	CString strView;

#ifdef _DEBUG
	// デバッグビューを作成します
	bNameValid = strView.LoadString(IDS_DEBUG_VIEW);
	ASSERT(bNameValid);
	if (!m_wndDebugView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DEBUGVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("デバッグビューを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}
#endif

	// レピーター ビューを作成します
	bNameValid = strView.LoadString(IDS_REPEATER_VIEW);
	ASSERT(bNameValid);
	if (!m_wndRepeaterView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPEATERVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("レピーター  ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// コールサイン履歴 ビューを作成します
	bNameValid = strView.LoadString(IDS_CALLSIGN_HISTORY_VIEW);
	ASSERT(bNameValid);
	if (!m_wndCallsignHistoryView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CALLSIGN_HISTORYVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("コールサイン履歴 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// 共通項目 ビューを作成します
	bNameValid = strView.LoadString(IDS_COMMON_VIEW);
	ASSERT(bNameValid);
	if (!m_wndCommonView.Create(strView, this, CRect(0, 0, 200, 60), TRUE, ID_VIEW_COMMONVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("共通項目 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// アナログ周波数 ビューを作成します
	bNameValid = strView.LoadString(IDS_ANALOG_FREQ_VIEW);
	ASSERT(bNameValid);
	if (!m_wndAnalogFreqView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_ANALOG_FREQ_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("アナログ周波数 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// デジタル周波数 ビューを作成します
	bNameValid = strView.LoadString(IDS_DIGITAL_FREQ_VIEW);
	ASSERT(bNameValid);
	if (!m_wndDigitalFreqView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DIGITAL_FREQ_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("デジタル周波数 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// メッセージ履歴 ビューを作成します
	bNameValid = strView.LoadString(IDS_MESSAGE_HISTORY_VIEW);
	ASSERT(bNameValid);
	if (!m_wndMessageHistoryView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_MESSAGE_HISTORYVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("メッセージ履歴 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	// GPS履歴 ビューを作成します
	bNameValid = strView.LoadString(IDS_GPS_HISTORY_VIEW);
	ASSERT(bNameValid);
	if (!m_wndGpsHistoryView.Create(strView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_GPS_HISTORYVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("GPS履歴 ビュー ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}


	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{

}

// CMainFrame 診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラー

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* メニューをスキャンします*/);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

LRESULT CMainFrame::OnCommonView(WPARAM wp,LPARAM lp)
{
	UCHAR				pBuffer[1024];
	PACKET_CMD_HEADER*	pCmdHdr			= reinterpret_cast<PACKET_CMD_HEADER*>(pBuffer);
	PUCHAR				pData			= reinterpret_cast<PUCHAR>(pCmdHdr->GetData ());
	PUSHORT				pData2			= reinterpret_cast<PUSHORT>(pCmdHdr->GetData ());
	PUCHAR				pLp				= reinterpret_cast<PUCHAR>(&lp);
	switch (wp)
	{
	case CCommonScrollView::IDC_BTN_MAIN_POWER:
		{
			//SET POWER
			pCmdHdr->m_Type = CIV_MAIN_POWER;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CCommonScrollView::IDC_BTN_MONITOR:
		{
			//SET MONITOR
			pCmdHdr->m_Type = CIV_MONITOR_MODE;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CCommonScrollView::IDC_BTN_DUAL:
		{
			//SET DUAL
			pCmdHdr->m_Type = CIV_DUAL_WATCH_MODE;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CCommonScrollView::IDC_BTN_ATT:
		{
			//SET ATT
			pCmdHdr->m_Type = CIV_ATTENUATOR;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CCommonScrollView::IDC_CMB_RF_POWER:
		{
			//SET RF POWER
			pCmdHdr->m_Type = CIV_RF_POWER2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
			pData[0] = pLp[1];
			pData[1] = pLp[0];
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CCommonScrollView::IDC_CMB_SPEEACH_LEVEL:
		{
			//SET SPEEACH LEVEL
			m_WaveSpeech.SetVolume (static_cast<ULONG>(lp));
			
			CStringA strLebelA;
			strLebelA.Format ("%u", m_WaveSpeech.GetVolume ());
			m_WaveSpeech.Play (strLebelA);
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}

	return 0;
}

LRESULT CMainFrame::OnAnalogFreqView(WPARAM wp, LPARAM lp)
{
	UCHAR				pBuffer[1024];
	PACKET_CMD_HEADER*	pCmdHdr			= reinterpret_cast<PACKET_CMD_HEADER*>(pBuffer);
	PUCHAR				pData			= reinterpret_cast<PUCHAR>(pCmdHdr->GetData ());
	PUSHORT				pData2			= reinterpret_cast<PUSHORT>(pCmdHdr->GetData ());
	PUCHAR				pLp				= reinterpret_cast<PUCHAR>(&lp);
	switch (wp)
	{
	case CAnalogFreqScrollView::IDC_BTN_BAND:
		{
			//SET BAND A
			pCmdHdr->m_Type = CIV_MAIN_BAND_SELECTION;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CAnalogFreqScrollView::IDC_CMB_SQL:
		{
			//SET SQL
			pCmdHdr->m_Type = CIV_SQL_LEVEL1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CAnalogFreqScrollView::IDC_CMB_VOL:
		{
			//SET VOL
			pCmdHdr->m_Type = CIV_AF_VOLUME1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
			pData[0] = pLp[1];
			pData[1] = pLp[0];
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CAnalogFreqScrollView::IDC_CMB_VFO:
		{
			//SET VFO/MR/CALL
			pCmdHdr->m_Type = CIV_VFO_MR_CALL_SELECTION1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CAnalogFreqScrollView::IDC_CMB_DUP:
		{
			//SET DUPLEX
			pCmdHdr->m_Type = CIV_DUPLEX_MODE1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;
	case CAnalogFreqScrollView::IDC_CMB_MODE:
		{
			//SET MODE
			pCmdHdr->m_Type = CIV_WRITE_OPERATING_MODE1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
			pData[0] = pLp[0];
			pData[1] = pLp[1];
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CAnalogFreqScrollView::IDC_FREQ:
		{
			//SET FREQ
			pCmdHdr->m_Type = CIV_WRITE_FREQUENCY1;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 5;
			ULONGLONG2SWAPBCD (*reinterpret_cast<ULONGLONG*>(lp), pData, 5);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;


	default:
		{
			ASSERT (FALSE);
		}
		break;
	}

	return 0;
}

LRESULT CMainFrame::OnDigitalFreqView(WPARAM wp, LPARAM lp)
{
	UCHAR				pBuffer[1024];
	PACKET_CMD_HEADER*	pCmdHdr			= reinterpret_cast<PACKET_CMD_HEADER*>(pBuffer);
	PUCHAR				pData			= reinterpret_cast<PUCHAR>(pCmdHdr->GetData ());
	PUSHORT				pData2			= reinterpret_cast<PUSHORT>(pCmdHdr->GetData ());
	PUCHAR				pLp				= reinterpret_cast<PUCHAR>(&lp);
	CString				strTemp;

	switch (wp)
	{
	case CDigitalFreqScrollView::IDC_BTN_BAND:
		{
			//SET BAND A
			pCmdHdr->m_Type = CIV_MAIN_BAND_SELECTION;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CMB_SQL:
		{
			//SET SQL
			pCmdHdr->m_Type = CIV_SQL_LEVEL2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CMB_VOL:
		{
			//SET VOL
			pCmdHdr->m_Type = CIV_AF_VOLUME2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
			pData[0] = pLp[1];
			pData[1] = pLp[0];
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CMB_VFO:
		{
			//SET VFO/MR/CALL
			pCmdHdr->m_Type = CIV_VFO_MR_CALL_SELECTION2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CMB_DUP:
		{
			//SET DUPLEX
			pCmdHdr->m_Type = CIV_DUPLEX_MODE2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;
	case CDigitalFreqScrollView::IDC_CMB_MODE:
		{
			//SET MODE
			pCmdHdr->m_Type = CIV_WRITE_OPERATING_MODE2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
			pData[0] = pLp[0];
			pData[1] = pLp[1];
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_FREQ:
		{
			//SET FREQ
			pCmdHdr->m_Type = CIV_WRITE_FREQUENCY2;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 5;
			ULONGLONG2SWAPBCD (*reinterpret_cast<ULONGLONG*>(lp), pData, 5);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;



		//MENU---------------------------------------------------

	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU:
		{
			//SET TX MSG ON/OFF
			pCmdHdr->m_Type = CIV_DV_TX_MESSAGE_ENABLE;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG1:
	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG2:
	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG3:
	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG4:
	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG5:
		{
			//SET TX MSG 1-5
			pCmdHdr->m_Type = CIV_DV_TX_MESSAGE1 + (static_cast<ULONG>(wp) - CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MENU_TXMSG1);
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 20;
			STR2DATA (reinterpret_cast<LPCTSTR>(lp), pData, 20);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CS_CMB_TX_MSG_MEMCH:
		{
			//SET TX MSG MEMCH
			pCmdHdr->m_Type = CIV_DV_TX_MESSAGE_MEMCH;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL1:
	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL2:
	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL3:
	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL4:
	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL5:
	case CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL6:
		{
			//SET MY CALLSIGN MEMCH
			pCmdHdr->m_Type = CIV_DV_MY_CALLSIGN1 + (static_cast<ULONG>(wp) - CDigitalFreqScrollView::IDC_CS_CMB_MY_MENU_CALL1);
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 12;
			STR2DATA (reinterpret_cast<LPCTSTR>(lp), pData, 12);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;

	case CDigitalFreqScrollView::IDC_CS_CMB_MY_CALLSIGN_MEMCH:
		{
			//SET MY CALLSIGN MEMCH
			pCmdHdr->m_Type = CIV_DV_MY_CALLSIGN_MEMCH;
			pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
			pData[0] = static_cast<UCHAR>(lp);
			AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
		}
		break;




	default:
		{
			ASSERT (FALSE);
		}
		break;
	}

	return 0;
}

LRESULT CMainFrame::OnSetCallsign(WPARAM wp,LPARAM lp)
{
	CRepeater*			pRepeater		= NULL;
	UCHAR				TxCache[1024];
	PACKET_CMD_HEADER*	pCmdHdr			= reinterpret_cast<PACKET_CMD_HEADER*>(TxCache);
	PUCHAR				pData			= reinterpret_cast<PUCHAR>(pCmdHdr->GetData ());
	PUSHORT				pData2			= reinterpret_cast<PUSHORT>(pCmdHdr->GetData ());
	ULONG				TxCacheLen;
	CRepeaterList*		pRepeaterList	= theApp.GetRepeaterList ();
	CString				strRpt2;
	CString				strRpt1;
	CString				strUr;
	CString				strCallsign;

	do
	{
		TxCacheLen = m_CivCache.Lookup (CIV_DV_TX_CALLSIGN, TxCache, sizeof (TxCache));
	
		ASSERT (TxCacheLen > 0);
		if (TxCacheLen <= 0)
		{	break;
		}

		strRpt2		= DATA2STR (TxCache + 6, 8);
		strRpt1		= DATA2STR (TxCache + 14, 8);
		strUr		= DATA2STR (TxCache + 22, 8);

		switch (wp)
		{
		case CMainFrame::SET_TOFROM_CALLSIGN:
			{
				pRepeater = reinterpret_cast<CRepeater*>(lp);
				if (pRepeater == NULL)
				{	break;
				}

				//SET VFO
				pCmdHdr->m_Type = CIV_VFO_MR_CALL_SELECTION2;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
				pData[0] = SELECT_VFO;
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
				
				//SET TX CALLSIGN
				if (strUr == _T("CQCQCQ  "))
				{	strCallsign.Format(_T("%- 8s"), pRepeater->GetCallsign ());
				}
				else
				{	strCallsign.Format(_T("%- 8s"), pRepeater->GetGateway ());
				}
				strCallsign = strCallsign.Left (8);

				strCallsign.AppendFormat(_T("%- 8s"), pRepeater->GetCallsign ());
				strCallsign = strCallsign.Left (16);

				strCallsign.AppendFormat(_T("%- 8s"), strUr.GetString ());
				strCallsign = strCallsign.Left (24);

				pCmdHdr->m_Type = CIV_DV_TX_CALLSIGN;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 24;
				STR2DATA (strCallsign, pData, 24);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
				
				//SET FREQ
				pCmdHdr->m_Type = CIV_WRITE_FREQUENCY2;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 5;
				ULONGLONG2SWAPBCD (pRepeater->GetFrequency (), pData, 5);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
				
				//SET MODE(DV/FM)
				pCmdHdr->m_Type = CIV_WRITE_OPERATING_MODE2;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 2;
				*pData2 = pRepeater->GetDvMode () ? OPERATING_MODE_DV : OPERATING_MODE_FM;
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);

				//SET DUP
				pCmdHdr->m_Type = CIV_DUPLEX_MODE2;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 1;
				*pData = pRepeater->GetDup ();
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);

				//SET OFFSET
				pCmdHdr->m_Type = CIV_WRITE_DUPLEX_OFFSET2;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 5;
				ULONGLONG2SWAPBCD (pRepeater->GetOffset (), pData, 5);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
			}
			break;

		case CMainFrame::SET_UR_CALLSIGN:
			{
				strUr	= reinterpret_cast<LPCTSTR>(lp);

				if (pRepeaterList->Lookup (strUr, pRepeater))
				{
					strUr = pRepeater->GetToCallsign ();
				}

				if (pRepeaterList->Lookup (strRpt1, pRepeater))
				{
					if ((strUr == _T("CQCQCQ  ")) || (strUr == _T("        ")))
					{	strRpt2 = pRepeater->GetCallsign ();
					}
					else
					{	strRpt2 = pRepeater->GetGateway ();
					}
				}

				strCallsign.Format (_T("%- 8s"), strRpt2.GetString ());
				strCallsign = strCallsign.Left (8);

				strCallsign.AppendFormat(_T("%- 8s"), strRpt1.GetString ());
				strCallsign = strCallsign.Left (16);

				strCallsign.AppendFormat (_T("%- 8s"), strUr.GetString ());
				strCallsign = strCallsign.Left (24);

				pCmdHdr->m_Type = CIV_DV_TX_CALLSIGN;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 24;
				STR2DATA (strCallsign, pData, 24);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
			}
			break;

		case CMainFrame::SET_R1_CALLSIGN:
			{
				strRpt1	= reinterpret_cast<LPCTSTR>(lp);

				strCallsign.Format (_T("%- 8s"), strRpt2.GetString ());
				strCallsign = strCallsign.Left (8);

				strCallsign.AppendFormat(_T("%- 8s"), strRpt1.GetString ());
				strCallsign = strCallsign.Left (16);

				strCallsign.AppendFormat (_T("%- 8s"), strUr.GetString ());
				strCallsign = strCallsign.Left (24);

				pCmdHdr->m_Type = CIV_DV_TX_CALLSIGN;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 24;
				STR2DATA (strCallsign, pData, 24);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
			}
			break;

		case CMainFrame::SET_R2_CALLSIGN:
			{
				strRpt2	= reinterpret_cast<LPCTSTR>(lp);

				strCallsign.Format (_T("%- 8s"), strRpt2.GetString ());
				strCallsign = strCallsign.Left (8);

				strCallsign.AppendFormat(_T("%- 8s"), strRpt1.GetString ());
				strCallsign = strCallsign.Left (16);

				strCallsign.AppendFormat (_T("%- 8s"), strUr.GetString ());
				strCallsign = strCallsign.Left (24);

				pCmdHdr->m_Type = CIV_DV_TX_CALLSIGN;
				pCmdHdr->m_Length = sizeof (PACKET_CMD_HEADER) + 24;
				STR2DATA (strCallsign, pData, 24);
				AddWork (TYPE_COMMAND, pCmdHdr, pCmdHdr->m_Length);
			}
			break;

		default:
			{
				ASSERT (FALSE);
			}
			break;
		}
	}
	while (0);

	return 0;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//m_wndOutput.UpdateFonts();
}


void CMainFrame::OnUpdateCmdTrxSetting(CCmdUI *pCmdUI)
{
	pCmdUI->Enable ();
}


void CMainFrame::OnCmdTrxSetting()
{
	CDlgTrxSetting Dlg;

	if (Dlg.DoModal () == IDOK)
	{
		theApp.SaveSetting ();
		StartIDCtrl ();
	}
}

void CMainFrame::OnUpdateCmdSoundSetting(CCmdUI *pCmdUI)
{
	pCmdUI->Enable ();
}


void CMainFrame::OnCmdSoundSetting()
{
	CDlgSoundSetting Dlg;

	if (Dlg.DoModal () == IDOK)
	{
		theApp.SaveSetting ();
		StartIDCtrl ();
	}
}



VOID CMainFrame::StartIDCtrl ()
{
	ULONG (WINAPI* IDCtrlInfo) (PLUGIN* pPlugin);
	CMd5			Md5;
	CString			strUserPass;
	CStringA		strUserPassA;
	HMODULE			hModule;
	PLUGIN_PARAM	PluginParam = {};

	StopIDCtrl ();


	if (theApp.m_Setting.m_Speaker.m_Enable)
	{
		m_DirectSoundRender.StartRender (m_hWnd, theApp.m_Setting.m_Speaker.m_strID);
	}

	do
	{
		hModule = ::LoadLibrary (theApp.m_Setting.m_Trx.m_strPluginPath);
		if (hModule == NULL)
		{	break;
		}
		
		*((FARPROC*)&IDCtrlInfo) = ::GetProcAddress (hModule, "IDCtrlInfo");
		if (IDCtrlInfo == NULL)
		{
			StopIDCtrl ();
			break;
		}

		m_Plugin.Size = sizeof (m_Plugin);
		IDCtrlInfo (&m_Plugin);

		if ((m_Plugin.Open == NULL) || (m_Plugin.Close == NULL) || (m_Plugin.Command == NULL))
		{
			StopIDCtrl ();
			break;
		}

		strUserPass.Format (_T("%s:%s"), theApp.m_Setting.m_Network.m_strUserName.GetString (), theApp.m_Setting.m_Network.m_strUserPass.GetString ());
		strUserPassA = strUserPass;

		Md5.Starts ();
		Md5.Update (strUserPassA.GetString (), strUserPassA.GetLength ());
		Md5.Finish (PluginParam.AuthMd5, sizeof (PluginParam.AuthMd5));
		
		PluginParam.ComNo		= theApp.m_Setting.m_Trx.m_ComNo;
		PluginParam.RemotePort	= theApp.m_Setting.m_Network.m_RemotePort;
		StringCchCopy (PluginParam.szRemoteAddr, _countof (PluginParam.szRemoteAddr), theApp.m_Setting.m_Network.m_strRemoteAddr);

		if (m_Plugin.Flags & FLAGS_SERIAL)
		{
			if (theApp.m_Setting.m_Mic.m_Enable)
			{
				m_DirectSoundCapture.SetCallBack (onWaveIn, this);
				m_DirectSoundCapture.StartCapture (m_hWnd, theApp.m_Setting.m_Mic.m_strID);
			}

			m_Plugin.Open (&PluginParam, onDataCallback, this);

			if (theApp.m_Setting.m_Network.m_ServerEnable)
			{
				m_TcpServer.SetCallback (onDataCallback, this);
				m_TcpServer.SetAuthMd5 (PluginParam.AuthMd5);
				m_TcpServer.Open (theApp.m_Setting.m_Network.m_ServerPort);
			}
		}
		else if (m_Plugin.Flags & FLAGS_NETWORK)
		{
			m_Plugin.Open (&PluginParam, onDataCallback, this);
		}
	}
	while (0);
}

VOID CMainFrame::StopIDCtrl ()
{
	m_VoiceEnable		= 0;
	m_MyCallsignIndex	= -1;

	m_MyCallsignArray.RemoveAll ();
	m_MyMemoArray.RemoveAll ();

	m_TcpServer.Close ();

	m_DirectSoundCapture.Stop ();
	m_DirectSoundRender.Stop ();

	do
	{
		if (m_Plugin.hModule == NULL)
		{	break;
		}

		ASSERT (m_Plugin.Close != NULL);
		if (m_Plugin.Close != NULL)
		{	m_Plugin.Close ();
		}

		::FreeLibrary (m_Plugin.hModule);
	}
	while (0);

	::ZeroMemory (&m_Plugin, sizeof (m_Plugin));

	m_WorkList.RemoveAll ();
	m_RecvList.RemoveAll ();

	//Power OFF
	UCHAR p[] = { 0xFE, 0xFE, 0xE0, 0x00, 0x18, 0x00, 0xFD};
	OnWorkSerial (p, sizeof (p));
}
	
VOID CMainFrame::onWaveIn (PUCHAR pBuffer, ULONG Length, PVOID pParam)
{
	CMainFrame* pThis = reinterpret_cast<CMainFrame*>(pParam);

	do
	{
		if (!pThis->m_VoiceEnable)
		{	break;
		}

		pThis->AddWork (TYPE_VOICE, pBuffer, Length);
	}
	while (0);
}

ULONG CMainFrame::onDataCallback (ULONG Type, PVOID pBuffer, ULONG Length, PVOID pParam)
{
	return reinterpret_cast<CMainFrame*>(pParam)->OnDataCallback (Type, pBuffer, Length);
}

ULONG CMainFrame::OnDataCallback (ULONG Type, PVOID pBuffer, ULONG Length)
{
	ULONG	Result = ERROR_SUCCESS;
	switch (Type)
	{
	case TYPE_CACHE_GET:
		{
			Result = m_CivCache.GetCache (pBuffer, Length);
		}
		break;

	case TYPE_CACHE_CLEAR:
		{
			m_CivCache.Clear ();

			m_MyCallsignArray.RemoveAll ();
			m_MyMemoArray.RemoveAll ();
			m_MyCallsignIndex = -1;
		}
		break;

	case TYPE_SERIAL:
		{
			if (pBuffer == NULL)
			{
				//シリアル通信が途切れたのでキャッシュをクリア
				m_CivCache.Clear ();

				m_MyCallsignArray.RemoveAll ();
				m_MyMemoArray.RemoveAll ();
				m_MyCallsignIndex = -1;
				break;
			}

			AddWork (Type, pBuffer, Length);
		}
		break;

	default:
		{
			AddWork (Type, pBuffer, Length);
		}
		break;
	}

	return Result;
}

VOID CMainFrame::OnWork (ULONG Type, PVOID pBuffer, ULONG Length)
{

	UCHAR				CmdBuff[4 * 1024];
	PACKET_CMD_HEADER*	pHeader = reinterpret_cast<PACKET_CMD_HEADER*>(CmdBuff);



	switch (Type)
	{
	case TYPE_SERIAL:
		{
			//DBG_MSG((_T("CMainFrame::OnWork - Type=TYPE_SERIAL \n")));
			//DBG_BUF (pBuffer, Length);

			if (m_Plugin.Flags & FLAGS_SERIAL)
			{	
				pHeader->m_Length = sizeof (PACKET_CMD_HEADER) + Length;
				pHeader->m_Type	= Type;
				memcpy (pHeader->GetData (), pBuffer, Length);

				m_TcpServer.SendClients (pHeader, pHeader->m_Length);
			}

			OnWorkSerial (pBuffer, Length);
		}
		break;

	case TYPE_VOICE:
		{
			//DBG_MSG((_T("CMainFrame::OnWork - Type=TYPE_VOICE \n")));
			
			if (m_Plugin.Flags & FLAGS_SERIAL)
			{	
				pHeader->m_Length = sizeof (PACKET_CMD_HEADER) + Length / 2;
				pHeader->m_Type	= TYPE_VOICE_G711;
				
				linear2ulaw (CmdBuff + sizeof (PACKET_CMD_HEADER), (SHORT*)pBuffer, Length / 2);

				m_TcpServer.SendClients (pHeader, pHeader->m_Length);
			}

			OnWorkVoice (pBuffer, Length);
		}
		break;

	case TYPE_VOICE_G711:
		{
			//DBG_MSG((_T("CMainFrame::OnWork - Type=TYPE_VOICE_G711 \n")));
			ulaw2linear ((SHORT*)CmdBuff, (UCHAR*)pBuffer, Length);
			
			OnWorkVoice (CmdBuff, Length * 2);
		}
		break;

	case TYPE_COMMAND:
		{
			DBG_MSG((_T("CMainFrame::OnWork - Type=TYPE_COMMAND \n")));
			OnWorkCommand (pBuffer, Length);
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}
}

VOID CMainFrame::AddWork (ULONG Type, PVOID pBuffer, ULONG Length)
{
	CMemFileEx*			pWork	= NULL;
	PACKET_CMD_HEADER*	pHeader;

	do
	{
		pWork = m_WorkList.GetFreeMemFile ();
		ASSERT (pWork != NULL);
		if (pWork == NULL)
		{	break;
		}

		pWork->SetLength (sizeof (PACKET_CMD_HEADER) + Length);

		pHeader = reinterpret_cast<PACKET_CMD_HEADER*>(pWork->GetBuffer ());

		pHeader->m_Length	= static_cast<ULONG>(pWork->GetLength ());
		pHeader->m_Type		= Type;

		memcpy (pHeader + 1, pBuffer, Length);

		m_WorkList.UsedMemFile (pWork);
		::SetEvent (m_hWorkEvent);
	}
	while (0);
}

UINT CMainFrame::WorkerThread (PVOID pParam)
{
	reinterpret_cast<CMainFrame*>(pParam)->WorkerThread ();
	return 0;
}

UINT CMainFrame::WorkerThread ()
{
	HANDLE				Handles[2];
	ULONG				Result;
	CList<CMemFileEx*>	WorkList;
	CMemFileEx*			pWork;
	PACKET_CMD_HEADER*	pHeader;

	DBG_MSG((_T("CMainFrame::WorkerThread - START \n")));

	Handles[0] = m_hStopEvent;
	Handles[1] = m_hWorkEvent;

	while ((Result = ::WaitForMultipleObjects (_countof (Handles), Handles, FALSE, INFINITE)) != WAIT_OBJECT_0)
	{
		switch (Result)
		{
		case (WAIT_OBJECT_0 + 1)://WORK
			{
				m_WorkList.GetUsedMemFiles (&WorkList);

				while (!WorkList.IsEmpty ())
				{
					pWork = WorkList.RemoveHead ();

					pHeader = reinterpret_cast<PACKET_CMD_HEADER*>(pWork->GetBuffer ());
					OnWork (pHeader->m_Type, pHeader->GetData (), pHeader->GetDatatLength ());

					m_WorkList.FreeMemFile (pWork);
				}
			}
			break;

		default:
			{
				ASSERT (FALSE);
			}
			break;
		}
	}

	DBG_MSG((_T("CMainFrame::WorkerThread - END \n")));
	return 0;
}

VOID CMainFrame::OnWorkSerial (PVOID pInBuffer, ULONG InLength)
{
	ULONG	CIV8;
	ULONG	CIV16;
	ULONG	CIV24;
	ULONG	Trx;
	PUCHAR	p		= reinterpret_cast<PUCHAR>(pInBuffer);
	ULONG	l		= InLength - (2 + 1);//2=FE FE, 1=FD

	Trx		= p[3];
	CIV8	= (p[3] << 24) | (p[4] << 16);
	CIV16	= (p[3] << 24) | (p[4] << 16) | (p[5] << 8);
	CIV24	= (p[3] << 24) | (p[4] << 16) | (p[5] << 8) | (p[6] << 0);

	switch (CIV8)
	{
	case CIV_READ_FREQUENCY1:// FE FE 80 01 03 00 00 12 38 04 FD 
	case CIV_READ_FREQUENCY2:
	case CIV_READ_OPERATING_MODE1:
	case CIV_READ_OPERATING_MODE2:
	case CIV_READ_DUPLEX_OFFSET1:
	case CIV_READ_DUPLEX_OFFSET2:
	case CIV_DUPLEX_MODE1:
	case CIV_DUPLEX_MODE2:
	case CIV_ATTENUATOR:
	case CIV_MAIN_POWER:
	case CIV_READ_ID:
		{
			OnCIV (Trx, CIV8, pInBuffer,  InLength);
		}
		break;

	default:
		{
			switch (CIV16)
			{
			case CIV_NOISE_SQL1:
			case CIV_NOISE_SQL2:
			case CIV_AF_VOLUME1:
			case CIV_AF_VOLUME2:
			case CIV_SQL_LEVEL1:
			case CIV_SQL_LEVEL2:
			case CIV_PTT_STATUS:
			case CIV_RF_POWER1:
			case CIV_RF_POWER2:
			case CIV_SIGNAL_LEVEL1:
			case CIV_SIGNAL_LEVEL2:
			case CIV_DV_MY_CALLSIGN_MEMCH:
			case CIV_DV_RX_CALLSIGN:
			case CIV_DV_TX_CALLSIGN:
			case CIV_DV_BREAK_IN_MODE:
			case CIV_DV_FM_DETECTED:
			case CIV_DV_DATA:
			case CIV_DV_RX_MESSAGE:
			case CIV_DV_TX_MESSAGE_MEMCH:
			case CIV_DV_TX_MESSAGE_ENABLE:
			case CIV_DV_EMERGENCY_MODE:
				{
					OnCIV (Trx, CIV16, pInBuffer,  InLength);
				}
				break;

			default:
				{
					switch (CIV24)
					{
					case CIV_MONITOR_MODE:
					case CIV_DUAL_WATCH_MODE:
					case CIV_MAIN_BAND_SELECTION:
					case CIV_VFO_MR_CALL_SELECTION1:
					case CIV_VFO_MR_CALL_SELECTION2:
					case CIV_DV_TX_MESSAGE1:
					case CIV_DV_TX_MESSAGE2:
					case CIV_DV_TX_MESSAGE3:
					case CIV_DV_TX_MESSAGE4:
					case CIV_DV_TX_MESSAGE5:
					case CIV_DV_MY_CALLSIGN1:
					case CIV_DV_MY_CALLSIGN2:
					case CIV_DV_MY_CALLSIGN3:
					case CIV_DV_MY_CALLSIGN4:
					case CIV_DV_MY_CALLSIGN5:
					case CIV_DV_MY_CALLSIGN6:
						{
							OnCIV (Trx, CIV24, pInBuffer,  InLength);
						}
						break;

					default:
						{
							OnCIV (Trx, CIV24, pInBuffer,  InLength);
						}
						break;
					}
				}
				break;
			}
		}
		break;
	}
}

VOID CMainFrame::OnCIV (ULONG Trx, ULONG CIV, PVOID pBuffer, ULONG Length)
{
	PUCHAR	p		= reinterpret_cast<PUCHAR>(pBuffer);
	ULONG	l		= Length - (2 + 1);//2=FE FE, 1=FD

	m_CivCache.Set (CIV, pBuffer, Length);
	
#ifdef _DEBUG
	m_wndDebugView.AddDump (_T("SERIAL : "), pBuffer, Length);
#endif
	
	m_wndCommonView.OnCIV (Trx, CIV, pBuffer, Length);
	m_wndAnalogFreqView.OnCIV (Trx, CIV, pBuffer, Length);
	m_wndDigitalFreqView.OnCIV (Trx, CIV, pBuffer, Length);

	switch (CIV)
	{
	case CIV_READ_FREQUENCY1:// FE FE 80 01 03 00 00 12 38 04 FD 
	case CIV_READ_FREQUENCY2:
		{
#ifdef _DEBUG
			ULONGLONG Freq = SWAPBCD2ULONGLONG (p + 5, 5);
			m_wndDebugView.AddFormat (_T("SERIAL : FREQ%u = %I64u"), Trx, Freq);
#endif
		}
		break;

	case CIV_READ_OPERATING_MODE1:
	case CIV_READ_OPERATING_MODE2:
	case CIV_READ_DUPLEX_OFFSET1:
	case CIV_READ_DUPLEX_OFFSET2:
	case CIV_DUPLEX_MODE1:
	case CIV_DUPLEX_MODE2:
		{
		}
		break;

		//CCommonView
	case CIV_RF_POWER1:
	case CIV_RF_POWER2:
	case CIV_ATTENUATOR:
	case CIV_MAIN_POWER:
	case CIV_READ_ID:
		{
		}
		break;

	case CIV_NOISE_SQL1:
		{
			m_VoiceEnable &= ~VOICE_SQL1;
			m_VoiceEnable |= p[6] != 0 ? VOICE_SQL1 : 0;
			
			if (p[6] == 0)
			{
				//Clear DATA
				m_DvData[Trx].SetLength (0);
				m_strLastGpsA = "";
			}
		}
		break;

	case CIV_NOISE_SQL2:
		{
			m_VoiceEnable &= ~VOICE_SQL2;
			m_VoiceEnable |= p[6] != 0 ? VOICE_SQL2 : 0;

			if (p[6] == 0)
			{
				CStringA strA(m_strLastCaller);
				m_strLastCaller = _T("");

				strA = strA.Left (8);
				strA.Trim ();
				if (!strA.IsEmpty ())
				{
					m_WaveSpeech.Play (strA);
				}

				//Clear DATA
				m_DvData[Trx].SetLength (0);
				m_strLastGpsA = "";
				
				DBG_MSG((_T("\n%s\n"), m_strPic.GetString ()));
				m_strPic = _T("");;
			}

		}
		break;

	case CIV_AF_VOLUME1:
	case CIV_AF_VOLUME2:
	case CIV_SQL_LEVEL1:
	case CIV_SQL_LEVEL2:
	case CIV_SIGNAL_LEVEL1:
	case CIV_SIGNAL_LEVEL2:
		{
		}
		break;


	case CIV_PTT_STATUS:
		{
			switch (p[6])//0x00=RX, 0x02=TX, 0x01=TX NG
			{
			case 0x00:
				{
					m_WaveSpeech.Stop ();
				}
				break;

			case 0x02:
				{
					UCHAR pTx[256] = {0};

					if (m_CivCache.Lookup (CIV_DV_TX_CALLSIGN, pTx, sizeof (pTx)))
					{
						CString strCaller;
						CString strMemo;
						CString strRpt2		= DATA2STR (pTx + 6, 8);
						CString strRpt1		= DATA2STR (pTx + 14, 8);
						CString strCalled	= DATA2STR (pTx + 22, 8);

						if ((m_MyCallsignIndex != -1) && (m_MyCallsignArray.GetCount () > m_MyCallsignIndex))
						{
							strCaller	= m_MyCallsignArray.GetAt (m_MyCallsignIndex);
							strMemo		= m_MyMemoArray.GetAt (m_MyCallsignIndex);
						}

						m_wndCallsignHistoryView.AddTxCall (strCaller, strMemo, strCalled, strRpt1, strRpt2);
					}
				}
				break;
			}
		}
		break;

	case CIV_DV_RX_CALLSIGN:
		{
			CString strCaller;
			CString strCalled;
			m_wndCallsignHistoryView.AddRxCall (pBuffer, Length, &strCaller, &strCalled);
			
			if (!strCaller.IsEmpty ())
			{
				m_strLastCaller = strCaller;
				theApp.AddRxCs (strCaller);

				m_wndDigitalFreqView.UpdateRxCs ();
			}
		}
		break;

	case CIV_DV_TX_CALLSIGN:
	case CIV_DV_BREAK_IN_MODE:
	case CIV_DV_FM_DETECTED:
		{
		}
		break;

	case CIV_DV_DATA:
		{
			ASSERT (p[6] == (Length - 7 - 1));
			for (int i=0;i<p[6];i++)
			{
				m_DvData[Trx].Write (p + 7 + i, 1);

				if (p[7 + i] == 0x0D)
				{

					DBG_MSG((_T("### 1 ### \n")));
					DBG_BUF (m_DvData[Trx].GetBuffer (), static_cast<ULONG>(m_DvData[Trx].GetLength ()));
					OnRecvData (Trx, m_DvData[Trx].GetBuffer (), static_cast<ULONG>(m_DvData[Trx].GetLength ()));
					m_DvData[Trx].SetLength (0);
				}
			}
		}
		break;

	case CIV_DV_RX_MESSAGE:
		{
			m_wndMessageHistoryView.AddRxMessage (pBuffer, Length);
		}
		break;

	case CIV_DV_TX_MESSAGE_MEMCH:
	case CIV_DV_TX_MESSAGE_ENABLE:
	case CIV_DV_EMERGENCY_MODE:
		{
		}
		break;

	case CIV_MONITOR_MODE:
		{
			m_VoiceEnable &= ~VOICE_MONITOR;
			m_VoiceEnable |= p[7] != 0 ? VOICE_MONITOR : 0;
		}
		break;

	case CIV_DUAL_WATCH_MODE:
		{
		}
		break;

	case CIV_MAIN_BAND_SELECTION:
	case CIV_VFO_MR_CALL_SELECTION1:
	case CIV_VFO_MR_CALL_SELECTION2:
		{
		}
		break;

	case CIV_DV_TX_MESSAGE1://FE FE 80 02 1D E0 01 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 FD 
	case CIV_DV_TX_MESSAGE2:
	case CIV_DV_TX_MESSAGE3:
	case CIV_DV_TX_MESSAGE4:
	case CIV_DV_TX_MESSAGE5:
		{
#ifdef _DEBUG
			CString strMsg = DATA2STR (p + 7, 20); 
			m_wndDebugView.AddFormat (_T("SERIAL : TXMSG%u %s"), Trx, strMsg.GetString ());
#endif
		}
		break;

	case CIV_DV_MY_CALLSIGN1://FE FE 80 01 1D 08 01 4A 4F 33 58 57 43 20 20 49 44 39 32 FD
	case CIV_DV_MY_CALLSIGN2://                      J  O  3  X  W  C        I  D  9  2
	case CIV_DV_MY_CALLSIGN3:
	case CIV_DV_MY_CALLSIGN4:
	case CIV_DV_MY_CALLSIGN5:
	case CIV_DV_MY_CALLSIGN6:
		{
			CString strCall = DATA2STR (p + 7, 8); 
			CString strMemo = DATA2STR (p + 15, 4); 
#ifdef _DEBUG
			m_wndDebugView.AddFormat (_T("SERIAL : MYCALL%u %s/%s"), Trx, strCall.GetString (), strMemo.GetString ());
#endif
		
			m_MyCallsignArray.SetAtGrow (p[6] - 1, strCall);
			m_MyMemoArray.SetAtGrow (p[6] - 1, strMemo);
		}
		break;

	case CIV_DV_MY_CALLSIGN_MEMCH:
		{
			m_MyCallsignIndex = p[6] - 1;
		}
		break;

	default:
		{
			//ASSERT (FALSE);
			DBG_MSG((_T("CMainFrame::OnWorkSerial - UNKNOWN CIV(%06X)!! \n"), CIV));
		}
		break;
	}
}

VOID CMainFrame::OnWorkCommand (PVOID pBuffer, ULONG Length)
{
	PACKET_CMD_HEADER*	pCmdHdr	= reinterpret_cast<PACKET_CMD_HEADER*>(pBuffer);

#ifdef _DEBUG
	m_wndDebugView.AddDump (_T("COMMAND : "), pBuffer, Length);
#endif

	do
	{
		if (m_Plugin.Flags & FLAGS_SERIAL)
		{
			ASSERT (m_Plugin.Command != NULL);
			if (m_Plugin.Command == NULL)
			{	break;
			}

			m_Plugin.Command (pCmdHdr->m_Type, pCmdHdr->GetData (), pCmdHdr->GetDatatLength ());

			break;
		}

		if (m_Plugin.Flags & FLAGS_NETWORK)
		{
			ASSERT (m_Plugin.Command != NULL);
			if (m_Plugin.Command == NULL)
			{	break;
			}

			m_Plugin.Command (TYPE_COMMAND, pBuffer, Length);

			break;
		}

	}
	while (0);
}


VOID CMainFrame::OnWorkVoice (PVOID pBuffer, ULONG Length)
{
#ifdef _DEBUG
	//m_wndDebugView.AddDump (_T("VOICE : "), pBuffer, Length);
	//m_wndDebugView.AddFormat (_T("VOICE : len=%u"), Length);
#endif

	do
	{
		m_DirectSoundRender.Play (pBuffer, Length);
	}
	while (0);
}




VOID CMainFrame::OnRecvData (ULONG Trx, PVOID pBuffer, ULONG Length)
{
	CHAR		szGPS[]	= "CRC";
	CHAR		szPIC[]	= "Pic";
	PCHAR		p		= reinterpret_cast<PCHAR>(pBuffer);
	CStringA	strA;

	//SKIP '$'
	for (ULONG i=0;i<Length;i++)
	{
		if (*p != '$')
		{	break;
		}
		p++;
		Length--;
	}

	if (StrCmpNIA (p, szGPS, sizeof (szGPS) -1) == 0)
	{
		//GPS
		p +=3;//skip "CRC"
		Length -= 3;

		strA.Format("%04X", GPS_Crc (p + 5, Length - 5));//5 = CRC 4byte + ','
		
		do
		{
			if (StrCmpNIA (p, strA, strA.GetLength ()) != 0)
			{	break;
			}

			p +=5;//skip CRC 4byte + ','
			Length -= 5;

			strA.SetString (p, Length);
			strA.TrimRight ("\r\n");

			if (m_strLastGpsA == strA)
			{	break;
			}

			m_strLastGpsA = strA;
			m_wndGpsHistoryView.AddGps(CString (strA));
		}
		while (0);
	}
	else if (StrCmpNIA (p, szPIC, sizeof (szPIC) -1) == 0)
	{
		//Pic
		CStringA	strCaller;
		CStringA	strCalled;
		CStringA	strNo;
		PCHAR		pStart;
//0000 : 24 24 50 69 63 2C 4A 4F 33 58 57 43 2C 43 51 43 $$Pic,JO3XWC,CQC
//0010 : 51 43 51 2C 30 31 33 33 02 EF 80 EF 80 F7 FA 28 QCQ,0133.......(
		for (ULONG i=0;i<Length;i++)
		{
			if (*p == ',')
			{	break;
			}
			p++;
			Length--;
		}
		p++;
		Length--;
		pStart = p;

		for (ULONG i=0;i<Length;i++)
		{
			if (*p == ',')
			{	break;
			}
			p++;
			Length--;
		}

		strCaller.SetString (pStart, static_cast<INT>(p - pStart));
		p++;
		Length--;
		pStart = p;

		for (ULONG i=0;i<Length;i++)
		{
			if (*p == ',')
			{	break;
			}
			p++;
			Length--;
		}

		strCalled.SetString (pStart, static_cast<INT>(p - pStart));
		p++;
		Length--;

		strNo.SetString (p, 4);
		p+=4;
		Length-=4;

		m_strPic.AppendFormat (_T("%s,%s,%s,"), CString (strCaller).GetString (), CString (strCalled).GetString (), CString (strNo).GetString ());
		for (ULONG i=0;i<Length;i++)
		{
			UCHAR d = (UCHAR)p[i];

			if (d == 0xEF)
			{
				i++;
				d = ((UCHAR)p[i] + 0x80) & 0xFF;
			}
			m_strPic.AppendFormat (_T("%02X "), d);
		}
		m_strPic += _T("\r\n");
	}
	else
	{

	}
}


void CMainFrame::OnCmdImportRepeaterList()
{
	CFileDialog Dlg (TRUE, NULL, NULL, 6, _T("Repeater List(*.csv)|*.csv||"));

	if (Dlg.DoModal () == IDOK)
	{
		m_wndRepeaterView.LoadRepeaterList (Dlg.GetPathName ());
		m_wndDigitalFreqView.OnRepeaterListChange();

		theApp.GetRepeaterList ()->Save (GetMyDir () + _T("RptList.csv"));

	}
}

