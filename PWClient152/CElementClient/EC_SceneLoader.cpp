# define _WIN32_WINNT 0x0500

#include "EC_Global.h"
#include "EC_SceneLoader.h"
#include "EC_SceneThreadObj.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_ManOrnament.h"
#include "EC_ManMatter.h"
#include "EC_ManPlayer.h"
#include "EC_Model.h"
#include "EC_Utility.h"
#include "EC_Player.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_NPC.h"
#include "EC_TimeSafeChecker.h"

#include "A3DTerrain2.h"
#include "A3DTerrain2LoaderB.h"
#include "AList2.h"
#include "AString.h"
#include "AArray.h"
#include "A3DFont.h"
#include "A3DFuncs.h"

volatile bool g_bIsLoadersInited = false;
extern HANDLE g_hToSuspendMain;
extern HANDLE g_hMainThread;
static const float _max_speed = 20.0f * 2.0f; // terrain or ornament must be loaded in middle distance

//	Multi-thread loaded terrain block info
struct MTL_TRNBLOCK
{
	int	r;
	int	c;
	int	iBlock;
};

//	Multi-thread loaded EC model info
struct MTL_ECMODEL
{
	int		iType;		//	MTL_ECM_xxx
	int		idObject;	//	Object ID
	DWORD	dwBornStamp;//	Born stamp
	float	fDist;
	AString	strFile;	//	EC model file
	int		p1;			//	Parameters
	int		p2;
	__int64	p3;
	int		p4[SIZE_ALL_EQUIPIVTR];
};

static DWORD WINAPI LoaderThreadFunc(LPVOID lpParam);

class LoaderThreadModul
{
protected:
	LoaderThreadModul(int nPriority) :
	m_nPriority(nPriority),
	m_hLoader(0),
	m_bSuspendedByCreate(false),
	m_bSuspended(false),
	m_hToSuspend(NULL),
	m_hToResume(NULL),
	m_hSuspendOK(NULL),
	m_hResumeOK(NULL),
	m_hToExit(NULL)
	{}

	virtual ~LoaderThreadModul() {}

protected:
	int		m_nPriority;
	HANDLE	m_hLoader;

	bool m_bSuspendedByCreate;		//	Loader 线程创建挂起后还未运行过
	bool m_bSuspended;						//	Loader 线程当前是否是挂起状态
	
public:
	HANDLE	m_hToSuspend;			//	通知 Loader 挂起
	HANDLE	m_hToResume;			//	通知 Loader 恢复
	HANDLE	m_hSuspendOK;			//	Loader 通知成功挂起
	HANDLE	m_hResumeOK;			//	Loader 通知成功恢复

	HANDLE	m_hToExit;						//	通知 Loader 退出

public:
	void Create();
	bool IsCreated() const{ return m_hLoader != NULL; }

	void Resume();

	bool IsSuspended() const { return IsCreated() && m_bSuspended; }
	void Suspend()
	{
		if (!IsCreated())	return;
		if (IsSuspended()) return;
		
		//	通知 Loader 准备进入等待状态
		SignalObjectAndWait(m_hToSuspend, m_hSuspendOK, INFINITE, FALSE);
		
		//	收到 Loader 回复已进入等待状态
		m_bSuspended = true;
	}

	void Release();

	virtual void LoaderFunc(bool bInLoaderThread) = 0;
	virtual void RemoveAllData() {}
};

inline void LoaderThreadModul::Create()
{
	if (IsCreated())
		return;
	
	m_hToSuspend = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSuspendOK = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hToResume = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hResumeOK = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hToExit = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hLoader = ::CreateThread(NULL, 0, LoaderThreadFunc, this, CREATE_SUSPENDED, NULL);
	::SetThreadPriority(m_hLoader, m_nPriority);
	::SetThreadPriorityBoost(m_hLoader, FALSE);	
	m_bSuspendedByCreate = true;
	m_bSuspended = true;
}

inline void LoaderThreadModul::Release()
{
	if (!IsCreated())
	{
		RemoveAllData();
		return;
	}	

	//	先恢复 Loader 运行
	if (IsSuspended())	Resume();
	
	//	通知 Loader 准备退出
	SetEvent(m_hToExit);

	//	等待 Loader 退出
	WaitForSingleObject(m_hLoader, INFINITE);

	//	Loader 成功退出
	CloseHandle(m_hLoader);
	m_hLoader = NULL;
	m_bSuspended = false;
	m_bSuspendedByCreate = false;

	//	清除事件对象
	CloseHandle(m_hToExit);
	m_hToExit = NULL;

	CloseHandle(m_hToSuspend);
	m_hToSuspend = NULL;

	CloseHandle(m_hSuspendOK);
	m_hSuspendOK = NULL;

	CloseHandle(m_hToResume);
	m_hToResume = NULL;

	CloseHandle(m_hResumeOK);
	m_hResumeOK = NULL;
}

inline void LoaderThreadModul::Resume()
{
	if (!IsCreated()) return;
	if (!IsSuspended())	return;

	if (m_bSuspendedByCreate)
	{
		//	Loader 首次恢复运行
		ResumeThread(m_hLoader);
		m_bSuspendedByCreate = false;
		m_bSuspended = false;
		return;
	}

	SignalObjectAndWait(m_hToResume, m_hResumeOK, INFINITE, FALSE);
	m_bSuspended = false;
}

class TerrLoaderModule : public LoaderThreadModul
{
public:
	TerrLoaderModule() :
	LoaderThreadModul(THREAD_PRIORITY_NORMAL),
	m_aBlkLoads(256, 64),
	m_aToLoad(256),
	m_aToRelease(256),
	m_ScnObjLoadList(512),
	m_ScnObjReleaseList(512)
	{
		::InitializeCriticalSection(&m_csTrnBlock);
		::InitializeCriticalSection(&m_csScnObj);
	}

	~TerrLoaderModule()
	{
		::DeleteCriticalSection(&m_csTrnBlock);
		::DeleteCriticalSection(&m_csScnObj);
	}

protected:
	AArray<MTL_TRNBLOCK, MTL_TRNBLOCK&> m_aBlkLoads;
	APtrList<CECSceneThreadObj*> m_aToLoad;
	APtrList<CECSceneThreadObj*> m_aToRelease;
	APtrList<CECSceneThreadObj*> m_ScnObjLoadList;
	APtrList<CECSceneThreadObj*> m_ScnObjReleaseList;

	CRITICAL_SECTION m_csTrnBlock;
	CRITICAL_SECTION m_csScnObj;

public:
	void LoadTerrainData(bool bInLoaderThread);
	void LoadOrnament(bool bInLoaderThread);
	void LoaderFunc(bool bInLoaderThread);
	void RemoveAllData()
	{
		::EnterCriticalSection(&m_csTrnBlock);
		m_aBlkLoads.RemoveAll();
		::LeaveCriticalSection(&m_csTrnBlock);

		::EnterCriticalSection(&m_csScnObj);
		m_ScnObjLoadList.RemoveAll();

		ALISTPOSITION pos = m_ScnObjReleaseList.GetHeadPosition();

		while (pos)
		{
			CECSceneThreadObj* pObj = m_ScnObjReleaseList.GetNext(pos);
			if (pObj->IsLoaded()) pObj->ReleaseInThread();
			delete pObj;
		}

		m_ScnObjReleaseList.RemoveAll();

		m_aToLoad.RemoveAll();
		m_aToRelease.RemoveAll();

		::LeaveCriticalSection(&m_csScnObj);
	}
	void QueueTerrainBlock(MTL_TRNBLOCK blk)
	{
		::EnterCriticalSection(&m_csTrnBlock);
		m_aBlkLoads.Add(blk);
		::LeaveCriticalSection(&m_csTrnBlock);
	}
	void QueueLoadScnObj(CECSceneThreadObj* pObj)
	{
		::EnterCriticalSection(&m_csScnObj);
		m_ScnObjLoadList.AddTail(pObj);
		::LeaveCriticalSection(&m_csScnObj);
	}
	void QueueReleaseScnObj(CECSceneThreadObj* pObj)
	{
		::EnterCriticalSection(&m_csScnObj);
		pObj->SetToRelease();
		m_ScnObjReleaseList.AddTail(pObj);
		::LeaveCriticalSection(&m_csScnObj);
	}
};

inline void TerrLoaderModule::LoadTerrainData(bool bInLoaderThread)
{
	//	Load terrain data
	A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();
	if (!pTerrain->IsMultiThreadLoad() || !pTerrain->GetDataLoadFlag()) return;

	DWORD dwMaxTick = DWORD(pTerrain->GetActRadius() * (1000.f / _max_speed));
	bool bSuspendMain = false;

	while (true)
	{
		A3DTerrain2Loader* pLoader = pTerrain->GetTerrainLoader();
		if (pLoader->GetLoaderID() == A3DTerrain2Loader::LOADER_B)
			((A3DTerrain2LoaderB*)pLoader)->DeactivateIdleSubTerrains();

		//	Release unloaded blocks ---------------------------
		A3DTerrain2Block* pBlock = pTerrain->GetNextBlockToBeUnloaded();
		while (pBlock)
		{
			pTerrain->ThreadUnloadBlock(pBlock);
			pBlock = pTerrain->GetNextBlockToBeUnloaded();
		}

		//	Load candidate blocks ------------------------------
		::EnterCriticalSection(&m_csTrnBlock);

		if (m_aBlkLoads.GetSize() == 0)
		{
			::LeaveCriticalSection(&m_csTrnBlock);
			break;
		}

		AArray<MTL_TRNBLOCK, MTL_TRNBLOCK&> aBlkLoads(64, 64);

		//	Transfer candidate terrain blocks
		int i(0);
		for (i = 0; i < m_aBlkLoads.GetSize(); i++)
			aBlkLoads.Add(m_aBlkLoads[i]);

		m_aBlkLoads.RemoveAll(false);

		::LeaveCriticalSection(&m_csTrnBlock);

		/*
		char buf[100];
		sprintf(buf, "load count = %d\n", aBlkLoads.GetSize());
		OutputDebugStringA(buf);
		*/

		DWORD dwStart = ::GetTickCount();

		//	Load candidate terrain blocks ...
		for (i=0; i < aBlkLoads.GetSize(); i++)
		{
			const MTL_TRNBLOCK& Blk = aBlkLoads[i];
			pTerrain->ThreadLoadBlock(Blk.r, Blk.c, Blk.iBlock);

			if (bInLoaderThread && !bSuspendMain)
			{
				if (CECTimeSafeChecker::ElapsedTimeFor(dwStart) >= dwMaxTick)
				{
  					bSuspendMain = true;
					::SetEvent(g_hToSuspendMain);
				}
				else
					::Sleep(10);
			}
		}
	}

	if (bSuspendMain && ::WaitForSingleObject(g_hToSuspendMain, 0) != WAIT_OBJECT_0)
		::ResumeThread(g_hMainThread);

	if (bInLoaderThread)
		::Sleep(200);
}

inline void TerrLoaderModule::LoadOrnament(bool bInLoaderThread)
{
	A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();
	DWORD dwMaxTick = DWORD(pTerrain->GetViewRadius() * (1000.f / _max_speed));
	bool bSuspendMain = false;
	static const float _min_square_dist = 2500.0f;

	while (true)
	{
		::EnterCriticalSection(&m_csScnObj);

		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		ALISTPOSITION pos = m_ScnObjLoadList.GetHeadPosition();

		if (pHost && bInLoaderThread)
		{
			A3DVECTOR3 vHost = pHost->GetPos();

			while (pos)
			{
				CECSceneThreadObj* pObj = m_ScnObjLoadList.GetNext(pos);
				A3DVECTOR3 vLoadPos = pObj->GetLoadPos();
				float xOff = vLoadPos.x - vHost.x;
				float zOff = vLoadPos.z - vHost.z;
				float fDist = xOff * xOff + zOff * zOff;
				pObj->SetDistToHost(fDist);

				if (bInLoaderThread && !bSuspendMain && fDist < _min_square_dist)
				{
					bSuspendMain = true;
					::SetEvent(g_hToSuspendMain);
				}

				ALISTPOSITION posLoad = m_aToLoad.GetHeadPosition();
				while (posLoad)
				{
					ALISTPOSITION posCur = posLoad;
					CECSceneThreadObj* p = m_aToLoad.GetNext(posLoad);

					if (fDist < p->GetDistToHost())
					{
						posLoad = posCur;
						m_aToLoad.InsertBefore(posLoad, pObj);
						break;
					}
				}

				if (posLoad == NULL)
					m_aToLoad.AddTail(pObj);
			}
		}
		else while (pos)
			m_aToLoad.AddTail(m_ScnObjLoadList.GetNext(pos));

		m_ScnObjLoadList.RemoveAll();

		pos = m_ScnObjReleaseList.GetHeadPosition();
		while (pos) m_aToRelease.AddTail(m_ScnObjReleaseList.GetNext(pos));
		m_ScnObjReleaseList.RemoveAll();

		::LeaveCriticalSection(&m_csScnObj);

		if (m_aToLoad.GetCount() == 0 && m_aToRelease.GetCount() == 0)
			break;

		// 先加载再释放

		if (m_aToLoad.GetCount())
		{
			/*
			char buf[100];
			sprintf(buf, "orn count = %d\n", m_aToLoad.GetCount());
			OutputDebugStringA(buf);
			*/

			DWORD dwStart = ::GetTickCount();
			ALISTPOSITION posLoad = m_aToLoad.GetHeadPosition();

			while (posLoad)
			{
				CECSceneThreadObj* pToLoad = m_aToLoad.GetNext(posLoad);

				if (pToLoad->IsToRelease())
					continue;
				else
				{
					pToLoad->LoadInThread(bInLoaderThread);
					pToLoad->SetLoaded();

					if (bInLoaderThread && !bSuspendMain)
					{
						if (CECTimeSafeChecker::ElapsedTimeFor(dwStart) >= dwMaxTick)
						{
  							bSuspendMain = true;
							::SetEvent(g_hToSuspendMain);
						}
						else
							::Sleep(10);
					}
				}
			}

			/*
			sprintf(buf, "orn time = %d\n", ::GetTickCount() - dwStart);
			OutputDebugStringA(buf);
			*/

			m_aToLoad.RemoveAll();
		}

		if (m_aToRelease.GetCount())
		{
			ALISTPOSITION posRelease = m_aToRelease.GetHeadPosition();

			while (posRelease)
			{
				CECSceneThreadObj* pToRelease = m_aToRelease.GetNext(posRelease);
				if (pToRelease->IsLoaded()) pToRelease->ReleaseInThread();
				delete pToRelease;
			}

			m_aToRelease.RemoveAll();
		}
	}

	if (bSuspendMain && ::WaitForSingleObject(g_hToSuspendMain, 0) != WAIT_OBJECT_0)
		::ResumeThread(g_hMainThread);

	if (bInLoaderThread) Sleep(100);
}

void TerrLoaderModule::LoaderFunc(bool bInLoaderThread)
{
	LoadTerrainData(bInLoaderThread);
	LoadOrnament(bInLoaderThread);
}

class ECModelLoaderModule : public LoaderThreadModul
{
protected:
	ECModelLoaderModule(int nPriority) :
	LoaderThreadModul(nPriority),
	m_aModelLoads(512),
	m_aDelModels(512, 64),
	m_aTemplDel(64, 64),
	m_aTemplLoad(256, 256)
	{
		::InitializeCriticalSection(&m_csDelECModel);
		::InitializeCriticalSection(&m_csECModel);
	}

	virtual ~ECModelLoaderModule()
	{
		::DeleteCriticalSection(&m_csDelECModel);
		::DeleteCriticalSection(&m_csECModel);
	}

protected:
	APtrList<MTL_ECMODEL*>	m_aModelLoads;
	APtrArray<CECModel*>	m_aDelModels;
	APtrArray<CECModel*>	m_aTemplDel;
	APtrArray<MTL_ECMODEL*> m_aTemplLoad;

	CRITICAL_SECTION		m_csDelECModel;
	CRITICAL_SECTION		m_csECModel;

public:
	void LoadAndReleaseECModels(bool bInLoaderThread);
	void QueueECModelForLoad(int iType, int idObject, DWORD dwBornStamp, const A3DVECTOR3& vPos, const char* szFile, int p1, int p2, __int64 p3, int* p4);
	void QueueUndoLoad(int id, DWORD dwBornStamp);
	void QueueUndoLoad(int id);
	void QueueECModelForRelease(CECModel* pModel);
	void LoaderFunc(bool bInLoaderThread);
	void RemoveOldData()
	{
		::EnterCriticalSection(&m_csDelECModel);

		for (int i = 0; i < m_aDelModels.GetSize(); i++)
		{
			CECModel* pModel = m_aDelModels[i];
			pModel->Release();
			delete pModel;
		}

		m_aDelModels.RemoveAll(false);
		::LeaveCriticalSection(&m_csDelECModel);
	}
	void RemoveAllData()
	{
		RemoveOldData();

		::EnterCriticalSection(&m_csECModel);
		ALISTPOSITION pos = m_aModelLoads.GetHeadPosition();
		while (pos) delete m_aModelLoads.GetNext(pos);
		m_aModelLoads.RemoveAll();
		::LeaveCriticalSection(&m_csECModel);
	}
	void RemoveAllDataUnneeded()
	{
		// 检查哪些 ECM 模型还需要加载，保留它们
		// 其它同 RemoveAllData ()
		// 只能在 Main 线程中调用

		while (true)
		{
			// 检查必要条件
			//
			if (!g_pGame)
				break;

			CECGameRun *pGameRun = g_pGame->GetGameRun();
			if (!pGameRun)
				break;

			CECWorld *pWorld = pGameRun->GetWorld();
			if (!pWorld)
				break;

			CECNPCMan *pNPCMan = pWorld->GetNPCMan();
			if (!pNPCMan ||
				!pNPCMan->GetNPCNum())
				break;

			// 删除数据1
			//
			RemoveOldData();
			
			// 有条件删除数据2
			::EnterCriticalSection(&m_csECModel);

			ALISTPOSITION pos = m_aModelLoads.GetHeadPosition(), posCur;
			while (pos)
			{
				posCur = pos;
				MTL_ECMODEL *pTemp = m_aModelLoads.GetNext(pos);

				if (pTemp &&
					pTemp->iType == MTL_ECM_NPC &&
					pNPCMan->ISNPCModelNeedLoad(pTemp->idObject, pTemp->dwBornStamp))
				{
					// 只有需要加载的 NPC 模型还未加载成功
					//
					continue;
				}
				
				// 其它情况，一律删除
				delete pTemp;
				m_aModelLoads.RemoveAt(posCur);
			}

			::LeaveCriticalSection(&m_csECModel);

			return;
		}

		RemoveAllData();
	}
};

inline void ECModelLoaderModule::QueueECModelForLoad(
	int iType,
	int idObject,
	DWORD dwBornStamp,
	const A3DVECTOR3& vPos,
	const char* szFile,
	int p1,
	int p2,
	__int64 p3,
	int* p4)
{
	::EnterCriticalSection(&m_csECModel);

	MTL_ECMODEL* Info = new MTL_ECMODEL;
	Info->iType			= iType;
	Info->idObject		= idObject;
	Info->dwBornStamp	= dwBornStamp;
	Info->fDist			= SquareMagnitude(vPos - g_pGame->GetGameRun()->GetHostPlayer()->GetPos());
	if (szFile) Info->strFile = szFile;
	Info->p1			= p1;
	Info->p2			= p2;
	Info->p3			= p3;

	if (p4)
	{
		for (int i = 0; i < SIZE_ALL_EQUIPIVTR; i++)
			Info->p4[i] = p4[i];
	}
	else
		memset(Info->p4, 0xff, sizeof(Info->p4));

	m_aModelLoads.AddTail(Info);
	::LeaveCriticalSection(&m_csECModel);
}

inline void ECModelLoaderModule::QueueUndoLoad(int cid, DWORD dwBornStamp)
{
	::EnterCriticalSection(&m_csECModel);

	// 判断加载队列是否已有此对象

	ALISTPOSITION pos = m_aModelLoads.GetHeadPosition(), posCur;

	while (pos)
	{
		posCur = pos;
		const MTL_ECMODEL* pInfo = m_aModelLoads.GetNext(pos);
		if (pInfo->idObject == cid && pInfo->dwBornStamp == dwBornStamp)
		{
			delete pInfo;
			m_aModelLoads.RemoveAt(posCur);
		}
	}

	::LeaveCriticalSection(&m_csECModel);
}

inline void ECModelLoaderModule::QueueUndoLoad(int cid)
{
	::EnterCriticalSection(&m_csECModel);
	
	// 判断加载队列是否已有此对象
	
	ALISTPOSITION pos = m_aModelLoads.GetHeadPosition(), posCur;
	
	while (pos)
	{
		posCur = pos;
		const MTL_ECMODEL* pInfo = m_aModelLoads.GetNext(pos);
		if (pInfo->idObject == cid)
		{
			delete pInfo;
			m_aModelLoads.RemoveAt(posCur);
		}
	}
	
	::LeaveCriticalSection(&m_csECModel);
}

inline void ECModelLoaderModule::QueueECModelForRelease(CECModel* pModel)
{
	::EnterCriticalSection(&m_csDelECModel);
	m_aDelModels.Add(pModel);
	::LeaveCriticalSection(&m_csDelECModel);
}

static const int _load_per_time = 4;

#include "EC_GameUIMan.h"
#include "EC_UIManager.h"

inline void ECModelLoaderModule::LoadAndReleaseECModels(bool bInLoaderThread)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	CECNPCMan* pNPCMan = pWorld->GetNPCMan();
	CECOrnamentMan* pOnmtMan = pWorld->GetOrnamentMan();
	CECMatterMan* pMatterMan = pWorld->GetMatterMan();
	CECPlayerMan* pPlayerMan = pWorld->GetPlayerMan();

	//	Delete at first -------------------------
	::EnterCriticalSection(&m_csDelECModel);
	m_aTemplDel.RemoveAll(false);

	int i(0);
	for (i = 0; i < m_aDelModels.GetSize(); i++)
		m_aTemplDel.Add(m_aDelModels[i]);

	m_aDelModels.RemoveAll(false);
	::LeaveCriticalSection(&m_csDelECModel);

	for (i = 0; i < m_aTemplDel.GetSize(); i++)
	{
		CECModel* pModel = m_aTemplDel[i];
		pModel->Release();
		delete pModel;
	}

	//	Load -------------------------------------

	//	Transfer candidate NPC models
	::EnterCriticalSection(&m_csECModel);
	m_aTemplLoad.RemoveAll(false);

	if (m_aModelLoads.GetCount())
	{
		if (bInLoaderThread)
		{
			int nLoadCount = 0;
			ALISTPOSITION pos = m_aModelLoads.GetHeadPosition();
			ALISTPOSITION posCur;

			struct
			{
				MTL_ECMODEL* p;
				ALISTPOSITION pos;
			} _tmp[_load_per_time + 1];

			memset(_tmp, 0, sizeof(_tmp));

			while (pos)
			{
				posCur = pos;
				MTL_ECMODEL* p = m_aModelLoads.GetNext(pos);
				bool bReplace = false;

				for (int i = 0; i < nLoadCount; i++)
				{
					if (p->fDist < _tmp[i].p->fDist)
					{
						memmove(&_tmp[i+1], &_tmp[i], (nLoadCount - i) * sizeof(_tmp[0]));
						_tmp[i].p = p;
						_tmp[i].pos = posCur;
						if (nLoadCount < _load_per_time) nLoadCount++;
						bReplace = true;
						break;
					}
				}

				if (!bReplace && nLoadCount < _load_per_time)
				{
					_tmp[nLoadCount].p = p;
					_tmp[nLoadCount].pos = posCur;
					nLoadCount++;
				}
			}

			for (int i = 0; i < nLoadCount; i++)
			{
				m_aTemplLoad.Add(_tmp[i].p);
				m_aModelLoads.RemoveAt(_tmp[i].pos);
			}
		}
		else
		{
			ALISTPOSITION pos = m_aModelLoads.GetHeadPosition();
			while (pos) m_aTemplLoad.Add(m_aModelLoads.GetNext(pos));
			m_aModelLoads.RemoveAll();
		}
	}

	::LeaveCriticalSection(&m_csECModel);

	if (m_aTemplLoad.GetSize() > 0)
	{
		//	Load EC models
		for (i = 0; i < m_aTemplLoad.GetSize(); i++)
		{
			MTL_ECMODEL& Info = *m_aTemplLoad[i];
			DWORD dwSleep = 100, dwTick;

			switch (Info.iType)
			{
			case MTL_ECM_PLAYER:

				dwTick = GetTickCount();
				pPlayerMan->ThreadLoadPlayerModel(Info.idObject, Info.dwBornStamp, Info.p1, Info.p2, (int)Info.p3, Info.p4, Info.strFile,false);
				dwSleep = CECTimeSafeChecker::ElapsedTimeFor(dwTick) * 3;
				if (dwSleep > 1000) dwSleep = 1000;
				break;

			case MTL_ECM_PLAYER_EQUIP:

				pPlayerMan->ThreadLoadPlayerEquips(Info.idObject, Info.dwBornStamp, Info.p1, Info.p2, Info.p4, Info.p3);
				break;

			case MTL_ECM_PLAYER_DUMMY:
				
				pPlayerMan->ThreadLoadDummyModel(Info.idObject, Info.dwBornStamp, Info.p1,false);
				break;
			case MTL_ECM_HOST_NAVIGATER_DUMMY:

				pPlayerMan->ThreadLoadDummyModel(Info.idObject, Info.dwBornStamp, Info.p1,true);
				break;
			case MTL_ECM_PET:

				pPlayerMan->ThreadLoadPetModel(Info.idObject, Info.dwBornStamp, Info.strFile);
				break;

			case MTL_ECM_NPC:

				pNPCMan->ThreadLoadNPCModel(Info.idObject, Info.dwBornStamp, Info.p1, Info.strFile);
				break;

			case MTL_ECM_SCNMODEL:

				pOnmtMan->ThreadLoadECModel((DWORD)Info.idObject, Info.strFile);
				break;

			case MTL_ECM_MATTER:

				pMatterMan->ThreadLoadMatterModel(Info.idObject, Info.strFile);
				break;

			case MTL_ECM_PLAYER_FACE:

				pPlayerMan->ThreadLoadPlayerFace(Info.idObject, Info.dwBornStamp, Info.p1, Info.p2, (int)Info.p3);
				break;
			case MTL_ECM_HOST_NAVIGATER:
				dwTick = GetTickCount();
				pPlayerMan->ThreadLoadPlayerModel(Info.idObject, Info.dwBornStamp, Info.p1, Info.p2, (int)Info.p3, Info.p4, Info.strFile,true);
				dwSleep = CECTimeSafeChecker::ElapsedTimeFor(dwTick) * 3;
				if (dwSleep > 1000) dwSleep = 1000;
				break;
			}

			delete &Info;

			if (bInLoaderThread)
				::Sleep(dwSleep);
		}
	}
	else if (bInLoaderThread)
	{
		::Sleep(200);
	}
}

void ECModelLoaderModule::LoaderFunc(bool bInLoaderThread)
{
	LoadAndReleaseECModels(bInLoaderThread);
}

class NPCLoaderModule : public ECModelLoaderModule
{
public:
	NPCLoaderModule() : ECModelLoaderModule(THREAD_PRIORITY_NORMAL) {}
	~NPCLoaderModule() {}
};

class PlayerLoaderModule : public ECModelLoaderModule
{
public:
	PlayerLoaderModule() : ECModelLoaderModule(THREAD_PRIORITY_NORMAL) {}
	~PlayerLoaderModule() {}

public:
	void CalcPlayersRelDist();
};

inline void PlayerLoaderModule::CalcPlayersRelDist()
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	if (!pWorld) return;

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost)	return;

	const A3DVECTOR3& vHost = pHost->GetPos();
	CECPlayerMan* pMan = pWorld->GetPlayerMan();

	::EnterCriticalSection(&m_csECModel);

	ALISTPOSITION pos = m_aModelLoads.GetHeadPosition();

	while (pos)
	{
		MTL_ECMODEL* p = m_aModelLoads.GetNext(pos);
		CECElsePlayer* pPlayer = pMan->GetElsePlayer(p->idObject);

		if (pPlayer)
			p->fDist = SquareMagnitude(pPlayer->GetPos() - vHost);
	}

	::LeaveCriticalSection(&m_csECModel);
}

// Loader Modules

TerrLoaderModule	g_TerrLoaderModule;
NPCLoaderModule		g_NPCLoaderModule;
PlayerLoaderModule	g_PlayerLoaderModule;

static DWORD WINAPI _LoaderThreadFuncImp(LPVOID lpParam)
{
	LoaderThreadModul* pLoader = static_cast<LoaderThreadModul*>(lpParam);

	while (true)
	{
		if (::WaitForSingleObject(pLoader->m_hToExit, 0) == WAIT_OBJECT_0)
			break;

		pLoader->LoaderFunc(true);

		if (WaitForSingleObject(pLoader->m_hToSuspend, 0) == WAIT_OBJECT_0)
		{
			//	收到通知，要求自己进入等待状态

			//	进入等待状态并发出回应通知
			SignalObjectAndWait(pLoader->m_hSuspendOK, pLoader->m_hToResume, INFINITE, FALSE);

			//	苏醒，发出回应
			SetEvent(pLoader->m_hResumeOK);
		}
	}

	pLoader->RemoveAllData();
	return 0;
}

static DWORD WINAPI LoaderThreadFunc(LPVOID lpParam)
{
	DWORD dwRet = -1;

#ifndef _NOMINIDUMP

	__try
	{

#endif
	
	dwRet = _LoaderThreadFuncImp(lpParam);

#ifndef _NOMINIDUMP

	}	//	__try

	__except (glb_HandleException(GetExceptionInformation()))
	{
		::OutputDebugString(_AL("Exception occurred...\n"));
		a_LogOutput(1, "Exception occurred... mini dumped in loader thread!");
		::ExitProcess(-1);
	}

#endif

	return dwRet;
}

bool InitLoaderThread()
{
	if (g_bIsLoadersInited) return true;
	g_TerrLoaderModule.Create();
	g_NPCLoaderModule.Create();
	g_PlayerLoaderModule.Create();
	g_bIsLoadersInited = true;
	return true;
}

void StartLoaderThread()
{
	if (!g_bIsLoadersInited) return;
	g_TerrLoaderModule.Resume();
	g_NPCLoaderModule.Resume();
	g_PlayerLoaderModule.Resume();
}

void ExitLoaderThread()
{
	if (!g_bIsLoadersInited)
		return;

	g_TerrLoaderModule.Release();
	g_NPCLoaderModule.Release();
	g_PlayerLoaderModule.Release();

	g_bIsLoadersInited = false;
}

void QueueLoadInThread(CECSceneThreadObj* pObj)
{
	g_TerrLoaderModule.QueueLoadScnObj(pObj);
}

void QueueReleaseInThread(CECSceneThreadObj* pObj)
{
	g_TerrLoaderModule.QueueReleaseScnObj(pObj);
}

void QueueTerrainBlock(int r, int c, int iBlock)
{
	MTL_TRNBLOCK Blk = {r, c, iBlock};
	g_TerrLoaderModule.QueueTerrainBlock(Blk);
}

void QueueECModelForLoad(
	int iType,
	int idObject,
	DWORD dwBornStamp,
	const A3DVECTOR3& vPos,
	const char* szFile,
	int p1/* 0 */,
	int p2/* 0 */,
	__int64 p3/* 0 */,
	int* p4)
{
	switch (iType)
	{
	case MTL_ECM_PLAYER:
	case MTL_ECM_PLAYER_EQUIP:
	case MTL_ECM_PLAYER_DUMMY:
	case MTL_ECM_PLAYER_FACE:
	case MTL_ECM_HOST_NAVIGATER:
	case MTL_ECM_HOST_NAVIGATER_DUMMY:
		g_PlayerLoaderModule.QueueECModelForLoad(iType, idObject, dwBornStamp, vPos, szFile, p1, p2, p3, p4);
		break;
	default:
		g_NPCLoaderModule.QueueECModelForLoad(iType, idObject, dwBornStamp, vPos, szFile, p1, p2, p3, p4);
		break;
	}
}

// called in main thread
void CalcPlayersRelDist()
{
	g_PlayerLoaderModule.CalcPlayersRelDist();
}

void QueueMatterUndoLoad(int mid)
{
	g_NPCLoaderModule.QueueUndoLoad(mid);
}

void QueueNPCUndoLoad(int nid, DWORD dwBornStamp)
{
	g_NPCLoaderModule.QueueUndoLoad(nid, dwBornStamp);
}

void QueuePlayerUndoLoad(int cid)
{
	g_PlayerLoaderModule.QueueUndoLoad(cid);
}

void QueueECModelForRelease(CECModel* pModel)
{
	g_PlayerLoaderModule.QueueECModelForRelease(pModel);
}

void SuspendLoadThread()
{
	if (!g_bIsLoadersInited) return;

	g_TerrLoaderModule.Suspend();
	g_NPCLoaderModule.Suspend();
	g_PlayerLoaderModule.Suspend();
}

void LoadInMainThread(bool bForceLoad)
{
	if (bForceLoad)
		SuspendLoadThread();
	else if (g_bIsLoadersInited && (!g_TerrLoaderModule.IsSuspended() || !g_NPCLoaderModule.IsSuspended() || !g_PlayerLoaderModule.IsSuspended()))
	{
		static DWORD _tick = 0;
		if (_tick++ % 4 == 0)
			CalcPlayersRelDist();
		return;
	}

	//	Load terrain data
	A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();
	if (pTerrain && pTerrain->IsMultiThreadLoad() && pTerrain->GetDataLoadFlag())
	{
		//	Release unloaded blocks
		A3DTerrain2Block* pBlock = pTerrain->GetNextBlockToBeUnloaded();
		while (pBlock)
		{
			pTerrain->ThreadUnloadBlock(pBlock);
			pBlock = pTerrain->GetNextBlockToBeUnloaded();
		}

		//	Terrain laoding will occupy about 45% loading progress, calculate
		//	each block's component
		float fBlockComp = 45.0f / pTerrain->GetCandidateBlockNum();

		//	Load candidate terrain blocks ...
		int r, c, iBlock;
		while (pTerrain->GetNextCandidateBlock(r, c, iBlock))
		{
			pTerrain->ThreadLoadBlock(r, c, iBlock);

			//	Step loading progress
			g_pGame->GetGameRun()->StepLoadProgress(fBlockComp);
		}
		
		pTerrain->ActivateLoadedBlocks();
	}
	
	g_NPCLoaderModule.RemoveOldData();
	g_PlayerLoaderModule.RemoveOldData();

	g_TerrLoaderModule.LoadOrnament(false);
	g_NPCLoaderModule.LoaderFunc(false);
	g_PlayerLoaderModule.LoaderFunc(false);
}

bool IsLoadThreadReady()
{
	return g_bIsLoadersInited;
}

void ThreadRemoveAllLoaded()
{
	g_TerrLoaderModule.RemoveAllData();
	g_NPCLoaderModule.RemoveAllData();
	g_PlayerLoaderModule.RemoveAllData();
}

void ThreadRemoveAllUnNeeded()
{
	g_TerrLoaderModule.RemoveAllData();
	g_NPCLoaderModule.RemoveAllDataUnneeded();
	g_PlayerLoaderModule.RemoveAllData();
}

//----------------------
//	Class LoadThreadSuspender
//----------------------
LoadThreadSuspender::LoadThreadSuspender()
: m_bResumeTerrLoader(false)
, m_bResumeNPCLoader(false)
, m_bResumePlayerLoader(false)
{
	if (IsLoadThreadReady())
	{
		if (!g_TerrLoaderModule.IsSuspended())
		{
			g_TerrLoaderModule.Suspend();
			m_bResumeTerrLoader = true;
		}
		if (!g_NPCLoaderModule.IsSuspended())
		{
			g_NPCLoaderModule.Suspend();
			m_bResumeNPCLoader = true;
		}
		if (!g_PlayerLoaderModule.IsSuspended())
		{
			g_PlayerLoaderModule.Suspend();
			m_bResumePlayerLoader = true;
		}
	}
}

LoadThreadSuspender::~LoadThreadSuspender()
{
	if (IsLoadThreadReady())
	{
		//	根据各 LoadModule 之前的暂停情况进行恢复、维持之前的暂停状态
		//
		if (m_bResumeTerrLoader)
			g_TerrLoaderModule.Resume();

		if (m_bResumeNPCLoader)
			g_NPCLoaderModule.Resume();

		if (m_bResumePlayerLoader)
			g_PlayerLoaderModule.Resume();
	}
}