
// IDControl.h : IDControl アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CIDControlApp:
// このクラスの実装については、IDControl.cpp を参照してください。
//

class CIDControlApp : public CWinAppEx
{
public:
	CIDControlApp();


// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	struct
	{
	public:
		struct
		{
			CString		m_strUserName;
			CString		m_strUserPass;
			CString		m_strRemoteAddr;
			INT			m_RemotePort;

			INT			m_ServerEnable;
			INT			m_ServerPort;
		}m_Network;

		struct
		{
			CString		m_strPluginPath;
			INT			m_ComNo;
		}m_Trx;

		struct
		{
			INT			m_Enable;
			CString		m_strID;
		}m_Mic;

		struct
		{
			INT			m_Enable;
			CString		m_strID;
		}m_Speaker;
		
		ULONG			m_SpeechLevel;


	}m_Setting;

	VOID LoadSetting ();
	VOID SaveSetting ();

public:
	//RX -> CS
	VOID LoadRxCs ();
	VOID SaveRxCs ();
	VOID AddRxCs (LPCTSTR pszCallSign, BOOL Save = TRUE);
	VOID GetRxCs (CStringArray* pArray);

private:
	CStringList	m_RxCsList;

public:
	CRepeaterList* GetRepeaterList ()
	{
		return &m_RepeaterList;
	}

protected:
	CRepeaterList	m_RepeaterList;
};

extern CIDControlApp theApp;
