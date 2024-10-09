// SceneObjectManager.cpp: implementation of the CSceneObjectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "global.h"
#include "elementeditor.h"
#include "Render.h"
#include "UndoMan.h"
#include "TerrainWater.h"
#include "MainFrm.h"
#include "a3d.h"
#include "AM.h"

#include "EL_Tree.h"
#include "EL_GrassType.h"
#include "EL_Forest.h"
#include "EL_GrassLand.h"
#include "EL_Archive.h"
#include "EL_Building.h"

#include "TerrainRender.h"
#include "TerrainPlants.h"
#include "A3DTerrain2.h"
#include "A3DTerrain2Blk.h"



#include "EditerBezier.h"
#include "SceneAIGenerator.h"
#include "SceneAudioObject.h"
#include "SceneBoxArea.h"
#include "SceneDummyObject.h"
#include "SceneLightObject.h"
#include "SceneObject.h"
#include "SceneSpeciallyObject.h"
#include "SceneVernier.h"
#include "SceneWaterObject.h"
#include "SceneSkinModel.h"
#include "StaticModelObject.h"
#include "A3DGFXExMan.h"
#include "CritterGroup.h"
#include "ScenePrecinctObject.h"
#include "TerrainObstruct.h"
#include "SceneObjectManager.h"
#include "SceneGatherObject.h"
#include "CloudBox.h"
#include "SceneInstanceBox.h"
#include "SceneDynamicObject.h"


abase::hash_map<unsigned long, PESTATICMODEL> g_TempModelMap;
PESTATICMODEL Aux_GetStaticModelFromTempMap(unsigned long id)
{
	abase::hash_map<unsigned long, PESTATICMODEL>::iterator it = g_TempModelMap.find(id);
	return it == g_TempModelMap.end() ? NULL : it->second;
}



extern int g_nStaticNum;



//#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneObjectManager::CSceneObjectManager(CElementMap* pMap)
{
	m_pMap = pMap;	
	m_pPlants = new CTerrainPlants(this);
	m_pForest = new CELForest();
	m_pGrassLand = new CELGrassLand();
	m_pVernier = new CSceneVernier();
	m_pTerrainObstruct = new CTerrainObstruct();
	m_nObjectIDFlag = 0;
	m_dwVssFileVersion = 0;
	m_pBackSound = NULL;//new AMSoundStream;
	m_pCurrentArea = NULL;
	m_bCamUnderWater = false;
	m_pCurrentPrecinct = NULL;
	m_pForest->Init(g_Render.GetA3DDevice(),g_Render.GetDirLight(),&g_Log);	
	m_pGrassLand->Init(g_Render.GetA3DDevice(),NULL, &g_Log);
	m_pGrassLand->SetForceRebuild(true);
	
	
	A3DSkySphere* pSky = new A3DSkySphere;
	if(pSky)
	{
		AString ext1("Textures\\sky\\Roof.bmp");
		AString ext2("Textures\\sky\\01.bmp");
		AString ext3("Textures\\sky\\02.bmp");
		if (!pSky->Init(g_Render.GetA3DDevice(), NULL, g_szWorkDir + ext1, 
			g_szWorkDir + ext2, g_szWorkDir + ext3))
		{
			g_Log.Log("CSceneObjectManager::CSceneObjectManager,Failed to create sphere sky !");
			delete pSky;
			m_pA3DSky = NULL;
		}else m_pA3DSky = pSky;
	}

	m_BackMusic.Init(g_Render.GetA3DEngine()->GetAMSoundEngine());
}

CSceneObjectManager::~CSceneObjectManager()
{
}

void CSceneObjectManager::AddSceneObject(CSceneObject *pObj)
{
	if(pObj==NULL) return;
	ClearSelectedList();
	m_listSceneObject.AddTail(pObj);
}

void CSceneObjectManager::InsertSceneObject(CSceneObject *pObj)
{
	if(pObj==NULL) return;
	
	switch(pObj->GetObjectType())
	{
	case CSceneObject::SO_TYPE_LIGHT:
		m_listLightObject.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_STATIC_MODEL:
		m_listStaticModelObject.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_WATER:
		m_listWaterObject.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_BEZIER:
		m_listDataBezier.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_AREA:
		m_listDataBoxArea.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_AUDIO:
		m_listAudio.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_DUMMY:
		m_listDummy.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_SPECIALLY:
		m_listSpecially.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_ECMODEL:
		m_listSkinModel.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_AIAREA:
		m_listAIGenerator.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_CRITTER_GROUP:
		m_listCritterGroup.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_FIXED_NPC:
		m_listFixedNpc.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_CLOUD_BOX:
		m_listCloudBox.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_INSTANCE_BOX:
		m_listInstanceBox.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_DYNAMIC:
		m_listDynamic.AddTail(pObj);
		break;
	case CSceneObject::SO_TYPE_GATHER:
		m_listGatherObject.AddTail(pObj);
		break;
	default:
		ASSERT(false);
		break;
	}
	ClearSelectedList();
	m_listSceneObject.AddTail(pObj);
}

void CSceneObjectManager::DeleteSceneObjectForever(CSceneObject *pObj)
{
	if(pObj==NULL) return;
	ClearSelectedList();
	DeleteSceneObject(pObj->GetObjectName());

	//因为所有对象都存在这儿，所以即使场景中可能不存在某些对象
	//但是在做UNDO后，有些对象会被恢复
	ALISTPOSITION pos = m_listLightObject.GetTailPosition();
	ALISTPOSITION oldPos;
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listLightObject.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listWaterObject.GetTailPosition();
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listWaterObject.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listWaterObject.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listStaticModelObject.GetTailPosition();
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listStaticModelObject.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listStaticModelObject.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listDataBezier.GetTailPosition();
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listDataBezier.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listDataBezier.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listDataBoxArea.GetTailPosition();
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listDataBoxArea.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listDataBoxArea.RemoveAt(oldPos);
			return;
		}
	}
	
	pos = m_listAudio.GetTailPosition();
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listAudio.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listAudio.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listDummy.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listDummy.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listDummy.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listSpecially.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listSpecially.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listSpecially.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listAIGenerator.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listAIGenerator.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listAIGenerator.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listSkinModel.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listSkinModel.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listSkinModel.RemoveAt(oldPos);
			return;
		}
	}
	
	pos = m_listCritterGroup.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listCritterGroup.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listCritterGroup.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listFixedNpc.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listFixedNpc.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listFixedNpc.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listPrecinct.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listPrecinct.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listPrecinct.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listGatherObject.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listGatherObject.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listGatherObject.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listRange.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listRange.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listRange.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listCloudBox.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listCloudBox.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listCloudBox.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listInstanceBox.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listInstanceBox.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listInstanceBox.RemoveAt(oldPos);
			return;
		}
	}

	pos = m_listDynamic.GetTailPosition();
	while(pos)
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listDynamic.GetPrev(pos);
		if(ptemp == pObj) 
		{
			m_listDynamic.RemoveAt(oldPos);
			return;
		}
	}


	pObj->Release();
	delete pObj;
}

void CSceneObjectManager::Release()
{
	//**这儿释放所有的表数据
	ALISTPOSITION pos = m_listLightObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listWaterObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listWaterObject.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listStaticModelObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listStaticModelObject.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listDataBezier.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBezier.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listDataBoxArea.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBoxArea.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listAudio.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listAudio.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listDummy.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listDummy.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listSpecially.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listSpecially.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listAIGenerator.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listAIGenerator.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listSkinModel.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listSkinModel.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}
	pos = m_listCritterGroup.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listCritterGroup.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}
	pos = m_listFixedNpc.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listFixedNpc.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}
	pos = m_listPrecinct.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listPrecinct.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}
	pos = m_listGatherObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listGatherObject.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listRange.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listRange.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listCloudBox.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listCloudBox.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listInstanceBox.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listInstanceBox.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}

	pos = m_listDynamic.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listDynamic.GetPrev(pos);
		ptemp->Release();
		delete ptemp;
	}
	
	m_listDynamic.RemoveAll();
	m_listInstanceBox.RemoveAll();
	m_listCloudBox.RemoveAll();
	m_listRange.RemoveAll();
	m_listGatherObject.RemoveAll();
	m_listPrecinct.RemoveAll();
	m_listFixedNpc.RemoveAll();
	m_listCritterGroup.RemoveAll();
	m_listSkinModel.RemoveAll();
	m_listSpecially.RemoveAll();
	m_listAIGenerator.RemoveAll();
	m_listDummy.RemoveAll();
	m_listAudio.RemoveAll();
	m_listDataBoxArea.RemoveAll();
	m_listDataBezier.RemoveAll();
	m_listStaticModelObject.RemoveAll();
	m_listLightObject.RemoveAll();
	m_listWaterObject.RemoveAll();
	m_listSceneObject.RemoveAll();
	m_listSelectedObject.RemoveAll();
	
	g_TempModelMap.clear();

	//**这儿释放所有的单个对象
	g_UndoMan.Reset();

	if(m_pA3DSky)
	{
		g_Render.GetA3DEngine()->SetSky(NULL);
		m_pA3DSky->Release();
		delete m_pA3DSky;
		m_pA3DSky = NULL;
	}

	if(m_pPlants)
	{
		m_pPlants->Release();
		delete m_pPlants;
		m_pPlants = NULL;
	}

	if(m_pGrassLand)
	{
		m_pGrassLand->Release();
		delete m_pGrassLand;
		m_pGrassLand = NULL;
	}

	if(m_pForest)
	{
		m_pForest->Release();
		delete m_pForest;
		m_pForest = NULL;
	}

	if(m_pBackSound)
	{
		m_pBackSound->Stop();
		m_pBackSound->Release();
		delete m_pBackSound;
		m_pBackSound = NULL;
	}

	if(m_pVernier)
	{
		m_pVernier->Release();
		delete m_pVernier;
		m_pVernier = NULL;
	}

	if(m_pTerrainObstruct)
	{
		m_pTerrainObstruct->Release();
		delete m_pTerrainObstruct;
		m_pTerrainObstruct = NULL;
	}
	
}

PSCENELIGHTOBJECT CSceneObjectManager::CreateLight(LIGHTDATA &lightData)
{
	CSceneLightObject *ptemp = new CSceneLightObject();
	ptemp->SetLightData(lightData);
	//这一步，看上去有点多余，但是灯光OJBECT暂时
	//只能如此了，因为属性表中有一次名字，对象有
	//一个名字，所以一但更新，就得两个
	ptemp->SetObjectName(lightData.m_strName);
	if(ptemp)
	{
		ClearSelectedList();
		ptemp->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)ptemp);
		m_listSelectedObject.AddTail((CSceneObject*)ptemp);
		m_listLightObject.AddTail((CSceneObject*)ptemp);
	}
	return ptemp;
}

PSCENEAUIDOOBJECT CSceneObjectManager::CreateAudio(const AString& name,GFX_AUDIO_DATA data)
{
	CSceneAudioObject *ptemp = new CSceneAudioObject();
	ptemp->SetProperty(data);
	ptemp->SetObjectName(name);
	if(ptemp)
	{
		ClearSelectedList();
		ptemp->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)ptemp);
		m_listSelectedObject.AddTail((CSceneObject*)ptemp);
		m_listAudio.AddTail((CSceneObject*)ptemp);
	}
	return ptemp;
}

CCloudBox *CSceneObjectManager::CreateCloudBox(const AString& name)
{
	CCloudBox *ptemp = new CCloudBox();
	ptemp->SetObjectName(name);
	if(ptemp)
	{
		ClearSelectedList();
		ptemp->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)ptemp);
		m_listSelectedObject.AddTail((CSceneObject*)ptemp);
		m_listCloudBox.AddTail((CSceneObject*)ptemp);
	}
	return ptemp;
}

PSCENEWATEROBJECT CSceneObjectManager::CreateWater(DWORD id,const AString& name)
{
	
	//看看水块是否已经存在
	if(m_pMap)
	{
		CTerrainWater *pWater = m_pMap->GetTerrainWater();
		A3DWaterArea * pArea = pWater->GetWaterAreaByID(id);
	
		PSCENEWATEROBJECT ptemp = new CSceneWaterObject(this);
		ptemp->SetObjectID(id);
		ptemp->SetObjectName(name);
		if(pArea==NULL)
			ptemp->CreateRenderWater();
		if(ptemp)
		{
			ClearSelectedList();
			ptemp->SetSelected(true);
			m_listSceneObject.AddTail((CSceneObject*)ptemp);
			m_listSelectedObject.AddTail((CSceneObject*)ptemp);
			m_listWaterObject.AddTail((CSceneObject*)ptemp);
		}
		return ptemp;
	}
		
	return NULL;
}

CSceneDynamicObject*CSceneObjectManager::CreateDynamic(const AString& name)
{
	CSceneDynamicObject *pNew = new CSceneDynamicObject();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listDynamic.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

PEDITERBEZIER  CSceneObjectManager::CreateBezier(const AString& name)
{
	CEditerBezier *ptemp = new CEditerBezier(0,g_Render.GetA3DDevice());
	if(ptemp)
	{
		ClearSelectedList();
		ptemp->SetObjectName(name);
		ptemp->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)ptemp);
		m_listSelectedObject.AddTail((CSceneObject*)ptemp);
		m_listDataBezier.AddTail((CSceneObject*)ptemp);
	}
	return ptemp;
}

PSCENEBOXAREA  CSceneObjectManager::CreateBoxArea(const AString& name,bool bIsDefaut)
{
	CSceneBoxArea *ptemp = new CSceneBoxArea(bIsDefaut);
	if(ptemp)
	{
		ClearSelectedList();
		ptemp->SetObjectName(name);
		ptemp->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)ptemp);
		m_listSelectedObject.AddTail((CSceneObject*)ptemp);
		m_listDataBoxArea.AddTail((CSceneObject*)ptemp);
	}
	return ptemp;
}

CSceneBoxArea* CSceneObjectManager::GetDefaultBoxArea()
{
	ALISTPOSITION pos = m_listDataBoxArea.GetHeadPosition();
	while(pos)
	{
		CSceneBoxArea* ptemp = (CSceneBoxArea*)m_listDataBoxArea.GetNext(pos);
		if(ptemp->IsDefaultArea()) return ptemp;
	}
	return NULL;
}

CSceneDummyObject*  CSceneObjectManager::CreateDummy(const AString& name)
{
	CSceneDummyObject *pDummy = new CSceneDummyObject();
	if(pDummy)
	{
		pDummy->SetObjectName(name);
		pDummy->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pDummy);
		m_listSelectedObject.AddTail((CSceneObject*)pDummy);
		m_listDummy.AddTail((CSceneObject*)pDummy);
		return pDummy;
	}
	return NULL;
}

CSceneSpeciallyObject*  CSceneObjectManager::CreateSpecially(const AString& name)
{
	CSceneSpeciallyObject *pNew = new CSceneSpeciallyObject();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listSpecially.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CSceneAIGenerator*  CSceneObjectManager::CreateAIGenerator(const AString& name)
{
	CSceneAIGenerator *pNew = new CSceneAIGenerator();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listAIGenerator.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CSceneGatherObject *CSceneObjectManager::CreateGather(const AString& name)
{
	CSceneGatherObject *pNew = new CSceneGatherObject();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listGatherObject.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CScenePrecinctObject*  CSceneObjectManager::CreatePrecinct(const AString& name)
{
	CScenePrecinctObject *pNew = new CScenePrecinctObject();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listPrecinct.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CSceneRangeObject*  CSceneObjectManager::CreateRange(const AString& name)
{
	CSceneRangeObject *pNew = new CSceneRangeObject();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listRange.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CSceneFixedNpcGenerator* CSceneObjectManager::CreateFixedNpc(const AString& name)
{
	CSceneFixedNpcGenerator *pNew = new CSceneFixedNpcGenerator();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listFixedNpc.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

SceneInstanceBox* CSceneObjectManager::CreateInstanceBox(const AString& name)
{
	SceneInstanceBox *pNew = new SceneInstanceBox();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listInstanceBox.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

CCritterGroup* CSceneObjectManager::CreateCritterGroup(const AString& name)
{
	CCritterGroup *pNew = new CCritterGroup();
	if(pNew)
	{
		pNew->SetObjectName(name);
		pNew->SetSelected(true);
		m_listSceneObject.AddTail((CSceneObject*)pNew);
		m_listSelectedObject.AddTail((CSceneObject*)pNew);
		m_listCritterGroup.AddTail((CSceneObject*)pNew);
		return pNew;
	}
	return NULL;
}

PSCENEOBJECT CSceneObjectManager::FindLightObject(const AString& name)
{
	ALISTPOSITION pos = m_listLightObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}
	return NULL;
}


bool CSceneObjectManager::IsBuildingObjectInScene(const AString& name)
{
	ALISTPOSITION pos = m_listStaticModelObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listStaticModelObject.GetNext(pos);
		if(0 == strcmp(ptemp->GetObjectName(),name)) 
		{
			if(ptemp->IsDeleted()) return false;
			else return true;
		}
	}
	return false;
}

PSCENEOBJECT CSceneObjectManager::FindSceneObject(const AString& name)
{

	//因为所有对象都存在这儿，所以即使场景中可能不存在某些对象
	//但是在做UNDO后，有些对象会被恢复
	ALISTPOSITION pos = m_listLightObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listWaterObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listWaterObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listStaticModelObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listStaticModelObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listDataBezier.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listDataBezier.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listDataBoxArea.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listDataBoxArea.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listAudio.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listAudio.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listDummy.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listDummy.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listSpecially.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSpecially.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listAIGenerator.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listAIGenerator.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}
	
	pos = m_listSkinModel.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSkinModel.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listCritterGroup.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listCritterGroup.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listFixedNpc.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listFixedNpc.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listPrecinct.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listPrecinct.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listGatherObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listGatherObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listRange.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listRange.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listCloudBox.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listCloudBox.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listInstanceBox.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listInstanceBox.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	pos = m_listDynamic.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listDynamic.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			return ptemp;
		}
	}

	return NULL;
}

void CSceneObjectManager::AddObjectPtrToSelected(PSCENEOBJECT pObj)
{
	if(!pObj->IsSelected())
	{
		pObj->SetSelected();
		m_listSelectedObject.AddTail(pObj);
	}
}

void CSceneObjectManager::ClearSelectedList()
{
	ALISTPOSITION pos = m_listSelectedObject.GetTailPosition();
	
	while( pos )
	{
		CSceneObject* ptemp = m_listSelectedObject.GetPrev(pos);
		ptemp->SetSelected(false);
	}
	m_listSelectedObject.RemoveAll();
}

bool CSceneObjectManager::RayTrace(A3DVECTOR3& vStart, A3DVECTOR3& vEnd,bool bAdd)
{
	float fMinDistance = 999999.0f;
	PSCENEOBJECT pSelected = NULL;
	A3DVECTOR3 lineMiddle,lineVect,maxEdge,minEdge;
	lineVect = vEnd - vStart;
	lineMiddle = (vEnd + vStart)*0.5f;
	//float halfLength = lineVect.Magnitude() * 0.5f;
	float vx,vy,vz,halfLength;
	vx = vEnd.x -vStart.x; vy = vEnd.y -vStart.y; vz = vEnd.z -vStart.z;
	halfLength = sqrt(vx*vx + vy*vy + vz*vz);
	
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		PSCENEOBJECT ptemp = m_listSceneObject.GetPrev(pos);
		
		if(!IsPickObject(ptemp)) continue;	
		
		if(ptemp->GetObjectType() != CSceneObject::SO_TYPE_BEZIER)
		{
			maxEdge = ptemp->GetMax();
			minEdge = ptemp->GetMin();
			
			if(intersectsBoxWithLine(lineMiddle,lineVect,halfLength,maxEdge,minEdge))
			{
				A3DVECTOR3 vPos = ((g_Render.GetA3DEngine())->GetActiveCamera())->GetPos();
				
				
				if(bAdd)
				{
					if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_STATIC_MODEL)
					{
						A3DVECTOR3 inter;
						if(((CStaticModelObject *)ptemp)->RayTrace(vStart,vEnd,inter))
							vPos = vPos - inter;
						else continue;
					}else
					{
						vPos = vPos - ptemp->GetPos();
					}
				}else vPos = vPos - ptemp->GetPos();
				
				float ftemp = vPos.Magnitude();
				if(fMinDistance> ftemp)
				{
					fMinDistance = ftemp;
					pSelected = ptemp;
				}
			}
		}else
		{//对b曲线的特殊处理
			if(((CEditerBezier*)ptemp)->RayTrace(vStart,vEnd,bAdd))
			{
				pSelected = ptemp;
				break;
			}
		}
		
	}
	
	//把选中的点加入到选择表中
	if(pSelected)
	{
		if(bAdd)
		{
			AddObjectPtrToSelected(pSelected);
		}
		return true;
	}
	return false;
}

bool CSceneObjectManager::GetTracePos(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& inter)
{
	float fMinDistance = 999999.0f;
	PSCENEOBJECT pSelected = NULL;
	A3DVECTOR3 lineMiddle,lineVect,maxEdge,minEdge,vTracePos;
	lineVect = vEnd - vStart;
	lineMiddle = (vEnd + vStart)*0.5f;
	//float halfLength = lineVect.Magnitude() * 0.5f;
	float vx,vy,vz,halfLength;
	bool bTraced = false;
	vx = vEnd.x -vStart.x; vy = vEnd.y -vStart.y; vz = vEnd.z -vStart.z;
	halfLength = sqrt(vx*vx + vy*vy + vz*vz);
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		PSCENEOBJECT ptemp = m_listSceneObject.GetPrev(pos);
		
		if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			maxEdge = ptemp->GetMax();
			minEdge = ptemp->GetMin();
			
			if(intersectsBoxWithLine(lineMiddle,lineVect,halfLength,maxEdge,minEdge))
			{
				A3DVECTOR3 vPos = ((g_Render.GetA3DEngine())->GetActiveCamera())->GetPos();
				if(((CStaticModelObject *)ptemp)->RayTrace(vStart,vEnd,inter))
				{
					vPos = vPos - inter;
					float ftemp = vPos.Magnitude();
					if(fMinDistance> ftemp)
					{
						fMinDistance = ftemp;
						pSelected = ptemp;
						bTraced = true;
						vTracePos = inter;
					}
				}
			}
		}
	}
	A3DVECTOR3 vtemp = ((g_Render.GetA3DEngine())->GetActiveCamera())->GetPos();
	RAYTRACERT TraceRt;	//	Used to store trace result
	if(m_pMap->GetTerrain()->RayTrace(vStart,vEnd - vStart,1.0f,&TraceRt))
	{
		vtemp = vtemp - TraceRt.vPoint;
		float dis = vtemp.Magnitude();
		if(dis < fMinDistance)
			vTracePos = TraceRt.vPoint;	
		bTraced = true;
	}
	inter = vTracePos;
	return bTraced;
}

bool CSceneObjectManager::IsNonNPCPath(CSceneObject *pObj)
{
	//	对象在 SO_TYPE_PRECINCT 或者 SO_TYPE_RANGE 中引用时，返回 true
	if (pObj && pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER){
		CEditerBezier *p = (CEditerBezier *)(pObj);
		int pathID = p->GetObjectID();

		ALISTPOSITION pos = m_listPrecinct.GetTailPosition();
		while( pos ){
			CScenePrecinctObject* ptemp = (CScenePrecinctObject *)(m_listPrecinct.GetPrev(pos));
			if (pathID == ptemp->GetProperty().nPath){
				return true;
			}
		}
		pos = m_listRange.GetTailPosition();
		while( pos ){
			CSceneRangeObject* ptemp = (CSceneRangeObject *)(m_listRange.GetPrev(pos));
			if (pathID == ptemp->GetBezierID()){
				return true;
			}
		}
	}
	return false;
}

bool CSceneObjectManager::IsShowPath(CSceneObject *pObj)
{
	bool bShow(false);
	if (pObj && pObj->GetObjectType() == CSceneObject::SO_TYPE_BEZIER){
		if (IsNonNPCPath(pObj)){
			bShow = g_Configs.bShowNonNPCPath;
		}else{
			bShow = g_Configs.bShowNPCPath;
		}
	}
	return bShow;
}

bool CSceneObjectManager::IsPickObject(CSceneObject *pObj)
{
	switch(pObj->GetObjectType())
	{
	case CSceneObject::SO_TYPE_AREA:
		if(g_Configs.bPickBoxArea) return true;
		else return false;
	case CSceneObject::SO_TYPE_AUDIO:
		if(g_Configs.bPickAudio) return true;
		else return false;
	case CSceneObject::SO_TYPE_BEZIER:
		if(IsNonNPCPath(pObj)) return g_Configs.bPickNonNPCPath;
		else return g_Configs.bPickNPCPath;
	case CSceneObject::SO_TYPE_DUMMY:
		if(g_Configs.bPickDummy) return true;
		else return false;
	case CSceneObject::SO_TYPE_LIGHT:
		if(g_Configs.bPickLight) return true;
		else return false;
	case CSceneObject::SO_TYPE_STATIC_MODEL:
		if(g_Configs.bPickStaticModel) return true;
		else return false;
	case CSceneObject::SO_TYPE_WATER:
		if(g_Configs.bPickWater) return true;
		else return false;
	case CSceneObject::SO_TYPE_SPECIALLY:
		if(g_Configs.bPickSpecially) return true;
		else return false;
	case CSceneObject::SO_TYPE_AIAREA:
		if(g_Configs.bPickAIArea && (((CSceneAIGenerator*)pObj)->GetCopyFlags()==g_Configs.nShowCopyNum)) return true;
		else return false;
	case CSceneObject::SO_TYPE_ECMODEL:
		if(g_Configs.bPickSkinModel) return true;
		else return false;
	case CSceneObject::SO_TYPE_CRITTER_GROUP:
		if(g_Configs.bPickCritterGroup) return true;
		else return false;
	case CSceneObject::SO_TYPE_FIXED_NPC:
		if(g_Configs.bPickFixedNpc && (((CSceneFixedNpcGenerator*)pObj)->GetCopyFlags()==g_Configs.nShowCopyNum)) return true;
		else return false;
	case CSceneObject::SO_TYPE_GATHER:
		if(g_Configs.bPickGather && (((CSceneGatherObject*)pObj)->GetCopyFlags()==g_Configs.nShowCopyNum)) return true;
		else return false;
	case CSceneObject::SO_TYPE_PRECINCT:
		if(g_Configs.bPickPrecinct) return true;
		else return false;
	case CSceneObject::SO_TYPE_RANGE:
		return g_Configs.bPickRange;
	case CSceneObject::SO_TYPE_CLOUD_BOX:
		if(g_Configs.bPickCloudBox) return true;
		else return false;
	case CSceneObject::SO_TYPE_INSTANCE_BOX:
		if(g_Configs.bPickInstanceBox) return true;
		else return false;
	case CSceneObject::SO_TYPE_DYNAMIC:
		return g_Configs.bPickDynamic;
	}
	return true;
}

void CSceneObjectManager::Tick(DWORD dwTimeDelta)
{
	
	if(!g_bShowSelectedOnly)
	{
	ALISTPOSITION  pos = m_listWaterObject.GetTailPosition();
	if(g_Configs.bShowWater)
	while( pos )
	{
		CSceneObject* ptemp = m_listWaterObject.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listDataBoxArea.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBoxArea.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	
	pos = m_listLightObject.GetTailPosition();
	if(g_Configs.bShowLight)
	while( pos )
	{
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	
	pos = m_listStaticModelObject.GetTailPosition();
	if(g_Configs.bShowStaticModel)
	while(pos)
	{
		CSceneObject* ptemp = m_listStaticModelObject.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listDataBezier.GetTailPosition();
	if(g_Configs.bShowNPCPath || g_Configs.bShowNonNPCPath)
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBezier.GetPrev(pos);
		if(!ptemp->IsDeleted() && IsShowPath(ptemp)){
			ptemp->Tick(dwTimeDelta);
		}
	}

	pos = m_listAudio.GetTailPosition();
	if(g_Configs.bShowAudio)
	while(pos)
	{
		CSceneObject* ptemp = m_listAudio.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listDummy.GetTailPosition();
	if(g_Configs.bShowDummy)
	while(pos)
	{
		CSceneObject* ptemp = m_listDummy.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	
	pos = m_listSpecially.GetTailPosition();
	if(g_Configs.bShowSpecially)
	while(pos)
	{
		CSceneObject* ptemp = m_listSpecially.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listAIGenerator.GetTailPosition();
	if(g_Configs.bShowAIArea)
	while(pos)
	{
		CSceneObject* ptemp = m_listAIGenerator.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listSkinModel.GetTailPosition();
	if(g_Configs.bShowSkinModel)
	while(pos)
	{
		CSceneObject* ptemp = m_listSkinModel.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	pos = m_listCritterGroup.GetTailPosition();
	if(g_Configs.bShowCritterGroup)
	while(pos)
	{
		CSceneObject* ptemp = m_listCritterGroup.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	pos = m_listFixedNpc.GetTailPosition();
	if(g_Configs.bShowFixedNpc)
	while(pos)
	{
		CSceneObject* ptemp = m_listFixedNpc.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	pos = m_listPrecinct.GetTailPosition();
	if(g_Configs.bPickPrecinct)
	while(pos)
	{
		CSceneObject* ptemp = m_listPrecinct.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	pos = m_listGatherObject.GetTailPosition();
	if(g_Configs.bShowGather)
	while(pos)
	{
		CSceneObject* ptemp = m_listGatherObject.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listRange.GetTailPosition();
	if(g_Configs.bShowRange)
	while(pos)
	{
		CSceneObject* ptemp = m_listRange.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listCloudBox.GetTailPosition();
	if(g_Configs.bShowCloudBox)
	while(pos)
	{
		CSceneObject* ptemp = m_listCloudBox.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	pos = m_listInstanceBox.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listInstanceBox.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}
	
	pos = m_listDynamic.GetTailPosition();
	if(g_Configs.bShowDynamic)
	while(pos)
	{
		CSceneObject* ptemp = m_listDynamic.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Tick(dwTimeDelta);
	}

	}else
	{
		ALISTPOSITION pos = m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSelectedObject.GetPrev(pos);
			ptemp->Tick(dwTimeDelta);
		}
	}

	if(g_Configs.bShowTree)
		m_pForest->Update(dwTimeDelta);
	if(g_Configs.bShowGrass)
	{
		m_pGrassLand->SetA3DTerrain(GetMap()->GetTerrain()->GetRender());
		m_pGrassLand->Update(g_Render.GetA3DEngine()->GetActiveCamera()->GetPos(),dwTimeDelta);
	}
	
	m_BackMusic.Update(dwTimeDelta);
	m_pA3DSky->TickAnimation();
	g_Render.GetA3DEngine()->GetActiveCamera()->UpdateEar();
}

void CSceneObjectManager::RenderSky(A3DViewport* pA3DViewport)
{
	//	Render sky first
	if (m_pA3DSky && g_Configs.bEnableSky)
	{
		m_pA3DSky->SetCamera((A3DCamera*)g_Render.GetA3DEngine()->GetActiveCamera());
		m_pA3DSky->Render();
	}
}

void CSceneObjectManager::RenderObjects(A3DViewport* pA3DViewport)
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	if(!g_bShowSelectedOnly)
	{
	ALISTPOSITION pos = m_listLightObject.GetTailPosition();
	if(g_Configs.bShowLight)
	while( pos )
	{
		CSceneObject* ptemp = m_listLightObject.GetPrev(pos);
		if(!ptemp->IsDeleted()) ptemp->Render(pA3DViewport);
	}

	pos = m_listDataBoxArea.GetTailPosition();
	if(g_Configs.bShowBoxArea)
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBoxArea.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	
	pos = m_listWaterObject.GetTailPosition();
	if(g_Configs.bShowWater)
	while( pos )
	{
		CSceneObject* ptemp = m_listWaterObject.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	
	pos = m_listStaticModelObject.GetTailPosition();
	while(pos)
	{
		CSceneObject* ptemp = m_listStaticModelObject.GetPrev(pos);
		if(!ptemp->IsDeleted())
		{
			if(((CStaticModelObject*)ptemp)->IsTree())
			{
				if(g_Configs.bShowTree) ptemp->Render(pA3DViewport);
			}else if(g_Configs.bShowStaticModel) ptemp->Render(pA3DViewport);
		}
	}

	pos = m_listDataBezier.GetTailPosition();
	if(g_Configs.bShowNPCPath || g_Configs.bShowNonNPCPath)
	while(pos)
	{
		CSceneObject* ptemp = m_listDataBezier.GetPrev(pos);
		if(!ptemp->IsDeleted() && IsShowPath(ptemp)){
			ptemp->Render(pA3DViewport);
		}
	}

	pos = m_listAudio.GetTailPosition();
	if(g_Configs.bShowAudio)
	while(pos)
	{
		CSceneObject* ptemp = m_listAudio.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	pos = m_listDummy.GetTailPosition();
	if(g_Configs.bShowDummy)
	while(pos)
	{
		CSceneObject* ptemp = m_listDummy.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	pos = m_listCritterGroup.GetTailPosition();
	if(g_Configs.bShowCritterGroup)
	while(pos)
	{
		CSceneObject* ptemp = m_listCritterGroup.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	pos = m_listFixedNpc.GetTailPosition();
	if(g_Configs.bShowFixedNpc)
	while(pos)
	{
		CSceneObject* ptemp = m_listFixedNpc.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	pos = m_listPrecinct.GetTailPosition();
	if(g_Configs.bPickPrecinct)
	while(pos)
	{
		CSceneObject* ptemp = m_listPrecinct.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	pos = m_listGatherObject.GetTailPosition();
	if(g_Configs.bShowGather)
	while(pos)
	{
		CSceneObject* ptemp = m_listGatherObject.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	pos = m_listRange.GetTailPosition();
	if(g_Configs.bShowRange)
	while(pos)
	{
		CSceneObject* ptemp = m_listRange.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	pos = m_listSkinModel.GetTailPosition();
	if(g_Configs.bShowSkinModel)
	while(pos)
	{
		CSceneObject* ptemp = m_listSkinModel.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	pos = m_listCloudBox.GetTailPosition();
	if(g_Configs.bShowCloudBox)
	while(pos)
	{
		CSceneObject* ptemp = m_listCloudBox.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
	pos = m_listInstanceBox.GetTailPosition();
	if(g_Configs.bShowInstanceBox)
	while(pos)
	{
		CSceneObject* ptemp = m_listInstanceBox.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	pos = m_listDynamic.GetTailPosition();
	if(g_Configs.bShowDynamic)
	while(pos)
	{
		CSceneObject* ptemp = m_listDynamic.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}
	
	pos = m_listAIGenerator.GetTailPosition();
	if(g_Configs.bShowAIArea)
	while(pos)
	{
		CSceneObject* ptemp = m_listAIGenerator.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	pos = m_listSpecially.GetTailPosition();
	if(g_Configs.bShowSpecially)
	while(pos)
	{
		CSceneObject* ptemp = m_listSpecially.GetPrev(pos);
		if(!ptemp->IsDeleted())ptemp->Render(pA3DViewport);
	}

	//渲染植物
	if(g_Configs.bShowTree)
	{
		if(m_pPlants) m_pPlants->Render(pA3DViewport);		
		if(m_pForest) m_pForest->Render(pA3DViewport);
	}

	//渲染坐标尺
	if(g_Configs.bShowVernier)
		m_pVernier->Render(pA3DViewport);
	if(g_Configs.bShowTerrainBlock)
		DrawTerrainBlockRect();
	}else
	{
		ALISTPOSITION pos = m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSelectedObject.GetPrev(pos);
			ptemp->Render(pA3DViewport);
		}
	}
	
#ifdef _EDEBUG
	//debug
	AString str;
	str.Format("Face of tree count  = %d ",m_pForest->GetTreeCount());
	g_Render.TextOut(8,20,str,A3DCOLORRGB(0,255,255));
	str.Format("Face of grass count = %d ",m_pGrassLand->GetGrassCount());
	g_Render.TextOut(8,30,str,A3DCOLORRGB(0,255,0));
#endif
}

void CSceneObjectManager::DrawTerrainBlockRect()
{
	if(g_nCurrentTerrainBlock==-1) return;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;
	
	CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
	if(pTR==NULL) return;
	A3DAABB aabb,sub;
	int h,c,n;
	A3DTerrain2Block* block = NULL; 
	if(g_Configs.bShowSmallBlock)
	{
		h = g_nCurrentTerrainBlock / pTR->GetBlockColNum();
		c = g_nCurrentTerrainBlock % pTR->GetBlockColNum();
		block = pTR->GetBlock(h,c);
		if(block==NULL) return;
		aabb = block->GetBlockAABB();
	}else
	{
		h = g_nCurrentTerrainBlock / (pTR->GetBlockColNum()/4);
		c = g_nCurrentTerrainBlock % (pTR->GetBlockColNum()/4);
		block = pTR->GetBlock(h*4,c*4);
		if(block==NULL) return;
		aabb = block->GetBlockAABB();
		for(n = 0; n < 4; n++)
		{
			block = pTR->GetBlock(h*4 + n,c*4 + 0);
			if(block==NULL) continue;
			sub = block->GetBlockAABB();
			aabb.Merge(sub);
			
			block = pTR->GetBlock(h*4 + n,c*4 + 1);
			if(block==NULL) continue;
			sub = block->GetBlockAABB();
			aabb.Merge(sub);
			
			block = pTR->GetBlock(h*4 + n,c*4 + 2);
			if(block==NULL) continue;
			sub = block->GetBlockAABB();
			aabb.Merge(sub);
			
			block = pTR->GetBlock(h*4 + n,c*4 + 3);
			if(block==NULL) continue;
			sub = block->GetBlockAABB();
			aabb.Merge(sub);
		}
	}
	
	
	float fCenterX = aabb.Center.x;
	float fCenterY =aabb.Center.z;

	int lseg = (int)((aabb.Maxs.x - aabb.Mins.x)/STEP_LEN);
	int wseg = (int)((aabb.Maxs.z - aabb.Mins.z)/STEP_LEN);

	lseg += 2;
	wseg += 2;

	float flRadius = (aabb.Maxs.x - aabb.Mins.x)/2.0f;
	float fwRadius = (aabb.Maxs.z - aabb.Mins.z)/2.0f;
	float offset_y = 0.1f;

	A3DVECTOR3 vLeftBottom = A3DVECTOR3(fCenterX - flRadius ,0, fCenterY - fwRadius);
	A3DVECTOR3 vRightBottom = A3DVECTOR3(fCenterX + flRadius ,0, fCenterY -  fwRadius);
	A3DVECTOR3 vRightUp = A3DVECTOR3(fCenterX + flRadius ,0,fCenterY + fwRadius);
	A3DVECTOR3 vLeftUp = A3DVECTOR3(fCenterX - flRadius,0,fCenterY + fwRadius);
	
	A3DVECTOR3* lvertices = new A3DVECTOR3[lseg];
	A3DVECTOR3* wvertices = new A3DVECTOR3[wseg];
	DWORD clr = A3DCOLORRGB(0,255,0);
	
	float temp;
	for(n = 0 ; n<lseg; n++)
	{
		temp = (float)n/(float)(lseg-1);
		lvertices[n] = vLeftBottom + (vRightBottom - vLeftBottom)*temp;
		lvertices[n].y = pMap->GetTerrain()->GetPosHeight(lvertices[n]) + offset_y;//0.1f
	}
	DrawLine(lvertices,lseg,clr);

	for(n = 0 ; n<lseg; n++)
	{
		temp = (float)n/(float)(lseg-1);
		lvertices[n] = vLeftUp + (vRightUp - vLeftUp)*temp;
		lvertices[n].y = pMap->GetTerrain()->GetPosHeight(lvertices[n]) + offset_y;//0.1f
	}
	DrawLine(lvertices,lseg,clr);

	for(n = 0 ; n < wseg; n++)
	{
		temp = (float)n/(float)(wseg-1);
		wvertices[n] = vRightUp + (vRightBottom - vRightUp)*temp;
		wvertices[n].y = pMap->GetTerrain()->GetPosHeight(wvertices[n]) + offset_y;//0.1f
	}
	DrawLine(wvertices,wseg,clr);

	for(n = 0 ; n < wseg; n++)
	{
		temp = (float)n/(float)(wseg-1);
		wvertices[n] = vLeftUp + (vLeftBottom - vLeftUp)*temp;
		wvertices[n].y = pMap->GetTerrain()->GetPosHeight(wvertices[n]) + offset_y;//0.1f
	}
	DrawLine(wvertices,wseg,clr);

	delete []lvertices;
	delete []wvertices;
}

void CSceneObjectManager::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	
	WORD* pIndices = new WORD[(dwNum - 1)*2];
	int n = 0;
	for(int i=0; i<(int)dwNum - 1; i++)
	{
		pIndices[n] = i;
		pIndices[n+1] = i+1;
		n += 2;
	}
	
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,dwNum,pIndices,(dwNum-1)*2,clr);
	}
	delete []pIndices;
}

void CSceneObjectManager::RenderReflectObjects(A3DViewport* pA3DViewport)
{
	//绘制全部的需要反射的对象
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_Render.GetA3DDevice()->SetAlphaRef(84);
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if( type == CSceneObject::SO_TYPE_STATIC_MODEL )
		{
			CStaticModelObject * pStaticModel = (CStaticModelObject *) ptemp;
			if (pStaticModel->GetReflect() )
			{
				pStaticModel->Render(pA3DViewport);
			}
		}
	}
	g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);

	m_pForest->Render(pA3DViewport);
}

void CSceneObjectManager::RenderRefractObjects(A3DViewport* pA3DViewport)
{
	//绘制全部的需要反射的对象
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
	g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_Render.GetA3DDevice()->SetAlphaRef(84);
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if( type == CSceneObject::SO_TYPE_STATIC_MODEL )
		{
			CStaticModelObject * pStaticModel = (CStaticModelObject *) ptemp;
			if (pStaticModel->GetRefract() )
			{
				pStaticModel->Render(pA3DViewport);
			}
		}
	}
	g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
	g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);

	m_pForest->Render(pA3DViewport);
}


void CSceneObjectManager::RenderForLightMapShadow(A3DViewport* pA3DViewport)
{
	//绘制全体对象
	if(!g_bOnlyCalMiniMap)
	{
		g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
		g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
		g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
		g_Render.GetA3DDevice()->SetAlphaRef(84);
		ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
			ptemp->RenderForLightMapShadow(pA3DViewport);
		}
		g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
		g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
		g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
		
		//渲染植物
		if(m_pPlants) m_pForest->Render(pA3DViewport);
	}
}

void CSceneObjectManager::RenderForLight(A3DViewport* pA3DViewport,float fOffsetX,float fOffsetZ)
{
	if(!g_bOnlyCalMiniMap)
	{
		//绘制全体对象
		g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
		g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
		g_Render.GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
		g_Render.GetA3DDevice()->SetAlphaRef(84);
		ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
			ptemp->RenderForLight(pA3DViewport,fOffsetX,fOffsetZ);
		}
		g_Render.GetA3DEngine()->FlushCachedAlphaMesh(pA3DViewport);
		g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
		g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
		
		//渲染植物(offset?)
		CELForest *pForest = m_pForest;
		int nPlants = m_pPlants->GetPlantNum();
		for(int n = 0; n < nPlants; n++)
		{
			PPLANT pPlant = m_pPlants->GetPlantByIndex(n);
			if(pPlant->GetPlantType() == PLANT_TREE)
			{
				int nTree = ((TREE_PLANT*)pPlant)->GetPlantNum();
				for(int k = 0; k < nTree; k++)
				{
					PPLANTPOS ptemp = ((TREE_PLANT*)pPlant)->GetPlantByIndex(k);
					CELTree *pTree = pForest->GetTreeTypeByID(((TREE_PLANT*)pPlant)->nID);
					if(pTree) pTree->AdjustTreePosition(ptemp->id,A3DVECTOR3(ptemp->x + fOffsetX,ptemp->y,ptemp->z + fOffsetZ));
				}
			}
		}
		
		m_pForest->Render(pA3DViewport);
	}
}

bool CSceneObjectManager::OnMouseMove(int x, int y, DWORD dwFlags)
{
	/*
	ALISTPOSITION pos = m_listSelectedObject.GetTailPosition();
	while( pos )
	{
		PLIGHT ptemp = m_listSelectedObject.GetPrev(pos);
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart =pCamera->GetPos();
		pViewport->InvTransform(vPos,vPos);
		ptemp->m_CoordinateDirection.RayTrace(vStart,vPos);
	}*/
	return true;
}

void CSceneObjectManager::SelectArea(ARectI &rc)
{
	//交换RC，使其左上角点小，同时根据视口剪切
	int temp;
	ARect<DWORD> tempRc;
	tempRc.left = rc.left;
	tempRc.right = rc.right;
	tempRc.bottom = rc.bottom;
	tempRc.top = rc.top;
	if(tempRc.left>tempRc.right) 
	{
		temp = tempRc.right;
		tempRc.right = tempRc.left;
		tempRc.left = temp;
	}
	if(tempRc.top>tempRc.bottom) 
	{
		temp = tempRc.bottom;
		tempRc.bottom = tempRc.top;
		tempRc.top = temp;
	}

	A3DVIEWPORTPARAM* vp = g_Render.GetA3DEngine()->GetActiveViewport()->GetParam();
	if(tempRc.left<0) tempRc.left = 0;
	if(tempRc.left>vp->Width) tempRc.left = vp->Width;
	if(tempRc.right<0) tempRc.right = 0;
	if(tempRc.right>vp->Width) tempRc.right = vp->Width;
	if(tempRc.top<0) tempRc.top = 0;
	if(tempRc.top>vp->Height) tempRc.top = vp->Height;
	if(tempRc.bottom<0) tempRc.bottom = 0;
	if(tempRc.bottom>vp->Height) tempRc.bottom = vp->Height;

	A3DVECTOR3 vPos,vScreenPos;
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		PSCENEOBJECT ptemp = m_listSceneObject.GetPrev(pos);
		if(!IsPickObject(ptemp)) continue;
		vPos = ptemp->GetPos();
		if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(vPos))
		{
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPos, vScreenPos);
			if(vScreenPos.x>tempRc.left && vScreenPos.x<tempRc.right && vScreenPos.y>tempRc.top && vScreenPos.y<tempRc.bottom)
			{
				AddObjectPtrToSelected(ptemp);
			}
		}
	}
}

void CSceneObjectManager::DeleteSceneObject(const AString& name)
{
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	ALISTPOSITION oldPos;
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		if(0==strcmp(ptemp->GetObjectName(),name))
		{
			//对水做特殊处理
			if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_WATER)
			{
				((CSceneWaterObject*)ptemp)->ReleaseRenderWater();
			}
			m_listSceneObject.RemoveAt(oldPos);
			if (m_pMap) m_pMap->SetModifiedFlag(true);
			break;
		}
	}
}

void CSceneObjectManager::DeleteSceneObject(CSceneObject *pObj)
{
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	ALISTPOSITION oldPos;
	while( pos )
	{
		oldPos = pos;
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		if(pObj==ptemp)
		{
			//对水做特殊处理
			if(ptemp->GetObjectType() == CSceneObject::SO_TYPE_WATER)
			{
				((CSceneWaterObject*)ptemp)->ReleaseRenderWater();
			}
			m_listSceneObject.RemoveAt(oldPos);
			if(m_pMap) m_pMap->SetModifiedFlag(true);
			break;
		}
	}
}

bool  CSceneObjectManager::SaveAllObject(CELArchive& ar)
{
	//写文件版本号
	DWORD dwFileVersion = FS_VERSION;
	ar.Write(&dwFileVersion,sizeof(DWORD));
	ar.Write(&m_dwVssFileVersion,sizeof(DWORD));
	ar.Write(&m_nObjectIDFlag,sizeof(int));
	//先保存所有其他对象
	DWORD dwNum = m_listSceneObject.GetCount();
	ar.Write(&dwNum,sizeof(DWORD));
	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetNext(pos);
		int type = ptemp->GetObjectType();
			
		//if(type!=CSceneObject::SO_TYPE_STATIC_MODEL)
		//{//4.0版本以后不保存静态模型
		//FS_VERSION>0x3d后恢复存储

			ar.Write(&type,sizeof(int));
			ptemp->Save(ar,dwFileVersion);
		//}
	}

	//保存植被
	m_pPlants->SavePlants(ar,dwFileVersion);
	return true;
}

bool CSceneObjectManager::ExpAiData(AFile *pFile)
{
	
	DWORD rs;
	//写文件版本号(0xa以后)
	DWORD dwFileVersion = FS_VERSION;
	pFile->Write(&dwFileVersion,sizeof(DWORD),&rs);

	//先保存所有其他对象
	DWORD dwNum = GetSortObjectNum(CSceneObject::SO_TYPE_AIAREA);
	dwNum = dwNum + GetSortObjectNum(CSceneObject::SO_TYPE_FIXED_NPC);
	pFile->Write(&dwNum,sizeof(DWORD),&rs);
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_AIAREA)
		{			
			pFile->Write(&type,sizeof(int),&rs);
			CSceneAIGenerator* pAI = (CSceneAIGenerator*)ptemp;
			pAI->ExpData(pFile,dwFileVersion);
		}else if(type == CSceneObject::SO_TYPE_FIXED_NPC)
		{
			pFile->Write(&type,sizeof(int),&rs);
			CSceneFixedNpcGenerator *pNpc = (CSceneFixedNpcGenerator*)ptemp;
			pNpc->ExpData(pFile,dwFileVersion);
		}
	}
	
	return true;
}

bool CSceneObjectManager::ImpAiData(AFile *pFile)
{
	DWORD rs;
	//写文件版本号(0xa以后)
	DWORD dwFileVersion;
	pFile->Read(&dwFileVersion,sizeof(DWORD),&rs);
	if(dwFileVersion<0xa) return false;
	//先保存所有其他对象
	DWORD dwNum;
	pFile->Read(&dwNum,sizeof(DWORD),&rs);
	int type;
	for(int i = 0; i < (int)dwNum; i++)
	{
		pFile->Read(&type,sizeof(int),&rs);
		if(type == CSceneObject::SO_TYPE_AIAREA)
		{
			CSceneAIGenerator* pnew = new CSceneAIGenerator();
			pnew->ImpData(pFile,dwFileVersion);
			AString name = pnew->GetObjectName();
			if(FindSceneObject(name)!=NULL)
			{
				int AiNum = 0;
				while(1)
				{
					name.Format("AiGenerator_%d",AiNum);
					if(FindSceneObject(name)!=NULL)
					{
						AiNum++;
					}else 
					{
						AiNum++;
						break;
					}
				}
				pnew->SetObjectName(name);
			}
			m_listSceneObject.AddTail(pnew);
			m_listAIGenerator.AddTail(pnew);
		}else if(type == CSceneObject::SO_TYPE_FIXED_NPC)
		{
			CSceneFixedNpcGenerator* pnew = new CSceneFixedNpcGenerator();
			pnew->ImpData(pFile,dwFileVersion);	
			
			AString name = pnew->GetObjectName();
			if(FindSceneObject(name)!=NULL)
			{
				int AiNum = 0;
				while(1)
				{
					name.Format("NpcGenerator_%d",AiNum);
					if(FindSceneObject(name)!=NULL)
					{
						AiNum++;
					}else 
					{
						AiNum++;
						break;
					}
				}
				pnew->SetObjectName(name);
			}
			m_listSceneObject.AddTail(pnew);
			m_listFixedNpc.AddTail(pnew);
		}
	}
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();//更新场景对象列表
	return true;
}

bool CSceneObjectManager::LoadAllObject(CELArchive& ar,int iLoadFlags)
{
#ifdef _EDEBUG
	DWORD loadTime[CSceneObject::SO_TYPE_DYNAMIC + 1];
	for( int s = 0; s < CSceneObject::SO_TYPE_DYNAMIC + 1; s++)
		loadTime[s] = 0;
	DWORD tempTime;
#endif
	//文件版本号校验
	DWORD dwFileVersion;
	ar.Read(&dwFileVersion,sizeof(DWORD));
	if(0 ==  dwFileVersion)
	{
		AfxMessageBox("场景对象文件版本太旧，编辑器不能正常加载！");
		return false;
	}
	if(dwFileVersion>FS_VERSION)
	{	
		AfxMessageBox("不知道的场景对象文件版本，编辑器不能正常加载！");
		return false;
	}
	if(dwFileVersion>0x2c) ar.Read(&m_dwVssFileVersion,sizeof(DWORD));
	if(dwFileVersion>0xd) ar.Read(&m_nObjectIDFlag,sizeof(int));
	if(m_dwVssFileVersion>50000)
	{
		AfxMessageBox("AUX_IncreaseVersion,程序跟踪VSS版本数据可能已经发生错误! 请马上与程序部门联系!");
		return false;
	}
	//读所有其他对象
	DWORD dwNum;
	ar.Read(&dwNum,sizeof(DWORD));
	for(DWORD n=0; n<dwNum; n++)
	{
		CSceneObject *pNewObject = NULL;
		int type;
		ar.Read(&type,sizeof(int));
		switch(type)
		{
		case CSceneObject::SO_TYPE_NO: break;
		case CSceneObject::SO_TYPE_LIGHT:
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneLightObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listLightObject.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_LIGHT] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_STATIC_MODEL:
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CStaticModelObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listStaticModelObject.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_STATIC_MODEL] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_WATER: break;
		case CSceneObject::SO_TYPE_AREA:
			if(dwFileVersion<2) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneBoxArea();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listDataBoxArea.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_AREA] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_AUDIO:
			if(dwFileVersion<2) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneAudioObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listAudio.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_AUDIO] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_BEZIER:
			if(dwFileVersion<2) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CEditerBezier(0,g_Render.GetA3DDevice());
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listDataBezier.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_BEZIER] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_DUMMY:
			if(dwFileVersion<3) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneDummyObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listDummy.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_DUMMY] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_ECMODEL:
			if(dwFileVersion<7) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneSkinModel();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listSkinModel.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_ECMODEL] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_CRITTER_GROUP:
			if(dwFileVersion<7) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CCritterGroup();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listCritterGroup.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_CRITTER_GROUP] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_SPECIALLY:
			if(dwFileVersion<8) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneSpeciallyObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listSpecially.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_SPECIALLY] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_FIXED_NPC:
			if(dwFileVersion<9) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneFixedNpcGenerator();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listFixedNpc.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_FIXED_NPC] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_AIAREA:
			if(dwFileVersion<9) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneAIGenerator();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listAIGenerator.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_AIAREA] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_PRECINCT:
			if(dwFileVersion<0xf) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CScenePrecinctObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listPrecinct.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_PRECINCT] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_GATHER:
			if(dwFileVersion<0x18) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneGatherObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listGatherObject.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_GATHER] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_RANGE:
			if(dwFileVersion<0x20) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneRangeObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listRange.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_RANGE] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_CLOUD_BOX:
			if(dwFileVersion<0x26) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CCloudBox();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listCloudBox.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_CLOUD_BOX] += a_GetTime() - tempTime;
#endif
			break;
		case CSceneObject::SO_TYPE_INSTANCE_BOX:
			if(dwFileVersion<0x2a) break;
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new SceneInstanceBox();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listInstanceBox.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_INSTANCE_BOX] += a_GetTime() - tempTime;
#endif
			break;
			
			case CSceneObject::SO_TYPE_DYNAMIC:	
#ifdef _EDEBUG
			tempTime = a_GetTime();
#endif 
			pNewObject = new CSceneDynamicObject();
			ASSERT(pNewObject);
			if(pNewObject->Load(ar,dwFileVersion,iLoadFlags))
			{
				m_listSceneObject.AddTail(pNewObject);
				m_listDynamic.AddTail(pNewObject);
			}else
			{
				pNewObject->Release();
				delete pNewObject;
				pNewObject = NULL;
				return false;
			}
#ifdef _EDEBUG
			loadTime[CSceneObject::SO_TYPE_DYNAMIC] += a_GetTime() - tempTime;
#endif
			break;
		}

		if(dwFileVersion<0xe)//0xe以前的场景对象，编辑器自动给它加一个ID
			if(pNewObject) pNewObject->SetObjectID(m_nObjectIDFlag++);
	}

#ifdef _EDEBUG
	int k;
	DWORD total = 0;
	CString LoadString[CSceneObject::SO_TYPE_DYNAMIC + 1] =
	{
		"SO_TYPE_NO",
		"SO_TYPE_LIGHT",
		"SO_TYPE_STATIC_MODEL",
		"SO_TYPE_WATER",
		"SO_TYPE_BEZIER",
		"SO_TYPE_AREA",
		"SO_TYPE_AUDIO",
		"SO_TYPE_DUMMY",
		"SO_TYPE_AIAREA",
		"SO_TYPE_SPECIALLY",
		"SO_TYPE_ECMODEL",
		"SO_TYPE_CRITTER_GROUP",
		"SO_TYPE_FIXED_NPC",
		"SO_TYPE_PRECINCT",
		"SO_TYPE_GATHER",
		"SO_TYPE_RANGE",
		"SO_TYPE_CLOUD_BOX",
		"SO_TYPE_INSTANCE_BOX",
		"SO_TYPE_DYNAMIC",
	};
	for(k = 0; k < CSceneObject::SO_TYPE_INSTANCE_BOX + 1; k++)
	{
		total += loadTime[k];
	}
	for(k = 0; k < CSceneObject::SO_TYPE_INSTANCE_BOX + 1; k++)
	{
		g_Log.Log("%s -- %d -- %4.2f",LoadString[k],loadTime[k],(float)loadTime[k]/(float)total);
	}
#endif
	
	//天空体,只有版本小于2之前存在，以后版本将不存在
	//已经全部移植到区域对象中去了.
	if(dwFileVersion<2)
	{
		bool m_bSphere;
		AString  m_listSkyPath[6]; 
		ar.Read(&m_bSphere,sizeof(bool));
		for(int i=0; i<6; i++)
			m_listSkyPath[i] = AString(ar.ReadString());
		
		if(m_bSphere)
		{
			float w = 512.0f;
			if(m_pMap)
			{
				CTerrain *pTerrain =  m_pMap->GetTerrain();
				if(pTerrain) w = pTerrain->GetTerrainSize();
			}
			//创建一个默认的区域
			PSCENEBOXAREA pDefaultArea = CreateBoxArea(AString("BoxArea_default"),true);
			BOXAREA_PROPERTY data;
			Init_BoxArea_Data(data);
			data.m_fLength = w;
			data.m_fWidth = data.m_fLength;
			data.m_fHeight = data.m_fLength;
			pDefaultArea->SetProperty(data);
			m_listSkyPath[0].CutLeft(13);
			m_listSkyPath[1].CutLeft(13);
			m_listSkyPath[2].CutLeft(13);
			pDefaultArea->SetSkyTexture(m_listSkyPath[0],m_listSkyPath[1],m_listSkyPath[2]);
			pDefaultArea->ReBuildArea();
		}
		
	}
	
	if(dwFileVersion<3 && dwFileVersion>0)
	{
		int nTypes = m_pForest->GetNumTreeType();
		for(int  i = 0; i< nTypes ; i++)
		{
			CELTree *pTree = m_pForest->GetTreeType(i);
			m_pPlants->AddPlant(pTree->GetTreeName(),pTree->GetTypeID(),PLANT_TREE);
		}
	}

	//植物的读取
	if(dwFileVersion>=1)//(版本0没有植物)
		m_pPlants->LoadPlants(ar,dwFileVersion);
	
	/*
		解决一个错误后果,保持默认区域为一个
	 */
	if(dwFileVersion<5)
	{
		int errornum = 0;
		ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
			int type = ptemp->GetObjectType();
			if(type == CSceneObject::SO_TYPE_AREA)
			{	
				if(((CSceneBoxArea*)ptemp)->IsDefaultArea())
					errornum++;
				if(errornum>1)
				{
					DeleteSceneObjectForever(ptemp);
					pos = m_listSceneObject.GetTailPosition();
					errornum = 0;
				}
			}
		}
	}

	if( dwFileVersion < 0x1d)
	{
		ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
			if((ptemp->GetObjectType()!=CSceneObject::SO_TYPE_FIXED_NPC) &&
				(ptemp->GetObjectType()!=CSceneObject::SO_TYPE_AIAREA))
				continue;
			
			ALISTPOSITION pos2 = pos;
			while( pos2 )
			{
				CSceneObject* ptemp2 = m_listSceneObject.GetPrev(pos2);
				if(strcmp(ptemp2->GetObjectName(),ptemp->GetObjectName())==0)
				{
					DeleteSceneObjectForever(ptemp2);
					break;
				}
			}
		}	
	}

	if(dwFileVersion<6)
		m_pPlants->RepairPlant_5();

	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	return true;
}


bool CSceneObjectManager::SaveForest(const char *szFile)
{
	return m_pForest->Save(szFile);
}

bool CSceneObjectManager::LoadForest(const char *szFile)
{
	if( !m_pForest->Load(szFile) )
		return false;
	
	return true;
}

int CSceneObjectManager::GetSortObjectNum(int nObjectType)
{
	int num = 0;
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if(type == nObjectType)
			num++;
	}
	return num;
}

APtrList<CSceneObject *>* CSceneObjectManager::GetSortObjectList(int sort)
{
	switch(sort) 
	{
	case CSceneObject::SO_TYPE_BEZIER:
		return &m_listDataBezier;
	case CSceneObject::SO_TYPE_PRECINCT:
		return &m_listPrecinct;
	case CSceneObject::SO_TYPE_CLOUD_BOX:
		return &m_listCloudBox;
	case CSceneObject::SO_TYPE_LIGHT:
		return &m_listLightObject;
	case CSceneObject::SO_TYPE_STATIC_MODEL:
		return &m_listStaticModelObject;
	case CSceneObject::SO_TYPE_AIAREA:
		return &m_listAIGenerator;
	case CSceneObject::SO_TYPE_FIXED_NPC:
		return &m_listFixedNpc;
	case CSceneObject::SO_TYPE_DYNAMIC:
		return &m_listDynamic;
	case CSceneObject::SO_TYPE_GATHER:
		return &m_listGatherObject;
	}
	return NULL;
}

bool CSceneObjectManager::SaveWaterMap(const char *szFile)
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  m_pMap->GetTerrain();

	if(pHeightMap == NULL || pTerrain == NULL) return false;
	ARectF   rcTerrain = pTerrain->GetTerrainArea();
	
	AFile fileToSave;
	if( !fileToSave.Open(szFile, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_Log.Log("CSceneObjectManager::SaveWaterMap(), failed to create file [%s]", szFile);
		return false;
	}
	
	int nWidth = pHeightMap->iWidth;
	int nHeight = pHeightMap->iHeight;
	float fWHeight;
	DWORD rw;
	float *pData = new float[nWidth * nHeight];
	if(pData==NULL) 
	{
		fileToSave.Close();
		return false;
	}

	// Build height map for water
	for(int h = 0; h < nHeight; h++)
	{
		for( int w = 0; w < nWidth; w++)
		{
			float fx = w * pTerrain->GetTileSize() + rcTerrain.left;
			float fz = rcTerrain.top - h * pTerrain->GetTileSize();
			A3DTerrainWater *pEWater = m_pMap->GetTerrainWater()->GetA3DTerrainWater();
			if(pEWater)
			{
				/*
				if( fz < -nHeight + 0.1f ) fz = -nHeight + 0.5f;
				if( fz > nHeight - 0.1f ) fz = nHeight - 0.5f;
				if( fx > nWidth - 0.1f ) fx = nWidth - 0.5f;
				if( fx < -nWidth + 0.1f) fx = -nWidth + 0.5f;*/
				fWHeight = pEWater->GetWaterHeight(A3DVECTOR3(fx,0.0f,fz));
				pData[h*nWidth + w] = fWHeight;
			}
		}
	}
	
	// Write water's data to file
	if(!fileToSave.Write(pData,sizeof(float)*nWidth*nHeight,&rw))
	{
		g_Log.Log("CSceneObjectManager::SaveWaterMap(), failed to write file [%s]", szFile);
		delete []pData;
		fileToSave.Close();
		return false;
	}
	
	// Release temp data
	delete []pData;
	fileToSave.Close();
	return true;
}

bool CSceneObjectManager::ExportConvexHullData(const char *szFile)
{
	DWORD dwWriteLen;
	DWORD dwWriteObjectNum = 0;

	AFile fileToSave;
	if( !fileToSave.Open(szFile, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_Log.Log("CSceneObjectManager::ExportHullData(), failed to create file [%s]", szFile);
		return false;
	}
	
	fileToSave.Write(&dwWriteObjectNum,sizeof(DWORD),&dwWriteLen);

	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetNext(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			PESTATICMODEL pModel = ((CStaticModelObject*)ptemp)->GetA3DModel();
			if(pModel==NULL)
			{
				continue;
			}
			if(pModel->pModel == NULL)
			{
				continue;
			}
			
			int nObjectID = ptemp->GetObjectID();
			fileToSave.Write(&nObjectID,sizeof(int),&dwWriteLen);
			
			if(!pModel->pModel->SaveCHFDataForEditor(&fileToSave))
			{
				assert(false);
				fileToSave.Close();
				return false;
			}

			pModel->pModel->ReleaseConvexHullData();
			pModel->pModel->ReleaseHullMeshList();

			dwWriteObjectNum ++;
		}	
	}
	fileToSave.Seek(0,AFILE_SEEK_SET);
	fileToSave.Write(&dwWriteObjectNum,sizeof(DWORD),&dwWriteLen);

	fileToSave.Close();

	return true;
}

bool CSceneObjectManager::ImportConvexHullData(const char *szFile)
{
	DWORD dwReadLen;
	DWORD dwReadObjectNum = 0;

	AFile fileToRead;
	if( !fileToRead.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		g_Log.Log("CSceneObjectManager::ImportHullData(), failed to create file [%s]", szFile);
		return false;
	}
	if( !fileToRead.Read(&dwReadObjectNum,sizeof(DWORD),&dwReadLen))
		return false;

	for( unsigned int i = 0; i < dwReadObjectNum; ++i)
	{
		int nObjectID;
		if( !fileToRead.Read(&nObjectID,sizeof(int),&dwReadLen))
		{
			fileToRead.Close();
			return false;
		}
		CStaticModelObject* pObj = (CStaticModelObject*)FindBuilding(nObjectID);
		if(pObj == NULL) 
		{
			fileToRead.Close();
			g_Log.Log("CSceneObjectManager::ImportHullData(), can't find id = [%d] static model", nObjectID);
			return false;
		}
		if(!pObj->GetA3DModel()->pModel->LoadCHFDataForEditor(&fileToRead))
		{
			fileToRead.Close();
			g_Log.Log("CSceneObjectManager::ImportHullData(), can't call ELBuilding::LoadCHFDataForEditor", nObjectID);
			return false;	
		}
	
	}
	fileToRead.Close();
	return true;
}

bool CSceneObjectManager::SaveLitModels(const char *szFile)
{
	AFile fileToSave;
	if( !fileToSave.Open(szFile, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_Log.Log("CSceneObjectManager::SaveLitModels(), failed to create file [%s]", szFile);
		return false;
	}

	DWORD dwWriteObjectNum = GetSortObjectNum(CSceneObject::SO_TYPE_STATIC_MODEL);
	DWORD dwWriteLen;
	// first of all write a version for future compatibility
	DWORD dwVersion = LIT_MODEL_VERSION;
	if( !fileToSave.Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
	{
		fileToSave.Close();
		return false;
	}

	bool bRet = true;
	dwWriteObjectNum = 0;
	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetNext(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			PESTATICMODEL pModel = ((CStaticModelObject*)ptemp)->GetA3DModel();
			if(pModel==NULL)
			{
				//assert(false);
				bRet = false;
				continue;
			}
			if(pModel->pModel == NULL)
			{
				//assert(false);
				bRet = false;
				continue;
			}
			dwWriteObjectNum++;
		}
	}

	
	if( !fileToSave.Write(&dwWriteObjectNum, sizeof(DWORD), &dwWriteLen) )
	{
		fileToSave.Close();
		return false;
	}

	pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetNext(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			PESTATICMODEL pModel = ((CStaticModelObject*)ptemp)->GetA3DModel();
			if(pModel==NULL)
			{
				continue;
			}
			if(pModel->pModel == NULL)
			{
				continue;
			}
			
			int nObjectID = ptemp->GetObjectID();
			fileToSave.Write(&nObjectID,sizeof(int),&dwWriteLen);
			if(!pModel->pModel->Save(&fileToSave))
			{
				assert(false);
				fileToSave.Close();
				return false;
			}

// 			fileToSave.WriteString(ptemp->GetObjectName());
// 			fileToSave.WriteString(pModel->m_strPathName);
// 			fileToSave.WriteString(pModel->m_strHullPathName);
// 			int nObjectID = ptemp->GetObjectID();
// 			fileToSave.Write(&nObjectID,sizeof(int),&dwWriteLen);
// 		
// 			bool btemp = ((CStaticModelObject*)ptemp)->GetReflectFlag();
// 			if(!fileToSave.Write(&btemp,sizeof(bool),&dwWriteLen))
// 			{
// 				fileToSave.Close();
// 				return false;
// 			}
// 			btemp = ((CStaticModelObject*)ptemp)->GetRefractFlag();
// 			if(!fileToSave.Write(&btemp,sizeof(bool),&dwWriteLen))
// 			{
// 				fileToSave.Close();
// 				return false;
// 			}
// 
// 			btemp = ((CStaticModelObject*)ptemp)->IsShawdow();
// 			if(!fileToSave.Write(&btemp,sizeof(bool),&dwWriteLen))
// 			{
// 				fileToSave.Close();
// 				return false;
// 			}
// 			
// 			btemp = ((CStaticModelObject*)ptemp)->IsTree();
// 			if(!fileToSave.Write(&btemp,sizeof(bool),&dwWriteLen))
// 			{
// 				fileToSave.Close();
// 				return false;
// 			}
// 
// 			if(!pModel->pModel->Save(&fileToSave))
// 			{
// 				fileToSave.Close();
// 				return false;
// 			}
		}
	}

	fileToSave.Close();
	return true;
}

bool CSceneObjectManager::LoadLitModels(const char *szFile, int iLoadFlags)
{
	g_TempModelMap.clear();
	if(iLoadFlags == LOAD_EXPLIGHTMAP) g_pA3DConfig->SetFlagNoTextures(true);
	AFile FileToRead;
	if( !FileToRead.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		g_Log.Log("CSceneObjectManager::LoadLitModels(), failed to open file [%s]", szFile);
		g_pA3DConfig->SetFlagNoTextures(false);
		return true;
	}
	
	DWORD dwWriteLen;
	DWORD dwVersion;
	DWORD dwReadObjectNum = 0;
	// first of all write a version for future compatibility
	
	if( !FileToRead.Read(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto READFAIL;
	if(dwVersion < 2)//0 version for lit model
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
			ptemp->SetObjectID(g_nStaticNum++);
			AString szObjectName;
			AString szModelPathName;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			
			
			if(dwVersion<1)
			{
				if( -1 == szModelPathName.Find("Models",0))
				{
					szModelPathName = "Models\\" + szModelPathName;
				}
			}

			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			//pStaticModel->m_strPathName = szModelPathName;
			AString strHull = szModelPathName;
			strHull.CutRight(4);
			strHull = strHull + ".chf";
			//pStaticModel->m_strHullPathName = strHull;
			A3DLitModel *pNewModel = new A3DLitModel();//Don't delete a3dlitmodel
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}
			A3DVECTOR3 pos = pNewModel->GetPos();
			A3DVECTOR3 dir = pNewModel->GetDir();
			A3DVECTOR3 up = pNewModel->GetUp();
			A3DVECTOR3 scl = pNewModel->GetScale();
			pNewModel->Release();
			delete pNewModel;
			CELBuilding *pBuilding = new CELBuilding();
			if(!pBuilding->LoadFromMOXAndCHF(g_Render.GetA3DDevice(),szModelPathName,strHull))
			{
				CString msg;
				msg.Format("CSceneObjectManager::LoadLitModels(),Load %s and %s failed!", szModelPathName,strHull);
				g_Log.Log(msg);
				g_LogDlg.Log(msg);
				delete pBuilding;
				delete pStaticModel;
				delete ptemp;
				continue;
			}
			pBuilding->SetPos(pos);
			pBuilding->SetDirAndUp(dir,up);
			pBuilding->SetScale(scl.x);
			pStaticModel->pModel = pBuilding;
			ptemp->SetA3dModel(pStaticModel);
			ptemp->SetObjectID(GenerateObjectID());
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);
			
			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(strHull);

			A3DMATRIX4 mat = pBuilding->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pBuilding->GetPos();
			A3DVECTOR3 vs = pBuilding->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
		}
	}else
	if(dwVersion==2)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
			ptemp->SetObjectID(m_nObjectIDFlag++);
			AString szObjectName;
			AString szModelPathName;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			ptemp->SetShawdow( b3 );
			ptemp->SetReflact( b2 );
			ptemp->SetReflect( b1 );
			
			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			//pStaticModel->m_strPathName = szModelPathName;
			AString strHull = szModelPathName;
			strHull.CutRight(4);
			strHull = strHull + ".chf";
			//pStaticModel->m_strHullPathName = strHull;
			A3DLitModel *pNewModel = new A3DLitModel();
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}
			A3DVECTOR3 pos = pNewModel->GetPos();
			A3DVECTOR3 dir = pNewModel->GetDir();
			A3DVECTOR3 up = pNewModel->GetUp();
			A3DVECTOR3 scl = pNewModel->GetScale();
			pNewModel->Release();
			delete pNewModel;
			CELBuilding *pBuilding = new CELBuilding();
			if(!pBuilding->LoadFromMOXAndCHF(g_Render.GetA3DDevice(),szModelPathName,strHull))
			{
				CString msg;
				msg.Format("CSceneObjectManager::LoadLitModels(),Load %s and %s failed!",szModelPathName,strHull);
				g_Log.Log(msg);
				g_LogDlg.Log(msg);
				delete pBuilding;
				delete pStaticModel;
				delete ptemp;
				continue;
			}
			pBuilding->SetPos(pos);
			pBuilding->SetDirAndUp(dir,up);
			pBuilding->SetScale(scl.x);
			pStaticModel->pModel = pBuilding;
			ptemp->SetA3dModel(pStaticModel);
			ptemp->SetObjectID(GenerateObjectID());
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);

			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(strHull);

			A3DMATRIX4 mat = pBuilding->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pBuilding->GetPos();
			A3DVECTOR3 vs = pBuilding->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
		}
	}
	if(dwVersion>2 && dwVersion<=3)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
			ptemp->SetObjectID(m_nObjectIDFlag++);
			AString szObjectName;
			AString szModelPathName;
			AString szModelHullPath;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelHullPath))
				goto READFAIL;
			
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			ptemp->SetShawdow( b3 );
			ptemp->SetReflact( b2 );
			ptemp->SetReflect( b1 );
			
			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}
			pStaticModel->pModel = pNewModel;
			ptemp->SetA3dModel(pStaticModel);
			ptemp->SetObjectID(GenerateObjectID());
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);

			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(szModelHullPath);

			A3DMATRIX4 mat = pNewModel->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pNewModel->GetPos();
			A3DVECTOR3 vs = pNewModel->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
		}	
	}else if(dwVersion == 0x4)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
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
			ptemp->SetObjectID(nObjectID);
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			ptemp->SetShawdow( b3 );
			ptemp->SetReflact( b2 );
			ptemp->SetReflect( b1 );
			
			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}
			pStaticModel->pModel = pNewModel;
			//pStaticModel->m_strPathName = szModelPathName;
			//pStaticModel->m_strHullPathName = szModelHullPath;
			ptemp->SetA3dModel(pStaticModel);

			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(szModelHullPath);

			A3DMATRIX4 mat = pNewModel->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pNewModel->GetPos();
			A3DVECTOR3 vs = pNewModel->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
			
			if(nObjectID + 1 > m_nObjectIDFlag) m_nObjectIDFlag = nObjectID + 1;
			ptemp->SetObjectID(GenerateObjectID());
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);
		}		
	}else if( dwVersion > 0x4 && dwVersion < 0x6)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
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
			ptemp->SetObjectID(nObjectID);
			bool b1,b2,b3;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			ptemp->SetShawdow( b3 );
			ptemp->SetReflact( b2 );
			ptemp->SetReflect( b1 );
			
			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}
			pStaticModel->pModel = pNewModel;
			//pStaticModel->m_strPathName = szModelPathName;
			//pStaticModel->m_strHullPathName = szModelHullPath;
			ptemp->SetA3dModel(pStaticModel);
			
			if(nObjectID + 1 > m_nObjectIDFlag) m_nObjectIDFlag = nObjectID + 1;
			//ptemp->SetObjectID(g_nStaticNum++);
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);

			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(szModelHullPath);

			A3DMATRIX4 mat = pNewModel->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pNewModel->GetPos();
			A3DVECTOR3 vs = pNewModel->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
		}		
	} else if(dwVersion == 0x6)
	{
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			CStaticModelObject* ptemp = new CStaticModelObject();
			ASSERT(ptemp);
			AString szObjectName;
			AString szModelPathName;
			AString szModelHullPath;
			
			if(!FileToRead.ReadString(szObjectName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelPathName))
				goto READFAIL;
			if(!FileToRead.ReadString(szModelHullPath))
				goto READFAIL;
			int nObjectID;
			FileToRead.Read(&nObjectID,sizeof(int),&dwWriteLen);
			ptemp->SetObjectID(nObjectID);
			bool b1,b2,b3,b4;
			if(!FileToRead.Read(&b1,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b2,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b3,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			if(!FileToRead.Read(&b4,sizeof(bool),&dwWriteLen))
				goto READFAIL;
			ptemp->SetShawdow( b3 );
			ptemp->SetReflact( b2 );
			ptemp->SetReflect( b1 );
			ptemp->SetTreeFlag( b4 );
			
			ptemp->SetObjectName(szObjectName);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			
			//pNewModel->Init(g_Render.GetA3DDevice());
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pNewModel;
				delete pStaticModel;
				delete ptemp;
				goto READFAIL;
			}

			A3DAABB aabb = pNewModel->GetModelAABB();
			A3DVECTOR3 max = aabb.Maxs;
			A3DVECTOR3 min = aabb.Mins;
			//pStaticModel->pModel = pNewModel;
			//pStaticModel->id = nObjectID;
			
			if(g_bOnlyLoadNearBuildForLight)
			{
				if(max.x > -400.0f && max.x < 400.0f &&
					max.z > -400.0f && max.z < 400.0f && 
					min.x > -400.0f && min.x < 400.0f &&
					min.z > -400.0f && min.z < 400.0f
					)
				{
					delete pStaticModel;
					pNewModel->Release();
					delete pNewModel;
					delete ptemp;
					continue;
				}
			}

			pStaticModel->pModel = pNewModel;
			//pStaticModel->m_strPathName = szModelPathName;
			//pStaticModel->m_strHullPathName = szModelHullPath;
			ptemp->SetA3dModel(pStaticModel);

			ptemp->SetModelPath(szModelPathName);
			ptemp->SetHullPath(szModelHullPath);

			A3DMATRIX4 mat = pNewModel->GetAbsoluteTM();
			
			A3DVECTOR3 vecX = mat.GetRow(0);
			A3DVECTOR3 vecY = mat.GetRow(1);
			A3DVECTOR3 vecZ = mat.GetRow(2);
			A3DVECTOR3 vPos = pNewModel->GetPos();
			A3DVECTOR3 vs = pNewModel->GetScale();
			vecX.Normalize();
			vecY.Normalize();
			vecZ.Normalize();
			
			ptemp->SetScale(vs);
			ptemp->SetDirAndUp(vecZ,vecY);
			ptemp->SetPos(vPos);
			
			if(nObjectID + 1 > m_nObjectIDFlag) m_nObjectIDFlag = nObjectID + 1;
			//ptemp->SetObjectID(g_nStaticNum++);
			m_listSceneObject.AddTail(ptemp);
			m_listStaticModelObject.AddTail(ptemp);
		}		
	}
	else if(dwVersion >= 0x7)
	{
		//这个版本有重大变更，以前放弃保存信息在.scene，现在重新启动
		if( !FileToRead.Read(&dwReadObjectNum, sizeof(DWORD), &dwWriteLen) )
			goto READFAIL;
		
		for(DWORD i = 0; i< dwReadObjectNum; i++)
		{
			int nObjectID = 0;
			FileToRead.Read(&nObjectID,sizeof(int),&dwWriteLen);
			PESTATICMODEL pStaticModel = new ESTATICMODEL();
			ASSERT(pStaticModel);
			CELBuilding *pNewModel = new CELBuilding();
			ASSERT(pNewModel);
			if(!pNewModel->Load(g_Render.GetA3DDevice(),&FileToRead))
			{
				delete pStaticModel;
				delete pNewModel;
				
			
				goto READFAIL;
			}
			
			A3DAABB aabb = pNewModel->GetModelAABB();
			A3DVECTOR3 max = aabb.Maxs;
			A3DVECTOR3 min = aabb.Mins;
			pStaticModel->pModel = pNewModel;
			pStaticModel->id = nObjectID;
			
			if(g_bOnlyLoadNearBuildForLight)
			{
				if(max.x > -410.0f && max.x < 410.0f &&
					max.z > -410.0f && max.z < 410.0f && 
					min.x > -410.0f && min.x < 410.0f &&
					min.z > -410.0f && min.z < 410.0f
					)
				{
					delete pStaticModel;
					pNewModel->Release();
					delete pNewModel;
					continue;
				}
			}
			
			g_TempModelMap[nObjectID] = pStaticModel;
		}		
	}

	FileToRead.Close();
	g_pA3DConfig->SetFlagNoTextures(false);
	return true;

READFAIL:
	CString msg;
	msg.Format("CSceneObjectManager::LoadLitModels(), loading %s file failed",szFile);
	g_Log.Log(msg);
	g_LogDlg.Log(msg);
	FileToRead.Close();
	g_pA3DConfig->SetFlagNoTextures(false);
	return false;
}

void CSceneObjectManager::ReloadLitModels()
{
	vector<CSceneObject *> delObject;
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			CStaticModelObject *pStaticModel = (CStaticModelObject *)ptemp;
			PESTATICMODEL pEM = pStaticModel->GetA3DModel();
			
			if(pEM==0) 
			{
				pEM = new ESTATICMODEL;
				pEM->pModel = 0;
			}
			if(pEM->pModel==0) pEM->pModel = new CELBuilding;
			CELBuilding *pLitModel = pEM->pModel;
			pLitModel->Release();


			//Save property data
// 			A3DVECTOR3 vPos = pLitModel->GetPos();
// 			A3DVECTOR3 vDir = pLitModel->GetDir();
// 			A3DVECTOR3 vUp = pLitModel->GetUp();
// 			A3DVECTOR3 vScale = pLitModel->GetScale();
// 			bool bCollide = pStaticModel->GetCollideFlags();
// 			//A3DMATRIX4 matAbs = pLitModel->GetAbsoluteTM();
// 			//bool bVisibility = pLitModel->GetVisibility();
// 			//Release object
// 			pLitModel->Release();
			
			/*
			//Reload object
			if(!pLitModel->LoadFromMOXAndCHF(g_Render.GetA3DDevice(),tem,pEM->m_strHullPathName))
			{
				AString msg;
				msg.Format("不能打开下面文件: (%s)!\n 该文件关联到静态模型%s上.\n你可以去改变该静态模型的关联路径，也可以删除该模型！你要删除该静态模型吗？",tem,ptemp->GetObjectName());
				g_Log.Log("Failed to load file %s or %s, in %s static model",tem,pEM->m_strHullPathName,ptemp->GetObjectName());
				if(IDYES==AfxMessageBox(msg,MB_YESNO))
				{
					delObject.push_back(ptemp);
				}
			}*/
			CString tem = pStaticModel->GetModelPath();
			tem.MakeLower();
			if(tem.Find("building\\models")!=-1)
			{
				AFile sFile;
				if(!sFile.Open(tem,AFILE_OPENEXIST))
				{
					AString msg;
					msg.Format("不能打开下面文件: (%s)!\n 该文件关联到静态模型%s上.\n你可以去改变该静态模型的关联路径，也可以删除该模型！你要删除该静态模型吗？",tem,ptemp->GetObjectName());
					g_Log.Log("CSceneObjectManager::ReloadLitModels(),Failed to open file %s, in %s static model",tem,ptemp->GetObjectName());
					if(IDYES==AfxMessageBox(msg,MB_YESNO))
					{
						delObject.push_back(ptemp);
					}

					delete pEM->pModel;
					pEM->pModel = 0;
				continue;
				}else
				{
					if(!pLitModel->Load(g_Render.GetA3DDevice(),&sFile))
					{
						AString msg;
						msg.Format("不能打开下面文件: (%s)!\n 该文件关联到静态模型%s上.\n你可以去改变该静态模型的关联路径，也可以删除该模型！你要删除该静态模型吗？",tem,ptemp->GetObjectName());
						g_Log.Log("CSceneObjectManager::ReloadLitModels(),Failed to load file %s, in %s static model",tem,ptemp->GetObjectName());
						if(IDYES==AfxMessageBox(msg,MB_YESNO))
						{
							delObject.push_back(ptemp);
						}
						pLitModel->Release();
						delete pEM->pModel;
						pEM->pModel = 0;
					}

				}
				sFile.Close();
			}else
			{
				AString msg;
				msg.Format("该模型依然是原始模型(%s),你只能删除掉该模型!",tem);
				if(IDYES==AfxMessageBox(msg,MB_YESNO))
				{
					delObject.push_back(ptemp);
				}
				delete pEM->pModel;
				pEM->pModel = 0;
				continue;
			}
			
			pStaticModel->SetA3dModel(pEM);
			pStaticModel->UpdateModel();
		}
	}

	int n = delObject.size();
	for( int i = 0; i < n ; i++)
	{
		CSceneObject *ptemp = delObject.at(i);
		ptemp->SetDeleted(true);
		DeleteSceneObject(ptemp->GetObjectName());
	}
	delObject.clear();
}

bool CSceneObjectManager::SaveGrassLand(const char *szFile)
{
	return m_pGrassLand->Save(szFile);
}

bool CSceneObjectManager::LoadGrassLand(const char *szFile)
{
	return m_pGrassLand->Load(szFile);
}

bool CSceneObjectManager::UpdateLighting(bool bIsNight)
{
	LIGHTINGPARAMS param;
	if(bIsNight) param = m_pMap->GetNightLightingParams();
	else param = m_pMap->GetLightingParams();

	return true;
}

int CSceneObjectManager::GenerateObjectID()
{ 
	m_nObjectIDFlag++;
	/* debug 
	CString msg;
	msg.Format("\nid = %d",m_nObjectIDFlag);
	g_Log.Log(msg);
	*/
	return m_nObjectIDFlag;
};

void CSceneObjectManager::CutLink_Bezier(int bezierID)
{
	ALISTPOSITION pos = m_listCritterGroup.GetTailPosition();
	while(pos)
	{
		CSceneObject *pObj = m_listCritterGroup.GetPrev(pos);
		if(pObj->GetObjectType() == CSceneObject::SO_TYPE_CRITTER_GROUP)
		{
			CCritterGroup* ptemp = (CCritterGroup*)pObj;
			CRITTER_DATA dat = ptemp->GetProperty();
			if(dat.nBezierID == bezierID)
			{
				dat.nBezierID = -1;
				dat.strBezierName = "";
				ptemp->SetProperty(dat);
			}
			
		}else if(pObj->GetObjectType() == CSceneObject::SO_TYPE_PRECINCT)
		{
			CScenePrecinctObject* ptemp = (CScenePrecinctObject*)pObj;
			PRECINCT_DATA dat = ptemp->GetProperty();
			if(dat.nPath == bezierID)
			{
				dat.nPath = -1;
				dat.strPathName = "";
				ptemp->SetProperty(dat);
			}
		}else if(pObj->GetObjectType() == CSceneObject::SO_TYPE_RANGE)
		{
			CSceneRangeObject *ptemp = (CSceneRangeObject*)pObj;
			int nID = ptemp->GetBezierID();
			if(nID == bezierID)
			{
				ptemp->SetBezierID(-1);
				ptemp->SetBezierName("");
			}
		}
		
	}
}

void CSceneObjectManager::CutLink_Precinct_Dummy(int dummyID)
{
	/*
	ALISTPOSITION pos = m_listPrecinct.GetTailPosition();
	while(pos)
	{
		CScenePrecinctObject* ptemp = (CScenePrecinctObject*)m_listPrecinct.GetPrev(pos);
		PRECINCT_DATA dat = ptemp->GetProperty();
		if(dat.nDummyID == dummyID)
		{
			dat.nDummyID = -1;
			dat.strDummyName = "";
			ptemp->SetProperty(dat);
		}
	}*/
}

//	Find a bezier route object with specified ID
CEditerBezier* CSceneObjectManager::FindBezier(int iID)
{
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while (pos)
	{
		PSCENEOBJECT ptemp = m_listSceneObject.GetPrev(pos);
		if (ptemp->GetObjectType() == CSceneObject::SO_TYPE_BEZIER &&
			ptemp->GetObjectID() == iID)
		{
			return (CEditerBezier*)ptemp;
		}
	}

	return NULL;
}

CSceneObject* CSceneObjectManager::FindBuilding(int iID)
{
	ALISTPOSITION pos = m_listSceneObject.GetTailPosition();
	while (pos)
	{
		PSCENEOBJECT ptemp = m_listSceneObject.GetPrev(pos);
		if (ptemp->GetObjectType() == CSceneObject::SO_TYPE_STATIC_MODEL &&
			ptemp->GetObjectID() == iID)
		{
			return ptemp;
		}
	}

	return NULL;
}

bool CSceneObjectManager::SphereTraceStaticModel(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData, bool bExactCD)
{
	float fraction, fMinFraction=1.0f;
	A3DVECTOR3 vHitPos,vNormal;
	bool bCollide=false;

	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* pSO = m_listSceneObject.GetNext(pos);
		if(pSO->GetObjectType()!=CSceneObject::SO_TYPE_STATIC_MODEL)
			continue;
		CELBuilding* pBuilding = ((CStaticModelObject *)pSO)->GetA3DModel()->pModel;
		CConvexHullData *pCHData=NULL;
		if(pBuilding && pBuilding->SphereCollideWithMe(vecStart,fRadius,vecDelta,vHitPos,&fraction,vNormal, pCHData))
		{
			if(fraction<fMinFraction)
			{
				fMinFraction=fraction;
				vecNormal=vNormal;
				vecHitPos=vHitPos;
				pCDCHData=pCHData;
				bCollide=true;
			}
		}
	}		

	// now collide with the forrest
	if( m_pForest->SphereCollideWithMe(vecStart, fRadius, vecDelta, vHitPos, &fraction, vNormal) )
	{
		if(fraction<fMinFraction)
		{
			fMinFraction=fraction;
			vecNormal=vNormal;
			vecHitPos=vHitPos;
			pCDCHData=NULL;
			bCollide=true;
		}
	}
	if(bCollide)
	{
		*pvFraction=fMinFraction;
		return true;
	}
	else
		return false;	
}

bool CSceneObjectManager::RayTraceStaticModel(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData, bool bExactCD)
{
	float fraction, fMinFraction=1.0f;
	A3DVECTOR3 vHitPos,vNormal;
	bool bCollide=false;

	// Ray trace building
	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* pSO = m_listSceneObject.GetNext(pos);
		if(pSO->GetObjectType()!=CSceneObject::SO_TYPE_STATIC_MODEL)
			continue; 
		CELBuilding* pBuilding = ((CStaticModelObject *)pSO)->GetA3DModel()->pModel;

		if(!pBuilding) continue;
		
		CConvexHullData* pCHData=NULL;
		if(bExactCD)
		{
			// must trace mesh
			if(pBuilding->RayTraceMesh(vecStart,vecDelta,vHitPos,&fraction,vNormal))
			{
				if(fraction<fMinFraction)
				{
					fMinFraction=fraction;
					vecNormal=vNormal;
					vecHitPos=vHitPos;
					pTraceCHData=NULL;
					bCollide=true;
				}
			}
		}
		else	// Just trace convex hull
			if(pBuilding->RayTraceConvexHull(vecStart,vecDelta,vHitPos,&fraction,vNormal,pCHData))
			{
				if(fraction<fMinFraction)
				{
					fMinFraction=fraction;
					vecNormal=vNormal;
					vecHitPos=vHitPos;
					pTraceCHData=pCHData;
					bCollide=true;
				}

			}
	}

	if(bCollide)
	{
		*pvFraction=fMinFraction;
		return true;
	}
	else
		return false;	

}

void CSceneObjectManager::GetAllConvexHulls(abase::vector<CConvexHullData *>& CHs)
{
	ALISTPOSITION pos = m_listSceneObject.GetHeadPosition();
	while( pos )
	{
		CSceneObject* pSO = m_listSceneObject.GetNext(pos);
		if(pSO->GetObjectType()!=CSceneObject::SO_TYPE_STATIC_MODEL)
			continue; 
		CELBuilding* pBuilding = ((CStaticModelObject *)pSO)->GetA3DModel()->pModel;
		if(!pBuilding) continue;
		
		for(int i=0; i< (int)(pBuilding->GetConvexHulls().size()); ++i)
		{
			CHs.push_back(pBuilding->GetConvexHulls()[i]);
		}
		
	}
}

void CSceneObjectManager::PlayMusic()
{
	if(m_pCurrentPrecinct) m_pCurrentPrecinct->PlayBackMusic();
}

void CSceneObjectManager::StopMusic()
{
	m_BackMusic.StopMusic();
	//m_pBackSound->Stop();
}

void CSceneObjectManager::SetCurrentPrecinct(CScenePrecinctObject *pCur)
{ 
	m_pCurrentPrecinct = pCur; 
	if(pCur == NULL) return;
	PlayMusic(); 
};

void CSceneObjectManager::ExportBuilding()
{
	//ElementResMan   mResMan;
	//mResMan.Load();
	char szModelPath[256];
	sprintf(szModelPath,"path_%s.txt",m_pMap->GetMapName());
	FILE * pPathFile = fopen(szModelPath,"wt");
	if(pPathFile == NULL) return;
	
	CTime time;
	int nYear = time.GetYear();
	int nMonth = time.GetMonth();
	
	ALISTPOSITION pos = m_listStaticModelObject.GetHeadPosition();
	while(pos)
	{
		CStaticModelObject* pStatic = (CStaticModelObject*)m_listStaticModelObject.GetNext(pos);
		if(pStatic->GetA3DModel()==NULL) continue;
		
		
		CString strName = pStatic->GetModelPath();
		CString strPath;
		CELBuilding mBuilding;
		mBuilding.Load(g_Render.GetA3DDevice(), strName);
		A3DLitModel* pModel = mBuilding.GetA3DLitModel();
		
	
		
		int n = pModel->GetNumMeshes();
		for( int i = 0; i < n; ++i)
		{
			A3DLitMesh* pMesh = pModel->GetMesh(i);
			const char* szTexPath = pMesh->GetTextureMap();
			ElementResMan::GetPathAndName(szTexPath,strPath,strName);
			strPath.Format("Building\\Textures\\Fixed%d.%d\\%s\\%s",nYear,nMonth,m_pMap->GetMapName(),strName);
			DWORD flag = GetFileAttributes(strPath);
			if (flag & FILE_ATTRIBUTE_READONLY) 
			{
				flag ^= FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(strPath,flag);
			}
			if(!ElementResMan::CopyResFileEx(strPath,szTexPath))
			{
				g_Log.Log("CSceneObjectManager::ExportBuilding(),Failed to copy files %s-%s",szTexPath,strPath);
				fclose(pPathFile);
				return;
			}
			pMesh->ChangeTextureMap(strPath);
		}
		
		ElementResMan::GetPathAndName(CString(pStatic->GetModelPath()),strPath,strName);
		strPath.Format("Building\\models\\Fixed%d.%d\\%s\\",nYear,nMonth,m_pMap->GetMapName(),strName);
		ElementResMan::CreateFolder(strPath);
		strPath = strPath + strName;
		AFile sFile;
		if(!sFile.Open(strPath,AFILE_CREATENEW))
		{
			g_Log.Log("CSceneObjectManager::ExportBuilding(),创建新文件%s失败!",strPath);
			mBuilding.Release();
			fclose(pPathFile);
			return;
		}
		if(!mBuilding.Save(&sFile))
		{
			g_Log.Log("CSceneObjectManager::ExportBuilding(),保存文件%s失败!",strPath);
			mBuilding.Release();
			fclose(pPathFile);
			return;
		}
		sFile.Close();
		mBuilding.Release();
		
		pStatic->SetModelPath(AString(strPath));
		fprintf(pPathFile,"%s\n",strPath);
		//mResMan.FastAddBuildingRes("Fixed2010",strPath);
	}
	fclose(pPathFile);
	//mResMan.Save();
}
