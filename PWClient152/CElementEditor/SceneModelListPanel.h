#if !defined(AFX_SCENEMODELLISTPANEL_H__2B5C288A_1839_4E76_9332_FA5958813880__INCLUDED_)
#define AFX_SCENEMODELLISTPANEL_H__2B5C288A_1839_4E76_9332_FA5958813880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneModelListPanel.h : header file
//
#include "ElementResMan.h"
/////////////////////////////////////////////////////////////////////////////
// CSceneModelListPanel dialog

class CSceneModelListPanel : public CDialog
{
// Construction
public:
	CSceneModelListPanel(CWnd* pParent = NULL);   // standard constructor
	void FreshObjectList();
	void EnumObjects(CString strPathName, HTREEITEM hTreeItemp);
	void EnumFiles(CString strPathName, HTREEITEM hTreeItemp);
	
// Dialog Data
	//{{AFX_DATA(CSceneModelListPanel)
	enum { IDD = IDD_SCENE_OBJECT_MODEL_LIST };
	CTreeCtrl	m_treeSceneModel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneModelListPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HTREEITEM InsertItem(CTreeCtrl *pTree, HTREEITEM hRoot, CString name);
	ElementResMan mResMan;
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CSceneModelListPanel)
	afx_msg void OnSelchangedSceneObjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkSceneObjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK(){};
	virtual void OnCancel(){};
	afx_msg void OnRclickSceneModelTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetToTree();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEMODELLISTPANEL_H__2B5C288A_1839_4E76_9332_FA5958813880__INCLUDED_)
