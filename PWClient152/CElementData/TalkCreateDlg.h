#if !defined(AFX_TALKCREATEDLG_H__EA712C46_7B5A_44D8_BCCB_DA6C06737EA5__INCLUDED_)
#define AFX_TALKCREATEDLG_H__EA712C46_7B5A_44D8_BCCB_DA6C06737EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TalkCreateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTalkCreateDlg dialog

class CTalkCreateDlg : public CDialog
{
// Construction
public:
	CTalkCreateDlg(CWnd* pParent = NULL);   // standard constructor
	
	void SetEnableCreateWindow(bool bEnable){ m_bEnableWnd = bEnable; };


	bool m_bWindow;
	bool m_bModified;
	UINT    m_dwServiceID;

	CString m_strRootDir;
	CString m_strExt;
	DWORD   m_dwServiceLinkID;
	DWORD	m_dwParamToSel;
	DWORD	m_dwParamToSel2;
	DWORD	m_dwSvrToSel;
	//DWORD   m_dwParams;

	bool	m_bPreset;

// Dialog Data
	//{{AFX_DATA(CTalkCreateDlg)
	enum { IDD = IDD_DIALOG_CREATE_WINDOW };
	CString	m_strName;
	CString	m_strDesc;
	UINT	m_dwID;
	UINT    m_dwOldID;
	CString	m_strCommandName;
	CString	m_strLinkName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableCreateWindow(bool bEnable);
	void EnableLinkCtrl(bool bEnable);
	void EnableParamsCtrl(bool bEnable);
	void InitServiceCombox();
	void InitParamsCombox();
	bool m_bEnableWnd;
	// Generated message map functions
	//{{AFX_MSG(CTalkCreateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioTalkType1();
	afx_msg void OnRadioTalkType2();
	afx_msg void OnSelchangeComboService();
	afx_msg void OnButtonServiceLink();
	afx_msg void OnSelchangeComboServiceParams();
	afx_msg void OnRadioTalkType3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKCREATEDLG_H__EA712C46_7B5A_44D8_BCCB_DA6C06737EA5__INCLUDED_)
