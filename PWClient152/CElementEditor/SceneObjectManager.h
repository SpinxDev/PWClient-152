//----------------------------------------------------------
// Filename	: SceneObjectManager.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 管理编辑器场景中所有对象
//-----------------------------------------------------------
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECTMANAGER_H__3077A395_96EA_491D_B77F_56661C1C5C2D__INCLUDED_)
#define AFX_SCENEOBJECTMANAGER_H__3077A395_96EA_491D_B77F_56661C1C5C2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoordinateDirection.h"
#include "A3DTypes.h"
#include "A3DSky.h"
#include "A3DSkySphere.h"
#include "A3DViewport.h"
#include "AList2.h"
#include "EL_BackMusic.h"

#include "CDWithCH.h"
using namespace CHBasedCD;

#include "vector.h"
#include "ModelContainer.h"

class CElementMap;
class AMSoundStream;
class CTerrainPlants;

class CEditerBezier;
class CSceneAIGenerator;
class CSceneAudioObject;
class CSceneBoxArea;
class CSceneDummyObject;
class CSceneLightObject;
class CSceneObject;
class CSceneSpeciallyObject;
class CSceneVernier;
class CSceneWaterObject;
class CStaticModelObject;
class CELForest;
class CELGrassLand;
class CELArchive;
class CSceneSkinModel;
class CCritterGroup;
class CSceneFixedNpcGenerator;
class CTerrainObstruct;
class CScenePrecinctObject;
class CSceneGatherObject;
class CSceneRangeObject;
class CCloudBox;
class SceneInstanceBox;
class CSceneDynamicObject;





struct LIGHTDATA;
struct GFX_AUDIO_DATA;


#define  FS_VERSION 0x41				//(0-3d)场景对象文件版本号  >0x3d时.scene中恢复存储静态模型信息
#define  LIT_MODEL_VERSION 0x7	//(0-7)	//LIT_MODEL_VERSION<0x7时,  
										//LIT_MODEL_VERSION=0x7时,静态模型信息又存储到了.scene,数据存储在.smod文件



//为整个场景对象产生ID
class CIDGenerator
{
public:
	CIDGenerator(){ m_nMax=0; };
	~CIDGenerator(){};
	DWORD GenerateID(){ return ++m_nMax; };
	void  AddID(DWORD id){ if(id>m_nMax) m_nMax = id; };
private:
	DWORD m_nMax;//已经产生的最大ID，该数据需要存盘
};

PESTATICMODEL				Aux_GetStaticModelFromTempMap(unsigned long id);

class CSceneObjectManager  
{
public:

	CSceneObjectManager(CElementMap* pMap);
	virtual ~CSceneObjectManager();

	// Added by He wenfeng, 05-3-26
	bool SphereTraceStaticModel(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData, bool bExactCD=false);
	bool RayTraceStaticModel(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData, bool bExactCD=false);
	
	// Added by wenfeng, 05-09-02
	// Get all convex hulls in the scene.
	// This interface is for computing the space passable octree.
	void GetAllConvexHulls(abase::vector<CConvexHullData *>& CHs);

	virtual void			Tick(DWORD dwTimeDelta);
	virtual void			RenderSky(A3DViewport* pA3DViewport);
	virtual void			RenderObjects(A3DViewport* pA3DViewport);
	virtual void			RenderReflectObjects(A3DViewport* pA3DViewport);
	virtual void			RenderRefractObjects(A3DViewport* pA3DViewport);
	virtual void			RenderForLightMapShadow(A3DViewport* pA3DViewport);
	virtual void            RenderForLight(A3DViewport* pA3DViewport,float fOffsetX,float fOffsetZ);

	//	Mouse action handler,
	virtual bool			OnMouseMove(int x, int y, DWORD dwFlags); 
	virtual bool			OnLButtonDown(int x, int y, DWORD dwFlags) { return true; };
	virtual bool			OnRButtonDown(int x, int y, DWORD dwFlags) { return true; };
	virtual bool			OnLButtonUp(int x, int y, DWORD dwFlags) { return true; };
	virtual bool			OnRButtonUp(int x, int y, DWORD dwFlags) { return true; };
	virtual bool			OnLButtonDbClk(int x, int y, DWORD dwFlags) { return true; };
	virtual bool			OnRButtonDbClk(int x, int y, DWORD dwFlags) { return true; };

	//更新光照
	bool					UpdateLighting(bool bIsNight);

	//把一个已经存在的对象重新放到场景中
	void					AddSceneObject(CSceneObject *pObj);

	//把一个不是由管理器自己创建的对象永久放入到场景中
	void					InsertSceneObject(CSceneObject *pObj);
	
	//永久性删除一个对象PTR,将释放占有内存
	void					DeleteSceneObjectForever(CSceneObject *pObj);
	
	//删除对象通过对象的名字，这个接口不要用,有专门操作UNDO
	//本身并不会对对象进行删除，不释放内存
	void					DeleteSceneObject(const AString& name);
	void					DeleteSceneObject(CSceneObject *pObj);
	
	//释放所有对象
	void					Release();
	
	//创建一个灯光
	CSceneLightObject *		CreateLight(LIGHTDATA &lightData);

	//创建一条B曲线对象
	CEditerBezier *			CreateBezier(const AString& name);

	//创建一个区域对象
	CSceneBoxArea *			CreateBoxArea(const AString& name,bool bIsDefaut = false);

	//创建一个默认水块，如果要改变它的属性，通过属性表修改
	CSceneWaterObject *		CreateWater(DWORD id,const AString& name);

	CSceneAudioObject *		CreateAudio(const AString& name,GFX_AUDIO_DATA data);

	CSceneDummyObject*      CreateDummy(const AString& name);

	CSceneSpeciallyObject*  CreateSpecially(const AString& name);

	CSceneAIGenerator*      CreateAIGenerator(const AString& name);

	CCritterGroup*          CreateCritterGroup(const AString& name);

	CSceneFixedNpcGenerator*CreateFixedNpc(const AString& name);

	CScenePrecinctObject   *CreatePrecinct(const AString& name);

	CSceneGatherObject     *CreateGather(const AString& name);

	CSceneRangeObject      *CreateRange(const AString& name);
	
	CCloudBox              *CreateCloudBox(const AString& name);

	SceneInstanceBox      *CreateInstanceBox(const AString& name);  

	CSceneDynamicObject   *CreateDynamic(const AString& name);

	// Exp and emp ai data
	bool ExpAiData(AFile *pFile);
	bool ImpAiData(AFile *pFile);
	
	//在加载到场景中的对象表中寻找一个对象，对象可以是任意场景类型
	CSceneObject *			FindSceneObject(const AString& name);
	
	//判断一个静态模型对象是否已经放入到场景
	bool                    IsBuildingObjectInScene(const AString& name);
	
	//从所有创建的灯光表中查找灯光，但是灯光不一定已经放入到场景中
	CSceneObject *			FindLightObject(const AString& name);
	
	//	Find a bezier route object with specified ID
	CEditerBezier*			FindBezier(int iID);
	
	CSceneObject*			FindBuilding(int iID);

	//清除选择对象表
	void					ClearSelectedList();

	//光线跟踪程序，如果bAdd==TRUE,会把命中的对象放入到选择表
	bool					RayTrace(A3DVECTOR3& vStart, A3DVECTOR3& vEnd,bool bAdd = true);
	bool                    GetTracePos(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& inter);
	//选取一个屏幕区域内的对象
	void					SelectArea(ARectI &rc);

	//把一个场景对象的指针加入到选择列表中，表示该对象已经被选
	void					AddObjectPtrToSelected(CSceneObject * pObj);

	//取得天空盒子的指针
	A3DSkySphere*			GetSky(){ return m_pA3DSky; };
	void					SetSky(A3DSkySphere* pSky){ m_pA3DSky = pSky; };

	//取得植被编辑接口
	CTerrainPlants*			GetPlants(){ return m_pPlants; };

	//取得植被SPEED TREE引擎接口
	CELForest *				GetElForest(){ return m_pForest; };

	//取得草地接口
	CELGrassLand *			GetElGrassLand(){ return m_pGrassLand; }

	//编辑对象存盘操作
	bool					SaveAllObject(CELArchive& ar);
	bool					LoadAllObject(CELArchive& ar,int iLoadFlags);

	//Save lit model
	bool                    SaveLitModels(const char *szFile);
	bool                    LoadLitModels(const char *szFile, int iLoadFlags);
	void                    ReloadLitModels();
	
	//Speed tree引擎接口的存盘
	bool					SaveForest(const char *szFile);
	bool					LoadForest(const char *szFile);

	//Grassland接口的存读盘
	bool					SaveGrassLand(const char *szFile);
	bool					LoadGrassLand(const char *szFile);

	bool                    SaveWaterMap(const char *szFile);

	AMSoundStream*			GetMusicInterface(){ return m_pBackSound; };

	//	Get map object
	CElementMap*			GetMap() { return m_pMap; }

	//全局唯一一个坐标尺寸指示器
	CSceneVernier *			GetVernier(){ return m_pVernier; };

	//取得该地图唯一一个默认区域
	CSceneBoxArea *			GetDefaultBoxArea();

	//取得摄像机当前所在的区域
	CSceneBoxArea *			GetCurrentArea(){ return m_pCurrentArea; };
	void					SetCurrentArea(CSceneBoxArea* pCur) { m_pCurrentArea = pCur; };
    
	CScenePrecinctObject*   GetCurrentPrecinct(){ return m_pCurrentPrecinct; };
	void                    SetCurrentPrecinct(CScenePrecinctObject *pCur);
	//取得地形障碍接口
	CTerrainObstruct*       GetTerrainObstruct(){ return m_pTerrainObstruct; };
	
	bool                    IsCameraUnderWater(){ return m_bCamUnderWater; };
	void                    SetCameraUnderWater(bool bUnder){ m_bCamUnderWater = bUnder; };
	
	//解除Bezier与生物组之间的关联
	void                    CutLink_Bezier(int bezierID);
	//解除Dummy与管辖区域之间的关联
	void                    CutLink_Precinct_Dummy(int dummyID);
	////////****protperty****//////////////////////
	
	//所有被选择的对象的列表
	APtrList<CSceneObject *>	m_listSelectedObject;
	
	//放入场景中的所有类型对象列表
	APtrList<CSceneObject *>	m_listSceneObject;
	
	//坐标指示器，场景中所有对象都用同一个坐标指示器，因此每个对象渲染时
	//都要重新初始化坐标指示对象
	CCoordinateDirection	m_coordinateDirection;
	
	int                     GenerateObjectID();

	int						GetSortObjectNum(int nObjectType);

	bool					ExportConvexHullData(const char *szFile);
	bool					ImportConvexHullData(const char *szFile);
	
	APtrList<CSceneObject *>* GetSortObjectList(int sort);
	
	void    PlayMusic();
	
	void    StopMusic();

	//全局ID发生器，凡是ID对象读盘时都必须去做标记
	CIDGenerator			m_IDGenerator;
	
	//这个是为了在两个不同项目之间转换资源而作，其会改变模型的路径，同时会在新的路径下拷贝份模型和纹理资源
	void ExportBuilding(); 
protected:
	void	DeleteAllLight();
	bool    IsPickObject(CSceneObject *pObj);
	bool	IsNonNPCPath(CSceneObject *pObj);
	bool	IsShowPath(CSceneObject *pObj);
	
	void    DrawTerrainBlockRect();
	void    DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	
	
	
	/*
	下面的数据不一定存在场景中，它可能曾经在场景中，但
	现在已经被删除，存放在UndoMan中，它的释放工作由UndoMan来做，
	当某个Undo被取消后，他将释放这儿的指针
	*/
	//灯光数据
	APtrList<CSceneObject *> m_listLightObject;
	//水块的数据
	APtrList<CSceneObject *> m_listWaterObject;
	//静态模型对象
	APtrList<CSceneObject *> m_listStaticModelObject;
	//曲线数据
	APtrList<CSceneObject *> m_listDataBezier;
	//区域对象列表
	APtrList<CSceneObject *> m_listDataBoxArea;
	//3d音效列表
	APtrList<CSceneObject *> m_listAudio;
	//哑元对象表
	APtrList<CSceneObject *> m_listDummy;

	APtrList<CSceneObject *> m_listSpecially;

	APtrList<CSceneObject *> m_listAIGenerator;

	APtrList<CSceneObject *> m_listSkinModel;

	APtrList<CSceneObject *> m_listCritterGroup;

	APtrList<CSceneObject *> m_listFixedNpc;

	APtrList<CSceneObject *> m_listPrecinct;

	APtrList<CSceneObject *> m_listGatherObject;

	APtrList<CSceneObject *> m_listRange;

	APtrList<CSceneObject *> m_listCloudBox;

	APtrList<CSceneObject *> m_listInstanceBox;

	APtrList<CSceneObject *> m_listDynamic;

	CElementMap*	m_pMap;				//	Map object

	//音乐接口
	AMSoundStream*	m_pBackSound;	
	CELBackMusic    m_BackMusic;

	//这儿放天空体
	A3DSkySphere*   m_pA3DSky;			//	Sky object

	//植被数据
	CTerrainPlants* m_pPlants;          // Plants object 
	CELForest* m_pForest;				//场景中的森林接口
	CELGrassLand * m_pGrassLand;		//场景中的草地接口

	//场景管理器中有唯一的一个游标，该对象不用存盘
	//不能做选择等操作
	CSceneVernier *m_pVernier;

	CSceneBoxArea *m_pCurrentArea;

	CScenePrecinctObject *m_pCurrentPrecinct;

	//地形障碍接口，用于显示和编辑地形障碍情况
	CTerrainObstruct *m_pTerrainObstruct;  

	bool           m_bCamUnderWater;
	int            m_nObjectIDFlag;
	DWORD          m_dwVssFileVersion;
};

#endif // !defined(AFX_SCENEOBJECTMANAGER_H__3077A395_96EA_491D_B77F_56661C1C5C2D__INCLUDED_)
