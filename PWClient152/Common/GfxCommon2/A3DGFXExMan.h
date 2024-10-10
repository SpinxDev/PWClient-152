#ifndef _A3DGFXEXMAN_H_
#define _A3DGFXEXMAN_H_

#include <time.h>
#include "A3DTypes.h"
#include "A3DGFXEx.h"
#include "A3DGFXCachedItemMan.h"
#include "A3DGFXStreamMan.h"
#include "A3DGFXTexMan.h"
#include "A3DGFXRenderSlot.h"
#include "A3DSkinModel.h"
#include "A3DViewport.h"
#include "A3DEngine.h"
#include "A3DWireCollector.h"
#include "A3DGFXLight.h"
#include "A3DGFXModelMan.h"
#include "A3DGFXSurfaceRender.h"
#include <hashmap.h>
#include "GfxCommonTypes.h"

class A3DCamera;
class A3DOrthoCamera;
class A3DViewport;
class APhysXScene;
class A3DGFXInterface;
class A3DRenderTarget;

#define GFX_STAT
#define GFX_DEBUG_LOG 0

#if GFX_DEBUG_LOG == 1
	extern const char* _gfx_log_file;
	extern int _cur_gfx_count;
#endif

#if GFX_DEBUG_LOG == 1
	inline void GfxWriteLog(const char* szLine)
	{
		FILE* fp = fopen(_gfx_log_file, "ab");
		if (fp)
		{
			time_t tm;
			time(&tm);
			char buf[50];
			strcpy(buf, ctime(&tm));
			buf[strlen(buf)-1] = 0;
			fprintf(fp, "%s: %s\r\n", buf, szLine);
			fclose(fp);
		}
	}
	#define GFX_WRITE_LOG0(szLine) GfxWriteLog(szLine);
	#define GFX_WRITE_LOG1(szFormat, p1) \
	{ \
		char szBuf[1024]; \
		sprintf(szBuf, szFormat, p1); \
		GfxWriteLog(szBuf); \
	}
	#define GFX_WRITE_LOG2(szFormat, p1, p2) \
	{ \
		char szBuf[1024]; \
		sprintf(szBuf, szFormat, p1, p2); \
		GfxWriteLog(szBuf); \
	}
	#define GFX_WRITE_LOG3(szFormat, p1, p2, p3) \
	{ \
		char szBuf[1024]; \
		sprintf(szBuf, szFormat, p1, p2, p3); \
		GfxWriteLog(szBuf); \
	}
	#define GFX_WRITE_LOG4(szFormat, p1, p2, p3, p4) \
	{ \
		char szBuf[1024]; \
		sprintf(szBuf, szFormat, p1, p2, p3, p4); \
		GfxWriteLog(szBuf); \
	}
	#define GFX_WRITE_LOG5(szFormat, p1, p2, p3, p4, p5) \
	{ \
		char szBuf[1024]; \
		sprintf(szBuf, szFormat, p1, p2, p3, p4, p5); \
		GfxWriteLog(szBuf); \
	}
	#define ON_GFX_COUNT_INC \
	{ \
		_cur_gfx_count++; \
		GFX_WRITE_LOG1("Count: %d", _cur_gfx_count); \
	}
	#define ON_GFX_COUNT_DEC \
	{ \
		_cur_gfx_count--; \
		GFX_WRITE_LOG1("Count: %d", _cur_gfx_count); \
	}

#else
	#define GFX_WRITE_LOG0(szLine)
	#define GFX_WRITE_LOG1(szFormat, p1)
	#define GFX_WRITE_LOG2(szFormat, p1, p2)
	#define GFX_WRITE_LOG3(szFormat, p1, p2, p3)
	#define GFX_WRITE_LOG4(szFormat, p1, p2, p3, p4)
	#define GFX_WRITE_LOG5(szFormat, p1, p2, p3, p4, p5)
	#define ON_GFX_COUNT_INC
	#define ON_GFX_COUNT_DEC

#endif

#define DEFAULT_GFX_FADE_OUT_TIME		1000

class A3DParticle;
class A3DGFXLight;
class A3DGFXModelMan;
class A3DGFXSurfaceRender;
class CECModel;
class ECMMotionBlurPSTable;
class IGFXECMInterface;

typedef abase::vector<A3DGFXLight*> GfxLightParamList;
typedef APtrList<A3DParticle*> ParticlePoolList;

class GfxRenderContainer
{
public:
	//	Attention old none parameter constructor is no longer used
	//	This function would be deleted in next version
	GfxRenderContainer();
	//	Use New version with one A3DGFXExMan* parameter
	explicit GfxRenderContainer(A3DGFXExMan* pGfxExMan);
	~GfxRenderContainer();

	ALISTPOSITION GetHeadPosition() const { return m_GfxList.GetHeadPosition(); }
	ALISTPOSITION GetTailPosition() const { return m_GfxList.GetTailPosition(); }
	A3DGFXEx* GetAt(ALISTPOSITION pos) { return m_GfxList.GetAt(pos); }
	A3DGFXEx* GetNext(ALISTPOSITION& pos) { return m_GfxList.GetNext(pos); }
	
	size_t GetCount() const { return m_GfxList.GetCount(); }
	void AddTail(A3DGFXEx* pGfx);
	void Remove(A3DGFXEx* pGfx);
	bool IsExist(A3DGFXEx* pGfx) const;
	void Clear();

private:

	GfxRenderContainer(const GfxRenderContainer&);
	GfxRenderContainer& operator= (const GfxRenderContainer&);

private:

	typedef abase::hash_map<A3DGFXEx*, ALISTPOSITION> Gfx2PosMap;
	Gfx2PosMap m_GfxMap;
	APtrList<A3DGFXEx*> m_GfxList;
};

class cs_locker
{
public:
	cs_locker()
	{
		::InitializeCriticalSection(&m_cs);
	}
	~cs_locker()
	{
		::DeleteCriticalSection(&m_cs);
	}

	void lock()
	{
		::EnterCriticalSection(&m_cs);
	}
	void unlock()
	{
		::LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

template<class T>
class lock_wrapper
{
public:
	lock_wrapper(T& locker_)
		: m_locker(locker_)
	{
		m_locker.lock();
	}
	~lock_wrapper()
	{
		m_locker.unlock();
	}
private:
	T& m_locker;
};

struct GFX_LOAD_ECM_PARAM;

// deprecated interface, please use the new interface IGFXECMInterface(A3DGFXECMInterface.h) instead.
typedef bool (*ON_GFX_LOAD_ECM_BY_ID)(GFX_LOAD_ECM_PARAM * pLoadSkinParam);

typedef void (*GFX_MULTI_THREAD_LOAD_FUNC)(A3DGFXEx* pGfx, const char* szFile, bool bCache);
typedef void (*CANCEL_GFX_MULTI_THREAD_LOAD_FUNC)(A3DGFXEx* pGfx);

struct ECM_CUSTOM_FX_PARAM_IN
{
	const char* szDefaultPath;
	int			nCustom_data;
	clientid_t  nClient_ID;
};

struct ECM_CUSTOM_FX_PARAM_OUT
{
	char szCustomPath[260];
};

typedef bool (*ECM_CUSTOM_FX_FILE_PATH)(const ECM_CUSTOM_FX_PARAM_IN* pIn, ECM_CUSTOM_FX_PARAM_OUT* pOut);

class A3DGFXExMan
{
public:

	typedef abase::vector<A3DSkinModel*>			ModelArray;
	typedef APtrList<A3DGFXEx*>						GfxList;
	typedef abase::hash_map<int, A3DGFXEx*>			GfxMap;
	typedef APtrList<CECModel*>						ECModelList;
	typedef abase::hash_map<CECModel*, CECModel*>	ECModelMap;
	friend class GfxRenderContainer;
	typedef abase::hash_map<DWORD, AString>			FileHashIDMap;
	typedef abase::hash_map<DWORD, GFX_TICK_TIME>	FileTickTimeMap;

	struct GFX_FILE_TICK_TIME_INFO
	{
		const char* szFileName;
		GFX_TICK_TIME tick_time;
	};

public:

	A3DGFXExMan();
	~A3DGFXExMan();

protected:

	A3DGFXInterface*			m_pGfxInterface;
	A3DDevice*					m_pDevice;
	A3DViewport*				m_pViewport2D;
	A3DOrthoCamera*				m_pCamera2D;
	A3DCamera*					m_pFixed3DCamera;
	bool						m_b2DNoMoveCam;
	bool						m_bUseZBias;
	bool						m_bEnableSfxVolume;

	//	Manager for GFX render container
	//	Each render container should be registered into this manager (for thread reason)
	//	A GFX may be deleted in another thread while at the previous time it has already been
	//	registered into a render container waiting to be rendered
	//	At this time, before we delete this GFX pointer, we need to delete the GFX's pointer
	//	from the render container first
	//	But we know there maybe some user defined Render containers out of control (we can not get them)
	//	That's why we set a render container table here (each render container is registered here),
	//	in order to be notified when a gfx is deleted
	typedef abase::hash_map<GfxRenderContainer*, GfxRenderContainer*> GfxRenderContTbl;
	GfxRenderContTbl			m_RenderContMan;

 	GfxRenderContainer*			m_p2DGfxLstBackLayer;		//	2D GFX Render Container
 	GfxRenderContainer*			m_p2DGfxLstFrontLayer;		//	2D GFX Render Container
	GfxRenderContainer*			m_pGfxLst;					//	Default 3D GFX Render Container
	//	Current using 3D GFX Render Container
	//	May be set to user defined render container
	GfxRenderContainer*			m_pCurGfxLst;				
	
	GfxList						m_ReleasedCacheLst;
	GfxList						m_DelayReleaseLst;
	CachedItemMan<A3DGFXEx*>	m_ItemMan;
	ParticlePoolList			m_ParticlePool;
	A3DGFXStreamMan*			m_pGfxStreamMan;
	A3DGFXSharedTexMan*			m_pTexMan;
	A3DGFXRenderSlotMan*		m_pRenderMan;
	A3DGFXModelMan*				m_pModelMan;
	A3DGFXSurfaceRender*		m_pSurfaceRender;
	int							m_nPriority;
	GfxMap						m_2DGfxMap;
	int							m_nReleasedCacheSize;
	int							m_nMaxCacheGfxCount;
	float						m_f2DStereoDepth;
	ON_GFX_LOAD_ECM_BY_ID		m_pGfxOnLoadECMCallback;
	IGFXECMInterface*			m_pGfxOnLoadECM;

	GFX_MULTI_THREAD_LOAD_FUNC			m_pGfxMultiThreadLoadFunc;
	CANCEL_GFX_MULTI_THREAD_LOAD_FUNC	m_pCancelGfxMultiThreadLoadFunc;
	ECM_CUSTOM_FX_FILE_PATH				m_pCustomFxFilePathFunc;

	struct FadeOutGfx
	{
		DWORD		m_dwTotalTTL;
		DWORD		m_dwTTL;
		A3DGFXEx*	m_pGfx;
		int		m_nDelay;
		float		m_fAlpha;
	};

	typedef APtrList<FadeOutGfx*> FadeOutGfxLst;
	FadeOutGfxLst m_FadeOutGfxLst;
	abase::vector<A3DGFXEx*> m_TempFadeLst;

	GfxElementList		m_WarpEleList;		// 空间扭曲的元素
	GfxElementList		m_PostRenderEleList;// 渲染完成一帧，得到FrameBuffer之后需要处理的元素
	GfxLightParamList	m_LightList;
	bool				m_bShowLightDemo;
	bool				m_bIsInRelease;		// Is in function Release() ? This will decide some functions' behavior
	bool				m_bDisable;			// For debug use only

	ECModelMap			m_MotionBlurECMMap;
	ECMMotionBlurPSTable* m_pMotionBlurPSTbl;

	CRITICAL_SECTION	m_csCache;
	CRITICAL_SECTION	m_csFadeOut;
	CRITICAL_SECTION	m_csGfxRender;
	CRITICAL_SECTION	m_csWarpEleList;
	cs_locker			m_csPostEleList;
	CRITICAL_SECTION	m_csLightList;
	CRITICAL_SECTION	m_csParticlePool;
	CRITICAL_SECTION	m_cs2DGfxMap;
	CRITICAL_SECTION	m_csDelayRelease;
	CRITICAL_SECTION	m_csSfxFileList;

	FileHashIDMap		m_FileHashIDMap;
	FileTickTimeMap		m_FileTickTimeMap;

	bool				m_bMultiThreadMode;
	DWORD				m_dwMainThreadID;

	static A3DGFX_CAMERA_BLUR_INFO	m_CameraBlurInfo;

    A3DRenderTarget*    m_pBloomRenderTarget;
	bool				m_bBloomRendered;

	struct PlayedSfx
	{
		AString		m_strFile;
		int		m_nTTL;
	};

	typedef APtrList<PlayedSfx*>	SfxFileList;
	SfxFileList		m_pSfxFileList;

protected:

	void RegisterGfxCont(GfxRenderContainer* pCont);
	void UnRegisterGfxCont(GfxRenderContainer* pCont);

	void LockCache() { ::EnterCriticalSection(&m_csCache); }
	void UnlockCache() { ::LeaveCriticalSection(&m_csCache); }
	void LockDelayRelease() { ::EnterCriticalSection(&m_csDelayRelease); }
	void UnlockDelayRelease() { ::LeaveCriticalSection(&m_csDelayRelease); }
	void LockGfxRender() { ::EnterCriticalSection(&m_csGfxRender); }
	void UnlockGfxRender() { ::LeaveCriticalSection(&m_csGfxRender); }
	void LockFadeOutList() { ::EnterCriticalSection(&m_csFadeOut); }
	void UnlockFadeOutList() { ::LeaveCriticalSection(&m_csFadeOut); }
	void Lock2DGfxMap() { ::EnterCriticalSection(&m_cs2DGfxMap); }
	void Unlock2DGfxMap() { ::LeaveCriticalSection(&m_cs2DGfxMap); }
	void LockSfxFileList() { ::EnterCriticalSection(&m_csSfxFileList); }
	void UnlockSfxFileList() { ::LeaveCriticalSection(&m_csSfxFileList); }
	bool Init2DViewport();

	void Release2DGfxMap();

	void InnerRenderGfx(GfxRenderContainer& Lst, A3DViewport* pView);
	void InnerRenderGfxWater(GfxRenderContainer& Lst, A3DViewport* pView, float fHeight, bool bUnderWater);

	void CheckDelayReleaseLst();
	
public:

	bool Init(A3DDevice* pDev);

	//	传入指针会由GFX库自行维护
	//	客户端像这样调用即可：
	//	GetA3DGFXExMan()->SetCustomGFXInterface(new GFXInterfaceImpl());
	void SetGfxInterface(A3DGFXInterface* pInterface);
	A3DGFXInterface* GetGfxInterface() const { return m_pGfxInterface; }

	A3DGFXEx* LoadGfx(A3DDevice* pDev, const AString& strPath, bool bCache = true, bool bLoadInThread = true);
	void LoadGfxInLoadThread(A3DGFXEx* pGfx, const AString& strPath, bool bCache);
	void CancelGfxMultiThreadLoad(A3DGFXEx* pGfx) { m_pCancelGfxMultiThreadLoadFunc(pGfx); }
	void Tick(DWORD dwDeltaTime);
	void CacheReleasedGfx(A3DGFXEx* pGfx);

	void InitParticlePool();
	void ReleaseParticlePool();

	bool Is2DViewport(A3DViewport* pViewport);
	A3DCamera* GetFixed3DCamera() const { return m_pFixed3DCamera; }

	//	Change the GFX render container by user / pass NULL to use default container
	//	This would only affect on the Normal 3D GFXs (2D GFX will take no affect from this)
	void SetCustomGfxRenderContainer(GfxRenderContainer* pCustomGfxRenderArray);

	void RegisterGfx(A3DGFXEx* pGfx);
	void RemoveFromQueue(A3DGFXEx* pGfx);

	void AddWarpEle(A3DGFXElement* pEle);
	void RemoveWarpEle(A3DGFXElement* pEle);
	void ClearWarpEleList();

	void RenderDelayedEles(A3DViewport* pViewport, A3DRenderTarget* pFrameBuffer);
	void AddDelayedEle(A3DGFXElement* pEle);
	void RemoveDelayedEle(A3DGFXElement* pEle);
	void ClearDelayedEleList();
	bool IsDelayedEleExists() const;

	void AddLightParam(A3DGFXLight* param)
	{
		if (param->GetAddedToLightQueue())
			return;

		param->SetAddedToLightQueue(true);
		LockLightParamList();
		m_LightList.push_back(param);
		UnlockLightParamList();
	}

	void RemoveLightParam(A3DGFXLight* param)
	{
		if (!param->GetAddedToLightQueue())
			return;

		param->SetAddedToLightQueue(false);

		LockLightParamList();

		for (size_t i = 0; i < m_LightList.size(); i++)
		{
			if (m_LightList[i] == param)
			{
				m_LightList.erase(m_LightList.begin()+i);
				break;
			}
		}

		UnlockLightParamList();
	}

	void ClearLightParamList()
	{
		LockLightParamList();

		for (size_t i = 0; i < m_LightList.size(); i++)
			m_LightList[i]->SetAddedToLightQueue(false);

		m_LightList.clear();
		UnlockLightParamList();
	}


	A3DGFXEx* Get2DGfx(int id)
	{
		A3DGFXEx* p;
		Lock2DGfxMap();
		GfxMap::iterator it = m_2DGfxMap.find(id);

		if (it != m_2DGfxMap.end())
			p = it->second;
		else
			p = 0;

		Unlock2DGfxMap();
		return p;
	}

	int Add2DGfx(const char* szPath, bool bPlay = true);
	void Release2DGfx(int id);
	void Play2DGfx(int id);
	void Stop2DGfx(int id);
	void StopAll2DGfx();
	void ReleaseAll2DGfx();
	void Set2DGfxPos(int id, float x, float y);
	void Show2DGfx(int id, bool bShow);
	void Set2DGfxScale(int id, float fScale);
	A3DViewport* GetViewport2D() { return m_pViewport2D; }
	void Set2DNoMoveCam(bool b) { m_b2DNoMoveCam = b; }
	bool UsingZBias() const { return m_bUseZBias; }
	void SetUseZBias(bool b) { m_bUseZBias = b; }
	bool IsSfxVolumeEnable() const { return m_bEnableSfxVolume; }
	void SetSfxVolumeEnable(bool b) { m_bEnableSfxVolume = b; }
	bool IsAlphaTestEnable() const { return m_pRenderMan->IsAlphaTestEnable(); }
	void SetAlphaTestEnable(bool b) { m_pRenderMan->SetAlphaTestEnable(b); }
	bool IsFogEnable() const { return m_pRenderMan->IsFogEnable(); }
	void SetFogEnable(bool b) { m_pRenderMan->SetFogEnable(b); }

	void QueueFadeOutGfx(A3DGFXEx* pGfx, DWORD dwFadeTime = DEFAULT_GFX_FADE_OUT_TIME, DWORD dwDelayFadeTime = 0);
	void UpdateFadeOutGfxLst(DWORD dwDelta);
	void RenderFadeOutGfxLst(A3DViewport* pView);
	void RenderFadeOutGfxLstWater(A3DViewport* pView, float fHeight, bool bUnderWater );
	void RenderAllGfxWater(A3DViewport* pView, float fHeight, bool bUnderWater, bool bOnlyRegistered = false);
	// bOnlyRegistered标志表示只渲外部注册的效果，内部fadeout等则忽略
	void RenderAllGfx(A3DViewport* pView, bool bOnlyRegistered = false);
	void ClearAllGfxLst();
	void ResetRender();
	void ReleaseFadeOut();
	void RenderAllSkinModels(A3DViewport* pView) {} // obsolete
	void RenderAllGfxAABB(bool bIncludeModel = false); // if call it should be called before RenderAllGfx
	void Release();
	void Resize2DViewport();
	//	p2DViewport: Pass NULL to use default 2DViewport
	//	Or User could specify a different 2DViewport
	void Render2DGfx(bool bBackLayer, A3DViewport* p2DViewport = NULL);
	void Register2DGfx();
	void ReleaseCache(bool bOnlyReleasedGfx = false);
	void SetCacheSize(int nSize) { m_nReleasedCacheSize = nSize; }
	void SetMaxCacheGfxCount(int nCount) { m_nMaxCacheGfxCount = nCount; }
	void Set2DStereoDepth(float fDepth);
	void Disable() { m_bDisable = true; } // For debebug use only
	void Enable() { m_bDisable = false; } // For debebug use only
	void RegisterECMForMotionBlur(CECModel* pModel);
	void RenderECMForMotionBlur(A3DViewport* pViewport);
	void ResetECMForMotionBlur();
	ECMMotionBlurPSTable* GetMotionBlurPSTbl() const { return m_pMotionBlurPSTbl; }

	A3DGFXStreamMan& GetGFXStreamMan() { return *m_pGfxStreamMan; }
	A3DGFXSharedTexMan& GetGFXTexMan() { return *m_pTexMan; }
	A3DGFXRenderSlotMan& GetRenderMan() { return *m_pRenderMan; }
	A3DGFXModelMan& GetModelMan() { return *m_pModelMan; }
	A3DGFXSurfaceRender& GetSurfaceRender() { return *m_pSurfaceRender; }

	void SetPriority(int nPrio) { m_nPriority = nPrio; }
	int GetPriority() { return m_nPriority; }

	void LockWarpEleList() { ::EnterCriticalSection(&m_csWarpEleList); }
	void UnlockWarpEleList() { ::LeaveCriticalSection(&m_csWarpEleList); }
	GfxElementList& GetWarpEleList() { return m_WarpEleList; }
	void LockLightParamList() { ::EnterCriticalSection(&m_csLightList); }
	void UnlockLightParamList() { ::LeaveCriticalSection(&m_csLightList); }
	GfxLightParamList& GetLightParamList() { return m_LightList; }
	void LockParticlePoolList() { ::EnterCriticalSection(&m_csParticlePool); }
	void UnlockParticlePoolList() { ::LeaveCriticalSection(&m_csParticlePool); }
	ParticlePoolList& GetParticlePool() { return m_ParticlePool; }
	bool GetShowLightDemoFlag() const { return m_bShowLightDemo; }
	void SetShowLightDemoFlag(bool b) { m_bShowLightDemo = b; }

	//	Deprecated, Please use the new interface (GetGfxLoadECM / SetGfxLoadECM) instead.
	//	Set / Get Callback functions
	//	This function would be called back by A3DGFXECModel when such a GFX element is loaded, so don't change it in a different thread
	//	Typically, user should set this function pointer during the initializing process and keep it all over the program's lifetime
	ON_GFX_LOAD_ECM_BY_ID	GetGfxLoadSkinByID() const { return m_pGfxOnLoadECMCallback; }
	void SetGfxLoadSkinByID(ON_GFX_LOAD_ECM_BY_ID pFunc) { m_pGfxOnLoadECMCallback = pFunc; }

	void SetGfxMultiThreadLoadFunc(GFX_MULTI_THREAD_LOAD_FUNC pFunc) { m_pGfxMultiThreadLoadFunc = pFunc; }
	void SetCancelGfxMultiThreadLoadFunc(CANCEL_GFX_MULTI_THREAD_LOAD_FUNC pFunc) { m_pCancelGfxMultiThreadLoadFunc = pFunc; }
	ECM_CUSTOM_FX_FILE_PATH GetCustomFxFilePathFunc() { return m_pCustomFxFilePathFunc; }
	void SetCustomFxFilePathFunc(ECM_CUSTOM_FX_FILE_PATH pFunc) { m_pCustomFxFilePathFunc = pFunc; }

	//	New interface for GFX load ECModel, if the m_pGfxOnLoadECM interface is unset, then the CustomECM loading type is not supported
	//	Custom skin loading type will try to GetGfxLoadSkinByID
	IGFXECMInterface* GetGfxECMInterface() const { return m_pGfxOnLoadECM; }
	void SetGfxECMInterface(IGFXECMInterface* pGfxOnLoadECM) { m_pGfxOnLoadECM = pGfxOnLoadECM; }

	void GetGfxTickTimeInfo(abase::vector<GFX_FILE_TICK_TIME_INFO>& aTickTimeInfoVec);

	void SetMultiThreadMode(bool b) { m_bMultiThreadMode = b; }
	void SetMainThreadID(DWORD dwID) { m_dwMainThreadID = dwID; }
	DWORD GetMainThreadID() const { return m_dwMainThreadID; }

    A3DRenderTarget* GetBloomRenderTaregt() const { return m_pBloomRenderTarget; }
    void SetBloomRenderTarget(A3DRenderTarget* pRenderTarget) { m_pBloomRenderTarget = pRenderTarget; }
	bool IsBloomRendered() const { return m_bBloomRendered; }
	void SetBloomRendered(bool b) { m_bBloomRendered = b; }

	static void SetCameraBlurInfo(const A3DGFX_CAMERA_BLUR_INFO& info) { m_CameraBlurInfo = info; }
	static const A3DGFX_CAMERA_BLUR_INFO& GetCameraBlurInfo() { return m_CameraBlurInfo; }

	int GetFadeOutGfxCount() const { return m_FadeOutGfxLst.GetCount(); }
	int GetCachedItemCount() const { return m_ItemMan.GetItemCount(); }
	int GetRealsedCacheCount() const { return m_ReleasedCacheLst.GetCount(); }
	int GetDelayedReleaseCount() const { return m_DelayReleaseLst.GetCount(); }

	static float GetTotalTickTime();
	static float GetTotalRenderTime();
	static int GetTotalGfxCount();
	static int GetRealTotalGfxCount();
	static int GetTotalDrawCount();
	static int GetTotalPrimCount();
	static int GetTotalGfxRenderCount();
	static int GetParticleTotalCount();
	static int GetParticleRenderCount();
	static float GetECMTotalTickTime();
	static void ResetStatCount();
	static int GetParticleActiveCount();
	static int GetPeakParticleActiveCount();
	static void ResetTotalPeakParticleActiveCount();

	static float TranslateZTo2D(float z);

	void UpdateSfxFileList(DWORD delta);
	void PushSfxFile(const AString& strFile, int ttl);;
	void ReleaseSfxFileList();
	bool IsInSfxFileList(const AString& strFile, int& ttl);

//performance
public:
	struct Performance 
	{
		Performance()
			: PrimCount(0), PeakPrimCount(0) {}

		void Clear()
		{
			PrimCount = 0;
		}

		void ClearPeak()
		{
			PeakPrimCount = 0;
		}

		int PrimCount;
		int PeakPrimCount;
	};

	abase::hash_map<int, Performance>& GetElementPerformanceMap() { return m_ElementPerformance; }
	void AddElementPrimCount(int type, int nCount);
	void ResetElementPerformance();
	void ResetElementPeakCount();

	void AddTotalPrimCount(size_t nPrimCount);
	void ResetTotalPeakPrimCount() { m_gfx_peak_prim_count = 0;}
	int GetTotalPeakPrimCount() const { return m_gfx_peak_prim_count; }

private:

	abase::hash_map<int, Performance>		m_ElementPerformance;
	int m_gfx_peak_prim_count;
};

struct TARGET_DATA
{
	clientid_t	idTarget;
	DWORD		dwModifier;
	int			nDamage;
	bool		bIsAttDelay;
	int			nAttDelayTimeIdx;
};

typedef abase::vector<TARGET_DATA> TargetDataVec;

class AM3DSoundBuffer;

#ifdef _ANGELICA21
	extern A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag, DWORD dwTexFlag = 0);
#else
	extern A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag);
#endif

extern A3DDevice* AfxGetA3DDevice();
extern A3DGFXExMan* AfxGetGFXExMan();
extern void AfxReleaseA3DSkinModel(A3DSkinModel* pModel);
extern bool AfxGetModelUpdateFlag();
extern const A3DLIGHTPARAM& AfxGetLightparam();
extern void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel);
extern void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel);
#ifdef _ANGELICA21
extern float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm, bool bUseRayTrace = false);
#else
extern float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm);
#endif
extern void AfxEndShakeCam();
extern bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel);
extern AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile, int nPriority);
extern AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile);
extern void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound);
extern void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound);
extern A3DCamera* AfxGetA3DCamera();
extern void AfxSetCamOffset(const A3DVECTOR3& vOffset);
extern void AfxBeginShakeCam();
extern bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets);
extern void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier);
extern void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier);
extern bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage);
extern float AfxGetAverageFrameRate();
extern bool gGfxGetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount);

inline A3DGFXStreamMan& AfxGetGFXStreamMan() { return AfxGetGFXExMan()->GetGFXStreamMan(); }
inline A3DGFXSharedTexMan& AfxGetGFXTexMan() { return AfxGetGFXExMan()->GetGFXTexMan(); }

extern void eff_stat_init(A3DEngine* pEngine);
extern void eff_stat_prepare();
extern void eff_stat_begin();
extern void eff_stat_end();
extern void eff_stat_report_one(const ACHAR* szName);
extern void eff_stat_flush_output();

extern APhysXScene* gGetAPhysXScene();

#ifdef GFX_EDITOR
#ifndef _SKILLGFXCOMPOSER

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets);
void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier);
void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier);
bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage);

#endif
#endif

#endif
