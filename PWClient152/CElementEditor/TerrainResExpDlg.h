#if !defined(AFX_TERRAINRESEXPDLG_H__EF86602C_976A_4F42_A1CA_753B9E3FC2F7__INCLUDED_)
#define AFX_TERRAINRESEXPDLG_H__EF86602C_976A_4F42_A1CA_753B9E3FC2F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainResExpDlg.h : header file
//

class CElementMap;
class CSceneObject;

/////////////////////////////////////////////////////////////////////////////
// CTerrainResExpDlg dialog
enum
{
	exp_res = 0,
	exp_topview,
	exp_test_precinct,
};

class CTerrainResExpDlg : public CDialog
{
// Construction
public:
	CTerrainResExpDlg(CWnd* pParent = NULL);   // standard constructor
	int nExpType;
// Dialog Data
	//{{AFX_DATA(CTerrainResExpDlg)
	enum { IDD = IDD_DIALOG_EXP_RES };
	CProgressCtrl	m_Progress;
	CString	m_strPath;
	BOOL	m_bIsExpTexture;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainResExpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ProcessStaticModel(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessAudio(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessAiArea(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessSpecially(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessECMdoel(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessCritterGroup(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessFixedNpc(CSceneObject *pObj ,CElementMap *pMap);
	void ProcessTree(CElementMap *pMap);
	void ProcessGrass(CElementMap *pMap);
	void ProcessTerrain(CElementMap *pMap);

	bool ExpResData();
	void TransMap(CElementMap *pMap);
	bool CopyResFile( const char *szDst, const char *szSrc, CSceneObject* pObj, CElementMap *pMap);
	bool CreateFileSystem(const char *szDst, const char *szSrc, CSceneObject* pObj, CElementMap *pMap);
	BOOL CreateFolder(CString path);
	CString GetParentPath(CString  path);
	ALog   m_Log;
	int    m_nTopViewWidth;
	// Generated message map functions
	//{{AFX_MSG(CTerrainResExpDlg)
	afx_msg void OnPathSelect();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioSize512();
	afx_msg void OnRadioSize1024();
	afx_msg void OnRadioSize256();
	afx_msg void OnCheckTexture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINRESEXPDLG_H__EF86602C_976A_4F42_A1CA_753B9E3FC2F7__INCLUDED_)
