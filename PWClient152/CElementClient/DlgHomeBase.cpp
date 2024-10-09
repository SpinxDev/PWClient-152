/*
 * FILE: DlgHomeBase.h
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
#include "DlgHomeBase.h"
#include "EC_HomeDlgsMgr.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"

#include "aui/AUIListBox.h"


AUI_BEGIN_COMMAND_MAP(CDlgHomeBase, AUIDialog)
AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_ON_COMMAND("ChooseTerrain", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseMountain", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseWater", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseRoad", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseLocalTexture", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChoosePlant", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBuilding", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseLight", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseTerrRes", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseOutdoorRes", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseIndoorRes", OnCommandSwitchDialog)

AUI_ON_COMMAND("ChooseBaseTerrain", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBaseMountain", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBaseMountain2", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBaseRoad", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBaseWater", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseBaseLocalTex", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseLevelOff", OnCommandSwitchDialog)

AUI_ON_COMMAND("ChooseOutdoorTree", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseOutdoorGrass", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseOutdoorBuilding", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseOutdoorOtherObject", OnCommandSwitchDialog)

AUI_ON_COMMAND("ChooseIndoorFurniture", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseIndoorToy", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseIndoorVirtu", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseIndoorPlant", OnCommandSwitchDialog)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgHomeBase, AUIDialog)

AUI_END_EVENT_MAP()

CDlgHomeBase::CDlgHomeBase() : m_pECHomeDlgsMgr(0), 
							   m_nActionCount(1)
{
}

CDlgHomeBase::~CDlgHomeBase()
{
}

bool CDlgHomeBase::InitContext(CECHomeDlgsMgr *pECHomeDlgsMgr)
{
	m_pECHomeDlgsMgr = pECHomeDlgsMgr;

	if( !m_pECHomeDlgsMgr)
		return false;

	if (!OnInitContext())
		return false;

	return true;
}

void CDlgHomeBase::OnShowDialog()
{
	AUIDialog::OnShowDialog();
}

void CDlgHomeBase::OnCommandSwitchDialog(const char *szCommand)
{
	GetHomeDlgsMgr()->SwitchDialog(szCommand);
}

int CDlgHomeBase::GetOperationType() const
{
	CAutoScene* pAutoScene = GetAutoScene();

	return pAutoScene->GetOperationType();
}

CAutoScene* CDlgHomeBase::GetAutoScene() const
{
	return m_pECHomeDlgsMgr->GetAutoScene();
}

void CDlgHomeBase::SetOperationType(int nType)
{
	CAutoScene* pAutoScene = GetAutoScene();

	pAutoScene->SetOperationType(nType);
	return;
}

CAutoParameters* CDlgHomeBase::GetAutoParam() const
{
	return m_pECHomeDlgsMgr->GetAutoParam();
}

bool CDlgHomeBase::IsValidIdx(int a_nIdx, AUIListBox* a_pListBox)
{
	if( !a_pListBox)
		return false;

	if( (a_nIdx < 0) || (a_nIdx >= a_pListBox->GetCount()))
		return false;

	return true;
}

void CDlgHomeBase::OnCommandCancel(const char* szCommand)
{
	
}

int CDlgHomeBase::Clamp(int &i_nVal, const int i_nUpper, const int i_nLower)
{
	if( i_nVal <= i_nLower) 
	{
		i_nVal = i_nLower;
		return i_nLower;
	}
	else if( i_nVal >= i_nUpper)
	{
		i_nVal = i_nUpper;
		return i_nUpper;
	}
	else
	{
		return i_nVal;
	}
}

CECGame * CDlgHomeBase::GetGame()
{
	return g_pGame;
}
*/