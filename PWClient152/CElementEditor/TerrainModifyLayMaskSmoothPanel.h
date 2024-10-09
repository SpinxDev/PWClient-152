#if !defined(AFX_TERRAINMODIFYLAYMASKSMOOTHPANEL_H__DA200A3F_D0B4_4A43_A073_6815E2740701__INCLUDED_)
#define AFX_TERRAINMODIFYLAYMASKSMOOTHPANEL_H__DA200A3F_D0B4_4A43_A073_6815E2740701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainModifyLayMaskSmoothPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskSmoothPanel dialog

class CTerrainModifyLayMaskSmoothPanel : public CDialog
{
// Construction
public:
	CTerrainModifyLayMaskSmoothPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainModifyLayMaskSmoothPanel)
	enum { IDD = IDD_TERRAIN_MODIFY_LAYMASKSMOOTH };
	CSpinButtonCtrl	m_spinLayMaskSmooth;
	CSliderCtrl	m_sliderLayMaskSmooth;
	CNumEdit	m_editLayMaskSmooth;
	CListBox	m_listLayMaskSmooth;
	//}}AFX_DATA
	void UpdateLayList();
	void SetMaskLay(char* szLayName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifyLayMaskSmoothPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTerrainModifyLayMaskSmoothPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTerrainModifyLaylistsmooth();
	afx_msg void OnChangeEditLaymasksmooth();
	afx_msg void OnReleasedcaptureSliderLaymasksmooth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLaymasksmooth(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINMODIFYLAYMASKSMOOTHPANEL_H__DA200A3F_D0B4_4A43_A073_6815E2740701__INCLUDED_)
