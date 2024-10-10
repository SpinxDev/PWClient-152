#include "StdAfx.h"
#include "A3DGFXExMan.h"
#include "A3DCamera.h"
#include "AAssist.h"
#include "A3DGFXLight.h"
#include "A3DParticle.h"
#include "A3DOrthoCamera.h"
#include "A3DGFXInterface.h"
#include "EC_ModelBlur.h"

#ifdef A3DPROFILE
#	include "A3DProfile.h"
#else
#	define A3DPROFILE_EVENT( VARIABLE, COLOR, NAME )
#endif

#ifdef _ANGELICA21
static const int _max_cached_item		= 128;
static const int _gfx_render_buf_size	= 256;
static const int _released_cache_size	= 64;
static const int _fadeout_list_size		= 128;
static const int _max_par_pool_size		= 150000;
#else
static const int _max_cached_item		= 128;
static const int _gfx_render_buf_size	= 256;
static const int _released_cache_size	= 64;
static const int _fadeout_list_size		= 128;
static const int _max_par_pool_size		= 20000;
#endif

static const float _2d_cam_pos_z		= 50000.f;
static volatile int _2d_gfx_id			= 1;
static const float _fixed_3d_camera_fov = 45.f;
static const float _fixed_3d_camera_z	= 5.f;

A3DGFX_CAMERA_BLUR_INFO A3DGFXExMan::m_CameraBlurInfo;

float _gfx_stat_tick_time			= 0;
float _gfx_stat_render_time			= 0;
int _gfx_total_count				= 0;
int _gfx_render_count				= 0;
int _gfx_par_total_count			= 0;
int _gfx_par_render_count			= 0;
int _gfx_draw_count					= 0;
int _gfx_prim_count					= 0;
float _ecm_total_tick_time			= 0;
int _gfx_par_active_count			=	0;
int _gfx_par_active_peak_count		=	0;

bool g_bGfxUseTickStat = false;

#include "AMSoundBufferMan.h"
#include "AMSoundEngine.h"

#if GFX_DEBUG_LOG == 1
	const char* _gfx_log_file = "gfx_log.txt";
	int _cur_gfx_count = 0;
#endif

#ifdef GFX_EDITOR
	A3DDevice* g_pDevice = NULL;
	A3DGFXExMan _gfx_ex_man;
#endif


//////////////////////////////////////////////////////////////////////////
//
//	Implement GfxRenderContainer
//
//////////////////////////////////////////////////////////////////////////

GfxRenderContainer::GfxRenderContainer()
: m_GfxList(_gfx_render_buf_size)
{
	AfxGetGFXExMan()->RegisterGfxCont(this);
}

GfxRenderContainer::GfxRenderContainer(A3DGFXExMan* pGfxExMan)
: m_GfxList(_gfx_render_buf_size)
{
	ASSERT( pGfxExMan );
	pGfxExMan->RegisterGfxCont(this);
}

GfxRenderContainer::~GfxRenderContainer()
{
	// Hey you shouldn't release AfxGetGFXExMan first before you release all GfxRenderContainers
	ASSERT( AfxGetGFXExMan() );

	AfxGetGFXExMan()->UnRegisterGfxCont(this);
}

void GfxRenderContainer::AddTail(A3DGFXEx* pGfx) 
{
	if (IsExist(pGfx))
		return;
	
	ALISTPOSITION pos = m_GfxList.AddTail(pGfx);
	m_GfxMap[pGfx] = pos;
}

void GfxRenderContainer::Remove(A3DGFXEx* pGfx)
{
	if (NULL == pGfx)
		return;
	Gfx2PosMap::iterator itr = m_GfxMap.find(pGfx);
	if (itr == m_GfxMap.end())
		return;
	
	m_GfxList.RemoveAt(m_GfxMap[pGfx]);
	m_GfxMap.erase(itr);
}

bool GfxRenderContainer::IsExist(A3DGFXEx* pGfx) const
{
	if (m_GfxMap.find(pGfx) != m_GfxMap.end())
		return true;
	
	return false;
}

void GfxRenderContainer::Clear()
{
	m_GfxMap.clear();
	m_GfxList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DGFXExMan
//
//////////////////////////////////////////////////////////////////////////

void A3DGFXExMan::RegisterGfxCont(GfxRenderContainer* pCont)
{
	if (NULL == pCont) 
		return;
	LockGfxRender();
	m_RenderContMan[pCont] = pCont;
	UnlockGfxRender();
}

void A3DGFXExMan::UnRegisterGfxCont(GfxRenderContainer* pCont)
{
	LockGfxRender();
	GfxRenderContTbl::iterator itr = m_RenderContMan.find(pCont);
	ASSERT( itr != m_RenderContMan.end());
	if (itr != m_RenderContMan.end())
	{
		m_RenderContMan.erase(itr);
	}
	UnlockGfxRender();
}

void A3DGFXExMan::InnerRenderGfx(GfxRenderContainer& Cont, A3DViewport* pView)
{
	LockGfxRender();

	ALISTPOSITION pos = Cont.GetHeadPosition();
	while (pos)
	{
		//	Why this ?
		//	Because the pGfx->Render might call RegisterGfx indirectly
		//	A3DGFXEx might contain some CECModels, and when such A3DGFXECModels render here
		//	they call the CECModel::Render, then RegisterGfx might be called
		//	then there might be new GFX members added to the Cont
		//	Which means after the pGfx->Render called, the Cont's size might changed
		A3DGFXEx* pGfx = Cont.GetAt(pos);
		pGfx->Render(pView);
		Cont.GetNext(pos);
	}

	Cont.Clear();

	UnlockGfxRender();
	
	m_pRenderMan->Render(pView);
}

void A3DGFXExMan::InnerRenderGfxWater( GfxRenderContainer& Cont, A3DViewport* pView, float fHeight, bool bUnderWater )
{
	LockGfxRender();

	ALISTPOSITION pos = Cont.GetHeadPosition();
	while (pos)
	{
		A3DGFXEx* pGfx = Cont.GetAt(pos);
		A3DAABB aabb = pGfx->GetModelAABB();
		if(!pGfx->UsingAABB() || 
			((bUnderWater && aabb.Mins.y <= fHeight) ||
		  (!bUnderWater && aabb.Maxs.y >= fHeight)))
		{
			pGfx->Render(pView);
		}

		Cont.GetNext(pos);
	}

	UnlockGfxRender();

	m_pRenderMan->Render(pView);
}

void A3DGFXExMan::ClearAllGfxLst()
{
	LockGfxRender();

	m_pCurGfxLst->Clear();

	UnlockGfxRender();
}

void A3DGFXExMan::RegisterGfx(A3DGFXEx* pGfx)
{
	if (!pGfx->IsResourceReady() || !pGfx->IsVisible())
		return;

	LockGfxRender();

	if (pGfx->Is2DRender())
	{
		if (pGfx->Is2DBackLayer())
			m_p2DGfxLstBackLayer->AddTail(pGfx);
		else
			m_p2DGfxLstFrontLayer->AddTail(pGfx);
	}
	else
		m_pCurGfxLst->AddTail(pGfx);

	UnlockGfxRender();
}

void A3DGFXExMan::RemoveFromQueue(A3DGFXEx* pGfx)
{
	LockGfxRender();
	
	for (GfxRenderContTbl::iterator itr = m_RenderContMan.begin()
		; itr != m_RenderContMan.end()
		; ++itr)
	{
		GfxRenderContainer* pCont = itr->second;
		ASSERT(pCont);
		if (NULL == pCont)
			continue;
		pCont->Remove(pGfx);
	}

	UnlockGfxRender();
}

void A3DGFXExMan::AddWarpEle(A3DGFXElement* pEle)
{
	if (pEle->GetAddedToWarpQueue())
		return;

	pEle->SetAddedToWarpQueue(true);
	LockWarpEleList();
	m_WarpEleList.push_back(pEle);
	UnlockWarpEleList();
}

void A3DGFXExMan::RemoveWarpEle(A3DGFXElement* pEle)
{
	if (!pEle->GetAddedToWarpQueue())
		return;

	pEle->SetAddedToWarpQueue(false);
	LockWarpEleList();

	for (size_t i = 0; i < m_WarpEleList.size(); i++)
	{
		if (m_WarpEleList[i] == pEle)
		{
			m_WarpEleList.erase(m_WarpEleList.begin()+i);
			break;
		}
	}

	UnlockWarpEleList();
}

void A3DGFXExMan::ClearWarpEleList()
{
	LockWarpEleList();

	for (size_t i = 0; i < m_WarpEleList.size(); i++)
		m_WarpEleList[i]->SetAddedToWarpQueue(false);

	m_WarpEleList.clear();
	UnlockWarpEleList();
}

void A3DGFXExMan::RenderDelayedEles(A3DViewport* pViewport, A3DRenderTarget* pFrameBuffer)
{
	A3DPROFILE_EVENT( RenderDelayedElesEvent, 0xFF3020, L"RenderDelayedElements" );

	ASSERT( m_pDevice && pViewport && pFrameBuffer );
	lock_wrapper<cs_locker> __wrapper(m_csPostEleList);
	for (size_t i = 0; i < m_PostRenderEleList.size(); ++i)
	{
		A3DGFXElement* pEle = m_PostRenderEleList[i];
		A3DSkinModel* pSkinModel = pEle->GetSkinModel();
		if (!pSkinModel)
			continue;

		pEle->RenderSkinModel(pViewport, pSkinModel, pFrameBuffer);
	}

	ClearDelayedEleList();
	m_pDevice->ClearVertexShader();
	m_pDevice->ClearPixelShader();
}

void A3DGFXExMan::AddDelayedEle(A3DGFXElement* pEle)
{
	if (pEle->GetAddedToPostQueue())
		return;

	pEle->SetAddedToPostQueue(true);
	{
		lock_wrapper<cs_locker> __wrapper(m_csPostEleList);
		m_PostRenderEleList.push_back(pEle);
	}
}

void A3DGFXExMan::RemoveDelayedEle(A3DGFXElement* pEle)
{
	lock_wrapper<cs_locker> __wrapper(m_csPostEleList);
	for (size_t i = 0; i < m_PostRenderEleList.size(); ++i)
	{
		if (m_PostRenderEleList[i] == pEle)
		{
			m_PostRenderEleList.erase(m_PostRenderEleList.begin() + i);
			break;
		}
	}
}

void A3DGFXExMan::ClearDelayedEleList()
{
	lock_wrapper<cs_locker> __wrapper(m_csPostEleList);
	for (size_t i = 0; i < m_PostRenderEleList.size(); ++i)
	{
		m_PostRenderEleList[i]->SetAddedToPostQueue(false);
	}
	m_PostRenderEleList.clear();
}

bool A3DGFXExMan::IsDelayedEleExists() const
{
	return !m_PostRenderEleList.empty();
}

void A3DGFXExMan::SetCustomGfxRenderContainer(GfxRenderContainer* pCustomGfxRenderArray)
{
	LockGfxRender();

	if (pCustomGfxRenderArray)
		m_pCurGfxLst = pCustomGfxRenderArray;
	else
		m_pCurGfxLst = m_pGfxLst;

	UnlockGfxRender();
}

A3DGFXExMan::A3DGFXExMan() :
m_ReleasedCacheLst(_released_cache_size),
m_FadeOutGfxLst(_fadeout_list_size),
m_ParticlePool(50000),
m_pGfxOnLoadECMCallback(NULL),
m_pGfxOnLoadECM(NULL)
{
#ifdef GFX_EDITOR
	m_pGfxInterface = new EditorGFXInterfaceDefImpl(NULL);
#else
	m_pGfxInterface = new A3DGFXInterfaceDefImpl();
#endif

	::InitializeCriticalSection(&m_csCache);
	::InitializeCriticalSection(&m_csDelayRelease);
	::InitializeCriticalSection(&m_csFadeOut);
	::InitializeCriticalSection(&m_csGfxRender);
	::InitializeCriticalSection(&m_csWarpEleList);
	::InitializeCriticalSection(&m_csLightList);
	::InitializeCriticalSection(&m_csParticlePool);
	::InitializeCriticalSection(&m_cs2DGfxMap);
	::InitializeCriticalSection(&m_csSfxFileList);

	m_pDevice = NULL;
	m_nPriority = 4;

	m_p2DGfxLstBackLayer = NULL;
	m_p2DGfxLstFrontLayer = NULL;
	m_pGfxLst = NULL;
	m_pCurGfxLst = NULL;
	
	m_WarpEleList.reserve(128);
	m_LightList.reserve(128);
	m_pGfxStreamMan = new A3DGFXStreamMan;
	m_pTexMan = new A3DGFXSharedTexMan;
	m_pRenderMan = new A3DGFXRenderSlotMan;
	m_pModelMan = new A3DGFXModelMan;
	m_pSurfaceRender  = new A3DGFXSurfaceRender;
	m_pMotionBlurPSTbl = new ECMMotionBlurPSTable;
	m_pCamera2D = NULL;
	m_pViewport2D = NULL;
	m_pFixed3DCamera = NULL;
	m_b2DNoMoveCam = false;
	m_bUseZBias = false;
	m_bEnableSfxVolume = false;
	m_bShowLightDemo = false;
	m_bIsInRelease = false;
	m_nReleasedCacheSize = _released_cache_size;
	m_nMaxCacheGfxCount = _max_cached_item;
	m_f2DStereoDepth = 5.0f;
	m_bDisable = false;
	m_bMultiThreadMode = false;
	m_dwMainThreadID = 0;
	m_pGfxMultiThreadLoadFunc = NULL;
	m_pCancelGfxMultiThreadLoadFunc = NULL;
	m_pCustomFxFilePathFunc = NULL;
    m_pBloomRenderTarget = NULL;
	m_bBloomRendered = false;
}

A3DGFXExMan::~A3DGFXExMan()
{
	ASSERT( m_RenderContMan.size() == 0 && "There is still some RenderContainer not delete ?" );
	delete m_pGfxStreamMan;
	delete m_pTexMan;
	delete m_pRenderMan;
	delete m_pModelMan;
	delete m_pSurfaceRender;
	delete m_pGfxInterface;
	delete m_pMotionBlurPSTbl;
	::DeleteCriticalSection(&m_csCache);
	::DeleteCriticalSection(&m_csDelayRelease);
	::DeleteCriticalSection(&m_csFadeOut);
	::DeleteCriticalSection(&m_csGfxRender);
	::DeleteCriticalSection(&m_csWarpEleList);
	::DeleteCriticalSection(&m_csLightList);
	::DeleteCriticalSection(&m_csParticlePool);
	::DeleteCriticalSection(&m_cs2DGfxMap);
	::DeleteCriticalSection(&m_csSfxFileList);
}

bool A3DGFXExMan::Init(A3DDevice* pDev)
{
#ifdef GFX_EDITOR
	g_pDevice = pDev;
#endif

	m_pDevice = pDev;

	m_p2DGfxLstBackLayer = new GfxRenderContainer(this);
	m_p2DGfxLstFrontLayer = new GfxRenderContainer(this);
	m_pGfxLst = new GfxRenderContainer(this);
	m_pCurGfxLst = m_pGfxLst;

	if (!m_pGfxStreamMan->Init(pDev))
		return false;

	if (!m_pTexMan->Init(pDev))
		return false;

	if (!m_pRenderMan->Init(pDev))
		return false;

	if (!m_pSurfaceRender->Init())
		return false;

	if (!m_pMotionBlurPSTbl->Init(pDev))
		return false;

	InitParticlePool();
	Init2DViewport();

	return true;
}

void A3DGFXExMan::SetGfxInterface(A3DGFXInterface* pInterface)
{
	if (m_pGfxInterface)
	{
		delete m_pGfxInterface;
		m_pGfxInterface = NULL;
	}

	if (pInterface == NULL)
		m_pGfxInterface = new A3DGFXInterfaceDefImpl();
	else
		m_pGfxInterface = pInterface;
}

bool A3DGFXExMan::Init2DViewport()
{
	RECT rc;
	::GetClientRect(m_pDevice->GetDeviceWnd(), &rc);
	DWORD w = rc.right - rc.left;
	DWORD h = rc.bottom - rc.top;
	float hw = w / 2.0f;
	float hh = h / 2.0f;
	m_pCamera2D = new A3DOrthoCamera;

	if (!m_pCamera2D->Init(m_pDevice, -hw, hw, -hh, hh, -100000.f, 100000.f))
	{
		delete m_pCamera2D;
		m_pCamera2D = 0;
		return false;
	}

	if (m_b2DNoMoveCam)
		m_pCamera2D->SetPos(A3DVECTOR3(0, 0, _2d_cam_pos_z));
	else
		m_pCamera2D->SetPos(A3DVECTOR3(hw, hh, _2d_cam_pos_z));

	m_pCamera2D->SetDirAndUp(-_unit_z, -_unit_y);
	Set2DStereoDepth(m_f2DStereoDepth);

	A3DVIEWPORTPARAM ViewParams;
	ViewParams.X		= 0;
	ViewParams.Y		= 0;
	ViewParams.Width	= w;
	ViewParams.Height	= h;
	ViewParams.MinZ		= 0;
	ViewParams.MaxZ		= 1.0f;
	m_pViewport2D = new A3DViewport;

	if (!m_pViewport2D->Init(m_pDevice, &ViewParams, false, false, 0))
	{
		delete m_pViewport2D;
		m_pViewport2D = 0;
		return false;
	}

	m_pViewport2D->SetCamera(m_pCamera2D);


	//////////////////////////////////////////////////////////////////////////
	//	add this camera for rendering objects that is in 2D render
	//	but actually uses a 3d fixed camera in order to get a 3D feeling

	ASSERT(!m_pFixed3DCamera);
	m_pFixed3DCamera = new A3DCamera();
	if (!m_pFixed3DCamera->Init(m_pDevice, DEG2RAD(_fixed_3d_camera_fov), 0.1f, 100000.f, (float)w / h))
		return false;



	if (m_b2DNoMoveCam)
		m_pFixed3DCamera->SetPos(A3DVECTOR3(0, 0, _fixed_3d_camera_z));
	else
		m_pFixed3DCamera->SetPos(A3DVECTOR3(hw, hh, _fixed_3d_camera_z));

	m_pFixed3DCamera->SetDirAndUp(-_unit_z, -_unit_y);

	//
	//////////////////////////////////////////////////////////////////////////

	return true;
}


void A3DGFXExMan::Release()
{
	m_bIsInRelease = true;

	Release2DGfxMap();

	LockGfxRender();
	if (m_pGfxLst)
		m_pGfxLst->Clear();
	
	if (m_p2DGfxLstBackLayer)
		m_p2DGfxLstBackLayer->Clear();
	
	if (m_p2DGfxLstFrontLayer)
		m_p2DGfxLstFrontLayer->Clear();
	UnlockGfxRender();

	delete m_pGfxLst;
	m_pGfxLst = NULL;
	delete m_p2DGfxLstBackLayer;
	m_p2DGfxLstBackLayer = NULL;
	delete m_p2DGfxLstFrontLayer;
	m_p2DGfxLstFrontLayer = NULL;
	m_pCurGfxLst = NULL;

	ClearWarpEleList();
	ClearDelayedEleList();
	ClearLightParamList();

	//	We must first release all fadeout gfxs then cached gfxs
	//	For during the fadeout gfx is releasing, the cached release queue may get some more members
	//	Also we changed the behavior of QueueFadeOutGfx in order to change them to the CachedRelease queue
	ReleaseFadeOut();
	ReleaseCache();

	ASSERT(m_FadeOutGfxLst.GetCount() == 0 && "Now there should be no fade out gfx.");
	ASSERT(m_ReleasedCacheLst.GetCount() == 0 && "Now there should be no gfx cached to be released.");

	CheckDelayReleaseLst();
	assert(m_ReleasedCacheLst.GetCount() == 0);

	//	After Release all cached / fading out gfx, do release particle pool
	//	For there may be some particle type GFX element put some particles to this pool
	//	During the ReleaseFadeOut / ReleaseCache
	ReleaseParticlePool();
	
	ReleaseSfxFileList();

	m_pGfxStreamMan->Release();
	m_pTexMan->Release();
	m_pRenderMan->Release();
	m_pModelMan->Release();
	m_pSurfaceRender->Release();
	m_pMotionBlurPSTbl->Release();

	A3DRELEASE(m_pFixed3DCamera);
	A3DRELEASE(m_pCamera2D);
	A3DRELEASE(m_pViewport2D);
	m_pDevice = NULL;
	m_FileHashIDMap.clear();
	m_bIsInRelease = false;
}

void A3DGFXExMan::Release2DGfxMap()
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.begin();

	for (; it != m_2DGfxMap.end(); ++it)
	{
		it->second->Release();
		delete it->second;
	}

	m_2DGfxMap.clear();
	Unlock2DGfxMap();
}

void A3DGFXExMan::ReleaseFadeOut()
{
#ifdef _ANGELICA21

	// 只能在主线程调用，有的gfx可能在渲染
	if (m_dwMainThreadID && m_dwMainThreadID != ::GetCurrentThreadId())
	{
		assert(false);
		return;
	}

#endif

	LockFadeOutList();

	while (m_FadeOutGfxLst.GetCount())
	{
		FadeOutGfx* pFade = m_FadeOutGfxLst.RemoveHead();
		pFade->m_pGfx->Release();
		delete pFade->m_pGfx;
		delete pFade;
	}

	UnlockFadeOutList();
}

void A3DGFXExMan::ReleaseCache(bool bOnlyReleasedGfx)
{
	LockCache();

	if (!bOnlyReleasedGfx)
	{
		while (m_ItemMan.GetItemCount())
		{
			A3DGFXEx* pGFX = m_ItemMan.RemoveHead();
			pGFX->Release();
			delete pGFX;
		}
	}

	while (m_ReleasedCacheLst.GetCount())
	{
		//	Why this ?
		//	Because pGfx->Release() may make a ECModel::Release happen by release an A3DGFXECModel element
		//	Then the A3DGFXECModel::Release() will release all its coGfxs and call CacheReleaseGfx
		//	Then the ReleasedCacheLst will be added a new element at tail 
		//	(sometimes it may remove the head item and delete that item, which may already be deleted here)
		A3DGFXEx* pGfx = m_ReleasedCacheLst.RemoveHead();
		pGfx->Release();
		delete pGfx;
	}

	UnlockCache();
}

void A3DGFXExMan::Set2DStereoDepth(float fDepth)
{
	m_f2DStereoDepth = fDepth;
	A3DMATRIX4 matPostProject = IdentityMatrix() * m_f2DStereoDepth;
	m_pCamera2D->SetPostProjectTM(matPostProject);
}

void A3DGFXExMan::Resize2DViewport()
{
	if (!m_pViewport2D || !m_pCamera2D || !m_pFixed3DCamera)
		return;

	RECT rc;
	::GetClientRect(m_pDevice->GetDeviceWnd(), &rc);
	int iWidth = rc.right - rc.left;
	int iHeight = rc.bottom - rc.top;

	A3DVIEWPORTPARAM vp = *m_pViewport2D->GetParam();
	vp.Width = iWidth;
	vp.Height = iHeight;
	m_pViewport2D->SetParam(&vp);

	float w = (float)vp.Width / 2.f;
	float h = (float)vp.Height / 2.f;
	float zn = m_pCamera2D->GetZFront();
	float zf = m_pCamera2D->GetZBack();
	m_pCamera2D->SetProjectionParam(-w, w, -h, h, zn, zf);

	if (!m_b2DNoMoveCam)
		m_pCamera2D->SetPos(A3DVECTOR3(w, h, _2d_cam_pos_z));

	m_pFixed3DCamera->SetProjectionParam(DEG2RAD(_fixed_3d_camera_fov), 0.1f, 100000.f, w / h);

	if (!m_b2DNoMoveCam)
		m_pFixed3DCamera->SetPos(A3DVECTOR3(w, h, _fixed_3d_camera_z));
}

void A3DGFXExMan::Register2DGfx()
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.begin();

	for (; it != m_2DGfxMap.end(); ++it)
		RegisterGfx(it->second);

	Unlock2DGfxMap();
}

void A3DGFXExMan::Render2DGfx(bool bBackLayer, A3DViewport* p2DViewport)
{
	A3DPROFILE_EVENT( Render2DGfxEvent, 0xFF60FF, L"Render2DGfx" );

	A3DViewport* pCurViewport = m_pDevice->GetA3DEngine()->GetActiveViewport();
	A3DViewport* pUse2DViewport = NULL;
	A3DCameraBase* pOldCamera = NULL;
	
	if (p2DViewport)
	{
		// 如果传入了一个自定义的Viewport
		// 则假设目前的所有2DGFX都是位于该Viewport的坐标系当中的
		// 由于我们使用内置的2D摄像机渲染这些2DGFX，所有的2DGFX坐标都必须是按照屏幕坐标设置(2DGFX渲染的时候世界坐标被当作屏幕坐标使用了)。
		// 因此，为了保证把2DGFX渲染到正确的位置上，需要对这些位置做一个从自定义视口到窗口视口的缩放
		float fWScale = p2DViewport->GetParam()->Width * 1.f / m_pViewport2D->GetParam()->Width;
		float fHScale = p2DViewport->GetParam()->Height * 1.f / m_pViewport2D->GetParam()->Height;

		if (bBackLayer)
		{
			ALISTPOSITION pos = m_p2DGfxLstBackLayer->GetHeadPosition();
			while (pos)
			{
				A3DGFXEx* pGFX = m_p2DGfxLstBackLayer->GetNext(pos);
				A3DVECTOR3 vPos((pGFX->GetPos().x) / fWScale
					, (pGFX->GetPos().y) / fHScale
					, pGFX->GetPos().z);
				
				pGFX->SetPos(vPos);
			}
		}
		else
		{
			ALISTPOSITION pos = m_p2DGfxLstFrontLayer->GetHeadPosition();
			while (pos)
			{
				A3DGFXEx* pGFX = m_p2DGfxLstFrontLayer->GetNext(pos);
				A3DVECTOR3 vPos((pGFX->GetPos().x) / fWScale
					, (pGFX->GetPos().y) / fHScale
					, pGFX->GetPos().z);
				
				pGFX->SetPos(vPos);
			}
		}


		pUse2DViewport = p2DViewport;
		pOldCamera = pUse2DViewport->GetCamera();
		pUse2DViewport->SetCamera(m_pCamera2D);
	}
	else
		pUse2DViewport = m_pViewport2D;

	pUse2DViewport->Active();
	bool bFogEnable = m_pDevice->GetFogEnable();
	m_pDevice->SetFogEnable(false);

	if (bBackLayer)
		InnerRenderGfx(*m_p2DGfxLstBackLayer, pUse2DViewport);
	else
		InnerRenderGfx(*m_p2DGfxLstFrontLayer, pUse2DViewport);

	if (pCurViewport)
		pCurViewport->Active();

	if (pOldCamera)
		p2DViewport->SetCamera(pOldCamera);

	m_pDevice->SetFogEnable(bFogEnable);
}

bool A3DGFXExMan::Is2DViewport(A3DViewport* pViewport)
{
	return m_pViewport2D == pViewport;
}

void A3DGFXExMan::InitParticlePool()
{
	LockParticlePoolList();

	for (int i = 0; i < _max_par_pool_size; i++)
		m_ParticlePool.AddTail(new A3DParticle);

	extern volatile int g_nCurParticlePoolSize;
	g_nCurParticlePoolSize = _max_par_pool_size;
	UnlockParticlePoolList();
}

void A3DGFXExMan::ReleaseParticlePool()
{
	LockParticlePoolList();

	ALISTPOSITION pos = m_ParticlePool.GetHeadPosition();
	while (pos)
	{
		A3DParticle* p = m_ParticlePool.GetNext(pos);
		assert(p->m_pDummy == NULL);
		delete p;
	}

	m_ParticlePool.RemoveAll();
	UnlockParticlePoolList();
}

A3DGFXEx* A3DGFXExMan::LoadGfx(A3DDevice* pDev, const AString& strPath, bool bCache, bool bLoadInThread)
{
	if (m_bDisable)
		return NULL;

	if (strPath.IsEmpty())
		return NULL;

#ifdef FUNCTION_ANALYSER
	ATTACH_FUNCTION_ANALYSER(true, 5, (int)bLoadInThread, strPath);
#endif

#ifdef GFX_EDITOR
	bCache = false;
#endif

	A3DGFXEx* pGfx;
	DWORD dwFileID  = a_MakeIDFromString(strPath);

	if (g_bGfxUseTickStat)
	{
		FileHashIDMap::iterator itID = m_FileHashIDMap.find(dwFileID);

		if (itID == m_FileHashIDMap.end())
			m_FileHashIDMap[dwFileID] = strPath;
	}

	if (!bCache)
	{
		pGfx = new A3DGFXEx;
		pGfx->m_strFile = strPath;
		pGfx->SetFileID(dwFileID);

		if (g_bGfxUseTickStat)
			pGfx->m_pTickTime = &m_FileTickTimeMap[dwFileID];

		if (m_bMultiThreadMode && bLoadInThread && ::GetCurrentThreadId() == m_dwMainThreadID)
		{
			m_pGfxMultiThreadLoadFunc(pGfx, strPath, false);
		}
		else if (!pGfx->Load(pDev, "Gfx\\" + strPath))
		{
			delete pGfx;
			return NULL;
		}
		else if (m_bMultiThreadMode && bLoadInThread)
			pGfx->StartElements();

		return pGfx;
	}

	LockCache();
	ALISTPOSITION pos = m_ReleasedCacheLst.GetHeadPosition();

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pGfx = m_ReleasedCacheLst.GetNext(pos);

		if (pGfx->GetFileID() == dwFileID)
		{
			m_ReleasedCacheLst.RemoveAt(posCur);
			UnlockCache();
			pGfx->SetPlaySpeed(1.0f);
			pGfx->SetScale(1.0f);
			pGfx->SetAlpha(1.0f);
			pGfx->SetVisible(true);
			pGfx->SetFadingOut(false);
			return pGfx;
		}
	}

	if (m_bMultiThreadMode && bLoadInThread && ::GetCurrentThreadId() == m_dwMainThreadID)
	{
		UnlockCache();
		pGfx = new A3DGFXEx;
		pGfx->m_strFile = strPath;
		pGfx->SetFileID(dwFileID);

		if (g_bGfxUseTickStat)
			pGfx->m_pTickTime = &m_FileTickTimeMap[dwFileID];

		m_pGfxMultiThreadLoadFunc(pGfx, strPath, true);
		return pGfx;
	}

	A3DGFXEx* pCache = m_ItemMan.TouchItem(strPath);

	if (pCache)
	{
		pGfx = pCache->Clone();
		UnlockCache();

		if (m_bMultiThreadMode && bLoadInThread)
			pGfx->StartElements();

		return pGfx;
	}

	if (m_ItemMan.GetItemCount() == m_nMaxCacheGfxCount)
	{
		pCache = m_ItemMan.RemoveLRUItem();
		pCache->Release();
		delete pCache;
	}

	pCache = new A3DGFXEx;

	if (!pCache->Load(pDev, "Gfx\\" + strPath))
	{
		delete pCache;
		UnlockCache();
		return NULL;
	}

	pCache->SetFileID(dwFileID);

	if (g_bGfxUseTickStat)
		pCache->m_pTickTime = &m_FileTickTimeMap[dwFileID];

	m_ItemMan.AddItem(strPath, pCache);
	pGfx = pCache->Clone();
	UnlockCache();

	if (m_bMultiThreadMode && bLoadInThread)
		pGfx->StartElements();

	return pGfx;
}

void A3DGFXExMan::LoadGfxInLoadThread(A3DGFXEx* pGfx, const AString& strPath, bool bCache)
{
	if (pGfx->m_bToRelease)
		return;

	if (bCache)
	{
		LockCache();
		A3DGFXEx* pCache = m_ItemMan.TouchItem(strPath);

		if (pCache)
		{
			*pGfx = *pCache;
			UnlockCache();
			pGfx->StartElements();
			pGfx->SetResourceReady(true);
			return;
		}

		if (m_ItemMan.GetItemCount() == m_nMaxCacheGfxCount)
		{
			pCache = m_ItemMan.RemoveLRUItem();
			pCache->Release();
			delete pCache;
		}

		pCache = new A3DGFXEx;

		if (!pCache->Load(m_pDevice, "Gfx\\" + strPath))
		{
			delete pCache;
			UnlockCache();
			return;
		}

		// 保留之前的一些不想被默认值替换的数据
		pCache->SetFileID(pGfx->GetFileID());
		pCache->SetDisableCamShake(pGfx->IsDisableCamShake());

		pCache->m_pTickTime = pGfx->m_pTickTime;
		m_ItemMan.AddItem(strPath, pCache);
		*pGfx = *pCache;
		UnlockCache();
		pGfx->StartElements();
		pGfx->SetResourceReady(true);
	}
	else
	{
		if (pGfx->Load(m_pDevice, "Gfx\\" + strPath))
		{
			pGfx->StartElements();
			pGfx->SetResourceReady(true);
		}
	}
}

void A3DGFXExMan::CacheReleasedGfx(A3DGFXEx* pGfx)
{
	if (pGfx == NULL)
		return;

	if (pGfx->IsLoadingInThread())
	{
		pGfx->m_bToRelease = true;
		LockDelayRelease();
		m_DelayReleaseLst.AddTail(pGfx);
		UnlockDelayRelease();
		return;
	}

	if (!pGfx->IsResourceReady())
	{
		pGfx->Release();
		delete pGfx;
		return;
	}

	assert(pGfx->GetFileID());
	pGfx->Stop();
	pGfx->SetId(0, 0);

	LockCache();

	while (m_ReleasedCacheLst.GetCount() >= m_nReleasedCacheSize)
	{
		A3DGFXEx* pGfx = m_ReleasedCacheLst.RemoveHead();
		pGfx->Release();
		delete pGfx;
	}

	m_ReleasedCacheLst.AddTail(pGfx);
	UnlockCache();
}

void A3DGFXExMan::QueueFadeOutGfx(A3DGFXEx* pGfx, DWORD dwFadeTime /*= DEFAULT_GFX_FADE_OUT_TIME*/, DWORD dwDelayFadeTime /*= 0*/)
{
	if (!pGfx)
		return;

	//	if this call happens during the A3DGFXExMan::Release()
	//	it's time to send all Gfx that is required to be fade out directly to the release queue.
	if (m_bIsInRelease)
	{
		CacheReleasedGfx(pGfx);
		return;
	}

#ifdef _ANGELICA21
	if (dwFadeTime > 2000)
		dwFadeTime = 2000;
#endif

	FadeOutGfx* pFadeOut = new FadeOutGfx;
	pFadeOut->m_dwTotalTTL = pFadeOut->m_dwTTL = dwFadeTime;
	pFadeOut->m_pGfx = pGfx;
	pFadeOut->m_fAlpha = pGfx->GetAlpha();
	pFadeOut->m_nDelay = (int)dwDelayFadeTime;
	pGfx->SetFadingOut(true);

	LockFadeOutList();
	m_FadeOutGfxLst.AddTail(pFadeOut);
	UnlockFadeOutList();
}

void A3DGFXExMan::CheckDelayReleaseLst()
{
	LockDelayRelease();

	A3DGFXEx* pGfx;
	ALISTPOSITION pos = m_DelayReleaseLst.GetHeadPosition();

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pGfx = m_DelayReleaseLst.GetNext(pos);

		if (!pGfx->IsLoadingInThread())
		{
			m_DelayReleaseLst.RemoveAt(posCur);
			pGfx->Release();
			delete pGfx;
		}
	}

	UnlockDelayRelease();
}

void A3DGFXExMan::Tick(DWORD dwDeltaTime)
{
	UpdateFadeOutGfxLst(dwDeltaTime);

	Lock2DGfxMap();

	GfxMap::iterator it = m_2DGfxMap.begin();

	while (it != m_2DGfxMap.end())
	{
		A3DGFXEx* pGfx = it->second;

		if (pGfx->GetState() == ST_STOP)
		{
			CacheReleasedGfx(pGfx);
			it = m_2DGfxMap.erase(it);
		}
		else
		{
			pGfx->TickAnimation(dwDeltaTime);
			++it;
		}
	}

	Unlock2DGfxMap();

	CheckDelayReleaseLst();

	UpdateSfxFileList(dwDeltaTime);
}

void A3DGFXExMan::RenderAllGfxWater( A3DViewport* pView, float fHeight, bool bUnderWater, bool bOnlyRegistered /*= false*/ )
{
	A3DPROFILE_EVENT( RenderAllGfxWaterEvent, 0x0060FF, L"RenderAllGfxWater" );
	AGPA_RECORD_FUNCTION_TIME;

	if (!bOnlyRegistered)
		RenderFadeOutGfxLstWater(pView, fHeight, bUnderWater);

	ClearWarpEleList();
	ClearDelayedEleList();
	InnerRenderGfxWater(*m_pCurGfxLst, pView, fHeight, bUnderWater);
}

void A3DGFXExMan::RenderAllGfx(A3DViewport* pView, bool bOnlyRegistered)
{
	A3DPROFILE_EVENT( RenderAllGfxEvent, 0x00FFFF, L"RenderAllGfx" );
	AGPA_RECORD_FUNCTION_TIME;

	if (!bOnlyRegistered)
		RenderFadeOutGfxLst(pView);

	ClearWarpEleList();
	ClearDelayedEleList();
	InnerRenderGfx(*m_pCurGfxLst, pView);
}

void A3DGFXExMan::ResetRender()
{
	ClearWarpEleList();
	ClearDelayedEleList();

	LockGfxRender();
	m_pCurGfxLst->Clear();
	UnlockGfxRender();

	m_CameraBlurInfo.Reset();
	m_bBloomRendered = false;
}

void A3DGFXExMan::RenderAllGfxAABB(bool bIncludeModel)
{
	A3DPROFILE_EVENT( RenderAllGfxAABBEvent, 0x303030, L"RenderAllGfxAABB" );

	LockGfxRender();

	m_pDevice->GetA3DEngine()->GetA3DWireCollector()->Flush();

	ALISTPOSITION pos = m_pCurGfxLst->GetHeadPosition();
	while (pos)
	{
		A3DGFXEx* pGfx = m_pCurGfxLst->GetNext(pos);
		m_pDevice->GetA3DEngine()->GetA3DWireCollector()->AddAABB(bIncludeModel ? pGfx->GetModelAABB() : pGfx->GetAABB(), A3DCOLORRGBA(255, 0, 0, 255));
	}

	m_pDevice->GetA3DEngine()->GetA3DWireCollector()->Flush();

	UnlockGfxRender();
}

void A3DGFXExMan::UpdateFadeOutGfxLst(DWORD dwDelta)
{
	m_TempFadeLst.clear();
	LockFadeOutList();

	ALISTPOSITION pos = m_FadeOutGfxLst.GetHeadPosition();
	FadeOutGfx* pFadeOut;

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pFadeOut = m_FadeOutGfxLst.GetNext(pos);
		A3DGFXEx* pGfx = pFadeOut->m_pGfx;

		if (pGfx->GetState() == ST_STOP || pFadeOut->m_dwTTL <= dwDelta)
		{
			m_TempFadeLst.push_back(pGfx);
			delete pFadeOut;
			m_FadeOutGfxLst.RemoveAt(posCur);
		}
		else if (pFadeOut->m_nDelay > 0)
		{
			pFadeOut->m_nDelay -= (int)dwDelta;
			pGfx->TickAnimation(dwDelta);
		}
		else
		{
			pFadeOut->m_dwTTL -= dwDelta;
			pGfx->SetAlpha(pFadeOut->m_fAlpha * pFadeOut->m_dwTTL / pFadeOut->m_dwTotalTTL);
			pGfx->TickAnimation(dwDelta);
		}
	}

	UnlockFadeOutList();

	for (size_t i = 0; i < m_TempFadeLst.size(); i++)
		CacheReleasedGfx(m_TempFadeLst[i]);
}

void A3DGFXExMan::RenderFadeOutGfxLst(A3DViewport* pView)
{
	A3DPROFILE_EVENT( RenderFadeOutGfxLstEvent, 0xFF00FF, L"RenderFadeOutGfxList" );

	LockFadeOutList();

	ALISTPOSITION pos = m_FadeOutGfxLst.GetHeadPosition();
	FadeOutGfx* pFadeOut;

	while (pos)
	{
		pFadeOut = m_FadeOutGfxLst.GetNext(pos);
		RegisterGfx(pFadeOut->m_pGfx);
	}

	UnlockFadeOutList();
}


void A3DGFXExMan::RenderFadeOutGfxLstWater( A3DViewport* pView, float fHeight, bool bUnderWater )
{
	A3DPROFILE_EVENT( RenderFadeOutGfxLstWaterEvent, 0xFF0000, L"RenderFadeOutGfxListWater" );

	LockFadeOutList();

	ALISTPOSITION pos = m_FadeOutGfxLst.GetHeadPosition();
	FadeOutGfx* pFadeOut;

	while (pos)
	{
		pFadeOut = m_FadeOutGfxLst.GetNext(pos);
		A3DAABB aabb = pFadeOut->m_pGfx->GetModelAABB();
		if((bUnderWater && aabb.Mins.y <= fHeight) ||
			(!bUnderWater && aabb.Maxs.y >= fHeight))
		{
			RegisterGfx(pFadeOut->m_pGfx);
		}	
	}

	UnlockFadeOutList();
}

void A3DGFXExMan::RegisterECMForMotionBlur(CECModel* pModel)
{
	m_MotionBlurECMMap[pModel] = pModel; 
}

void A3DGFXExMan::RenderECMForMotionBlur(A3DViewport* pViewport)
{
	if (m_MotionBlurECMMap.empty())
		return;

	A3DPROFILE_EVENT( RenderECMForMotionBlurEvent, 0x00FFFF, L"RenderECMForMotionBlur" );

	m_pDevice->SetZWriteEnable(false);
	DWORD dwZFunc = m_pDevice->GetDeviceRenderState(D3DRS_ZFUNC);
	m_pDevice->SetZFunc(A3DCMP_LESS);
	
	for (ECModelMap::iterator itr = m_MotionBlurECMMap.begin()
		; itr != m_MotionBlurECMMap.end()
		; ++itr)
	{
		itr->first->RenderMotionBlur(pViewport);
	}

	m_pDevice->SetZWriteEnable(true);
	m_pDevice->SetZFunc(dwZFunc);
	m_pDevice->ClearVertexShader();
	m_pDevice->ClearPixelShader();
}

void A3DGFXExMan::ResetECMForMotionBlur()
{
	m_MotionBlurECMMap.clear();
}

int A3DGFXExMan::Add2DGfx(const char* szPath, bool bPlay)
{
	A3DGFXEx* pGfx = LoadGfx(m_pDevice, szPath);

	if (!pGfx)
		return 0;

	if (!pGfx->Is2DRender())
	{
		pGfx->Release();
		delete pGfx;
		return 0;
	}

	Lock2DGfxMap();

	int id = _2d_gfx_id;
	m_2DGfxMap[id] = pGfx;
	_2d_gfx_id++;

	if (bPlay)
		pGfx->Start();

	Unlock2DGfxMap();
	return id;
}

void A3DGFXExMan::Release2DGfx(int id)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
	{
		CacheReleasedGfx(it->second);
		m_2DGfxMap.erase(it);
	}

	Unlock2DGfxMap();
}

void A3DGFXExMan::Play2DGfx(int id)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
		it->second->Start(true);

	Unlock2DGfxMap();
}

void A3DGFXExMan::Stop2DGfx(int id)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
		it->second->Stop();

	Unlock2DGfxMap();
}

void A3DGFXExMan::StopAll2DGfx()
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.begin();

	for (; it != m_2DGfxMap.end(); ++it)
		it->second->Stop();

	Unlock2DGfxMap();
}

void A3DGFXExMan::ReleaseAll2DGfx()
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.begin();

	for (; it != m_2DGfxMap.end(); ++it)
		CacheReleasedGfx(it->second);

	m_2DGfxMap.clear();
	Unlock2DGfxMap();
}

void A3DGFXExMan::Set2DGfxPos(int id, float x, float y)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
		it->second->SetPos(A3DVECTOR3(x, y, 0));

	Unlock2DGfxMap();
}

void A3DGFXExMan::Show2DGfx(int id, bool bShow)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
		it->second->SetVisible(bShow);

	Unlock2DGfxMap();
}

void A3DGFXExMan::Set2DGfxScale(int id, float fScale)
{
	Lock2DGfxMap();
	GfxMap::iterator it = m_2DGfxMap.find(id);

	if (it != m_2DGfxMap.end())
		it->second->SetScale(fScale);

	Unlock2DGfxMap();
}

float A3DGFXExMan::GetTotalTickTime()
{
	return _gfx_stat_tick_time;
}

float A3DGFXExMan::GetTotalRenderTime()
{
	return _gfx_stat_render_time;
}

int A3DGFXExMan::GetTotalGfxCount()
{
	A3DGFXExMan* pMan = AfxGetGFXExMan();
	return _gfx_total_count - pMan->m_ReleasedCacheLst.GetCount() - pMan->m_ItemMan.GetItemCount();
}

int A3DGFXExMan::GetRealTotalGfxCount()
{
	return _gfx_total_count;
}

int A3DGFXExMan::GetTotalDrawCount()
{
	return _gfx_draw_count;
}

int A3DGFXExMan::GetTotalPrimCount()
{
	return _gfx_prim_count;
}

int A3DGFXExMan::GetTotalGfxRenderCount()
{
	return _gfx_render_count;
}

int A3DGFXExMan::GetParticleTotalCount()
{
	return _gfx_par_total_count;
}

int A3DGFXExMan::GetParticleRenderCount()
{
	return _gfx_par_render_count;
}

float A3DGFXExMan::GetECMTotalTickTime()
{
	return _ecm_total_tick_time;
}

int A3DGFXExMan::GetParticleActiveCount()
{
	return _gfx_par_active_count;
}

int A3DGFXExMan::GetPeakParticleActiveCount()
{
	if (_gfx_par_active_peak_count < _gfx_par_active_count)
	{
		_gfx_par_active_peak_count = _gfx_par_active_count;
	}
	return _gfx_par_active_peak_count;
}

void A3DGFXExMan::ResetTotalPeakParticleActiveCount()
{
	_gfx_par_active_peak_count = 0;
}

void A3DGFXExMan::ResetStatCount()
{
	_gfx_stat_tick_time		= 0;
	_gfx_stat_render_time	= 0;
	_gfx_render_count		= 0;
	_gfx_par_render_count	= 0;
	_gfx_draw_count			= 0;
	_gfx_prim_count			= 0;
	_ecm_total_tick_time	= 0;
	_gfx_par_active_count	= 0;

	if (g_bGfxUseTickStat)
	{
		FileTickTimeMap& info_map = AfxGetGFXExMan()->m_FileTickTimeMap;
		FileTickTimeMap::iterator it = info_map.begin();

		for (; it != info_map.end(); ++it)
		{
			GFX_TICK_TIME& t = it->second;
			t.dwCount = 0;
			t.dwTickTime = 0;
		}
	}
}


bool _compare_gfx_tick_time_info(const A3DGFXExMan::GFX_FILE_TICK_TIME_INFO& info1, const A3DGFXExMan::GFX_FILE_TICK_TIME_INFO& info2)
{
	return info1.tick_time.dwTickTime > info2.tick_time.dwTickTime;
}

void A3DGFXExMan::GetGfxTickTimeInfo(abase::vector<GFX_FILE_TICK_TIME_INFO>& aTickTimeInfoVec)
{
	if (!g_bGfxUseTickStat)
		return;

	FileTickTimeMap::iterator it = m_FileTickTimeMap.begin();

	for (; it != m_FileTickTimeMap.end(); ++it)
	{
		GFX_TICK_TIME& t = it->second;

		if (t.dwCount == 0)
			continue;

		GFX_FILE_TICK_TIME_INFO info;
		info.szFileName = m_FileHashIDMap[it->first];
		info.tick_time = t;
		aTickTimeInfoVec.push_back(info);
	}

	std::sort(aTickTimeInfoVec.begin(), aTickTimeInfoVec.end(), _compare_gfx_tick_time_info);
}

float A3DGFXExMan::TranslateZTo2D(float z)
{
	return 100000.f + _2d_cam_pos_z - 200000.f * z;
}

void A3DGFXExMan::AddElementPrimCount( int type, int nCount )
{
	m_ElementPerformance[type].PrimCount += nCount;
	if (m_ElementPerformance[type].PrimCount > m_ElementPerformance[type].PeakPrimCount)
	{
		m_ElementPerformance[type].PeakPrimCount = m_ElementPerformance[type].PrimCount;
	}
}

void A3DGFXExMan::ResetElementPerformance()
{
	for (abase::hash_map<int,Performance>::iterator i=m_ElementPerformance.begin(); i!=m_ElementPerformance.end(); ++i)
	{
		i->second.Clear();
	}
}

void A3DGFXExMan::ResetElementPeakCount()
{
	for (abase::hash_map<int,Performance>::iterator i=m_ElementPerformance.begin(); i!=m_ElementPerformance.end(); ++i)
	{
		i->second.ClearPeak();
	}
}

void A3DGFXExMan::AddTotalPrimCount( size_t nPrimCount )
{
	_gfx_prim_count += nPrimCount;
	if (_gfx_prim_count > m_gfx_peak_prim_count)
		m_gfx_peak_prim_count = _gfx_prim_count;
}

void A3DGFXExMan::UpdateSfxFileList( DWORD delta )
{
#ifdef _ANGELICA21

	LockSfxFileList();

	ALISTPOSITION pos = m_pSfxFileList.GetHeadPosition();
	PlayedSfx* pSfx;

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pSfx = m_pSfxFileList.GetNext(pos);
		if (pSfx->m_nTTL <= (int)delta)
		{
			m_pSfxFileList.RemoveAt(posCur);
			delete pSfx;
		}
		else
		{
			pSfx->m_nTTL -= (int)delta;
		}
	}

	UnlockSfxFileList();

#endif
}

void A3DGFXExMan::PushSfxFile( const AString& strFile, int ttl )
{
#ifdef _ANGELICA21

	PlayedSfx* pSfx = new PlayedSfx;
	pSfx->m_nTTL = ttl;
	pSfx->m_strFile = strFile;

	LockSfxFileList();

	m_pSfxFileList.AddTail(pSfx);

	UnlockSfxFileList();

#endif
}

void A3DGFXExMan::ReleaseSfxFileList()
{
#ifdef _ANGELICA21

	LockSfxFileList();

	while (m_pSfxFileList.GetCount())
	{
		PlayedSfx* sfx = m_pSfxFileList.RemoveHead();
		delete sfx;
	}

	UnlockSfxFileList();

#endif
}

bool A3DGFXExMan::IsInSfxFileList( const AString& strFile, int& ttl )
{
	bool ret = false;

#ifdef _ANGELICA21

	LockSfxFileList();

	ALISTPOSITION pos = m_pSfxFileList.GetHeadPosition();
	PlayedSfx* pSfx;

	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pSfx = m_pSfxFileList.GetNext(pos);
		if (pSfx->m_strFile == strFile)
		{
			ttl = pSfx->m_nTTL;
			ret = true;
		}
	}
	
	UnlockSfxFileList();

#endif

	return ret;
}

#ifdef GFX_EDITOR

typedef bool (*pFunctionGfxGetSurfaceData)(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount);
typedef float (*pFunctionGetGrndNorm)(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm);
typedef void (*pFunctionGetLightParam)(A3DLIGHTPARAM& lightParam);

static pFunctionGetGrndNorm s_CallBackOnGetGrndNorm;
static pFunctionGfxGetSurfaceData s_CallBackOnGfxGetSurfaceData;
static pFunctionGetLightParam s_CallBackOnGetLightParam;

// 增加了一个挂钩函数，用于在编辑器中自定义取地面高度，法向的函数
void Afx_SetGroundFuncHook(pFunctionGfxGetSurfaceData pCallBackOnGetSurfaceData, pFunctionGetGrndNorm pCallBackOnGetGrndNorm)
{
	s_CallBackOnGfxGetSurfaceData = pCallBackOnGetSurfaceData;
	s_CallBackOnGetGrndNorm = pCallBackOnGetGrndNorm;
}

void Afx_SetLightParamFuncHook(pFunctionGetLightParam pCallback)
{
	s_CallBackOnGetLightParam = pCallback;
}

A3DDevice* AfxGetA3DDevice()
{
	return g_pDevice;
}

A3DGFXExMan* AfxGetGFXExMan()
{
	return &_gfx_ex_man;
}

#ifdef _ANGELICA21
A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag, DWORD dwTexFlag)
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(g_pDevice->GetA3DEngine()) || !pSkinModel->Load(szFile, iSkinFlag))
	{
		pSkinModel->Release();
		delete pSkinModel;
		return NULL;
	}

	return pSkinModel;
}
#else
A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag)
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(g_pDevice->GetA3DEngine()) || !pSkinModel->Load(szFile, iSkinFlag))
	{
		pSkinModel->Release();
		delete pSkinModel;
		return NULL;
	}

	return pSkinModel;
}
#endif

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	pModel->Release();
	delete pModel;
}

bool AfxGetModelUpdateFlag()
{
	return true;
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
	A3DSkinModel::LIGHTINFO LightInfo = pA3DSkinModel->GetLightInfo();
	GfxLightParamList& ls = AfxGetGFXExMan()->GetLightParamList();

	if (ls.size())
	{
		A3DLIGHTPARAM* param = &ls[0]->GetLightParam();
		LightInfo.bPtLight = true;
		LightInfo.colPtDiff = param->Diffuse * param->Diffuse.a * 2.0f;
		LightInfo.colPtAmb =  param->Ambient * param->Ambient.a * 2.0f;
		LightInfo.fPtRange = param->Range;
		LightInfo.vPtAtten = A3DVECTOR3(param->Attenuation0, param->Attenuation1, param->Attenuation2);
		LightInfo.vPtLightPos = param->Position;

		if ((LightInfo.vPtLightPos - pA3DSkinModel->GetModelAABB().Center).Magnitude() > LightInfo.fPtRange)
			return;
	}
//	else
//		LightInfo.bPtLight = false;

	pA3DSkinModel->SetLightInfo(LightInfo);
}

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
}

A3DCamera* AfxGetA3DCamera()
{
	return static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
}

#ifdef _ANGELICA21
float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm, bool bUseRayTrace)
#else
float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
#endif
{
	if (s_CallBackOnGetGrndNorm)
		return (*s_CallBackOnGetGrndNorm)(vPos, pNorm);

	if (pNorm) *pNorm = _unit_y;
	return 0;
}

const A3DLIGHTPARAM& AfxGetLightparam()
{
	static A3DLIGHTPARAM p;
	memset(&p, 0, sizeof(p));

	if (s_CallBackOnGetLightParam)
	{
		s_CallBackOnGetLightParam(p);
		return p;
	}

	static const A3DCOLORVALUE diffuse(0.8f, 0.8f, 0.9f, 1.0f);
	static const A3DCOLORVALUE spec(1.0f, 1.0f, 1.0f, 1.0f);
	static const A3DCOLORVALUE ambient(0.0f, 0.0f, 0.0f, 1.0f);
	
	p.Type		= A3DLIGHT_DIRECTIONAL;
	p.Diffuse	= diffuse;
	p.Specular	= spec;
	p.Ambient	= ambient;
	p.Position	= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	p.Direction	= Normalize(A3DVECTOR3(0.0f, -0.7f, 1.0f));
	return p;
}

bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel)
{
	return false;
}

AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile, int nPriority)
{
	AMSoundBufferMan* pMan = g_pDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	return pMan->Load3DSound(szFile, true);
}

AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile)
{
	return AfxLoadNonLoopSound(szFile, 0);
}

void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound)
{
	g_pDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound)
{
	AfxReleaseSoundNonLoop(pSound);
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
	A3DCamera* pCamera = static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
	pCamera->SetPos(pCamera->GetPos() + vOffset);
}


void AfxBeginShakeCam()
{
	
}

void AfxEndShakeCam()
{

}


float AfxGetAverageFrameRate()
{
	return 30.f;
}

bool gGfxGetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount)
{
	if (s_CallBackOnGfxGetSurfaceData)
		return (*s_CallBackOnGfxGetSurfaceData)(vCenter, fRadus, pVerts, nVertCount, pIndices, nIndexCount);

	pVerts[0] = A3DVECTOR3(vCenter.x-fRadus, vCenter.y, vCenter.z+fRadus);
	pVerts[1] = A3DVECTOR3(vCenter.x+fRadus, vCenter.y, vCenter.z+fRadus);
	pVerts[2] = A3DVECTOR3(vCenter.x-fRadus, vCenter.y, vCenter.z-fRadus);
	pVerts[3] = A3DVECTOR3(vCenter.x+fRadus, vCenter.y, vCenter.z-fRadus);
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 1;
	pIndices[4] = 3;
	pIndices[5] = 2;
	nVertCount = 4;
	nIndexCount = 6;
	return true;
}

#endif

// efficiency statistic

#include "A3DFont.h"

const int _eff_stat_max_item = 100;
static A3DEngine* _eff_stat_engine = NULL;
static int _eff_stat_count = 0;
static double _eff_stat_max_tick[_eff_stat_max_item];
static ACString _eff_stat_string[_eff_stat_max_item];
static double _eff_stat_frequ;
static LARGE_INTEGER _eff_stat_start, _eff_stat_end;
static const int _eff_stat_x = 5;
static const int _eff_stat_y = 100;
static const int _eff_stat_hei = 20;
static int _eff_stat_cur = 0;
static double _eff_stat_cur_total = 0;
static double _eff_stat_max_total = 0;
static double _eff_cur_tick = 0;

void eff_stat_init(A3DEngine* pEngine)
{
	_eff_stat_engine = pEngine;
	memset(_eff_stat_max_tick, 0, sizeof(_eff_stat_max_tick));

	LARGE_INTEGER l;
	QueryPerformanceFrequency(&l);
	_eff_stat_frequ = (double)l.QuadPart;
}

void eff_stat_prepare()
{
	if (++_eff_stat_count == 50)
	{
		_eff_stat_count = 0;
		memset(_eff_stat_max_tick, 0, sizeof(_eff_stat_max_tick));
		_eff_stat_max_total = 0;
	}

	_eff_stat_cur = 0;
	_eff_stat_cur_total = 0;
}

void eff_stat_begin()
{
	QueryPerformanceCounter(&_eff_stat_start);
}

void eff_stat_end()
{
	QueryPerformanceCounter(&_eff_stat_end);
	_eff_cur_tick = ((double)_eff_stat_end.QuadPart - _eff_stat_start.QuadPart) * 1000.0 / _eff_stat_frequ;
	if (_eff_cur_tick > _eff_stat_max_tick[_eff_stat_cur])
		_eff_stat_max_tick[_eff_stat_cur] = _eff_cur_tick;
	_eff_stat_cur_total += _eff_cur_tick;
}

void eff_stat_report_one(const ACHAR* szName)
{
	ACString& str = _eff_stat_string[_eff_stat_cur];
	str.Format(_AL("%s: CurTick %6.2f, MaxTick %6.2f"), szName, _eff_cur_tick, _eff_stat_max_tick[_eff_stat_cur]);
	_eff_stat_cur++;
}

void eff_stat_flush_output()
{
	/*
	for (int i = 0; i < _eff_stat_cur; i++)
	{
		_eff_stat_engine->GetSystemFont()->TextOut(
			_eff_stat_x,
			_eff_stat_y + i * _eff_stat_hei,
			_eff_stat_string[i],
			0xffffffff);
	}

	if (_eff_stat_cur_total > _eff_stat_max_total)
		_eff_stat_max_total = _eff_stat_cur_total;

	ACString str;
	str.Format(_AL("Total: %6.2f, Max: %6.2f"), _eff_stat_cur_total, _eff_stat_max_total);

	_eff_stat_engine->GetSystemFont()->TextOut(
		_eff_stat_x,
		_eff_stat_y + i * _eff_stat_hei,
		str,
		0xffffffff);
		*/
}

#ifdef GFX_EDITOR
#ifndef _SKILLGFXCOMPOSER

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	return true;
}

void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
}

void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier)
{
}

bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
	return true;
}

#endif
#endif
