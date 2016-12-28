// CDlgTrxSetting.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CDlgTrxSetting.h"
#include "afxdialogex.h"

#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgTrxSetting ダイアログ

IMPLEMENT_DYNAMIC(CDlgTrxSetting, CDialogEx)

CDlgTrxSetting::CDlgTrxSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRX_SETTING, pParent)
{

}

CDlgTrxSetting::~CDlgTrxSetting()
{
}

void CDlgTrxSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLUGIN_LABEL		, m_PluginLabel);
	DDX_Control(pDX, IDC_PLUGIN_COMBO		, m_PluginComboBox);
	DDX_Control(pDX, IDC_COMPORT_LABEL		, m_ComPortLabel);
	DDX_Control(pDX, IDC_COMPORT_COMBO		, m_ComPortComboBox);
	DDX_Control(pDX, IDC_SERVER_PORT_CHECK	, m_ServerPortCheck);
	DDX_Control(pDX, IDC_SERVER_PORT_EDIT	, m_ServerPortEditBox);
	DDX_Control(pDX, IDC_HOST_ADDR_LABEL	, m_RemoteAddrLabel);
	DDX_Control(pDX, IDC_HOST_ADDR_EDIT		, m_RemoteAddrEditBox);
	DDX_Control(pDX, IDC_HOST_PORT_LABEL	, m_RemotePortLabel);
	DDX_Control(pDX, IDC_HOST_PORT_EDIT		, m_RemotePortEditBox);
	DDX_Control(pDX, IDOK					, m_OkButton);
	DDX_Control(pDX, IDCANCEL				, m_CancelButton);
	DDX_Control(pDX, IDC_USER_NAME_LABEL	, m_UserNameLabel);
	DDX_Control(pDX, IDC_USER_NAME_EDIT		, m_UserNameEditBox);
	DDX_Control(pDX, IDC_USER_PASS_LABEL	, m_UserPassLabel);
	DDX_Control(pDX, IDC_USER_PASS_EDIT		, m_UserPassEditBox);
}


BEGIN_MESSAGE_MAP(CDlgTrxSetting, CDialogEx)
	ON_CBN_SELCHANGE(IDC_PLUGIN_COMBO, &CDlgTrxSetting::OnCbnSelchangePluginCombo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SERVER_PORT_CHECK, &CDlgTrxSetting::OnBnClickedServerPortCheck)
END_MESSAGE_MAP()


// CDlgTrxSetting メッセージ ハンドラー


BOOL CDlgTrxSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString			strPlugin = GetMyDir () + _T("plugins\\");
	WIN32_FIND_DATA	Data;
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	HMODULE			hModule;
	PLUGIN			Plugin;
	CPluginData*	pPlugin;
	INT				Index;
	CString			strValue;
	ULONG (WINAPI* IDCtrlInfo) (PLUGIN* pPlugin);

	SetWindowText (MlLoadString (IDS_TRX_SETTING_TITLE));
	m_PluginLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_PLUGIN_LABEL));
	m_ComPortLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_COM_LABEL));
	m_ServerPortCheck.SetWindowText (MlLoadString (IDS_TRX_SETTING_SERVER_PORT_CHECK));
	m_RemoteAddrLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_REMOTE_ADDR_LABEL));
	m_RemotePortLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_REMOTE_PORT_LABEL));
	m_OkButton.SetWindowText (MlLoadString (IDS_OK_BUTTON));
	m_CancelButton.SetWindowText (MlLoadString (IDS_CANCEL_BUTTON));
	m_UserNameLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_USER_NAME_LABEL));
	m_UserPassLabel.SetWindowText (MlLoadString (IDS_TRX_SETTING_USER_PASS_LABEL));

	CString strPort;
	for (int i=0;i<256;i++)
	{
		strPort.Format (_T("%u"), i+1);
		Index = m_ComPortComboBox.AddString (strPort);

		if ((i+1) == theApp.m_Setting.m_Trx.m_ComNo)
		{
			m_ComPortComboBox.SetCurSel (Index);
		}
	}

	m_RemotePortEditBox.SetLimitText (5);
	m_RemoteAddrEditBox.SetLimitText (128);
	m_ServerPortEditBox.SetLimitText (5);

	do
	{
		hFind = ::FindFirstFile (strPlugin + _T("*.dll"), &Data);
		if (hFind == INVALID_HANDLE_VALUE)
		{	break;
		}

		do
		{
			if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{	continue;
			}

			hModule = ::LoadLibrary (strPlugin + Data.cFileName);
			if (hModule == NULL)
			{	continue;
			}

			do
			{
				*((FARPROC*)&IDCtrlInfo) = ::GetProcAddress (hModule, "IDCtrlInfo");
				if (IDCtrlInfo == NULL)
				{	break;
				}

				Plugin.Size = sizeof (Plugin);
				
				if (IDCtrlInfo (&Plugin) != ERROR_SUCCESS)
				{	break;
				}

				if (!(Plugin.Flags & (FLAGS_SERIAL | FLAGS_NETWORK)))
				{	break;
				}

				pPlugin = new CPluginData;

				pPlugin->m_strFileName		= strPlugin + Data.cFileName;
				pPlugin->m_Flags			= Plugin.Flags;
				pPlugin->m_strPluginName	= Plugin.pszPluginName;
				pPlugin->m_strDescription	= Plugin.pszDescription;
				pPlugin->m_strCopyright		= Plugin.pszCopyright;

				Index = m_PluginComboBox.AddString (pPlugin->m_strPluginName);
				m_PluginComboBox.SetItemDataPtr (Index, pPlugin);

				if (theApp.m_Setting.m_Trx.m_strPluginPath == pPlugin->m_strFileName)
				{
					m_PluginComboBox.SetCurSel (Index);
				}
			}
			while (0);

			::FreeLibrary (hModule);

		}
		while (::FindNextFile (hFind, &Data));
	}
	while (0);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		::FindClose (hFind);
	}


	m_ServerPortCheck.SetCheck (theApp.m_Setting.m_Network.m_ServerEnable ? BST_CHECKED : BST_UNCHECKED);
	
	strValue.Format(_T("%u"), theApp.m_Setting.m_Network.m_RemotePort);
	m_RemotePortEditBox.SetWindowText (strValue);

	m_RemoteAddrEditBox.SetWindowText (theApp.m_Setting.m_Network.m_strRemoteAddr);
	
	strValue.Format(_T("%u"), theApp.m_Setting.m_Network.m_ServerPort);
	m_ServerPortEditBox.SetWindowText (strValue);
	
	m_UserNameEditBox.SetWindowText (theApp.m_Setting.m_Network.m_strUserName);
	m_UserPassEditBox.SetWindowText (theApp.m_Setting.m_Network.m_strUserPass);

	OnCbnSelchangePluginCombo ();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgTrxSetting::OnDestroy()
{
	CDialogEx::OnDestroy();

	CPluginData* pPlugin;

	for (int i=0;i<m_PluginComboBox.GetCount ();i++)
	{
		pPlugin = reinterpret_cast<CPluginData*>(m_PluginComboBox.GetItemDataPtr (i));
		delete pPlugin;
	}

}


void CDlgTrxSetting::OnOK()
{
	INT Value;
	CString strValue;
	CPluginData* pPlugin;

	Value = m_ComPortComboBox.GetCurSel ();
	if (Value == CB_ERR)
	{	
		theApp.m_Setting.m_Trx.m_ComNo = 1;
	}
	else
	{
		m_ComPortComboBox.GetLBText (Value, strValue);
		theApp.m_Setting.m_Trx.m_ComNo = _ttoi (strValue);
	}


	Value = m_PluginComboBox.GetCurSel ();
	if (Value == CB_ERR)
	{	
		theApp.m_Setting.m_Trx.m_strPluginPath = _T("");
	}
	else
	{
		pPlugin = reinterpret_cast<CPluginData*>(m_PluginComboBox.GetItemDataPtr (Value));
		theApp.m_Setting.m_Trx.m_strPluginPath = pPlugin->m_strFileName;
	}

	theApp.m_Setting.m_Network.m_ServerEnable = m_ServerPortCheck.GetCheck () == BST_CHECKED;
	
	m_RemotePortEditBox.GetWindowText (strValue);
	theApp.m_Setting.m_Network.m_RemotePort = _ttoi (strValue);

	
	m_ServerPortEditBox.GetWindowText (strValue);
	theApp.m_Setting.m_Network.m_ServerPort = _ttoi (strValue);
	
	m_RemoteAddrEditBox.GetWindowText (theApp.m_Setting.m_Network.m_strRemoteAddr);
	m_UserNameEditBox.GetWindowText (theApp.m_Setting.m_Network.m_strUserName);
	m_UserPassEditBox.GetWindowText (theApp.m_Setting.m_Network.m_strUserPass);

	CDialogEx::OnOK();
}


void CDlgTrxSetting::OnCbnSelchangePluginCombo()
{
	CPluginData* pPlugin;
	INT Index;

	m_ComPortLabel.EnableWindow (FALSE);
	m_ComPortComboBox.EnableWindow (FALSE);
	m_ServerPortCheck.EnableWindow (FALSE);
	m_RemotePortEditBox.EnableWindow (FALSE);
	m_RemoteAddrLabel.EnableWindow (FALSE);
	m_RemoteAddrEditBox.EnableWindow (FALSE);
	m_RemotePortLabel.EnableWindow (FALSE);
	m_ServerPortEditBox.EnableWindow (FALSE);
	m_UserNameLabel.EnableWindow (FALSE);
	m_UserNameEditBox.EnableWindow (FALSE);
	m_UserPassLabel.EnableWindow (FALSE);
	m_UserPassEditBox.EnableWindow (FALSE);

	do
	{
		Index = m_PluginComboBox.GetCurSel ();
		ASSERT (Index >= 0);
		if (Index == CB_ERR)
		{	break;
		}

		pPlugin = reinterpret_cast<CPluginData*>(m_PluginComboBox.GetItemDataPtr (Index));
		
		m_ComPortLabel.EnableWindow (pPlugin->m_Flags & FLAGS_SERIAL);
		m_ComPortComboBox.EnableWindow (pPlugin->m_Flags & FLAGS_SERIAL);
		m_ServerPortCheck.EnableWindow (pPlugin->m_Flags & FLAGS_SERIAL);
		m_ServerPortEditBox.EnableWindow ((pPlugin->m_Flags & FLAGS_SERIAL) && (m_ServerPortCheck.GetCheck () == BST_CHECKED));

		m_RemoteAddrLabel.EnableWindow (pPlugin->m_Flags & FLAGS_NETWORK);
		m_RemoteAddrEditBox.EnableWindow (pPlugin->m_Flags & FLAGS_NETWORK);
		m_RemotePortLabel.EnableWindow (pPlugin->m_Flags & FLAGS_NETWORK);
		m_RemotePortEditBox.EnableWindow (pPlugin->m_Flags & FLAGS_NETWORK);

		if (pPlugin->m_Flags & FLAGS_NETWORK)
		{
			m_UserNameLabel.EnableWindow (TRUE);
			m_UserNameEditBox.EnableWindow (TRUE);
			m_UserPassLabel.EnableWindow (TRUE);
			m_UserPassEditBox.EnableWindow (TRUE);
		}
		else
		{
			m_UserNameLabel.EnableWindow (m_ServerPortCheck.GetCheck () == BST_CHECKED);
			m_UserNameEditBox.EnableWindow (m_ServerPortCheck.GetCheck () == BST_CHECKED);
			m_UserPassLabel.EnableWindow (m_ServerPortCheck.GetCheck () == BST_CHECKED);
			m_UserPassEditBox.EnableWindow (m_ServerPortCheck.GetCheck () == BST_CHECKED);
		}
	}
	while (0);
}




void CDlgTrxSetting::OnBnClickedServerPortCheck()
{
	OnCbnSelchangePluginCombo ();
}


