#if !defined(AFX_DLGRANDOMMAPINFOSETTING_H__6E592F76_678A_4628_BA05_7BCF5D007256__INCLUDED_)
#define AFX_DLGRANDOMMAPINFOSETTING_H__6E592F76_678A_4628_BA05_7BCF5D007256__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRandomMapInfoSetting.h : header file
//
#include "DlgMapGrid.h"
#include "EL_RandomMapInfo.h"

class CRandMapProp;
class CDlgRandomMapInfoSetting;

//	选择单个地图块
class CSingleMapGridSelectPolicy : public CMapGridSelectPolicy
{
public:
	CSingleMapGridSelectPolicy(CDlgRandomMapInfoSetting* pDlg,CRandMapProp* prop):m_pDlgSetting(pDlg),m_randmapProp(prop),m_iSelectedIndex(-1){}
	//	继承自父类
	virtual bool IsSelected(int iTileIndex);
	virtual void ClearSelect();
	virtual bool OnLButtonDown(int iTileMouseOn, UINT nFlags);
	virtual bool OnRButtonDown(int iTile,UINT nFlags);
	virtual bool OnMouseMove(int iTileMouseOn, UINT nFlags);
	int GetSelectIndex() { return m_iSelectedIndex;}

protected:
	bool OnDataChanged(int iOld,int iNew=-1);

	//	继承自父类
	virtual void ResetMore();
	virtual void ClearSelectEnableMore(bool bEnable);
	
private:
	int	m_iSelectedIndex;
	CDlgRandomMapInfoSetting* m_pDlgSetting;
	CRandMapProp* m_randmapProp;
};

// 每个块的绘制策略
class CGridDrawPolicy
{
public:
	CGridDrawPolicy(){}
	virtual ~CGridDrawPolicy(){}
	
	virtual void DrawGird(const CString& name,int iGrid,bool bSel,HDC hDC,RECT& rc) = 0;
protected:

};

class CRandomMapGridDrawPolicy: public CGridDrawPolicy
{
public:
/*	enum
	{
		COL_NORMAL = 0,
		COL_START,
		COL_END,
		COL_NOUSE,
		COL_SEL,
		COL_NUM,
	};*/
public:
	CRandomMapGridDrawPolicy(CRandMapProp* prop);
	~CRandomMapGridDrawPolicy();
	void DrawGird(const CString& name,int iGrid,bool bSel,HDC hDC,RECT& rc);
	bool GetConnectMark(int grid_con,int c,RECT& rc);
protected:
	HBRUSH hBrush[CRandMapProp::GRID_TYPE_NUM+1];
	HPEN   hPen;
	CRandMapProp* m_randMapProp;
};

// 用来显示随机地图块
class CRandomMapGird : public CMapGrid
{
public:
	void SetDrawPolicy(CGridDrawPolicy* draw){ m_pGridDrawPolicy = draw;}

	int GetSelectIndex();
protected:

	virtual bool DrawBackBuffer(RECT* lprc);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	CGridDrawPolicy* m_pGridDrawPolicy;	
};

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomMapInfoSetting dialog

#include "ProjList.h"

class CDlgRandomMapInfoSetting : public CDialog
{
// Construction
public:
	CDlgRandomMapInfoSetting(CWnd* pParent = NULL);   // standard constructor

	void SetMask(int m);
	int GetMask();

	void SetHeaderInfo(const CRandMapProp::FILEHEADER& header);
	void GetHeaderInfo(CRandMapProp::FILEHEADER& header);

protected:
	void OnCancel();

public:

// Dialog Data
	//{{AFX_DATA(CDlgRandomMapInfoSetting)
	enum { IDD = IDD_RANDOM_MAP_INFO };
	int		m_iMapType;
	BOOL	m_mkCon[CRandMapProp::GRID_CON_NUM];
	CWnd	m_crtlMapType[CRandMapProp::GRID_TYPE_NUM];
	CString m_strMapName;
	int		m_iMainLineMinLen;
	int		m_iMainLineMaxLen;
	int		m_iBranchMinCount;
	int		m_iBranchMaxCount;
	int		m_iBranchMinLen;
	int		m_iBranchMaxLen;
	
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRandomMapInfoSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CRandomMapGird	m_mapGrid;
	CSingleMapGridSelectPolicy* m_pMapGridSelectPolicy;	
	ProjList	m_projList;

	CRandomMapGridDrawPolicy* m_pMapGridDrawProlicy;
	CRandMapProp m_randMapGridProp;

	// Generated message map functions
	//{{AFX_MSG(CDlgRandomMapInfoSetting)
	afx_msg void OnPaint();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCheckout();
	afx_msg void OnBtnCheckin();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnChangeEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRANDOMMAPINFOSETTING_H__6E592F76_678A_4628_BA05_7BCF5D007256__INCLUDED_)
