/*
 * FILE: ElementMap.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Terrain.h"

#include "ElementMap.h"
#include "vector.h"
#include "EL_Tree.h"
#include "EL_GrassType.h"
#include "EL_Forest.h"
#include "EL_GrassLand.h"
#include "EL_Archive.h"
#include "EL_Building.h"
#include "TerrainExportor.h"
#include "MaskModifier.h"
#include "TerrainRender.h"
#include "LightMapGen.h"
#include "SceneObjectManager.h"
#include "SceneAIGenerator.h"
#include "SceneAudioObject.h"
#include "SceneBoxArea.h"
#include "SceneDummyObject.h"
#include "SceneLightObject.h"
#include "SceneObject.h"
#include "SceneSpeciallyObject.h"
#include "SceneWaterObject.h"
#include "SceneVernier.h"
#include "Render.h"
#include "StaticModelObject.h"
#include "TerrainWater.h"
#include "AF.h"
#include "A3D.h"
#include "MainFrm.h"
#include "LogDlg.h"
#include "A3DGFXExMan.h"

#include "SceneBoxArea.h"
#include "TerrainPlants.h"
#include "pathmap\WaterAreaMap.h"
#include "PolygonFunc.h"
#include "ScenePrecinctObject.h"
#include "EditerBezier.h"
#include "bitmap.h"
#include "CloudBox.h"
#include "LightMapTransition.h"
#include "spoctree\CompactSpacePassableOctree.h"
#include <A3DWaterArea.h>
#include "pathmap\PlayerPassMapGenerator.h"
//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Elemment project file identity and version
#define	EPRJFILE_IDENTIFY		(('E'<<24) | ('P'<<16) | ('R'<<8) | 'J')
#define EPRJFILE_VERSION		22    //=22,地图下一些新文件放在了VSS上,静态模型存放进行了修改 

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////
int            g_nStaticNum;

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
//	Implement CElementMap
//	
///////////////////////////////////////////////////////////////////////////

CElementMap::CElementMap() : m_strResDir(""), m_strMapFile("")
{
	m_pTerrain			= NULL;
	m_pSceneObjectMan	= NULL;
	m_pCloudRender      = NULL;
	m_pNeighborTerrain  = NULL;
	m_pCloudCreator     = NULL;
	m_pTerrainWater		= NULL;
	m_pPassMapGenerator = NULL;
	m_pSpacePassGenerator = NULL;
	m_pPlayerPassMapGenerator = NULL;
	m_nPlayerPassMapCurLayer = 0;
	m_bModified			= false;
	m_fOffsetX			= 0.0f;
	m_fOffsetZ			= 0.0f;

	//	Default lighting parameters
	m_Lighting.dwAmbient	= A3DCOLORRGB(128,128,128);
	m_Lighting.dwSunCol		= 0xffffffff;
	m_Lighting.fSunPower    = 1.0f;
	m_Lighting.vSunDir		= A3DVECTOR3(0.1f,-0.6,0.7f);
	m_Lighting.iMapSize		= 512;
	
	m_NightLighting.dwAmbient   = A3DCOLORRGB(64,64,64);
	m_NightLighting.dwSunCol	= A3DCOLORRGB(128,128,128);
	m_NightLighting.fSunPower   = 1.0f;
	m_NightLighting.vSunDir     = A3DVECTOR3(0.1f,-0.6,0.7f);
	m_NightLighting.iMapSize	= 512;

	m_iLoadFlag					= LOAD_NORMAL;

	m_FogColor.Reset(ColorRGBAToColorValue(0xffffffff));
	m_FogStart.Reset(300.0f);
	m_FogEnd.Reset(500.0f);
	m_FogDensity.Reset(1.0f);

	m_FogColorNight.Reset(ColorRGBAToColorValue(0xff000000));
	m_FogStartNight.Reset(300.0f);
	m_FogEndNight.Reset(500.0f);
	m_FogDensityNight.Reset(1.0f);

	m_FogColorUW.Reset(ColorRGBAToColorValue(0xff0000ff));
	m_FogStartUW.Reset(100.0f);
	m_FogEndUW.Reset(300.0f);
	m_FogDensityUW.Reset(1.0f);

	m_LitDir.Reset(ColorRGBAToColorValue(0xffffffff));
	m_LitAmb.Reset(ColorRGBAToColorValue(0xff808080));

	m_LitDirNight.Reset(ColorRGBAToColorValue(0xff808080));
	m_LitAmbNight.Reset(ColorRGBAToColorValue(0xff303030));

	m_CloudDir.Reset(ColorRGBAToColorValue(0xffffffff));
	m_CloudAmb.Reset(ColorRGBAToColorValue(0xff808080));

	m_CloudDirNight.Reset(ColorRGBAToColorValue(0xff808080));
	m_CloudAmbNight.Reset(ColorRGBAToColorValue(0xff303030));

	m_fUoff= 0.0f; m_fVoff = 0.0f;
}

CElementMap::~CElementMap()
{
}

//	Create an empty map
bool CElementMap::CreateEmpty(const char* szMapName, const CTerrain::INITDATA& TrnInitData)
{
	m_bModified	= false;

	//	Set map name
	ChangeMapName(szMapName);

	//	Create map objects
	if (!CreateMapObjects())
	{
		g_Log.Log("CElementMap::CreateEmpty, Failed to create terrain object.");
		return false;
	}

	if (!m_pTerrain->Init(TrnInitData))
	{
		g_Log.Log("CElementMap::CreateEmpty, Failed to initalize terrain.");
		return false;
	}

	if (!m_pTerrainWater->Init())
	{
		g_Log.Log("CElementMap::CreateEmpty, Failed to initalize terrain water.");
		return false;
	}
	
	m_pSceneObjectMan->GetElGrassLand()->SetA3DTerrain(m_pTerrain->GetRender());
	
	return true;
}

//	Release map object
void CElementMap::Release()
{
	ReleaseMapObjects();
}

//	Make map file name
void CElementMap::MakeMapFileName()
{
	m_strMapFile = m_strResDir + "\\";

	const char* szTemp = strrchr(m_strResDir, '\\');
	if (!szTemp)
		m_strMapFile += m_strResDir + ".elproj";
	else
	{
		m_strMapFile += szTemp + 1;
		m_strMapFile += ".elproj";
	}
}

//	Create map objects
bool CElementMap::CreateMapObjects()
{
	//AfxGetGFXExMan()->Init(g_Render.GetA3DDevice());
	if (!(m_pTerrain = new CTerrain(this)))
		return false;

	if (!(m_pTerrainWater = new CTerrainWater(this)))
		return false;

	if (!(m_pSceneObjectMan = new CSceneObjectManager(this)))
		return false;

	if(!(m_pCloudRender = new CELCloudSceneRender()))
		return false;

	if(!(m_pCloudCreator = new CELCloudCreator()))
		return false;

	if(!(m_pPassMapGenerator = new CPassMapGenerator()))
		return false;

	if (!(m_pPlayerPassMapGenerator = new CPlayerPassMapGenerator()))
		return false;

	if(!(m_pSpacePassGenerator = new SPOctree::CSpacePassableOctree()))
		return false;
	
	if(!m_pCloudRender->Init(g_Render.GetA3DDevice(),"Textures\\sky\\cloud.tga"))
		return false;
	m_pNeighborTerrain = NULL;
	return true;
}

//	Release terrain object
void CElementMap::ReleaseMapObjects()
{
  	if (m_pTerrain)
	{
		m_pTerrain->Release();
		delete m_pTerrain;
		m_pTerrain = NULL;
	}

	if(m_pNeighborTerrain)
	{
		m_pNeighborTerrain->Release();
		delete m_pNeighborTerrain;
		m_pNeighborTerrain = NULL;
	}

	if (m_pTerrainWater)
	{
		m_pTerrainWater->Release();
		delete m_pTerrainWater;
		m_pTerrainWater = NULL;
	}

	if (m_pSceneObjectMan)
	{
		m_pSceneObjectMan->Release();
		delete m_pSceneObjectMan;
		m_pSceneObjectMan = NULL;
	}

	
	if (m_pCloudRender)
	{
		m_pCloudRender->Release();
		delete m_pCloudRender;
		m_pCloudRender = NULL;
	}

	if (m_pCloudCreator)
	{
		m_pCloudCreator->Release();
		delete m_pCloudCreator;
		m_pCloudCreator = NULL;
	}

	if (m_pPassMapGenerator)
	{
		m_pPassMapGenerator->Release();
		delete m_pPassMapGenerator;
		m_pPassMapGenerator = NULL;
	}

	if(m_pSpacePassGenerator)
	{
		m_pSpacePassGenerator->Release();
		delete m_pSpacePassGenerator;
		m_pSpacePassGenerator = NULL;
	}

	if (m_pPlayerPassMapGenerator){
		m_pPlayerPassMapGenerator->Release();
		delete m_pPlayerPassMapGenerator;
		m_pPlayerPassMapGenerator = NULL;
	}
	m_nPlayerPassMapCurLayer = 0;

	ReleaseNeighborModels();
	//AfxGetGFXExMan()->Release();
}

//	Change map name. Note: this funciton also change map resources directory
//	and map file name !!
void CElementMap::ChangeMapName(const char* szMapName)
{
	m_strMapName = szMapName;

	//	Make resource directory name
	m_strResDir = g_szEditMapDir;
	m_strResDir += "\\";
	m_strResDir += szMapName;

	//	Make map file name
	MakeMapFileName();
}

//	Tick routine
bool CElementMap::FrameMove(DWORD dwDeltaTime)
{
	// first adjust fog and light color
	float fTime = dwDeltaTime * 0.001f;
	m_FogColor.Shade(fTime);
	m_FogStart.Shade(fTime);
	m_FogEnd.Shade(fTime);
	m_FogDensity.Shade(fTime);

	m_FogColorNight.Shade(fTime);
	m_FogStartNight.Shade(fTime);
	m_FogEndNight.Shade(fTime);
	m_FogDensityNight.Shade(fTime);

	m_FogColorUW.Shade(fTime);
	m_FogStartUW.Shade(fTime);
	m_FogEndUW.Shade(fTime);
	m_FogDensityUW.Shade(fTime);
	
	m_FogColorUWNight.Shade(fTime);
	m_FogStartUWNight.Shade(fTime);
	m_FogEndUWNight.Shade(fTime);
	m_FogDensityUWNight.Shade(fTime);

	//Direction light
	m_LitDir.Shade(fTime);
	m_LitDirNight.Shade(fTime);
	
	//Ambient light
	m_LitAmb.Shade(fTime);
	m_LitAmbNight.Shade(fTime);

	//Cloud color
	m_CloudDir.Shade(fTime);
	m_CloudDirNight.Shade(fTime);

	m_CloudAmb.Shade(fTime);
	m_CloudAmbNight.Shade(fTime);

	float d = 1.0f - g_fNightFactor;
	float n = g_fNightFactor;

	if( m_pSceneObjectMan->IsCameraUnderWater() )
	{
		g_Render.GetA3DDevice()->SetFogColor((m_FogColorUW.cur * d + m_FogColorUWNight.cur * n).ToRGBAColor());
		g_Render.GetA3DDevice()->SetFogStart(m_FogStartUW.cur * d + m_FogStartUWNight.cur * n);
		g_Render.GetA3DDevice()->SetFogEnd(m_FogEndUW.cur * d + m_FogEndUWNight.cur * n);
		g_Render.GetA3DDevice()->SetFogDensity(m_FogDensityUW.cur * d + m_FogDensityUWNight.cur*n);
	}
	else
	{
		g_Render.GetA3DDevice()->SetFogColor((m_FogColor.cur * d + m_FogColorNight.cur * n).ToRGBAColor());
		g_Render.GetA3DDevice()->SetFogStart(m_FogStart.cur * d + m_FogStartNight.cur * n);
		g_Render.GetA3DDevice()->SetFogEnd(m_FogEnd.cur * d + m_FogEndNight.cur * n);
		g_Render.GetA3DDevice()->SetFogDensity(m_FogDensity.cur * d + m_FogDensityNight.cur * n);
	}
	
	A3DLIGHTPARAM param = g_Render.GetDirLight()->GetLightparam();
	param.Diffuse = m_LitDir.cur * d + m_LitDirNight.cur * n;
	param.Diffuse.a = 1.0f;
	g_Render.GetDirLight()->SetLightParam(param);
	g_Render.GetA3DDevice()->SetAmbient(0xff000000 | (m_LitAmb.cur * d + m_LitAmbNight.cur * n).ToRGBAColor());

	if (m_pTerrain)
		m_pTerrain->FrameMove(dwDeltaTime);

	if (m_pNeighborTerrain)
		m_pNeighborTerrain->FrameMove(dwDeltaTime);

	if (m_pTerrainWater)
	{
		m_pTerrainWater->FrameMove(dwDeltaTime);
		if( m_pTerrainWater->GetA3DTerrainWater() )
		{
			m_pTerrainWater->GetA3DTerrainWater()->SetFogAir((m_FogColor.cur * d + m_FogColorNight.cur * n).ToRGBAColor(), m_FogStart.cur * d + m_FogStartNight.cur * n, m_FogEnd.cur * d + m_FogEndNight.cur * n);
			m_pTerrainWater->GetA3DTerrainWater()->SetFogWater((m_FogColorUW.cur * d + m_FogColorUWNight.cur * n).ToRGBAColor(), m_FogStartUW.cur * d + m_FogStartUWNight.cur * n, m_FogEndUW.cur * d + m_FogEndUWNight.cur * n);
		}
	}

	if (m_pSceneObjectMan)
	{
		m_pSceneObjectMan->Tick(dwDeltaTime);
		if(g_Configs.bEnableSpeak)
		{
			g_BackMusic.Update(dwDeltaTime);
			g_BackMusic.UpdateSFX(dwDeltaTime);
			CScenePrecinctObject *pObj = m_pSceneObjectMan->GetCurrentPrecinct();
			APtrList<CSceneObject *>* tempList = m_pSceneObjectMan->GetSortObjectList(CSceneObject::SO_TYPE_PRECINCT);
			ALISTPOSITION pos = tempList->GetTailPosition();
			while( pos )
			{
				CSceneObject* ptemp = tempList->GetPrev(pos);
				if(ptemp!=pObj) ((CScenePrecinctObject *)ptemp)->StopBackMusic();
			}
			if(pObj) pObj->PlayBackMusic();
		}
	}
	LIGHTINGPARAMS params = GetLightingParams();
	A3DVECTOR3 vSunPos = -params.vSunDir * 2000.0f;
	if (m_pCloudRender)
	{
		m_pCloudRender->SetSpriteColor(m_CloudAmb.cur * d + m_CloudAmbNight.cur * n, m_CloudDir.cur * d + m_CloudDirNight.cur * n);
		m_pCloudRender->Tick(dwDeltaTime,vSunPos);
	}
	return true;
}

//	render routine
bool CElementMap::Render(A3DViewport* pA3DViewport)
{
	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	pA3DDevice->Clear(D3DCLEAR_TARGET,pA3DDevice->GetFogColor(),0.0f,0);
	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderSky(pA3DViewport);

	if(g_Configs.bShowTerrain)
	{
		if (m_pTerrain)
		{
			CTerrainRender *pRender = m_pTerrain->GetRender();
			if(pRender) pRender->SetDNFactor(g_fNightFactor);
			m_pTerrain->Render(pA3DViewport);
		}
		if (m_pNeighborTerrain)
		{
			CTerrainRender *pRender = m_pNeighborTerrain->GetRender();
			if(pRender) pRender->SetDNFactor(g_fNightFactor);
			m_pNeighborTerrain->Render(pA3DViewport);
		}
	}

	if(g_Configs.bShowStaticModel && m_pNeighborTerrain)
		RenderNeighborModels(pA3DViewport);

	g_Render.GetA3DEngine()->BeginCacheAlphaMesh(pA3DViewport);
	A3DCOLOR clAmbient = g_Render.GetA3DDevice()->GetAmbientColor();
	A3DCOLORVALUE clValueAmbient = a3d_ColorRGBAToColorValue(clAmbient) * 0.5f;
	clValueAmbient.a = 1.0f;
	A3DLIGHTPARAM param = g_Render.GetDirLight()->GetLightparam();
	A3DLIGHTPARAM param2 = param;
	param2.Diffuse = param.Diffuse * 0.5f;
	param2.Diffuse.a = 1.0f;
	//param2.Ambient = param.Ambient * 0.5f;
	//g_Render.GetDirLight()->SetLightParam(param2);
	//pA3DDevice->SetAmbient(a3d_ColorValueToColorRGBA(clValueAmbient));
	pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
	pA3DDevice->SetAlphaTestEnable(true);
	pA3DDevice->SetAlphaBlendEnable(false);
	pA3DDevice->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	pA3DDevice->SetAlphaRef(84);

	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderObjects(pA3DViewport);

	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	
	pA3DEngine->GetA3DLitModelRender()->SetDNFactor(g_fNightFactor);
	pA3DEngine->GetA3DLitModelRender()->Render(pA3DViewport, false);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(false);
	
	//pA3DDevice->SetAmbient(clAmbient);
	//g_Render.GetDirLight()->SetLightParam(param);
	AfxGetGFXExMan()->RenderAllSkinModels(pA3DViewport);
	A3DSkinRender* pSkinRender = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	pSkinRender->Render(pA3DViewport, false);
	pSkinRender->ResetRenderInfo(false);
	
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);

	if(g_Configs.bShowGrass)
	{
		CELGrassLand* pGrassLand = m_pSceneObjectMan->GetElGrassLand();
		pGrassLand->SetA3DTerrain(m_pTerrain->GetRender());
		pGrassLand->Render(pA3DViewport, false);
	}
	pA3DEngine->FlushCachedAlphaMesh(pA3DViewport);
	pA3DDevice->SetAlphaTestEnable(false);
	pA3DDevice->SetAlphaBlendEnable(true);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	
	pA3DEngine->GetA3DLitModelRender()->RenderAlpha(pA3DViewport);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(true);

	if (m_pTerrainWater && g_Configs.bShowWater)
	{
		m_pTerrainWater->GetA3DTerrainWater()->SetDNFactor(g_fNightFactor);
		m_pTerrainWater->Render(m_Lighting.vSunDir, pA3DViewport, m_pTerrain->GetRender(), m_pSceneObjectMan->GetSky(), MapRenderForReflected, MapRenderForRefracted, this);
	}

	pSkinRender->RenderAlpha(pA3DViewport);
	pSkinRender->ResetRenderInfo(true);

	if(g_Configs.bShowGrass)
	{
		CELGrassLand* pGrassLand = m_pSceneObjectMan->GetElGrassLand();
		pGrassLand->SetA3DTerrain(m_pTerrain->GetRender());
		pGrassLand->SetDNFactor(g_fNightFactor);
		pGrassLand->Render(pA3DViewport, true);
	}
	// Render for cloud
	if(m_pCloudRender && g_Configs.bShowCloudBox)
	{
		m_pCloudRender->SetRenderLine(g_Configs.bShowCloudLine);
		m_pCloudRender->Render(pA3DViewport);
	}
	pA3DDevice->SetAlphaBlendEnable(true);
	
	//渲染效果
	AfxGetGFXExMan()->RenderAllGfx(pA3DViewport);

	// Render readonly flags
	if(g_bReadOnly) 
	{
		A3DVIEWPORTPARAM *param = g_Render.GetA3DEngine()->GetActiveViewport()->GetParam();
		g_Render.TextOut(param->Width/2,param->Height/2,"**Read Only**",A3DCOLORRGB(255,0,0));
	}

	if(m_pSpacePassGenerator) m_pSpacePassGenerator->Render(g_Render.GetA3DEngine()->GetA3DWireCollector());
	
	return true;
}

bool  CElementMap::RenderForLight(A3DViewport* pA3DViewport)
{
	//Del state block by xqf on 2005.10
	//DWORD hStateBlock;
	//if( FAILED(g_Render.GetA3DDevice()->GetD3DDevice()->CreateStateBlock(D3DSBT_ALL, &hStateBlock)) )
	//	return false;

	//g_Render.GetA3DDevice()->GetD3DDevice()->CaptureStateBlock(hStateBlock);

	if (m_pTerrain)
	{
		m_pTerrain->FrameMove(10, pA3DViewport->GetCamera());
		A3DCULLTYPE oldType = g_Render.GetA3DDevice()->GetFaceCull();
		g_Render.GetA3DDevice()->SetFaceCull(A3DCULL_NONE);
		m_pTerrain->Render(pA3DViewport);
		g_Render.GetA3DDevice()->SetFaceCull(oldType);
	}

	//g_Render.GetA3DDevice()->GetD3DDevice()->ApplyStateBlock(hStateBlock);
	//g_Render.GetA3DDevice()->GetD3DDevice()->DeleteStateBlock(hStateBlock);
	g_Render.GetA3DEngine()->BeginCacheAlphaMesh(pA3DViewport);
	A3DCOLOR clAmbient = g_Render.GetA3DDevice()->GetAmbientColor();
	A3DCOLORVALUE clValueAmbient = a3d_ColorRGBAToColorValue(clAmbient) * 0.5f;
	A3DLIGHTPARAM param = g_Render.GetDirLight()->GetLightparam();
	A3DLIGHTPARAM param2 = param;
	param2.Diffuse = param.Diffuse * 0.5f;
	g_Render.GetDirLight()->SetLightParam(param2);
	g_Render.GetA3DDevice()->SetAmbient(a3d_ColorValueToColorRGBA(clValueAmbient));
	g_Render.GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE2X);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_Render.GetA3DDevice()->SetAlphaRef(84);

	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderForLight(pA3DViewport,m_fOffsetX,m_fOffsetZ);

	//g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	//g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);

	A3DSkinRender* pSkinRender = g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	pSkinRender->Render(pA3DViewport);
	pSkinRender->ResetRenderInfo(true);
	pSkinRender->ResetRenderInfo(false);
	
	g_Render.GetA3DDevice()->SetAmbient(clAmbient);
	g_Render.GetDirLight()->SetLightParam(param);
	g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
	g_Render.GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);

	return true;
}

//	render for reflect pass
bool CElementMap::RenderObjectsForReflect(A3DViewport * pA3DViewport)
{
	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderReflectObjects(pA3DViewport);

	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	
	A3DSkinRender* pSkinRender = g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	pSkinRender->Render(pA3DViewport);
	pSkinRender->ResetRenderInfo(true);
	pSkinRender->ResetRenderInfo(false);

	return true;
}

//	render for refract pass
bool CElementMap::RenderObjectsForRefract(A3DViewport * pA3DViewport)
{
	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderRefractObjects(pA3DViewport);

	A3DTerrainWater *pWater = GetTerrainWater()->GetA3DTerrainWater();
	bool bUnderWater = pWater->IsUnderWater(pA3DViewport->GetCamera()->GetPos());
	if(bUnderWater)
	{
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, true);
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	}else
	{
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->RenderForRefract(pA3DViewport, pWater->GetCurWaterHeight());
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
		g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	}

	A3DSkinRender* pSkinRender = g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	pSkinRender->Render(pA3DViewport);
	pSkinRender->ResetRenderInfo(true);
	pSkinRender->ResetRenderInfo(false);

	return true;
}

//	render for light map shadow routine
bool CElementMap::RenderForLightMapShadow(A3DViewport* pA3DViewport)
{
	g_Render.GetA3DEngine()->BeginCacheAlphaMesh(pA3DViewport);

	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_Render.GetA3DDevice()->SetAlphaRef(84);

	if (m_pSceneObjectMan)
		m_pSceneObjectMan->RenderForLightMapShadow(pA3DViewport);

	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_Render.GetA3DDevice()->SetAlphaRef(84);
	//g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	//g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);

	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
	g_Render.GetA3DDevice()->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);

	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_Render.GetA3DDevice()->SetAlphaRef(84);
	A3DSkinRender* pSkinRender = g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	pSkinRender->Render(pA3DViewport);
	pSkinRender->ResetRenderInfo(true);
	pSkinRender->ResetRenderInfo(false);

	g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
	return true;
}

//	Load data from file
bool CElementMap::Load(const char* szFile)
{
	AFile File;
	if (!File.Open(szFile, AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::Load, Failed to open file %s.", szFile);
		return false;
	}

	//	Create file archive
	CELFileArchive ar(&File, false);

	//	Load empty map
	if (!Load(ar))
	{
		File.Close();
		g_Log.Log("CElementMap::Load, Failed load data.");
		return false;
	}

	File.Close();
	if(m_pTerrain)
	{
		CTerrainRender *pRender = m_pTerrain->GetRender();
		if(pRender) pRender->EnableLight(g_bShowTerrainLight);
	}
	return true;
}


bool CElementMap::LoadPassMap()
{
	// Test if we have generated the pass map in the directory!
	CString strRMapPath, strDHMapPath;
	strRMapPath.Format("%s%s\\%s\\%s.rmap",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	strDHMapPath.Format("%s%s\\%s\\%s.dhmap",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	FILE *fRMap=fopen(strRMapPath, "rb");
	FILE *fDHMap=fopen(strDHMapPath, "rb");
	if(!(fRMap && fDHMap)) return true;

	// 装载可达图，高度图
	CBitImage ReachMap;
	CBlockImage<FIX16> DHMap;
	if(!(ReachMap.Load(fRMap) && DHMap.Load(fDHMap))){
		fclose(fRMap);
		fclose(fDHMap);
		return false;
	}

	fclose(fRMap);
	fclose(fDHMap);
	
	if(!m_pPassMapGenerator->ImportMaps(&ReachMap, &DHMap)) return false;

	if(!m_pPassMapGenerator->GetPassMap())
		g_Log.Log("CElementMap::LoadPassMap(), Failed to import maps");
	return true;
}

bool CElementMap::SavePassMap()
{
	if(m_pPassMapGenerator==0) return true;
	CString strProjPath;
	if(!m_pPassMapGenerator->GetPassMap())
		return true;
	
	// Test if we have generated the pass map in the directory!
	CBitImage ReachableImage;
	CBlockImage<FIX16> DeltaHImage;
	
	m_pPassMapGenerator->ExportMaps(&ReachableImage,&DeltaHImage);
	
	strProjPath.Format("%s%s\\%s\\%s.rmap",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	FILE* sFile = fopen(strProjPath,"wb");
	if(sFile==NULL)
	{
		g_Log.Log("CElementMap::SavePassMap(), Open file %s failed!",strProjPath);
		return false;
	}
	
	if(!ReachableImage.Save(sFile))
	{
		g_Log.Log("CElementMap::SavePassMap(), Save file %s failed!",strProjPath);
		return false;
	}
	fclose(sFile);
	
	strProjPath.Format("%s%s\\%s\\%s.dhmap",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	sFile = fopen(strProjPath,"wb");
	if(sFile==NULL)
	{
		g_Log.Log("CElementMap::SavePassMap(), Open file %s failed!",strProjPath);
		return false;
	}
	
	if(!DeltaHImage.Save(sFile))
	{
		g_Log.Log("CElementMap::SavePassMap(), Save file %s failed!",strProjPath);
		return false;
	}
	fclose(sFile);
	return true;
}

bool CElementMap::SaveSpacePassMap()
{
	if(m_pSpacePassGenerator==0) return true; 
	CString spm;
	SPOctree::CCompactSpacePassableOctree comp;
	m_pSpacePassGenerator->Export(comp,0,0,0);
	spm.Format("%s%s\\%s\\%s.octr",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	if(!comp.Save(spm))
	{
		g_Log.Log("CElementMap::SaveSpacePassMap(), failed to save %s!",spm);
		return false;
	}
	return true;
}

bool CElementMap::SaveAllPassMap()
{
	bool bResult;
	bResult = SaveSpacePassMap();
	bResult = bResult && SavePassMap();
	return bResult;
}

//	Load data
bool CElementMap::Load(CELArchive& ar)
{
	if(m_iLoadFlag == LOAD_EXPLIGHTMAP)
		g_pA3DConfig->SetFlagNoTextures(true);
	
	//	Load general map data
	if (!LoadGeneralMapData(ar))
	{
		g_Log.Log("CElementMap::Load, Failed to call LoadGeneralMapData()");
		return false;
	}

	//	Create map object
	if (!CreateMapObjects())
	{
		g_Log.Log("CElementMap::LoadTerrain, Failed to create terrain object");
		return false;
	}

	//	Load terrain data
	if (!LoadTerrain(ar))
	{
		g_Log.Log("CElementMap::Load, Failed to load terrain data");
		return false;
	}

	//	Load terrain water data
	if(m_iLoadFlag != LOAD_EXPLIGHTMAP)
	{
		if (!LoadTerrainWater(ar))
		{
			g_Log.Log("CElementMap::Load, Failed to load terrain water data");
			return false;
		}
	}

	//	Load edit record data
	LoadEditRecord();

	g_pA3DConfig->SetFlagNoTextures(false);
	
	//	Load scene object data
	if (!LoadSceneObjects(ar))
	{
		g_Log.Log("CElementMap::Load, Failed to load scene object data");
		return false;
	}	

	//  Load cloud data
	CString cloudPath;
	cloudPath.Format("%s%s\\%s\\%s.scld",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	if (m_pCloudCreator && m_Header.dwVersion > 16) 
	{
		if(!m_pCloudCreator->LoadCloud(cloudPath))
		{
			g_Log.Log("CElementMap::Load, Failed to load cloud data");
			return false;
		}
		cloudPath.Format("%s%s\\%s\\%s.cldo",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
		if(!LoadCloudOption(cloudPath))
		{
			g_Log.Log("CElementMap::Load, Failed to load cloud option data");
			return false;
		}
		abase::vector<CloudSprite>* pSprite = NULL;
		abase::vector<CloudGroup>* pGroup   = NULL;
		m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
		m_pCloudRender->SetCloudInfo(m_pCloudCreator->GetCloudCreatorParam(),*pSprite,*pGroup);
	}
	if(m_Header.dwVersion < 21)
		RecreateAllCloudGroup();

	//  Load terrain pass map
	if(!LoadPassMap())
	{
		g_Log.Log("CElementMap::Load, Failed to load terrain pass map");
		return false;
	}
	if (!LoadCurPlayerPassMap()){
		g_Log.Log("CElementMap::Load, Failed to load player pass map");
		return false;
	}
	return true;
}

//	Save data to file
bool CElementMap::Save(const char* szFile)
{
	AFile File;
	if (!File.Open(szFile, AFILE_CREATENEW))
	{
		g_Log.Log("CElementMap::Save, Failed to create new file %s", szFile);
		return false;
	}

	//	Create file archive
	CELFileArchive ar(&File, true);

	if (!Save(ar))
	{
		File.Close();
		g_Log.Log("CElementMap::Save, Failed to save map data.");
		return false;
	}
	File.Close();
	return true;
}

//	Save data
bool CElementMap::Save(CELArchive& ar)
{
	//	File header
	MAPFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwIdentity	= EPRJFILE_IDENTIFY;
	Header.dwVersion	= EPRJFILE_VERSION;

	ar.Write(&Header, sizeof (Header));

	//	Save project name
	ar.WriteString(m_strMapName);

	//	Save resource directory
	ar.WriteString(m_strResDir);

	//	Save lighting parameters
	ar.Write(&m_Lighting, sizeof (m_Lighting));

	//  Save night light Parameters
	ar.Write(&m_NightLighting, sizeof (m_NightLighting));
	
	ar.Write(&g_Configs.fSunFaction,sizeof(float));
	
	ar.Write(&g_Configs.fNightSunFaction,sizeof(float));
	
	//	Save terrain data
	if (!SaveTerrain(ar))
	{
		g_Log.Log("CElementMap::Save, Failed to save terrain data.");
		return false;
	}

	//	Save terrain water data
	if (!SaveTerrainWater(ar))
	{
		g_Log.Log("CElementMap::Save, Failed to save terrain water data.");
		return false;
	}

	//	Save scene object data
	if (!SaveSceneObjects(ar))
	{
		g_Log.Log("CElementMap::Save, Failed to save scene object data");
		return false;
	}

	//  Save cloud data
	CString cloudPath;
	cloudPath.Format("%s%s\\%s\\%s.scld",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
	if (m_pCloudCreator) 
	{
		if(!m_pCloudCreator->SaveCloud(cloudPath))
		{
			g_Log.Log("CElementMap::Save, Failed to save cloud data");
			return false;
		}
		cloudPath.Format("%s%s\\%s\\%s.cldo",g_szWorkDir,g_szEditMapDir,GetMapName(),GetMapName());
		SaveCloudOption(cloudPath,0);
	}

	//  Save terrain pass map
	if(!SavePassMap()) 
	{
		g_Log.Log("CElementMap::Save, Failed to save terrain pass map");
		return false;
	}

	if (!SaveCurPlayerPassMap()){
		g_Log.Log("CElementMap::Save, Failed to save current layer player pass map");
		return false;
	}

	return true;
}

bool CElementMap::SaveCloudOption(CString pathName, DWORD dwVersion)
{
	AFile file;
	DWORD dwWrited = 0;
	if(!file.Open(pathName,AFILE_CREATENEW))
	{
		g_Log.Log("CElementMap::SaveCloudOption(),Failed to open file %s.",pathName);
		return false;
	}
	CloudCreatorParam* par = m_pCloudCreator->GetCloudCreatorParam();
	if(!file.Write(&dwVersion,sizeof(DWORD),&dwWrited)) return false;
	if(!file.Write(&par->bOneBoxOneSprite,sizeof(bool),&dwWrited)) return false;
	if(!file.Write(&par->bUseBase,sizeof(bool),&dwWrited)) return false;
	if(!file.Write(par->color,sizeof(A3DCOLORVALUE)*5,&dwWrited)) return false;
	if(!file.Write(&par->fCullDist,sizeof(float),&dwWrited)) return false;
	if(!file.Write(&par->fSpriteSize,sizeof(float),&dwWrited)) return false;
	if(!file.Write(par->height,sizeof(float)*5,&dwWrited)) return false;
	if(!file.Write(&par->nCloudType,sizeof(int),&dwWrited)) return false;
	if(!file.Write(&par->nSpriteNum,sizeof(int),&dwWrited)) return false;
	return true;
}

bool CElementMap::LoadCloudOption(CString pathName)
{
	AFile file;
	DWORD dwWrited = 0;
	DWORD dwVersion;
	if(!file.Open(pathName,AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::LoadCloudOption(),Failed to open file %s.",pathName);
		return false;
	}
	CloudCreatorParam par;
	if(!file.Read(&dwVersion,sizeof(DWORD),&dwWrited)) return false;
	if(!file.Read(&par.bOneBoxOneSprite,sizeof(bool),&dwWrited)) return false;
	if(!file.Read(&par.bUseBase,sizeof(bool),&dwWrited)) return false;
	if(!file.Read(par.color,sizeof(A3DCOLORVALUE)*5,&dwWrited)) return false;
	if(!file.Read(&par.fCullDist,sizeof(float),&dwWrited)) return false;
	if(!file.Read(&par.fSpriteSize,sizeof(float),&dwWrited)) return false;
	if(!file.Read(par.height,sizeof(float)*5,&dwWrited)) return false;
	if(!file.Read(&par.nCloudType,sizeof(int),&dwWrited)) return false;
	if(!file.Read(&par.nSpriteNum,sizeof(int),&dwWrited)) return false;
	if(m_pCloudCreator) m_pCloudCreator->SetCloudCreatorParam(&par);
	return true;
}

//	Load map general data
bool CElementMap::LoadGeneralMapData(CELArchive& ar)
{
	//	Read file header
	if (!ar.Read(&m_Header, sizeof (m_Header)))
	{
		g_Log.Log("CElementMap::LoadGeneralMapData, Failed to load project file header.");
		return false;
	}

	if(m_Header.dwVersion > EPRJFILE_VERSION)
	{
		AfxMessageBox("编辑器版本太旧，请用新的编辑器!");
		return false;
	}

	if (m_Header.dwIdentity != EPRJFILE_IDENTIFY)
	{
		g_Log.Log("CElementMap::LoadGeneralMapData, This isn't a element project file.");
		return false;
	}

	if (m_Header.dwVersion < 7)
	{
		g_Log.Log("CElementMap::LoadGeneralMapData, File's version (%d) is too old, (%d) wanted.", m_Header.dwVersion, EPRJFILE_VERSION);
		return false;
	}

	//	Read project file name
	m_strMapName = ar.ReadString();

	//	Read resource directory
	m_strResDir = ar.ReadString();
	m_strResDir.Format("%s\\%s",g_szEditMapDir,m_strMapName);

	//	Make map file name
	MakeMapFileName();

	//	Read lighting parameters
	if (m_Header.dwVersion < 8)
	{
		ar.Read(&m_Lighting.vSunDir, sizeof (A3DVECTOR3));
		ar.Read(&m_Lighting.dwAmbient,sizeof(DWORD));
		ar.Read(&m_Lighting.dwSunCol,sizeof(DWORD));
		ar.Read(&m_Lighting.iMapSize,sizeof(int));
		m_Lighting.fSunPower = 1.0f;
		m_NightLighting = m_Lighting;
	}else if(m_Header.dwVersion < 9)
	{
		ar.Read(&m_Lighting, sizeof (m_Lighting));
		m_NightLighting = m_Lighting;
	}else if(m_Header.dwVersion < 10)
	{
		ar.Read(&m_Lighting, sizeof (m_Lighting));
		ar.Read(&m_NightLighting, sizeof(m_NightLighting));
	}else
	{
		ar.Read(&m_Lighting, sizeof (m_Lighting));
		ar.Read(&m_NightLighting, sizeof(m_NightLighting));
		ar.Read(&g_Configs.fSunFaction,sizeof(float));
		ar.Read(&g_Configs.fNightSunFaction,sizeof(float));
	}

	if(m_Header.dwVersion < 16)
	{
		m_Lighting.iMapSize = 1024;
		if(m_Lighting.fSunPower <= 0.01f) m_Lighting.fSunPower = m_NightLighting.fSunPower;
	}
	m_NightLighting.vSunDir = m_Lighting.vSunDir;
	m_NightLighting.iMapSize = m_Lighting.iMapSize;
	return true;
}

bool CElementMap::ReadGeneralMapData(CELArchive& ar)
{
	//	Read file header
	MAPFILEHEADER head;
	if (!ar.Read(&head, sizeof (head))) return false;
	if (m_Header.dwIdentity != EPRJFILE_IDENTIFY) return false;
	if (m_Header.dwVersion < 7) return false;

	//	Read project file name
	ar.ReadString();
	//	Read resource directory
	ar.ReadString();
	A3DVECTOR3 vTemp;
	DWORD dwTemp;
	int   nTemp;
	LIGHTINGPARAMS light;
	float fTemp;
	//	Read lighting parameters
	if (m_Header.dwVersion < 8)
	{
	
		ar.Read(&vTemp, sizeof (A3DVECTOR3));
		ar.Read(&dwTemp,sizeof(DWORD));
		ar.Read(&dwTemp,sizeof(DWORD));
		ar.Read(&nTemp,sizeof(int));
	}else if(m_Header.dwVersion < 9)
	{
		ar.Read(&light, sizeof (light));
	}else if(m_Header.dwVersion < 10)
	{
		ar.Read(&light, sizeof (light));
		ar.Read(&light, sizeof(light));
	}else
	{
		ar.Read(&light, sizeof (light));
		ar.Read(&light, sizeof(light));
		ar.Read(&fTemp,sizeof(float));
		ar.Read(&fTemp,sizeof(float));
	}
	return true;
}

//	Load terrain data
bool CElementMap::LoadTerrain(CELArchive& ar)
{
	//	Read terrain file name
	CString strFile = ar.ReadString();
	//	Make full path name
	CString strFullFile;
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, strFile);

	AFile File;
	if (!File.Open(strFullFile, AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::LoadTerrain, Failed to open terrain file %s", strFile);
		return false;
	}

	//	Create file archive
	CELFileArchive arTerrain(&File, false);

	//	Load empty map
	if (!m_pTerrain->Load(arTerrain))
	{
		File.Close();
		g_Log.Log("CElementMap::LoadTerrain, Failed to call CTerrain::Load");
		return false;
	}

	File.Close();

	return true;
}

//

bool CElementMap::LoadTerrainForNeighbor(CELArchive& ar)
{
	//	Read terrain file name
	CString strFile = ar.ReadString();
	//	Make full path name
	CString strFullFile;
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, strFile);

	AFile File;
	if (!File.Open(strFullFile, AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::LoadTerrain, Failed to open terrain file %s", strFile);
		return false;
	}

	//	Create file archive
	CELFileArchive arTerrain(&File, false);

	//	Load empty map
	if (!m_pNeighborTerrain->Load(arTerrain))
	{
		File.Close();
		g_Log.Log("CElementMap::LoadTerrain, Failed to call CTerrain::Load");
		return false;
	}
	File.Close();
	return true;
}

//	Save terrain data
bool CElementMap::SaveTerrain(CELArchive& ar)
{
	//	Save terrain map file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s.terrain", m_strMapName);
	ar.WriteString(szFile);

	//	Make full path name
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);

	AFile File;
	if (!File.Open(szFullName, AFILE_CREATENEW))
	{
		g_Log.Log("CElementMap::SaveTerrain, Failed to create terrain file %s", szFile);
		return false;
	}

	//	Create file archive
	CELFileArchive arTerrain(&File, true);

	//	Save terrain data
	if (!m_pTerrain->Save(arTerrain))
	{
		g_Log.Log("CElementMap::SaveTerrain, Failed to call CTerrain::Save.");
		return false;
	}

	File.Close();

	return true;
}

//	Load terrain water data
bool CElementMap::LoadTerrainWater(CELArchive& ar)
{
	DWORD dwWaterID;
	ar.Read(&dwWaterID, sizeof(DWORD));
	if( dwWaterID != 0x1234 )
	{
		// not contain the water in this file
		ar.Seek(-sizeof(DWORD), AFILE_SEEK_CUR);
		// now init an empty terrain water
		if (!m_pTerrainWater->Init())
		{
			g_Log.Log("CElementMap::LoadTerrain, Failed to CTerrainWater::Load.");
			return false;
		}
		return true;
	}

	//	Read terrain file name
	CString strFile = ar.ReadString();
	
	//	Make full path name
	CString strFullFile;
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, strFile);

	if (!m_pTerrainWater)
		return true;

	//	Load terrain water
	if (!m_pTerrainWater->Load(strFullFile))
	{
		g_Log.Log("CElementMap::LoadTerrain, Failed to CTerrainWater::Load.");
		return false;
	}

	return true;
}

bool CElementMap::ExpWaterAreaMap(const char *szFile)
{
	CWaterAreaMap wmap;
	
	int n = m_pTerrainWater->GetNumWaterAreas();
	float fWidth = m_pTerrain->GetTerrainSize();
	wmap.SetSize(fWidth, fWidth);
	for( int i = 0; i < n; i++)
	{
		A3DWaterArea *pArea = m_pTerrainWater->GetWaterArea(i);
		if(pArea==NULL) continue;
		A3DVECTOR3 vCenter = pArea->GetCenter();
		float fHeight = pArea->GetWaterHeight();
		int h = pArea->GetHeight();
		int w = pArea->GetWidth();
		float half = pArea->GetGridSize()/2.0f;
		wmap.AddWaterArea(vCenter.x,vCenter.z,half*w,half*h,fHeight);
	}
	
	if(!wmap.Save(szFile))
	{
		wmap.Release();
		return false;
	}
	wmap.Release();
	return true;
}

//	Save terrain water data
bool CElementMap::SaveTerrainWater(CELArchive& ar)
{
	if (!m_pTerrainWater)
		return true;

	// first write a id flag
	DWORD dwWaterID = 0x1234;
	ar.Write(&dwWaterID, sizeof(DWORD));

	//	Save terrain map file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s.water", m_strMapName);
	ar.WriteString(szFile);
	
	//	Make full path name
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);

	//	Save terrain data
	if (!m_pTerrainWater->Save(szFullName))
	{
		g_Log.Log("CElementMap::SaveTerrainWater, Failed to call CTerrainWater::Save.");
		return false;
	}

	return true;
}

//	Load edit record from file
bool CElementMap::LoadEditRecord()
{
	//	Make full path name
	char szFullName[MAX_PATH], szFile[MAX_PATH];
	sprintf(szFile, "%s.record", m_strMapName);
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);

	AFile File;
	if (!File.Open(szFullName, AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::LoadEditRecord, Failed to open record file %s", szFullName);
		return false;
	}

	DWORD dwRead;
	EDITRECORDFILEDATA Data;

	File.Read(&Data, sizeof (Data), &dwRead);

	File.Close();

	m_pTerrain->SetRenderUpdateCnt2(Data.dwTrnRndUpdate2);

	return true;
}

//	Save edit record data to file
bool CElementMap::SaveEditRecord()
{
	//	Make full path name
	char szFullName[MAX_PATH], szFile[MAX_PATH];
	sprintf(szFile, "%s.record", m_strMapName);
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);

	AFile File;
	if (!File.Open(szFullName, AFILE_CREATENEW))
	{
		g_Log.Log("CElementMap::SaveEditRecord, Failed to create record file %s", szFullName);
		return false;
	}

	EDITRECORDFILEDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.dwTrnRndUpdate2	= m_pTerrain->GetRenderUpdateCnt2();

	DWORD dwWrite;
	File.Write(&Data, sizeof (Data), &dwWrite);

	File.Close();

	return true;
}

//	Get lights which infect specified position, don't consider sun and environment ambient
bool CElementMap::GetInfectiveLight(const A3DVECTOR3& vPos, APtrArray<CLight*>& aLights)
{
	return true;
}

//	Do ray trace in map
bool CElementMap::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt)
{
	pTraceRt->fFraction	= 1.0f;

	RAYTRACERT TraceRt;

	if (m_pTerrain && m_pTerrain->RayTrace(vStart, vVel, fTime, &TraceRt))
	{
		if (TraceRt.fFraction < pTraceRt->fFraction)
			*pTraceRt = TraceRt;
	}

	if (pTraceRt->fFraction < 1.0f)
		return true;

	return false;
}

/*	Recreate terrain render. This function is often called when terrain's 
	height map size or layers changed

	bHMChanged: height map changed flag.
	bLighting: true, generate light map
*/
bool CElementMap::RecreateTerrainRender(bool bHMChanged, bool bLighting)
{
	//  Get project list data
	int     pNeiIndices[9];
	CString pNeiName[9];
	//	Export terrain data to temporary directory
	CString strName = m_strMapName;
	ChangeMapName(g_szTempMap);

	CString strPath;
	strPath.Format("%s%s", g_szWorkDir, m_strMapFile);

	//	Save terrain normal data, this can accelerate terrain loading speed
	m_pTerrain->SetSaveNormalFlag(true);

	if (!Save(strPath))
	{
		m_pTerrain->SetSaveNormalFlag(false);
		ChangeMapName(strName);
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to save temporary map data");
		return false;
	}
	
	m_pTerrain->SetSaveNormalFlag(false);

	//	Restore map name
	ChangeMapName(strName);

	//	Fill terrain exporter initial data
	strName += "Render";
	strPath.Format("%s%s", g_szWorkDir, m_strResDir);
	
	CTerrainExportor::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szName		= strName;
	InitData.szDestDir	= strPath;
	InitData.iBlockGrid	= g_Configs.iTExpBlkGrid;
	InitData.iMaskTile	= AUX_GetMaskTileSize();
	InitData.iNumCol	= 1;
	InitData.iNumRow	= 1;
	InitData.pLMGen		= NULL;

	CLightMapGen LMGen;         // Light for day
	CLightMapGen LMNightGen;    // Light for night
	char szFile[256];
	char szFullName[256];
	if (bLighting)
	{
		//	Create a light map generator
		if (!LMGen.Init(GetLightingParams()))
		{
			g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize light map generator");
			return false;
		}
		//  A temp solution, we want LightGen can adjust the light result of this map's scene objects
		LMGen.SetCurrentMap(this);
		
		if(!LMNightGen.Init(GetNightLightingParams()))
		{
			g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize night light map generator");
			g_Configs.bShowGrass = true; 
			return false;
		}
		LMNightGen.SetCurrentMap(this);
		InitData.pLMGen	= &LMGen;
		InitData.pLMNightGen = &LMNightGen;
		m_pSceneObjectMan->UpdateLighting(false);
	}

	g_Render.GetA3DDevice()->Reset();
	//	Create terrain exporter
	CTerrainExportor Exportor;
	if(bLighting) Exportor.m_bLightNeighbor = false;
	if (!Exportor.Init(InitData))
	{
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to initialize terrain exporter");
		return false;
	}

	if(InitData.pLMGen && InitData.pLMNightGen)
	{
		if(!GetProjectListInfor(m_strMapName,pNeiIndices,pNeiName))
		{
			for( int n = 0; n < 9; n++)
			{
				pNeiIndices[n] = -1;
				pNeiName[n] = "";
			}
		}
		//	Add project names ...
		for(int i = 0; i < 9; i++)
			Exportor.m_strExtProj[i] = pNeiName[i];//Add by XQF
	}
	
	
	Exportor.AddProject(g_szTempMap);

	//	Release current terrain render, so that file share collision won't happen
	m_pTerrain->ReleaseRender();
	
	//	Make full path name
	sprintf(szFile, "%s.gld", m_strMapName);
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	
	bool bRet;
	
	if (!(bRet = Exportor.DoExport()))
		g_Log.Log("CElementMap::RecreateTerrainRender, Failed to export terrain data");

	Exportor.Release();
	LMGen.Release();
	LMNightGen.Release();
	
	//	Reload terrain render
	m_pTerrain->ReloadRender(false);
	m_pTerrain->SetUpdateRenderFlag(false);
	m_pTerrain->GetMaskModifier()->ReleaseAllModifies();
	if(m_pTerrain)
	{
		m_pTerrain->GetMaskModifier()->ClearModifyFlag();
		CTerrainRender *pRender = m_pTerrain->GetRender();
		if(pRender) pRender->EnableLight(g_bShowTerrainLight);
	}
	CSceneBoxArea *pArea = m_pSceneObjectMan->GetCurrentArea();
	if(pArea==NULL) return true;
	BOXAREA_PROPERTY dat;
	pArea->GetProperty(dat);
	A3DLIGHTPARAM param;
	A3DLight* pLight = g_Render.GetDirLight();
	param = pLight->GetLightparam();
	param.Diffuse = A3DCOLORVALUE(dat.m_dwDirLightClr)*dat.m_fSunPower;
	param.Direction = dat.m_vLightDir;
	pLight->SetLightParam(param);	
	return bRet;
}

//	Update terrain's heights. This function is often called when terrain's 
//	height map content but not size changed.
bool CElementMap::UpdateTerrainHeight(const ARectF& rcArea)
{
	m_pTerrain->UpdateVertexPos(rcArea);
	return true;
}

//	Load scene object
bool CElementMap::LoadSceneObjects(CELArchive& ar)
{
	g_LogDlg.Clear();
	m_pSceneObjectMan->GetElGrassLand()->SetA3DTerrain(m_pTerrain->GetRender());
	CString strFullFile;
	char szFile[256];
	
	//	Read lit model file name
	if(m_Header.dwVersion > 21)	//此时静态模型信息存放到了XRender.smod文件中
		sprintf(szFile, "%sRender.smod", m_strMapName);
	else						//<=21,存放在X.smod文件中
		sprintf(szFile, "%s.smod", m_strMapName);
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	g_nStaticNum = 0;
	if (!m_pSceneObjectMan->LoadLitModels(strFullFile,m_iLoadFlag))
	{
		CString msg;
		msg.Format("CElementMap::LoadSceneObjects, Failed to load %s",strFullFile);
		g_Log.Log(msg);
		AfxMessageBox(msg);
		return false;
	}


	//	Read scene object file name
	CString strFile = ar.ReadString();
	
	//	Make full path name
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, strFile);

	AFile File;
	if (!File.Open(strFullFile, AFILE_OPENEXIST))
		return true;

	//	Create file archive
	CELFileArchive arScene(&File, false);
	
	DWORD testTime = a_GetTime();
	//	Load empty map
	if (!m_pSceneObjectMan->LoadAllObject(arScene,m_iLoadFlag))
	{
		File.Close();
		g_Log.Log("CElementMap::LoadSceneObjects, Failed to call CSceneObjectManager::Load");
		return false;
	}
	File.Close();
	testTime = a_GetTime() - testTime;
	// 兼容10以前版本的灯光数据
	if(m_Header.dwVersion<15)
	{
		ALISTPOSITION pos = m_pSceneObjectMan->m_listSceneObject.GetHeadPosition();
		while( pos )
		{
			CSceneObject *pObj = m_pSceneObjectMan->m_listSceneObject.GetNext(pos);
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_AREA)
			{
				CSceneBoxArea *pArea = (CSceneBoxArea *)pObj;
				BOX_AREA_PROPERTY data;
				pArea->GetProperty(data);
				data.m_dwAmbient		= m_Lighting.dwAmbient;
				data.m_dwDirLightClr	= m_Lighting.dwSunCol; 
				data.m_fSunPower		= m_Lighting.fSunPower;
				data.m_nMapSize			= m_Lighting.iMapSize;
				data.m_vLightDir		= m_Lighting.vSunDir; 
				
				data.m_dwAmbientNight		= m_NightLighting.dwAmbient;
				data.m_dwDirLightClrNight	= m_NightLighting.dwSunCol; 
				data.m_fSunPowerNight		= m_NightLighting.fSunPower;
				data.m_nMapSizeNight		= m_NightLighting.iMapSize;
				data.m_vLightDirNight		= m_NightLighting.vSunDir; 
				pArea->SetProperty(data);
				pArea->UpdateProperty(false);
			}
		}
	}

	//	Read plants file name
	sprintf(szFile, "%s.plt", m_strMapName);
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->LoadForest(strFullFile))
	{
		CString msg;
		msg.Format("CElementMap::LoadSceneObjects, Failed to load %s",strFullFile);
		g_Log.Log(msg);
		AfxMessageBox(msg);
		return false;
	}


	if(m_iLoadFlag != LOAD_EXPLIGHTMAP)
	{
		sprintf(szFile, "%s.gld", m_strMapName);
		strFullFile.Format("%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
		if (!m_pSceneObjectMan->LoadGrassLand(strFullFile))
		{
			CString msg;
			msg.Format("CElementMap::LoadSceneObjects, Failed to load %s",strFullFile);
			g_Log.Log(msg);
			AfxMessageBox(msg);
			return false;
		}
		//  从读取的水块数据创建可编辑水块对象
		for (int x = 0; x<m_pTerrainWater->GetNumWaterAreas(); x++)
		{
			A3DWaterArea *pArea = m_pTerrainWater->GetWaterArea(x);
			AString strName;
			strName.Format("Water_%d",pArea->GetID());
			
			PSCENEWATEROBJECT pObj = m_pSceneObjectMan->CreateWater(pArea->GetID(),strName);
			A3DVECTOR3 vPos;
			vPos = pArea->GetCenter();
			vPos.y = pArea->GetWaterHeight();
			
			pObj->SetWaterColor(pArea->GetWaterColor());
			pObj->SetWaveSize(pArea->GetEdgeSize());
			pObj->UpdateProperty(false);
			//pObj->SetWaterSize(pArea->GetWidth(),pArea->GetHeight(),pArea->GetGridSize());
			pObj->SetWaterGridSize(pArea->GetGridSize());
			pObj->SetWaterWidth(pArea->GetWidth());
			pObj->SetWaterHeight(pArea->GetHeight());
			pObj->SetPos(vPos);
			m_pSceneObjectMan->m_IDGenerator.AddID(pArea->GetID());
		}
		//判断TerrainPlants与Forest 或是 GrassLand 是否是一致的，如果不一致
		//我们就以渲染部分为准，该段程序主要用于排除以前数据相互覆盖而带来的错误
		//start
		//if(m_Header.dwVersion<14)
		{
			CTerrainPlants *pPlants = m_pSceneObjectMan->GetPlants();
			pPlants->Release();
			
			CELGrassLand *pGrassLand = m_pSceneObjectMan->GetElGrassLand();
			int count_type = pGrassLand->GetNumGrassType();
			for(int t = 0; t < count_type; t ++)
			{
				CELGrassType *pGrassType = pGrassLand->GetGrassType(t);
				if(pGrassType == NULL) continue;
				AString name;
				name.Format("Grass_%d",t);
				PPLANT pNew = pPlants->AddPlant(name,pGrassType->GetTypeID() , PLANT_GRASS);
				ASSERT(pNew);
				int count_map = pGrassType->GetGrassBitsMapCount();
				for(int m = 0; m < count_map; m++)
				{
					CELGrassBitsMap* pBtMap = pGrassType->GetGrassBitsMapByIndex(m);
					((GRASS_PLANT*)pNew)->SetMaskFlag(GetGrassMaskAreaID(pBtMap->GetCenter()),pBtMap->GetID());
				}
			}
			
			CELForest *pForest = m_pSceneObjectMan->GetElForest();
			count_type = pForest->GetNumTreeType();
			for(t = 0; t < count_type; t++)
			{
				CELTree * pTree = pForest->GetTreeType(t);
				if(pTree==NULL) continue;
				AString treeName;
				treeName.Format("Tree_%d",t);
				PPLANT pNew = pPlants->AddPlant(treeName,pTree->GetTypeID(),PLANT_TREE);
				if(pNew==NULL) continue;
			
				for( int i = 0; i < pTree->m_Trees.GetSize(); i++)
				{
					CELTree::ELTREE dat1 = pTree->m_Trees.GetAt(i);
					for( int j = i + 1; j < pTree->m_Trees.GetSize(); j++)
					{
						CELTree::ELTREE dat2 = pTree->m_Trees.GetAt(j);
						float a = fabs(dat1.vecPos.x - dat2.vecPos.x);
						float b = fabs(dat1.vecPos.y - dat2.vecPos.y);
						float c = fabs(dat1.vecPos.z - dat2.vecPos.z);
						
						if(a <0.001f && b < 0.001f && c < 0.001f)
							pTree->DeleteTree(dat2.dwID);
					}
				}
				
				int numTree = pTree->m_Trees.GetSize();
				for( int m = 0; m < numTree; m++)
				{
					CELTree::ELTREE dat = pTree->m_Trees.GetAt(m);
					((TREE_PLANT*)pNew)->AddPlant2(dat.vecPos.x,dat.vecPos.y,dat.vecPos.z,dat.dwID);
				}
			}
		}
		//end
		AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	}
	return true;
}

int CElementMap::GetGrassMaskAreaID(A3DVECTOR3 vCenter)
{
	
	ARectF rc = m_pTerrain->GetTerrainArea();
	float fTileSize = m_pTerrain->GetTileSize();
	if(NULL==m_pTerrain->GetRender()) return -1;
	int maskGrid = ((A3DTerrain2*)m_pTerrain->GetRender())->GetMaskGrid();
	fTileSize = maskGrid * fTileSize;
	
	int w = (int)((vCenter.x - rc.left)/fTileSize);
	int h = (int)((rc.top - vCenter.z)/fTileSize);
	int width = (int)(m_pTerrain->GetTerrainSize()/fTileSize);
	
	int nMaskAreaID  =  h* width + w;
	return nMaskAreaID;
}

//	Save scene object
bool CElementMap::SaveSceneObjects(CELArchive& ar)
{
	//	Save scene object file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s.scene", m_strMapName);
	ar.WriteString(szFile);

	//	Make full path name
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);

	AFile File;
	if (!File.Open(szFullName, AFILE_CREATENEW))
	{
		g_Log.Log("CElementMap::SaveSceneObjects, Failed to create scene object file %s", szFile);
		return false;
	}

	//	Create file archive
	CELFileArchive arScene(&File, true);

	//	Save scene object data
	if (!m_pSceneObjectMan->SaveAllObject(arScene))
	{
		g_Log.Log("CElementMap::SaveSceneObjects, Failed to call CSceneObjectManager::Save.");
		return false;
	}

	// Save static lit model
	sprintf(szFile, "%sRender.smod", m_strMapName);
	//	Make full path name
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->SaveLitModels(szFullName))
	{
		g_Log.Log("CElementMap::SaveSceneObjects, Failed to call CSceneObjectManager::SaveLitModels.");
		return false;
	}
	
	sprintf(szFile, "%s.plt", m_strMapName);
	//	Make full path name
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->SaveForest(szFullName))
	{
		g_Log.Log("CElementMap::SaveForest, Failed to call CSceneObjectManager::SaveForest.");
		return false;
	}

	//	Make full path name
	sprintf(szFile, "%s.gld", m_strMapName);
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->SaveGrassLand(szFullName))
	{
		g_Log.Log("CElementMap::SaveGrassLand, Failed to call CSceneObjectManager::SaveGrassLand.");
		return false;
	}
	
	//	Save water map data
	sprintf(szFile, "%s.whmap", m_strMapName);
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->SaveWaterMap(szFullName))
	{
		g_Log.Log("CElementMap::SaveWaterMap, Failed to call CSceneObjectManager::SaveWaterMap.");
		return false;
	}
	

	File.Close();
	return true;
}

bool CElementMap::SaveLitModel()
{
	char szFile[256];
	char szFullName[256];
	// Save static lit model
	sprintf(szFile, "%s.smod", m_strMapName);
	//	Make full path name
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_strResDir, szFile);
	if (!m_pSceneObjectMan->SaveLitModels(szFullName))
	{
		g_Log.Log("CElementMap::SaveLitModel(), Failed to call CSceneObjectManager::SaveLitModels.");
		return false;
	}
	return true;
}

void MapRenderForReflected(A3DViewport * pViewport, void * pArg)
{
	CElementMap * pMap = (CElementMap *) pArg;
	pMap->RenderObjectsForReflect(pViewport);
}

void MapRenderForRefracted(A3DViewport * pViewport, void * pArg)
{
	CElementMap * pMap = (CElementMap *) pArg;
	pMap->RenderObjectsForRefract(pViewport);
}

// Get 8 neighbor information of project list /XQF
bool GetProjectListInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[])
{
	DWORD dwNumCol;
	int   nCurrent = -1;
	int   nItem = 0;
	// Load project file list
	AScriptFile File;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!",szProj);
		return false;
	}
	
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			dwNumCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			dwNumCol = 1;
		}
			
	}
	
	abase::vector<CString *> tempArray;
	
	while (File.GetNextToken(true))
	{
		CString *pNew = new CString(File.m_szToken);
		ASSERT(pNew);
		tempArray.push_back(pNew);
		if(strcmp(File.m_szToken,strCurrentPrj)==0)
			nCurrent = nItem; 
		nItem++;
	}
	File.Close();
	
	// 0 1 2
	// 3 4 5
	// 6 7 8
	if((nCurrent+1)/dwNumCol <= 0 || (nCurrent+1)%dwNumCol==1) pNeiIndices[0] = -1;
	else pNeiIndices[0] = (nCurrent - dwNumCol) - 1;
	
	if((nCurrent+1)/dwNumCol <= 0) pNeiIndices[1] = -1;
	else pNeiIndices[1] = (nCurrent - dwNumCol);
	
	if((nCurrent+1)%dwNumCol==0 || (nCurrent+1)/dwNumCol <= 0) pNeiIndices[2] = -1;
	else pNeiIndices[2] = (nCurrent - dwNumCol) + 1;

	if((nCurrent+1)%dwNumCol==1) pNeiIndices[3] = -1;
	else pNeiIndices[3] = (nCurrent) - 1;
	
	pNeiIndices[4] = (nCurrent);
	
	if((nCurrent+1)%dwNumCol==0) pNeiIndices[5] = -1;
	else pNeiIndices[5] = (nCurrent) + 1;

	if((nCurrent+1)%dwNumCol==1 || (nCurrent+1)/dwNumCol >= (int)tempArray.size()/dwNumCol) pNeiIndices[6] = -1;
	else pNeiIndices[6] = (nCurrent + dwNumCol) - 1;
	
	if((nCurrent+1)/dwNumCol >= (int)tempArray.size()/dwNumCol) pNeiIndices[7] = -1;
	else pNeiIndices[7] = (nCurrent + dwNumCol);
	
	if((nCurrent+1)%dwNumCol==0 || (nCurrent+1)/dwNumCol >= (int)tempArray.size()/dwNumCol) pNeiIndices[8] = -1;
	else pNeiIndices[8] = (nCurrent + dwNumCol) + 1;

	for(int i = 0; i < 9; i++)
	{
		if(pNeiIndices[i] < 0 || pNeiIndices[i] >= (int)tempArray.size())
			pNeiIndices[i] = -1;
		
		int index = pNeiIndices[i];
		
		if(index==-1) continue;
		pNeiName[i] = *tempArray.at(index);
	}
 
	
	// Release array
	for(i = 0; i< (int)tempArray.size();i++)
	{
		CString* p = tempArray.at(i);
		delete p;
	}
	tempArray.clear();
	
	return true;
}

const LIGHTINGPARAMS& CElementMap::GetLightingParams() 
{ 
	return m_Lighting;
	/*
	if(m_pSceneObjectMan && EPRJFILE_VERSION > 9)
	{
		BOX_AREA_PROPERTY proper;
		CSceneBoxArea *pArea = m_pSceneObjectMan->GetDefaultBoxArea();
		if(pArea) pArea->GetProperty(proper);
		else {
			g_Log.Log("错误：场景中居然没有默认区域，请赶快加一个默认区域!");
			return m_Lighting;
		}
		m_Lighting.dwAmbient = proper.m_dwAmbient;
		m_Lighting.dwSunCol =  proper.m_dwDirLightClr;
		m_Lighting.fSunPower = proper.m_fSunPower;
		m_Lighting.iMapSize =  proper.m_nMapSize;
		m_Lighting.vSunDir =   proper.m_vLightDir;
		return m_Lighting;
	}else return m_Lighting; 
	*/
}
	//  Get night lighting parameters;
const LIGHTINGPARAMS& CElementMap::GetNightLightingParams()
{ 
	return m_NightLighting;
	/*
	if(m_pSceneObjectMan && EPRJFILE_VERSION > 9)
	{
		BOX_AREA_PROPERTY proper;
		CSceneBoxArea *pArea = m_pSceneObjectMan->GetDefaultBoxArea();
		if(pArea) pArea->GetProperty(proper);
		else {
			g_Log.Log("错误：场景中居然没有默认区域，请赶快加一个默认区域!");
			return m_Lighting;
		}
		m_NightLighting.dwAmbient = proper.m_dwAmbientNight;
		m_NightLighting.dwSunCol =  proper.m_dwDirLightClrNight;
		m_NightLighting.fSunPower = proper.m_fSunPowerNight;
		m_NightLighting.iMapSize =  proper.m_nMapSizeNight;
		m_NightLighting.vSunDir =   proper.m_vLightDirNight;
		return m_NightLighting;
	}else return m_NightLighting; */
};

//	Set map offset
void CElementMap::SetMapOffset(float x, float z)
{ 
	//先把他移到原始位置
	if (m_pTerrain)
		m_pTerrain->SetMapOffset(-m_fOffsetX, -m_fOffsetZ);
	
	m_fOffsetX = x; 
	m_fOffsetZ = z; 

	//重新偏移
	if (m_pTerrain)
		m_pTerrain->SetMapOffset(x, z);
}

bool GetMapBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f)
{
	A3DVECTOR3 verts[8];
	A3DVECTOR3 vecX = A3DVECTOR3(2 * aabb.Extents.x, 0.0f, 0.0f);
	A3DVECTOR3 vecY = A3DVECTOR3(0.0f, 2 * aabb.Extents.y, 0.0f);
	A3DVECTOR3 vecZ = A3DVECTOR3(0.0f, 0.0f, 2 * aabb.Extents.z);

	verts[0] = aabb.Mins;
	verts[1] = aabb.Mins + vecX;
	verts[2] = aabb.Mins + vecY;
	verts[3] = aabb.Mins + vecZ;
	verts[4] = aabb.Maxs;
	verts[5] = aabb.Maxs - vecX;
	verts[6] = aabb.Maxs - vecY;
	verts[7] = aabb.Maxs - vecZ;

	A3DAABB aabbWorld;
	aabbWorld.Clear();

	A3DVECTOR3 vert;
	int i;
	for(i=0; i<8; i++)
	{
		vert = verts[i]	* viewTM;
		aabbWorld.AddVertex(vert);
	}

	l = aabbWorld.Mins.x;
	r = aabbWorld.Maxs.x;
	b = aabbWorld.Mins.y;
	t = aabbWorld.Maxs.y;
	n = aabbWorld.Mins.z;
	f = aabbWorld.Maxs.z;

	return true;
}

// Export a bitmap of top view
bool CElementMap::ExpTopView(CString expPath ,int nWidth,bool bNight)
{
	int nMapWidth = nWidth;
	bool bReadOnlyOld = g_bReadOnly;
	g_bReadOnly = false;
	bool bOldFlags = g_pA3DConfig->RT_GetShowFPSFlag();
	float fOldFactor = g_fNightFactor;
	if(bNight) g_fNightFactor = 1.0f;
	else g_fNightFactor = 0.0f;
	//Only for topview export,start...
	/*
	m_Lighting.dwAmbient = A3DCOLORRGB(192,192,192);
	m_Lighting.dwSunCol = A3DCOLORRGB(192,192,192);
	m_Lighting.fSunPower = 1.0f;
	A3DLight* pLight = g_Render.GetDirLight();
	A3DLIGHTPARAM param;
	param = pLight->GetLightparam();
	param.Ambient = A3DCOLORVALUE(m_Lighting.dwAmbient);
	param.Diffuse = A3DCOLORVALUE(m_Lighting.dwSunCol);
	param.Direction = m_Lighting.vSunDir;
    pLight->SetLightParam(param);
	RecreateTerrainRender(true,true);
	*/
	// Topview end.
	
	g_pA3DConfig->RT_SetShowFPSFlag(false);
	//Create top view map Viewport
	A3DViewport* pViewport = NULL;
	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	pViewport = pA3DEngine->GetActiveViewport();
	A3DVIEWPORTPARAM* ptemp = pViewport->GetParam();
	A3DVIEWPORTPARAM newvp;
	A3DVIEWPORTPARAM oldvp;
	oldvp = *ptemp;
	newvp.Height = nMapWidth;
	newvp.Width  = nMapWidth;
	newvp.MinZ   = 0.0f;
	newvp.MaxZ   = 1.0f;
	newvp.X      = 0.0f;
	newvp.Y      = 0.0f;
	pViewport->SetParam(&newvp);
	A3DCameraBase *pOldCam = pViewport->GetCamera();
	
	// now create a ortho camera
	A3DOrthoCamera *pCamera = new A3DOrthoCamera();
	ASSERT(pCamera);
	if( !pCamera->Init(pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CElementMap::ExpTopView(), failed to init ortho camera!");
		return false;
	}
	
	CTerrain *pTerrain = GetTerrain();
	pTerrain->GetRender()->SetLODType(A3DTerrain2::LOD_NONE);

	ARectF rectTerrain = pTerrain->GetTerrainArea();
	A3DVECTOR3 vecCenter = A3DVECTOR3((rectTerrain.left + rectTerrain.right) / 2.0f, 
	pTerrain->GetMaxHeight() * 0.5f + pTerrain->GetYOffset(), (rectTerrain.top + rectTerrain.bottom) / 2.0f);
	A3DVECTOR3 vecExt = A3DVECTOR3(pTerrain->GetTerrainSize()/2.0f);
	A3DAABB aabbWorld(vecCenter - vecExt, vecCenter + vecExt);
	
	float l,r,b,t,n,f;
	pCamera->SetPos(vecCenter);
	pCamera->SetDirAndUp(-g_vAxisY, g_vAxisZ);
	
	GetMapBounds(aabbWorld,pCamera->GetViewTM(),l,r,b,t,n,f);
	pCamera->SetProjectionParam(l, r, b, t, n, f);

	if(m_pTerrainWater) m_pTerrainWater->GetA3DTerrainWater()->ChangeHostCameraAndView(g_Render.GetA3DEngine()->GetActiveViewport(),(A3DCamera*)pCamera);
	// create top view map render target
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= nMapWidth;
	devFmt.nHeight		= nMapWidth;
	devFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	A3DRenderTarget *pRenderTarget = new A3DRenderTarget();
	ASSERT(pRenderTarget);
	if( !pRenderTarget->Init(pA3DDevice, devFmt, true, true) )
	{
		g_Log.Log("CElementMap::ExpTopView(), failed to init render target!");
		g_fNightFactor = fOldFactor;
		return false;
	}
	
	if(pTerrain) pTerrain->FrameMove(0,pCamera);
	if(m_pTerrainWater) m_pTerrainWater->FrameMove(0);
	if(m_pSceneObjectMan) m_pSceneObjectMan->Tick(0);

	pA3DDevice->SetRenderTarget(pRenderTarget);
	pA3DDevice->SetRestoreToRenderTarget(pRenderTarget);
	
	if (!g_Render.BeginRender())
	{
		g_fNightFactor = fOldFactor;
		return false;
	}

	A3DMaterial& Mat = ((A3DTerrain2*)(pTerrain->GetRender()))->GetTerrainMaterial();
	A3DCOLORVALUE old = Mat.GetSpecular();
	Mat.SetSpecular(0,0,0,1.0f);
	pViewport->SetClearColor(A3DCOLORRGB(255,0,255));
	pViewport->SetCamera(pCamera);
	pViewport->Active();
	pViewport->ClearDevice();

//	pA3DEngine->RenderAllRenderTarget();
	pA3DDevice->SetFillMode(A3DFILL_SOLID);
	Render(pViewport);
	//g_Render.GetA3DEngine()->RenderGFX(pViewport, 0xffffffff);
	//pA3DEngine->FlushVertexCollector(pViewport);
	//pA3DEngine->FlushCachedAlphaMesh(pViewport);
	//pA3DEngine->FlushInternalSpriteBuffer();
	//pA3DEngine->FlushMeshCollector(pViewport);
	
	Mat.SetSpecular(old);
	if (!g_Render.EndRender())
	{
		g_fNightFactor = fOldFactor;
		return false;
	}
	pA3DDevice->SetDefaultRestoreToRenderTarget();
	pA3DDevice->RestoreRenderTarget();

	CString pathName;
	int k = atof(GetMapName());
	if(k>=0 && k < 10) pathName.Format("%s\\0%s.bmp",expPath,GetMapName());
	else pathName.Format("%s\\%s.bmp",expPath,GetMapName());
	
	D3DXSaveSurfaceToFile(pathName, D3DXIFF_BMP, pRenderTarget->GetTargetSurface(), NULL , NULL);
	
	g_bReadOnly = bReadOnlyOld;
	g_fNightFactor = fOldFactor;
	pViewport->SetCamera(pOldCam);
	pViewport->SetParam(&oldvp);
	pA3DEngine->SetActiveViewport(pViewport);
	pA3DEngine->SetActiveCamera(pOldCam);
	pA3DEngine->RemoveRenderTarget(pRenderTarget);
	pRenderTarget->Release();
	delete pRenderTarget;
	pCamera->Release();
	delete pCamera;
	g_pA3DConfig->RT_SetShowFPSFlag(bOldFlags);
	return true;
}

CELBitmap *CElementMap::ExpPrecinctMap()
{
	APtrList<CSceneObject *>*pPrecinctList = m_pSceneObjectMan->GetSortObjectList(CSceneObject::SO_TYPE_PRECINCT);
	int np = pPrecinctList->GetCount();
	DWORD* polyList = new DWORD[np];
	DWORD*  polyNum = new DWORD[np];
	int i = 0;
	ALISTPOSITION pos = pPrecinctList->GetHeadPosition();
	while(pos)
	{
		CScenePrecinctObject*pPrecinct = (CScenePrecinctObject*)pPrecinctList->GetNext(pos);
		PRECINCT_DATA dat = pPrecinct->GetProperty();
		CEditerBezier *pBezier = m_pSceneObjectMan->FindBezier(dat.nPath);
		if(pBezier==NULL) 
		{
			polyList[i] = 0;
			polyNum[i++] = 0;
			continue;
		}
		int plNum = pBezier->GetPointCount() + 1;
		POINT_FLOAT* ptList = new POINT_FLOAT[plNum];	
		A3DVECTOR3 ptpos;	
		POSITION pos = pBezier->m_list_points.GetHeadPosition();
		int index = 0;
		while(pos)
		{
			CEditerBezierPoint * pBPt = (CEditerBezierPoint *)pBezier->m_list_points.GetNext(pos);
			ptpos = pBPt->GetPos();
			ptList[index].x = ptpos.x;
			ptList[index++].y = ptpos.z;
		}
		CEditerBezierPoint * phead = (CEditerBezierPoint *)pBezier->m_list_points.GetHead();
		ptpos = phead->GetPos();
		ptList[index].x = ptpos.x;
		ptList[index].y = ptpos.z;
		polyNum[i] = index + 1;
		polyList[i++] = (DWORD)ptList;
	}

	float div = 1.0f;
	ARectF rc = m_pTerrain->GetTerrainArea();
	ARectI srcRc;
	srcRc.left  = 0;
	srcRc.right = (int)((rc.right - rc.left)/div);
	srcRc.top   = 0;
	srcRc.bottom = (int)((rc.top - rc.bottom)/div);
	
	CELBitmap *pBitmap = new CELBitmap;
	if (!pBitmap->CreateBitmap(srcRc.right, srcRc.bottom, 8, NULL))
	{
		g_Log.Log("CElementMap::ExpPrecinctMap(), Failed to create bitmap");
		for( int k = 0; k<np;k++)
			delete[] (POINT_FLOAT *)polyList[k];
		
		delete[]polyList;
		delete[]polyNum;
		return NULL;
	}
	BYTE *pData = pBitmap->GetBitmapData();
	memset(pData,255,srcRc.right * srcRc.bottom);
	int w = 0;
	for(float left = rc.left; left < rc.right; left +=div)
	{
		int h = srcRc.bottom-1;
		for( float up = rc.bottom; up < rc.top; up +=div)
		{
			for(int x = 0; x < np; x++)
			{
				POINT_FLOAT *pTemp = (POINT_FLOAT *)polyList[x];
				POINT_FLOAT pt;
				pt.x = left;
				pt.y = up;
				if(IsInPolyGon(pt,pTemp,polyNum[x]))
				{
					pData[h*srcRc.right + w] = 0;
					break;
				}
			}
			h--;
		}
		w++;
	}

	for( int k = 0; k<np;k++)
		delete[] (POINT_FLOAT *)polyList[k];
	
	delete[]polyList;
	delete[]polyNum;
	return pBitmap;
}
void CElementMap::MergeTreeType()
{
	CTerrainPlants *pPlants = m_pSceneObjectMan->GetPlants();
	pPlants->Release();
	
	CELGrassLand *pGrassLand = m_pSceneObjectMan->GetElGrassLand();
	int count_type = pGrassLand->GetNumGrassType();
	for(int t = 0; t < count_type; t ++)
	{
		CELGrassType *pGrassType = pGrassLand->GetGrassType(t);
		if(pGrassType == NULL) continue;
		AString name;
		name.Format("Grass_%d",t);
		PPLANT pNew = pPlants->AddPlant(name,pGrassType->GetTypeID() , PLANT_GRASS);
		ASSERT(pNew);
		int count_map = pGrassType->GetGrassBitsMapCount();
		for(int m = 0; m < count_map; m++)
		{
			CELGrassBitsMap* pBtMap = pGrassType->GetGrassBitsMapByIndex(m);
			((GRASS_PLANT*)pNew)->SetMaskFlag(GetGrassMaskAreaID(pBtMap->GetCenter()),pBtMap->GetID());
		}
	}
	
	CELForest *pForest = m_pSceneObjectMan->GetElForest();
	
	DWORD dwID;
	while(1)
	{
		bool bDel = false;
		count_type = pForest->GetNumTreeType();
		for(int t = 0; t < count_type; t++)
		{
			CELTree * pTree1 = pForest->GetTreeType(t);
			if(pTree1->m_Trees.GetSize()==0)
			{
				pForest->DeleteTreeType(pTree1);
				bDel = true;
				break;
			}
			
			for( int x = t + 1; x < count_type; x++)
			{
				CELTree * pTree2 = pForest->GetTreeType(x);
				if(strcmp(pTree1->GetTreeFile(),pTree2->GetTreeFile())==0 && pTree1!=pTree2)
				{
					//Save tree2 type data to tree1 and delete tree2 type
					bDel = true;
					for( int i = 0; i < pTree2->m_Trees.GetSize(); i++)
					{
						CELTree::ELTREE dat1 = pTree2->m_Trees.GetAt(i);
						pTree1->AddTree(dat1.vecPos,dwID);
					}
					pForest->DeleteTreeType(pTree2);
					break;
				}
			}
			if(bDel) break;
		}
		if(!bDel) break;
	}

	count_type = pForest->GetNumTreeType();
	for(t = 0; t < count_type; t++)
	{
		CELTree * pTree = pForest->GetTreeType(t);
		if(pTree==NULL) continue;
		AString treeName;
		treeName.Format("Tree_%d",t);
		PPLANT pNew = pPlants->AddPlant(treeName,pTree->GetTypeID(),PLANT_TREE);
		if(pNew==NULL) continue;
		
		for( int i = 0; i < pTree->m_Trees.GetSize(); i++)
		{
			CELTree::ELTREE dat1 = pTree->m_Trees.GetAt(i);
			for( int j = i + 1; j < pTree->m_Trees.GetSize(); j++)
			{
				CELTree::ELTREE dat2 = pTree->m_Trees.GetAt(j);
				float a = fabs(dat1.vecPos.x - dat2.vecPos.x);
				float b = fabs(dat1.vecPos.y - dat2.vecPos.y);
				float c = fabs(dat1.vecPos.z - dat2.vecPos.z);
				
				if(a <0.001f && b < 0.001f && c < 0.001f)
					pTree->DeleteTree(dat2.dwID);
			}
		}
		
		int numTree = pTree->m_Trees.GetSize();
		for( int m = 0; m < numTree; m++)
		{
			CELTree::ELTREE dat = pTree->m_Trees.GetAt(m);
			((TREE_PLANT*)pNew)->AddPlant2(dat.vecPos.x,dat.vecPos.y,dat.vecPos.z,dat.dwID);
		}
	}
}

bool CElementMap::RecreateCloud()
{
	m_pCloudCreator->ClearBoxInfo();
	APtrList<CSceneObject *>* pList = m_pSceneObjectMan->GetSortObjectList(CSceneObject::SO_TYPE_CLOUD_BOX);	
	int idx = 0; 
	ALISTPOSITION pos = pList->GetTailPosition();
	while( pos )
	{
		CCloudBox* ptemp = (CCloudBox*)pList->GetPrev(pos);
		if(ptemp->IsDeleted()) continue;
		A3DVECTOR3 vMin,vMax;
		CloudBox boxInfo;
		CLOUD_BOX_DATA dat = ptemp->GetProperty();
		ptemp->GetAABB(vMin,vMax);
		boxInfo.nGroup = dat.nGroup;
		boxInfo.nType  = dat.nType;
		boxInfo.vMaxPos = vMax;
		boxInfo.vMinPos = vMin;
		boxInfo.nSeed   =   dat.dwRandSeed;
		m_pCloudCreator->AddBoxInfo(boxInfo);
		idx++;
	}
	
	abase::vector<CloudSprite>* pSprite = NULL;
	abase::vector<CloudGroup>* pGroup   = NULL;
	m_pCloudCreator->CreateCloud();
	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	m_pCloudRender->SetCloudInfo(m_pCloudCreator->GetCloudCreatorParam(),*pSprite,*pGroup);
	
	return true;
}

bool CElementMap::RecreateAllCloudGroup()
{
	m_pCloudCreator->ClearBoxInfo();
	APtrList<CSceneObject *>* pList = m_pSceneObjectMan->GetSortObjectList(CSceneObject::SO_TYPE_CLOUD_BOX);	
	int idx = 0; 
	ALISTPOSITION pos = pList->GetTailPosition();
	while( pos )
	{
		CCloudBox* ptemp = (CCloudBox*)pList->GetPrev(pos);
		if(ptemp->IsDeleted()) continue;
		A3DVECTOR3 vMin,vMax;
		CloudBox boxInfo;
		CLOUD_BOX_DATA dat = ptemp->GetProperty();
		ptemp->GetAABB(vMin,vMax);
		boxInfo.nGroup = dat.nGroup;
		boxInfo.nType  = dat.nType;
		boxInfo.vMaxPos = vMax;
		boxInfo.vMinPos = vMin;
		boxInfo.nSeed   =   dat.dwRandSeed;
		m_pCloudCreator->AddBoxInfo(boxInfo);
		idx++;
	}
	
	abase::vector<CloudSprite>* pSprite = NULL;
	abase::vector<CloudGroup>* pGroup   = NULL;
	
	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	
	int *sizeArray = new int[(int)pGroup->size()];
	if(sizeArray==NULL) return false;
	for( int i = 0; i < (int)pGroup->size(); i++)
	{
		sizeArray[i]= (*pGroup)[i].fSpriteSize;
	}

	for(i = 0; i < (int)pGroup->size(); i++)
	{
		int n = (*pGroup)[i].vSprites.size();
		if(n==0) n = 10; 
		m_pCloudCreator->ReCreateCloud_Group(i,n);
	}

	for(i = 0; i < (int)pGroup->size(); i++)
	{	
		m_pCloudCreator->ChangeSpriteSize(i,sizeArray[i]);
	}

	delete []sizeArray;

	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	m_pCloudRender->SetCloudInfo(m_pCloudCreator->GetCloudCreatorParam(),*pSprite,*pGroup);
	return true;
}

bool CElementMap::RecreateCloudGroup(int nGroup,  float size, int nSpriteNum)
{
	abase::vector<CloudSprite>* pSprite = NULL;
	abase::vector<CloudGroup>* pGroup   = NULL;
	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	int n = nSpriteNum;
	if(nSpriteNum==-1)
	{
		if(nGroup<(int)pGroup->size()) 
			n = (*pGroup)[nGroup].vSprites.size();
		else return false;
	}
	
	//Save old states
	int *sizeArray = new int[(int)pGroup->size()];
	if(sizeArray==NULL) return false;
	for( int i = 0; i < (int)pGroup->size(); i++)
	{
		sizeArray[i]= (*pGroup)[i].fSpriteSize;
	}
	m_pCloudCreator->ReCreateCloud_Group(nGroup,n);
	for(i = 0; i < (int)pGroup->size(); i++)
	{
		m_pCloudCreator->ChangeSpriteSize(i,sizeArray[i]);
	}
	delete []sizeArray;
	
	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	m_pCloudRender->SetCloudInfo(m_pCloudCreator->GetCloudCreatorParam(),*pSprite,*pGroup);
	if(size!=-1) m_pCloudCreator->ChangeSpriteSize(nGroup,size);
	m_pCloudCreator->GetCloudInfo(&pSprite,&pGroup);
	m_pCloudRender->SetCloudInfo(m_pCloudCreator->GetCloudCreatorParam(),*pSprite,*pGroup);
	return true;
}

	//Render for neighbor map
bool CElementMap::LoadNeighborTerrain(int nNeighbor)
{
	if(m_pNeighborTerrain) 
	{
		m_pNeighborTerrain->Release();
		delete m_pNeighborTerrain;
		m_pNeighborTerrain = NULL;
	}

	if(nNeighbor==4) return false;
	
	if (!(m_pNeighborTerrain = new CTerrain(this)))
		return false;

	CString pNeiName[9];
	int     pNeiIndices[9];
	CLightMapTransition trans;
	trans.LoadProjList();
	trans.GetProjectInfor(GetMapName(),pNeiIndices,pNeiName);
	if(pNeiName[nNeighbor].IsEmpty()) return false;
	
	CString strPath;
	strPath.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,pNeiName[nNeighbor],pNeiName[nNeighbor]);
	
	AFile File;
	if (!File.Open(strPath, AFILE_OPENEXIST))
	{
		g_Log.Log("CElementMap::Load, Failed to open file %s.", strPath);
		return false;
	}
	
	CString strOldResPath = m_strResDir;
	CString strOldMapName = m_strMapName;
	m_strMapName = pNeiName[nNeighbor];
	m_strResDir.Format("%s\\%s",g_szEditMapDir,m_strMapName);
	
	//	Create file archive
	CELFileArchive ar(&File, false);
	if(!ReadGeneralMapData(ar)) 
	{
		m_strResDir = strOldResPath;
		m_strMapName = strOldMapName;
		return false;
	}
	if(!LoadTerrainForNeighbor(ar))
	{
		m_strResDir = strOldResPath;
		m_strMapName = strOldMapName;
		return false;
	}
	float fSize = m_pTerrain->GetTerrainSize(); // 1024
	switch(nNeighbor) 
	{
	case 1:
		if(m_pNeighborTerrain) m_pNeighborTerrain->SetMapOffset(0,fSize); break;
	case 3:
		if(m_pNeighborTerrain) m_pNeighborTerrain->SetMapOffset(-fSize,0); break;
	case 5: 
		if(m_pNeighborTerrain) m_pNeighborTerrain->SetMapOffset(fSize,0); break;
	case 7: 
		if(m_pNeighborTerrain) m_pNeighborTerrain->SetMapOffset(0,-fSize); break;
	}
	
	m_strResDir = strOldResPath;
	m_strMapName = strOldMapName;
	File.Close();
	
	if(m_pNeighborTerrain)
	{
		CTerrainRender *pRender = m_pNeighborTerrain->GetRender();
		if(pRender) pRender->EnableLight(g_bShowTerrainLight);
	}
	return true;
}

bool CElementMap::LoadNeighborModels(int nNeighbor)
{
	ReleaseNeighborModels();
	
	CString pNeiName[9];
	int     pNeiIndices[9];
	CLightMapTransition trans;
	trans.LoadProjList();
	trans.GetProjectInfor(GetMapName(),pNeiIndices,pNeiName);
	if(pNeiName[nNeighbor].IsEmpty()) return false;
	
	CString strPath;
	strPath.Format("%s%s\\%s\\%sRender.smod",g_szWorkDir,g_szEditMapDir,pNeiName[nNeighbor],pNeiName[nNeighbor]);

	AFile FileToRead;
	if( !FileToRead.Open(strPath, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		g_Log.Log("CElementMap::LoadNeighborModels(), failed to open file [%s]", strPath);
		return false;
	}
	
	DWORD dwWriteLen;
	DWORD dwVersion;
	DWORD dwReadObjectNum = 0;

	float fSize = m_pTerrain->GetTerrainSize(); // 1024

	// first of all write a version for future compatibility
	
	if( !FileToRead.Read(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto READFAIL;

	if( dwVersion > 0x4 && dwVersion < 0x06)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			AString szObjectName;
			AString szModelPathName;
			AString szModelHullPath;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelHullPath))
				goto READFAIL;
			int nObjectID;;
			FileToRead.Read(&nObjectID,sizeof(int),&dwWriteLen);
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				goto READFAIL;
			}
			A3DVECTOR3 vpos = pNewModel->GetPos();
			switch(nNeighbor) 
			{
			case 1:
				vpos +=A3DVECTOR3(0.0f,0.0f,fSize);break;
			case 3:
				vpos +=A3DVECTOR3(-fSize,0.0f,0.0f);break;
				
			case 5: 
				vpos +=A3DVECTOR3(fSize,0.0f,0.0f);break;
				
			case 7: 
				vpos +=A3DVECTOR3(0.0f,0.0f,-fSize);break;
			}
			pNewModel->SetPos(vpos);
			m_pNeighborModelList.push_back(pNewModel);
		}		
	}else if(dwVersion < 0x7)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			AString szObjectName;
			AString szModelPathName;
			AString szModelHullPath;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelHullPath))
				goto READFAIL;
			int nObjectID;;
			FileToRead.Read(&nObjectID,sizeof(int),&dwWriteLen);
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				goto READFAIL;
			}
			A3DVECTOR3 vpos = pNewModel->GetPos();
			switch(nNeighbor) 
			{
			case 1:
				vpos +=A3DVECTOR3(0.0f,0.0f,fSize);break;
			case 3:
				vpos +=A3DVECTOR3(-fSize,0.0f,0.0f);break;
				
			case 5: 
				vpos +=A3DVECTOR3(fSize,0.0f,0.0f);break;
				
			case 7: 
				vpos +=A3DVECTOR3(0.0f,0.0f,-fSize);break;
			}
			pNewModel->SetPos(vpos);
			m_pNeighborModelList.push_back(pNewModel);
		}		
	}else
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			AString szObjectName;
			AString szModelPathName;
			AString szModelHullPath;
			
			int nObjectID;;
			FileToRead.Read(&nObjectID,sizeof(int),&dwWriteLen);
			/*
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;*/
			
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				goto READFAIL;
			}
			A3DVECTOR3 vpos = pNewModel->GetPos();
			switch(nNeighbor) 
			{
			case 1:
				vpos +=A3DVECTOR3(0.0f,0.0f,fSize);break;
			case 3:
				vpos +=A3DVECTOR3(-fSize,0.0f,0.0f);break;
				
			case 5: 
				vpos +=A3DVECTOR3(fSize,0.0f,0.0f);break;
				
			case 7: 
				vpos +=A3DVECTOR3(0.0f,0.0f,-fSize);break;
			}
			pNewModel->SetPos(vpos);
			m_pNeighborModelList.push_back(pNewModel);
		}		
	}
	FileToRead.Close();
	return true;

READFAIL:
	CString msg;
	msg.Format("CElementMap::LoadNeighborModels(), loading %s file failed",strPath);
	g_Log.Log(msg);
	FileToRead.Close();
	g_pA3DConfig->SetFlagNoTextures(false);
	return false;
}

bool CElementMap::ReleaseNeighborModels()
{
	int n = m_pNeighborModelList.size();
	for( int i = 0; i < n; i++)
	{
		m_pNeighborModelList[i]->Release();
		delete m_pNeighborModelList[i];
	}
	m_pNeighborModelList.clear();
	return true;
}

bool CElementMap::RenderNeighborModels(A3DViewport* pA3DViewport)
{
	int n = m_pNeighborModelList.size();
	for( int i = 0; i < n; i++)
	{
		m_pNeighborModelList[i]->Render(pA3DViewport);
	}
	return true;
}

CString CElementMap::GetPlayerPassMapPath(const char *szMapName, int iCurLayer, const char *szExt)
{
	CString strPath;
	strPath.Format("%s%s\\%s\\%s-%d.%s",g_szWorkDir,g_szEditMapDir,szMapName,szMapName,iCurLayer,szExt);
	return strPath;
}

bool CElementMap::SaveCurPlayerPassMap()
{
	if(m_pPlayerPassMapGenerator==0) return true;
	CString strProjPath;
	if(!m_pPlayerPassMapGenerator->GetPassMap())
		return true;
	int iCurLayer = GetPlayerPassMapCurLayer();
	if (iCurLayer < 0){
		return true;
	}
	
	// Test if we have generated the pass map in the directory!
	CBitImage ReachableImage;
	CBlockImage<FIX16> DeltaHImage;	
	m_pPlayerPassMapGenerator->ExportMaps(&ReachableImage,&DeltaHImage);
	
	strProjPath = GetPlayerPassMapPath(GetMapName(), iCurLayer, "prmap");
	FILE* sFile = fopen(strProjPath,"wb");
	if(sFile==NULL)
	{
		g_Log.Log("CElementMap::SavePassMap(), Open file %s failed!",strProjPath);
		return false;
	}
	
	if(!ReachableImage.Save(sFile))
	{
		fclose(sFile);
		g_Log.Log("CElementMap::SavePassMap(), Save file %s failed!",strProjPath);
		return false;
	}
	fclose(sFile);
	
	strProjPath = GetPlayerPassMapPath(GetMapName(), iCurLayer, "pdhmap");
	sFile = fopen(strProjPath,"wb");
	if(sFile==NULL)
	{
		g_Log.Log("CElementMap::SavePassMap(), Open file %s failed!",strProjPath);
		return false;
	}
	
	if(!DeltaHImage.Save(sFile))
	{
		fclose(sFile);
		g_Log.Log("CElementMap::SavePassMap(), Save file %s failed!",strProjPath);
		return false;
	}
	fclose(sFile);
	return true;
}

bool CElementMap::LoadCurPlayerPassMap()
{
	bool bOK(false);
	while (true)
	{
		m_pPlayerPassMapGenerator->Release();

		int iCurLayer = GetPlayerPassMapCurLayer();
		if (iCurLayer < 0){
			break;
		}
		// Test if we have generated the pass map in the directory!
		CString strRMapPath = GetPlayerPassMapPath(GetMapName(), iCurLayer, "prmap");
		CString strDHMapPath = GetPlayerPassMapPath(GetMapName(), iCurLayer, "pdhmap");
		FILE *fRMap=fopen(strRMapPath, "rb");
		FILE *fDHMap=fopen(strDHMapPath, "rb");
		if(!(fRMap && fDHMap)){
			bOK = true;
			break;
		}		
		// 装载可达图，高度图
		CBitImage ReachMap;
		CBlockImage<FIX16> DHMap;
		if(!(ReachMap.Load(fRMap) && DHMap.Load(fDHMap))){
			fclose(fRMap);
			fclose(fDHMap);
			break;
		}		
		fclose(fRMap);
		fclose(fDHMap);		
		if(!m_pPlayerPassMapGenerator->ImportMaps(&ReachMap, &DHMap)){
			break;
		}		
		if(!m_pPlayerPassMapGenerator->GetPassMap())
			g_Log.Log("CElementMap::LoadPlayerPassMap(), Failed to import maps");

		bOK = true;
		break;
	}
	return bOK;
}

int  CElementMap::GetPlayerPassMapCurLayer()
{
	return m_nPlayerPassMapCurLayer;
}

int  CElementMap::GetPlayerPassMapLayerCount()
{
	return 3;
}

void CElementMap::SetPlayerPassMapCurLayer(int iLayer)
{
	if (CanChangePlayerPassMapLayer(iLayer)){
		m_nPlayerPassMapCurLayer = iLayer;
	}
}

bool CElementMap::CanChangePlayerPassMapLayer(int iLayer)
{
	while (iLayer > 0){
		CString  strPath = GetPlayerPassMapPath(GetMapName(), iLayer-1, "prmap");
		if (!AUX_FileIsExist(strPath)){
			return false;
		}
		strPath = GetPlayerPassMapPath(GetMapName(), iLayer-1, "pdhmap");
		if (!AUX_FileIsExist(strPath)){
			return false;
		}
		iLayer --;
	}
	return true;
}