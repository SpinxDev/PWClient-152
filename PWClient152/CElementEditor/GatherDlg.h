#if !defined(AFX_GATHERDLG_H__4BD50034_ED74_4BD8_9533_4778B318A791__INCLUDED_)
#define AFX_GATHERDLG_H__4BD50034_ED74_4BD8_9533_4778B318A791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GatherDlg.h : header file
//

#include "SceneGatherObject.h"

/////////////////////////////////////////////////////////////////////////////
// CGatherDlg dialog

class CGatherDlg : public CDialog
{
// Construction
public:
	CGatherDlg(CWnd* pParent = NULL);   // standard constructor
	void FreshList();
	inline void Init(CSceneGatherObject* pGather){ m_pGather = pGather; };
// Dialog Data
	//{{AFX_DATA(CGatherDlg)
	enum { IDD = IDD_GATER_AREA };
	CListBox	m_listGather;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGatherDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CSceneGatherObject *m_pGather;
	// Generated message map functions
	//{{AFX_MSG(CGatherDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGatherAdd();
	afx_msg void OnGatherModify();
	afx_msg void OnGatherDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATHERDLG_H__4BD50034_ED74_4BD8_9533_4778B318A791__INCLUDED_)
