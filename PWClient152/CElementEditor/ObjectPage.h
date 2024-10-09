/*
 * FILE: ObjectPage.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: QingFeng Xin, 2004/8/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "RollUpWnd.h"
#include "DlgOptions.h"
#include "DlgRenderOpt.h"
#include "BrushSetPanel.h"

#include "SceneObjectPanel.h"
#include "ObjectLightPanel.h"
#include "ObjectPropertyPanel.h"
#include "SceneModelListPanel.h"
#include "SceneObjectsList.h"
#include "SkySetDialog.h"
#include "OptVernierDlg.h"
#include "CloudSetPanel.h"
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CObjectsPage
//	
///////////////////////////////////////////////////////////////////////////

class CObjectsPage : public CRollUpWnd
{
public:		//	Types

	//	Index of panels
	enum
	{
		PANEL_SCENE_OBJECT = 0,
		PANEL_LIGHT_OBJECT,
		PANEL_MODEL_LIST_OBJECT,
		PANEL_SCENE_LIST_OBJECT,
		PANEL_SKY_SET,
		PANEL_VERNIER_OPT,
		PANEL_CLOUD_SET,
		PANEL_PROPERTY_OBJECT,//last panel
		NUM_PANEL,
	};

public:		//	Constructor and Destructor

	CObjectsPage();
	virtual ~CObjectsPage();

public:		//	Attributes

public:		//	Operations
	inline void UpdateCloudPanel(){ m_CloudSetPanel.UpdateCloudData(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectsPage)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//	Initlaize device objects
	//bool InitDeviceObjects();
	//	Release resources
	void Release();
protected:	//	Attributes
	//管理场景中的模型对象
	CSceneObjectPanel		m_SceneObjectPanel;
	//管理场景中的灯光对象
	CObjectLightPanel		m_ObjectLightPanel;
	CObjectPropertyPanel	m_ObjectPropertyPanel;
	CSceneModelListPanel    m_SceneModelListPanel;
	CSceneObjectsList       m_SceneObjectsListPanel;
	CSkySetDialog           m_SkySetDialog;
	COptVernierDlg          m_VernierDlg;
	CCloudSetPanel          m_CloudSetPanel;

protected:	//	Operations
	virtual void OnOK(){};
	virtual void OnCancel(){};
	//{{AFX_MSG(CObjectsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////