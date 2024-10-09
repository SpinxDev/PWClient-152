/*
 * FILE: DlgAutoLevelOff.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoLevelOff.h"
#include "AutoSelectedArea.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoSwitchDlg.h"

#include "aui/AUIManager.h"

#include "AutoTerrainCommon.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoHillPoly.h"
#include "AutoRoad.h"
#include "AutoTerrainFlat.h"
#include "AutoParameters.h"
#include "PolygonMountain.h"
#include "AutoBuildingOperation.h"
#include "AutoTerrainAction.h"

#include "windowsx.h"

#include "A3DMacros.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoLevelOff, CDlgHomeBase)

AUI_ON_COMMAND("CreateFlat", OnCommandCreateFlat)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoLevelOff, CDlgHomeBase)

AUI_END_EVENT_MAP()

CDlgAutoLevelOff::CDlgAutoLevelOff()
{
}

CDlgAutoLevelOff::~CDlgAutoLevelOff()
{
}
	
AUIStillImageButton * CDlgAutoLevelOff::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnLevelOff;
}

void CDlgAutoLevelOff::OnShowDialog()
{
	SetCanMove(false);

	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

void CDlgAutoLevelOff::OnCommandCreateFlat(const char* szCommand)
{
	if (GetOperationType() != OT_TERRAIN_FLAT)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoTerrainFlat* pAutoTerrainFlat = pScene->GetAutoTerrainFlat();
	if (NULL == pAutoTerrainFlat)
		return;
	
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();
	bool bClosed = pSelArea->GetPolyEditAreaClosed();
	pAutoTerrainFlat->SetAreaPoints(nNum, pPoints);
	
	if (!pAutoTerrainFlat->IsPolygonValid())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	if (!pAutoTerrainFlat->FlatTerrain())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	if (!pAction->AddActionTerrainFlat(
		pAutoTerrainFlat->GetArea()->GetNumPoints(),
		pAutoTerrainFlat->GetArea()->GetFinalPoints()))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	pScene->RecreateTerrainRender(true);

	// Update buildings
	pScene->GetAutoTerrain()->Tick(0);// Update terrain height
	CAutoBuildingOperation* pBuildingOperation = pScene->GetAutoBuildingOperation();
	pBuildingOperation->AdjustModelsAfterTerrainChange();


	pSelArea->DeleteAllPolyEditPoints();

	return;
}
*/

