// TaskTemplEditorView.h : interface of the CTaskTemplEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKTEMPLEDITORVIEW_H__72E0FE10_4F4D_4380_9755_F8FB934B5731__INCLUDED_)
#define AFX_TASKTEMPLEDITORVIEW_H__72E0FE10_4F4D_4380_9755_F8FB934B5731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)
#include "TaskTempl.h"
#include "TaskPropertySheet.h"
#include "TaskPropertyPage.h"
#include "TaskPremisePage.h"
#include "TaskMethodPage.h"
#include "TaskAwardPage.h"
#include "TaskClassSheet.h"
#include "TaskClassPage.h"
#include "MainFrm.h"
#include "TaskTemplEditorDoc.h"
#include <AString.h>
#include <hashmap.h>
struct  TASK_CACHE
{
	ATaskTempl* Task;
	int         TaskID;
	HTREEITEM   Item;
	AString		File;
	AString		Path;
	int			Mask;//0: Unload  1: Succ -1: Error		
	bool		Mutex;
	bool		Delete;
	int         Index;
};
typedef abase::hash_map<HTREEITEM,TASK_CACHE> TaskCacheMap;
typedef abase::hash_map<HTREEITEM, ATaskTempl*> SelectedTempls;

class CTaskTemplEditorView : public CFormView
{
protected: // create from serialization only
	CTaskTemplEditorView();
	DECLARE_DYNCREATE(CTaskTemplEditorView)

public:
	//{{AFX_DATA(CTaskTemplEditorView)
	enum { IDD = IDD_TASKTEMPLEDITOR_FORM };
	CTreeCtrl	m_TemplClassTree;
	CTreeCtrl	m_TaskTree;
	//}}AFX_DATA
	SelectedTempls m_SelectedTempls;

// Attributes
public:
	CTaskTemplEditorDoc* GetDocument();
	CTaskPropertySheet		m_Sheet;
	CTaskClassSheet			m_ClassSheet;
	CTaskPropertyPage		m_PropertyPage;
	CTaskPremisePage		m_PremisePage;
	CTaskMethodPage			m_MethodPage;
	CTaskAwardPage			m_AwardPage;
	CTaskClassPage			m_ClassPage;
	CImageList				m_ImageList;
	HTREEITEM				m_ShiftStartItem;

	int						m_iKeyTaskCount;

// Operations
protected:
	ATaskTempl* CreateNewTask(BOOL bTop);

public:
	void UpdateTaskTree();
	void UpdateTaskTreeFromPack();
	void UpdateTemplClassTree();
	void InitTaskTree();
	ATaskTempl* LoadTaskTempl(AString strFn,HTREEITEM hItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskTemplEditorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskTemplEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void UpdateSheet(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly = FALSE);
	HTREEITEM UpdateTaskTree(HTREEITEM hParent, ATaskTempl* pTask,bool bCalcTask = true);
	void ClearTaskTree();
	void ClearTaskTree(HTREEITEM hItem, BOOL bClearSelf = TRUE);
	HTREEITEM FindTask(HTREEITEM hParent, const CString& strTask, bool bByName);
	HTREEITEM FindTaskByID(HTREEITEM hParent, unsigned long ulID);
	HTREEITEM FindNext(HTREEITEM hPrev, const CString& strTask, bool bByName);
	bool CompareTask(HTREEITEM h, const CString& strTask, bool bByName);
	void ExportDescript(HTREEITEM hItem, FILE* fp);
	HTREEITEM SetTaskTreeItemData(HTREEITEM hItem,ATaskTempl* pTask);
	HTREEITEM InitTaskTree(HTREEITEM hParent, ATaskTempl* pTask);
	void ClearTreeData();
	void ReleaseTreeTopMemory(HTREEITEM hItem);
	HTREEITEM FindTask2(HTREEITEM hParent, const CString& strTask);

// Generated message map functions
public:
	//{{AFX_MSG(CTaskTemplEditorView)
	afx_msg void OnRclickTaskTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTaskTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApply();
	afx_msg void OnCancel();
	afx_msg void OnAppAbout();
	afx_msg void OnNewTask();
	afx_msg void OnDestroy();
	afx_msg void OnExport();
	afx_msg void OnView();
	afx_msg void OnUpdateView(CCmdUI* pCmdUI);
	afx_msg void OnFindTask();
	afx_msg void OnTestPack();
	afx_msg void OnExportDescript();
	afx_msg void OnClassMode();
	afx_msg void OnUpdateClassMode(CCmdUI* pCmdUI);
	afx_msg void OnRclickTaskTemplClass(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTaskTemplClass(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApplyClass();
	afx_msg void OnTaskLink();
	afx_msg void OnUpdateSaveall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewTask(CCmdUI* pCmdUI);
	afx_msg void OnFindNext();
	afx_msg void OnFindName();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCheckNpc();
	afx_msg void OnFindById();
	afx_msg void OnTestDynPack();
	afx_msg void OnTestExportDyn();
	afx_msg void OnViewNpcInfo();
	afx_msg void OnEditNpcInfo();
	afx_msg void OnExportNpcInfo();
	afx_msg void OnRecord();
	afx_msg void OnFindById2();
	afx_msg void OnFindName2();
	afx_msg void OnFindNext2();
	afx_msg void OnUpdateExport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportDescript(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTaskLink(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNpcInfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditNpcInfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportNpcInfo(CCmdUI* pCmdUI);
	afx_msg void OnLoadFromExcel();
	//}}AFX_MSG
	afx_msg void OnInsertTask();
	afx_msg void OnAddSubTask();
	afx_msg void OnDelSubTask();
	afx_msg void OnSaveTask();
	afx_msg void OnSaveAll();
	afx_msg void OnUpdateSaveTask(CCmdUI* pCmdUI);
	afx_msg void OnCheckIn();
	afx_msg void OnCheckOut();
	afx_msg void OnRefresh();
	afx_msg void OnCreateClassTask();
	afx_msg void OnCheckInClass();
	afx_msg void OnCheckOutClass();
	DECLARE_MESSAGE_MAP()

public:
	void OnLoadSucc (HTREEITEM hItem);
	void OnLoadError(HTREEITEM hItem);

};

#ifndef _DEBUG  // debug version in TaskTemplEditorView.cpp
inline CTaskTemplEditorDoc* CTaskTemplEditorView::GetDocument()
   { return (CTaskTemplEditorDoc*)m_pDocument; }
#endif

inline CTaskTemplEditorView* AfxGetEditorView()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (pMain == NULL) return NULL;
	return (CTaskTemplEditorView*) pMain->GetActiveView();
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKTEMPLEDITORVIEW_H__72E0FE10_4F4D_4380_9755_F8FB934B5731__INCLUDED_)
