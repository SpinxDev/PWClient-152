#if !defined(AFX_SCENEOBJECTPANEL_H__A4FDBCEA_95C6_4623_A623_FCA3EAFE36C4__INCLUDED_)
#define AFX_SCENEOBJECTPANEL_H__A4FDBCEA_95C6_4623_A623_FCA3EAFE36C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneObjectPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectPanel dialog

class CSceneObjectPanel : public CDialog
{
// Construction
public:
	CSceneObjectPanel(CWnd* pParent = NULL);   // standard constructor
	//弹起所有按纽，并把当前操作清空
	void RiseUpAllButton();

	void ShowPropertyPanel(bool bShow);
	void ShowListPlanel(bool bShow);
// Dialog Data
	//{{AFX_DATA(CSceneObjectPanel)
	enum { IDD = IDD_SCENE_OBJECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneObjectPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	void CheckOtherButtonCheck(CButton *pButton);
	// Generated message map functions
	//{{AFX_MSG(CSceneObjectPanel)
	afx_msg void OnSceneObjectLight();
	afx_msg void OnSceneObjectModel();
	afx_msg void OnSceneObjectSkybox();
	virtual void OnOK(){};
	virtual void OnCancel(){};
	afx_msg void OnSceneObjectWater();
	afx_msg void OnSceneObjectArea();
	afx_msg void OnSceneObjectPath();
	afx_msg void OnSceneObjectAudio();
	afx_msg void OnSceneObjectAi();
	afx_msg void OnSceneObjectVernier();
	afx_msg void OnSceneObjectDummy();
	afx_msg void OnObjectModelReload();
	afx_msg void OnSceneObjectSkinmodel();
	afx_msg void OnSceneObjectCritter();
	afx_msg void OnSceneObjectFixnpc();
	afx_msg void OnSceneObjectPrecinct();
	afx_msg void OnSceneObjectGather();
	afx_msg void OnSceneObjectRange();
	afx_msg void OnSceneObjectCloudSet();
	afx_msg void OnSceneObjectCloudBox();
	afx_msg void OnSceneObjectBoxInstance();
	afx_msg void OnSceneObjectDynamicAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEOBJECTPANEL_H__A4FDBCEA_95C6_4623_A623_FCA3EAFE36C4__INCLUDED_)
