#if !defined(AFX_CLOUDSETPANEL_H__89EA838E_5C4E_4211_8095_A00DE418701C__INCLUDED_)
#define AFX_CLOUDSETPANEL_H__89EA838E_5C4E_4211_8095_A00DE418701C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloudSetPanel.h : header file
//
#include "SelColorWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CCloudSetPanel dialog

class CCloudSetPanel : public CDialog
{
// Construction
public:
	CCloudSetPanel(CWnd* pParent = NULL);   // standard constructor
	void UpdateCloudData();
// Dialog Data
	//{{AFX_DATA(CCloudSetPanel)
	enum { IDD = IDD_CLOUD_SET_DLG };
	CComboBox	m_cbGroup;
	CSelColorWnd	m_clrWnd5;
	CSelColorWnd	m_clrWnd4;
	CSelColorWnd	m_clrWnd3;
	CSelColorWnd	m_clrWnd2;
	CSelColorWnd	m_clrWnd1;
	float	m_fDist;
	int		m_nNum;
	float	m_fSize;
	float	m_fHeight1;
	float	m_fHeight2;
	float	m_fHeight3;
	float	m_fHeight4;
	float	m_fHeight5;
	UINT	m_dwSpriteNum;
	float	m_fGroupSize;
	//}}AFX_DATA
	float sr[5];
	float sg[5];
	float sb[5];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloudSetPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nType;
	// Generated message map functions
	//{{AFX_MSG(CCloudSetPanel)
	afx_msg void OnButtonRecreateCloud();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCloudType();
	afx_msg void OnOutofmemorySpinSpriteNum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutofmemorySpinSpriteSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutofmemorySpinSpriteDist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinSpriteDist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinSpriteNum(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinSpriteSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRecreateGroup();
	afx_msg void OnSelchangeComboCloudGroup();
	afx_msg void OnButtonRecreateAllgroup();
	//}}AFX_MSG
	afx_msg HRESULT OnColorChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOUDSETPANEL_H__89EA838E_5C4E_4211_8095_A00DE418701C__INCLUDED_)
