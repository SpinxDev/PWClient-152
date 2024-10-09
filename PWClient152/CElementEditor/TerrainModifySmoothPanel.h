#if !defined(AFX_TERRAINMODIFYSMOOTHPANEL_H__5E8D6FE9_B825_4DA6_9ACB_DDE4DEE5294C__INCLUDED_)
#define AFX_TERRAINMODIFYSMOOTHPANEL_H__5E8D6FE9_B825_4DA6_9ACB_DDE4DEE5294C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainModifySmoothPanel.h : header file
//
#include "NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CTerrainModifySmoothPanel dialog

class CTerrainModifySmoothPanel : public CDialog
{
// Construction
public:
	CTerrainModifySmoothPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainModifySmoothPanel)
	enum { IDD = IDD_TERRAIN_MODIFY_SMOOTH };
	CButton	m_checkLockEdge;
	CSpinButtonCtrl	m_spinBrushSmooth;
	CSliderCtrl	m_sliderBrushSmooth;
	CNumEdit	m_editBrushSmooth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifySmoothPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){};
	virtual void OnCancel() {};
	// Generated message map functions
	//{{AFX_MSG(CTerrainModifySmoothPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditBrushsmooth();
	afx_msg void OnReleasedcaptureSliderBrushsmooth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBrushsmooth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckLockMapEdge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINMODIFYSMOOTHPANEL_H__5E8D6FE9_B825_4DA6_9ACB_DDE4DEE5294C__INCLUDED_)
