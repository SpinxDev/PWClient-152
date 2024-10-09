// StatusStatic.h : header file
//
#include "StatusControl.h"
/////////////////////////////////////////////////////////////////////////////
// CStatusStatic window

class CStatusStatic : public CStatic
{
// Construction
public:
	CStatusStatic();
	BOOL Create(CStatusBar * parent, UINT id, DWORD style);
// Attributes
public:

// Operations
public:
	__inline void Reposition() { CStatusControl::reposition(this); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
