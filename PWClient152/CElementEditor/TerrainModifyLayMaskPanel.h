#if !defined(AFX_CTerrainModifyLayMaskPanel_H__22EA7C69_A362_442B_A01B_390F16659833__INCLUDED_)
#define AFX_CTerrainModifyLayMaskPanel_H__22EA7C69_A362_442B_A01B_390F16659833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTerrainModifyLayMaskPanel.h : header file
//
#include "NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskPanel dialog
class CTerrainModifyLayMaskPanel : public CDialog
{
// Construction
public:
	CTerrainModifyLayMaskPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainModifyLayMaskPanel)
	enum { IDD = IDD_TERRAIN_MODIFY_LAYMASK };
	CListBox	m_listLayMask;
	CSpinButtonCtrl	m_spinBrushHard;
	CSpinButtonCtrl	m_spinBrushGray;
	CSliderCtrl	m_sliderBrushHard;
	CSliderCtrl	m_sliderBrushGray;
	CNumEdit	m_editBrushHard;
	CNumEdit	m_editBrushGray;
	CString	m_strUserBrushName;
	//}}AFX_DATA

	void UpdateLayList();
	void SetMaskLay(char* szLayName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainModifyLayMaskPanel)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	BYTE *m_pData;//This a user define's brush data;
	int  m_nWidth;
	int  m_nHeight;
	bool m_bUserBrush;
// Implementation
protected:
	virtual void OnOK(){};
	virtual void OnCancel() {};
	// Generated message map functions
	//{{AFX_MSG(CTerrainModifyLayMaskPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureSliderBrushhgray(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderBrushlayhard(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBrushhgray(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinBrushlayhard(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditBrushhgray();
	afx_msg void OnChangeEditBrushlayhard();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeTerrainModifyLaylist();
	afx_msg void OnFreshLayerList();
	afx_msg void OnRadioMaskpaintClrsub();
	afx_msg void OnRadioMaskpaintClradd();
	afx_msg void OnCheckUserBrush();
	afx_msg void OnLoadBrushRaw();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTerrainModifyLayMaskPanel_H__22EA7C69_A362_442B_A01B_390F16659833__INCLUDED_)
