// ClientSimulatorDlg.h : header file
//

#if !defined(AFX_CLIENTSIMULATORDLG_H__B60B90D7_2B61_4F69_A0CB_95DEE0A51199__INCLUDED_)
#define AFX_CLIENTSIMULATORDLG_H__B60B90D7_2B61_4F69_A0CB_95DEE0A51199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientSimulatorDlg dialog

class CClientSimulatorDlg : public CDialog
{
// Construction
public:
	CClientSimulatorDlg(CWnd* pParent = NULL);	// standard constructor

	// 输出信息
	void PrintMessage( const char* szMessage,int color );
	// 清除信息
	void ClearMessage();

// Dialog Data
	//{{AFX_DATA(CClientSimulatorDlg)
	enum { IDD = IDD_CLIENTSIMULATOR_DIALOG };
	CRichEditCtrl	m_ConsoleText;
	CEdit	m_ConsoleCmd;
	CString	m_strCommand;
	CFont	m_Font;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSimulatorDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CBrush m_BlackBrush;

	// Generated message map functions
	//{{AFX_MSG(CClientSimulatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeConsoleCmd();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSIMULATORDLG_H__B60B90D7_2B61_4F69_A0CB_95DEE0A51199__INCLUDED_)
