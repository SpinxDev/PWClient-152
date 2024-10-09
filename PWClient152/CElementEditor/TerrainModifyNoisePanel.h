#if !defined(AFX_TERRAINMODIFYNOISEPANEL_H__ABC22DB1_F888_469D_BD45_1E2FED757497__INCLUDED_)
#define AFX_TERRAINMODIFYNOISEPANEL_H__ABC22DB1_F888_469D_BD45_1E2FED757497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainModifyNoisePanel.h : header file
//
#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyNoisePanel dialog

class CTerrainModifyNoisePanel : public CDialog
{
// Construction
public:
	CTerrainModifyNoisePanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainModifyNoisePanel)
	enum { IDD = IDD_TERRAIN_MODIFY_NOISE };
	CButton	m_checkLockEdge;
	CNumEdit	m_editDelta;
	CSpinButtonCtrl	m_spinDelta;
	CSpinButtonCtrl	m_spinPersistence;
	CNumEdit	    m_editPersistence;
	CSpinButtonCtrl	m_spinNoiseRadius;
	CNumEdit	    m_editNoiseRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifyNoisePanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){};
	virtual void OnCancel() {};
	// Generated message map functions
	//{{AFX_MSG(CTerrainModifyNoisePanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditNoisepersistence();
	afx_msg void OnChangeEditNoiseradius();
	afx_msg void OnDeltaposSpinNoiseoctavenum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinNoisepersistence(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinNoiseradius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinNoisewavelen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboNoiseType();
	afx_msg void OnDeltaposSpinNoisedelta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditNoisedelta();
	afx_msg void OnCheckLockMapEdge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINMODIFYNOISEPANEL_H__ABC22DB1_F888_469D_BD45_1E2FED757497__INCLUDED_)
