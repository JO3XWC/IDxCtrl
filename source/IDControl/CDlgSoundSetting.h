#pragma once
#include "afxwin.h"


// CDlgSoundSetting ダイアログ

class CDlgSoundSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSoundSetting)

public:
	CDlgSoundSetting(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgSoundSetting();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOUND_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CButton m_MicCheckButton;
	CComboBox m_MicComboBox;
	CButton m_SpeakerCheckButton;
	CComboBox m_SpeakerComboBox;

private:
	class CPluginData
	{
	public:
		CString		m_strFileName;
		ULONG		m_Flags;
		CString		m_strPluginName;
		CString		m_strDescription;
		CString		m_strCopyright;
	};

public:
	afx_msg void OnDestroy();
};
