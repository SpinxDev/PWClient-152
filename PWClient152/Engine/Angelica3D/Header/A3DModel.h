/*
 * FILE: A3DModel.h
 *
 * DESCRIPTION: model is composed of frame and child model
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMODEL_H_
#define _A3DMODEL_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include "A3DFrame.h"
#include "AList.h"
#include "A3DTrace.h"

#include "AM3DSoundBuffer.h"

#define A3D_GROUP_ACTION_MAX_ELEMENT	6

class A3DFrame;
class A3DTexture;
class A3DDevice;
class AM3DSoundBuffer;
class A3DMaterial;
class A3DViewport;
class A3DGraphicsFX;
class A3DModel;

//	Action
struct A3DACTION
{
	char    szName[32];
	int		nAnimStart;
	int		nAnimEnd;
	bool	bAnimLoop;
};

//	Group Action
struct A3DGROUPACTION
{
	char		szName[32];
	int			nActionNum;
	A3DACTION*	pActionElement[A3D_GROUP_ACTION_MAX_ELEMENT];
};

typedef void (*ACTION_CHANGE_CALLBACK)(A3DACTION* pNextAction, LPVOID pArg);
typedef void (*ACTION_LOOPEND_CALLBACK)(A3DModel* pModel, LPVOID pArg); // This callback will occur when an action loop's ending point has been reached;

//	pLogicEvent is address of a A3DModel::LOGICEVENT object
typedef void (*LOGIC_EVENT_CALLBACK)(void* pLogicEvent, LPVOID pArg);

class A3DModel : public A3DObject
{
	friend class A3DModelMan;
	friend class A3DModelCollectorElement; // Garbage collector element, that will release model
	friend class A3DParticleSystem; // For superspray may spray models, and when reset, it can not 
									// use A3DModelMan to release model, so it must call A3DModel::Release directly
public:		//	Types

	enum
	{
		PROPERTY_SIZE	= 32,
	};

	//	A3DModel's bounding box element;
	struct MODELOBB
	{
		A3DFrame*		pFrame;		//	Used when update a transform matrix from the frame;
		A3DMATRIX4		tm;			//	Used when to calculate OBB in world space;
		A3DOBB			obb;		//	Transformed world space OBB;
		A3DFRAMEOBB		frameOBB;	//	frame's OBB element's copy;
		OBBBEVELS		Bevels;		//	Bevels used to do AABB trace
	};

	//	An event that model will automatically create and start playing a gfx;
	//	we can use this for create open fire and walk dust effects automatically 
	//	all this kind of effects will be loaded when this model loaded...
	struct GFXEVENT
	{
		int				nFrame;	// When this gfx event should occurs and -1 means start right after create and never restart again, such as smoke tail;
		char			szGFXName[MAX_PATH]; // Filename of the gfx;

		bool			bLinked; // Whether this gfx will be linked with its parent frame;
		char			szParentName[32]; // In which frame we put this gfx;

		FLOAT			vScale; // The scale factor of this graphics fx;
		A3DGraphicsFX*	pGFX; // If the gfx can be reused, this will store its pointer;
		A3DVECTOR3		vecPos;
		A3DVECTOR3		vecDir;
		A3DVECTOR3		vecUp;

		void*			pGFXRecord; // Where this event's pointer is stored now; NULL if not applied yet;
	};

	//	An structure that describe one gfx that current is playing in this model;
	//	When an gfx event happens, it will create one gfx record, and user can 
	//	add gfx record by calling to AddGFX();
	struct GFXRECORD
	{
		A3DGraphicsFX*	pGFX;		//	Pointer of the gfx object;
		void*			pGFXEvent;	//	Where this gfx is from;
	};

	//	An structure that describe the logic command that will pass to the model's owner;
	struct LOGICEVENT
	{
		int				nFrame;
		char			szNotifyString[32];
	};

	struct SFXRECORD;

	//	An event that model will automatically create and start playing a sfx;
	//	all this kind of sound effects will be loaded when this model loaded...
	struct SFXEVENT
	{
		int				nFrame;	// When this sfx event should occurs and -1 means start right after create and never restart again, such as the missile's flying sound;
		char			szSFXName[MAX_PATH]; // Filename of the sfx, typically is a .wav file;

		bool			b2DSound; // Whether this sfx will be forced to be played as a 2D sound;
		bool			bLoop; // Whether this sound effect will loop automatically when it reaches its end;

		char			szParentName[32]; // In which frame we put this gfx;

		A3DFrame *		pParentFrame;

		FLOAT			vVolume; // The volume factor of this sound fx;

		AM3DSoundBuffer* pSFX;
		SFXRECORD*		pSFXRecord;
		FLOAT			vMinDis;
		FLOAT			vMaxDis;

		A3DVECTOR3		vecPos;
		A3DVECTOR3		vecDir;
		A3DVECTOR3		vecUp;
	};

	//	An structure that describe one sfx that current is used in this model;
	//	When a sfx event was loaded, it will create one sfx record or used a preloaded one;
	struct SFXRECORD
	{
		char			szSFXName[MAX_PATH]; 
		SFXEVENT*		pCurEvent;	//	Pointer of current sfx event that control this sfx object;
		AM3DSoundBuffer* pSFX;		//	Pointer of the gfx object;
	};

	//	Ray trace or aabb trace result
	struct TRACERT
	{
		A3DModel*	pRootModel;		//	the root model pointer;
		MODELOBB*	pModelOBB;		//	the frameObb data;
	};

private:
	A3DDevice			* m_pA3DDevice;

	// Settle down in which container;
	A3DCONTAINER		m_Container;

	int					m_nHeartBeats;

	// These list will exist only in the original models;
	// For duplicated ones, we use the pointer directly;
	AList				* m_pActionDefinitionList;
	AList				* m_pActionGroupList;

	// For child frames, we use a completely seperated frames for each model;
	AList				m_ChildFrameList;

	//Animation will flow through the list;
	bool				m_bHasAction;
	AList				m_ChildModelList;
	AList				m_ActionScheduleList;
	AList				m_GFXEventList;
	AList				m_SFXEventList;
	AList				m_LogicEventList;

	// A list which stored all effects this model owns; 
	// for example, smoke, fire, such an effect will move along with the model
	AList				m_GFXList; // Each element is an GFXRECORD structure
	AList				m_SFXList; // Each element is an SFXRECORD structure

	bool				m_bTraceMoveAABBOnly;
	A3DVECTOR3			m_vecAABBTraceCenter;
	A3DVECTOR3			m_vecAABBTraceExtents;

	bool				m_bHasMoved;
	AList				m_ModelOBBList; // OBBs which art designer has pointed out handedly
	A3DAABB				m_ModelAABB; //AABB of this model, calculated whether by obblist or auto obbs
	A3DOBB				m_ModelOBB;

	//For a child model, its reference parent maybe a model or a frame;
	A3DModel *			m_pRealParentModel; // This is the real parent model event if m_pParentFrame is not NULL and m_pParentModel is NULL;
	A3DModel *			m_pParentModel;
	A3DFrame *			m_pParentFrame;
	A3DFrame *			m_pConnectionCenterFrame;

	A3DVECTOR3			m_vecScale;
	A3DMATRIX4			m_matRelativeTM;
	A3DMATRIX4			m_matAbsoluteTM;

	A3DVECTOR3			m_vecPos;
	A3DVECTOR3			m_vecDir;
	A3DVECTOR3			m_vecUp;

	A3DVECTOR3			m_vecVelocity;

	//Each model will have only one rotation axis;
	A3DVECTOR3			m_vecPivotAxis;
	FLOAT				m_vRotateRad;

	// Flag if use Z-Delta to avoid this model be covered by something else;
	bool				m_bZPull;

	// Flag if the model's animation has been paused 
	bool				m_bPaused;

	// Flag if OBB changed
	bool				m_bBuildOBBBevels;

	// The sum of child frame and child model's vert count and index count;
	int					m_nVertCount;
	int					m_nIndexCount;

	// Animation informations
	FLOAT				m_vTimeStored;	// time accumulated for run to next frame;
	int					m_nFrameCount;
	int					m_nFrameOld;
	int					m_nFrame;
	int					m_nAnimStart;
	int					m_nAnimEnd;
	bool				m_bAnimLoop;

	bool				m_bVisible;

	DWORD				m_dwRayTraceBits;
	DWORD				m_dwAABBTraceBits;

	bool				m_bDuplicatedOne;

	BYTE				m_pProperties[PROPERTY_SIZE];

	bool SetAction(A3DACTION * pAction);

	ACTION_CHANGE_CALLBACK	m_pfnActionChangeCallBack;
	LPVOID					m_pActionChangeArg;

	ACTION_LOOPEND_CALLBACK	m_pfnActionLoopEndCallBack;	
	LPVOID					m_pActionLoopArg;


	LOGIC_EVENT_CALLBACK	m_pfnLogicEventCallBack;
	LPVOID					m_pLogicArg;

	TRACERT				m_RayTraceRt;	//	Result of last ray trace
	TRACERT				m_AABBTraceRt;	//	Result of last aabb trace

public:
	A3DModel();
	~A3DModel();

	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACERT* pRayTrace, DWORD dwTraceMask=0xffffffff);
	// Ray trace model's mesh triangles;
	bool RayTraceMesh(A3DVECTOR3& vecStart, A3DVECTOR3& vecDelta, A3DVECTOR3 * pvecPos, A3DVECTOR3 * pvecNormal, float * pvFraction);
	//	AABB trace routine
	bool AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTrace, DWORD dwTraceMask=0xffffffff);

	bool TraceMove(A3DVECTOR3& vecDelta);

	bool AddChildFrame(A3DFrame * pFrame);
	bool RetrieveFrameOBB(A3DFrame * pFrame);
	bool DeleteChildFrame(A3DFrame * pFrame);

	bool AddChildModel(A3DModel * pModel, const char * pszParentFrameName=NULL, const char * pszConnectionCenterFrameName=NULL, ALISTELEMENT ** ppElement=NULL);
protected:
	bool DeleteChildModel(ALISTELEMENT * pElement);
public:

	bool SetParentModel(A3DModel * pParentModel);
	bool SetParentFrame(A3DFrame * pParentFrame);
	bool SetConnectionCenterFrame(const char * pszConnectionCenterFrameName);

	//For model editor only;
	bool DeleteChildModel(A3DModel * pModel);
	
	//We have to set up some methods to control the models' action
	//For example we can use name search for predefined animation range;
	bool DefineAction(A3DACTION ** ppAction, const char * szActionName, int nAnimStart, int nAnimEnd, bool bAnimLoop=true);
	A3DACTION * FindActionByName(const char * szActionName, ALISTELEMENT ** ppElement=NULL);

	//AddGroupAction will first find that group, if not found, just add a new group,
	//then add a group element and if szActionName refer to a NULL action, just leave an 
	//empty action group there;
	bool AddGroupAction(A3DGROUPACTION ** ppGroupAction, const char * szGroupActionName, const char * szActionName);
	bool DeleteGroupActionElement(const char * szGroupActionName, const char * szActionName, bool bDeleteEmpty=true);
	A3DGROUPACTION * FindGroupActionByName(const char * szGroupActionName, ALISTELEMENT ** ppElement=NULL);
	bool DeleteGroupAction(const char * szGroupActionName);
	bool UpdateGroupActionName(const char * szGroupNameOld, const char * szGroupNameNew);
	
	//Used by Model Editor;
	A3DModel * FindChildModelByName(const char * szName, ALISTELEMENT ** ppElement=NULL);
	A3DFrame * FindChildFrameByName(const char * szName, ALISTELEMENT ** ppElement=NULL);
	bool DeleteAction(const char * szActionName);
	bool UpdateAction(const char * szActionNameOld, const char * szActionNameNew, int nAnimStart, int nAnimEnd, bool bAnimLoop);

	bool ReleasePredefinedAction();
	bool PushAction(A3DACTION * pAction, bool bDiscardOld=true);
	bool PlayActionByName(const char * szActionName, bool bDiscardOld, bool bAffectChild=true);
	A3DACTION * PopAction();
	bool DiscardActionSchedule();
	bool StopAction();

	bool AddGFXEvent(int nFrame, const char * szGFXFile, bool bLinked, FLOAT vScale, const char * szParentName,
			A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool DeleteGFXEvent(int nIndex);
	GFXEVENT* GetGFXEvent(int nIndex);
	bool CheckGFXEvents();
	bool StopAllGFX();
	bool SoftStopAllGFX();
	bool RestartGFXEvents();

	SFXRECORD* FindSFXRecord(const char * szFilename);
	bool AddSFXEvent(int nFrame, const char * szSFXFile, bool bLoop, bool b2DSound, FLOAT vMinDis, FLOAT vMaxDis, FLOAT vVolume, const char * szParentName,
			A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool DeleteSFXEvent(int nIndex);
	SFXEVENT* GetSFXEvent(int nIndex);
	bool CheckSFXEvents();
	bool StopAllSFX();
	bool RestartSFXEvents();

	bool PauseAllSFX(bool bPause);
	
	bool AddLogicEvent(int nFrame, const char * szNotifyString);
	bool DeleteLogicEvent(int nIndex);
	LOGICEVENT * GetLogicEvent(int nIndex);
	bool CheckLogicEvents();

	// Model manipulation, Not fully implemented yet, Do not use it;
	bool SetExtraMaterial(A3DMaterial * pMaterialHost);
	bool SetScale(float vScaleX, float vScaleY, float vScaleZ);
	//bool SetCenterMode(A3DCENTER_MODE mode); // decide the center to use when scale and rotate;

protected:
	// Add a gfx into this model, and start playing after the call;
	bool AddGFX(A3DGraphicsFX * pGFX, bool bLinked, FLOAT vScale, const char *szParentName, A3DVECTOR3 vecPos=A3DVECTOR3(0.0f), 
		A3DVECTOR3 vecDir=A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3 vecUp=A3DVECTOR3(0.0f, 1.0f, 0.0), GFXEVENT* pGFXEvent=NULL); // All these reused;
	bool RemoveGFX(GFXRECORD * pGFXRecord);

public:
	// Add a gfx which is specified with name;
	bool AddGFXByName(const char * szGFXFile, bool bLinked, const char * szParentName, A3DVECTOR3 vecPos=A3DVECTOR3(0.0f), 
		A3DVECTOR3 vecDir=A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3 vecUp=A3DVECTOR3(0.0f, 1.0f, 0.0), FLOAT vScale=1.0f); // All these not reused;
	
	bool Init(A3DDevice * pDevice, bool bDuplicated=true);
	bool Duplicate(A3DModel ** pNewModel);
	bool Release();
	bool Reset(); // Reset all variables to initial state

public:
	// when bOptimized is true, model will be rendered 
	// into mesh collector, so engine must be in work
	bool Render(A3DViewport * pCurrentViewport, bool bNeedSort=true, bool bNeedCollect=false); 
	
	bool TickAnimation(FLOAT vDeltaTime=0.0333333f);
	bool PauseAnimation(bool bPause);

	bool UpdateAllInfos();

	bool SetAnimRange(int nAnimStart, int nAnimEnd, bool bAnimLoop=true);
	
	void SetVisibility(bool bVisible);

	bool UpdateToFrame(int nFrame);
	bool UpdateModelOBB();
	bool UpdateModelAABB();

	//Manipulation functions;
	bool SetPos(A3DVECTOR3 vecPos);
	bool SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool SetVelocity(A3DVECTOR3 vecVelocity);
	bool Move(A3DVECTOR3 vecPosDelta);
	bool Turn(FLOAT vDeltaRad);
	
	bool SetRelativePos(A3DModel * pRefModel, A3DVECTOR3 vecPos);
	bool SetRelativeDirAndUp(A3DModel * pRefModel, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool SetPosInParent(A3DVECTOR3& vecPos);
	bool SetDirAndUpInParent(A3DVECTOR3& vecDir, A3DVECTOR3& vecUp);

	bool MoveRelative(A3DModel * pRefModel, A3DVECTOR3 vecDeltaPos);
	bool RotateXRelative(FLOAT vDeltaRad, bool bRelativeLocal=true);
	bool RotateYRelative(FLOAT vDeltaRad, bool bRelativeLocal=true);
	bool RotateZRelative(FLOAT vDeltaRad, bool bRelativeLocal=true);
	// Rotate around an axis which is through the coordinates center;
	bool RotateAxisRelative(A3DVECTOR3 vecAxis, FLOAT vDeltaRad, bool bRelativeLocal=true);
	// Rotate around an axis which is through the point of vecPos;
	bool RotateAxisRelative(A3DVECTOR3 vecPos, A3DVECTOR3 vecAxis, FLOAT vDeltaRad, bool bRelativeLocal=true);

	bool UpdateAbsoluteTM();
	bool UpdateRelativeTM();
	
	bool GetFrameLocation(const char * szFrameName, A3DMATRIX4 matParent, A3DVECTOR3 * vecPos, A3DVECTOR3 * vecX, A3DVECTOR3 * vecY, A3DVECTOR3 * vecZ);

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool LoadImmEffect();
	bool UnloadImmEffect();

	bool UnloadSFX();
	bool ReloadSFX();

	bool SetSFXForce2D(bool bForce2D);

	inline int GetFrame() { return m_nFrame; }
	inline int GetFrameCount() { return m_nFrameCount; }
	inline int GetAnimStart() { return m_nAnimStart; }
	inline int GetAnimEnd() { return m_nAnimEnd; }
	inline A3DVECTOR3 GetPos() { return m_vecPos; }
	inline A3DVECTOR3 GetDir() { return m_vecDir; }
	inline A3DVECTOR3 GetUp()  { return m_vecUp; }
	inline A3DVECTOR3 GetVelocity() { return m_vecVelocity; }
	inline bool GetVisibility() { return m_bVisible; }
	inline void SetIsDuplicated(bool bDuplicated) { m_bDuplicatedOne = bDuplicated; }
	inline int GetModelOBBNum() { return m_ModelOBBList.GetSize(); }
	inline A3DAABB& GetModelAABB() { return m_ModelAABB; }
	inline A3DOBB& GetModelOBB() { return m_ModelOBB; }

	inline int GetVertCount() { return m_nVertCount; }
	inline int GetIndexCount() { return m_nIndexCount; }
	inline void SetVertCount(int nVertCount) { m_nVertCount = nVertCount; }
	inline void SetIndexCount(int nIndexCount) { m_nIndexCount = nIndexCount; }

	inline A3DMATRIX4 GetAbsoluteTM() { return m_matAbsoluteTM; }
	inline A3DMATRIX4 GetRelativeTM() { return m_matRelativeTM; }

	inline void SetAbsoluteTM(A3DMATRIX4& mat) { m_matAbsoluteTM = mat; }
	inline void SetRelativeTM(A3DMATRIX4& mat) { m_matRelativeTM = mat; }

	inline bool HasScheduledAction() { return m_ActionScheduleList.GetSize() ? true : false; }
	inline void SetActionChangeCallBack(ACTION_CHANGE_CALLBACK pfnCallBack, LPVOID pArg) { m_pfnActionChangeCallBack = pfnCallBack; m_pActionChangeArg = pArg; }
	inline void SetActionLoopEndCallBack(ACTION_LOOPEND_CALLBACK pfnCallBack, LPVOID pArg) { m_pfnActionLoopEndCallBack = pfnCallBack; m_pActionLoopArg = pArg; }
	inline void SetLogicEventCallBack(LOGIC_EVENT_CALLBACK pfnCallBack, LPVOID pArg) { m_pfnLogicEventCallBack = pfnCallBack; m_pLogicArg = pArg; }

	inline LPBYTE GetPropertyBuffer() { return m_pProperties; }

	inline AList * GetDefinedActionList() { return m_pActionDefinitionList; }
	inline AList * GetGroupActionList() { return m_pActionGroupList; }
	inline AList * GetChildFrameList() { return &m_ChildFrameList; }
	inline void SetDefinedActionList(AList * pList) { m_pActionDefinitionList = pList; }
	inline void SetGroupActionList(AList * pList) { m_pActionGroupList = pList; }
	
	inline AList * GetChildModelList() { return &m_ChildModelList; }
	inline AList * GetGFXEventList() { return &m_GFXEventList; }
	inline AList * GetGFXList()	{ return &m_GFXList; }
	inline AList * GetSFXEventList() { return &m_SFXEventList; }
	inline AList * GetLogicEventList() { return &m_LogicEventList; }
	inline AList * GetModelOBBList() { return &m_ModelOBBList; }
	inline A3DModel * GetParentModel() { return m_pParentModel; }
	inline A3DFrame * GetParentFrame() { return m_pParentFrame; }
	inline bool IsChildModel() { return m_pParentFrame || m_pParentModel ? true : false; }
	
	inline bool GetRayTraceEnable(DWORD dwMask) { return (m_dwRayTraceBits & dwMask) ? true : false; }
	inline bool GetAABBTraceEnable(DWORD dwMask) { return (m_dwAABBTraceBits & dwMask) ? true : false; }
	inline void SetRayTraceEnable(bool bEnable) { if( bEnable ) m_dwRayTraceBits = 0xffffffff; else m_dwRayTraceBits = 0; }
	inline void SetAABBTraceEnable(bool bEnable) { if( bEnable ) m_dwAABBTraceBits = 0xffffffff; else m_dwAABBTraceBits = 0; }

	inline void SetRayTraceBits(DWORD dwBits) { m_dwRayTraceBits |= dwBits; }
	inline void SetAABBTraceBits(DWORD dwBits) { m_dwAABBTraceBits |= dwBits; }
	inline void ClearRayTraceBits(DWORD dwBits) { m_dwRayTraceBits &= (~dwBits); }
	inline void ClearAABBTraceBits(DWORD dwBits) { m_dwAABBTraceBits &= (~dwBits); }

	inline void SetZPull(bool bZPull) { m_bZPull = bZPull; }
	inline bool GetZPull() { return m_bZPull; }

	inline A3DCONTAINER GetContainer() { return m_Container; }
	inline void SetContainer(A3DCONTAINER container) { m_Container = container; }

	inline void SetAABBTraceOnlyShape(A3DVECTOR3& vecLocalCenter, A3DVECTOR3& vecExtents)
	{ 
		m_vecAABBTraceCenter = vecLocalCenter;
		m_vecAABBTraceExtents = vecExtents; 
		m_bTraceMoveAABBOnly = true; 
	}
	inline void ClearAABBTraceExtents() { m_bTraceMoveAABBOnly = false; }
	inline bool GetTraceMoveAABBOnly() { return m_bTraceMoveAABBOnly; }

	inline A3DModel * GetRealParentModel() { return m_pRealParentModel; }
	inline void SetRealParentModel(A3DModel * pModel) { m_pRealParentModel = pModel; }

	inline const TRACERT& GetRayTraceResult() { return m_RayTraceRt; }
	inline const TRACERT& GetAABBTraceResult() { return m_AABBTraceRt; }
};

typedef A3DModel * PA3DModel;

#endif