// CDlgSoundSetting.cpp : 実装ファイル
//

#include "stdafx.h"
#include "IDControl.h"
#include "CDlgSoundSetting.h"
#include "afxdialogex.h"
#include "CDirectSound.h"


// CDlgSoundSetting ダイアログ

IMPLEMENT_DYNAMIC(CDlgSoundSetting, CDialogEx)

CDlgSoundSetting::CDlgSoundSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SOUND_SETTING, pParent)
{

}

CDlgSoundSetting::~CDlgSoundSetting()
{
}

void CDlgSoundSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MIC_PLUGIN_CHECK, m_MicCheckButton);
	DDX_Control(pDX, IDC_MIC_PLUGIN_COMBO, m_MicComboBox);
	DDX_Control(pDX, IDC_SPEAKER_PLUGIN_CHECK, m_SpeakerCheckButton);
	DDX_Control(pDX, IDC_SPEAKER_PLUGIN_COMBO, m_SpeakerComboBox);
}


BEGIN_MESSAGE_MAP(CDlgSoundSetting, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgSoundSetting メッセージ ハンドラー


BOOL CDlgSoundSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	INT				Index;
	CString			strValue;
	CArray<ESoundDevice>	CaptureDeviceArray;
	CArray<ESoundDevice>	RenderDeviceArray;
	ESoundDevice			Device;
	ESoundDevice*			pDevice;

	SetWindowText (MlLoadString (IDS_SOUND_SETTING_TITLE));
	m_MicCheckButton.SetWindowText (MlLoadString (IDS_SOUND_SETTING_MIC_CHECK));
	m_SpeakerCheckButton.SetWindowText (MlLoadString (IDS_SOUND_SETTING_SPEAKER_CHECK));

	CDirectSound::EnumCaptureDevice (&CaptureDeviceArray);
	CDirectSound::EnumRenderDevice (&RenderDeviceArray);

	for (int i=0;i<CaptureDeviceArray.GetCount ();i++)
	{
		Device = CaptureDeviceArray.GetAt (i);
		
		pDevice = new ESoundDevice;
		*pDevice = Device;

		Index = m_MicComboBox.AddString (Device.m_strName);
		m_MicComboBox.SetItemDataPtr (Index, pDevice);

		if (theApp.m_Setting.m_Mic.m_strID == pDevice->m_strID)
		{
			m_MicComboBox.SetCurSel (Index);
		}
	}

	for (int i=0;i<RenderDeviceArray.GetCount ();i++)
	{
		Device = RenderDeviceArray.GetAt (i);
		
		pDevice = new ESoundDevice;
		*pDevice = Device;

		Index = m_SpeakerComboBox.AddString (Device.m_strName);
		m_SpeakerComboBox.SetItemDataPtr (Index, pDevice); 

		if (theApp.m_Setting.m_Speaker.m_strID == pDevice->m_strID)
		{
			m_SpeakerComboBox.SetCurSel (Index);
		}
	}

	m_MicCheckButton.SetCheck (theApp.m_Setting.m_Mic.m_Enable ? BST_CHECKED : BST_UNCHECKED);
	m_SpeakerCheckButton.SetCheck (theApp.m_Setting.m_Speaker.m_Enable ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgSoundSetting::OnDestroy()
{
	CDialogEx::OnDestroy();

	ESoundDevice*	pDevice;

	for (int i=0;i<m_MicComboBox.GetCount ();i++)
	{
		pDevice = reinterpret_cast<ESoundDevice*>(m_MicComboBox.GetItemDataPtr (i));
		delete pDevice;
	}

	for (int i=0;i<m_SpeakerComboBox.GetCount ();i++)
	{
		pDevice = reinterpret_cast<ESoundDevice*>(m_SpeakerComboBox.GetItemDataPtr (i));
		delete pDevice;
	}

}

void CDlgSoundSetting::OnOK()
{
	INT				Index;
	CString			strValue;
	ESoundDevice*	pDevice;

	Index = m_MicComboBox.GetCurSel ();
	if (Index == CB_ERR)
	{	
		theApp.m_Setting.m_Mic.m_strID = _T("");
	}
	else
	{
		pDevice = reinterpret_cast<ESoundDevice*>(m_MicComboBox.GetItemDataPtr (Index));
		theApp.m_Setting.m_Mic.m_strID = pDevice->m_strID;
	}


	Index = m_SpeakerComboBox.GetCurSel ();
	if (Index == CB_ERR)
	{	
		theApp.m_Setting.m_Speaker.m_strID = _T("");
	}
	else
	{
		pDevice = reinterpret_cast<ESoundDevice*>(m_SpeakerComboBox.GetItemDataPtr (Index));
		theApp.m_Setting.m_Speaker.m_strID = pDevice->m_strID;
	}

	theApp.m_Setting.m_Mic.m_Enable = m_MicCheckButton.GetCheck () == BST_CHECKED;
	theApp.m_Setting.m_Speaker.m_Enable = m_SpeakerCheckButton.GetCheck () == BST_CHECKED;
	
	CDialogEx::OnOK();
}


