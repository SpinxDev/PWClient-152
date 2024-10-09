#if !defined(AFX_DLGLIGHTING_H__633E3EBD_4D6A_4CA5_89AC_EFCE0ADD9A6C__INCLUDED_)
#define AFX_DLGLIGHTING_H__633E3EBD_4D6A_4CA5_89AC_EFCE0ADD9A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLighting.h : header file
//

#include "Global.h"
#include "BackBufWnd.h"
#include "SelColorWnd.h"
#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CLightingView window

class CLightingView : public CBackBufWnd
{
// Construction
public:
	CLightingView();

// Attributes
public:

// Operations
public:

	//	Create window
	bool Create(const RECT& rc, CWnd* pParent);

	//	Set sun direction
	void SetSunDirAndHeight(int iSunDir, int iHeight);
	//	Get sun degree on xz plane
	int GetSunDir() { return m_iSunDir; }
	//	Get sun height
	int GetSunHeight() { return m_iSunHei; }
	//	Set sun color
	void SetSunColor(DWORD dwCol) { m_dwSunColor = dwCol; }
	//	Set ambient color
	void SetAmbient(DWORD dwAmbient) { m_dwAmbient = dwAmbient; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightingView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLightingView();

// Attributes
protected:

	int		m_iSunDir;		//	Sun degreen on xz plane
	int		m_iSunHei;		//	Sun's height
	DWORD	m_dwSunColor;	//	Sun color
	DWORD	m_dwAmbient;	//	Ambient color
	bool	m_bLeftPress;	//	Left button is pressed

// Operations
protected:

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);

	//	Drag sun direction to specified position
	void DragSunDir(int x, int y);

	// Generated message map functions
protected:
	//{{AFX_MSG(CLightingView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgLighting dialog

class CDlgLighting : public CDialog
{
public:		//	Types

	//	Size
	enum
	{
		SIZE_64 = 0,
		SIZE_128,
		SIZE_256,
		SIZE_512, 
		SIZE_1024,
		SIZE_2048,
		SIZE_4096,
		NUM_SIZE,
	};

	struct INITDATA
	{
		bool		bVecDir;	//	true, using vSunDir; false, using iSunDir and iSunHei
		int			iSunDir;
		int			iSunHei;
		
		A3DVECTOR3	vSunDir;	//	Sun direction
		DWORD		dwAmbient;	//	Ambient color
		DWORD		dwSunCol;	//	Sun color
		int			iMapSize;	//	Light map size (width and height in pixels)
		float       fSunPower;

		A3DVECTOR3	vNightSunDir;	//	Sun direction
		DWORD		dwNightAmbient;	//	Ambient color
		DWORD		dwNightSunCol;	//	Sun color
		int			iNightMapSize;	//	Light map size (width and height in pixels)
		float       fNightSunPower;
	};

// Construction
public:
	CDlgLighting(const INITDATA& InitData, CWnd* pParent = NULL);   // standard constructor

	const LIGHTINGPARAMS& GetParams() { return m_Params; } 
	const LIGHTINGPARAMS& GetNightParams() { return m_NightParams; } 

	//	Set sun direction position
	void SetSunDir(int iSunDir);
	void SetModifyDirOnly( bool bOnlyDir){ m_bOnlyDir = bOnlyDir; };
// Dialog Data
	//{{AFX_DATA(CDlgLighting)
	enum { IDD = IDD_LIGHTING };
	CNumEdit	m_editSunPower;
	CNumEdit	m_editSunHeight;
	CNumEdit	m_editSunDir;
	CNumEdit	m_editSunColor;
	CNumEdit	m_editAmbient;
	CNumEdit	m_editSunDirZ;
	CNumEdit	m_editSunDirY;
	CNumEdit	m_editSunDirX;
	CSelColorWnd	m_SunColWnd;
	CSelColorWnd	m_AmbientWnd;
	CComboBox	m_MapSizeCombo;
	CSliderCtrl	m_SunHeiSlider;
	CSliderCtrl	m_SunDirSlider;
	float	m_fShadowFaction;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLighting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CLightingView	m_LightingView;
	LIGHTINGPARAMS	m_Params;
	LIGHTINGPARAMS  m_NightParams;
	bool            m_bInited;
	bool            m_bIsDay;
	bool            m_bOnlyDir;


	int		iLightMapSize;		//	Light map size
	int		iSunDir;			//	Sun direction
	int		iSunHeight;			//	Sun height
	DWORD	dwSunCol;			//	Sun color
	DWORD	dwAmbient;			//	Ambient color
	float   fSunPower;          //  Light power

	int		iNightLightMapSize;		//	Light map size
	int		iNightSunDir;			//	Sun direction
	int		iNightSunHeight;			//	Sun height
	DWORD	dwNightSunCol;			//	Sun color
	DWORD	dwNightAmbient;			//	Ambient color
	float   fNightSunPower;          //  Light power

protected:

	//	Update sun direction position
	void UpdateSunDir(int iSunDir);
	//	Update sun height position
	void UpdateSunHei(int iSunHei);
	//  Reset interface data
	void UpdateControl();
	//  Save item's data 
	void SaveItemData();

	// Generated message map functions
	//{{AFX_MSG(CDlgLighting)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnReleasedcaptureSliderSundir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderSunheight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditSundirpos();
	afx_msg void OnChangeEditSunheipos();
	afx_msg void OnChangeEditSunpower();
	afx_msg void OnRadioDay();
	afx_msg void OnRadioNight();
	//}}AFX_MSG
	
	afx_msg HRESULT OnColorChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLIGHTING_H__633E3EBD_4D6A_4CA5_89AC_EFCE0ADD9A6C__INCLUDED_)
