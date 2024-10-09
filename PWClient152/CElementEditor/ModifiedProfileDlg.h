#if !defined(AFX_MODIFIEDPROFILEDLG_H__7580A257_EC15_4F62_B313_91DC94D5083F__INCLUDED_)
#define AFX_MODIFIEDPROFILEDLG_H__7580A257_EC15_4F62_B313_91DC94D5083F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifiedProfileDlg.h : header file
//
#include "HeightMapView.h"
#include "NumEdit.h"

class CTerrain;
class CELBitmap;
/////////////////////////////////////////////////////////////////////////////
// CModifiedProfileDlg dialog

class CModifiedProfileDlg : public CDialog
{
// Construction
public:
	CModifiedProfileDlg(CWnd* pParent = NULL);   // standard constructor
	void SetTerrain(CTerrain * pTerrain){ m_pTerrain = pTerrain; };
	void ReleaseHeightMapBmp();
	void ReleasePreViewMap();
// Dialog Data
	//{{AFX_DATA(CModifiedProfileDlg)
	enum { IDD = IDD_MODIFIED_TERRAIN_PROFILE };
	CNumEdit	m_MapPath4;
	CNumEdit	m_MapPath3;
	CNumEdit	m_MapPath2;
	CNumEdit	m_MapPath1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifiedProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RebuildHeightMap();
	void Build();
	void ShowPreViewMap();
	void ShowProfileMap();
	void DrawPixel(int x, int y);
	void GetData(bool bGet);

	
	CHeightMapView	m_MapView;		//	Height map view
	CHeightMapView  m_PreView;      //  Preview for profile map
	
	CELBitmap*	    m_pBitmap;		//	Height map bitmap
	CELBitmap*      m_pPreBitmap;
	CTerrain*		m_pTerrain;		//	Terrain object
	BYTE*           m_pByteData;
	BYTE*           m_pProfileData;
	
	//边缘检测，现使用Sobel算子，阈值为象素灰度的差分阈值
	int			m_nDiffThresh;	//0-255
	//对提取的边缘出现的指纹现象进行处理，采用5*5邻域过滤，阈值表示邻域中边界象素的计数阈值
	int			m_nThresh;        //0-25
	//搜索出所有的轮廓线，SizeThresh为设定的轮廓线所包含象素的最小数量
	// DistThresh阈值为两轮廓线连接的距离阈值
	int			m_nSizeThresh;
	int			m_nDistThresh;
	float		m_fFitThresh;

	bool m_bMedianFilter;
	bool m_bShowProfile;

	// Generated message map functions
	//{{AFX_MSG(CModifiedProfileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnButtonPreview();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnCheck3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFIEDPROFILEDLG_H__7580A257_EC15_4F62_B313_91DC94D5083F__INCLUDED_)
