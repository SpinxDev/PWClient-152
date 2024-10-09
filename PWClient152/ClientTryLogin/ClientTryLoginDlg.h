// ClientTryLoginDlg.h : header file
//

#if !defined(AFX_CLIENTTRYLOGINDLG_H__4B58A2E9_B6B5_482A_A048_F3BA8B169A6D__INCLUDED_)
#define AFX_CLIENTTRYLOGINDLG_H__4B58A2E9_B6B5_482A_A048_F3BA8B169A6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector.h>
#include <hashmap.h>
#include <AString.h>
#include <vector>
#include <string>
#include "ABaseDef.h"
#include "AChar.h"
#include "AAssist.h"
#include "SessionManager.h"
/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginDlg dialog

class CClientTryLoginDlg : public CDialog
{
// Construction
public:
	CClientTryLoginDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClientTryLoginDlg)
	enum { IDD = IDD_CLIENTTRYLOGIN_DIALOG };
	CButton	m_btnReStart;
	CListCtrl	m_accountCtrl;
	CButton	m_btnStart;
	CButton m_btnNormal;
	CButton m_btnCrossServer;
	CProgressCtrl	m_progressCtrl;
	CListCtrl	m_successInfoCtrl;
	CListCtrl	m_failInfoCtrl;
	CString	m_serverList;
	CString	m_userName;
	CString	m_pwd;
	CString	m_serverNum;
	CString	m_version;
	CStatic m_Warning;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientTryLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void StartTest();
	void StartFailTest();
	void ReadServerList();
	void ReadFailServerList();
	void ReLoadAccount();

	bool IsTestCrossServer();
	void DisablePanel();
	void EnablePanel();
	void TestCrossServerLeft(); // 测试跨服中央服的所有端口，需要多次调用，每次调用都要等待上轮测试完毕

	typedef abase::vector<AWString> SplitStrings;

	void SplitString(const AWString& str,SplitStrings&splits,const wchar_t*sep,bool bOnce = false);

	struct OneServerInfo
	{
		AWString serverName;
		AWString serverIP;
		int serverPortStart;
		int serverPortEnd;
		int serverZoneID;
		int serverLine;
		int destPort;
		ConnectResults cResults;
		bool bHaveSuccess;
		bool bHaveFail;
	};

	struct AccountInfo
	{
		CString userName;
		CString pwd;
	};

	typedef abase::vector<OneServerInfo> AreaServers;
	typedef abase::hash_map<AWString,AreaServers> ServerInfo;
	typedef abase::hash_map<AWString,AWString> ServerName2GroupName;
	typedef abase::vector<AccountInfo> AccountInfos;
	typedef abase::hash_map<int,bool> LineInfo;

	ServerInfo m_serverInfo;
	ServerName2GroupName m_serverName2Group;
	AccountInfos m_accountInfos;
	LineInfo m_lineInfos;

	ResultCollector m_collector;
	int m_TestCount;
	ConnectResults m_LoginVector;
	int m_nPortTestLoopCount;

	// Generated message map functions
	//{{AFX_MSG(CClientTryLoginDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonFindServerlist();
	afx_msg void OnButtonLogin();
	afx_msg void OnDrawListResultSuccess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFailDrawListResultFail(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonOpenError();
	afx_msg void OnButtonAddaccount();
	afx_msg void OnButtonDelaccount();
	afx_msg void OnButtonRelogin();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCancelMode();
	afx_msg void OnNormalTest();
	afx_msg void OnCrossTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTTRYLOGINDLG_H__4B58A2E9_B6B5_482A_A048_F3BA8B169A6D__INCLUDED_)
