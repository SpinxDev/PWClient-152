#if !defined(AFX_TERRAINMODIFYPLANTSPANEL_H__9DB337A8_8201_4FCB_AB1F_7856CA04873A__INCLUDED_)
#define AFX_TERRAINMODIFYPLANTSPANEL_H__9DB337A8_8201_4FCB_AB1F_7856CA04873A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainModifyPlantsPanel.h : header file
//
#include "IconButton.h"
#include "NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyPlantsPanel dialog

class CTerrainModifyPlantsPanel : public CDialog
{
// Construction
public:
	CTerrainModifyPlantsPanel(CWnd* pParent = NULL);   // standard constructor
	void FreshList();
	AString *m_pListTreeName;
	int FindTypeByName(AString name);
// Dialog Data
	//{{AFX_DATA(CTerrainModifyPlantsPanel)
	enum { IDD = IDD_TERRAIN_MODIFY_PLANTS };
	CNumEdit	m_editGrassMaxNum;
	CNumEdit	m_ctrlSoft;
	CNumEdit	m_ctrlSizeVar;
	CNumEdit	m_ctrlGrassSize;
	CNumEdit	m_ctrlSightRange;
	CNumEdit	m_ctrlAlphaRef;
	CNumEdit	m_editDensity;
	CButton	    m_btShowPlants;
	CTreeCtrl	m_treePlants;
	CString	m_strName;
	CString	m_strRes;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifyPlantsPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	void EnableGrassProperyCtrl(bool bEnable = true);
	void UpdateProperty(bool bGet = true);
	bool IsPlantMaxNum();
	
	void RaiseAllButton(CButton *pCur);

	int		m_nGrassAlphaRef;
	float	m_fGrassSightRange;
	float	m_fGassSize;
	float	m_fGrassSizeVar;
	float	m_fGrassSoft;
	int     m_nDensity;
	int     m_nGrassNum;

	int m_nNumTypes;
	// Generated message map functions
	//{{AFX_MSG(CTerrainModifyPlantsPanel)
	virtual void OnOK();
	virtual void OnCancel() {};
	virtual BOOL OnInitDialog();
	afx_msg void OnPlantAddSort();
	afx_msg void OnPlantDelete();
	afx_msg void OnPlantDeleteFromTerrain();
	afx_msg void OnDblclkTreePlantList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreePlantList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditPlantsDensity();
	afx_msg void OnDeltaposSpinPlantsDensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlantAddToTerrain();
	afx_msg void OnPlantMoveY();
	afx_msg void OnPlantMoveXz();
	afx_msg void OnDeltaposSpinGrassAlpharef(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGrassSightrange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGrassSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGrassSizevar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGrassSoft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckGrassEnablealpha();
	afx_msg void OnChangeEditPlantName();
	afx_msg void OnTreeTypesAdd();
	afx_msg void OnSelchangedTreePlantList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditGrassAlpharef();
	afx_msg void OnChangeEditGrassSightrange();
	afx_msg void OnChangeEditGrassSize();
	afx_msg void OnChangeEditGrassSizevar();
	afx_msg void OnChangeEditGrassSoft();
	afx_msg void OnKillfocusEditGrassAlpharef();
	afx_msg void OnKillfocusEditGrassSightrange();
	afx_msg void OnKillfocusEditGrassSize();
	afx_msg void OnKillfocusEditGrassSizevar();
	afx_msg void OnKillfocusEditGrassSoft();
	afx_msg void OnKillfocusEditPlantsDensity();
	afx_msg void OnKillfocusEditPlantName();
	afx_msg void OnOutofmemorySpinGrassMaxnum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditGrassMaxNum();
	afx_msg void OnDeltaposSpinGrassMaxnum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonChangeRes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINMODIFYPLANTSPANEL_H__9DB337A8_8201_4FCB_AB1F_7856CA04873A__INCLUDED_)
