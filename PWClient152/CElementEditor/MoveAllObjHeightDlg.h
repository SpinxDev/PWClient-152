#if !defined(AFX_MOVEALLOBJHEIGHTDLG_H__3CA97139_0DF1_4989_9F13_DB46E880B7D7__INCLUDED_)
#define AFX_MOVEALLOBJHEIGHTDLG_H__3CA97139_0DF1_4989_9F13_DB46E880B7D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveAllObjHeightDlg.h : header file
//

class CSceneObject;
class CElementMap;

/////////////////////////////////////////////////////////////////////////////
// CMoveAllObjHeightDlg dialog

class CMoveAllObjHeightDlg : public CDialog
{
// Construction
public:
	CMoveAllObjHeightDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMoveAllObjHeightDlg)
	enum { IDD = IDD_MOVE_ALL_OBJ_HEIGHT };
	CProgressCtrl	m_Progress;
	float	m_fDelta;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveAllObjHeightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ProcessStaticModel(CSceneObject *pObj);
	void ProcessAudio(CSceneObject *pObj);
	void ProcessAiArea(CSceneObject *pObj);
	void ProcessSpecially(CSceneObject *pObj);
	void ProcessECMdoel(CSceneObject *pObj);
	void ProcessCritterGroup(CSceneObject *pObj);
	void ProcessFixedNpc(CSceneObject *pObj);
	
	void ProcessTree(CElementMap *pMap);
	void ProcessGrass(CElementMap *pMap);
	void ProcessTerrain(CElementMap *pMap);
	
	void ProcessLight(CSceneObject*pObj);
	void ProcessWater(CSceneObject*pObj);
	void ProcessBezier(CSceneObject*pObj);
	void ProcessArea(CSceneObject*pObj);
	void ProcessDummy(CSceneObject*pObj);
	void ProcessPrecinct(CSceneObject*pObj);

	bool ProcessMapData();
	void TransMap(CElementMap *pMap);
	
	float m_fMoveDelta;
	// Generated message map functions
	//{{AFX_MSG(CMoveAllObjHeightDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEALLOBJHEIGHTDLG_H__3CA97139_0DF1_4989_9F13_DB46E880B7D7__INCLUDED_)
