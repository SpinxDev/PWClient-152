/*
 * FILE: EC_ManDecal.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_ManDecal.h"
#include "EC_Decal.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Resource.h"
#include "EC_ImageRes.h"
#include "EC_Viewport.h"
#include "EC_PateText.h"

#include "A3DDevice.h"
#include "A3DFont.h"

#define new A_DEBUG_NEW

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
//	Implement CECDecalMan
//	
///////////////////////////////////////////////////////////////////////////

CECDecalMan::CECDecalMan(CECGameRun* pGameRun) :
CECManager(pGameRun)
{
}

CECDecalMan::~CECDecalMan()
{
}

//	Release manager
void CECDecalMan::Release()
{
	OnLeaveGameWorld();
}

//	On entering game world
bool CECDecalMan::OnEnterGameWorld()
{
	return true; 
}

//	On leaving game world
bool CECDecalMan::OnLeaveGameWorld()
{
	m_aDecals.RemoveAll();
	m_aPateTexts.RemoveAll();

	//	清除HostPlayer中保留的Decal指针，避免出现悬留指针导致客户端崩溃
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost)
		pHost->ClearBubbleText();

	//	Release all bubble decals
	ALISTPOSITION pos = m_BubbleList.GetHeadPosition();
	while (pos)
	{
		CECBubbleDecal* pDecal = m_BubbleList.GetNext(pos);
		delete pDecal;
	}

	m_BubbleList.RemoveAll();

	return true; 
}

//	Create a auto decal
CECBubbleDecal* CECDecalMan::CreateBubbleDecal(int iDCID)
{
	CECBubbleDecal* pDecal = new CECBubbleDecal(iDCID);
	if (!pDecal)
		return NULL;

	m_BubbleList.AddTail(pDecal);
	return pDecal;
}

//	Tick routine
bool CECDecalMan::Tick(DWORD dwDeltaTime)
{
	//	Tick bubble decals
	ALISTPOSITION pos = m_BubbleList.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posTemp = pos;
		CECBubbleDecal* pDecal = m_BubbleList.GetNext(pos);
		pDecal->Tick(dwDeltaTime);

		if (pDecal->IsDead())
		{
			m_BubbleList.RemoveAt(posTemp);
			delete pDecal;
		}
	}

	return true; 
}

//	Render routine
bool CECDecalMan::Render(CECViewport* pViewport)
{
	A3DViewport* pA3DView = pViewport->GetA3DViewport();
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();

	//	Disable the facular effect under water
	A3DTEXTUREOP opTex1 = pA3DDevice->GetTextureColorOP(1);
	pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetAlphaBlendEnable(true);
	pA3DDevice->SetZWriteEnable(false);

	//	Render normal decals
	for (int i=0; i < m_aDecals.GetSize(); i++)
	{
		CECDecal* pDecal = m_aDecals[i];
		pDecal->Render(pViewport);
	}

	m_aDecals.RemoveAll(false);

	//	Render bubble decals
	ALISTPOSITION pos = m_BubbleList.GetHeadPosition();
	while (pos)
	{
		CECBubbleDecal* pDecal = m_BubbleList.GetNext(pos);
		pDecal->Render(pViewport);
	}

	//	Flush text
	g_pGame->GetFont(RES_FONT_TITLE)->Flush();
	g_pGame->GetFont(RES_FONT_DAMAGE)->Flush();

	//	Flush all images
	g_pGame->GetImageRes()->Flush();

	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetTextureColorOP(1, opTex1);

	return true;
}

//	Render registered pate text
bool CECDecalMan::RenderPateTexts(CECViewport* pViewport)
{
	for (int i=0; i < m_aPateTexts.GetSize(); i++)
	{
		CECPateText* pPateText = m_aPateTexts[i];
		const CECPateText::REGRENDER& Info = pPateText->GetRegRender();
		pPateText->Render(pViewport, Info.x, Info.y, Info.col, Info.z);
	}

	m_aPateTexts.RemoveAll(false);
	
	A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
	pFont->Flush();

	return true;
}


