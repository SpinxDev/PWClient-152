// StatusProgress.h : header file
//
#include "StatusControl.h"
/////////////////////////////////////////////////////////////////////////////
// CStatusProgress window

class CStatusProgress : public CProgressCtrl
{
// Construction
public:
	CStatusProgress();
	BOOL Create(CStatusBar * parent, UINT id, DWORD style);
// Attributes
public:

// Operations
public:
	__inline void Reposition() { CStatusControl::reposition(this); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusProgress)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusProgress();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusProgress)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
