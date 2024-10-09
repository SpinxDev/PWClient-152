#if !defined(AFX_EXPGAMEDATADLG_H__2FF2EFD3_9DD0_4BFC_AAC4_063DF5E6E058__INCLUDED_)
#define AFX_EXPGAMEDATADLG_H__2FF2EFD3_9DD0_4BFC_AAC4_063DF5E6E058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpGameDataDlg.h : header file
//
class CPtrList;
/////////////////////////////////////////////////////////////////////////////
// CExpGameDataDlg dialog

class CExpGameDataDlg : public CDialog
{
// Construction
public:
	enum
	{
		MASKTILE_64 = 0,
		MASKTILE_128,
		MASKTILE_256,
		MASKTILE_512,
		MASKTILE_NUM,
	};


	CExpGameDataDlg(CWnd* pParent = NULL);   // standard constructor


	//	Add a message to log window
	void AddLogMessage(const char* szMsg, ...);
	//	Clear all messages
	void ClearLogMessages();
	//	Reset progress bar
	void ResetProgressBar(int iTotal);
	//	Step progress bar
	void StepProgressBar(int iCurStep);

// Dialog Data
	//{{AFX_DATA(CExpGameDataDlg)
	enum { IDD = IDD_EXP_GAME_DATA };
	CComboBox	m_cbCopySel;
	CComboBox		m_MaskTileCombo;
	CEdit			m_LogEdit;
	CProgressCtrl	m_Progress;
	BOOL			m_bLighting;
	BOOL	m_bLightMapTrans;
	BOOL	m_bExpTerrain;
	BOOL	m_bExpObject;
	CString	m_strCopys;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpGameDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_aMaskTiles[MASKTILE_NUM];
	int		m_iProjMaskCol;		//	Column number of mask area in a project
	int		m_iProjMaskRow;		//	Row number of mask area in a project
	int		m_iSubTrnGrid;
	UINT	m_dwNumCol;
	
	//	Export terrain data
	bool ExportTerrainData(const char* szPath, const char* szName, int iNumCol, int iNumRow);
	//	Export scene data
	bool ExportSceneData(const char* szPath, const char* szName, int iNumCol, int iNumRow);
	//	Export lightmap data
	bool ExportLightmapData(const char* szPath, const char* szName, int iNumCol, int iNumRow);
	//	Export AI generator
	bool ExportAIGenerator(const char* szPath, const char *szWork ,int iNumCol, int iNumRow);

	virtual void OnOK(); 

	void UpdateString();
	CPtrList m_listProj;
	// Generated message map functions
	//{{AFX_MSG(CExpGameDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckLighting();
	afx_msg void OnDestroy();
	afx_msg void OnBtnExpaigen();
	afx_msg void OnCheckLightingTrans();
	afx_msg void OnCheckExpTerrain();
	afx_msg void OnCheckExpObject();
	afx_msg void OnCheckExportMiniMap();
	afx_msg void OnButtonSelCopy();
	afx_msg void OnBtnSavemask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPGAMEDATADLG_H__2FF2EFD3_9DD0_4BFC_AAC4_063DF5E6E058__INCLUDED_)
