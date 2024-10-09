// TaskTemplEditorDoc.h : interface of the CTaskTemplEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKTEMPLEDITORDOC_H__2B0CC989_CDF8_4CBE_A9E9_0E5EF4B500B7__INCLUDED_)
#define AFX_TASKTEMPLEDITORDOC_H__2B0CC989_CDF8_4CBE_A9E9_0E5EF4B500B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTaskTemplEditorDoc : public CDocument
{
protected: // create from serialization only
	CTaskTemplEditorDoc();
	DECLARE_DYNCREATE(CTaskTemplEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskTemplEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskTemplEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTaskTemplEditorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKTEMPLEDITORDOC_H__2B0CC989_CDF8_4CBE_A9E9_0E5EF4B500B7__INCLUDED_)
