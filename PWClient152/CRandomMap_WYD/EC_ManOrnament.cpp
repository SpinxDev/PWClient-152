/*
 * FILE: EC_ManOrnament.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_Global.h"
#include "EC_ManOrnament.h"
#include "EC_Ornament.h"
#include "EC_HomeOrnament.h"
#include "EC_WorldFile.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Model.h"
#include "EC_GFXCaster.h"
#include "EC_Scene.h"
#include "EC_SceneObject.h"
#include "EC_Configs.h"
#include "EC_SceneLoader.h"
#include "EC_HostPlayer.h"
#include "EC_Viewport.h"
#include "EC_SunMoon.h"
#include "EC_SceneBlock.h"

#include "EC_CDS.h"
#include "EL_BrushBuilding.h"
#include "EL_BuildingWithBrush.h"
#include "EL_Forest.h"
#include "EC_BrushMan.h"
#include "EC_TriangleMan.h"

#include <A3DCamera.h>
#include <A3DViewport.h>
#include <A3DGFXEx.h>
#include <A3DFuncs.h>
#include <A3DFrame.h>
#include <A3DLitModel.h>
#include <AMSoundBufferMan.h>
#include <AM3DSoundBuffer.h>
#include <A3DLight.h>
#include <A3DTerrainWater.h>
#include <A3DCollision.h>

#include "A3DGFXElement.h"
#include "A3DGFXExMan.h"

#include "AutoScene.h"

#define new A_DEBUG_NEW

#ifdef USING_BRUSH_MAN
#define ORN_USE_BMAN_TRACE
//#define ORN_BMAN_DEBUG
#ifdef ORN_BMAN_DEBUG
#include "A3dFont.h"
ACHAR     msg[200];
#endif

#endif

#ifdef USING_TRIANGLE_MAN
//#define  ORN_TMAN_DEBUG   
#ifdef ORN_TMAN_DEBUG
#include "A3dFont.h"
#include "a3dflatcollector.h"

ACHAR     msg[200];
bool      bTestTMan = false;
TMan_Overlap_Info   tManInfo;
#define             ORN_TMAN_MAX_VERT 1000
A3DVECTOR3          tManVert[ORN_TMAN_MAX_VERT];
WORD                tManInx[ORN_TMAN_MAX_VERT];
#endif

#endif





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
//	Implement CECOrnamentMan
//	
///////////////////////////////////////////////////////////////////////////

CECOrnamentMan::CECOrnamentMan(CECGameRun* pGameRun) :
CECManager(pGameRun),
m_OrnamentTab(512),
m_HomeOrnamentTab(256),
m_GFXTab(128),
m_ECModelTab(128),
m_SoundTab(32)
{
#ifdef USING_BRUSH_MAN
	m_pBrushMan = new CECBrushMan();
#endif

#ifdef USING_TRIANGLE_MAN
	m_pTriangleMan = new CECTriangleMan();
	m_bUsingTriMan = false;
#endif

	m_SndTimeCnt.SetPeriod(1000);

	InitializeCriticalSection(&m_csLoad);
}

CECOrnamentMan::~CECOrnamentMan()
{
#ifdef USING_BRUSH_MAN
	m_pBrushMan->Release();
	delete m_pBrushMan;
#endif

#ifdef USING_TRIANGLE_MAN
	delete m_pTriangleMan;
#endif

	DeleteCriticalSection(&m_csLoad);
}

//	Release manager
void CECOrnamentMan::Release()
{
	OnLeaveGameWorld();
}

//	Tick routine
bool CECOrnamentMan::Tick(DWORD dwDeltaTime)
{
	//	Deliver loaded EC models
	DeliverLoadedECModels();

	float fDNFactor = m_pGameRun->GetWorld()->GetSunMoon()->GetDNFactor();

	if (m_OrnamentTab.size())
	{
		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();
			pOrnament->Tick(dwDeltaTime);
		}
	}

	if (m_HomeOrnamentTab.size())
	{
		HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
		for (; it != m_HomeOrnamentTab.end(); ++it)
		{
			CECHomeOrnament* pHomeOrnament = *it.value();
			pHomeOrnament->Tick(dwDeltaTime);
		}
	}

	if (m_ECModelTab.size())
	{
		ECModelTable::iterator it = m_ECModelTab.begin();
		for (; it != m_ECModelTab.end(); ++it)
		{
			ECMODELNODE* pNode = *it.value();
			if (pNode->pModel)
				pNode->pModel->Tick(dwDeltaTime);
		}
	}

	A3DVECTOR3 vHostPos = g_pGame->GetGameRun()->GetSafeHostPos(NULL);

	if( m_SoundTab.size() )
	{
		SoundTable::iterator it = m_SoundTab.begin();
		for (; it != m_SoundTab.end(); ++it)
		{
			SOUNDNODE* pNode = *it.value();
			
			if( pNode->pSound )
				pNode->pSound->Tick(dwDeltaTime);

			if (IsValidTime(fDNFactor, pNode->iValidTime))
			{
				if (!pNode->bLoaded)
				{
					LoadSoundFromFile(pNode);
					FadeSound(pNode->pSound, true);

					// only load one sound per frame
					break;
				}
				else if (pNode->pSound)
				{
					// see if need adjust according to distance to host player
					AMS3DBUFFERDESC* pDesc = pNode->pSound->Get3DBufferDesc();
					if (a3d_Magnitude(pDesc->vecPos - vHostPos) >= pDesc->vMaxDistance)
					{
						FadeSound(pNode->pSound, false);
					}
					else
					{
						FadeSound(pNode->pSound, true);
					}
				}
			}
			else
			{
				if (pNode->bLoaded)
					FadeSound(pNode->pSound, false);

				if (pNode->pSound && pNode->pSound->IsStopped())
				{
					//	Release sound data
					g_pGame->GetAMSoundBufferMan()->Release3DSound(&pNode->pSound);
					pNode->bLoaded = false;
					pNode->pSound = NULL;
				}
			}
		}
	}

	if( m_GFXTab.size() )
	{
		GFXTable::iterator it = m_GFXTab.begin();
		for (; it != m_GFXTab.end(); ++it)
		{
			GFXNODE* pNode = *it.value();
			
			if( pNode->pGfx )
				pNode->pGfx->TickAnimation(dwDeltaTime);

			if (IsValidTime(fDNFactor, pNode->iValidTime))
			{
				if (!pNode->bLoaded)
				{
					LoadGFXFromFile(pNode);
					FadeGFX(pNode, true);

					// only load one gfx per frame
					break;
				}
				else
				{
					if( pNode->bAttenuation )
					{
						// see if need adjust the gfx according to distance to host player
						if (a3d_Magnitude(pNode->vPos - vHostPos) >= CECSceneBlock::GetResLoadDists(CECSceneBlock::SCNRES_EFFECT))
						{
							FadeGFX(pNode, false);
						}
						else
						{
							FadeGFX(pNode, true);
						}
					}
				}
			}
			else
			{
				if (pNode->bLoaded)
					FadeGFX(pNode, false);

				if (pNode->pGfx && (pNode->pGfx->GetState() == ST_STOP || (!pNode->pGfx->InAlphaTransition() && pNode->pGfx->GetAlpha() == 0.0f)) )
				{
					//	Release gfx data
					pNode->pGfx->Release();
					delete pNode->pGfx;
					pNode->bLoaded = false;
					pNode->pGfx= NULL;
				}
			}
		}
	}

	return true; 
}

//	Render routine
bool CECOrnamentMan::Render(CECViewport* pViewport)
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();

	if (m_OrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);

		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();
			pOrnament->Render(pViewport);
		}

		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}

	if (m_HomeOrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);

		HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
		for (; it != m_HomeOrnamentTab.end(); ++it)
		{
			CECHomeOrnament* pHomeOrnament = *it.value();
			pHomeOrnament->Render(pViewport);
		}

		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}

	if (m_ECModelTab.size())
	{
		A3DVECTOR3 vecCamPos = pViewport->GetA3DCamera()->GetPos();

		ECModelTable::iterator it = m_ECModelTab.begin();
		for (; it != m_ECModelTab.end(); ++it)
		{
			ECMODELNODE* pNode = *it.value();
			if (pNode->pModel)
			{
				// we show shadow for building like ecmodel
				if( MagnitudeH(pNode->pModel->GetPos() - vecCamPos) < 120.0f )
					pNode->pModel->SetCastShadow(true);
				else
					pNode->pModel->SetCastShadow(false);
				pNode->pModel->Render(pViewport->GetA3DViewport());
			}
		}
	}

	if( m_GFXTab.size() )
	{
		GFXTable::iterator it = m_GFXTab.begin();
		for (; it != m_GFXTab.end(); ++it)
		{
			GFXNODE* pNode = *it.value();
			
			if( pNode->pGfx )
			{
				g_pGame->GetGFXCaster()->GetGFXExMan()->RegisterGfx(pNode->pGfx);
			}
		}
	}
#ifdef ORN_BMAN_DEBUG
#if  BMAN_VERBOSE_STAT
	A3DFont * pFont = g_pGame->GetFont(0);
	int y = 300;
	CBManStat  * pStat = m_pBrushMan->GetStat();
	a_sprintf(msg, _AL("build %d prv %d brush %d "), 
		pStat->dwBuildTime, pStat->nProvider, pStat->nBrush);
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;
	a_sprintf(msg, _AL("trace %d brush %d "), 
		pStat->dwTraceTime, pStat->nTraceBrush);
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;
#ifdef BMAN_USE_GRID
	a_sprintf(msg, _AL("cellsize %d wid %d  hei %d outofrangebrush %d"), 
		pStat->nCellSize, pStat->nWidth, pStat->nHeight, pStat->nOutOfRangeBrush);
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;
#endif		
#endif	
#endif

#ifdef ORN_TMAN_DEBUG
#ifdef  TMAN_VERBOSE_STAT
	A3DFont * pFont = g_pGame->GetFont(0);
	int y = 300;
	CTManStat  * pStat = m_pTriangleMan->GetStat();
	a_sprintf(msg, _AL("cellsize %d w %d h %d  n %d"), 
		pStat->nCellSize, pStat->nWidth, pStat->nHeight, pStat->pCellTbl->size());
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;
	a_sprintf(msg, _AL("build %d prv %d unorgprv %d tri %d add %d"), 
		pStat->dwBuildTime, pStat->nProvider, pStat->nUnorgPrv, pStat->nTriangle, pStat->nAddTriangle);
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;
	a_sprintf(msg, _AL("check %d checktri %d "), 
		pStat->dwTestTime, pStat->nTestTriangle);
	pFont->TextOut(50, y, msg, A3DCOLORRGB(255, 0, 0) );
	y += 16;

#endif	
#endif

	
	return true; 
}

//	RenderForReflected routine
bool CECOrnamentMan::RenderForReflect(CECViewport * pViewport)
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	CECConfigs* pConfigs = g_pGame->GetConfigs();

	if (m_OrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();

			if( pConfigs->GetSystemSettings().nWaterEffect == 2 )
			{
				if( pOrnament->IsAboveWater() )
					pOrnament->Render(pViewport);
			}
			else
			{
				if( pOrnament->GetReflectFlag() )
					pOrnament->Render(pViewport);
			}
		}
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}

	if (m_HomeOrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
		HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
		for (; it != m_HomeOrnamentTab.end(); ++it)
		{
			CECHomeOrnament* pHomeOrnament = *it.value();

			if( pConfigs->GetSystemSettings().nWaterEffect == 2 )
			{
				pHomeOrnament->Render(pViewport);
			}
		}
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}


	if (m_ECModelTab.size() && pConfigs->GetSystemSettings().nWaterEffect == 2)
	{
		A3DVECTOR3 vecCamPos = pViewport->GetA3DCamera()->GetPos();

		ECModelTable::iterator it = m_ECModelTab.begin();
		for (; it != m_ECModelTab.end(); ++it)
		{
			ECMODELNODE* pNode = *it.value();
			if (pNode->pModel)
			{
				// we show shadow for building like ecmodel
				pNode->pModel->SetCastShadow(false);
				pNode->pModel->Render(pViewport->GetA3DViewport());
			}
		}
	}

	return true;
}

//	RenderForRefract routine
bool CECOrnamentMan::RenderForRefract(CECViewport * pViewport)
{
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	CECConfigs* pConfigs = g_pGame->GetConfigs();

	A3DTerrainWater * pTerrainWater = g_pGame->GetGameRun()->GetWorld()->GetTerrainWater();
	bool bCamUnderWater = pTerrainWater->IsUnderWater(pViewport->GetA3DViewport()->GetCamera()->GetPos());

	if (m_OrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();

			if( pConfigs->GetSystemSettings().nWaterEffect == 2 )
			{
				if( !bCamUnderWater && pOrnament->IsUnderWater() )
					pOrnament->Render(pViewport);
				else if( bCamUnderWater && pOrnament->IsAboveWater() )
					pOrnament->Render(pViewport);
			}
			else
			{
				if( pOrnament->GetRefractFlag() )
					pOrnament->Render(pViewport);
			}
		}
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}

	if (m_HomeOrnamentTab.size())
	{
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
		HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
		for (; it != m_HomeOrnamentTab.end(); ++it)
		{
			CECHomeOrnament* pHomeOrnament = *it.value();

			if( pConfigs->GetSystemSettings().nWaterEffect == 2 )
			{
				pHomeOrnament->Render(pViewport);
			}
		}
		pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	}

	if (m_ECModelTab.size() && bCamUnderWater && pConfigs->GetSystemSettings().nWaterEffect == 2)
	{
		A3DVECTOR3 vecCamPos = pViewport->GetA3DCamera()->GetPos();

		ECModelTable::iterator it = m_ECModelTab.begin();
		for (; it != m_ECModelTab.end(); ++it)
		{
			ECMODELNODE* pNode = *it.value();
			if (pNode->pModel)
			{
				// we show shadow for building like ecmodel
				pNode->pModel->SetCastShadow(false);
				pNode->pModel->Render(pViewport->GetA3DViewport());
			}
		}
	}

	return true;
}

//	On entering game world
bool CECOrnamentMan::OnEnterGameWorld()
{
	return true; 
}

void CECOrnamentMan::LoadTriangleMan(const A3DVECTOR3& vCenter)
{
	if (m_bUsingTriMan)
		return;

	m_bUsingTriMan = true;

	for (OnmtTable::iterator it = m_OrnamentTab.begin(); it != m_OrnamentTab.end(); ++it)
	{
		CECOrnament* pOrnament = *it.value();

		if (!pOrnament->IsAddedToTriMan() && pOrnament->GetBrushBuilding())
		{
			m_pTriangleMan->AddProvider(pOrnament->GetBrushBuilding());
			pOrnament->SetAddedToTriMan(true);
		}
	}

	m_pTriangleMan->Build(vCenter, TMAN_BUILD_FORCE);
}

void CECOrnamentMan::ReleaseTriangleMan(const A3DVECTOR3& vCenter)
{
	if (!m_bUsingTriMan)
		return;

	m_bUsingTriMan = false;

	for (OnmtTable::iterator it = m_OrnamentTab.begin(); it != m_OrnamentTab.end(); ++it)
	{
		CECOrnament* pOrnament = *it.value();

		if (pOrnament->IsAddedToTriMan())
		{
			m_pTriangleMan->RemoveProvider(pOrnament->GetBrushBuilding());
			pOrnament->SetAddedToTriMan(false);
		}
	}

	m_pTriangleMan->Build(vCenter, TMAN_BUILD_FORCE);
}

//	On leaving game world
bool CECOrnamentMan::OnLeaveGameWorld()
{
	//	Release all orenaments
	if (m_OrnamentTab.size())
	{
		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();

			if (pOrnament->IsAddedToBrushMan())
			{
#ifdef USING_BRUSH_MAN
				m_pBrushMan->RemoveProvider(pOrnament->GetBrushBuilding());
#endif
			}

			if (pOrnament->IsAddedToTriMan())
			{
#ifdef USING_TRIANGLE_MAN
				m_pTriangleMan->RemoveProvider(pOrnament->GetBrushBuilding());
#endif
			}

			QueueReleaseInThread(pOrnament);
		}

		m_OrnamentTab.clear();
	}

	if (m_HomeOrnamentTab.size())
	{
		HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
		for (; it != m_HomeOrnamentTab.end(); ++it)
		{
			CECHomeOrnament* pHomeOrnament = *it.value();
			QueueReleaseInThread(pHomeOrnament);
		}

		m_HomeOrnamentTab.clear();
	}

	//	Release all EC models
	if (m_ECModelTab.size())
	{
		ECModelTable::iterator it = m_ECModelTab.begin();
		for (; it != m_ECModelTab.end(); ++it)
		{
			ECMODELNODE* pNode = *it.value();
			if (pNode->pModel)
			{
				pNode->pModel->Release();
				delete pNode->pModel;
			}
			
			for (int i = 0; i < pNode->nBrushes; i++)
				delete pNode->ppBrushes[i];

			delete[] pNode->ppBrushes;
			delete pNode;
		}

		m_ECModelTab.clear();
	}

	//	Release all GFX
	if (m_GFXTab.size())
	{
		GFXTable::iterator it = m_GFXTab.begin();
		for (; it != m_GFXTab.end(); ++it)
		{
			GFXNODE * pNode = *it.value();

			if( pNode->pGfx )
			{
				pNode->pGfx->Release();
				delete pNode->pGfx;
				pNode->pGfx = NULL;
			}

			delete pNode;
		}

		m_GFXTab.clear();
	}

	//	Release all sounds object
	if (m_SoundTab.size())
	{
		AMSoundBufferMan* pSoundMan = g_pGame->GetAMSoundBufferMan();

		SoundTable::iterator it = m_SoundTab.begin();
		for (; it != m_SoundTab.end(); ++it)
		{
			SOUNDNODE* pNode = *it.value();
			if (pNode->pSound)
				pSoundMan->Release3DSound(&pNode->pSound);
			
			delete pNode;
		}

		m_SoundTab.clear();
	}

	//	Release all loaded models
	ACSWrapper csa(&m_csLoad);

	for (int i=0; i < m_aLoadedECMs.GetSize(); i++)
	{
		const LDECMODEL& m = m_aLoadedECMs[i];
		if (m.pModel)
		{
			m.pModel->Release();
			delete m.pModel;
		}
	}

	m_aLoadedECMs.RemoveAll();

	return true; 
}

//	Process message
bool CECOrnamentMan::ProcessMessage(const ECMSG& Msg)
{ 
	return true; 
}

//	Load ornament from file
CECOrnament* CECOrnamentMan::LoadOrnament(DWORD dwID, CECScene* pScene, const char* szMapPath, 
										  float fOffX, float fOffZ)
{
	CECOrnament* pOrnament = new CECOrnament(this);
	if (!pOrnament)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECOrnament::LoadOrnament", __LINE__);
		return false;
	}

	if (!pOrnament->Load(pScene, szMapPath, fOffX, fOffZ))
	{
		a_LogOutput(1, "CECOrnament::LoadOrnament, failed to load ornament");
		return false;
	}

	if (pScene->GetSceneFileVersion() >= 4)
		pOrnament->SetOrnamentID(dwID);

	m_OrnamentTab.put((int)pOrnament->GetOrnamentID(), pOrnament);
	return pOrnament;
}

//	Release ornament of specified ID
void CECOrnamentMan::ReleaseOrnament(DWORD dwOnmtID)
{
	CECOrnament* pOrnament = GetOrnament(dwOnmtID);
	if (!pOrnament)
		return;

	if (pOrnament->IsAddedToBrushMan())
	{
#ifdef USING_BRUSH_MAN
		m_pBrushMan->RemoveProvider(pOrnament->GetBrushBuilding());
#endif
	}

	if (pOrnament->IsAddedToTriMan())
	{
#ifdef USING_TRIANGLE_MAN
		m_pTriangleMan->RemoveProvider(pOrnament->GetBrushBuilding());
#endif
	}

	//	Remove form table
	m_OrnamentTab.erase(dwOnmtID);
	QueueReleaseInThread(pOrnament);
}

//	Get ornament through it's ID
CECOrnament* CECOrnamentMan::GetOrnament(DWORD dwOnmtID)
{
	//	Ornament exists ?
	OnmtTable::pair_type Pair = m_OrnamentTab.get((int)dwOnmtID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this ornament

	return *Pair.first;
}

//	Load home ornament from file
CECHomeOrnament* CECOrnamentMan::LoadHomeOrnament(DWORD dwID, CAutoScene* pScene, const char* szMoxFile, const char * szCHFFile, const A3DMATRIX4& matTM)
{
	CECHomeOrnament* pHomeOrnament = new CECHomeOrnament(this);
	if (!pHomeOrnament)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECOrnament::LoadHomeOrnament", __LINE__);
		return false;
	}

	if (!pHomeOrnament->Load(pScene, szMoxFile, szCHFFile, matTM))
	{
		a_LogOutput(1, "CECOrnament::LoadHomeOrnament, failed to load home ornament");
		return false;
	}

	pHomeOrnament->SetOrnamentID(dwID);

	m_HomeOrnamentTab.put((int)pHomeOrnament->GetOrnamentID(), pHomeOrnament);
	return pHomeOrnament;
}

//	Release ornament of specified ID
void CECOrnamentMan::ReleaseHomeOrnament(DWORD dwOnmtID)
{
	CECHomeOrnament* pHomeOrnament = GetHomeOrnament(dwOnmtID);
	if (!pHomeOrnament)
		return;

	//	Remove form table
	m_HomeOrnamentTab.erase(dwOnmtID);
	QueueReleaseInThread(pHomeOrnament);
}

//	Get ornament through it's ID
CECHomeOrnament* CECOrnamentMan::GetHomeOrnament(DWORD dwOnmtID)
{
	//	Ornament exists ?
	HomeOnmtTable::pair_type Pair = m_HomeOrnamentTab.get((int)dwOnmtID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this ornament

	return *Pair.first;
}

//	Load effect from file
DWORD CECOrnamentMan::LoadEffect(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ)
{
	DWORD dwRead;
	ECWDFILEEFFECT12 Data;

	if (pScene->GetSceneFileVersion() < 6)
	{
		ECWDFILEEFFECT TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return 0;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.fSpeed			= 1.0f;
		Data.iValidTime		= 2;	//	24-hour valid
		Data.fAlpha			= 1.0f;
		Data.bAttenuation	= true;
	}
	else if (pScene->GetSceneFileVersion() < 8)
	{
		ECWDFILEEFFECT6 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return 0;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.iValidTime		= 2;	//	24-hour valid
		Data.fAlpha			= 1.0f;
		Data.bAttenuation	= true;
	}
	else if (pScene->GetSceneFileVersion() < 12)
	{
		ECWDFILEEFFECT8 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return 0;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.fAlpha			= 1.0f;
		Data.bAttenuation	= true;
	}
	else	//	pScene->GetSceneFileVersion() >= 12
	{
		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return 0;
	}

	AString strGFXFile;
	if (!pFile->ReadString(strGFXFile))
		return 0;

	//	Create a gfx node
	GFXNODE* pNode = new GFXNODE;
	if (!pNode)
		return 0;

	pNode->strFile		= strGFXFile;
	pNode->vPos.Set(Data.vPos[0]+fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
	pNode->vDir.Set(Data.vDir[0], Data.vDir[1], Data.vDir[2]);
	pNode->vUp.Set(Data.vUp[0], Data.vUp[1], Data.vUp[2]);
	pNode->fScale		= Data.fScale;
	pNode->fSpeed		= Data.fSpeed;
	pNode->iValidTime	= Data.iValidTime;
	pNode->fAlpha		= Data.fAlpha;
	pNode->bAttenuation	= Data.bAttenuation;
	pNode->max_alpha	= 255;
	pNode->pGfx			= NULL;
	pNode->bLoaded 		= false;

	//	Add to table
	m_GFXTab.put(Data.dwExportID, pNode);

	return Data.dwExportID;
}

//	Release effect of specified ID
void CECOrnamentMan::ReleaseEffect(DWORD dwEffectID)
{
	GFXNODE * pNode = GetEffect(dwEffectID);
	if (!pNode)
		return;

	if( pNode->pGfx )
	{
		pNode->pGfx->Release();
		delete pNode->pGfx;
		pNode->pGfx = NULL;
	}

	delete pNode;

	//	Remove from table
	m_GFXTab.erase(dwEffectID);
}

bool CECOrnamentMan::LoadGFXFromFile(GFXNODE * pNode)
{
	if( pNode->bLoaded )
		return true;

	pNode->bLoaded = true;
	A3DGFXEx* pGFX = g_pGame->GetGFXCaster()->GetGFXExMan()->LoadGfx(g_pGame->GetA3DDevice(), pNode->strFile, false);
	if (!pGFX)
		return 0;

	A3DMATRIX4 mat = a3d_TransformMatrix(pNode->vDir, pNode->vUp, pNode->vPos);
	pGFX->SetScale(pNode->fScale);
	pGFX->SetPlaySpeed(pNode->fSpeed);
	pGFX->SetParentTM(mat);

	pNode->max_alpha = 1.0f;
	pNode->pGfx = pGFX;
	return true;
}

//	Fade in/out gfx
void CECOrnamentMan::FadeGFX(GFXNODE * pNode, bool bFadeIn)
{
	A3DGFXEx * pGfx = pNode->pGfx;
	float a = pNode->max_alpha * pNode->fAlpha;
	if (pGfx)
	{
		if( bFadeIn )
		{
			if( pGfx->GetState() == ST_STOP )
			{
				pGfx->Start(true);
				pGfx->SetAlpha(0.0f * a);
				pGfx->SetAlphaTransition(1.0f * a, 3000);
			}
			else if( !pGfx->AlphaIsInc() )
				pGfx->SetAlphaTransition(1.0f * a, 3000);
		}
		else
		{
			if( pGfx->GetState() == ST_PLAY )
			{
				if( pGfx->GetAlpha() == 0.0f * a )
					pGfx->Stop();
				else
				{
					if( !pGfx->InAlphaTransition() || pGfx->AlphaIsInc() )
						pGfx->SetAlphaTransition(0.0f * a, 3000);
				}
			}
		}
	}
}

//	Get effect through it's ID
CECOrnamentMan::GFXNODE * CECOrnamentMan::GetEffect(DWORD dwEffectID)
{
	//	GFX exists ?
	GFXTable::pair_type Pair = m_GFXTab.get((int)dwEffectID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this effect

	return *Pair.first;
}

//	Load static EC model from file
DWORD CECOrnamentMan::LoadECModel(AFile* pFile, float fOffX, float fOffZ)
{
	DWORD dwRead;
	ECWDFILEECMODEL Data;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead))
		return 0;

	AString strModelFile, strActName;
	if (!pFile->ReadString(strModelFile))
		return 0;

	if (!pFile->ReadString(strActName))
		return 0;

	A3DVECTOR3 vPos(Data.vPos[0] + fOffX, Data.vPos[1], Data.vPos[2] + fOffZ);
	A3DVECTOR3 vDir(Data.vDir[0], Data.vDir[1], Data.vDir[2]);
	A3DVECTOR3 vUp(Data.vUp[0], Data.vUp[1], Data.vUp[2]);

	CECModel* pModel = NULL;

	if (IsLoadThreadReady())
	{
		//	Submit EC model loading request to loading thread
		QueueECModelForLoad(MTL_ECM_SCNMODEL, (int)Data.dwExportID, 0, vPos, strModelFile);
	}
	else
	{
		//	Load model object immediately
		pModel = LoadECModelFromFile(strModelFile);

		if (pModel)
		{
			pModel->SetPos(vPos);
			pModel->SetDirAndUp(vDir, vUp);
			if( pModel->GetComActByName(strActName) )
				pModel->PlayActionByName(strActName, 1.0f);
			else
			{
				pModel->PlayActionByName("ÐÝÏÐ", 1.0f);
			}
			pModel->Tick(rand() % 1000);
		}
	}

	//	Add model to table
	ECMODELNODE* pNode = new ECMODELNODE;
	pNode->nBrushes = 0;
	pNode->ppBrushes = NULL;
	pNode->pModel = pModel;
	pNode->vInitPos = vPos;
	pNode->vInitDir = vDir;
	pNode->vInitUp = vUp;
	pNode->strActName = strActName;

	// now construct a CCDBrush from CConvexhullData;
	if( pModel && pModel->HasCHAABB() )
	{
		ConvexHullDataArray& cdArray = pModel->GetConvexHullData();

		A3DMATRIX4 matTM = TransformMatrix(vDir, vUp, vPos);
		if( int(cdArray.size()) != pNode->nBrushes )
		{
			pNode->nBrushes = cdArray.size();
			pNode->ppBrushes = new CCDBrush *[cdArray.size()];

			for(int i=0; i<pNode->nBrushes; i++)
			{
				CConvexHullData chData = *cdArray[i];
				chData.Transform(matTM);

				CQBrush qBrush;
				qBrush.AddBrushBevels(&chData);
				CCDBrush * pCDBrush = new CCDBrush();
				qBrush.Export(pCDBrush);

				pNode->ppBrushes[i] = pCDBrush;
			}
		}
	}

	m_ECModelTab.put((int)Data.dwExportID, pNode);

	return Data.dwExportID;
}

//	Release static EC model of specified ID
void CECOrnamentMan::ReleaseECModel(DWORD dwModelID)
{
	ECMODELNODE* pNode = GetECModel(dwModelID);
	if (!pNode)
		return;

	if (pNode->pModel)
		DeleteECModel(pNode->pModel);

	// now release brush objects if has
	for(int i=0; i<pNode->nBrushes; i++)
	{
		if( pNode->ppBrushes[i]	)
		{
			delete pNode->ppBrushes[i];
			pNode->ppBrushes[i] = NULL;
		}
	}
	if( pNode->ppBrushes )
	{
		delete [] pNode->ppBrushes;
		pNode->ppBrushes = NULL;
	}
	
	delete pNode;

	//	Remove from table
	m_ECModelTab.erase((int)dwModelID);
}

//	Load EC model from file
CECModel* CECOrnamentMan::LoadECModelFromFile(const char* szFile)
{
	CECModel* pModel = new CECModel;
	if (!pModel)
		return NULL;
	
	if (!pModel->Load(szFile))
	{
		// now try to load error ecm file
		if (!pModel->Load("models\\error\\error.ecm"))
		{
			a_LogOutput(1, "CECOrnamentMan::LoadECModelFromFile, Failed to load ec model %s", szFile);
			delete pModel;
			return NULL;
		}
	}

	pModel->SetAABBType(CECModel::AABB_AUTOSEL);
	pModel->SetAutoUpdateFlag(false);
	
	return pModel;
}

//	Delete EC model object
void CECOrnamentMan::DeleteECModel(CECModel* pModel)
{
	if (!pModel)
		return;

	if (IsLoadThreadReady())
		QueueECModelForRelease(pModel);
	else
	{
		pModel->Release();
		delete pModel;
	}
}

//	Get static EC model through it's ID
CECOrnamentMan::ECMODELNODE* CECOrnamentMan::GetECModel(DWORD dwModelID)
{
	//	EC model exists ?
	ECModelTable::pair_type Pair = m_ECModelTab.get((int)dwModelID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this model

	return *Pair.first;
}

//	Load sound object from file
DWORD CECOrnamentMan::LoadSoundObject(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ)
{
	DWORD dwRead;
	ECWDFILESOUND10 Data;

	if (pScene->GetSceneFileVersion() < 10)
	{
		ECWDFILESOUND TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return 0;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.iValidTime	= 2;
	}
	else	//	pScene->GetSceneFileVersion() >= 10
	{
		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return 0;
	}

	AString strSndFile;
	if (!pFile->ReadString(strSndFile))
		return 0;

	//	Create a sound node
	SOUNDNODE* pNode = new SOUNDNODE;
	if (!pNode)
		return 0;

	pNode->vPos.Set(Data.vPos[0]+fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
	pNode->strFile		= "sfx\\" + strSndFile;
	pNode->fMinDist		= Data.fMinDist;
	pNode->fMaxDist		= Data.fMaxDist;
	pNode->iValidTime	= Data.iValidTime;
	pNode->pSound		= NULL;
	pNode->bLoaded 		= false;

	//	Add to table
	m_SoundTab.put((int)Data.dwExportID, pNode);

	return Data.dwExportID;
}

//	Release sound object
void CECOrnamentMan::ReleaseSoundObject(DWORD dwSoundID)
{
	SOUNDNODE* pNode = GetSoundObject(dwSoundID);
	if (!pNode)
		return;

	if (pNode->pSound)
		g_pGame->GetAMSoundBufferMan()->Release3DSound(&pNode->pSound);
	
	delete pNode;

	//	Remove from table
	m_SoundTab.erase((int)dwSoundID);
}

//	Load sound from file
bool CECOrnamentMan::LoadSoundFromFile(SOUNDNODE* pNode)
{
	if (!pNode || pNode->bLoaded)
		return true;

	pNode->bLoaded = true;

	AM3DSoundBuffer* pSound = g_pGame->GetAMSoundBufferMan()->Load3DSound(pNode->strFile, false);
	if (!pSound)
	{
		a_LogOutput(1, "CECOrnamentMan::LoadSoundFromFile, Failed to load sound %s", pNode->strFile);
		return 0;
	}

	AMS3DBUFFERDESC * pDesc = pSound->Get3DBufferDesc();
	pDesc->vecPos = pNode->vPos;
	pDesc->vecConeOrientation = g_vAxisY;
	pDesc->vMaxDistance = pNode->fMaxDist;
	pDesc->vMinDistance = pNode->fMinDist;
	pSound->UpdateChanges();

	pNode->pSound = pSound;

	return true;
}

//	Fade in/out sound
void CECOrnamentMan::FadeSound(AM3DSoundBuffer * pSound, bool bFadeIn)
{
	if (pSound)
	{
		if( bFadeIn )
		{
			if( !pSound->IsPlaying() )
			{
				pSound->SetVolume(0);
				pSound->Play(true);
				pSound->FadeIn(3000);
			}
			else if( !pSound->IsFadeIn() )
				pSound->FadeIn(3000);
		}
		else
		{
			if( pSound->IsPlaying() && !pSound->IsFadeOut() )
				pSound->FadeOut(3000);
		}
	}
}

//	Get effect through it's ID
CECOrnamentMan::SOUNDNODE* CECOrnamentMan::GetSoundObject(DWORD dwSoundID)
{
	//	sound exists ?
	SoundTable::pair_type Pair = m_SoundTab.get((int)dwSoundID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this sound

	return *Pair.first;
}

//	Ray trace
bool CECOrnamentMan::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, float * pFraction, A3DVECTOR3& vNormal)
{
	BrushTraceInfo btInfo;
	btInfo.Init(vStart, vDelta, A3DVECTOR3(0.0f), true);
	bool bCollide =	TraceWithBrush(&btInfo);
	if (bCollide) {
		vHitPos = vStart + vDelta * btInfo.fFraction;
		*pFraction = btInfo.fFraction;
		vNormal = btInfo.ClipPlane.GetNormal();	
	}
	return bCollide;
}
#ifdef  ORN_USE_BMAN_TRACE
bool CECOrnamentMan::RayTrace(ECRAYTRACE* pTraceInfo)
{

	BrushTraceInfo brhInfo;
	BrushTraceInfo * pInfo = &brhInfo;

	pInfo->Init(pTraceInfo->vStart, pTraceInfo->vDelta, A3DVECTOR3(0), true);
	bool bRet=false;
	
	if ((pTraceInfo->pTraceRt->fFraction > 0.0f) 
		&& m_pBrushMan && m_pBrushMan->Trace(pInfo) 
		&& (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction ))
	{
		pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
		pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
		pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
		pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

		pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
		pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
		pTraceInfo->pECTraceRt->iObjectID	= 0;
		pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
		bRet = true;
	}	

	HomeOnmtTable::iterator it1 = m_HomeOrnamentTab.begin();
	for (; it1 != m_HomeOrnamentTab.end(); ++it1)
	{
		CECHomeOrnament* pHomeOrnament = *it1.value();
		if (!pHomeOrnament->IsLoaded())
			continue;

		CELBuildingWithBrush* pBuildingWithBrush = pHomeOrnament->GetBuildingWithBrush();
		if(pBuildingWithBrush && pBuildingWithBrush->TraceWithBrush(pInfo)  && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction )){
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
				pTraceInfo->pECTraceRt->iObjectID	= (int)pHomeOrnament;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
		}
	}		

	// now see if collide with forest
	CELForest * pForest = g_pGame->GetGameRun()->GetWorld()->GetForest();
	if ( (pTraceInfo->pTraceRt->fFraction > 0.0f)
		&& pForest && pForest->TraceWithBrush(pInfo) 
		&& (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction  ) ) 
	{
		pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
		pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
		pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
		pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

		pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
		pTraceInfo->pECTraceRt->iEntity		= ECENT_FOREST;
		pTraceInfo->pECTraceRt->iObjectID	= 0;
		pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
		bRet = true;
	}

	// now see if collide with dynamic scene building
	ECModelTable::iterator it = m_ECModelTab.begin();
	for (; it != m_ECModelTab.end(); ++it)
	{
		ECMODELNODE* pNode = *it.value();
		if(pNode && pNode->TraceWithBrush(pInfo)  && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction )){
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
				pTraceInfo->pECTraceRt->iObjectID	= 0;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
		}
	}		

	return bRet;


}

#else
bool CECOrnamentMan::RayTrace(ECRAYTRACE* pTraceInfo)
{

	BrushTraceInfo brhInfo;
	BrushTraceInfo * pInfo = &brhInfo;

	pInfo->Init(pTraceInfo->vStart, pTraceInfo->vDelta, A3DVECTOR3(0), true);
	bool bRet=false;
	//save original result
		
	OnmtTable::iterator it = m_OrnamentTab.begin();
	for (; it != m_OrnamentTab.end(); ++it)
	{
		CECOrnament* pOrnament = *it.value();
		if (!pOrnament->IsLoaded())
			continue;
		
		CELBrushBuilding* pBrushBuilding = pOrnament->GetBrushBuilding();
		if(pBrushBuilding && pBrushBuilding->TraceWithBrush(pInfo)  && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction )){
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
				pTraceInfo->pECTraceRt->iObjectID	= (int)pOrnament;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
		}
	}		

	HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
	for (; it != m_HomeOrnamentTab.end(); ++it)
	{
		CECHomeOrnament* pHomeOrnament = *it.value();
		if (!pHomeOrnament->IsLoaded())
			continue;

		CELBuildingWithBrush* pBuildingWithBrush = pHomeOrnament->GetBuildingWithBrush();
		if(pBuildingWithBrush && pBuildingWithBrush->TraceWithBrush(pInfo)  && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction )){
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
				pTraceInfo->pECTraceRt->iObjectID	= (int)pHomeOrnament;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
		}
	}		

	// now see if collide with forest
	CELForest * pForest = g_pGame->GetGameRun()->GetWorld()->GetForest();
	if (pForest && pForest->TraceWithBrush(pInfo) && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction  ) ) {
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_FOREST;
				pTraceInfo->pECTraceRt->iObjectID	= 0;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
	}

	// now see if collide with dynamic scene building
	ECModelTable::iterator it = m_ECModelTab.begin();
	for (; it != m_ECModelTab.end(); ++it)
	{
		ECMODELNODE* pNode = *it.value();
		if(pNode && pNode->TraceWithBrush(pInfo)  && (pInfo->fFraction < pTraceInfo->pTraceRt->fFraction )){
				pTraceInfo->pTraceRt->fFraction		= pInfo->fFraction;
				pTraceInfo->pTraceRt->vHitPos		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vPoint		= pInfo->vStart + pInfo->vDelta * pInfo->fFraction;
				pTraceInfo->pTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();

				pTraceInfo->pECTraceRt->fFraction	= pInfo->fFraction;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_BUILDING;
				pTraceInfo->pECTraceRt->iObjectID	= 0;
				pTraceInfo->pECTraceRt->vNormal		= pInfo->ClipPlane.GetNormal();
				bRet = true;
		}
	}		

	return bRet;


}
#endif

// brush trace
#ifdef  ORN_USE_BMAN_TRACE
//trace with brushman
bool CECOrnamentMan::TraceWithBrush(BrushTraceInfo * pInfo)
{

	bool bCollide=false;
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	float		fFraction = 100.0f;		//	Fraction
	A3DVECTOR3  vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
	
	if (m_pBrushMan && m_pBrushMan->Trace(pInfo) 
		&&  (pInfo->fFraction < fFraction) )
	{
		fFraction = pInfo->fFraction;
		bAllSolid = pInfo->bAllSolid;
		bStartSolid = pInfo->bStartSolid;
		iClipPlane = pInfo->iClipPlane;
		vNormal = pInfo->ClipPlane.GetNormal();
		fDist = pInfo->ClipPlane.GetDist();
		bCollide=true;
	}	

	HomeOnmtTable::iterator it1 = m_HomeOrnamentTab.begin();
	for (; it1 != m_HomeOrnamentTab.end(); ++it1)
	{
		CECHomeOrnament* pHomeOrnament = *it1.value();
		if (!pHomeOrnament->IsLoaded())
			continue;

		CELBuildingWithBrush* pBuildingWithBrush = pHomeOrnament->GetBuildingWithBrush();
		if(pBuildingWithBrush && pBuildingWithBrush->TraceWithBrush(pInfo)  && (pInfo->fFraction < fFraction )){
				fFraction = pInfo->fFraction;
				bAllSolid = pInfo->bAllSolid;
				bStartSolid = pInfo->bStartSolid;
				iClipPlane = pInfo->iClipPlane;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();
				bCollide=true;
		}
	}	

	// now see if collide with forest
	CELForest * pForest = g_pGame->GetGameRun()->GetWorld()->GetForest();
	if ((fFraction > 0.0f) && pForest 
		&& pForest->TraceWithBrush(pInfo) 
		&& (pInfo->fFraction < fFraction ) ) 
	{
		fFraction = pInfo->fFraction;
		bAllSolid = pInfo->bAllSolid;
		bStartSolid = pInfo->bStartSolid;
		iClipPlane = pInfo->iClipPlane;
		vNormal = pInfo->ClipPlane.GetNormal();
		fDist = pInfo->ClipPlane.GetDist();
		bCollide=true;
	}

	// now see if collide with dynamic scene building
	ECModelTable::iterator it = m_ECModelTab.begin();
	for (; it != m_ECModelTab.end(); ++it)
	{
		ECMODELNODE* pNode = *it.value();
		if ((fFraction > 0.0f) && pNode 
			&& pNode->TraceWithBrush(pInfo) 
			&& (pInfo->fFraction < fFraction ) ) 
		{
			fFraction = pInfo->fFraction;
			bAllSolid = pInfo->bAllSolid;
			bStartSolid = pInfo->bStartSolid;
			iClipPlane = pInfo->iClipPlane;
			vNormal = pInfo->ClipPlane.GetNormal();
			fDist = pInfo->ClipPlane.GetDist();
			bCollide=true;
		}
	}		

	//set back
	pInfo->fFraction = fFraction;
	pInfo->bStartSolid = bStartSolid;
	pInfo->bAllSolid = bAllSolid;
	pInfo->iClipPlane = iClipPlane;
	pInfo->ClipPlane.SetNormal(vNormal);
	pInfo->ClipPlane.SetD(fDist);
	return bCollide;

}

#else
//trace with building
bool CECOrnamentMan::TraceWithBrush(BrushTraceInfo * pInfo)
{
//#define LOCAL_DEBUG

	
	bool bCollide=false;
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	float		fFraction = 100.0f;		//	Fraction
	A3DVECTOR3  vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist
	
#ifdef LOCAL_DEBUG
	char msg[200];
	int  nBuildings;
	nBuildings = 0;
#endif
	
	OnmtTable::iterator it = m_OrnamentTab.begin();
	for (; it != m_OrnamentTab.end(); ++it)
	{
		CECOrnament* pOrnament = *it.value();
		if (!pOrnament->IsLoaded())
			continue;

		CELBrushBuilding* pBrushBuilding = pOrnament->GetBrushBuilding();


		pInfo->BoundAABB.Clear();
		pInfo->BoundAABB.AddVertex(pInfo->vStart);
		pInfo->BoundAABB.AddVertex(pInfo->vStart + pInfo->vExtents);
		pInfo->BoundAABB.AddVertex(pInfo->vStart - pInfo->vExtents);
		pInfo->BoundAABB.AddVertex(pInfo->vStart + pInfo->vDelta);
		pInfo->BoundAABB.AddVertex(pInfo->vStart + pInfo->vDelta + pInfo->vExtents);
		pInfo->BoundAABB.AddVertex(pInfo->vStart + pInfo->vDelta - pInfo->vExtents);
		pInfo->BoundAABB.CompleteCenterExts();

#ifdef LOCAL_DEBUG
		nBuildings++;
#endif
		if(pBrushBuilding && pBrushBuilding->TraceWithBrush(pInfo)  && (pInfo->fFraction < fFraction )){
				fFraction = pInfo->fFraction;
				bAllSolid = pInfo->bAllSolid;
				bStartSolid = pInfo->bStartSolid;
				iClipPlane = pInfo->iClipPlane;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();
				bCollide=true;
		}
	}		

	HomeOnmtTable::iterator it = m_HomeOrnamentTab.begin();
	for (; it != m_HomeOrnamentTab.end(); ++it)
	{
		CECHomeOrnament* pHomeOrnament = *it.value();
		if (!pHomeOrnament->IsLoaded())
			continue;

		CELBuildingWithBrush* pBuildingWithBrush = pHomeOrnament->GetBuildingWithBrush();
		if(pBuildingWithBrush && pBuildingWithBrush->TraceWithBrush(pInfo)  && (pInfo->fFraction < fFraction )){
				fFraction = pInfo->fFraction;
				bAllSolid = pInfo->bAllSolid;
				bStartSolid = pInfo->bStartSolid;
				iClipPlane = pInfo->iClipPlane;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();
				bCollide=true;
		}
	}		

#ifdef  LOCAL_DEBUG
	sprintf(msg, "check %d builidings\n", nBuildings);
	OutputDebugStringA(msg);
#endif
	// now see if collide with forest
	CELForest * pForest = g_pGame->GetGameRun()->GetWorld()->GetForest();
	if (pForest && pForest->TraceWithBrush(pInfo) && (pInfo->fFraction < fFraction ) ) {
				fFraction = pInfo->fFraction;
				bAllSolid = pInfo->bAllSolid;
				bStartSolid = pInfo->bStartSolid;
				iClipPlane = pInfo->iClipPlane;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();
				bCollide=true;
	}

	// now see if collide with dynamic scene building
	ECModelTable::iterator it = m_ECModelTab.begin();
	for (; it != m_ECModelTab.end(); ++it)
	{
		ECMODELNODE* pNode = *it.value();
		if ((fFraction > 0.0f) && pNode 
			&& pNode->TraceWithBrush(pInfo) 
			&& (pInfo->fFraction < fFraction ) ) 
		{
			fFraction = pInfo->fFraction;
			bAllSolid = pInfo->bAllSolid;
			bStartSolid = pInfo->bStartSolid;
			iClipPlane = pInfo->iClipPlane;
			vNormal = pInfo->ClipPlane.GetNormal();
			fDist = pInfo->ClipPlane.GetDist();
			bCollide=true;
		}
	}		

	//set back
	pInfo->fFraction = fFraction;
	pInfo->bStartSolid = bStartSolid;
	pInfo->bAllSolid = bAllSolid;
	pInfo->iClipPlane = iClipPlane;
	pInfo->ClipPlane.SetNormal(vNormal);
	pInfo->ClipPlane.SetD(fDist);
	return bCollide;

}
#endif

//	Load EC model in loading thread
bool CECOrnamentMan::ThreadLoadECModel(DWORD dwModelID, const char* szFile)
{
	LDECMODEL Info;
	Info.dwID = dwModelID;

	CECModel* pModel = LoadECModelFromFile(szFile);
	if (!pModel)
		return false;

	Info.pModel = pModel;

	ACSWrapper csa(&m_csLoad);
	m_aLoadedECMs.Add(Info);

	return true;
}

//	Deliver loaded EC models
void CECOrnamentMan::DeliverLoadedECModels()
{
	ACSWrapper csa(&m_csLoad);

	for (int i=0; i < m_aLoadedECMs.GetSize(); i++)
	{
		const LDECMODEL& m = m_aLoadedECMs[i];

		//	Get record model initial info
		ECModelTable::pair_type Pair = m_ECModelTab.get((int)m.dwID);
		if (!Pair.second)
		{
			//	Counldn't find this model's node, it may has been removed
			//	before resource is loaded !
			DeleteECModel(m.pModel);
		}
		else
		{
			ECMODELNODE* pNode = *Pair.first;
			if (pNode->pModel)
			{
				//	This case is seldom happed, but we have to handle it. See
				//	CECNPC::SetNPCLoadedResult() for detail information.
				DeleteECModel(pNode->pModel);
			}
			
			pNode->pModel = m.pModel;

			m.pModel->SetPos(pNode->vInitPos);
			m.pModel->SetDirAndUp(pNode->vInitDir, pNode->vInitUp);
			if( m.pModel->GetComActByName(pNode->strActName) )
				m.pModel->PlayActionByName(pNode->strActName, 1.0f);
			else
			{
				m.pModel->PlayActionByName("ÐÝÏÐ", 1.0f);
			}
			m.pModel->Tick(rand() % 1000);

			if( pNode->pModel && pNode->pModel->HasCHAABB() )
			{
				ConvexHullDataArray& cdArray = pNode->pModel->GetConvexHullData();

				A3DMATRIX4 matTM = TransformMatrix(pNode->vInitDir, pNode->vInitUp, pNode->vInitPos);
				if( int(cdArray.size()) != pNode->nBrushes )
				{
					pNode->nBrushes = cdArray.size();
					pNode->ppBrushes = new CCDBrush *[cdArray.size()];

					for(int i=0; i<pNode->nBrushes; i++)
					{
						CConvexHullData chData = *cdArray[i];
						chData.Transform(matTM);

						CQBrush qBrush;
						qBrush.AddBrushBevels(&chData);
						CCDBrush * pCDBrush = new CCDBrush();
						qBrush.Export(pCDBrush);

						pNode->ppBrushes[i] = pCDBrush;
					}
				}
			}
		}
	}

	m_aLoadedECMs.RemoveAll(false);
}

bool CECOrnamentMan::UpdateOrnamentsLight(A3DLIGHTPARAM paramDay, A3DLIGHTPARAM paramNight)
{
#ifdef LM_BUMP_ENABLE
	if (m_OrnamentTab.size())
	{
		OnmtTable::iterator it = m_OrnamentTab.begin();
		for (; it != m_OrnamentTab.end(); ++it)
		{
			CECOrnament* pOrnament = *it.value();

			if( pOrnament->GetBrushBuilding() && pOrnament->GetBrushBuilding()->GetA3DLitModel() )
				pOrnament->GetBrushBuilding()->GetA3DLitModel()->SetLightInfo(paramDay, paramNight);
		}
	}
#endif
	return true;
}

bool CECOrnamentMan::ECMODELNODE::TraceWithBrush(BrushTraceInfo * pInfo)
{
	if( !pModel || !pModel->HasCHAABB() )
		return false;

	// Expand the AABB of the model to handle the case that
	// convex hulls of the model protrude the AABB!
	A3DAABB aabb = pModel->GetCHAABB();
	aabb.Extents+=A3DVECTOR3(8.0f, 8.0f, 8.0f);
	aabb.Center += vInitPos;
	aabb.CompleteMinsMaxs();

	A3DVECTOR3 vPoint, vNormal;
	float		fFraction;		//	Fraction
	
	if (pInfo->bIsPoint && !CLS_RayToAABB3(pInfo->vStart, pInfo->vDelta, aabb.Mins, aabb.Maxs, vPoint, &fFraction, vNormal ) ) {
		return false;
	}
	
	if (!pInfo->bIsPoint && !CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents) ) {
		return false;
	}
	
	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	
	fFraction = 100.0f;
	vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist

	// Detect the collision with Convex Hull
	bool bCollide = false;
	for(int i=0;i<nBrushes;i++)
	{
		if (ppBrushes[i]->Trace(pInfo) && (pInfo->fFraction < fFraction)) {
				//update the saving info
				bStartSolid = pInfo->bStartSolid;
				bAllSolid = pInfo->bAllSolid;
				iClipPlane = pInfo->iClipPlane;
				fFraction = pInfo->fFraction;
				vNormal = pInfo->ClipPlane.GetNormal();
				fDist = pInfo->ClipPlane.GetDist();

				bCollide = true;
		}
	}	
	if (bCollide) {
		pInfo->bStartSolid = bStartSolid;
		pInfo->bAllSolid = bAllSolid;
		pInfo->iClipPlane = iClipPlane;
		pInfo->fFraction = fFraction;
		pInfo->ClipPlane.SetNormal(vNormal);
		pInfo->ClipPlane.SetD(fDist);
	}
	return bCollide;
}