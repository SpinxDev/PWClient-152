#pragma once

//	Multi-thread loaded EC model type
enum
{
	MTL_ECM_PLAYER = 0,
	MTL_ECM_NPC,
	MTL_ECM_MATTER,
	MTL_ECM_SCNMODEL,		//	EC model in scene
	MTL_ECM_PLAYER_EQUIP,
	MTL_ECM_PET,
	MTL_ECM_PLAYER_DUMMY,	//  Player transform
	MTL_ECM_PLAYER_FACE,
	MTL_ECM_HOST_NAVIGATER,
	MTL_ECM_HOST_NAVIGATER_DUMMY,
};

class CECSceneThreadObj;
class CECModel;

bool InitLoaderThread();
void StartLoaderThread();
void ExitLoaderThread();

void QueueLoadInThread(CECSceneThreadObj* pObj);
void QueueReleaseInThread(CECSceneThreadObj* pObj);
void QueueTerrainBlock(int r, int c, int iBlock);
void QueueECModelForLoad(int iType, int idObject, DWORD dwBornStamp, const A3DVECTOR3& vPos, const char* szFile, int p1=0, int p2=0, __int64 p3=0, int* p4 = NULL);
void QueueECModelForRelease(CECModel* pModel);
void QueueMatterUndoLoad(int mid);
void QueueNPCUndoLoad(int nid, DWORD dwBornStamp);
void QueuePlayerUndoLoad(int cid);

void SuspendLoadThread();
void LoadInMainThread(bool bForceLoad);
void ThreadRemoveAllLoaded();
void ThreadRemoveAllUnNeeded();

bool IsLoadThreadReady();

// 类 LoadThreadSuspender : 构造函数暂停所有加载线程、析构函数根据情况恢复加载线程
class LoadThreadSuspender
{
public:
	LoadThreadSuspender();
	~LoadThreadSuspender();

private:
	bool	m_bResumeTerrLoader;
	bool	m_bResumeNPCLoader;
	bool	m_bResumePlayerLoader;

	LoadThreadSuspender(const LoadThreadSuspender &);
	LoadThreadSuspender & operator = (const LoadThreadSuspender &);
};