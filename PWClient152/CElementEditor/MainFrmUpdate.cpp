/*
 * FILE: MainFrmUpdate.cpp
 *
 * DESCRIPTION: Update routines for menu item and dialog controls
 *
 * CREATED BY: Duyuxin, 2003/9/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MainFrm.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

void CMainFrame::OnUpdateTerrainHeightmap(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateTerrainSuraces(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateTerrainLighting(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateTerrainCreatesurfs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.HasTerrainData() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateFileSaveas(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_wndViewframe.GetMap() ? TRUE : FALSE);
}



