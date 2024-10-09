#include "global.h"
#include "OperationManager.h"
#include "TerrainStretchOperation.h"
#include "TerrainNoiseOperation.h"
#include "TerrainSmoothOperation.h"
#include "TerrainMaskPaintOperation.h"
#include "TerrainMaskSmoothOperation.h"
#include "SceneLightAddOperation.h"
#include "SceneObjectSelectOperation.h"
#include "SceneObjectMoveOperation.h"
#include "SceneObjectRotateOperation.h"
#include "SceneObjectMultiSelectOperation.h"
#include "SceneModelAddOperation.h"
#include "SceneObjectScaleOperation.h"
#include "SceneWaterAddOperation.h"
#include "TerrainPlantPaintOperation.h"
#include "BezierAddOperation.h"
#include "SceneAreaAddOperation.h"
#include "BezierDeletePointOperation.h"
#include "AudioAddOperation.h"
#include "SetVernierOperation.h"
#include "SceneDummyAddOperation.h"
#include "SpeciallyAddOperation.h"
#include "AIGeneratorOperation.h"
#include "ObstructOperation.h"
#include "ObstructOperation2.h"


//#define new A_DEBUG_NEW
/*
COperationManager::COperationManager()
{

}

COperationManager::~COperationManager()
{
	DeleteAllOperation();
}

COperation* COperationManager::CreateOperation(int type,char* name)
{
	
	COperation* ptemp = NULL;
	
	switch(type)
	{//You will modify on there
	case OPERATION_TERRAIN_STRETCH:
		ptemp = CreateTerrainStretchOperation(name);
		break;
	}
	
	if(ptemp)
		m_listOperation.AddTail(ptemp);
	return ptemp;
}

COperation* COperationManager::FindOperation(char* name)
{
	ALISTPOSITION pos = m_listOperation.GetTailPosition();
	
	while( pos )
	{
		COperation* ptemp = m_listOperation.GetPrev(pos);
		if(0==strcmp(ptemp->GetOperationName(),name))
		{
			return ptemp;
		}
	}
	return NULL;
}


void  COperationManager::DeleteOperation( char *name)
{

	ALISTPOSITION pos = m_listOperation.GetTailPosition();
	
	while( pos )
	{
		COperation* ptemp = m_listOperation.GetPrev(pos);
		if(0==strcmp(ptemp->GetOperationName(),name))
		{
			delete ptemp;
			ptemp = NULL;
			m_listOperation.RemoveAt(pos);
			return;
		}
	}
}

void  COperationManager::DeleteAllOperation()
{

	ALISTPOSITION pos = m_listOperation.GetTailPosition();
	
	while( pos )
	{
		COperation* ptemp = m_listOperation.GetPrev(pos);
		delete ptemp;
		ptemp = NULL;
	}

	m_listOperation.RemoveAll();
}

//Stretch operation
COperation* COperationManager::CreateTerrainStretchOperation( char*name )
{
	CTerrainStretchOperation *pTemp;

	pTemp = new CTerrainStretchOperation();
	if(pTemp)
	{
		pTemp->SetOperationName(name);
		return (COperation*)pTemp;
	}
	return NULL;
}
*/


OperationContainer::OperationContainer()
{
	for(int i=0; i<OPERATION_NUM; i++)
	m_nOperationArray[i] = NULL;

	CreateOperation();
}

OperationContainer::~OperationContainer()
{
	for(int i=0; i<OPERATION_NUM; i++)
	{
		delete m_nOperationArray[i];
		m_nOperationArray[i] = NULL;
	}
}

void OperationContainer::Release()
{
	for(int i=0; i<OPERATION_NUM; i++)
	{
		m_nOperationArray[i]->Release();
	}	
}


void OperationContainer::CreateOperation()
{
	CTerrainStretchOperation *pStretch = new CTerrainStretchOperation();
	CTerrainNoiseOperation *pNoise = new CTerrainNoiseOperation();
	CTerrainSmoothOperation *pSmooth = new CTerrainSmoothOperation();
	CTerrainMaskPaintOperation *pMaskPaint = new CTerrainMaskPaintOperation();
	CTerrainMaskSmoothOperation *pMaskSmooth = new CTerrainMaskSmoothOperation();
	CSceneLightAddOperation *pLightAdd = new CSceneLightAddOperation();
	CSceneObjectSelectOperation *pSelect = new CSceneObjectSelectOperation();
	CSceneObjectMoveOperation *pMove = new CSceneObjectMoveOperation();
	CSceneObjectRotateOperation *pRotate = new CSceneObjectRotateOperation();
	SceneObjectMultiSelectOperation *pMulti = new SceneObjectMultiSelectOperation();
	SceneModelAddOperation*pModel = new SceneModelAddOperation();
	SceneObjectScaleOperation *pScale = new SceneObjectScaleOperation();
	CSceneWaterAddOperation *pWater = new CSceneWaterAddOperation();
	CTerrainPlantPaintOperation *pPlant = new CTerrainPlantPaintOperation();
	CBezierAddOperation *pBezierAdd = new CBezierAddOperation();
	CSceneAreaAddOperation* pArea = new CSceneAreaAddOperation();
	CBezierDeletePointOperation* pBezierPtDelete = new CBezierDeletePointOperation();
	CBezierCutOperation *pBezierCut = new CBezierCutOperation();
	CBezierAddPointOperation *pPointAdd = new CBezierAddPointOperation();
	CBezierDragOperation *pBezierDrag = new CBezierDragOperation();
	CAudioAddOperation *pAudioAdd = new CAudioAddOperation();
	CSetVernierOperation *pVernier = new CSetVernierOperation();
	CBezierTestOperation *pBezierTest = new CBezierTestOperation();
	CSceneDummyAddOperation *pDummyAdd = new CSceneDummyAddOperation();
	CSpeciallyAddOperation *pSpecially = new CSpeciallyAddOperation();
	CAIGeneratorOperation *pAIGenerator = new CAIGeneratorOperation();
	CSceneSkinModelAddOperation *pSkin = new CSceneSkinModelAddOperation();
	CSceneCritterAddOperation *pCritter = new CSceneCritterAddOperation();
	CFixedNpcGeneratorOperation *pNpc = new CFixedNpcGeneratorOperation();
	CBezierMergeOperation *pMerge = new CBezierMergeOperation();
	CPrecinctOperation *pPrecinct = new CPrecinctOperation();
	CObstructOperation *pObstruct = new CObstructOperation();
	CSceneGatherOperation *pGather = new CSceneGatherOperation();
	CRangeOperation *pRange = new CRangeOperation();
	CSceneObjectRotateOperationEx* pRotateEx = new CSceneObjectRotateOperationEx();
	CCloudBoxOperation* pCloud = new CCloudBoxOperation();
	CSceneInstanceBoxAddOperation *pInstance = new CSceneInstanceBoxAddOperation();
	CDynamicObjectOperation*pDynamic = new CDynamicObjectOperation;
	CSceneObjectRotateOperation2  *pRotate2 = new CSceneObjectRotateOperation2();
	CObstructOperation2 *pObstruct2 = new CObstructOperation2();

	if(pStretch) m_nOperationArray[OPERATION_TERRAIN_STRETCH] = pStretch;
	if(pNoise) m_nOperationArray[OPERATION_TERRAIN_NOISE] = pNoise;
	if(pSmooth) m_nOperationArray[OPERATION_TERRAIN_SMOOTH] = pSmooth;
	if(pMaskPaint) m_nOperationArray[OPERATION_TERRAIN_PAINT] = pMaskPaint;
	if(pMaskSmooth) m_nOperationArray[OPERATION_TERRAIN_MASK_SMOOTH] = pMaskSmooth;
	if(pLightAdd) m_nOperationArray[OPERATION_SCENE_LIGHT_ADD] = pLightAdd;
	if(pSelect) m_nOperationArray[OPERATION_SCENE_OBJECT_SELECT] = pSelect;
	if(pMove) m_nOperationArray[OPERATION_SCENE_OBJECT_MOVE] = pMove;
	if(pRotate) m_nOperationArray[OPERATION_SCENE_OBJECT_ROTATE] = pRotate;
	if(pMulti) m_nOperationArray[OPERATION_SCENE_OBJECT_MULTISELECT] = pMulti;
	if(pModel) m_nOperationArray[OPERATION_SCENE_MODEL_ADD] = pModel;
	if(pScale) m_nOperationArray[OPERATION_SCENE_OBJECT_SCALE] = pScale;
	if(pWater) m_nOperationArray[OPERATION_SCENE_WATER_ADD] = pWater;
	if(pPlant) m_nOperationArray[OPERATION_TERRAIN_PLANTS] = pPlant;
	if(pBezierAdd) m_nOperationArray[OPERATION_BEZIER_ADD] = pBezierAdd;
	if(pArea) m_nOperationArray[OPERATION_SCENE_AREA_ADD] = pArea;
	if(pBezierPtDelete) m_nOperationArray[OPERATION_BEZIER_DELETE_POINT] = pBezierPtDelete;
	if(pBezierCut) m_nOperationArray[OPERATION_BEZIER_CUT_SEGMENT] = pBezierCut;
	if(pPointAdd) m_nOperationArray[OPERATION_BEZIER_ADD_POINT] = pPointAdd;
	if(pBezierDrag) m_nOperationArray[OPERATION_BEZIER_DRAG] = pBezierDrag;
	if(pAudioAdd) m_nOperationArray[OPERATION_SCENE_AUDIO_ADD] = pAudioAdd;
	if(pVernier)  m_nOperationArray[OPERATION_SET_VERNIER] = pVernier;
	if(pBezierTest) m_nOperationArray[OPERATION_BEZIER_TEST] = pBezierTest;
	if(pDummyAdd) m_nOperationArray[OPERATION_SCENE_DUMMY_ADD] = pDummyAdd;
	if(pSpecially) m_nOperationArray[OPERATION_SCENE_SPECIALLY_ADD] = pSpecially;
	if(pAIGenerator) m_nOperationArray[OPEAATION_SCENE_AIGENERATOR_ADD] = pAIGenerator;
	if(pSkin) m_nOperationArray[OPERATION_SCENE_OBJECT_SKINMODEL_ADD] = pSkin;
	if(pCritter) m_nOperationArray[OPERATION_SCENE_OBJECT_CRITTER_ADD] = pCritter;
	if(pNpc) m_nOperationArray[OPERATION_SCENE_NPCGENERATOR_ADD] = pNpc;
	if(pMerge) m_nOperationArray[OPERATION_BEZIER_MERGE] = pMerge;
	if(pPrecinct) m_nOperationArray[OPERATION_SCENE_PRECINCT_ADD] = pPrecinct;
	if(pObstruct) m_nOperationArray[OPERATION_TERRAIN_OBSTRUCT] = pObstruct;
	if(pGather) m_nOperationArray[OPERATION_SCENE_GATHER_ADD] = pGather;
	if(pRange) m_nOperationArray[OPERATION_SCENE_RANGE_ADD] = pRange;
	if(pRotateEx) m_nOperationArray[OPERATION_SCENE_OBJECT_ROTATE_EX] = pRotateEx;
	if(pCloud) m_nOperationArray[OPERATION_SCENE_CLOUD_BOX_ADD] = pCloud;
	if(pInstance) m_nOperationArray[OPERATION_SCENE_INSTANCE_BOX_ADD] = pInstance;
	if(pDynamic) m_nOperationArray[OPERATION_SCENE_OBJECT_DYNAMIC_ADD] = pDynamic;
	if(pRotate2) m_nOperationArray[OPERATION_SCENE_OBJECT_ROTATE2] = pRotate2;
	if(pObstruct2) m_nOperationArray[OPERATION_TERRAIN_OBSTRUCT2] = pObstruct2;
}


//Global Operation manager
OperationContainer g_OperationContainer;