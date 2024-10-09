#if !defined(AFX_BRUSHSETPANEL_H__424DD45F_39E3_410A_BA76_6ABDD0F5038C__INCLUDED_)
#define AFX_BRUSHSETPANEL_H__424DD45F_39E3_410A_BA76_6ABDD0F5038C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrushSetPanel.h : header file
//

#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CBrushSetPanel dialog

class CBrushSetPanel : public CDialog
{
// Construction
public:
	CBrushSetPanel(CWnd* pParent = NULL);   // standard constructor
	void UpdateUI();

// Dialog Data
	//{{AFX_DATA(CBrushSetPanel)
	enum { IDD = IDD_BRUSH_SET };
	CSpinButtonCtrl	m_spinBrushRadius;
	CNumEdit	m_editBrushRadius;
	CSliderCtrl	m_sliderBrushRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushSetPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void OnOK();
	virtual void OnCancel() {}
	// Generated message map functions
	//{{AFX_MSG(CBrushSetPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBrushradius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditBrushradius();
	afx_msg void OnReleasedcaptureSliderBrushradius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutofmemorySliderBrushradius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioBrushcircle();
	afx_msg void OnRadioBrushrect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSHSETPANEL_H__424DD45F_39E3_410A_BA76_6ABDD0F5038C__INCLUDED_)
