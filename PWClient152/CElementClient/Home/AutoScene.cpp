/*
 * FILE: AutoScene.cpp
 *
 * DESCRIPTION: Class for a scene
 *
 * CREATED BY: Jiang Dalong, 2006/06/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoScene.h"
#include "AutoHome.h"
#include "HomeTerrain.h"
#include "EC_World.h"
#include "AutoSceneConfig.h"
#include "AutoHomeBorderData.h"
#include "Render.h"
#include "AutoHomeMan.h"

#include <AAssist.h>
#include <A3DMacros.h>
#include <A3DEngine.h>
#include <A3DCamera.h>
#include <A3DViewport.h>
#include <A3DTypes.h>
#include <A3DLight.h>
#include <A3DDevice.h>
#include <A3DTerrainWater.h>
#include <A3DSkySphere.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoScene::CAutoScene(CECWorld* pECWorld, A3DEngine* pA3DEngine)
{
	m_pA3DEngine		= pA3DEngine;
	m_pECWorld			= pECWorld;
	m_pHomeTerrain		= NULL;
	m_pAutoSceneConfig	= NULL;
	m_pAutoHomeBorderData	= NULL;
	m_pAutoHomeMan		= NULL;
	
	m_iBlkLoadSpeed		= 1;
	m_fActRadius		= 100.0f;
	m_fBlockSize		= 0.0f;
	m_iNumBlock			= 0;
	m_iNumBlockRow		= 0;
	m_iNumBlockCol		= 0;

	m_fDayWeight		= 0.0f;
	m_fDayWeightOld		= 0.0f;
	
	m_dwUniqueID		= 0;

	m_vLoadCenter.Clear();

	m_ActBlocks1.rcArea.Clear();
	m_ActBlocks2.rcArea.Clear();
	m_pCurActBlocks	= &m_ActBlocks1;
	m_pOldActBlocks	= &m_ActBlocks2;

	m_fCameraSpeed	= 100.0f;
}

CAutoScene::~CAutoScene()
{

}

void CAutoScene::Release()
{
	//	Release home terrain
//	A3DRELEASE(m_pHomeTerrain);

	//	Release all loaded blocks
	ReleaseAllBlocks();

	A3DRELEASE(m_pAutoSceneConfig);
	A3DRELEASE(m_pAutoHomeBorderData);
	A3DRELEASE(m_pAutoHomeMan);
}

//	Calculate area represented in blocks
void CAutoScene::CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea)
{
	float fInvBlockSize = 1.0f / m_fBlockSize;
	int iRadiusInBlock = (int)(ceil(fRadius * fInvBlockSize) + 0.5f);
	int l, t, r, b;

	int imaxr = m_iNumBlockCol;
	int imaxb = m_iNumBlockRow;

	int cx = (int)((vCenter.x - m_rcWorld.left) * fInvBlockSize);
	int cz = (int)(-(vCenter.z - m_rcWorld.top) * fInvBlockSize);

	int d  = iRadiusInBlock * 2 + 1;

	l = cx - iRadiusInBlock;
	t = cz - iRadiusInBlock;
	r = l + d;
	b = t + d;

	if (l < 0)
	{
		l = 0;
		r = imaxr > d ? d : imaxr;
	}

	if (t < 0)
	{
		t = 0;
		b = imaxb > d ? d : imaxb;
	}

	if (r > imaxr) 
	{
		r = imaxr;
		l = imaxr > d ? imaxr - d : 0;
	}

	if (b > imaxb)
	{
		b = imaxb;
		t = imaxb > d ? imaxb - d : 0;
	}

	a_ClampFloor(l, 0);
	a_ClampFloor(t, 0);
	a_ClampRoof(r, imaxr);
	a_ClampRoof(b, imaxb);

	if (l > m_iNumBlockCol || t > m_iNumBlockRow ||
		r < 0 || b < 0 || l > r || t > b)
	{
		ASSERT(0);
		return;
	}

	rcArea.SetRect(l, t, r, b);
}

//	Synchronously update active blocks
bool CAutoScene::UpdateActiveBlocks(const A3DVECTOR3& vCenter)
{
	ARectI rcNew;
	CalcAreaInBlocks(vCenter, m_fActRadius, rcNew);
	m_vLoadCenter = vCenter;

	if (!m_pCurActBlocks->aBlocks.GetSize())
	{
		//	Create active block buffer
		m_pCurActBlocks->rcArea = rcNew;
		return LoadActiveBlocks(*m_pCurActBlocks);
	}

	if (rcNew == m_pCurActBlocks->rcArea)
	{
		//	Active block don't change
		return true;
	}

	//	Exchange currently active block buffer
	a_Swap(m_pCurActBlocks, m_pOldActBlocks);

	ASSERT(!m_pCurActBlocks->aBlocks.GetSize());
	m_pCurActBlocks->rcArea = rcNew;
	
	ARectI rcInter = rcNew & m_pOldActBlocks->rcArea;
	if (rcInter.IsEmpty())
	{
		//	The two rectangles havn't intersection
		UnloadActiveBlocks(*m_pOldActBlocks);
		return LoadActiveBlocks(*m_pCurActBlocks);
	}
	else
	{
		int iBaseIdx = rcNew.top * m_iNumBlockCol + rcNew.left;

		for (int r=rcNew.top; r < rcNew.bottom; r++)
		{
			int iIndex = iBaseIdx;

			for (int c=rcNew.left; c < rcNew.right; c++, iIndex++)
			{
				if (m_pOldActBlocks->rcArea.PtInRect(c, r))
					m_pCurActBlocks->aBlocks.Add(m_pOldActBlocks->GetBlock(r, c, true));
				else
				{
					//	Add block to candidate list
					m_CandidateBlkList.AddTail(iIndex);
					m_pCurActBlocks->aBlocks.Add(NULL);
				}
			}

			iBaseIdx += m_iNumBlockCol;
		}

		UnloadActiveBlocks(*m_pOldActBlocks);

		return true;
	}

	return true;
}

//	Load all active blocks immediately
bool CAutoScene::LoadActiveBlocks(ACTBLOCKS& ActBlocks)
{
	//	Scene laoding will occupy about 45% loading progress, calculate
	//	each block's component
	float fBlockComp = 45.0f / (ActBlocks.rcArea.Width() * ActBlocks.rcArea.Height());
	float fProgCnt = 0.0f;

	int r, c, iBaseIdx;
	iBaseIdx = ActBlocks.rcArea.top * m_iNumBlockCol + ActBlocks.rcArea.left;

	for (r=ActBlocks.rcArea.top; r < ActBlocks.rcArea.bottom; r++)
	{
		int iIndex = iBaseIdx;

		for (c=ActBlocks.rcArea.left; c < ActBlocks.rcArea.right; c++, iIndex++)
		{
			CAutoHome* pBlock = LoadBlock(r, c, iIndex);
			ActBlocks.aBlocks.Add(pBlock);
/*
			//	Step loading progress
			fProgCnt += fBlockComp;
			int iStep = (int)fProgCnt;
			if (iStep)
			{
				g_pGame->GetGameRun()->StepLoadProgress(iStep);
				fProgCnt -= (float)iStep;
			}
*/		}

		iBaseIdx += m_iNumBlockCol;
	}

	return true;
}

//	Unload active blocks
void CAutoScene::UnloadActiveBlocks(ACTBLOCKS& ActBlocks)
{
	BlockArray& aBlocks = ActBlocks.aBlocks;

	for (int i=0; i < aBlocks.GetSize(); i++)
	{
		CAutoHome* pBlock = aBlocks[i];
		if (pBlock)
			UnloadBlock(pBlock);
	}
/*
	if (aBlocks.GetSize())
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		A3DVECTOR3 vCenter = g_pGame->GetGameRun()->GetSafeHostPos(g_pGame->GetViewport());

#ifdef USING_BRUSH_MAN
		m_pWorld->GetOrnamentMan()->GetBrushMan()->Build(vCenter);
#endif

#ifdef USING_TRIANGLE_MAN
		m_pWorld->GetOrnamentMan()->GetTriangleMan()->Build(vCenter);
#endif
	}
*/
	ActBlocks.aBlocks.RemoveAll(false);
	ActBlocks.rcArea.Clear();
}

//	Load blocks from candidate list
bool CAutoScene::LoadCandidateBlocks()
{
	//	Number of block which will be loaded
	int iNumBlock = m_CandidateBlkList.GetCount();
	if (iNumBlock > m_iBlkLoadSpeed)
		iNumBlock = m_iBlkLoadSpeed;

	if (!iNumBlock)
		return true;

	for (int i=0; i < iNumBlock; i++)
	{
		int iIndex = m_CandidateBlkList.RemoveHead();

		//	Row and column in whole terrain
		int r = iIndex / m_iNumBlockCol;
		int c = iIndex - r * m_iNumBlockCol;

		//	Check whether candidate block is still in active area. We have
		//	to do this check because the Active area may change every frame.
		if (!m_pCurActBlocks->rcArea.PtInRect(c, r))
			continue;

		//	If camera move very fast and change it's direction frequently, 
		//	there is possible one candidate appears twice. 
		if (m_pCurActBlocks->GetBlock(r, c, false))
			continue;

		m_pCurActBlocks->SetBlock(r, c, LoadBlock(r, c, iIndex));
	}

	return true;
}

//	Load a block
CAutoHome* CAutoScene::LoadBlock(int r, int c, int iBlock)
{
	if (iBlock < 0)
		iBlock = r * m_iNumBlockCol + c;

	ASSERT(iBlock >= 0 && iBlock < m_iNumBlock);

	//	Create a autohome object
	CAutoHome* pBlock = NULL;

	if (!(pBlock = new CAutoHome(this)))
		return NULL;

	// Create a new home
	if (!pBlock->Init(r, c))
		return NULL;

	return pBlock;
}

//	Unload a block
void CAutoScene::UnloadBlock(CAutoHome* pBlock)
{
	ASSERT(pBlock);

	//	Cache has been fill, release this block
	pBlock->Release();
	delete pBlock;
}

//	Release all loaded blocks
void CAutoScene::ReleaseAllBlocks()
{
	//	Release all active blocks
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;

	for (int i=0; i < aBlocks.GetSize(); i++)
	{
		CAutoHome* pBlock = aBlocks[i];
		if (pBlock)
		{
			pBlock->Release();
			delete pBlock;
		}
	}
/*
	A3DVECTOR3 vCenter = g_pGame->GetGameRun()->GetSafeHostPos(g_pGame->GetViewport());

#ifdef USING_BRUSH_MAN
	m_pWorld->GetOrnamentMan()->GetBrushMan()->Build(vCenter);
#endif

#ifdef USING_TRIANGLE_MAN
	m_pWorld->GetOrnamentMan()->GetTriangleMan()->Build(vCenter);
#endif
*/
	m_ActBlocks1.aBlocks.RemoveAll();
	m_ActBlocks2.aBlocks.RemoveAll();
}

bool CAutoScene::Init(const char* szConfigFile)
{
	if (!(m_pAutoSceneConfig = new CAutoSceneConfig()))
		return false;

	if (!m_pAutoSceneConfig->Init(szConfigFile))
		return false;
	
	m_fWorldWid		= m_pAutoSceneConfig->GetWorldWid();
	m_fWorldLen		= m_pAutoSceneConfig->GetWorldLen();
	m_iNumBlockRow	= m_pAutoSceneConfig->GetNumBlockrow();
	m_iNumBlockCol	= m_pAutoSceneConfig->GetNumBlockCol();
	m_iNumBlock		= m_iNumBlockRow * m_iNumBlockCol;
	m_fBlockSize	= m_pAutoSceneConfig->GetBlockSize();

	// Calculate world rect
	m_rcWorld.left		= -m_fWorldWid * 0.5f;
	m_rcWorld.top		= m_fWorldLen * 0.5f;
	m_rcWorld.right		= m_rcWorld.left + m_fWorldWid;
	m_rcWorld.bottom	= m_rcWorld.top - m_fWorldLen;

	//	Create home terrain object
	m_pHomeTerrain = new CHomeTerrain(this);
	if (!m_pHomeTerrain || !m_pHomeTerrain->Init(m_fActRadius))
	{
		a_LogOutput(1, "CAutoScene::Init, Failed to initialize home terrain !");
		return false;
	}

	//	Create home border lines
	m_pAutoHomeBorderData = new CAutoHomeBorderData();
	if (!m_pAutoHomeBorderData || !m_pAutoHomeBorderData->LoadPoints(m_pAutoSceneConfig->GetBorderFile()))
	{
		a_LogOutput(1, "CAutoScene::Init, Failed to initialize home border line !");
		return false;
	}

	//	Create home manager
	m_pAutoHomeMan = new CAutoHomeMan(this);
	if (!m_pAutoHomeMan || !m_pAutoHomeMan->Init())
	{
		a_LogOutput(1, "CAutoScene::Init, Failed to initialize home manager !");
		return false;
	}

	return true;
}

bool CAutoScene::Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter)
{
	//	Update active blocks
	if (!UpdateActiveBlocks(vLoadCenter))
		return false;

	//	Load blocks from candidate list
	if (!LoadCandidateBlocks())
		return false;
	
	AdjustEnvironment(g_Render.GetA3DCamera());

	if (m_pAutoHomeMan)
		m_pAutoHomeMan->Tick(dwDeltaTime);
	
	// Update all visible home
	UpdateVisibleHome(dwDeltaTime);

	return true;
}

bool CAutoScene::Render(A3DViewport* pA3DViewport)
{
	if (m_pAutoHomeMan)
		m_pAutoHomeMan->Render(pA3DViewport);
	
	return true;
}

// Get terrain height at a specified position
float CAutoScene::GetPosHeight(const A3DVECTOR3& vPos)
{
	if (NULL == m_pHomeTerrain)
		return 0.0f;

	return m_pHomeTerrain->GetPosHeight(vPos);
}

// Get a home block
CAutoHome* CAutoScene::GetHome(int r, int c)
{
	if (!m_pCurActBlocks->rcArea.PtInRect(c, r))
		return NULL;

	CAutoHome* pAutoHome;
	pAutoHome = m_pCurActBlocks->GetBlock(r, c, false);

	return pAutoHome;
}

// Ray trace
bool CAutoScene::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& vTracePos)
{
	// Ray trace ground
	if (NULL == m_pHomeTerrain)
		return false;

	RAYTRACERT TraceRt;
	if (!m_pHomeTerrain->RayTrace(vStart, vEnd - vStart, 1.0f, &TraceRt))
		return false;

	vTracePos = TraceRt.vPoint;

	return true;
}

// Adjust environment
bool CAutoScene::AdjustEnvironment(A3DCameraBase* pCamera)
{
	// Set light
	A3DLIGHTPARAM param = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colDay;
	A3DCOLORVALUE colNight;
	colDay = A3DCOLORVALUE(m_pAutoSceneConfig->GetSunColorDay());
	colNight = A3DCOLORVALUE(m_pAutoSceneConfig->GetSunColorNight());
	param.Diffuse = colDay * (1 - m_fDayWeight) + colNight * m_fDayWeight;
	param.Diffuse.a = 1.0f;
	g_Render.GetDirLight()->SetLightParam(param);
	colDay = A3DCOLORVALUE(m_pAutoSceneConfig->GetAmbientDay());
	colNight = A3DCOLORVALUE(m_pAutoSceneConfig->GetAmbientNight());
	DWORD dwAmbient = (colDay * (1 - m_fDayWeight) + colNight * m_fDayWeight).ToRGBAColor();
	g_Render.GetA3DDevice()->SetAmbient(0xff000000 | dwAmbient);

	// Set fog
	colDay = A3DCOLORVALUE(m_pAutoSceneConfig->GetFogColorDay());
	colNight = A3DCOLORVALUE(m_pAutoSceneConfig->GetFogColorNight());
	DWORD dwFogColor, dwUWFogColor;
	float fFogStart, fUWFogStart;
	float fFogEnd, fUWFogEnd;

	dwFogColor = (colDay * (1 - m_fDayWeight) + colNight * m_fDayWeight).ToRGBAColor();
	fFogStart = m_pAutoSceneConfig->GetFogStartDay() * (1 - m_fDayWeight) + m_pAutoSceneConfig->GetFogStartNight() * m_fDayWeight;
	fFogEnd = m_pAutoSceneConfig->GetFogEndDay() * (1 - m_fDayWeight) + m_pAutoSceneConfig->GetFogEndNight() * m_fDayWeight;
	dwUWFogColor = m_pAutoSceneConfig->GetFogColorWater();
	fUWFogStart = 0.0f;
	fUWFogEnd = 100.0f;
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	A3DTerrainWater* pA3DTerrainWater = m_pECWorld->GetTerrainWater();
	if (m_pAutoSceneConfig->IsFogEnable() && (NULL != pA3DTerrainWater))
	{
		bool bUnderWater = pA3DTerrainWater->IsUnderWater(pCamera->GetPos());
		if (bUnderWater)
		{
			pA3DDevice->SetFogColor(dwUWFogColor);
			pA3DDevice->SetFogStart(fUWFogStart);
			pA3DDevice->SetFogEnd(fUWFogEnd);
		}
		else
		{
			pA3DDevice->SetFogColor(dwFogColor);
			pA3DDevice->SetFogStart(fFogStart);
			pA3DDevice->SetFogEnd(fFogEnd);
		}
		pA3DDevice->SetFogEnable(true);
	}
	else
		pA3DDevice->SetFogEnable(false);
	
	// Set water fog
	if (NULL != pA3DTerrainWater)
	{
		pA3DTerrainWater->SetFogAir(dwFogColor, fFogStart, fFogEnd);
		pA3DTerrainWater->SetFogWater(dwUWFogColor, fUWFogStart, fUWFogEnd);
	}

	// Set sky
/*	if (m_fDayWeight < 0.5f && m_fDayWeightOld < 0.5f)
		return true;
	if (m_fDayWeight >= 0.5f && m_fDayWeightOld >= 0.5f)
		return true;
*/
	UpdateSky();

	return true;
}

void CAutoScene::SetDayWeight(float fWeight)
{ 
	m_fDayWeightOld = m_fDayWeight;
	m_fDayWeight = fWeight; 
}

// Update sky
bool CAutoScene::UpdateSky()
{
	DWORD dwSkyID;
	if (m_fDayWeight < 0.5f)// Day
		dwSkyID = m_pAutoSceneConfig->GetSkyIDDay();
	else
		dwSkyID = m_pAutoSceneConfig->GetSkyIDNight();

	AUTOSKYTYPELIST type;
	int nIndex = m_pAutoSceneConfig->GetAutoSkyTypeListByID(dwSkyID, &type);
	if (nIndex >= 0)
	{
		AString strCap, strSurroundF, strSurroundB;
		strCap = type.strCap;
		strSurroundF = type.strSurroundF;
		strSurroundB = type.strSurroundB;

		A3DSkySphere* pA3DSky = m_pECWorld->GetSkySphere();
		if (NULL != pA3DSky)
		{
			if (pA3DSky->IsTransing())
				return true;
			if (pA3DSky->NeedTrans(strCap, strSurroundF, strSurroundB))
				pA3DSky->TransSky(strCap, strSurroundF, strSurroundB, 1000);
			while(pA3DSky->IsTextureLoading())
				Sleep(10);
		}
	}

	return true;
}

// Under water?
bool CAutoScene::IsUnderWater(const A3DVECTOR3& vPos)
{
	A3DTerrainWater* pTerrainWater = m_pECWorld->GetTerrainWater();
	if (NULL == pTerrainWater)
		return false;
	return pTerrainWater->IsUnderWater(vPos);
}

// Update all visible home
bool CAutoScene::UpdateVisibleHome(DWORD dwDeltaTime)
{
	int i, j;
	CAutoHome* pAutoHome;
	for (i=0; i<m_iNumBlockCol; i++)
	{
		for (j=0; j<m_iNumBlockRow; j++)
		{
			pAutoHome = GetHome(j, i);
			if (NULL != pAutoHome)
				pAutoHome->Tick(dwDeltaTime);
		}
	}

	return true;
}
