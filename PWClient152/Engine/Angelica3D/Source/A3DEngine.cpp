#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DCameraBase.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DSky.h"
#include "A3DTerrain.h"
#include "A3DTerrain2Env.h"
#include "A3DModel.h"
#include "A3DFont.h"
#include "A3DFontMan.h"
#include "A3DFrame.h"
#include "A3DGFXMan.h"
#include "A3DViewport.h"
#include "A3DLightMan.h"
#include "A3DMathUtility.h"
#include "A3DRenderTarget.h"
#include "A2DSpriteTexMan.h"
#include "A3DMoxMan.h"
#include "A3DTextureMan.h"
#include "A3DModelMan.h"
#include "A3DSurfaceMan.h"
#include "A3DShaderMan.h"
#include "A3DImgModelMan.h"
#include "A3DSkinModelMan.h"
#include "A3DSkinMan.h"
#include "A3DSkinRender.h"
#include "A3DSkinRenderWithEffect.h"
#include "A3DWorld.h"
#include "A3DGDI.h"
#include "A3DMeshSorter.h"
#include "A3DMeshCollector.h"
#include "A3DVertexCollector.h"
#include "A3DFlatCollector.h"
#include "A3DWireCollector.h"
#include "A3DLitModelRender.h"
#include "A3DPlants.h"
#include "AMEngine.h"
#include "AMVideoEngine.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"
#include "AM3DSoundDevice.h"
#include "AChar.h"
#include "AMemory.h"
#include "AAssist.h"
#include "A3DHLSL.h"
#include "A3DOcclusionMan.h"
#include "A3DOcclusionManHW.h"
#include "A3DOcclusionManSW.h"
#include "A3DSkinModel.h"
#include "A3DEnvironment.h"
#include "AGPAPerfIntegration.h"
#include "A3DShadowMap.h"
#include "A3DLitModelSharedDataMan.h"

#define new A_DEBUG_NEW

A3DEngine::A3DEngine()
{
	m_pD3D				= NULL;
	m_pA3DDevice		= NULL;
	m_pA2DSpriteTexMan	= NULL;
	m_pA3DLightMan		= NULL;
	m_pA3DMeshCollector = NULL;
	m_pA3DVertexCollector = NULL;
	m_pA3DMeshSorter	= NULL;
	m_pA3DMoxMan		= NULL;
	m_pA3DModelMan		= NULL;
	m_pA3DSurfaceMan	= NULL;
	m_pA3DTextureMan	= NULL;
	m_pA3DImgModelMan	= NULL;
	m_pA3DSkinMan		= NULL;
	m_pA3DFlatCollector	= NULL;
	m_pA3DWireCollector	= NULL;
	m_pA3DLitModelRender= NULL;
	m_pA3DDefLitModelRender = NULL;
	m_pA3DSkinModelMan	= NULL;
	m_pHLSLMan          = NULL;
    m_pA3DOcclusionMan     = NULL;
	m_pA3DEnvironment	= NULL;
	m_pA3DShadowMap		= NULL;
    m_pA3DGFXMan        = NULL;
	m_pA3DFontMan		= NULL;
    m_pA3DShaderMan     = NULL;
	m_hInstance			= NULL;
	m_hDeviceWnd		= NULL;
	
	m_pA3DCDS			= NULL;

	m_pA3DWorld			= NULL;
	m_pActiveViewport	= NULL;
	m_pActiveCamera		= NULL;
	
	m_pSky				= NULL;
	m_pTerrain			= NULL;

	m_pRFPSCounter		= NULL;
	m_pTFPSCounter		= NULL;
	m_pSystemFont		= NULL;

	m_dwAnimationTicks	= 0;
	m_dwRenderTicks		= 0;
	m_dwLogicTicks		= 0;

	m_pMotionBlurRenderTarget = NULL;
	m_nMotionBlurValue	= 128; //0.5f;

	//	Debug section;
	a_strcpy(m_aPerformances[PERF_DRAWPRIMITIVE].szTagName, _AL("Draw Primitives"));
	a_strcpy(m_aPerformances[PERF_TICK_SKY].szTagName, _AL("Sky Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_MODEL].szTagName, _AL("Model Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_TEXTURE].szTagName, _AL("Texture Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_GFX].szTagName, _AL("Gfx Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_ANIMATION].szTagName, _AL("Animation Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_MEDIA].szTagName, _AL("Media Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SCENEUPDATE].szTagName, _AL("Scene Update Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SCENERAYTRACE].szTagName, _AL("Scene RayTrace Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SCENEAABBTRACE].szTagName, _AL("Scene AABBTrace Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SCENESPHERETRACE].szTagName, _AL("Scene SphereTrace Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SKINMODEL].szTagName, _AL("SkinModel Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_SKELETON].szTagName, _AL("Skeleton Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_OTHERS].szTagName, _AL("Others Tick"));
	a_strcpy(m_aPerformances[PERF_TICK_ALL].szTagName, _AL("Total Tick"));
	a_strcpy(m_aPerformances[PERF_RENDER_MESH].szTagName, _AL("Mesh Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_MESHCOLLECTOR].szTagName, _AL("Mesh Collector Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_VERTCOLLECTOR].szTagName, _AL("Vertex Collector Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_SKINMODEL].szTagName, _AL("SkinModel Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_GFX].szTagName, _AL("GFX Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_SCENE].szTagName, _AL("Scene Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_IMAGEMODEL].szTagName, _AL("ImageModel Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_TEXT].szTagName, _AL("Text Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_OTHERS].szTagName, _AL("Others Render"));
	a_strcpy(m_aPerformances[PERF_RENDER_ALL].szTagName, _AL("Total Render"));

	a_strcpy(m_aObjectCnts[OBJECTCNT_MODEL].szTagName, _AL("Total Model"));
	a_strcpy(m_aObjectCnts[OBJECTCNT_GFX].szTagName, _AL("Total GFX"));
	a_strcpy(m_aObjectCnts[OBJECTCNT_TEXTURE].szTagName, _AL("Total Texture"));
	a_strcpy(m_aObjectCnts[OBJECTCNT_SPRITE].szTagName, _AL("Total Sprite"));
	a_strcpy(m_aObjectCnts[OBJECTCNT_STRING].szTagName, _AL("Total String"));	
	a_strcpy(m_aObjectCnts[OBJECTCNT_STREAM].szTagName, _AL("Total Stream"));	
	a_strcpy(m_aObjectCnts[OBJECTCNT_SKINMODEL].szTagName, _AL("Total SkinModel"));	

	int i;

	for (i=0; i < NUM_OBJECTCNT; i++)
		m_aObjectCnts[i].iObjectCnt = 0;
	
	a_strcpy(m_aFrameCnts[FRAMECNT_SCENERAYTRACE].szTagName, _AL("Count Scene Ray Trace"));
	a_strcpy(m_aFrameCnts[FRAMECNT_SCENEAABBTRACE].szTagName, _AL("Count Scene AABB Trace"));
	a_strcpy(m_aFrameCnts[FRAMECNT_SCENESPHERETRACE].szTagName, _AL("Count Scene Sphere Trace"));

	for (i=0; i < NUM_FRAMECNT; i++)
		m_aFrameCnts[i].iCounter = 0;

	// Angelica Media Engine;
	m_pAMEngine			= NULL;
	m_pAMSoundEngine	= NULL;
	m_pAMVideoEngine	= NULL;

	m_pA3DPlants		= NULL;

    m_pA3DLitModelSharedDataMan = NULL;

	m_curIBLLightParam.dynamicLightParam.Type = A3DLIGHT_FORCE_DWORD;

	//d3d9ex 
	m_bSupportD3D9Ex = false;
}

A3DEngine::~A3DEngine()
{
}

//d3d9ex
IDirect3D9* A3DEngine::CheckCreateD3DEx()
{
	HRESULT hr = E_FAIL;
	HMODULE libHandle = NULL;
	IDirect3D9* pD3D9Base = NULL;

	// Manually load the d3d9.dll library.
	libHandle = LoadLibraryW(L"d3d9.dll");

	if (libHandle != NULL)
	{
		// Define a function pointer to the Direct3DCreate9Ex function.
		typedef HRESULT (WINAPI *LPDIRECT3DCREATE9EX)( UINT, void **);

		// Obtain the address of the Direct3DCreate9Ex function. 
		LPDIRECT3DCREATE9EX Direct3DCreate9ExPtr = NULL;

		Direct3DCreate9ExPtr = (LPDIRECT3DCREATE9EX)GetProcAddress( libHandle, "Direct3DCreate9Ex" );

		if ( Direct3DCreate9ExPtr != NULL)
		{
			// Direct3DCreate9Ex is supported.
			hr = S_OK;
		}
		else
		{
			// Direct3DCreate9Ex is not supported on this
			// operating system.
			hr = ERROR_NOT_SUPPORTED;
		}

		// Free the library.
		FreeLibrary( libHandle );

		if (hr == S_OK)
		{
			if ((*Direct3DCreate9ExPtr)(D3D_SDK_VERSION, (void**)&pD3D9Base) == S_OK)
			{
				m_bSupportD3D9Ex = true;
				return pD3D9Base;
			}
		}
	}

	return Direct3DCreate9(D3D_SDK_VERSION);
}

bool A3DEngine::Init(HINSTANCE hInstance, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType)
{
#ifdef _DEBUG
	MessageBox(hDeviceWnd, _AL("DEBUG"), _AL(""), MB_OK);
#endif

	//	Initalize random number generator
	a_InitRandom();

	if (!g_A3DErrLog.Init("A3D.log", "Angelica 3D Error Log", false))
		return false;

	if (!(g_pA3DConfig = new A3DConfig))
		return false;

	// By default we turn on optimization flag, for editors, we should turn off this flag;
	m_bOptimizedEngine = true;
	//m_bFSMotionBlur	   = false;
	//m_nMotionBlurValue = 128; // this is half fade out;

	m_hInstance = hInstance;
	m_hDeviceWnd = hDeviceWnd;
	m_devFmt = *pA3DDevFmt;

	// *****************************************************
	// First we should initialize the angelica media engine;
	// *****************************************************
	m_pAMEngine = new AMEngine();
	if( NULL == m_pAMEngine )
	{
		g_A3DErrLog.Log("A3DEngine::Init, Enough memory!");
		return false;
	}

	if( !m_pAMEngine->Init(hDeviceWnd) )
	{
		g_A3DErrLog.Log("A3DEngine::Init, Init Angelica Media Engine Fail!");
		return false;
	}

	AMSOUNDDEVFORMAT devFormat;
	ZeroMemory(&devFormat, sizeof(devFormat));
	devFormat.nChannels = 2;
	devFormat.nSamplesPerSec = 44100;
	devFormat.wBitsPerSample = 16;
	devFormat.soundCL = AMSCL_PRIORITY;
	devFormat.bGlobalFocus = pA3DDevFmt->bSndGlobal;
	if( !m_pAMEngine->InitSoundEngine(&devFormat, true) )
	{
		g_A3DErrLog.Log("A3DEngine::Init, Init Angelica Sound Engine Fail!");
		return false;
	}

	if( !m_pAMEngine->InitVideoEngine() )
	{
		g_A3DErrLog.Log("A3DEngine::Init, Init Angelica Video Engine Fail!");
		return false;
	}



	m_pAMSoundEngine = m_pAMEngine->GetAMSoundEngine();
	m_pAMVideoEngine = m_pAMEngine->GetAMVideoEngine();

	// *****************************************
	// Now we initialize the Angelica 3D engine;
	// *****************************************
	m_pD3D = CheckCreateD3DEx();
	//m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if( NULL == m_pD3D )
	{
		g_A3DErrLog.Log("A3DEngine::Init Error Create IDirect3D8 with version %d", D3D_SDK_VERSION);
		return false;
	}

	m_pA3DDevice = new A3DDevice();
	if (!m_pA3DDevice)
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DDevice->Init(this, m_hDeviceWnd, pA3DDevFmt, dwDevFlags, devType))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init A3DDevice Fail!");
		return false;
	}
    
	// You must first turn on motion blur flag before you can switch motion blur in game;
	if (g_pA3DConfig->GetFlagMotionBlur())
	{
		if (!(m_pMotionBlurRenderTarget = new A3DRenderTarget))
		{
			g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
			return false;
		}

		A3DRenderTarget::RTFMT rtFmt;
		A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
		rtFmt.iWidth = devFmt.nWidth;
		rtFmt.iHeight = devFmt.nHeight;
		rtFmt.fmtTarget = devFmt.fmtTarget;
		rtFmt.fmtDepth = devFmt.fmtDepth;

		if (!m_pMotionBlurRenderTarget->Init(m_pA3DDevice, rtFmt, true, false))
		{
			g_A3DErrLog.Log("A3DEngine::Init() Init motion blur render target fail!");
			return false;
		}
	}

	m_pA3DLightMan = new A3DLightMan();
	if( NULL == m_pA3DLightMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DLightMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init() LightMan Init Fail!");
		return false;
	}

	m_pA3DTextureMan = new A3DTextureMan();
	if( NULL == m_pA3DTextureMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DTextureMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init TextureMan Init Fail!");
		return false;
	}

	m_pA3DMeshCollector = new A3DMeshCollector();
	if( NULL == m_pA3DMeshCollector )
	{
		g_A3DErrLog.Log("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DMeshCollector->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init A3DMeshCollector Init fail!");
		return false;
	}

	m_pA3DVertexCollector = new A3DVertexCollector();
	if( NULL == m_pA3DMeshCollector )
	{
		g_A3DErrLog.Log("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DVertexCollector->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init A3DVertexCollector Init fail!");
		return false;
	}

	m_pA3DMeshSorter = new A3DMeshSorter();
	if( NULL == m_pA3DMeshSorter )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DMeshSorter->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init Mesh Sorter Fail!");
		return false;
	}

	m_pA3DFontMan = new A3DFontMan();
	if( NULL == m_pA3DFontMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DFontMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Init A3DFontMan fail!");
		return false;
	}

	m_pA3DGFXMan = new A3DGFXMan();
	if( NULL == m_pA3DGFXMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DGFXMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init() A3DGFXMan Init Fail!");
		return false;
	}

	m_pA3DMoxMan = new A3DMoxMan();
	if( NULL == m_pA3DMoxMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DMoxMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init MoxMan Init Fail!");
		return false;
	}

	m_pA3DModelMan = new A3DModelMan();
	if( NULL == m_pA3DModelMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DModelMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init ModelMan Init Fail!");
		return false;
	}

	m_pA3DSurfaceMan = new A3DSurfaceMan();
	if( NULL == m_pA3DSurfaceMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DSurfaceMan->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init SurfaceMan Init Fail!");
		return false;
	}

	if (!(m_pA3DShaderMan = new A3DShaderMan))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DShaderMan->Init(this))
	{
		g_A3DErrLog.Log("A3DEngine::Init ShaderMan Init Fail!");
		return false;
	}

	m_pHLSLMan = new A3DHLSLMan(m_pA3DDevice);
	if(m_pHLSLMan->Init(false) == false)
	{
		g_A3DErrLog.Log("A3DEngine::Init(), fail to initialize HLSL manager!");
		return false;
	}

    m_pA3DEnvironment = new A3DEnvironment;
    if(m_pA3DEnvironment == NULL)
    {
        g_A3DErrLog.Log("A3DEngine::Init, can not create m_pA3DEnvironment, not enough memory.");
        return false;
    }
    if( ! m_pA3DEnvironment->Init())
    {
        g_A3DErrLog.Log("A3DEngine::Init, can not init m_pA3DEnvironment.");
        A3DRELEASE(m_pA3DEnvironment);
        return false;
    }

	m_pA2DSpriteTexMan = new A2DSpriteTexMan();
	if( NULL == m_pA2DSpriteTexMan )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA2DSpriteTexMan->Init(m_pA3DDevice, 1000) )
	{
		g_A3DErrLog.Log("A3DEngine::Init 2DSpriteTexMan Init Fail!");
		return false;
	}

	m_pA3DImgModelMan = new A3DImageModelMan;
	if (!m_pA3DImgModelMan)
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DImgModelMan->Init(m_pA3DDevice))
	{
		g_A3DErrLog.Log("A3DEngine::Init Image model man Init Fail!");
		return false;
	}

	m_pA3DSkinMan = new A3DSkinMan;
	if (!m_pA3DSkinMan)
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DSkinMan->Init(this))
	{
		g_A3DErrLog.Log("A3DEngine::Init skin man Init Fail!");
		return false;
	}

	m_pA3DDefLitModelRender = new A3DLitModelRender();
	if (!m_pA3DDefLitModelRender)
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Not enough memory!");
		return false;
	}

	if (!m_pA3DDefLitModelRender->Init(this, 60000, 60000 * 2) )
	{
		g_A3DErrLog.Log("A3DEngine::Init(), lit model render init fail!");
		return false;
	}

	m_pA3DLitModelRender = m_pA3DDefLitModelRender;

	m_pA3DSkinModelMan = new A3DSkinModelMan();
	if (!m_pA3DSkinModelMan)
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Not enough memory!");
		return false;
	}

	if (!m_pA3DSkinModelMan->Init(this))
	{
		g_A3DErrLog.Log("A3DEngine::Init(), skin model man init fail!");
		return false;
	}

    if (g_pA3DConfig->GetOcclusionType() == A3DOCCTYPE_D3DQUERY)
    {
        //	Initialize after HLSL manager
        A3DOcclusionManHW* pA3DOccManHW = new A3DOcclusionManHW;
        if(!pA3DOccManHW || !pA3DOccManHW->Init(this))
        {
            g_A3DErrLog.Log("A3DEngine::Init A3DOccusionManHW Init Fail!");
            A3DRELEASE(pA3DOccManHW);
            // if failed, continue without occlusion culling instead of returning
            g_pA3DConfig->SetOcclusionType(A3DOCCTYPE_NONE);
        }
        
        m_pA3DOcclusionMan = pA3DOccManHW;
    }
    else if (g_pA3DConfig->GetOcclusionType() == A3DOCCTYPE_SOFTWARE)
    {
        //	Initialize after HLSL manager
        A3DOcclusionManSW* pA3DOccManSW = new A3DOcclusionManSW;
        if(!pA3DOccManSW || !pA3DOccManSW->Init(this))
        {
            g_A3DErrLog.Log("A3DEngine::Init A3DOccusionManSW Init Fail!");
            A3DRELEASE(pA3DOccManSW);
            // if failed, continue without occlusion culling instead of returning
            g_pA3DConfig->SetOcclusionType(A3DOCCTYPE_NONE);
        }

        m_pA3DOcclusionMan = pA3DOccManSW;
    }

    m_pA3DLitModelSharedDataMan = new A3DLitModelSharedDataMan();
    if (!m_pA3DLitModelSharedDataMan)
    {
        g_A3DErrLog.Log("A3DEngine::Init(), Not enough memory!");
        return false;
    }

    if (!m_pA3DLitModelSharedDataMan->Init(this))
    {
        g_A3DErrLog.Log("A3DEngine::Init(), litmodel shared data man init fail!");
        return false;
    }

    m_ListViewport.Init();
	m_ListRenderTarget.Init();
	m_ListModel.Init();
	
	m_pA3DPlants = new A3DPlants();
	if( NULL == m_pA3DPlants )
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DPlants->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init(), Init A3DPlants Fail!");
		return false;
	}

	m_pRFPSCounter = new ACounter();
	if( NULL == m_pRFPSCounter )
	{
		g_A3DErrLog.Log("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pRFPSCounter->Init() )
	{
		g_A3DErrLog.Log("A3DEngine::Init init FPSCounter fail!");
		return false;
	}
	m_pRFPSCounter->ResetFPSCounter();

	m_pTFPSCounter = new ACounter();
	if( NULL == m_pTFPSCounter )
	{
		g_A3DErrLog.Log("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pTFPSCounter->Init() )
	{
		g_A3DErrLog.Log("A3DEngine::Init init FPSCounter fail!");
		return false;
	}
	m_pTFPSCounter->ResetFPSCounter();

	g_pA3DGDI = new A3DGDI();
	if( NULL == g_pA3DGDI )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !g_pA3DGDI->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init A3DGDI Fail!");
		return false;
	}

	if (!(m_pA3DFlatCollector = new A3DFlatCollector))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DFlatCollector->Init(this))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init A3DFlatCollector Fail!");
		return false;
	}

	if (!(m_pA3DWireCollector = new A3DWireCollector))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DWireCollector->Init(this, 4098, 8192 * 2))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init A3DWireCollector Fail!");
		return false;
	}

	const int iShadowMapSize = 512;
	bool bUseHWPCF = true;	

	m_pA3DShadowMap = new A3DShadowMap;
	if(!m_pA3DShadowMap->Init(m_pA3DDevice, iShadowMapSize, false, true, false, bUseHWPCF, false))
	{
		g_A3DErrLog.Log("A3DEngine::Init() Init A3DShadowMap Fail!");
		A3DRELEASE(m_pA3DShadowMap);
	}

	//	Create system font
	if (!CreateSystemFont())
		g_A3DErrLog.Log("A3DEngine::Init(), Failed to create system font");

	return true;
}

bool A3DEngine::Release()
{
	if (m_pA3DFontMan)
		m_pA3DFontMan->ReleaseA3DFont(m_pSystemFont);

	A3DTerrain2Env::ReleaseTerrain2Env();

	A3DRELEASE(g_pA3DGDI);
	A3DRELEASE(m_pRFPSCounter);
	A3DRELEASE(m_pTFPSCounter);

	A3DRELEASE(m_pA3DPlants);
	A3DRELEASE(m_pA2DSpriteTexMan);
	A3DRELEASE(m_pA3DModelMan);
	A3DRELEASE(m_pA3DMoxMan)
	A3DRELEASE(m_pA3DImgModelMan);
	A3DRELEASE(m_pA3DSkinMan);
	A3DRELEASE(m_pA3DDefLitModelRender);
	A3DRELEASE(m_pA3DGFXMan);
	A3DRELEASE(m_pA3DFontMan);
	A3DRELEASE(m_pA3DMeshSorter);
	A3DRELEASE(m_pA3DMeshCollector);
	A3DRELEASE(m_pA3DVertexCollector);
	A3DRELEASE(m_pA3DLightMan);
	A3DRELEASE(m_pA3DTextureMan);
	A3DRELEASE(m_pA3DSurfaceMan);
	A3DRELEASE(m_pA3DShaderMan);
	A3DRELEASE(m_pMotionBlurRenderTarget);
	A3DRELEASE(m_pA3DFlatCollector);
	A3DRELEASE(m_pA3DWireCollector);
	A3DRELEASE(m_pA3DSkinModelMan);
    A3DRELEASE(m_pA3DOcclusionMan);
	A3DRELEASE(m_pA3DEnvironment);
	A3DRELEASE(m_pA3DShadowMap);
    A3DRELEASE(m_pA3DLitModelSharedDataMan);

	m_pA3DLitModelRender = NULL;

	m_ListModel.Release();
	m_ListRenderTarget.Release();
	m_ListViewport.Release();

	A3DRELEASE(m_pHLSLMan);

	A3DRELEASE(m_pA3DDevice);

	if (m_pD3D)
	{
		int nRef = m_pD3D->Release();
		m_pD3D = NULL;
	}

	// *****************************************************
	// Now release angelica media engine;
	// *****************************************************
	A3DRELEASE(m_pAMEngine);

	if (g_pA3DConfig)
	{
		delete g_pA3DConfig;
		g_pA3DConfig = NULL;
	}

	g_A3DErrLog.Release();

	return true;
}

//	Logic time
bool A3DEngine::Tick(DWORD dwDeltaTime)
{
	m_dwLogicTicks++;
	return true;
}

bool A3DEngine::BeginRender()
{
	m_dwRenderTicks++;
	return m_pA3DDevice->BeginRender();
}

bool A3DEngine::Clear()
{
	return m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, A3DCOLORRGB(0, 0, 0), 1.0f, 0);
}

bool A3DEngine::EndRender()
{
	if (m_pA3DMeshCollector)
		m_pA3DMeshCollector->RemoveUnrenderedMeshes();

	if (m_pA3DSkinMan && m_pA3DSkinMan->GetCurSkinRender())
	{
		m_pA3DSkinMan->GetCurSkinRender()->ResetRenderInfo(false);
		m_pA3DSkinMan->GetCurSkinRender()->ResetRenderInfo(true);
	}

	if (m_pA3DLitModelRender)
	{
		m_pA3DLitModelRender->ResetRenderInfo(false);
		m_pA3DLitModelRender->ResetRenderInfo(true);
	}

	if (m_pA3DFlatCollector)
		m_pA3DFlatCollector->Flush();

	if (m_pA3DWireCollector)
		m_pA3DWireCollector->Flush();

	if (m_pA3DMeshSorter)
		m_pA3DMeshSorter->RemoveAllMeshes();

	if( g_pA3DConfig->RT_GetShowBoundBoxFlag() )
	{
		// we now show all 3d sfx here
		A3DAABB aabb;
		aabb.Extents = A3DVECTOR3(0.25f, 0.25f, 0.25f);
		aabb.Center = m_pAMSoundEngine->GetAM3DSoundDevice()->GetPosition();
		aabb.CompleteMinsMaxs();
	//	g_pA3DGDI->DrawBox(aabb, A3DCOLORRGBA(0, 0, 255, 128));

		const APtrList<AMSoundBufferMan::LOADED3D *>& loaded3DList = m_pAMSoundEngine->GetAMSoundBufferMan()->GetLoaded3DList();
		ALISTPOSITION pos = loaded3DList.GetHeadPosition();
		while (pos)
		{
			AM3DSoundBuffer* pSoundBuffer = loaded3DList.GetNext(pos)->pSoundBuf;
			
			aabb.Center = pSoundBuffer->Get3DBufferDesc()->vecPos;
			aabb.CompleteMinsMaxs();
			g_pA3DGDI->DrawBox(aabb, A3DCOLORRGBA(0, 255, 0, 128));
		}
	}

/*	A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
	A3DVIEWPORTPARAM viewParam;
	viewParam.X = 0;
	viewParam.Y = 0;
	viewParam.Width  = devFmt.nWidth;
	viewParam.Height = devFmt.nHeight;
	viewParam.MinZ = 0.0f;
	viewParam.MaxZ = 1.0f;
	m_pA3DDevice->SetViewport(&viewParam);
*/	
	if (g_pA3DConfig->RT_GetShowPerformanceFlag() || (g_pA3DConfig->RT_GetShowFPSFlag() && ((GetKeyState(VK_F12) & 0x8000) || (GetKeyState(VK_F11) & 0x8000))) )
    {
        DisplayPerformanceRecord();
    }

	//	Reset performance record
	int i = 0;
	for (i=0; i < NUM_PERFORMANCE; i++)
	{
	//	if (i != PERF_TICK_ANIMATION && i != PERF_TICK_MEDIA)
		m_aPerformances[i].dwTimeUsed = 0;
	}

	for (i=0; i < NUM_FRAMECNT; i++)
		m_aFrameCnts[i].iCounter = 0;
	
	if (g_pA3DConfig->RT_GetShowFPSFlag())
	{
		ACHAR szFPS[32];
		int nWidth = m_pActiveViewport ? m_pActiveViewport->GetParam()->Width : m_devFmt.nWidth;
		int nOffsetX = m_pActiveViewport ? m_pActiveViewport->GetParam()->X : 0;

		int nOffsetY = m_pActiveViewport ? m_pActiveViewport->GetParam()->Y : 0;

		a_sprintf(szFPS, _AL("RFPS: %5.2f"), m_pRFPSCounter->GetFPS());
		m_pSystemFont->TextOut(nOffsetX + nWidth - 8 * a_strlen(szFPS), nOffsetY + 0, szFPS, A3DCOLORRGBA(255, 0, 0, 255));
		a_sprintf(szFPS, _AL("TFPS: %5.2f"), m_pTFPSCounter->GetFPS());
		m_pSystemFont->TextOut(nOffsetX + nWidth - 8 * a_strlen(szFPS), nOffsetY + 16, szFPS, A3DCOLORRGBA(255, 0, 0, 255));
	}

	m_pSystemFont->Flush();

//	if( m_pActiveViewport )
//		m_pActiveViewport->Active();

	//	Clear character buffers
	if (m_pA3DFontMan)
		m_pA3DFontMan->RemoveUnrenderedChars();

	return m_pA3DDevice->EndRender();
}

bool A3DEngine::RenderImageModels(A3DViewport * pViewport)
{
	BeginPerformanceRecord(PERF_RENDER_IMAGEMODEL);

	if (m_pA3DImgModelMan && !m_pA3DImgModelMan->Render(pViewport))
		return false;

	EndPerformanceRecord(PERF_RENDER_IMAGEMODEL);

	return true;
}

bool A3DEngine::FlushInternalSpriteBuffer()
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	BeginPerformanceRecord(PERF_RENDER_OTHERS);

	if (m_pA2DSpriteTexMan && !m_pA2DSpriteTexMan->FlushInternalBuffer())
		return false;

	EndPerformanceRecord(PERF_RENDER_OTHERS);

	return true;
}

bool A3DEngine::RenderGFX(A3DViewport * pCurrentViewport, int nCategoryMask)
{
	BeginPerformanceRecord(PERF_RENDER_GFX);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	if (m_pA3DGFXMan && !m_pA3DGFXMan->Render(pCurrentViewport, nCategoryMask))
		return false;
	
	EndPerformanceRecord(PERF_RENDER_GFX);

	if (!FlushVertexCollector(pCurrentViewport))
		return false;
		
	return true;
}

bool A3DEngine::RenderWorld(A3DViewport * pCurrentViewport)
{
	// Now render the world;
	if( m_pA3DWorld && !m_pA3DWorld->Render(pCurrentViewport) )
	{
		g_A3DErrLog.Log("A3DEngine::Render() Render Scene Frame Fail!");
		return false;
	}

	// Now Render Engine Models
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	ALISTELEMENT * pThisModelElement = m_ListModel.GetHead()->pNext;
	while( pThisModelElement != m_ListModel.GetTail() )
	{
		A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
		if( !pModel->Render(pCurrentViewport) )
			return false;
		pThisModelElement = pThisModelElement->pNext;
	}

	return true;
}

bool A3DEngine::BeginCacheAlphaMesh(A3DViewport * pCurrentViewport)
{
	// Now set the current camera pos into mesh sorter;
	m_pA3DMeshSorter->SetCameraPos(pCurrentViewport->GetCamera()->GetPos());

	return true;
}

bool A3DEngine::FlushCachedAlphaMesh(A3DViewport * pCurrentViewport)
{
	BeginPerformanceRecord(PERF_RENDER_MESH);

	if (m_pA3DMeshSorter && !m_pA3DMeshSorter->Render(pCurrentViewport))
		return false;

	m_pA3DMeshSorter->RemoveAllMeshes();

	EndPerformanceRecord(PERF_RENDER_MESH);

	return true;
}

bool A3DEngine::FlushVertexCollector(A3DViewport * pCurrentViewport)
{
	// For effects, we will render into vertex collector, so we should flush the 
	// collector's buffer here;
	BeginPerformanceRecord(PERF_RENDER_VERTCOLLECTOR);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	if( m_pA3DVertexCollector && !m_pA3DVertexCollector->Flush(pCurrentViewport) )
		return false;

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

	EndPerformanceRecord(PERF_RENDER_VERTCOLLECTOR);
	return true;
}

bool A3DEngine::FlushMeshCollector(A3DViewport * pCurrentViewport)
{
	BeginPerformanceRecord(PERF_RENDER_MESHCOLLECTOR);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	if( m_pA3DMeshCollector && !m_pA3DMeshCollector->Render(pCurrentViewport) )
		return false;

	EndPerformanceRecord(PERF_RENDER_MESHCOLLECTOR);
	return true;
}

bool A3DEngine::RenderPlants(A3DViewport* pCurrentViewport)
{
	BeginPerformanceRecord(PERF_RENDER_OTHERS);

	if (m_pA3DPlants && !m_pA3DPlants->Render(pCurrentViewport))
		return false;

	EndPerformanceRecord(PERF_RENDER_OTHERS);

	return true;
}

bool A3DEngine::RenderScene(A3DViewport * pCurrentViewport)
{
	BeginCacheAlphaMesh(pCurrentViewport);
	RenderWorld(pCurrentViewport);
	RenderPlants(pCurrentViewport);
	FlushCachedAlphaMesh(pCurrentViewport);

	RenderGFX(pCurrentViewport, 0xffffffff);
	return true;
}

bool A3DEngine::Present()
{
	m_pRFPSCounter->UpdateFPSCounter();
	return m_pA3DDevice->Present();
}



bool A3DEngine::TickAnimation()
{

	//	Sky animation
	BeginPerformanceRecord(PERF_TICK_SKY);
	if (m_pSky && !m_pSky->TickAnimation())
	{
		g_A3DErrLog.Log("A3DEngine::TickAnimation() tick sky fail!");
		return false;
	}
	EndPerformanceRecord(PERF_TICK_SKY);

	//	World animation
/*	if (m_pA3DWorld && !m_pA3DWorld->TickAnimation())
	{
		g_A3DErrLog.Log("A3DEngine::TickAnimation() tick frame scene fail!");
		return false;
	}
*/
	//	Now update The models	
	BeginPerformanceRecord(PERF_TICK_MODEL);
	ALISTELEMENT* pThisModelElement = m_ListModel.GetHead()->pNext;
	while (pThisModelElement != m_ListModel.GetTail())
	{
		A3DModel* pModel = (A3DModel *) pThisModelElement->pData;
		if (!pModel->TickAnimation())
			return false;

		pThisModelElement = pThisModelElement->pNext;
	}
	EndPerformanceRecord(PERF_TICK_MODEL);

	//	GFX animation
	BeginPerformanceRecord(PERF_TICK_GFX);
	if (m_pA3DGFXMan && !m_pA3DGFXMan->TickAnimation())
		return false;
	EndPerformanceRecord(PERF_TICK_GFX);

	//	Texture animation
	BeginPerformanceRecord(PERF_TICK_TEXTURE);
	//FIXME!! history garbage?!
	//if (m_pA3DTextureMan && !m_pA3DTextureMan->TickAnimation())
	//	return false;
	EndPerformanceRecord(PERF_TICK_TEXTURE);

//	if (m_pA3DShaderMan && !m_pA3DShaderMan->TickAnimation())
//		return false;
	
	BeginPerformanceRecord(PERF_TICK_ANIMATION);
	//	Skin model manager
	if (m_pA3DSkinModelMan)
		m_pA3DSkinModelMan->TickAnimation();
	EndPerformanceRecord(PERF_TICK_ANIMATION);

	if (m_pTFPSCounter)
		m_pTFPSCounter->UpdateFPSCounter();

	m_dwAnimationTicks++;

	

	//	AMedia tick
	BeginPerformanceRecord(PERF_TICK_MEDIA);

	if (m_pAMEngine)
		m_pAMEngine->Tick();

	EndPerformanceRecord(PERF_TICK_MEDIA);
	m_pA3DEnvironment->Tick();

	return true;
}

bool A3DEngine::AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement)
{
	if( m_ListViewport.GetSize() == 0 )
		pViewport->Active();
	return m_ListViewport.Append((LPVOID)pViewport, ppElement);
}

bool A3DEngine::RemoveViewport(A3DViewport * pViewport)
{
	return m_ListViewport.Delete((LPVOID) pViewport);
}

bool A3DEngine::AddRenderTarget(A3DRenderTarget * pRenderTarget, ALISTELEMENT ** ppElement)
{
	return m_ListRenderTarget.Append((LPVOID) pRenderTarget, ppElement);
}

bool A3DEngine::RemoveRenderTarget(A3DRenderTarget * pRenderTarget)
{
	return m_ListRenderTarget.Delete((LPVOID) pRenderTarget);
}

bool A3DEngine::AddModel(A3DModel * pModel, ALISTELEMENT ** ppElement)
{
	assert(pModel->GetContainer() == A3DCONTAINER_NULL);
	pModel->SetContainer(A3DCONTAINER_ENGINE);

	return m_ListModel.Append((LPVOID) pModel, ppElement);
}

bool A3DEngine::RemoveModel(A3DModel * pModel)
{
	assert(pModel->GetContainer() == A3DCONTAINER_ENGINE);
	pModel->SetContainer(A3DCONTAINER_NULL);

	return m_ListModel.Delete((LPVOID) pModel);
}

bool A3DEngine::SetWorld(A3DWorld * pWorld)
{
	m_pA3DWorld = pWorld;
	return true;
}

bool A3DEngine::SetSky(A3DSky * pSky)
{
	m_pSky = pSky;
	return true;
}

bool A3DEngine::SetTerrain(A3DTerrain * pTerrain)
{
	m_pTerrain = pTerrain;
	return true;
}

bool A3DEngine::Reset()
{
	ResetWorldRelated();

	//This function is used to release all resources but keep the hardware device unchanged;
	m_ListModel.Reset();
	m_ListRenderTarget.Reset();
	m_ListViewport.Reset();

	m_pA3DPlants->Reset();

	if (m_pA2DSpriteTexMan) 
		m_pA2DSpriteTexMan->Reset();

	if (m_pA3DModelMan)
		m_pA3DModelMan->Reset();
	
	if (m_pA3DMoxMan)
		m_pA3DMoxMan->Reset();

	if (m_pA3DSkinMan)
		m_pA3DSkinMan->Reset();

	if (m_pA3DLightMan)
		m_pA3DLightMan->Reset();
	
	if (m_pA3DGFXMan)
		m_pA3DGFXMan->Reset();
	
	if (m_pA3DTextureMan)
		m_pA3DTextureMan->Reset();
	
	if (m_pA3DSurfaceMan)
		m_pA3DSurfaceMan->Reset();
	
	if (m_pA3DImgModelMan)
		m_pA3DImgModelMan->Reset();
	
	//	Re-create system font
	CreateSystemFont();

	if (m_pAMEngine)
		m_pAMEngine->Reset();
	
	m_pRFPSCounter->ResetFPSCounter();
	m_pTFPSCounter->ResetFPSCounter();
	m_dwAnimationTicks	= 0;
	m_dwRenderTicks		= 0;
	m_dwLogicTicks		= 0;

	return true;
}

// Reset world related objects;
bool A3DEngine::ResetWorldRelated()
{
	//	This function should not be used any more;
	if (m_pA3DMeshCollector)
		m_pA3DMeshCollector->Reset();

	if (m_pA3DVertexCollector)
		m_pA3DVertexCollector->Reset();

	return true;
}

bool A3DEngine::DisplayPerformanceRecord()
{
	ACHAR szInfo[128];
	
	int	nDrawVertCount = m_pA3DDevice->GetDrawVertCount();
	int	nDrawFaceCount = m_pA3DDevice->GetDrawFaceCount();
	int	nDrawCount = m_pA3DDevice->GetDrawCount();
	double fInvFreq = 1000000.0 / ACounter::GetCPUFrequency();
	int i, x=50, y=80;

	//	Calculate total tick time
	m_aPerformances[PERF_TICK_ALL].dwTimeUsed = 
				m_aPerformances[PERF_TICK_SKY].dwTimeUsed +
				m_aPerformances[PERF_TICK_MODEL].dwTimeUsed +
				m_aPerformances[PERF_TICK_GFX].dwTimeUsed +
				m_aPerformances[PERF_TICK_TEXTURE].dwTimeUsed +
				m_aPerformances[PERF_TICK_ANIMATION].dwTimeUsed + 
				m_aPerformances[PERF_TICK_MEDIA].dwTimeUsed + 
				m_aPerformances[PERF_TICK_SCENEUPDATE].dwTimeUsed + 
				m_aPerformances[PERF_TICK_SCENERAYTRACE].dwTimeUsed +
				m_aPerformances[PERF_TICK_SCENEAABBTRACE].dwTimeUsed + 
				m_aPerformances[PERF_TICK_SCENESPHERETRACE].dwTimeUsed +
				m_aPerformances[PERF_TICK_SKINMODEL].dwTimeUsed + 
				m_aPerformances[PERF_TICK_OTHERS].dwTimeUsed;

	//	Calculate total render time
	m_aPerformances[PERF_RENDER_ALL].dwTimeUsed = 
				m_aPerformances[PERF_RENDER_MESH].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_MESHCOLLECTOR].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_VERTCOLLECTOR].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_SKINMODEL].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_IMAGEMODEL].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_GFX].dwTimeUsed +
				m_aPerformances[PERF_RENDER_SCENE].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_TEXT].dwTimeUsed + 
				m_aPerformances[PERF_RENDER_OTHERS].dwTimeUsed;

	for (i=0; i < NUM_PERFORMANCE; i++, y+=16)
	{
        int iTimeUsed = (int)(m_aPerformances[i].dwTimeUsed * fInvFreq);
		a_sprintf(szInfo, _AL("%32s:%8d"), m_aPerformances[i].szTagName, iTimeUsed);
		m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(255, 0, 0, 255));
	}

	x = 300, y = 80;

	a_sprintf(szInfo, _AL("%32s:%8d / %d"), _AL("Vertex / Face Count"), nDrawVertCount, nDrawFaceCount);
	m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	y += 16;

	a_sprintf(szInfo, _AL("%32s:%8d"), _AL("Draw Count"), nDrawCount);
	m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	y += 16;

	a_sprintf(szInfo, _AL("%32s:%8d"), _AL("GraphicsFX Count"), m_pA3DGFXMan->GetGFXCount());
	m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	y += 16;

	if (m_pA3DWorld)
	{
		a_sprintf(szInfo, _AL("%32s:%8d"), _AL("World Obj-Mod Count"), m_pA3DWorld->GetObjectModelCount());
		m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
		y += 16;
	}

	a_sprintf(szInfo, _AL("%32s:%8d"), _AL("Engine Model Count"), m_ListModel.GetSize());
	m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	y += 16;

	//	Object counters
	for (i=0; i < NUM_OBJECTCNT; i++, y+=16)
	{
		a_sprintf(szInfo, _AL("%32s:%8d"), m_aObjectCnts[i].szTagName, m_aObjectCnts[i].iObjectCnt);
		m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(0, 255, 255, 255));
	}

	//	Frame counters
	for (i=0; i < NUM_FRAMECNT; i++, y+=16)
	{
		a_sprintf(szInfo, _AL("%32s:%8d"), m_aFrameCnts[i].szTagName, m_aFrameCnts[i].iCounter);
		m_pSystemFont->TextOut(x, y, szInfo, A3DCOLORRGBA(255, 255, 0, 255));
	}

	return true;	
}

bool A3DEngine::ResetResource()
{
	ResetWorldRelated();

	// Release all resources but keep some important interface such as viewport, rendertarget and so on...
	// This function is used to release all resources but keep the hardware device unchanged;
	m_ListModel.Reset();
	
	if (m_pA3DPlants)
		m_pA3DPlants->Reset();

	if (m_pA3DModelMan)
		m_pA3DModelMan->Reset();
	
	if (m_pA3DMoxMan)
		m_pA3DMoxMan->Reset();

	if (m_pA3DSkinMan)
		m_pA3DSkinMan->Reset();

	if (m_pA3DGFXMan)
		m_pA3DGFXMan->Reset();
	
	if (m_pA2DSpriteTexMan)
		m_pA2DSpriteTexMan->Reset();

	if (m_pA3DTextureMan)
		m_pA3DTextureMan->Reset();

	if (m_pA3DSurfaceMan)
		m_pA3DSurfaceMan->Reset();
	
	if (m_pA3DImgModelMan)
		m_pA3DImgModelMan->Reset();
	
	if (m_pAMEngine)
		m_pAMEngine->Reset();
	
	return true;
}

bool A3DEngine::SetDisplayMode(int nWidth, int nHeight, A3DFORMAT formatTarget, bool bWindowed, bool bVSync, DWORD dwFlags)
{
	if( !m_pA3DDevice->SetDisplayMode(nWidth, nHeight, formatTarget, bWindowed, bVSync, dwFlags) )
		return false;

	if( dwFlags & SDM_WIDTH )
		m_devFmt.nWidth = nWidth;
	if( dwFlags & SDM_HEIGHT )
		m_devFmt.nHeight = nHeight;
	if( (dwFlags & SDM_FORMAT) && !m_devFmt.bWindowed )
		m_devFmt.fmtTarget = formatTarget;
	if( dwFlags & SDM_WINDOW )
		m_devFmt.bWindowed = bWindowed;
	if( dwFlags & SDM_VSYNC )
		m_devFmt.bVSync = bVSync;
	return true;
}

bool A3DEngine::PrecacheAllTextures()
{
	if( m_pA3DDevice->GetNeedResetFlag() )
		return false;

	// first of all create an offscreen rendertarget to do the cache rendering task.
	A3DRenderTarget * pRenderTarget;

	// create light's shadow map render target
	A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
	A3DRenderTarget::RTFMT rtFmt;

	rtFmt.fmtTarget		= devFmt.fmtTarget;
	rtFmt.fmtDepth		= devFmt.fmtDepth;
	rtFmt.iWidth		= 256;
	rtFmt.iHeight		= 256;
	pRenderTarget = new A3DRenderTarget();
	if( !pRenderTarget->Init(m_pA3DDevice, rtFmt, true, true) )
	{
		g_A3DErrLog.Log("A3DEngine::PrecacheAllTextures, Failed to initialize render target!");
		return false;
	}

	pRenderTarget->ApplyToDevice();

	A3DVIEWPORTPARAM viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.Width = 256;
	viewport.Height = 256;
	viewport.MaxZ = 1.0f;
	m_pA3DDevice->SetViewport(&viewport);
	m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	
	if( m_pA3DTextureMan )
	{
		m_pA3DTextureMan->PrecacheAllTexture();
	}

	m_pA3DDevice->EndRender();

	pRenderTarget->WithdrawFromDevice();
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);

	//D3DXSaveTextureToFileA("test.bmp", D3DXIFF_BMP, pRenderTarget->GetTargetTexture(), NULL);

	pRenderTarget->Release();
	delete pRenderTarget;

	return true;
}

//	Create system font
bool A3DEngine::CreateSystemFont()
{
	if (!m_pA3DFontMan)
		return false;

	HA3DFONT hA3DFont = NULL;

	if (m_pSystemFont)	//	Recreate font
	{
		hA3DFont = m_pSystemFont->GetA3DFont();
		m_pA3DFontMan->ReleaseA3DFont(m_pSystemFont);
	}

	if (!hA3DFont)
		hA3DFont = m_pA3DFontMan->RegisterFontType(_AL("Courier New"), 12, NULL, 'W');

	if (!(m_pSystemFont = m_pA3DFontMan->CreateA3DFont(hA3DFont, 256, 128, 128)))
	{
		g_A3DErrLog.Log("A3DEngine::CreateSystemFont, Failed to initialize system font!");
		return false;
	}

	return true;
}

void A3DEngine::SetA3DLitModelRender(A3DLitModelRender* pRender)
{
	if (pRender)
		m_pA3DLitModelRender = pRender;
	else
		m_pA3DLitModelRender = m_pA3DDefLitModelRender;
}

A3DSkinRenderBase* A3DEngine::CreateSkinRender(bool bInit)
{
	A3DSkinRenderBase* pSkinRender = NULL;
	if(g_pA3DConfig->GetFlagHLSLEffectEnable())  {
		pSkinRender = new A3DSkinRenderWithEffect;
	}
	else  {
		pSkinRender = new A3DSkinRender;
	}

	if( bInit && ( ! pSkinRender->Init(this)) )  {
		g_A3DErrLog.Log("A3DEngine::CreateSkinRender, failed to init SkinRender!");
		A3DRELEASE(pSkinRender);
	}
	return pSkinRender;
}

void A3DEngine::BeginPerformanceRecord(int iIndex)
{
    ASSERT(iIndex >= 0 && iIndex < NUM_PERFORMANCE);
    if (g_pA3DConfig->RT_GetRecordPerformanceFlag())
    {
        m_aPerformances[iIndex].i64TimeStart = ACounter::GetCPUCycle();
    }
    //AString strDbg;
    //strDbg.Format("%2d: S counter = %I64d\n", iIndex, m_aPerformances[iIndex].i64TimeStart);
    //OutputDebugStringA(strDbg);
}

void A3DEngine::EndPerformanceRecord(int iIndex)
{
    ASSERT(iIndex >= 0 && iIndex < NUM_PERFORMANCE);
    if (g_pA3DConfig->RT_GetRecordPerformanceFlag())
    {
        __int64 iEnd = ACounter::GetCPUCycle();
        __int64 iStart = m_aPerformances[iIndex].i64TimeStart;
        m_aPerformances[iIndex].dwTimeUsed += (DWORD)(iEnd - iStart);
    }
    //AString strDbg;
    //strDbg.Format("%2d: E counter = %I64d\n", iIndex, iEnd);
    //OutputDebugStringA(strDbg);
}