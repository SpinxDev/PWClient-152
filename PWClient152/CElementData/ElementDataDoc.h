// ElementDataDoc.h : interface of the CElementDataDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTDATADOC_H__759F3E37_CD6A_46CA_9E62_B803FAE9746A__INCLUDED_)
#define AFX_ELEMENTDATADOC_H__759F3E37_CD6A_46CA_9E62_B803FAE9746A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "PropertyList.h"
#include "AObject.h"
#include "AVariant.h"
#include "alist.h"


class CElementDataDoc : public CDocument
{
protected: // create from serialization only
	CElementDataDoc();
	DECLARE_DYNCREATE(CElementDataDoc)

// Attributes
public:

// Operations
public:
	BaseDataTempl m_Tmpl;
	ExtendDataTempl m_ExtTmpl;
	bool          m_bExt;
	AString       m_strDocPath;
	ADynPropertyObject* m_pProperty;
	CPtrList	m_listTypes;
	CPtrList	m_listCustoms;
	void Explain(CString type,AVariant var,CString name);
	void ExplainEx(CString type,AVariant var,CString name);
	bool EnumFiles(ASetTemplate<int>* pType,AVariant var);
	bool IsModifed();
	bool IsEqual(AString type,AVariant var1,AVariant var2);
	void Save();
	void SetName();
	void Fresh();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementDataDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void DeleteContents();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CElementDataDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString szID;
	bool bInited;
	int  nFlag;

	// Generated message map functions
protected:
	//{{AFX_MSG(CElementDataDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnWindowNew();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTDATADOC_H__759F3E37_CD6A_46CA_9E62_B803FAE9746A__INCLUDED_)
