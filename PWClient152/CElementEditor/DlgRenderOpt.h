#if !defined(AFX_DLGRENDEROPT_H__4547418F_C69D_4A01_9758_9677CBA99A46__INCLUDED_)
#define AFX_DLGRENDEROPT_H__4547418F_C69D_4A01_9758_9677CBA99A46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRenderOpt.h : header file
//

#include "NumEdit.h"
#include "SelColorWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRenderOpt dialog

class CDlgRenderOpt : public CDialog
{
// Construction
public:
	CDlgRenderOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRenderOpt)
	enum { IDD = IDD_RENDEROPT };
	CComboBox	m_cbCopySel;
	CComboBox	m_WeightCombo;
	float	m_fViewRadius;
	BOOL	m_bImmeMaskPaint;
	//}}AFX_DATA

	//	Initlaize device objects
	bool InitDeviceObjects();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRenderOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//	Override OnOK and OnCanel to prevent dialog to be closed when
	//	Enter and ESC are pressed
	virtual void OnOK();
	virtual void OnCancel() {}

	void UpdateShowOptionsCheck();

	// Generated message map functions
	//{{AFX_MSG(CDlgRenderOpt)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckImmemaskpaint();
	afx_msg void OnCheckRenderSky();
	afx_msg void OnSelchangeComboWeight();
	afx_msg void OnCheckRenderShowName();
	afx_msg void OnKillfocusEditViewradius();
	afx_msg void OnChangeEditViewradius();
	afx_msg void OnCheckShowStaticobject();
	afx_msg void OnCheckShowLight();
	afx_msg void OnCheckShowAudio();
	afx_msg void OnCheckShowBoxarea();
	afx_msg void OnCheckShowDummy();
	afx_msg void OnCheckShowSpecially();
	afx_msg void OnCheckShowWater();
	afx_msg void OnCheckShowAiarea();
	afx_msg void OnCheckShowNPCPath();
	afx_msg void OnCheckShowNonNPCPath();
	afx_msg void OnCheckShowSkinmodel();
	afx_msg void OnCheckShowCritterGroup();
	afx_msg void OnCheckRenderEnableFog();
	afx_msg void OnCheckShowFixedNpc();
	afx_msg void OnCheckShowTerrainBlock();
	afx_msg void OnCheckShowTree();
	afx_msg void OnCheckShowGrass();
	afx_msg void OnCheckShowGather();
	afx_msg void OnCheckShowTerrain();
	afx_msg void OnCheckShowRange();
	afx_msg void OnCheckShowCloud();
	afx_msg void OnCheckShowCloudLine();
	afx_msg void OnCheckRenderSampleWater();
	afx_msg void OnCheckShowInstanceBox();
	afx_msg void OnSelchangeComboCopy();
	afx_msg void OnCheckShowDynamic();
	afx_msg void OnCheckShowMonsterView();
	afx_msg void OnButtonSwitchShowAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRENDEROPT_H__4547418F_C69D_4A01_9758_9677CBA99A46__INCLUDED_)
