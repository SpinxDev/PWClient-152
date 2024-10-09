#if !defined(AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_)
#define AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTabWnd.h : header file
//

#include "TerrainPage.h"
#include "OptionsPage.h"
#include "ObjectPage.h"

/////////////////////////////////////////////////////////////////////////////
// CToolTabWnd window

class CToolTabWnd : public CWnd
{
public:		//	Types

	//	Index of pages
	enum
	{
		PAGE_TERRAIN = 0,
		PAGE_OBJECTS,
		PAGE_OPTIONS,
	};

public:		//	Constructions and Destructions
	
	CToolTabWnd();

public:		//	Attributes

public:		//	Operations

	//	Initlaize device objects
	bool InitDeviceObjects();
	//	Release resources
	void Release();

	//���涼��Ϊ�˸����ұߵĸ�����������PANEL���Զ������
	//Start
	// On terrain layer has been added or deleted
	void OnTrnLayerAddedOrDeleted();
    // Update brush option panel
	void OnBrushOptionPanel();
	void OnPlayerPassMapCurLayerChange();
	// Update stretch brush height show
	void UpdateBrushCenterHeight( float fHeight);
	// Update stretch brush height rect
	void UpdateBrushHeightRect();

	void UpdateSkyBoxPanel();
	
	void UpdatePlantsPanel();
	// ���³����������Ա�
	void ShowPropertyPannel(ADynPropertyObject *pPropertyObject);
	// ���������򳡾��м���Ʒ�İ�Ŧ��ͬʱ�ѵ�ǰ������Ϊ0
	void RiseUpAllSceneObjectButton();

	void RiseUpAllTerrainOperationButton();
	// 
	void UpdatePropertyData(bool bGet);
	//���³��������ж����
	void UpdateSceneObjectList();
	//
	void UpdateVernierDlg();
	void RiseUpVernierButton();
	//����ģ�ͱ�
	void UpdateModelList();
	//���ض�������Panel
	void HideObjectProperyPanel(bool bHide = true);
	//���ص�������Panel
	void HideTerrainProperyPanel();
	//���õ���Ҫ���ĵ�MASK��
	void SetMaskLay( char *szMaskLay);
	//End

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTabWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CToolTabWnd();

protected:	//	Attributes

	CTabCtrl		m_TabCtrl;	
	CWnd*			m_pCurPage;

	CTerrainPage	m_TerrainPage;		//	Terrain page
	COptionsPage	m_OptionsPage;		//	Options page
	CObjectsPage    m_ObjectsPage;      //  !

protected:	//	Operations

	//	Change page
	void ChangePage();
	//	Resize current page
	void ResizeCurPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTabWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTABWND_H__8254C98C_3C2C_45FC_AF99_26E7E5A13724__INCLUDED_)
