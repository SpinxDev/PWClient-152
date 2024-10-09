// EleDataManDoc.h : interface of the CEleDataManDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEDATAMANDOC_H__BA379B69_7663_497A_A05A_6E1A9CD5FEA1__INCLUDED_)
#define AFX_ELEDATAMANDOC_H__BA379B69_7663_497A_A05A_6E1A9CD5FEA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEleDataManDoc : public CDocument
{
protected: // create from serialization only
	CEleDataManDoc();
	DECLARE_DYNCREATE(CEleDataManDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEleDataManDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEleDataManDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int  SearchTest(unsigned int times);
	float  GenerateTest(unsigned int times);
protected:
	void Initialize(elementdataman& dataman);

	unsigned int m_nNumElementData;

// Generated message map functions
protected:
	//{{AFX_MSG(CEleDataManDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEDATAMANDOC_H__BA379B69_7663_497A_A05A_6E1A9CD5FEA1__INCLUDED_)
