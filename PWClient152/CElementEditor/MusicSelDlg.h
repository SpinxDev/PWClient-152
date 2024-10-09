#if !defined(AFX_MUSICSELDLG_H__A0C61FE9_C2BB_49FC_92F5_D95EA465E69E__INCLUDED_)
#define AFX_MUSICSELDLG_H__A0C61FE9_C2BB_49FC_92F5_D95EA465E69E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MusicSelDlg.h : header file
//

#include "ScenePrecinctObject.h"

/////////////////////////////////////////////////////////////////////////////
// CMusicSelDlg dialog

class CMusicSelDlg : public CDialog
{
// Construction
public:
	CMusicSelDlg(CWnd* pParent = NULL);   // standard constructor

	inline void Init(CScenePrecinctObject *pobj){ m_pSceneBoxArea = pobj; };
	void FreshList();

// Dialog Data
	//{{AFX_DATA(CMusicSelDlg)
	enum { IDD = IDD_DIALOG_MUSIC_SEL };
	CListBox	m_listMusic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMusicSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CScenePrecinctObject *m_pSceneBoxArea;
	// Generated message map functions
	//{{AFX_MSG(CMusicSelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMusicAdd();
	afx_msg void OnMusicDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUSICSELDLG_H__A0C61FE9_C2BB_49FC_92F5_D95EA465E69E__INCLUDED_)
