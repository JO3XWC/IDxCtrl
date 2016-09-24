#pragma once
#include "afxwin.h"


// CDlgTrxSetting ダイアログ

class CDlgTrxSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTrxSetting)

public:
	CDlgTrxSetting(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgTrxSetting();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRX_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CStatic m_PluginLabel;
	CComboBox m_PluginComboBox;
	CStatic m_ComPortLabel;
	CComboBox m_ComPortComboBox;
	CButton m_ServerPortCheck;
	CEdit m_RemotePortEditBox;
	CStatic m_RemoteAddrLabel;
	CEdit m_RemoteAddrEditBox;
	CStatic m_RemotePortLabel;
	CEdit m_ServerPortEditBox;
	CButton m_OkButton;
	CButton m_CancelButton;

public:
	class CPluginData
	{
	public:
		CString		m_strFileName;
		ULONG		m_Flags;			//	SERIAL / NETWORK
		CString		m_strPluginName;
		CString		m_strDescription;
		CString		m_strCopyright;
	};
private:

public:
	afx_msg void OnCbnSelchangePluginCombo();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedServerPortCheck();

	CStatic m_UserNameLabel;
	CEdit m_UserNameEditBox;
	CStatic m_UserPassLabel;
	CEdit m_UserPassEditBox;
};
