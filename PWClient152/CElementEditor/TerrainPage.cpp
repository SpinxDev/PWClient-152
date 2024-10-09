/*
 * FILE: TerrainPage.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "TerrainPage.h"
#include "RollUpPanel.h"

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
//	Implement CTerrainPage
//	
///////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTerrainPage, CRollUpWnd)
	//{{AFX_MSG_MAP(CTerrainPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTerrainPage::CTerrainPage()
{
}

CTerrainPage::~CTerrainPage()
{
}

BOOL CTerrainPage::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CRollUpWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
	{
		g_Log.Log("CTerrainPage::Create, Failed to create terrain page");
		return FALSE;
	}

	// Create Terrain modify dialog panel for modify height map command\xqf
	m_TerrainModifyPanel.Create(IDD_TERRAIN_MODIFY,this);
	if(CreatePanel("地形",0,&m_TerrainModifyPanel) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create ModifyPanel");
		return FALSE;
	}

	// Create Terrain modify dialog panel for option
	m_TerrainModifyStretchPanel.Create(IDD_TERRAIN_MODIFY_STRETCH,this);
	if(CreatePanel("拉伸设置",0,&m_TerrainModifyStretchPanel) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create ModifyStretchPanel");
		return FALSE;
	}
	m_TerrainModifyNoisePanel.Create(IDD_TERRAIN_MODIFY_NOISE,this);
	if(CreatePanel("噪波设置",0,&m_TerrainModifyNoisePanel) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create ModifyNoisePanel");
		return FALSE;
	}

	m_TerrainModifySmoothPanle.Create(IDD_TERRAIN_MODIFY_SMOOTH,this);
	if(CreatePanel("平滑设置",0,&m_TerrainModifySmoothPanle) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create ModifyNoisePanel");
		return FALSE;
	}

	m_TerrainModifyLayMaskPanel.Create(IDD_TERRAIN_MODIFY_LAYMASK,this);
	if(CreatePanel("掩模刷子设置",0,&m_TerrainModifyLayMaskPanel) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create LayMask Panel");
		return FALSE;
	}

	m_TerrainModifyLayMaskSmoothPanel.Create(IDD_TERRAIN_MODIFY_LAYMASKSMOOTH,this);
	if(CreatePanel("选择层",0,&m_TerrainModifyLayMaskSmoothPanel) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create LayMask Smooth Panel");
		return FALSE;
	}
	
	m_TerrainModifyPlants.Create(IDD_TERRAIN_MODIFY_PLANTS,this);
	if(CreatePanel("植被面板",0,&m_TerrainModifyPlants) < 0)
	{
		g_Log.Log("CTerrainPage::Create, Failed to create plants Panel");
		return FALSE;
	}

	//	Show panels
	int aIndices[1] = {PANEL_MODIFY};
	bool aOpenFlags[1] = {true};
	ShowPanels(aIndices, aOpenFlags, 1);
	return TRUE;
}

//	Release resources
void CTerrainPage::Release()
{
}

