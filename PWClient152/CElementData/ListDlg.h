#if !defined(AFX_LISTDLG_H__E0AE7C8F_409F_405F_9E3E_D2FC05605528__INCLUDED_)
#define AFX_LISTDLG_H__E0AE7C8F_409F_405F_9E3E_D2FC05605528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListDlg.h : header file
//

#include "TreeFlagMan.h"
#include "MltiTree.h"
#include "DlgFindTemplate.h"
#include "DlgFindTemplResult.h"
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog

class CListDlg : public CDialog
{
// Construction
public:
	CListDlg(CWnd* pParent = NULL);   // standard constructor
	void EnumObjects(CString strPathName , HTREEITEM hTreeItemp, bool bProperty, int iCount ,bool bOnlyFirst = false);
	void EnumFiles(CString strPathName, HTREEITEM hTreeItemp, bool bProperty, bool bOnlyFirst = false);
	void DeleteAllItems();
	bool IsCreateFolder(CString path);
	bool IsHideFiles(CString path);
	void UpdateMenu(CMenu *pMenu);
	bool IsFixedItem(CString path);
	void UpdateRecord();
	CString  GetItemPath( HTREEITEM hItem);
	
	void FreshObjectList(CString strPathName , HTREEITEM hTreeItemp, int iCount);
	HTREEITEM InsertItem(CTreeCtrl *pTree, HTREEITEM hRoot, CString name);
	
	typedef std::map<unsigned long, AString> ID2NameMap;
	ID2NameMap	m_tmplID2Name;
	ID2NameMap	m_extID2Name;
	ID2NameMap	m_recipeID2Name;
	ID2NameMap	m_configID2Name;
	ID2NameMap	m_faceID2Name;
	bool					m_bNeedReInitSearch;
	ID2NameMap & GetID2NameMapByFindType(CDlgFindTemplate::FIND_TYPE type);
	void	ReInitSearch();
	void	ReInitSearchImpl(bool bUpdateSearchFile);
	void	AddSearch(const AString &strPathName, CDlgFindTemplate::FIND_TYPE type);
	void	AddSearch(const AString &strPathName, ID2NameMap &id2name);
	bool	LoadSearchFromFile();
	void	SaveSearchToFile();
	void	AddToSearchFile(const AString &strPathName, CDlgFindTemplate::FIND_TYPE type);
	bool	IsCanOpen(unsigned long uID, CDlgFindTemplate::FIND_TYPE type);

	HTREEITEM Open(unsigned long uID, CDlgFindTemplate::FIND_TYPE type);
	bool Find(CString name, CDlgFindTemplate::FIND_TYPE type, CDlgFindTemplResult::FindResult &result, CString &strColumnName);

// Dialog Data
	//{{AFX_DATA(CListDlg)
	enum { IDD = IDD_DIALOG_LIST };
	CMultiTree	m_Tree;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnumCheck(HTREEITEM root, bool bCheckOut, bool bAppend);
	void EnumSave(HTREEITEM root);
	CImageList m_ImageList;
	bool bRclick;
	// Generated message map functions
	//{{AFX_MSG(CListDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCreateObject();
	afx_msg void OnClickTreeProperty(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkTreeProperty(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCreateObjectExt();
	afx_msg void OnUpdateCreateObjectExt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateObject(CCmdUI* pCmdUI);
	afx_msg void OnVssCheckIn();
	afx_msg void OnVssCheckOut();
	afx_msg void OnSelchangedTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditTemplateProperty();
	afx_msg void OnDestroy();
	afx_msg void OnModifiedObjectExt();
	virtual void OnOK(){};
	virtual void OnCancel(){};
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVssCheckInAll();
	afx_msg void OnVssCheckOutAll();
	afx_msg void OnClickTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedTreeSortList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVssUndoCheckOut();
	afx_msg void OnUpdateVssUndoCheckOut(CCmdUI* pCmdUI);
	afx_msg void OnAutoResaveTemplate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTDLG_H__E0AE7C8F_409F_405F_9E3E_D2FC05605528__INCLUDED_)
