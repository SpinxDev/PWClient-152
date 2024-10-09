#if !defined(AFX_NPCSELDLG_H__958D0460_19E6_4071_90FE_F762E711EA76__INCLUDED_)
#define AFX_NPCSELDLG_H__958D0460_19E6_4071_90FE_F762E711EA76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NpcSelDlg.h : header file
//

#include "SceneAIGenerator.h"
#include "elementdataman.h"
#include "aassist.h"
/////////////////////////////////////////////////////////////////////////////
// CNpcSelDlg dialog

class CNpcSelDlg : public CDialog
{
// Construction
public:
	CNpcSelDlg(CWnd* pParent = NULL);   // standard constructor
	
	void Init(CSceneAIGenerator *pAIGenerator){ m_pSceneAIGenerator = pAIGenerator; };
	
	AString GetNpcName(DWORD id);

// Dialog Data
	//{{AFX_DATA(CNpcSelDlg)
	enum { IDD = IDD_AI_AREA };
	CListBox	m_listAI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSceneAIGenerator *m_pSceneAIGenerator;
	void FreshList();
	
	// Generated message map functions
	//{{AFX_MSG(CNpcSelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnNewAi();
	afx_msg void OnModifyAi();
	afx_msg void OnDeleteAi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NPCSELDLG_H__958D0460_19E6_4071_90FE_F762E711EA76__INCLUDED_)
