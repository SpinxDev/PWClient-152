// StatusEdit.h : header file
//

#include "StatusControl.h"

/////////////////////////////////////////////////////////////////////////////
// CStatusEdit window

class CStatusEdit : public CEdit
{
 // Construction
    public:
       CStatusEdit();
       BOOL Create(CStatusBar * parent, UINT id, DWORD style);
// Attributes
    public:

// Operations
    public:
       __inline void Reposition() { CStatusControl::reposition(this); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
