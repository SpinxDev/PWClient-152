//----------------------------------------------------------
// Filename	: SceneObjectManager.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: ����༭�����������ж���
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


#define  FS_VERSION 0x41				//(0-3d)���������ļ��汾��  >0x3dʱ.scene�лָ��洢��̬ģ����Ϣ
#define  LIT_MODEL_VERSION 0x7	//(0-7)	//LIT_MODEL_VERSION<0x7ʱ,  
										//LIT_MODEL_VERSION=0x7ʱ,��̬ģ����Ϣ�ִ洢����.scene,���ݴ洢��.smod�ļ�



//Ϊ���������������ID
class CIDGenerator
{
public:
	CIDGenerator(){ m_nMax=0; };
	~CIDGenerator(){};
	DWORD GenerateID(){ return ++m_nMax; };
	void  AddID(DWORD id){ if(id>m_nMax) m_nMax = id; };
private:
	DWORD m_nMax;//�Ѿ����������ID����������Ҫ����
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

	//���¹���
	bool					UpdateLighting(bool bIsNight);

	//��һ���Ѿ����ڵĶ������·ŵ�������
	void					AddSceneObject(CSceneObject *pObj);

	//��һ�������ɹ������Լ������Ķ������÷��뵽������
	void					InsertSceneObject(CSceneObject *pObj);
	
	//������ɾ��һ������PTR,���ͷ�ռ���ڴ�
	void					DeleteSceneObjectForever(CSceneObject *pObj);
	
	//ɾ������ͨ����������֣�����ӿڲ�Ҫ��,��ר�Ų���UNDO
	//��������Զ������ɾ�������ͷ��ڴ�
	void					DeleteSceneObject(const AString& name);
	void					DeleteSceneObject(CSceneObject *pObj);
	
	//�ͷ����ж���
	void					Release();
	
	//����һ���ƹ�
	CSceneLightObject *		CreateLight(LIGHTDATA &lightData);

	//����һ��B���߶���
	CEditerBezier *			CreateBezier(const AString& name);

	//����һ���������
	CSceneBoxArea *			CreateBoxArea(const AString& name,bool bIsDefaut = false);

	//����һ��Ĭ��ˮ�飬���Ҫ�ı��������ԣ�ͨ�����Ա��޸�
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
	
	//�ڼ��ص������еĶ������Ѱ��һ�����󣬶�����������ⳡ������
	CSceneObject *			FindSceneObject(const AString& name);
	
	//�ж�һ����̬ģ�Ͷ����Ƿ��Ѿ����뵽����
	bool                    IsBuildingObjectInScene(const AString& name);
	
	//�����д����ĵƹ���в��ҵƹ⣬���ǵƹⲻһ���Ѿ����뵽������
	CSceneObject *			FindLightObject(const AString& name);
	
	//	Find a bezier route object with specified ID
	CEditerBezier*			FindBezier(int iID);
	
	CSceneObject*			FindBuilding(int iID);

	//���ѡ������
	void					ClearSelectedList();

	//���߸��ٳ������bAdd==TRUE,������еĶ�����뵽ѡ���
	bool					RayTrace(A3DVECTOR3& vStart, A3DVECTOR3& vEnd,bool bAdd = true);
	bool                    GetTracePos(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& inter);
	//ѡȡһ����Ļ�����ڵĶ���
	void					SelectArea(ARectI &rc);

	//��һ�����������ָ����뵽ѡ���б��У���ʾ�ö����Ѿ���ѡ
	void					AddObjectPtrToSelected(CSceneObject * pObj);

	//ȡ����պ��ӵ�ָ��
	A3DSkySphere*			GetSky(){ return m_pA3DSky; };
	void					SetSky(A3DSkySphere* pSky){ m_pA3DSky = pSky; };

	//ȡ��ֲ���༭�ӿ�
	CTerrainPlants*			GetPlants(){ return m_pPlants; };

	//ȡ��ֲ��SPEED TREE����ӿ�
	CELForest *				GetElForest(){ return m_pForest; };

	//ȡ�òݵؽӿ�
	CELGrassLand *			GetElGrassLand(){ return m_pGrassLand; }

	//�༭������̲���
	bool					SaveAllObject(CELArchive& ar);
	bool					LoadAllObject(CELArchive& ar,int iLoadFlags);

	//Save lit model
	bool                    SaveLitModels(const char *szFile);
	bool                    LoadLitModels(const char *szFile, int iLoadFlags);
	void                    ReloadLitModels();
	
	//Speed tree����ӿڵĴ���
	bool					SaveForest(const char *szFile);
	bool					LoadForest(const char *szFile);

	//Grassland�ӿڵĴ����
	bool					SaveGrassLand(const char *szFile);
	bool					LoadGrassLand(const char *szFile);

	bool                    SaveWaterMap(const char *szFile);

	AMSoundStream*			GetMusicInterface(){ return m_pBackSound; };

	//	Get map object
	CElementMap*			GetMap() { return m_pMap; }

	//ȫ��Ψһһ������ߴ�ָʾ��
	CSceneVernier *			GetVernier(){ return m_pVernier; };

	//ȡ�øõ�ͼΨһһ��Ĭ������
	CSceneBoxArea *			GetDefaultBoxArea();

	//ȡ���������ǰ���ڵ�����
	CSceneBoxArea *			GetCurrentArea(){ return m_pCurrentArea; };
	void					SetCurrentArea(CSceneBoxArea* pCur) { m_pCurrentArea = pCur; };
    
	CScenePrecinctObject*   GetCurrentPrecinct(){ return m_pCurrentPrecinct; };
	void                    SetCurrentPrecinct(CScenePrecinctObject *pCur);
	//ȡ�õ����ϰ��ӿ�
	CTerrainObstruct*       GetTerrainObstruct(){ return m_pTerrainObstruct; };
	
	bool                    IsCameraUnderWater(){ return m_bCamUnderWater; };
	void                    SetCameraUnderWater(bool bUnder){ m_bCamUnderWater = bUnder; };
	
	//���Bezier��������֮��Ĺ���
	void                    CutLink_Bezier(int bezierID);
	//���Dummy���Ͻ����֮��Ĺ���
	void                    CutLink_Precinct_Dummy(int dummyID);
	////////****protperty****//////////////////////
	
	//���б�ѡ��Ķ�����б�
	APtrList<CSceneObject *>	m_listSelectedObject;
	
	//���볡���е��������Ͷ����б�
	APtrList<CSceneObject *>	m_listSceneObject;
	
	//����ָʾ�������������ж�����ͬһ������ָʾ�������ÿ��������Ⱦʱ
	//��Ҫ���³�ʼ������ָʾ����
	CCoordinateDirection	m_coordinateDirection;
	
	int                     GenerateObjectID();

	int						GetSortObjectNum(int nObjectType);

	bool					ExportConvexHullData(const char *szFile);
	bool					ImportConvexHullData(const char *szFile);
	
	APtrList<CSceneObject *>* GetSortObjectList(int sort);
	
	void    PlayMusic();
	
	void    StopMusic();

	//ȫ��ID������������ID�������ʱ������ȥ�����
	CIDGenerator			m_IDGenerator;
	
	//�����Ϊ����������ͬ��Ŀ֮��ת����Դ���������ı�ģ�͵�·����ͬʱ�����µ�·���¿�����ģ�ͺ�������Դ
	void ExportBuilding(); 
protected:
	void	DeleteAllLight();
	bool    IsPickObject(CSceneObject *pObj);
	bool	IsNonNPCPath(CSceneObject *pObj);
	bool	IsShowPath(CSceneObject *pObj);
	
	void    DrawTerrainBlockRect();
	void    DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	
	
	
	/*
	��������ݲ�һ�����ڳ����У������������ڳ����У���
	�����Ѿ���ɾ���������UndoMan�У������ͷŹ�����UndoMan������
	��ĳ��Undo��ȡ���������ͷ������ָ��
	*/
	//�ƹ�����
	APtrList<CSceneObject *> m_listLightObject;
	//ˮ�������
	APtrList<CSceneObject *> m_listWaterObject;
	//��̬ģ�Ͷ���
	APtrList<CSceneObject *> m_listStaticModelObject;
	//��������
	APtrList<CSceneObject *> m_listDataBezier;
	//��������б�
	APtrList<CSceneObject *> m_listDataBoxArea;
	//3d��Ч�б�
	APtrList<CSceneObject *> m_listAudio;
	//��Ԫ�����
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

	//���ֽӿ�
	AMSoundStream*	m_pBackSound;	
	CELBackMusic    m_BackMusic;

	//����������
	A3DSkySphere*   m_pA3DSky;			//	Sky object

	//ֲ������
	CTerrainPlants* m_pPlants;          // Plants object 
	CELForest* m_pForest;				//�����е�ɭ�ֽӿ�
	CELGrassLand * m_pGrassLand;		//�����еĲݵؽӿ�

	//��������������Ψһ��һ���α꣬�ö����ô���
	//������ѡ��Ȳ���
	CSceneVernier *m_pVernier;

	CSceneBoxArea *m_pCurrentArea;

	CScenePrecinctObject *m_pCurrentPrecinct;

	//�����ϰ��ӿڣ�������ʾ�ͱ༭�����ϰ����
	CTerrainObstruct *m_pTerrainObstruct;  

	bool           m_bCamUnderWater;
	int            m_nObjectIDFlag;
	DWORD          m_dwVssFileVersion;
};

#endif // !defined(AFX_SCENEOBJECTMANAGER_H__3077A395_96EA_491D_B77F_56661C1C5C2D__INCLUDED_)
