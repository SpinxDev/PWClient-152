#if !defined(AFX_DLGSURFACES_H__48BB626F_0068_4AFA_8C11_B3050D47E544__INCLUDED_)
#define AFX_DLGSURFACES_H__48BB626F_0068_4AFA_8C11_B3050D47E544__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSurfaces.h : header file
//

#include "IconButton.h"
#include "NumEdit.h"
#include "ImageWnd.h"
#include "WndInput.h"

class CTerrain;
class CTerrainLayer;
class CELBitmap;

/////////////////////////////////////////////////////////////////////////////
// CDlgSurfaces dialog

class CDlgSurfaces : public CDialog
{
public:		//	Types

	enum
	{
		SIZE_512,
		SIZE_1024,
		SIZE_2048,
		SIZE_4096,
		SIZE_8192,
		NUM_SIZE,
	};

// Construction
public:
	CDlgSurfaces(CTerrain* pTerrain, CWnd* pParent = NULL);   // standard constructor

public:		//	Attributes

public:		//	Operations

	//	Initlaize layers
	void InitLayers(int iCurLayer);

	//	Get modified flag
	bool GetModifiedFlag() { return m_bModified; }
	//	Get layer add or delete flag
	bool GetLayerAddDelFlag() { return m_bAddDelLayer ? true : false; }

// Dialog Data
	//{{AFX_DATA(CDlgSurfaces)
	enum { IDD = IDD_SURFACES };
	CComboBox	m_WeightCombo;
	CIconButton	m_DelSpecuMapBtn;
	CComboBox	m_MaskSizeCombo;
	CIconButton	m_MULayerBtn;
	CIconButton	m_MDLayerBtn;
	CSpinButtonCtrl	m_VScaleSpin;
	CSpinButtonCtrl	m_UScaleSpin;
	CNumEdit	m_VScaleEdit;
	CNumEdit	m_UScaleEdit;
	CNumEdit	m_AltiStartEdit;
	CNumEdit	m_AltiEndEdit;
	CListBox	m_LayerList;
	CComboBox	m_ProjAxisCombo;
	CNumEdit	m_SlopeStartEdit;
	CNumEdit	m_SlopeEndEdit;
	CIconButton	m_AddLayerBtn;
	CIconButton	m_DelLayerBtn;
	BOOL	m_bEnable;
	BOOL	m_bAutoMask;
	BOOL	m_bSmooth;
	BOOL	m_bMaskAxisX;
	BOOL	m_bMaskAxisY;
	BOOL	m_bMaskAxisZ;
	CString	m_strTexturePath1;
	CString	m_strTexturePath2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSurfaces)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTerrain*		m_pTerrain;
	CImageWnd		m_TextureWnd;	//	Texture window
	CImageWnd		m_SpecuMapWnd;	//	Specular map window
	CImageWnd		m_MaskWnd;		//	Mask preview window
	CWndInput		m_InputWnd;		//	Window used to input data
	bool			m_bModified;	//	Modified flag
	CELBitmap*		m_pTexture;		//	Texture map
	CELBitmap*		m_pSpecuMap;	//	Specular map
	int				m_bAddDelLayer;	//	Some layers has been added or deleted

protected:

	//	Change current selected layer
	void ChangeLayer(int iLayer);
	//	Get selected layer
	CTerrainLayer* GetSelLayer();
	//	Enable controls base on auto mask flag
	void EnableAutoMaskControls();
	//	Accept edit box value
	void AcceptEditBoxValue(int iEditID);
	//	Set mask view title
	void SetMaskViewTitle();
	//	Change mask map
	bool ChangeMaskMap(const char* szFile);
	//	Change texture
	bool ChangeTexture(const char* szFile);
	//	Release texture
	void ReleaseTexture();
	//	Change specular map
	bool ChangeSpecularMap(const char* szFile);
	//	Release specular map
	void ReleaseSpecularMap();
	//	Update auto mask map
	void UpdateAutoMaskMap(CTerrainLayer* pLayer, int iNewSize);
	//	On check auto mask axis
	void OnCheckAutoMaskAxis(int iAxis, BOOL& bValue);

	virtual void OnCancel();
	
	bool CheckLayerValid();

	// Generated message map functions
	//{{AFX_MSG(CDlgSurfaces)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnLoadtexture();
	afx_msg void OnCheckAutogenmask();
	afx_msg void OnSelchangeListLayers();
	afx_msg void OnBtnAddlayer();
	afx_msg void OnBtnDellayer();
	afx_msg void OnBtnMovednlayer();
	afx_msg void OnBtnMoveuplayer();
	afx_msg void OnBtnImportmask();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinUscale(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinVscale(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboProjaxis();
	afx_msg void OnDoubleclickedBtnMovednlayer();
	afx_msg void OnDoubleclickedBtnMoveuplayer();
	afx_msg void OnDestroy();
	afx_msg void OnBtnExportmask();
	afx_msg void OnCheckEnable();
	afx_msg void OnCheckSmooth();
	afx_msg void OnSelchangeComboAutomasksize();
	afx_msg void OnCheckMaskaxisX();
	afx_msg void OnCheckMaskaxisY();
	afx_msg void OnCheckMaskaxisZ();
	afx_msg void OnDblclkListLayers();
	afx_msg void OnBtnLoadspecumap();
	afx_msg void OnBtnDelspecumap();
	afx_msg void OnSufFileLoad();
	afx_msg void OnSufFileSave();
	afx_msg void OnSelchangeComboWeight();
	afx_msg void OnKillfocusEditAltiend();
	afx_msg void OnKillfocusEditAltistart();
	afx_msg void OnKillfocusEditSlopeend();
	afx_msg void OnKillfocusEditSlopestart();
	afx_msg void OnKillfocusEditUscale();
	afx_msg void OnKillfocusEditVscale();
	//}}AFX_MSG

	afx_msg HRESULT OnInputWndEnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSURFACES_H__48BB626F_0068_4AFA_8C11_B3050D47E544__INCLUDED_)
