#if !defined(AFX_DLGOPTIONS_H__5407833F_4611_4D1F_A673_02D66E793CCE__INCLUDED_)
#define AFX_DLGOPTIONS_H__5407833F_4611_4D1F_A673_02D66E793CCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOptions.h : header file
//

#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions dialog

class CDlgOptions : public CDialog
{
// Construction
public:
	CDlgOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOptions)
	enum { IDD = IDD_OPTIONS };
	CSliderCtrl	m_sliderSfx;
	CSliderCtrl	m_sliderMusic;
	CNumEdit	m_CamRotEdit;
	CNumEdit	m_CamMoveEdit;
	CSpinButtonCtrl	m_CamRotSpin;
	CSpinButtonCtrl	m_CamMoveSpin;
	BOOL	m_bAlongGround;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//	Override OnOK and OnCanel to prevent dialog to be closed when
	//	Enter and ESC are pressed
	virtual void OnOK();
	virtual void OnCancel() {}

	void UpdatePickCheck();

	// Generated message map functions
	//{{AFX_MSG(CDlgOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinCameramove(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinCamerarot(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckAlongground();
	afx_msg void OnCheckObjectAddContinue();
	afx_msg void OnKillfocusEditCameramove();
	afx_msg void OnKillfocusEditCamerarot();
	afx_msg void OnCheckPickStaticobject();
	afx_msg void OnCheckPickLight();
	afx_msg void OnCheckPickBoxarea();
	afx_msg void OnCheckPickDummy();
	afx_msg void OnCheckPickSpecially();
	afx_msg void OnCheckPickWater();
	afx_msg void OnCheckPickAiarea();
	afx_msg void OnCheckPickNPCPath();
	afx_msg void OnCheckPickNonNPCPath();
	afx_msg void OnCheckPickAudio();
	afx_msg void OnCheckPickCritter();
	afx_msg void OnCheckPickSkinModel();
	afx_msg void OnCheckPickFixednpc();
	afx_msg void OnCheckShowHull();
	afx_msg void OnCheckShowHull2();
	afx_msg void OnCheckPickPrecinct();
	afx_msg void OnCheckPickGather();
	afx_msg void OnCheckBezierPtSnap();
	afx_msg void OnCheckPickRange();
	afx_msg void OnCheckPickCloudBox();
	afx_msg void OnCheckPickInstanceBox();
	afx_msg void OnReleasedcaptureSliderMusic(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderSfx(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckPickDynamic();
	afx_msg void OnButtonSwitchPickAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTIONS_H__5407833F_4611_4D1F_A673_02D66E793CCE__INCLUDED_)
