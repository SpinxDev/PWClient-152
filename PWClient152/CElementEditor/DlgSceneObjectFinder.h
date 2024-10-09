#if !defined(AFX_DLGSCENEOBJECTFINDER_H__763C058F_7E5F_4D50_8631_2E63C708893A__INCLUDED_)
#define AFX_DLGSCENEOBJECTFINDER_H__763C058F_7E5F_4D50_8631_2E63C708893A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSceneObjectFinder.h : header file
//

#include "AList2.h"

class CSceneObject;
class CSceneAIGenerator;
class CSceneFixedNpcGenerator;
class CSceneGatherObject;
class AString;
class CSceneObjectIDNameMatcher;

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneObjectFinder dialog

class CDlgSceneObjectFinder : public CDialog
{
// Construction
public:
	CDlgSceneObjectFinder(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSceneObjectFinder)
	enum { IDD = IDD_SCENE_OBJECT_FINDER };
	CStatic	m_staticCount;
	CEdit	m_editKey;
	CListBox	m_listObject;
	//}}AFX_DATA

	APtrList<CSceneObject *>	m_listSelectedObject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSceneObjectFinder)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	bool CanSelectSceneObject(CSceneObject *pObj, CSceneObjectIDNameMatcher *pMatcher)const;
	bool CanSelect(CSceneAIGenerator *pObj, CSceneObjectIDNameMatcher *pMatcher)const;
	bool CanSelect(CSceneFixedNpcGenerator *pObj, CSceneObjectIDNameMatcher *pMatcher)const;
	bool CanSelect(CSceneGatherObject *pObj, CSceneObjectIDNameMatcher *pMatcher)const;

	void GetSearchKey(CString &strKey)const;
	void SaveSearchKey();
	void LoadLastSearchKey();
	void RebuildSceneObjectList();
	void RefreshSceneObjectCount();

	// Generated message map functions
	//{{AFX_MSG(CDlgSceneObjectFinder)
	afx_msg void OnButtonFind();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCENEOBJECTFINDER_H__763C058F_7E5F_4D50_8631_2E63C708893A__INCLUDED_)
