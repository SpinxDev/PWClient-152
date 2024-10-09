// TerrainPlants.cpp: implementation of the CTerrainPlants class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "TerrainPlants.h"
#include "Render.h"
#include "MainFrm.h"
#include "EL_Forest.h"
#include "EL_Tree.h"
#include "SceneObjectManager.h"
#include "CoordinateDirection.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerrainPlants::CTerrainPlants(CSceneObjectManager* pSceneMan)
{
	m_pSceneMan	= pSceneMan;
}

CTerrainPlants::~CTerrainPlants()
{

}

PPLANT CTerrainPlants::AddPlant(AString name,int nID,int nType)
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return false;

	CElementMap* pMap = m_pSceneMan->GetMap();

	if(GetPlant(name)==NULL)
	{
		PPLANT pPlant = NULL;
		switch(nType)
		{
			
		case PLANT_TREE:
			pPlant= new TREE_PLANT(m_pSceneMan);
			ASSERT(pPlant);
			pPlant->density = 10;
			pPlant->minDistance = 1;
			pPlant->strName = name;
			pPlant->nID = nID;
			pPlant->bShow = true;
			pPlant->pData = NULL;
			m_listPlants.AddTail(pPlant);
			break;
		case PLANT_GRASS:
			pPlant= new GRASS_PLANT(m_pSceneMan);
			ASSERT(pPlant);
			pPlant->density = 10;
			pPlant->minDistance = 1;
			pPlant->strName = name;
			pPlant->nID = nID;
			pPlant->bShow = true;
			pPlant->pData = NULL;
			
		//	A3DTerrain2 *pTerrain = (A3DTerrain2*)(pMap->GetTerrain()->GetRender());
		//	if (pTerrain)
			if (1)
			{
			//	int nMaskGridNum = pTerrain->GetMaskAreaNum();
				int nMaskGridNum = 16;
				if(nMaskGridNum<=0) 
				{
					delete pPlant;
					pPlant = NULL;
					return NULL;
				}
				((GRASS_PLANT*)pPlant)->Init(nMaskGridNum);
				m_listPlants.AddTail(pPlant);
			}
			else
			{
				delete pPlant;
				pPlant = NULL;
			}

			break;
		}
		return pPlant;
	}
	return NULL;
}

void CTerrainPlants::DeletePlant(AString name)
{
	ALISTPOSITION pos = m_listPlants.GetTailPosition();
	ALISTPOSITION oldPos;
	while(pos)
	{
		oldPos = pos;
		PPLANT ptemp = m_listPlants.GetPrev(pos);
		if(0==strcmp(name,ptemp->strName))
		{
			delete ptemp;
			m_listPlants.RemoveAt(oldPos);
			return;
		}
	}
}

PPLANT CTerrainPlants::GetPlant(const AString& name)
{
	ALISTPOSITION pos = m_listPlants.GetTailPosition();
	while( pos )
	{
		
		PPLANT ptemp = m_listPlants.GetPrev(pos);
		if(0==strcmp(ptemp->strName,name))
		{
			return ptemp;
		}
	}
	return NULL;
}

PPLANT CTerrainPlants::GetPlantByIndex(int index)
{
	return m_listPlants.GetByIndex(index);
}

void CTerrainPlants::Release()
{
	ALISTPOSITION pos = m_listPlants.GetTailPosition();
	while( pos )
	{
		
		PPLANT ptemp = m_listPlants.GetPrev(pos);
		delete ptemp->pData;
		delete ptemp;
	}
	m_listPlants.RemoveAll();
}

void CTerrainPlants::Render(A3DViewport* pA3DViewport)
{
}

void CTerrainPlants::RenderDebug()
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;

	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	if(pTerrain==NULL || pHeightMap == NULL) return;

	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	
	
	ALISTPOSITION pos = m_listPlants.GetTailPosition();
	while( pos )
	{
		PPLANT ptemp = m_listPlants.GetPrev(pos);
		//if(!ptemp->bShow) continue;
		ALISTPOSITION pos2;
		pos2 = ptemp->m_listPlantsPos.GetTailPosition();
		while(pos2)
		{
			PPLANTPOS plant = ptemp->m_listPlantsPos.GetPrev(pos2);
			DrawRect(plant->x,plant->z,A3DCOLORRGB(255,255,255));
		}
	
	}
	
}

void CTerrainPlants::DrawRect(float x,float z,DWORD dwColor)
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	A3DVECTOR3 vPos = A3DVECTOR3(x,0,z);
	float height = pMap->GetTerrain()->GetPosHeight(vPos) + 0.1f;
	
	A3DVECTOR3 pVertices[4];
	WORD pIndices[8];
	float fGridSize = 3.0f;
	pVertices[0] = A3DVECTOR3(x-fGridSize,height,z-fGridSize);
	pVertices[1] = A3DVECTOR3(x+fGridSize,height,z-fGridSize);
	pVertices[2] = A3DVECTOR3(x+fGridSize,height,z+fGridSize);
	pVertices[3] = A3DVECTOR3(x-fGridSize,height,z+fGridSize);

	pIndices[0] = 0; pIndices[1] = 1;
	pIndices[2] = 1; pIndices[3] = 2;
	pIndices[4] = 2; pIndices[5] = 3;
	pIndices[6] = 3; pIndices[7] = 0;

	
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,4,pIndices,8,dwColor);
	}
	
}

/*
void CTerrainPlants::BuildTrees()
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	if(pTerrain==NULL || pHeightMap == NULL) return;

	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	CSceneObjectManager*pMan = pMap->GetSceneObjectMan();
	
	ALISTPOSITION pos = m_listPlants.GetTailPosition();
	
	while( pos )
	{
		PPLANT ptemp = m_listPlants.GetPrev(pos);
		if(!ptemp->bShow) continue;
		int plantWidth = ptemp->pData->GetChunkWidth();
		int plantHeight = ptemp->pData->GetChunkHeight();
		float m_fTileSize = (m_rcTerrain.right - m_rcTerrain.left)/plantWidth;
		CELForest *pForest = pMan->GetElForest();
		CELTree *pTree = pForest->GetTreeType(ptemp->nID);
	    for(int h=0; h<plantHeight; h++)
		{
			for( int w=0; w<plantWidth; w++)
			{
				if(ptemp->pData->GetVaule(w,h)==1)
				{//渲染一点点东西出来
					float posx = w*m_fTileSize+m_rcTerrain.left;
					float posz = m_rcTerrain.top - h*m_fTileSize;
					A3DVECTOR3 vPos = A3DVECTOR3(posx,0,posz);
					float height = pMap->GetTerrain()->GetPosHeight(vPos) + 0.1f;
					vPos.y = height;
					DWORD dwID;
					pTree->AddTree(vPos,dwID);
				}
			}
		}
	}
}
*/

void TREE_PLANT::DeletePlant(float x,float z)
{
	if (!m_pSceneMan)
		return;
	
	CELForest *pForest = m_pSceneMan->GetElForest();
	CELTree *pTree = pForest->GetTreeTypeByID(nID);
	
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();
	ALISTPOSITION oldPos;
	while(pos)
	{
		oldPos = pos;
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		float deltax = abs(ptemp->x - x);
		float deltaz = abs(ptemp->z - z);
		
		if(deltax<0.00001f && deltaz<0.00001f)
		{
			if(pTree->DeleteTree(ptemp->id))
			{
				
				delete ptemp;
				m_listPlantsPos.RemoveAt(oldPos);
				return;
			}
		}
	}
}

void TREE_PLANT::AddPlant(float x, float z)
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();

	CELForest *pForest = m_pSceneMan->GetElForest();
	CELTree *pTree = pForest->GetTreeTypeByID(nID);
	if(pTree==NULL) return;
	A3DVECTOR3 vPos = A3DVECTOR3(x,0,z);
	float height = pMap->GetTerrain()->GetPosHeight(vPos);
	vPos.y = height - 0.1f;
	DWORD dwID;
	pTree->AddTree(vPos,dwID);
	PLANTPOS *pPos = new PLANTPOS;
	pPos->x = x;
	pPos->y = vPos.y;
	pPos->z = z;
	pPos->id = dwID;
	////////////////////////////////////////
	m_listPlantsPos.AddTail(pPos);
}

void TREE_PLANT::AddPlant2(float x, float y, float z,DWORD id)
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();

	PLANTPOS *pPos = new PLANTPOS;
	pPos->x = x;
	pPos->y = y;
	pPos->z = z;
	pPos->id = id;
	////////////////////////////////////////
	m_listPlantsPos.AddTail(pPos);
}

void TREE_PLANT::MovePlant(int id,const A3DVECTOR3& vDelta)
{
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();

	CELForest *pForest = m_pSceneMan->GetElForest();
	CELTree *pTree = pForest->GetTreeTypeByID(nID);

	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();
	while(pos)
	{
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		if(ptemp->id==id)
		{
			ptemp->x +=vDelta.x;
			ptemp->y +=vDelta.y;
			ptemp->z +=vDelta.z;
			pTree->AdjustTreePosition(id,A3DVECTOR3(ptemp->x,ptemp->y,ptemp->z));
		}
	}
}

bool TREE_PLANT::FindPlant(float x,float z)
{
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();
	while(pos)
	{
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		if(abs(ptemp->x - x)<2.0f && abs(ptemp->z - z)<2.0f)
			return true;
	}
	return false;
}

PPLANTPOS TREE_PLANT::GetPlant(float x, float z)
{
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();
	while(pos)
	{
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		if(abs(ptemp->x - x)<0.0001f && abs(ptemp->z - z)<0.0001f)
			return ptemp;
	}
	return NULL;
}

int TREE_PLANT::GetPlantType()
{
	return nPlantType;
}

void TREE_PLANT::DeleteAllPlant()
{
	/*
	if (!m_pSceneMan)
		return;

	CELForest *pForest = m_pSceneMan->GetElForest();
	CELTree *pTree = pForest->GetTreeType(nID);
	
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();

	while(pos)
	{
		
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		pTree->DeleteTree(ptemp->id);
		delete ptemp;
	}
	m_listPlantsPos.RemoveAll();
	*/
}

void TREE_PLANT::DeleteAllPlantFromEngine()
{
	/*
	if (!m_pSceneMan)
		return;

	CELForest *pForest = m_pSceneMan->GetElForest();
	
	CELForest *pForest = pMan->GetElForest();
	CELTree *pTree = pForest->GetTreeType(nID);
	
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();

	while(pos)
	{
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		pTree->DeleteTree(ptemp->id);
	}*/
}

void TREE_PLANT::ReBuildEnginePlant()
{
	/*
	if (!m_pSceneMan || !m_pSceneMan->GetMap())
		return;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	CSceneObjectManager*pMan = pMap->GetSceneObjectMan();
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();

	CELForest *pForest = pMan->GetElForest();
	CELTree *pTree = pForest->GetTreeType(nID);
	
	int plantWidth = pData->GetChunkWidth();
	float m_fTileSize = (m_rcTerrain.right - m_rcTerrain.left)/(float)plantWidth;
	
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();

	while(pos)
	{
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		float posx = ptemp->x*m_fTileSize+m_rcTerrain.left;
		float posz = m_rcTerrain.top - ptemp->z*m_fTileSize;
		A3DVECTOR3 vPos = A3DVECTOR3(posx,0,posz);
		float height = pMap->GetTerrain()->GetPosHeight(vPos);
		vPos.y = height;
		DWORD dwID;
		pTree->AddTree(vPos,dwID);
		PLANTPOS *pPos = new PLANTPOS;
		pPos->id = dwID;
	}
	*/
}

TREE_PLANT::~TREE_PLANT()
{
	ALISTPOSITION pos = m_listPlantsPos.GetTailPosition();
	while( pos )
	{
		
		PPLANTPOS ptemp = m_listPlantsPos.GetPrev(pos);
		delete ptemp;
	}
	m_listPlantsPos.RemoveAll();
};

bool CTerrainPlants::LoadPlants( CELArchive& ar ,DWORD dwVersion)
{
	//2.0以前的版本，少保存了plant->y成员，为了保持以后的版本兼容
	//可以通过取地形相应位置的高度来弥补
	PPLANT pnew = NULL;
	int typenum;
	if( dwVersion < 2)
	{
		 ar.Read(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			AString strTypeName = AString(ar.ReadString());
			pnew = GetPlant(strTypeName);
			int plantnum;
			ar.Read(&plantnum,sizeof(int));
			for(int j=0; j<plantnum; j++)
			{
				PPLANTPOS plant = new PLANTPOS;
				int x,z;
				ar.Read(&x,sizeof(int));
				ar.Read(&z,sizeof(int));
				ar.Read(&plant->id,sizeof(DWORD));
				plant->x = x;
				plant->z = z;
				ASSERT(m_pSceneMan);
				CElementMap *pMap = m_pSceneMan->GetMap();
				plant->y = pMap->GetTerrain()->GetPosHeight(A3DVECTOR3(plant->x,0.f,plant->z));
				if(pnew) pnew->m_listPlantsPos.AddHead(plant);
			}
			pnew = NULL;
		}
	}
	if( dwVersion == 2 )
	{
		ar.Read(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			AString strTypeName = AString(ar.ReadString());
			pnew = GetPlant(strTypeName);
			int plantnum;
			ar.Read(&plantnum,sizeof(int));
			for(int j=0; j<plantnum; j++)
			{
				PPLANTPOS plant = new PLANTPOS;
				float x,y,z;
				ar.Read(&x,sizeof(float));
				ar.Read(&y,sizeof(float));
				ar.Read(&z,sizeof(float));
				ar.Read(&plant->id,sizeof(DWORD));
				plant->x = x;
				plant->y = y;
				plant->z = z;
				if(pnew) pnew->m_listPlantsPos.AddHead(plant);
			}
			pnew = NULL;
		}
	}

	if( dwVersion > 2 )
	{
		ar.Read(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			int nPlantType,nID;
			AString strTypeName = AString(ar.ReadString());
			pnew = GetPlant(strTypeName);
			
			ar.Read(&nPlantType,sizeof(int));
			ar.Read(&nID,sizeof(int));
			
			if(!pnew)
			{
				if(nPlantType == PLANT_GRASS)
				{
					//pnew = new GRASS_PLANT(m_pSceneMan);
					pnew = m_pSceneMan->GetPlants()->AddPlant(strTypeName,nID,PLANT_GRASS);
					if(!pnew) continue;
				}else 
				{
					//pnew = new TREE_PLANT(m_pSceneMan);
					pnew = m_pSceneMan->GetPlants()->AddPlant(strTypeName,nID,PLANT_TREE);
					if(!pnew) continue;
				}
			}
			if(pnew->nPlantType == PLANT_TREE)
			{
				int plantnum;
				ar.Read(&plantnum,sizeof(int));
				for(int j=0; j<plantnum; j++)
				{
					PPLANTPOS plant = new PLANTPOS;
					float x,y,z;
					ar.Read(&x,sizeof(float));
					ar.Read(&y,sizeof(float));
					ar.Read(&z,sizeof(float));
					ar.Read(&plant->id,sizeof(DWORD));
					plant->x = x;
					plant->y = y;
					plant->z = z;
					if(pnew) pnew->m_listPlantsPos.AddHead(plant);
				}
			}else
			{
				ar.Read(&((GRASS_PLANT*)pnew)->m_nFlagsNum,sizeof(int));
				for(int j = 0; j < ((GRASS_PLANT*)pnew)->m_nFlagsNum; j++)
				{
					int iFlag;
					ar.Read(&iFlag, sizeof(int));
					((GRASS_PLANT*)pnew)->SetMaskFlag(j, iFlag);
				}
			}
			pnew = NULL;
		}
	}
	return true;
}

bool CTerrainPlants::SavePlants( CELArchive& ar ,DWORD dwVersion)
{
	int typenum = m_listPlants.GetCount();
	if(dwVersion<2)
	{
		ar.Write(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			PPLANT ptemp = m_listPlants.GetByIndex(i);
			ar.WriteString(ptemp->strName);
			int plantnum = ptemp->m_listPlantsPos.GetCount();
			ar.Write(&plantnum,sizeof(int));
			for( int j=0; j<plantnum; j++)
			{
				PPLANTPOS plant = ptemp->m_listPlantsPos.GetByIndex(j);
				ar.Write(&plant->x,sizeof(int));
				ar.Write(&plant->z,sizeof(int));
				ar.Write(&plant->id,sizeof(DWORD));
			}
		}
	}
	if(dwVersion==2)
	{
		ar.Write(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			PPLANT ptemp = m_listPlants.GetByIndex(i);
			ar.WriteString(ptemp->strName);
			int plantnum = ptemp->m_listPlantsPos.GetCount();
			ar.Write(&plantnum,sizeof(int));
			for( int j=0; j<plantnum; j++)
			{
				PPLANTPOS plant = ptemp->m_listPlantsPos.GetByIndex(j);
				ar.Write(&plant->x,sizeof(float));
				ar.Write(&plant->y,sizeof(float));
				ar.Write(&plant->z,sizeof(float));
				ar.Write(&plant->id,sizeof(DWORD));
			}
		}
	}

	if(dwVersion > 2)
	{
		ar.Write(&typenum,sizeof(int));
		for(int i=0; i<typenum; i++)
		{
			PPLANT ptemp = m_listPlants.GetByIndex(i);
			ar.WriteString(ptemp->strName);
			ar.Write(&ptemp->nPlantType,sizeof(int));
			ar.Write(&ptemp->nID,sizeof(int));
			if(ptemp->nPlantType == PLANT_TREE)
			{
				int plantnum = ptemp->m_listPlantsPos.GetCount();
				ar.Write(&plantnum,sizeof(int));
				for( int j=0; j<plantnum; j++)
				{
					PPLANTPOS plant = ptemp->m_listPlantsPos.GetByIndex(j);
					ar.Write(&plant->x,sizeof(float));
					ar.Write(&plant->y,sizeof(float));
					ar.Write(&plant->z,sizeof(float));
					ar.Write(&plant->id,sizeof(DWORD));
				}
			}else
			{
				ar.Write(&(((GRASS_PLANT*)ptemp)->m_nFlagsNum),sizeof(int));
				for(int j =0; j<((GRASS_PLANT*)ptemp)->m_nFlagsNum; j++)
				{
					ar.Write(&(((GRASS_PLANT*)ptemp)->m_listMaskFlags[j]),sizeof(int));
				}
			}
		}
	}
	return true;
}

//对于场景文件版本5及以前的PLANT_TREE文件
void CTerrainPlants::RepairPlant_5()
{
	
	CELForest *pForest = m_pSceneMan->GetElForest();
	int typenum = m_listPlants.GetCount();
	
	//删除所有的SpeedTree接口中的数据
	for(int i = 0; i<typenum; i++)
	{
		PPLANT ptemp = m_listPlants.GetByIndex(i);
		if(ptemp->GetPlantType() == PLANT_TREE)
		{
			CELTree *pTree = pForest->GetTreeTypeByID(ptemp->nID);
			int plantnum = ptemp->m_listPlantsPos.GetCount();
			for(int j = 0; j< plantnum; j++)
			{
				PPLANTPOS plant = ptemp->m_listPlantsPos.GetByIndex(j);
				pTree->DeleteTree(plant->id);
			}
		}
	}
	
	//重新创建所有SpeedTree的数据
	for(i = 0; i<typenum; i++)
	{
		PPLANT ptemp = m_listPlants.GetByIndex(i);
		if(ptemp->GetPlantType() == PLANT_TREE)
		{
			CELTree *pTree = pForest->GetTreeTypeByID(ptemp->nID);
			int plantnum = ptemp->m_listPlantsPos.GetCount();
			for(int j = 0; j< plantnum; j++)
			{
				PPLANTPOS plant = ptemp->m_listPlantsPos.GetByIndex(j);
				DWORD dwID;
				A3DVECTOR3 vPos(plant->x,plant->y,plant->z);
				pTree->AddTree(vPos,dwID);
				plant->id = dwID;
			}
		}
	}
}


//------------------------------------------------------------------
//Grass class
//------------------------------------------------------------------

void GRASS_PLANT::DeletePlant(float x,float z)
{
}

void GRASS_PLANT::AddPlant(float x, float z)
{
}

bool GRASS_PLANT::FindPlant(float x,float z)
{
	return true;
}

int GRASS_PLANT::GetPlantType()
{
	return nPlantType;
}

void GRASS_PLANT::DeleteAllPlant()
{
}

void GRASS_PLANT::DeleteAllPlantFromEngine()
{
}

void GRASS_PLANT::ReBuildEnginePlant()
{
}

void GRASS_PLANT::SetSize(int w,int h)
{
	if(pData) pData->Release();
	pData->SetSize(w,h);
}

GRASS_PLANT::~GRASS_PLANT()
{
	if(pData) delete pData;
	if(m_listMaskFlags) delete []m_listMaskFlags;
};

void GRASS_PLANT::Init(int nMaskGridNum)
{
	m_nFlagsNum = nMaskGridNum;
	m_listMaskFlags = new int[m_nFlagsNum];
	for(int i=0; i<m_nFlagsNum; i++)
		m_listMaskFlags[i] = -1;
}

void GRASS_PLANT::SetMaskFlag(int maskId,int mapId)
{
	if (!m_listMaskFlags || maskId==-1)
		return;

	ASSERT(maskId<m_nFlagsNum);
	m_listMaskFlags[maskId] = mapId;
}

int GRASS_PLANT::GetMaskFlag(int maskId)
{
	if (!m_listMaskFlags)
		return -1;

	ASSERT(maskId<m_nFlagsNum);
	return m_listMaskFlags[maskId];
}
