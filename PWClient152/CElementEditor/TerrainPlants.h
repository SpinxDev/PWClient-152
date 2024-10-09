// TerrainPlants.h: interface for the CTerrainPlants class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAINPLANTS_H__78A5A206_677C_439E_A601_937CDD5B05C9__INCLUDED_)
#define AFX_TERRAINPLANTS_H__78A5A206_677C_439E_A601_937CDD5B05C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PLANT_TYPES 8 //这儿暂时设为8层
#define PLANTS_DEBUG
#include "BitChunk.h"
#include "AList2.h"
#include "EL_Archive.h"
#include "EL_GrassType.h"

class A3DViewport;
class CSceneObjectManager;

enum
{
	PLANT_TREE = 0,
	PLANT_GRASS,
	PLANT_NUM,
};


typedef struct PLANTPOS
{
	float x;
	float y;
	float z;
	DWORD id;//植物本身在FOREST中的ID
}* PPLANTPOS;


typedef class PLANT
{
public:

	PLANT(CSceneObjectManager* pSceneMan)
	{ 
		m_pSceneMan = pSceneMan;
		pData = 0;
	}

	virtual ~PLANT(){};

	CSceneObjectManager*	m_pSceneMan;

	AString		strName;
	int			nID;//该类型的植物的ID
	int			density;
	int			minDistance;//植物之间的最小距离
	CBitChunk	*pData;//这儿存放该植被的数据
	bool		bShow;
	bool        bInit;//是否初始化BitChunk
	int         nPlantType;
	//Model point
	APtrList<PPLANTPOS> m_listPlantsPos;

	virtual void DeletePlant(float x,float z) = 0;
	virtual void AddPlant(float x, float z) = 0;
	virtual void MovePlant(int id,const A3DVECTOR3& vDelta){};

	virtual void DeleteAllPlant() = 0;

	virtual void DeleteAllPlantFromEngine() = 0; //仅仅删掉显示部分
	virtual void ReBuildEnginePlant() = 0; //重新建立要显示的部分

	virtual bool FindPlant(float x,float z) = 0;//

	virtual int  GetPlantType() = 0;
}* PPLANT;

class TREE_PLANT :public PLANT
{

public:

	TREE_PLANT(CSceneObjectManager* pSceneMan) : PLANT(pSceneMan)
	{ 
		bShow = true; 
		bInit = false; 
		nPlantType = PLANT_TREE;
	}

	virtual ~TREE_PLANT();

	virtual void DeletePlant(float x,float z);
	virtual void AddPlant(float x, float z);
	virtual void MovePlant(int id,const A3DVECTOR3& vDelta);
	virtual void DeleteAllPlant();
	virtual void DeleteAllPlantFromEngine();
	virtual void ReBuildEnginePlant();
	virtual bool FindPlant(float x,float z);
	virtual int  GetPlantType();

	int GetPlantNum(){ return m_listPlantsPos.GetCount(); };
	PPLANTPOS GetPlantByIndex(int index){ return m_listPlantsPos.GetByIndex(index); };
	
	PPLANTPOS GetPlant(float x, float z);
	void AddPlant2(float x, float y, float z,DWORD id);
};

class GRASS_PLANT :public PLANT
{
	public:

	GRASS_PLANT(CSceneObjectManager* pSceneMan) : PLANT(pSceneMan)
	{ 
		bShow = true; 
		bInit = false;
		pData = NULL;
		density =10;
		m_pGrassType = NULL;
		nPlantType = PLANT_GRASS;
		m_listMaskFlags = NULL;
		m_nFlagsNum = 0;
	}

	virtual ~GRASS_PLANT();

	virtual void DeletePlant(float x,float z);
	virtual void AddPlant(float x, float z);
	virtual void DeleteAllPlant();
	virtual void DeleteAllPlantFromEngine();
	virtual void ReBuildEnginePlant();
	virtual bool FindPlant(float x,float z);
	virtual int  GetPlantType();
	void		 SetSize(int w, int h);
	void          SetGrassType(CELGrassType *pGrassType){ m_pGrassType = pGrassType;};
	CELGrassType* GetGrassType(){ return m_pGrassType; };
	
	void         Init(int nMaskGridNum);
	void         SetMaskFlag(int maskId,int mapId = -1);
	int          GetMaskFlag(int maskId);
	int          GetFlagNum(){ return m_nFlagsNum; };

	CELGrassType *m_pGrassType;
	int *m_listMaskFlags;
	int m_nFlagsNum;
};


class CTerrainPlants  
{

public:
	CTerrainPlants(CSceneObjectManager* pSceneMan);
	virtual ~CTerrainPlants();

	void Render(A3DViewport* pA3DViewport);
	
	PPLANT AddPlant(AString name,int nID,int nType);
	void DeletePlant(AString name);

	//void BuildTrees();

	PPLANT GetPlant(const AString& name);
	int    GetPlantNum(){ return m_listPlants.GetCount(); };
	PPLANT GetPlantByIndex(int index);

	bool LoadPlants( CELArchive& ar ,DWORD dwVersion);
	bool SavePlants( CELArchive& ar ,DWORD dwVersion);
	
	void Release();

	//	Get plant list
	APtrList<PPLANT>& GetPlantList() { return m_listPlants; }
	
	//对于场景文件版本5及以前的PLANT_TREE文件
	void RepairPlant_5();

private:
	void RenderDebug();
	void DrawRect(float x,float z,DWORD dwColor);//for debug

	CSceneObjectManager*	m_pSceneMan;

	//所有的植被列表
	APtrList<PPLANT> m_listPlants;
};

#endif // !defined(AFX_TERRAINPLANTS_H__78A5A206_677C_439E_A601_937CDD5B05C9__INCLUDED_)
