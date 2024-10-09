#if !defined(AFX_SCENEOBJECTSLIST_H__BDE9E0A1_4186_42BA_B9D4_70EE5FFB5244__INCLUDED_)
#define AFX_SCENEOBJECTSLIST_H__BDE9E0A1_4186_42BA_B9D4_70EE5FFB5244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneObjectsList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectsList dialog

class CSceneObjectsList : public CDialog
{
// Construction
public:
	CSceneObjectsList(CWnd* pParent = NULL);   // standard constructor

	void FreshObjectList();
// Dialog Data
	//{{AFX_DATA(CSceneObjectsList)
	enum { IDD = IDD_SCENE_OBJECTS_LIST };
	CTreeCtrl	m_treeScene;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneObjectsList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CSceneObjectsList)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkSceneTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK(){};
	virtual void OnCancel(){};
	afx_msg void OnClickSceneTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedSceneTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonFreshSceneList();
	afx_msg void OnButtonFreshDelSceneObject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEOBJECTSLIST_H__BDE9E0A1_4186_42BA_B9D4_70EE5FFB5244__INCLUDED_)
