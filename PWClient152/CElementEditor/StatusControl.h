// StatusControl.h : header file
//
#ifndef _STATUS_CONTROL_
#define _STATUS_CONTROL_
/////////////////////////////////////////////////////////////////////////////
// CStatusControl window

class CStatusControl : public CWnd
{
// Construction
public:
    friend class CStatusEdit;
	friend class CStatusProgress;
	friend class CStatusStatic;
	friend class CStatusCombo;
	CStatusControl();
	BOOL Create(LPCTSTR classname, CStatusBar * parent, UINT id, DWORD style);

// Attributes
public:

// Operations
public:
	void Reposition();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusIcon)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusControl();

	// Generated message map functions
protected:
	static void reposition(CWnd * wnd);
	static BOOL setup(CStatusBar * parent, UINT id, CRect & r);
	//{{AFX_MSG(CStatusControl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
