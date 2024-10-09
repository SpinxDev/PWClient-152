/*
 * FILE: EC_HomeDlgsMgr.cpp
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
#include "EC_HomeDlgsMgr.h"


#include "aui/AUIManager.h"
#include "aui/AUIStillImageButton.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
//#include "AutoTerrainAction.h"
#include "render.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DCamera.h"
#include "A3DOrthoCamera.h"
#include "EC_Viewport.h"
#include "AutoParameters.h"

#include "DlgAutoLight.h"
#include "DlgAutoLocalTexture.h"
#include "DlgAutoMountain.h"
#include "DlgAutoMountain2.h"
#include "DlgAutoRoad.h"
#include "DlgAutoTerrain.h"
#include "DlgAutoWater.h"
#include "DlgAutoChooseTexture.h"
#include "DlgCurve.h"

#include "DlgAutoChooseSky.h"
#include "DlgAutoChooseColor.h"
#include "DlgAutoChooseLight.h"
#include "DlgAutoAllRes.h"

#include "DlgAutoBaseTerrain.h"
#include "DlgAutoBaseMountain.h"
#include "DlgAutoBaseMountain2.h"
#include "DlgAutoBaseRoad.h"
#include "DlgAutoBaseWater.h"
#include "DlgAutoBaseRoad.h"
#include "DlgAutoBaseLocalTex.h"
#include "DlgAutoLevelOff.h"
#include "DlgAutoTools.h"
#include "DlgAutoCtrl.h"
#include "DlgAutoNote.h"
#include "DlgAutoNote2.h"
#include "DlgAutoSwitchDlg.h"


#include "TerrainLayer.h"

#include "AutoTerrain.h"
#include "AutoHillPoly.h"
#include "AutoHillLine.h"
#include "AutoRoad.h"
#include "AutoPartTexture.h"
#include "AutoTerrainFlat.h"
#include "AutoForestLine.h"
#include "AutoForestPoly.h"
#include "AutoGrassPoly.h"
#include "AutoBuilding.h"
#include "AutoSelectedArea.h"
#include "AutoBuildingOperation.h"

#include "DlgAutoOutdoorTree.h"
#include "DlgAutoOutdoorGrass.h"
#include "DlgAutoOutdoorOtherObject.h"
#include "DlgAutoOutdoorBuilding.h"

#include "DlgAutoIndoorFurniture.h"
#include "DlgAutoIndoorPlant.h"
#include "DlgAutoIndoorToy.h"
#include "DlgAutoIndoorVirtu.h"

#include "DlgAutoCamera.h"

#include "DlgAutoCtrlEditArea.h"

#include "DlgAutoGenLightMapProgress.h"
#include "windowsx.h"



CECHomeDlgsMgr::CECHomeDlgsMgr()
{
	Clear();	
}
CECHomeDlgsMgr::~CECHomeDlgsMgr()
{
	Clear();	
}

void CECHomeDlgsMgr::Clear()
{
	m_pAUIMgr = NULL;

	m_pAutoScene = NULL;
	m_pAutoParam = NULL;

	m_pDlgAutoLight = NULL;
	m_pDlgAutoLocalTexture = NULL;
	m_pDlgAutoMountain = NULL;

	m_pDlgAutoRoad = NULL;
	m_pDlgAutoTerrain = NULL;
	m_pDlgAutoWater = NULL;
	m_pCurDlg = NULL;
	
	m_pDlgAutoChooseTexture = NULL;
	m_pDlgCurve = NULL;

	m_pDlgAutoChooseSky = NULL;
	m_pDlgAutoChooseColor = NULL;


	m_pDlgAutoAllRes = NULL;

	m_pDlgAutoBaseTerrain = NULL;
	m_pDlgAutoBaseMountain = NULL;
	m_pDlgAutoBaseMountain2 = NULL;
	m_pDlgAutoBaseRoad = NULL;
	m_pDlgAutoBaseWater = NULL;
	m_pDlgAutoBaseLocalTex = NULL;
	m_pDlgAutoLevelOff = NULL;
	m_pDlgCurBase = NULL;


	m_pDlgAutoTools = NULL;
	m_pDlgAutoNote = NULL;
	m_pDlgAutoNote2 = NULL;
	m_pDlgAutoCtrl = NULL;
	
	m_pDlgAutoSwitchDlg = NULL;
	m_pDlgAutoCtrlEditArea = NULL;
	m_pDlgAutoCamera = NULL;



	m_pBtnBaseTerrain = NULL;
	m_pBtnBaseMountain = NULL;
	m_pBtnBaseMountain2 = NULL;
	m_pBtnBaseRoad = NULL;
	m_pBtnBaseWater = NULL;
	m_pBtnBaseLocalTex = NULL;
	m_pBtnLevelOff = NULL;
	m_pBtnCurBase = NULL;

	m_pDlgAutoIndoorToy = NULL;
	m_pDlgAutoIndoorPlant = NULL;
	m_pDlgAutoIndoorFurniture = NULL;
	m_pDlgAutoIndoorVirtu = NULL;
	m_pDlgCurIndoor = NULL;

	m_pDlgAutoOutdoorBuilding = NULL;
	m_pDlgAutoOutdoorGrass = NULL;
	m_pDlgAutoOutdoorTree = NULL;
	m_pDlgAutoOutdoorOtherObject = NULL;
	m_pDlgCurOutdoor = NULL;

	m_pDlgAutoGenLightMapProgress = NULL;

	m_pBtnIndoorToy = NULL;
	m_pBtnIndoorFurniture = NULL;
	m_pBtnIndoorVirtu = NULL;
	m_pBtnIndoorPlant = NULL;
	m_pBtnCurIndoor = NULL;

	m_pBtnOutdoorTree = NULL;
	m_pBtnOutdoorGrass = NULL;
	m_pBtnOutdoorBuilding = NULL;
	m_pBtnOutdoorOtherObject = NULL;
	m_pBtnCurOutdoor = NULL;
	
	m_bInitReady = false;	
	m_bModifying = false;
	m_nModelState = MSE_CREATE;
}

void CECHomeDlgsMgr::ClearDlgsVars()
{
		m_pDlgAutoLight = NULL;
	m_pDlgAutoLocalTexture = NULL;
	m_pDlgAutoMountain = NULL;

	m_pDlgAutoRoad = NULL;
	m_pDlgAutoTerrain = NULL;
	m_pDlgAutoWater = NULL;
	m_pCurDlg = NULL;
	
	m_pDlgAutoChooseTexture = NULL;
	m_pDlgCurve = NULL;

	m_pDlgAutoChooseSky = NULL;
	m_pDlgAutoChooseColor = NULL;


	m_pDlgAutoAllRes = NULL;

	m_pDlgAutoBaseTerrain = NULL;
	m_pDlgAutoBaseMountain = NULL;
	m_pDlgAutoBaseMountain2 = NULL;
	m_pDlgAutoBaseRoad = NULL;
	m_pDlgAutoBaseWater = NULL;
	m_pDlgAutoBaseLocalTex = NULL;
	m_pDlgAutoLevelOff = NULL;
	m_pDlgCurBase = NULL;


	m_pDlgAutoTools = NULL;
	m_pDlgAutoNote = NULL;
	m_pDlgAutoNote2 = NULL;
	m_pDlgAutoCtrl = NULL;
	
	m_pDlgAutoSwitchDlg = NULL;
	m_pDlgAutoCtrlEditArea = NULL;
	m_pDlgAutoCamera = NULL;



	m_pBtnBaseTerrain = NULL;
	m_pBtnBaseMountain = NULL;
	m_pBtnBaseMountain2 = NULL;
	m_pBtnBaseRoad = NULL;
	m_pBtnBaseWater = NULL;
	m_pBtnBaseLocalTex = NULL;
	m_pBtnLevelOff = NULL;
	m_pBtnCurBase = NULL;

	m_pDlgAutoIndoorToy = NULL;
	m_pDlgAutoIndoorPlant = NULL;
	m_pDlgAutoIndoorFurniture = NULL;
	m_pDlgAutoIndoorVirtu = NULL;
	m_pDlgCurIndoor = NULL;

	m_pDlgAutoOutdoorBuilding = NULL;
	m_pDlgAutoOutdoorGrass = NULL;
	m_pDlgAutoOutdoorTree = NULL;
	m_pDlgAutoOutdoorOtherObject = NULL;
	m_pDlgCurOutdoor = NULL;

	m_pDlgAutoGenLightMapProgress = NULL;

	m_pBtnIndoorToy = NULL;
	m_pBtnIndoorFurniture = NULL;
	m_pBtnIndoorVirtu = NULL;
	m_pBtnIndoorPlant = NULL;
	m_pBtnCurIndoor = NULL;

	m_pBtnOutdoorTree = NULL;
	m_pBtnOutdoorGrass = NULL;
	m_pBtnOutdoorBuilding = NULL;
	m_pBtnOutdoorOtherObject = NULL;
	m_pBtnCurOutdoor = NULL;
	
	m_bInitReady = false;	
	m_bModifying = false;
	m_nModelState = MSE_CREATE;
}

bool CECHomeDlgsMgr::Reset()
{
	HideHomeDlgs();

	ClearDlgsVars();

	if( !InitDialogs())
		return false;
	
	if( !InitSwitchBtn())
		return false;

	m_bInitReady = true;
	
	ShowHomeDlgs();
	
	return true;
}

bool CECHomeDlgsMgr::Init(HomeDlgsMgrInitParams_t params)
{
	m_pAUIMgr = params.pAUIMgr;

	m_pAutoScene = g_pGame->GetGameRun()->GetWorld()->GetAutoScene();
	if( !m_pAutoScene)
	{
		a_LogOutput(1, "CECHomeDlgsMgr::Init, Failed to get CAutoScene!");
		AutoHomeAssert();
		return false;
	}

	m_pAutoParam = m_pAutoScene->GetAutoParameters();
	if( !m_pAutoParam)
	{
		AutoHomeReport("CECHomeDlgsMgr::Init, Failed to get CAutoParameters");
		return false;
	}

// 	if(m_bInitReady)
// 		return true;

	if( !InitDialogs())
		return false;
	
	if( !InitSwitchBtn())
		return false;



	m_bInitReady = true;

	return true;
}

//-------------------------------------------------------------------------------
void CECHomeDlgsMgr::HideAllOldDlg()
{
	int nNumDlg = m_pAUIMgr->GetDialogCount();
	for( int i = 0; i < nNumDlg; i++)
	{
		AUIDialog* pDlg = m_pAUIMgr->GetDialog(i);
		if( pDlg->IsShow())
		{
			m_vecOldDlg.push_back(pDlg);
			pDlg->Show(false);
		}
	}
}

//-------------------------------------------------------------------------------
void CECHomeDlgsMgr::ShowAllOldDlg()
{
	OldDlgVecIter_t iter = m_vecOldDlg.begin();
	for( ; iter != m_vecOldDlg.end(); iter++)
	{
		(*iter)->Show(true);
	}
	m_vecOldDlg.clear();
}

//-------------------------------------------------------------------------------
#define INIT_DIALOG_POINTER(variableName, className, dialogName) \
{ \
	variableName = dynamic_cast<className *>(m_pAUIMgr->GetDialog(dialogName)); \
	ASSERT(variableName); \
}

//-------------------------------------------------------------------------------
bool CECHomeDlgsMgr::InitDialogs()
{
	INIT_DIALOG_POINTER(m_pDlgAutoLight, CDlgAutoLight, "Dlg_Light");
	INIT_DIALOG_POINTER(m_pDlgAutoLocalTexture, CDlgAutoLocalTexture, "Dlg_AdvanceLocalTex");
	INIT_DIALOG_POINTER(m_pDlgAutoMountain, CDlgAutoMountain, "Dlg_AdvanceMountain");
	INIT_DIALOG_POINTER(m_pDlgAutoMountain2, CDlgAutoMountain2, "Dlg_AdvanceMountain2");
	

	INIT_DIALOG_POINTER(m_pDlgAutoRoad, CDlgAutoRoad, "Dlg_AdvanceRoad");
	INIT_DIALOG_POINTER(m_pDlgAutoTerrain, CDlgAutoTerrain, "Dlg_AdvanceTerrain");
	INIT_DIALOG_POINTER(m_pDlgAutoWater, CDlgAutoWater, "Dlg_AdvanceWater");
	INIT_DIALOG_POINTER(m_pDlgAutoChooseTexture, CDlgAutoChooseTexture, "Dlg_ChooseTexture");

	INIT_DIALOG_POINTER(m_pDlgCurve, CDlgCurve, "Dlg_Curve");
	
	INIT_DIALOG_POINTER(m_pDlgAutoChooseSky, CDlgAutoChooseSky, "Dlg_ChooseSky");
	INIT_DIALOG_POINTER(m_pDlgAutoChooseColor, CDlgAutoChooseColor, "Dlg_ChooseColor");
	INIT_DIALOG_POINTER(m_pDlgAutoChooseLight, CDlgAutoChooseLight, "Dlg_ChooseLight");

	INIT_DIALOG_POINTER(m_pDlgAutoTools, CDlgAutoTools, "Dlg_Tools");
	INIT_DIALOG_POINTER(m_pDlgAutoNote, CDlgAutoNote, "Dlg_Note");
	INIT_DIALOG_POINTER(m_pDlgAutoNote2, CDlgAutoNote2, "Dlg_Note2");
	INIT_DIALOG_POINTER(m_pDlgAutoCtrl, CDlgAutoCtrl, "Dlg_Ctrl");

	
	INIT_DIALOG_POINTER(m_pDlgAutoAllRes, CDlgAutoAllRes, "Dlg_AllRes");

	INIT_DIALOG_POINTER(m_pDlgAutoBaseTerrain, CDlgAutoBaseTerrain, "Dlg_BaseTerrain");
	INIT_DIALOG_POINTER(m_pDlgAutoBaseMountain, CDlgAutoBaseMountain, "Dlg_BaseMountain");
	INIT_DIALOG_POINTER(m_pDlgAutoBaseMountain2, CDlgAutoBaseMountain2, "Dlg_BaseMountain2");
	INIT_DIALOG_POINTER(m_pDlgAutoBaseWater, CDlgAutoBaseWater, "Dlg_BaseWater");
	INIT_DIALOG_POINTER(m_pDlgAutoBaseRoad, CDlgAutoBaseRoad, "Dlg_BaseRoad");
	INIT_DIALOG_POINTER(m_pDlgAutoBaseLocalTex, CDlgAutoBaseLocalTex, "Dlg_BaseLocalTex");
	INIT_DIALOG_POINTER(m_pDlgAutoLevelOff, CDlgAutoLevelOff, "Dlg_LevelOff");

	INIT_DIALOG_POINTER(m_pDlgAutoOutdoorBuilding, CDlgAutoOutdoorBuilding, "Dlg_OutdoorBuilding");
	INIT_DIALOG_POINTER(m_pDlgAutoOutdoorTree, CDlgAutoOutdoorTree, "Dlg_OutdoorTree");
	INIT_DIALOG_POINTER(m_pDlgAutoOutdoorGrass, CDlgAutoOutdoorGrass, "Dlg_OutdoorGrass");
	INIT_DIALOG_POINTER(m_pDlgAutoOutdoorOtherObject, CDlgAutoOutdoorOtherObject, "Dlg_OutdoorOtherObject");

	INIT_DIALOG_POINTER(m_pDlgAutoIndoorPlant, CDlgAutoIndoorPlant, "Dlg_IndoorPlant");
	INIT_DIALOG_POINTER(m_pDlgAutoIndoorToy, CDlgAutoIndoorToy, "Dlg_IndoorToy");
	INIT_DIALOG_POINTER(m_pDlgAutoIndoorVirtu, CDlgAutoIndoorVirtu, "Dlg_IndoorVirtu");
	INIT_DIALOG_POINTER(m_pDlgAutoIndoorFurniture, CDlgAutoIndoorFurniture, "Dlg_IndoorFurniture");

	INIT_DIALOG_POINTER(m_pDlgAutoSwitchDlg, CDlgAutoSwitchDlg, "Dlg_SwitchDlg");
	INIT_DIALOG_POINTER(m_pDlgAutoCtrlEditArea, CDlgAutoCtrlEditArea, "Dlg_CtrlEditArea");

	INIT_DIALOG_POINTER(m_pDlgAutoGenLightMapProgress, CDlgAutoGenLightMapProgress, "Dlg_GenLightMapProgress");

	INIT_DIALOG_POINTER(m_pDlgAutoCamera, CDlgAutoCamera, "Dlg_Camera");

	const int nNumDlgs = 42;
	bool bOK[nNumDlgs];

	bOK[0] = true;
	bOK[1] = m_pDlgAutoLight->InitContext(this);
	bOK[2] = m_pDlgAutoLocalTexture->InitContext(this);
	bOK[3] = m_pDlgAutoMountain->InitContext(this);
	bOK[4] = true;
	bOK[5] = m_pDlgAutoRoad->InitContext(this);
	bOK[6] = m_pDlgAutoTerrain->InitContext(this);
	bOK[7] = m_pDlgAutoWater->InitContext(this);
	bOK[8] = m_pDlgAutoChooseTexture->InitContext(this);
	bOK[9] = true;
	bOK[10] = true;
	bOK[11] = m_pDlgAutoChooseSky->InitContext(this);
	bOK[12] = m_pDlgAutoChooseColor->InitContext(this);
	bOK[13] = m_pDlgAutoChooseLight->InitContext(this);
	
	bOK[14] = m_pDlgAutoNote->InitContext(this);
	bOK[15] = m_pDlgAutoTools->InitContext(this);
	bOK[16] = m_pDlgAutoCtrl->InitContext(this);
	bOK[17] = true;
	bOK[18] = true;
	bOK[19] = true;

	bOK[20] = m_pDlgAutoBaseTerrain->InitContext(this);
	bOK[21] = m_pDlgAutoBaseMountain->InitContext(this);
	bOK[22] = m_pDlgAutoBaseMountain2->InitContext(this);
	bOK[23] = m_pDlgAutoBaseRoad->InitContext(this);
	bOK[24] = m_pDlgAutoBaseWater->InitContext(this);
	bOK[25] = m_pDlgAutoBaseLocalTex->InitContext(this);
	bOK[26] = m_pDlgAutoLevelOff->InitContext(this);

	bOK[27] = m_pDlgAutoNote2->InitContext(this);
	bOK[28] = m_pDlgAutoMountain2->InitContext(this);

	bOK[29] = m_pDlgAutoOutdoorBuilding->InitContext(this);
	bOK[30] = m_pDlgAutoOutdoorTree->InitContext(this);
	bOK[31] = m_pDlgAutoOutdoorGrass->InitContext(this);
	bOK[32] = m_pDlgAutoOutdoorOtherObject->InitContext(this);

	bOK[33] = m_pDlgAutoIndoorFurniture->InitContext(this);
	bOK[34] = m_pDlgAutoIndoorToy->InitContext(this);
	bOK[35] = m_pDlgAutoIndoorVirtu->InitContext(this);
	bOK[36] = m_pDlgAutoIndoorPlant->InitContext(this);

	bOK[37] = m_pDlgAutoSwitchDlg->InitContext(this);
	bOK [38] = m_pDlgAutoAllRes->InitContext(this);
	bOK[39] = m_pDlgAutoCtrlEditArea->InitContext(this);

	bOK[40] = m_pDlgAutoGenLightMapProgress->InitContext(this);
	bOK[41] = m_pDlgAutoCamera->InitContext(this);

	bool bRet = true;
	for(int i = 0; i < nNumDlgs; i++)
	{
		bRet = bRet && bOK[i];
	}

	m_pCurDlg = m_pDlgAutoTerrain;
	
	m_pDlgCurBase = m_pDlgAutoBaseTerrain;
	m_pDlgCurIndoor = m_pDlgAutoIndoorFurniture;
	m_pDlgCurOutdoor = m_pDlgAutoOutdoorTree;

	return bRet;

}


*/
//void CECHomeDlgsMgr::Show(bool bShow /* = true*/)
/*{
	if(!m_pCurDlg)
		return ;
	
	if( bShow)
	{	
		HideAllOldDlg();
		ShowHomeDlgs();

		
	}
	else
	{
		HideHomeDlgs();
		ShowAllOldDlg();
	}


}

#define INIT_BUTTON_POINTER(variableName, dialogPointer, buttonName) \
{ \
	variableName = dynamic_cast<AUIStillImageButton *>( \
		dialogPointer->GetDlgItem(buttonName)); \
	ASSERT(variableName); \
}

bool CECHomeDlgsMgr::InitSwitchBtn()
{

	INIT_BUTTON_POINTER(m_pBtnBaseTerrain, m_pDlgAutoBaseTerrain, "Btn_BaseTerrain");
	INIT_BUTTON_POINTER(m_pBtnBaseMountain, m_pDlgAutoBaseMountain, "Btn_BaseMountain");
	INIT_BUTTON_POINTER(m_pBtnBaseMountain2, m_pDlgAutoBaseMountain2, "Btn_BaseMountain2");
	INIT_BUTTON_POINTER(m_pBtnBaseRoad, m_pDlgAutoBaseRoad, "Btn_BaseRoad");
	INIT_BUTTON_POINTER(m_pBtnBaseWater, m_pDlgAutoBaseWater, "Btn_BaseWater");
	INIT_BUTTON_POINTER(m_pBtnBaseLocalTex, m_pDlgAutoBaseLocalTex, "Btn_BaseLocalTex");
	INIT_BUTTON_POINTER(m_pBtnLevelOff, m_pDlgAutoLevelOff, "Btn_LevelOff");

	m_pBtnBaseTerrain->SetPushLike(true);
	m_pBtnBaseMountain->SetPushLike(true);
	m_pBtnBaseMountain2->SetPushLike(true);
	m_pBtnBaseRoad->SetPushLike(true);
	m_pBtnBaseWater->SetPushLike(true);
	m_pBtnBaseLocalTex->SetPushLike(true);
	m_pBtnLevelOff->SetPushLike(true);

	m_pBtnCurBase = m_pBtnBaseTerrain;
	m_pBtnCurBase->SetPushed(true);
		
	INIT_BUTTON_POINTER(m_pBtnIndoorPlant, m_pDlgAutoIndoorPlant, "Btn_IndoorPlant");
	INIT_BUTTON_POINTER(m_pBtnIndoorVirtu, m_pDlgAutoIndoorVirtu, "Btn_IndoorVirtu");
	INIT_BUTTON_POINTER(m_pBtnIndoorToy, m_pDlgAutoIndoorToy, "Btn_IndoorToy");
	INIT_BUTTON_POINTER(m_pBtnIndoorFurniture, m_pDlgAutoIndoorFurniture, "Btn_IndoorFurniture");
	
	m_pBtnIndoorPlant->SetPushLike(true);
	m_pBtnIndoorVirtu->SetPushLike(true);
	m_pBtnIndoorToy->SetPushLike(true);
	m_pBtnIndoorFurniture->SetPushLike(true);
	
	m_pBtnCurIndoor= m_pBtnIndoorFurniture;
	m_pBtnCurIndoor->SetPushed(true);

	INIT_BUTTON_POINTER(m_pBtnOutdoorTree, m_pDlgAutoOutdoorTree, "Btn_OutdoorTree");
	INIT_BUTTON_POINTER(m_pBtnOutdoorGrass, m_pDlgAutoOutdoorGrass, "Btn_OutdoorGrass");
	INIT_BUTTON_POINTER(m_pBtnOutdoorBuilding, m_pDlgAutoOutdoorBuilding, "Btn_OutdoorBuilding");
	INIT_BUTTON_POINTER(m_pBtnOutdoorOtherObject, m_pDlgAutoOutdoorOtherObject, "Btn_OutdoorOtherObject");

	m_pBtnOutdoorTree->SetPushLike(true);
	m_pBtnOutdoorGrass->SetPushLike(true);
	m_pBtnOutdoorBuilding->SetPushLike(true);
	m_pBtnOutdoorOtherObject->SetPushLike(true);

	m_pBtnCurOutdoor = m_pBtnOutdoorTree;
	m_pBtnCurOutdoor->SetPushed(true);

	
	return true;
}

void CECHomeDlgsMgr::SwitchDialog(const char * szCommand)
{
	if(m_bModifying)
		return;

	AString strCommand = szCommand;
	
	//Base dialogs
	CDlgHomeBase* pDlgBase = NULL;
	if( strCommand == "ChooseBaseTerrain")
	{
		pDlgBase = m_pDlgAutoBaseTerrain;
		m_pAutoScene->SetOperationType(OT_NORMAL);
		m_pDlgAutoNote2->Show(false);
		m_pDlgAutoCtrl->Show(false);
		m_pDlgAutoCtrlEditArea->Show(false);
	}
	else if( strCommand == "ChooseBaseMountain")
	{
		pDlgBase = m_pDlgAutoBaseMountain;
		m_pAutoScene->SetOperationType(OT_HILL_POLY);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
		

	}
	else if( strCommand == "ChooseBaseMountain2")
	{
		pDlgBase = m_pDlgAutoBaseMountain2;
		m_pAutoScene->SetOperationType(OT_HILL_LINE);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_LINE);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
	}
	else if( strCommand == "ChooseBaseRoad")
	{
		pDlgBase = m_pDlgAutoBaseRoad;
		m_pAutoScene->SetOperationType(OT_ROAD);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_LINE);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
	}
	else if ( strCommand == "ChooseBaseWater")
	{
		pDlgBase = m_pDlgAutoBaseWater;
		m_pDlgAutoNote2->Show(false);
		m_pAutoScene->SetOperationType(OT_WATER);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
		
	}
	else if( strCommand == "ChooseBaseLocalTex")
	{
		pDlgBase = m_pDlgAutoBaseLocalTex;
		m_pAutoScene->SetOperationType(OT_TEXTURE_POLY);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
	}
	else if( strCommand == "ChooseLevelOff")
	{
		pDlgBase = m_pDlgAutoLevelOff;
		m_pAutoScene->SetOperationType(OT_TERRAIN_FLAT);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
	}

	if( pDlgBase)
	{
		m_pDlgCurBase->Show(false);
		m_pDlgCurBase = pDlgBase;
		m_pDlgCurBase->Show(true);

		m_pBtnCurBase->SetPushed(false);
		m_pBtnCurBase = pDlgBase->GetSwithButton();
		m_pBtnCurBase->SetPushed(true);
	}

	//Outdoor Dlgs
	CDlgHomeBase* DlgCurOutdoor = NULL;
	if( strCommand == "ChooseOutdoorTree")
	{
		DlgCurOutdoor = m_pDlgAutoOutdoorTree;
		m_pAutoScene->SetOperationType(OT_TREE_POLY);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
		
	}
	else if( strCommand == "ChooseOutdoorGrass")
	{
		DlgCurOutdoor = m_pDlgAutoOutdoorGrass;
		m_pAutoScene->SetOperationType(OT_GRASS_POLY);
		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
		m_pDlgAutoCtrlEditArea->Show(true);
		m_pDlgAutoCtrl->Show(false);
	}
	else if( strCommand == "ChooseOutdoorBuilding")
	{
		DlgCurOutdoor = m_pDlgAutoOutdoorBuilding;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}
	else if( strCommand == "ChooseOutdoorOtherObject")
	{
		DlgCurOutdoor = m_pDlgAutoOutdoorOtherObject;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}

	if( DlgCurOutdoor)
	{
		m_pDlgCurOutdoor->Show(false);
		m_pDlgCurOutdoor = DlgCurOutdoor;
		m_pDlgCurOutdoor->Show(true);

		m_pBtnCurOutdoor->SetPushed(false);
		m_pBtnCurOutdoor = DlgCurOutdoor->GetSwithButton();
		m_pBtnCurOutdoor->SetPushed(true);
	}
	
	//Indoor Dlgs
	CDlgHomeBase* pDlgCurIndoor = NULL;
	if( strCommand == "ChooseIndoorFurniture")
	{
		pDlgCurIndoor = m_pDlgAutoIndoorFurniture;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}
	else if( strCommand == "ChooseIndoorPlant")
	{
		pDlgCurIndoor = m_pDlgAutoIndoorPlant;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}
	else if( strCommand == "ChooseIndoorToy")
	{
		pDlgCurIndoor = m_pDlgAutoIndoorToy;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}
	else if( strCommand == "ChooseIndoorVirtu")
	{
		pDlgCurIndoor = m_pDlgAutoIndoorVirtu;
		m_pAutoScene->SetOperationType(OT_BUILDING);
		m_pDlgAutoCtrl->Show(true);
		m_pDlgAutoCtrlEditArea->Show(false);

		m_pDlgAutoNote2->Show(true);
		m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_MODEL);
	}

	if( pDlgCurIndoor)
	{
		m_pDlgCurIndoor->Show(false);
		m_pDlgCurIndoor = pDlgCurIndoor;
		m_pDlgCurIndoor->Show(true);

		m_pBtnCurIndoor->SetPushed(false);
		m_pBtnCurIndoor = pDlgCurIndoor->GetSwithButton();
		m_pBtnCurIndoor->SetPushed(true);
	}


}

bool CECHomeDlgsMgr::LoadAutoSceneAction(const char* szActionFile)
{
	CAutoScene* pAutoScene = NULL;
	pAutoScene = g_pGame->GetGameRun()->GetWorld()->GetAutoScene();
		if( !pAutoScene)
			return false;
	
	CAutoTerrainAction* pAction = NULL;
	pAction = pAutoScene->GetAutoTerrainAction();
	if( !pAction)
		return false;

	if(!pAction->LoadActions(szActionFile))
		return false;

	if(!pAction->DoActions(pAutoScene))
		return false;

	if (!pAutoScene->RecreateTerrainRender(true))
	{
		return false;
	}

	return true;
}

bool CECHomeDlgsMgr::LoadTextureParamInfo(TextureParamInfo_t& param, TEXTURETYPE mainTexture, TEXTURETYPE patchTexture)
{
	CAutoTerrain* pAutoTerrain = m_pAutoScene->GetAutoTerrain();
	
	//Load main texture
	CTerrainLayer* pLayer = pAutoTerrain->GetLayer(mainTexture + 1);
	if( NULL == pLayer)
		return false;
	
	//Main Texture file
	param.strMainTextureFile = pLayer->GetTextureFile();

	GetTextureIDAndIdx(param.strMainTextureFile, param.nMainTexID, param.nMainTexIdx);

	//Main texture UV
	param.fMainTextureU = 1.0f / pLayer->GetScaleU();
	param.fMainTextureV = 1.0f / pLayer->GetScaleV();

	//Load Patch texture
	pLayer = pAutoTerrain->GetLayer(patchTexture + 1);

	//Patch texture
	param.strPatchTextureFile = pLayer->GetTextureFile();

	GetTextureIDAndIdx(param.strPatchTextureFile, param.nPatchTexID, param.nPatchTexIdx);
	
	param.fPatchTextureU = 1.0f / pLayer->GetScaleU();
	param.fPatchTextureV = 1.0f / pLayer->GetScaleV();

	
	return true;
}

bool CECHomeDlgsMgr::SaveTextureParamInfo(TextureParamInfo_t& param, TEXTURETYPE mainTexture, TEXTURETYPE patchTexture)
{
	CAutoTexture* pTexture = m_pAutoScene->GetAutoTerrain()->GetTexture();
	bool bOK = pTexture->ChangeTextureParam(
		mainTexture,
		patchTexture,
		param.fMainTextureU,
		param.fMainTextureV,
		param.fPatchTextureU,
		param.fPatchTextureV,
		param.strMainTextureFile,
		param.strPatchTextureFile);

	if( !bOK)
		return false;

	pTexture->SetTextureID(mainTexture, param.nMainTexID);
	pTexture->SetTextureID(patchTexture, param.nPatchTexID);
	
	return true;
}

bool CECHomeDlgsMgr::GetTextureIDAndIdx(const AString& strTexName, unsigned int& uTexID, int& uTexIdx)
{
	CAutoTexture* pAutoTexture = m_pAutoScene->GetAutoTerrain()->GetTexture();

	CAutoTexture::TEXTURETYPEINFO* pTexInfo = pAutoTexture->GetTextureInfos();
	if( !pTexInfo)
	{
		a_LogOutput(1, "CECHomeDlgsMgr::GetTextureIDAndIdx, Failed to Get CAutoTexture!");
		AutoHomeAssert();
	}

	int nNumTexInfos = pAutoTexture->GetNumTextureInfos();

	for( int i = 0; i < nNumTexInfos; ++i)
	{
		if( pTexInfo[i].strTexture == strTexName)
		{
			uTexID = pTexInfo[i].dwID;
			uTexIdx = i;
			return true;
		}
	}

	return false;

}

void CECHomeDlgsMgr::ShowHomeDlgs()
{
	m_pCurDlg->Show(false);

	m_pDlgAutoAllRes->Show(true);
	m_pDlgCurBase->Show(true);
	m_pDlgAutoSwitchDlg->Show(true);


	m_pDlgAutoTools->Show(true);
	m_pDlgAutoNote->Show(false);
	m_pDlgAutoNote2->Show(false);
	m_pDlgAutoCtrl->Show(false);

	m_pDlgAutoCamera->Show(true);
}

void CECHomeDlgsMgr::HideHomeDlgs()
{
	m_pCurDlg->Show(false);

	m_pDlgAutoAllRes->Show(false);
	m_pDlgCurBase->Show(false);
	m_pDlgCurIndoor->Show(false);
	m_pDlgCurOutdoor->Show(false);
	m_pDlgAutoSwitchDlg->Show(false);

	m_pDlgAutoTools->Show(false);
	m_pDlgAutoNote->Show(false);
	m_pDlgAutoNote2->Show(false);
	m_pDlgAutoCtrl->Show(false);

	m_pDlgAutoLight->Show(false);

	m_pDlgAutoCtrlEditArea->Show(false);
	
	m_pDlgAutoCamera->Show(false);
}



void CECHomeDlgsMgr::OnLButtonDownScreen(WPARAM wParam, LPARAM lParam)
{
	if( !m_pAutoScene)
		return;

	if( m_pDlgAutoTools->IsInTesting())
		return;
	
	APointI point;

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	point.x = GET_X_LPARAM(lParam)  - p->X;
	point.y = GET_Y_LPARAM(lParam)  - p->Y;
	
	int nType = m_pAutoScene->GetOperationType();
	CAutoSelectedArea* pSelArea = GetAutoScene()->GetAutoSelectedArea();

	switch( nType)
	{
	case OT_TEXTURE_POLY:
	case OT_HILL_POLY:
	case OT_TERRAIN_FLAT:
	case OT_TREE_POLY:
 	case OT_GRASS_POLY:
		{
			bool bClose = pSelArea->GetPolyEditAreaClosed();
			if(!bClose)
			{
				pSelArea->AddPolyEditPoint(point);
			}
		}
		break;

	case OT_ROAD:
	case OT_HILL_LINE:
 	case OT_TREE_LINE:
		{
			pSelArea->AddLineEditPoint(point);	
		}
		break;
	case OT_BUILDING:
		{
			if( m_pAutoScene->GetAutoBuildingOperation())
			{
				if( m_pAutoScene->GetAutoBuildingOperation()->GetBuildingOpera() != CAutoBuildingOperation::BP_ADD 
					&& m_pAutoScene->GetAutoBuildingOperation()->GetBuildingOpera() != CAutoBuildingOperation::BP_REPOS)
					break;

				if( m_pAutoScene->GetAutoBuildingOperation()->GetBuildingOpera() == CAutoBuildingOperation::BP_REPOS)
				{
					m_pAutoScene->GetAutoBuildingOperation()->OnLButtonDown(NULL, point);
					m_pDlgAutoAllRes->OnCommandCancelRes("");
					break;
				}

				m_pAutoScene->GetAutoBuildingOperation()->OnLButtonDown(NULL, point);
				
				if(!m_pAutoScene->GetAutoBuildingOperation()->GetModelFailed())
				{
					//Ëã¹â
					int nIdx = m_pAutoScene->GetAutoBuildingOperation()->GetSelectedModelIndex();	
					m_pAutoScene->GetAutoBuildingOperation()->CalBuildingLight(nIdx, true);

					CAutoBuilding* pBuilding = m_pAutoScene->GetAutoBuilding();
					int nNumBuilding = pBuilding->GetNumBuildings();
					ASSERT(nNumBuilding > 0);
					AArray<CAutoBuilding::MODELSTATUS, CAutoBuilding::MODELSTATUS&>* pAryStatus = pBuilding->GetModelStatus();
					unsigned int uUniqueID =(*pAryStatus)[nNumBuilding-1].dwUniqueID;
				

					switch((*pAryStatus)[nNumBuilding-1].dwAttribute)
					{
					case CAutoBuilding::MA_HOUSE:
						{
							m_pDlgAutoOutdoorBuilding->AddAllResTreeViewItem(uUniqueID);
							
						}
						break;

					case CAutoBuilding::MA_OUTDOOR:
						{
							m_pDlgAutoOutdoorOtherObject->AddAllResTreeViewItem(uUniqueID);
						}
						break;

					case CAutoBuilding::MA_FURNITURE:
						{
							m_pDlgAutoIndoorFurniture->AddAllResTreeViewItem(uUniqueID);
						}
						break;

					case CAutoBuilding::MA_PLANT:
						{
							m_pDlgAutoIndoorPlant->AddAllResTreeViewItem(uUniqueID);
						}
						break;

					case CAutoBuilding::MA_TOY:
						{
							m_pDlgAutoIndoorToy->AddAllResTreeViewItem(uUniqueID);

						}
						break;

					case CAutoBuilding::MA_VIRTU:
						{
							m_pDlgAutoIndoorVirtu->AddAllResTreeViewItem(uUniqueID);
						}
						break;

					default:
						break;
					}
					

				}

			}
			
		}
		break;
	default:
	
		break;
	}

	return;
}

void CECHomeDlgsMgr::HideAllAdvanceDlg()
{
	m_pDlgAutoLocalTexture->Show(false) ;
	m_pDlgAutoMountain->Show(false) ;
	m_pDlgAutoMountain2->Show(false);

	m_pDlgAutoRoad->Show(false);
	m_pDlgAutoTerrain->Show(false);
	m_pDlgAutoWater->Show(false);

	return;
}

void CECHomeDlgsMgr::SetModifying(bool i_bModifying)
{ 
	m_bModifying = i_bModifying;
	if( !i_bModifying)
	{
		m_pDlgAutoAllRes->SetBtnsEnable(true, false, true);
	}
}

void CECHomeDlgsMgr::TransformTexParamToTexInfo(const TextureParamInfo_t &texParam, TEXTUREINFO &texInfo)
{
	texInfo.dwMainTextureID = texParam.nMainTexID;
	texInfo.dwPatchTextureID = texParam.nPatchTexID;
	texInfo.fMainTextureU = texParam.fMainTextureU;
	texInfo.fMainTextureV = texParam.fMainTextureV;
	texInfo.fPatchTextureU = texParam.fPatchTextureU;
	texInfo.fPatchTextureV = texParam.fPatchTextureV;

}

void CECHomeDlgsMgr::TransformTexInfoToTexParam(const TEXTUREINFO &texInfo, TextureParamInfo_t &texParam)
{
	 texParam.nMainTexID = texInfo.dwMainTextureID;
	 texParam.nPatchTexID = texInfo.dwPatchTextureID;
	 texParam.fMainTextureU = texInfo.fMainTextureU;
	 texParam.fMainTextureV = texInfo.fMainTextureV;
	 texParam.fPatchTextureU = texInfo.fPatchTextureU;
	 texParam.fPatchTextureV = texInfo.fPatchTextureV;

	 CAutoTexture *pTex = GetAutoScene()->GetAutoTerrain()->GetTexture();
	 texParam.nMainTexIdx = pTex->GetTextureIndexByID(texParam.nMainTexID);
	 ASSERT( texParam.nMainTexIdx != -1);
	 texParam.nPatchTexIdx = pTex->GetTextureIndexByID(texParam.nPatchTexID);
	 ASSERT( texParam.nPatchTexIdx != -1);
	 
}

void CECHomeDlgsMgr::SetUnAdvDlgsDisable()
{
	m_pDlgAutoAllRes->SetEnable(false);
	m_pDlgCurBase->SetEnable(false);
	m_pDlgAutoSwitchDlg->SetEnable(false);
	m_pDlgAutoTools->SetEnable(false);
}

void CECHomeDlgsMgr::SetUnAdvDlgsEnable()
{
	m_pDlgAutoAllRes->SetEnable(true);
	m_pDlgCurBase->SetEnable(true);
	m_pDlgAutoSwitchDlg->SetEnable(true);
	m_pDlgAutoTools->SetEnable(true);
}
*/