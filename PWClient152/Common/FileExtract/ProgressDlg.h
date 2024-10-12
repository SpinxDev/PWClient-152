#if !defined(AFX_PROGRESSDLG_H__A78D2C36_1B49_4DCF_9D54_A0686F46AD4B__INCLUDED_)
#define AFX_PROGRESSDLG_H__A78D2C36_1B49_4DCF_9D54_A0686F46AD4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	~CProgressDlg();

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_EXPORT_PROGRESS };
	CProgressCtrl	m_ctlProgress;
	CString	m_szCurFile;
	//}}AFX_DATA

	enum
	{
		OP_EXPORT = 1,			// �����ļ��Ĳ���
		OP_SEARCHSCALEDNPC,		// ���������Ź���NPCģ��
		OP_SEARCHEQUIPGFXNOHANGER,	// ����ӵ����Чȴû�йҵ��װ��
	};

	// ִ�е�������
	static int DoExport(const char* szOutDir);
	// ִ����������ģ�Ͳ���
	static int DoSearchScaledNPC();
	// ִ����������Ч�޹ҵ��װ��
	static int DoSearchEquipGfxNoHanger();

	// ����ָ��
	static CProgressDlg* g_pThis;
	// �̺߳���
	static DWORD WINAPI ProcessProc(LPVOID lpParameter);
	// �ص�����
	static bool OPCallback(const char* filename);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_iOperation;
	char m_szOutDir[MAX_PATH];
	UINT m_nFileCount;
	UINT m_nFileIndex;
	bool m_bRunning;
	int m_iCurProgress;

	HANDLE m_hThreadProc;
	CRITICAL_SECTION m_csLock;

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__A78D2C36_1B49_4DCF_9D54_A0686F46AD4B__INCLUDED_)
