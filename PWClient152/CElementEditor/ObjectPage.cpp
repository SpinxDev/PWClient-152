/*
 * FILE: ObjectPage.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: QingFeng Xin, 2004/8/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "ObjectPage.h"
#include "RollUpPanel.h"
#include "Render.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CObjectsPage
//	
///////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CObjectsPage, CRollUpWnd)
	//{{AFX_MSG_MAP(CObjectsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CObjectsPage::CObjectsPage()
{
}

CObjectsPage::~CObjectsPage()
{
}

BOOL CObjectsPage::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CRollUpWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
	{
		g_Log.Log("CObjectsPage::Create, Failed to create object page");
		return FALSE;
	}

	//	Create scene object panel
	m_SceneObjectPanel.Create(IDD_SCENE_OBJECT, this);
	if (CreatePanel("场景对象", 0, &m_SceneObjectPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene object panel");
		return FALSE;
	}
    // Create scene light object panel
	m_ObjectLightPanel.Create(IDD_SCENE_OBJECT_LIGHT, this);
	if (CreatePanel("灯光属性", 0, &m_ObjectLightPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene light object panel");
		return FALSE;
	}
	
	//PANEL_MODEL_LIST_OBJECT
	m_SceneModelListPanel.Create(IDD_SCENE_OBJECT_MODEL_LIST, this);
	if (CreatePanel("模型列表", 0, &m_SceneModelListPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene model object panel");
		return FALSE;
	}

	m_SceneObjectsListPanel.Create(IDD_SCENE_OBJECTS_LIST, this);
	if (CreatePanel("场景对象列表", 0, &m_SceneObjectsListPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene objects list panel");
		return FALSE;
	}

	m_SkySetDialog.Create(IDD_SCENE_OBJECT_SKY, this);
	if (CreatePanel("天空体设置", 0, &m_SkySetDialog) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create sky set dialog panel");
		return FALSE;
	}

	m_VernierDlg.Create(IDD_DIALOG_VERNIER_SET, this);
	if (CreatePanel("坐标指示设置", 0, &m_VernierDlg) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene object vernier panel");
		return FALSE;
	}

	m_CloudSetPanel.Create(IDD_CLOUD_SET_DLG, this);
	if (CreatePanel("云层设置", 0, &m_CloudSetPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create cloud set panel");
		return FALSE;
	}
	
	m_ObjectPropertyPanel.Create(IDD_SCENE_OBJECT_PROPERTY, this);
	if (CreatePanel("对象属性", 0, &m_ObjectPropertyPanel) < 0)
	{
		g_Log.Log("CObjectsPage::Create, Failed to create scene object property panel");
		return FALSE;
	}

	//	Show panels
	int aIndices[2] = {PANEL_SCENE_OBJECT,PANEL_SCENE_LIST_OBJECT};
	bool aOpenFlags[2] = {true,false};
	ShowPanels(aIndices, aOpenFlags, 2);
	return TRUE;
}

//	Release resources
void CObjectsPage::Release()
{
}


