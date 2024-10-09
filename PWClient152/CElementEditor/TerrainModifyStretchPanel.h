#if !defined(AFX_TERRAINMODIFYSTRETCHPANEL_H__299077AF_B0C4_48BE_AF09_C2068A35B8AE__INCLUDED_)
#define AFX_TERRAINMODIFYSTRETCHPANEL_H__299077AF_B0C4_48BE_AF09_C2068A35B8AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainModifyStretchPanel.h : header file
//
#include "CurveFilter.h"
#include "NumEdit.h"
//#define new A_DEBUG_NEW
/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyStretchPanel dialog

class CTerrainModifyStretchPanel : public CDialog
{
// Construction
public:
	CTerrainModifyStretchPanel(CWnd* pParent = NULL);   // standard constructor
	
	void UpdateCurrentBrushHeight(float fHeight);
	void UpdateBrushHeightRect();
	// Dialog Data
	//{{AFX_DATA(CTerrainModifyStretchPanel)
	enum { IDD = IDD_TERRAIN_MODIFY_STRETCH };
	CButton	m_checkLockEdge;
	CNumEdit	m_editNoiseRadius;
	CNumEdit	m_editNoisePersistence;
	CSpinButtonCtrl	m_spinNoiseRadius;
	CSpinButtonCtrl	m_spinNosizePersistence;
	CComboBox	m_comboStrecthType;
	CNumEdit	m_editBrushHeight;
	CNumEdit	m_editBrushHard;
	CSpinButtonCtrl	m_spinBrushHeight;
	CSpinButtonCtrl	m_spinBrushHard;
	CSliderCtrl	m_sliderBrushHeight;
	CSliderCtrl	m_sliderBrushHard;
	CString	m_strBrushName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifyStretchPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BYTE *m_pData;//This a user define's brush data;
	int  m_nBrushWidth;
	int  m_nBrushHeight;

	int m_nHeight;
	virtual void OnOK(){};
	virtual void OnCancel() {};
	void ShowSmoothCtrl(bool bSmooth);
	void ShowHeightCtrl(bool bShow);
	// Generated message map functions
	//{{AFX_MSG(CTerrainModifyStretchPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditBrushhard();
	afx_msg void OnChangeEditBrushheight();
	afx_msg void OnReleasedcaptureSliderBrushhard(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderBrushheight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBrushhard(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBrushheight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioBrushflat();
	afx_msg void OnRadioBrushsmooth();
	afx_msg void OnSelchangeComboStretchType();
	afx_msg void OnDeltaposSpinStretchNoisePersistence(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinStretchNoiseRadius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditStretchNoisePersistence();
	afx_msg void OnChangeEditStretchNoiseRadius();
	afx_msg void OnRadioHeightsub();
	afx_msg void OnRadioHeightadd();
	afx_msg void OnCheckEnablenoise();
	afx_msg void OnCheckLockMapEdge();
	afx_msg void OnButtonSelBrush();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINMODIFYSTRETCHPANEL_H__299077AF_B0C4_48BE_AF09_C2068A35B8AE__INCLUDED_)
