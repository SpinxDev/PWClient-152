#if !defined(AFX_STATUSCOMBO_H__04B11A87_B5B7_11D2_8398_26DCA8000000__INCLUDED_)
#define AFX_STATUSCOMBO_H__04B11A87_B5B7_11D2_8398_26DCA8000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusCombo.h : header file
//
#include "StatusControl.h"
/////////////////////////////////////////////////////////////////////////////
// CStatusCombo window

class CStatusCombo : public CComboBox
{
// Construction
public:
	CStatusCombo();
	BOOL Create(CStatusBar * parent, UINT id, DWORD style);
// Attributes
public:

// Operations
public:
   __inline void Reposition() { CStatusControl::reposition(this); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusCombo();
	// maxlen limits the number of items that can be displayed in the combo box
	// before a scrollbar is needed. It will adjust the dropdown to pop upwards
	// or downwards to hold the maximum possible. If this value is set to
	// 0, the combo box is limited by the screen size
	// If it is set to -1, there is no limit to the combo box.
	int maxlen;
	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusCombo)
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSCOMBO_H__04B11A87_B5B7_11D2_8398_26DCA8000000__INCLUDED_)
