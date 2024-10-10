#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DCursor.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DLightMan.h"
#include "A3DConfig.h"
#include "A3DDevObject.h"
#include "A3DVertexShader.h"
#include "A3DPixelShader.h"
#include "A3DAdditionalView.h"
#include "AList.h"
#include "ACSWrapper.h"
#include "A3DTextureMan.h"
#include "A3DDeviceCaps.h"
#include "A3DRenderTargetMan.h"
#include "A3DDeviceRSCache.h"
#include "A3DHLSL.h"
#include "A3DHLSLDataSrc.H"
#include <dxerr.h>
#include "A3DMultiThreadRender.h"
#include "A3DOccQuery.h"

//	Force to disable Nvidia PerfHUD
// #define FORCEDISABLE_PERFHUD

//	Disable ENABLE_STATECACHECHECK will cause state cache checking to be
//	ignored in A3DDevice::raw_xxx functions
#define ENABLE_STATECACHECHECK

volatile DWORD _render_thread_id = 0;
volatile int _render_record_flag = 0;
extern volatile bool _render_reset_flag;

void SetRenderThreadId(DWORD id)
{
	_render_thread_id = id;
}

void AssertMTRender()
{
#ifdef _DEBUG
	if (_render_thread_id)
	{
		assert(_render_thread_id == ::GetCurrentThreadId());
	}

	if (A3DMultiThreadRender::IsMultiThreadRender())
		assert(_render_record_flag);
#endif
}

bool GetProperDisplayFreq(DWORD dwWidth, DWORD dwHeight, DWORD dwBitsPerPixel, DWORD * pdwFrequency)
{
	int indexMode = 0;
	DWORD dwHighestCertFreq = 0;

	DEVMODE WorkingDM;

	while( EnumDisplaySettings(NULL, indexMode, &WorkingDM) )
	{
		if( dwWidth == WorkingDM.dmPelsWidth 
			&& dwHeight == WorkingDM.dmPelsHeight 
			&& dwBitsPerPixel == WorkingDM.dmBitsPerPel )
		{
			if( dwHighestCertFreq < WorkingDM.dmDisplayFrequency )
			{
				if( DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&WorkingDM, CDS_TEST) )
				{
					dwHighestCertFreq = WorkingDM.dmDisplayFrequency;
					if( dwHighestCertFreq >= 85 )
						break;
				}
			}
		}
		
		indexMode ++;
	}

	*pdwFrequency = dwHighestCertFreq;
	return true;
}

A3DDevice::A3DDevice() : m_UnmanagedDevObjectTab(1024)
{
	m_dwDeviceFlags		= 0;
	m_bHWIDevice		= false;
	m_A3DDevType		= A3DDEVTYPE_HAL;

	m_bAutoReset		= true;
	m_bNeedReset		= false;

	m_hDeviceWnd		= NULL;
	m_pD3DDevice		= NULL;
	m_pRealD3DDevice	= NULL;
	m_pBackBuffer		= new A3DSurfaceProxy;
	m_pRenderTarget		= new A3DSurfaceProxy;
	m_pDepthBuffer		= new A3DSurfaceProxy;
	m_pIntZRT			= NULL;

	m_pA3DEngine		= NULL;

	m_dwGammaLevel		= 100;

	m_A3DCullType		= A3DCULL_CCW;
	m_bHardwareTL		= false;

	m_bShowCursor		= false;
	m_pA3DCursor		= NULL;

	m_bFogEnable		= false;
	m_bFogTableEnable	= false;
	m_bFogVertEnable	= false;
	m_fogMode			= A3DFOG_LINEAR;
	m_iSkinModelRender	= SMRD_VERTEXSHADER;

	InitializeCriticalSection(&m_csDevObj);
	
	m_bEnableSetAlphaBlend = true;

	m_pDevCaps = new A3DDeviceCaps;
	m_pRTMan			= NULL;

	m_pD3DQuery = NULL;
	m_bQueryIssued = false;

	m_pRSCache = new A3DDeviceRSCache;

	//MultiThread Render---begin
	m_nCurRecorderIdx = 0;
	m_pCurCBRecorder = NULL;
	m_pCBPlayer = &m_CBPlayer;

	//FIXME!! Invilid multiThread
	//m_ComBuffer[0].Init(5 * 1024 * 1024, 1024 * 1024);
	//m_ComBuffer[1].Init(5 * 1024 * 1024, 1024 * 1024);
	//m_RResComBuffer.Init(1024 * 1024, 32 * 1024);

	m_CBRResRecorder.SetCommandBuffer(&m_RResComBuffer);
	m_CBRResRecorder.BeginCommandBuffer();

	m_CBRecorder[0].SetCommandBuffer(&m_ComBuffer[0]);
	m_CBRecorder[1].SetCommandBuffer(&m_ComBuffer[1]);
	InvalidAllCBRecorder();

	m_bResetting = false;
	//MultiThread Render---end
}

A3DDevice::~A3DDevice()
{
	DeleteCriticalSection(&m_csDevObj);

	if( m_pRSCache)
	{
		delete m_pRSCache;
		m_pRSCache = NULL;
	}

	if( m_pDevCaps)
	{
		delete m_pDevCaps;
		m_pDevCaps = NULL;
	}
}

bool A3DDevice::InitAsHWI(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType)
{
	m_dwDeviceFlags	= dwDevFlags;
	m_bHWIDevice	= true;

	// Retrieve current desktop display mode and keep it;
	HDC	hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	ReleaseDC(NULL, hdc);

	m_pA3DEngine = pA3DEngine;
	m_hDeviceWnd = hDeviceWnd;
	m_A3DDevType = devType;
	
	//Keep them;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth			= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight			= pA3DDevFmt->nHeight;
	d3dpp.Windowed					= pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow				= hDeviceWnd;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtTarget			= A3DFMT_X8R8G8B8;
	pA3DDevFmt->fmtDepth			= A3DFMT_D16;
	m_A3DDevFmt	= *pA3DDevFmt;

	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
	ConfigDevice();

	return true;
}

bool A3DDevice::Init(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType)
{
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return InitAsHWI(pA3DEngine, hDeviceWnd, pA3DDevFmt, dwDevFlags, devType);

	D3DDISPLAYMODE d3ddm;
	HRESULT	hval;

	//	Init some members;
	m_pA3DEngine	= pA3DEngine;
	m_hDeviceWnd	= hDeviceWnd;
	m_A3DDevType	= devType;
	m_dwDeviceFlags	= dwDevFlags;
	
	// First let's find what is 16-bit target format and what is 32-bit target format;
	m_fmtTarget16 = A3DFMT_UNKNOWN;
	m_fmtTarget32 = A3DFMT_UNKNOWN;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
	if( D3D_OK != hval )
	{
		hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, false);
		if( D3D_OK != hval )
		{
			g_A3DErrLog.Log("Can not determine the 16-bit target format!");
			return false;
		}
		else
		{
			m_fmtAdapter16 = A3DFMT_X1R5G5B5;
			m_fmtTarget16 = A3DFMT_X1R5G5B5;
		}
	}
	else
	{
		m_fmtAdapter16 = A3DFMT_R5G6B5;
		m_fmtTarget16 = A3DFMT_R5G6B5;
	}

	hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false);
	if( D3D_OK == hval )
	{
		m_fmtAdapter32 = A3DFMT_X8R8G8B8;
		if( dwDevFlags & A3DDEV_ALPHATARGETFIRST )
		{
			hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, false);
			if( D3D_OK == hval )
				m_fmtTarget32 = A3DFMT_A8R8G8B8;
		}
		else
			m_fmtTarget32 = A3DFMT_X8R8G8B8;
	}

	// Retrieve current desktop display mode and keep it;
	HDC	hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	ReleaseDC(NULL, hdc);

	hval = m_pA3DEngine->GetD3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get adapter display mode");
		return false;
	}

	A3DFORMAT	fmtAdapter;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth	= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight	= pA3DDevFmt->nHeight;
	d3dpp.Windowed			= pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow		= hDeviceWnd;


	if( pA3DDevFmt->bVSync )
	{
		if (pA3DDevFmt->bWindowed)
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		else 
		{
			// full screen
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; 
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
	}
	else
	{
		d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	d3dpp.BackBufferCount	= 1;
	
	if( pA3DDevFmt->bWindowed )
	{
		// see whether 16bit or 32bit color depth we are
		if( (D3DFORMAT)m_fmtAdapter32 == d3ddm.Format )
		{
			fmtAdapter = m_fmtAdapter32;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_fmtTarget32;
		}
		else
		{
			fmtAdapter = m_fmtAdapter16;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_fmtTarget16;
		}
	}
	else
	{
		if( pA3DDevFmt->fmtTarget == 0 )
		{
			// Use 32-bit first
			if( m_fmtTarget32 != A3DFMT_UNKNOWN )
			{
				fmtAdapter = m_fmtAdapter32;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
			}
			else
			{
				fmtAdapter = m_fmtAdapter16;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
			}
		}
		else 
		{
			if( pA3DDevFmt->fmtTarget == A3DFMT_X1R5G5B5 || pA3DDevFmt->fmtTarget == A3DFMT_R5G6B5 )
			{
				fmtAdapter = m_fmtAdapter16;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
			}
			else
			{
				fmtAdapter = m_fmtAdapter32;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
			}
		}
	}
	d3dpp.EnableAutoDepthStencil = TRUE;



TryAgain:
	if( d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat);
	if (FAILED(hval))
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
	{
		hval = m_pA3DEngine->GetD3D()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, 
			d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat);
		if (FAILED(hval))
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	
	}

	g_A3DErrLog.Log("Chosen Device Format: BackBuffer [%s], DepthStencil [%s]", GetFormatString((A3DFORMAT)d3dpp.BackBufferFormat), GetFormatString((A3DFORMAT)d3dpp.AutoDepthStencilFormat));
	
	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
	hval = m_pA3DEngine->GetD3D()->GetDeviceCaps(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, &m_d3dcaps);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::Init(), GetDeviceCaps() Fail.");
		return false;
	}


	DWORD dwFlags = 0;
	if (m_dwDeviceFlags & A3DDEV_ALLOWMULTITHREAD)
		dwFlags |= D3DCREATE_MULTITHREADED;
	if (m_dwDeviceFlags & A3DDEV_FPU_PRESERVE)
		dwFlags	|= D3DCREATE_FPU_PRESERVE;

#ifdef FORCEDISABLE_PERFHUD
	// When building a shipping version, disable PerfHUD (opt-out)
	dwFlags |= D3DCREATE_PUREDEVICE;
#endif


	if (!(m_dwDeviceFlags & A3DDEV_FORCESOFTWARETL) && 
		(m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	{
		
		
	UINT AdapterToUse = D3DADAPTER_DEFAULT;

	#ifdef FORCEDISABLE_PERFHUD
		g_pA3DConfig->SetPerfHUD(false);
	#else
		if (g_pA3DConfig->GetPerfHUD())
		{
			for (UINT Adapter=0;Adapter<m_pA3DEngine->GetD3D()->GetAdapterCount();Adapter++)
			{
				D3DADAPTER_IDENTIFIER9 Identifier;
				HRESULT Res;
				Res = m_pA3DEngine->GetD3D()->GetAdapterIdentifier(Adapter, 0, &Identifier);
				if (strstr(Identifier.Description,"PerfHUD") != 0)
				{
					AdapterToUse = Adapter;
					m_A3DDevType = (A3DDEVTYPE)D3DDEVTYPE_REF;
					break;
				}
			}

			//	Disable pure device to enable 'Shader visualization' function in PerfHUD
			dwFlags &= ~D3DCREATE_PUREDEVICE;
			MessageBox(m_hDeviceWnd, _AL("PerfHUD Enabled!"), _AL(""), MB_OK);
		}
	#endif	//	FORCEDISABLE_PERFHUD

		if (d3dpp.Windowed)
			d3dpp.BackBufferFormat=D3DFMT_UNKNOWN;

		/*
		 note: i dont know why ,but if auto depth stencil disabled, 
		 and d3dpp.BackBufferFormatnot is not D3DFMT_UNKNOWN,
		 CreateDevice fails, or driver crashes and blue screen comes out :-(
		 */
		if(!d3dpp.Windowed)
			GetProperDisplayFreq(d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, (d3dpp.BackBufferFormat == (D3DFORMAT) m_fmtTarget16 ? 16 : 32), (DWORD*)&d3dpp.FullScreen_RefreshRateInHz);

		if (m_pA3DEngine->GetSupportD3D9ExFlag())
		{
			D3DDISPLAYMODEEX modeEx;
			if(!d3dpp.Windowed)
			{				
				modeEx.Width = d3dpp.BackBufferWidth;
				modeEx.Height = d3dpp.BackBufferHeight;
				modeEx.RefreshRate = d3dpp.FullScreen_RefreshRateInHz;
				modeEx.Size = sizeof(D3DDISPLAYMODEEX);
				modeEx.Format = d3dpp.BackBufferFormat;
				modeEx.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
			}

			hval = m_pA3DEngine->GetD3DEx()->CreateDeviceEx(AdapterToUse, (D3DDEVTYPE)m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp, (d3dpp.Windowed ? NULL : &modeEx), (IDirect3DDevice9Ex**)&m_pD3DDevice);

			g_A3DErrLog.Log("A3DDevice::Init(), CreateDeviceEx");
		}
		else
		{
			hval = m_pA3DEngine->GetD3D()->CreateDevice(AdapterToUse, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
			
		}
		m_bHardwareTL = true;
		
	}
	else
	{
		if (m_pA3DEngine->GetSupportD3D9ExFlag())
		{
			hval = m_pA3DEngine->GetD3DEx()->CreateDeviceEx(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, NULL, (IDirect3DDevice9Ex**)&m_pD3DDevice);

			g_A3DErrLog.Log("A3DDevice::Init(), CreateDeviceEx");
		}
		else
		{
			g_A3DErrLog.Log("A3DDevice::Init(), Chosen SOFTWARE Vertex Processing...");
			hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
		}
			m_bHardwareTL = false;
	}

	if( D3D_OK != hval )
	{
		char szErrorCode[1024];
		strcpy_s(szErrorCode, "unknown");
		g_A3DErrLog.Log("Can not create device[%dx%dx%d], reason: %s", pA3DDevFmt->nWidth, pA3DDevFmt->nHeight, (pA3DDevFmt->fmtTarget == A3DFMT_X8R8G8B8 || pA3DDevFmt->fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16, szErrorCode);

		if( hval == D3DERR_OUTOFVIDEOMEMORY )
		{
			if( d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 || d3dpp.BackBufferFormat == D3DFMT_A8R8G8B8 )
			{
				fmtAdapter = m_fmtAdapter16;

				hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, 
					(D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
				if (FAILED(hval)) 
					d3dpp.BackBufferFormat = D3DFMT_X1R5G5B5;
				else
					d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
				g_A3DErrLog.Log("Now switch to 16-bit color depth, and try again...");
				goto TryAgain;
			}
		}
		
		return false;
	}


	

	// Keep them;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtAdapter = fmtAdapter;
	pA3DDevFmt->fmtTarget = (A3DFORMAT) d3dpp.BackBufferFormat;
	pA3DDevFmt->fmtDepth =(A3DFORMAT)d3dpp.AutoDepthStencilFormat;	//Set Depth Stencil format   m_pIntZRT ? A3DFMT_INTZ : 
	
	m_A3DDevFmt	= *pA3DDevFmt;

	// Log chosen format;
	g_A3DErrLog.Log("A3DDevice::Init(), Create device %dx%dx%d", pA3DDevFmt->nWidth, pA3DDevFmt->nHeight, (pA3DDevFmt->fmtTarget == A3DFMT_X8R8G8B8 || pA3DDevFmt->fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16);

	//	Create render target manager
	m_pRTMan = new A3DRenderTargetMan;
	if (!m_pRTMan || !m_pRTMan->Init(this))
	{
		g_A3DErrLog.Log("Failed to create render target manager !");
		return false;
	}

	// FIXME!! Turn on by parameters. Create IntZ Render Target
	 {
	 	A3DRELEASE(m_pIntZRT);
	 	if(CreateIntZRenderTarget(0, 0))
	 	{
	 		//m_pIntZRT->ApplyToDevice();
			m_pD3DDevice->SetDepthStencilSurface(m_pIntZRT->GetDepthSurface()->m_pD3DSurface);
	 	}
	 }

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(0, &m_pRenderTarget->m_pD3DSurface);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get rendertarget pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer->m_pD3DSurface);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get depth stencil surface pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer->m_pD3DSurface);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get backbuffer pointer!");
		return false;
	}



	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}



	// Enumerate all display modes.
	EnumDisplayModes(m_pA3DEngine->GetD3D(), m_A3DDevFmt.fmtAdapter, m_DisplayModes);
	ConfigDevice();

	//	Do capability check
	m_pDevCaps->CheckCaps(this);


	// For the first time, we set the fog data;
	SetFogEnable(true);
	SetFogTableEnable(true);
	SetFogColor(A3DCOLORRGBA(255, 255, 255, 255));
	SetFogStart(500.0f);
	SetFogEnd(800.0f);
	SetFogDensity(1.0f);


	if (D3D_OK != m_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_pD3DQuery))
	{
		m_pD3DQuery = NULL;
		m_bQueryIssued = false;
	}
	else if (m_d3dpp.Windowed)
	{
		m_pD3DQuery->Issue(D3DISSUE_END);
		m_bQueryIssued = true;
	}

	//MultiThread Render---begin
	m_pRealD3DDevice = m_pD3DDevice;
	m_CBRecorder[0].SetD3DDevice(m_pRealD3DDevice);
	m_CBRecorder[1].SetD3DDevice(m_pRealD3DDevice);
	m_pCurCBRecorder = &m_CBRecorder[m_nCurRecorderIdx];

	//MultiThread Render---end

	return true;
}

bool A3DDevice::Release()
{
	EnterCriticalSection(&m_csDevObj);
	m_UnmanagedDevObjectTab.clear();
	LeaveCriticalSection(&m_csDevObj);
    
    if (m_pBackBuffer && m_pBackBuffer->m_pD3DSurface)
    {
        m_pBackBuffer->m_pD3DSurface->Release();
    }
    if (m_pDepthBuffer && m_pDepthBuffer->m_pD3DSurface)
    {
        m_pDepthBuffer->m_pD3DSurface->Release();
    }
    if (m_pRenderTarget && m_pRenderTarget->m_pD3DSurface)
    {
        m_pRenderTarget->m_pD3DSurface->Release();
    }
	delete m_pBackBuffer;
    m_pBackBuffer = NULL;
	delete m_pRenderTarget;
    m_pRenderTarget = NULL;
	delete m_pDepthBuffer;
    m_pDepthBuffer = NULL;

	A3DRELEASE(m_pIntZRT);
	A3DRELEASE(m_pRTMan);

    if (m_pD3DQuery)
    {
        int nRef = m_pD3DQuery->Release();
        m_pD3DQuery = NULL;
        m_bQueryIssued = false;
    }


	if( m_pD3DDevice )
	{
		int nRef = m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	return true;
}

bool A3DDevice::ConfigDevice()
{
	if( m_bHWIDevice )
		return true;

	HRESULT hval;

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));
	hval = m_pD3DDevice->GetDeviceCaps(&m_d3dcaps);
	if (FAILED(hval))
		return false;

	if (!m_pRSCache->Reset(this))
		return false;
	
	if(g_pA3DConfig->GetFlagForceShaderVer20()) {
		m_d3dcaps.PixelShaderVersion = D3DPS_VERSION(2,0);
		m_d3dcaps.VertexShaderVersion = D3DVS_VERSION(2,0);
	}

	SetFaceCull(A3DCULL_CCW);

	//We use depth buffer;
	SetZTestEnable(true);
	SetZWriteEnable(true);

	//Initialize the device variables;
	SetAlphaBlendEnable(true);
	SetSourceAlpha(A3DBLEND_SRCALPHA);
	SetDestAlpha(A3DBLEND_INVSRCALPHA);

	SetDitherEnable(true);
	SetSpecularEnable(false); // If you want to make specular enable, you must set it explictly
	
	SetTextureFilterType(0, A3DTEXF_LINEAR);
	SetTextureColorOP(0, A3DTOP_MODULATE);
	SetTextureAlphaOP(0, A3DTOP_MODULATE);

	SetDeviceRenderState(D3DRS_FOGENABLE, TRUE);

	g_A3DErrLog.Log("Max vertex blend matrix index: %d", GetMaxVertBlendMatrixIndex());
	g_A3DErrLog.Log("VS version: %d.%d", D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(m_d3dcaps.VertexShaderVersion));
	g_A3DErrLog.Log("VS constant registers number: %d", GetVSConstantNum());
	g_A3DErrLog.Log("PS version: %d.%d", D3DSHADER_VERSION_MAJOR(m_d3dcaps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(m_d3dcaps.PixelShaderVersion));

	//	Determine render method of skin model
	DetermineSkinModelRenderMethod();

	return true;
}

void A3DDevice::DetermineSkinModelRenderMethod()
{
	//Angelica2.2 实际上已经不在支持VertexShader 2.0 以下版本的skinmodel渲染
	m_iSkinModelRender = SMRD_VERTEXSHADER;
	

	
/*
	//	For software TL, we consider indexed blend matrix first. For hardware TL
	//	we consider vertex shader first.
	if (!IsHardwareTL() && GetMaxVertBlendMatrixIndex() > 200)
	{
		m_iSkinModelRender = SMRD_INDEXEDVERTMATRIX;
	//	m_iSkinModelRender = SMRD_SOFTWARE;
	}
	else
	{
		if (TestVertexShaderVersion(1, 1))
		{
			m_iSkinModelRender = SMRD_VERTEXSHADER;
		}
		else if (GetMaxVertBlendMatrixIndex() > 200)
		{
			//	200 blending matrices means skin model can have 100 bone at most.
			//	Is there any display card support so many blend matrices now ?
			//	I don't down, but ATI 9700 only supports 38 blending matrices.
			m_iSkinModelRender = SMRD_INDEXEDVERTMATRIX;
		}
		else
		{
			m_iSkinModelRender = SMRD_SOFTWARE;
		}
	}
*/
//	m_iSkinModelRender = SMRD_SOFTWARE;
	
	switch (m_iSkinModelRender)
	{
	case SMRD_VERTEXSHADER:

		g_A3DErrLog.Log("Skin model render method: Vertex shader.");
		break;

	case SMRD_INDEXEDVERTMATRIX:

		g_A3DErrLog.Log("Skin model render method: Indexed Vertex Blend Matrix.");
		break;

	case SMRD_SOFTWARE:

		g_A3DErrLog.Log("Skin model render method: Software.");
		break;
	}
}

//	Handle device lost
bool A3DDevice::HandleDeviceLost()
{
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICENOTRESET)
	{
		if (!m_bAutoReset)
			m_bNeedReset = true;
		else if (!Reset())
			return false;
	}	

	return true;
}

bool A3DDevice::SetWorldMatrix(const A3DMATRIX4& matWorld, int iIndex)
{
	if( m_bHWIDevice )	return true;
	return raw_SetTransform(D3DTS_WORLDMATRIX(iIndex), (D3DMATRIX*)&matWorld) == D3D_OK;
}

bool A3DDevice::SetViewMatrix(const A3DMATRIX4& matView)
{
	if( m_bHWIDevice )	return true;
	return raw_SetTransform(D3DTS_VIEW, (D3DMATRIX *)&matView) == D3D_OK;
}

bool A3DDevice::SetProjectionMatrix(const A3DMATRIX4& matProject)
{
	if( m_bHWIDevice )	return true;
	return raw_SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&matProject) == D3D_OK;
}

bool A3DDevice::SetTextureMatrix(int iStage, const A3DMATRIX4& matTexture)
{
	if( m_bHWIDevice )	return true;
	return raw_SetTransform((D3DTRANSFORMSTATETYPE)(iStage + D3DTS_TEXTURE0), (D3DMATRIX *)&matTexture) == D3D_OK;
}
const A3DMATRIX4& A3DDevice::GetWorldMatrix()
{ 
	return m_pRSCache->m_aWorldMats[0];
}
 
const A3DMATRIX4& A3DDevice::GetViewMatrix()
{ 
 	return m_pRSCache->m_matView;
}
 
const A3DMATRIX4& A3DDevice::GetProjectionMatrix()
{ 
 	return m_pRSCache->m_matProj;
}

const A3DMATRIX4& A3DDevice::GetTextureMatrix(int iTexStage)
{ 
	return m_pRSCache->m_aTexStages[iTexStage].matTrans;
}

bool A3DDevice::BeginRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->BeginScene();
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevcie BeginScene Failure!");
		return false;
	}

	m_dwDrawVertCount = 0;
	m_dwDrawFaceCount = 0;
	m_dwDrawCount = 0;
	
	return true;
}

bool A3DDevice::EndRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	
	// We should show cursor here;
	if( m_pA3DCursor && m_bShowCursor )
	{
		m_pA3DCursor->TickAnimation();
		m_pA3DCursor->Draw();
	}

	hval = m_pD3DDevice->EndScene();
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevcie EndScene Failure!");
		return false;
	}

	return true;
}

bool A3DDevice::Present()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if (m_pA3DEngine->GetSupportD3D9ExFlag())
	{
		if (GetNeedResetFlag())
			return true;

		hval = ((IDirect3DDevice9Ex*)m_pD3DDevice)->PresentEx(NULL, NULL, NULL, NULL, NULL);
	}
	else
		hval = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	if (FAILED(hval))
	{
		if( hval == D3DERR_DEVICELOST )
			return HandleDeviceLost();
		else
		{
			g_A3DErrLog.Log("A3DDevice: Present Failure!");
			return false;
		}
	}
	bool bQueryRenderOn = true;

	if (m_d3dpp.Windowed && m_pD3DQuery && bQueryRenderOn)
	{
		if (m_bQueryIssued)
		{
			while (S_FALSE == m_pD3DQuery->GetData(NULL, 0, D3DGETDATA_FLUSH))
			{
			}

			m_bQueryIssued = false;
		}

		m_pD3DQuery->Issue(D3DISSUE_END);
		m_bQueryIssued = true;
		m_pD3DQuery->GetData(NULL, 0, D3DGETDATA_FLUSH);
	}

	return true;
}

bool A3DDevice::Clear(DWORD dwFlags, A3DCOLOR color, FLOAT vZ, DWORD dwStencil)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->Clear(0, NULL, dwFlags, color, vZ, dwStencil);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevice: Clear() Failure! err=%x", hval);
		return false;
	}

	return true;
}

bool A3DDevice::SetTextureFilterType(int nTextureStage, A3DTEXTUREFILTERTYPE filterType)
{
	if( m_bHWIDevice )	return true;

	SetSamplerState(nTextureStage, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)filterType);
	SetSamplerState(nTextureStage, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)filterType);

	if( g_pA3DConfig->GetFlagTrilinear() )
		SetSamplerState(nTextureStage, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE) filterType);
	else
		SetSamplerState(nTextureStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	return true;
}

//	Get texture coordinate index
int A3DDevice::GetTextureCoordIndex(int iTexStage)
{ 
	return m_pRSCache->m_aTexStages[iTexStage].TSMap[D3DTSS_TEXCOORDINDEX];
}

bool A3DDevice::SetTextureColorOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;
	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLOROP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureColorArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLORARG1, dwArg1);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLORARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureAlphaOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAOP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureAlphaArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAARG1, dwArg1);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureResultArg(int nTextureStage, DWORD dwArg)
{
	if( m_bHWIDevice )	return true;
	SetDeviceTextureStageState(nTextureStage, D3DTSS_RESULTARG, dwArg);
	return true;
}

A3DTEXTUREOP A3DDevice::GetTextureColorOP(int iTextureStage)
{
	if (m_bHWIDevice)
		return A3DTOP_DISABLE;

	DWORD op = GetDeviceTextureStageState(iTextureStage, D3DTSS_COLOROP);
	return (A3DTEXTUREOP)op;
}

A3DTEXTUREOP A3DDevice::GetTextureAlphaOP(int iTextureStage)
{
	if (m_bHWIDevice)
		return A3DTOP_DISABLE;

	DWORD op = GetDeviceTextureStageState(iTextureStage, D3DTSS_ALPHAOP);
	return (A3DTEXTUREOP)op;
}

bool A3DDevice::SetSamplerState(int layer, D3DSAMPLERSTATETYPE type, DWORD value)
{
	if( m_bHWIDevice )	return true;

	if ( (layer < 0 || layer >= MAX_SAMPLE_LAYERS)
		|| (layer >= MIN_VS_SAMPLE_LAYERS && layer <= MAX_VS_SAMPLE_LAYERS) )
	{
		ASSERT( (layer >= 0 && layer < MAX_SAMPLE_LAYERS) || (layer >= MIN_VS_SAMPLE_LAYERS && layer <= MAX_VS_SAMPLE_LAYERS));
		return false;
	}
	return raw_SetSamplerState(layer, (D3DSAMPLERSTATETYPE)type, value) == D3D_OK;
}

DWORD A3DDevice::GetSamplerState(int layer, D3DSAMPLERSTATETYPE type)
{
	if(m_bHWIDevice) return 0;
	if(layer >= 0 || layer < MAX_SAMPLE_LAYERS)
		return m_pRSCache->m_aSSMaps[layer][(D3DSAMPLERSTATETYPE)type];
	if(layer >= MIN_VS_SAMPLE_LAYERS && layer <= MAX_VS_SAMPLE_LAYERS)
		return m_pRSCache->m_aVsSSMaps[layer - MIN_VS_SAMPLE_LAYERS][(D3DSAMPLERSTATETYPE)type];
	else
	{
		ASSERT( (layer >= 0 || layer < MAX_SAMPLE_LAYERS) || (layer >= MIN_VS_SAMPLE_LAYERS && layer <= MAX_VS_SAMPLE_LAYERS));
		return 0;
	}
}

bool A3DDevice::SetTextureAddress(int nTextureStage, A3DTEXTUREADDR UAddrMethod, A3DTEXTUREADDR VAddrMethod)
{
	if( m_bHWIDevice )	return true;

	SetSamplerState(nTextureStage, D3DSAMP_ADDRESSU, (D3DTEXTUREADDRESS) UAddrMethod);
	SetSamplerState(nTextureStage, D3DSAMP_ADDRESSV, (D3DTEXTUREADDRESS) VAddrMethod);
	return true;
}

bool A3DDevice::SetTextureCoordIndex(int nTextureStage, int iIndex)
{
	if( m_bHWIDevice )	return true;
	raw_SetTextureStageState(nTextureStage, D3DTSS_TEXCOORDINDEX, iIndex);
	return true;
}

bool A3DDevice::SetTextureBumpMat(int nTextureStage, float _00, float _01, float _10, float _11)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT00, *((DWORD*)(&_00)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT01, *((DWORD*)(&_01)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT10, *((DWORD*)(&_10)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT11, *((DWORD*)(&_11)));
	return true;
}

bool A3DDevice::SetTextureBumpScale(int nTextureStage, float fScale)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVLSCALE, *((DWORD*)(&fScale)));
	return true;
}

bool A3DDevice::SetTextureBumpOffset(int nTextureStage, float fOffset)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVLOFFSET, *((DWORD*)(&fOffset)));
	return true;
}

bool A3DDevice::SetTextureTransformFlags(int nTextureStage, A3DTEXTURETRANSFLAGS Flags)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, Flags);
	return true;
}

bool A3DDevice::SetZTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::SetZWriteEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZWRITEENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::GetZTestEnable()
{
	return GetDeviceRenderState(D3DRS_ZENABLE) ? true : false;
}

bool A3DDevice::GetZWriteEnable()
{
	return GetDeviceRenderState(D3DRS_ZWRITEENABLE) ? true : false;
}

bool A3DDevice::SetZBias(float fBias)
{
	if( m_bHWIDevice )	return true;
	DWORD dwBias = *((DWORD*) &fBias);
	SetDeviceRenderState(D3DRS_DEPTHBIAS, dwBias);
	return true;
}

bool A3DDevice::SetZFunc(DWORD dwFunc)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZFUNC, dwFunc);
	return true;
}
bool A3DDevice::GetAlphaBlendEnable()
{
	return GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;
}

bool A3DDevice::SetAlphaBlendEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_ALPHABLENDENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetSourceAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;

	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_SRCBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetDestAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;
	
	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_DESTBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetAlphaTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHATESTENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::GetAlphaTestEnable()
{
	if( m_bHWIDevice )	return false;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHATESTENABLE);
	return dwValue ? true : false;
}

bool A3DDevice::SetAlphaFunction(A3DCMPFUNC cmpFunc)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHAFUNC, cmpFunc);
	return true;
}

A3DCMPFUNC A3DDevice::GetAlphaFunction()
{
	if( m_bHWIDevice )	return A3DCMP_LESS;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHAFUNC);
	return (A3DCMPFUNC)dwValue;
}

bool A3DDevice::SetAlphaRef(DWORD dwRefValue)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHAREF, dwRefValue);
	return true;
}

DWORD A3DDevice::GetAlphaRef()
{
	if( m_bHWIDevice )	return 0;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHAREF);
	return dwValue;
}

bool A3DDevice::SetLightingEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_LIGHTING, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetLightParam(int nLightID, A3DLIGHTPARAM * pLightParam)
{
	if( m_bHWIDevice )	return true;
	return raw_SetLight(nLightID, (CONST D3DLIGHT9*)pLightParam) == D3D_OK;
}

bool A3DDevice::LightEnable(int nLightID, bool bEnable)
{
	if( m_bHWIDevice )	return true;
	return raw_LightEnable(nLightID, bEnable ? TRUE : FALSE) == D3D_OK;
}

bool A3DDevice::SetAmbient(A3DCOLOR colorAmbient)
{
	if( m_bHWIDevice )	return true;

	m_colorAmbient = colorAmbient;
	m_colorValueAmbient = a3d_ColorRGBAToColorValue(colorAmbient);

	SetDeviceRenderState(D3DRS_AMBIENT, colorAmbient);
	return true;
}

bool A3DDevice::SetMaterial(const A3DMATERIALPARAM* pMaterialParam, bool b2Sided)
{
	if (m_bHWIDevice)
		return true;

	raw_SetMaterial((D3DMATERIAL9*)pMaterialParam);
	if (b2Sided)
	{
		SetDeviceRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		//	Set To Original Configuration;
		SetFaceCull(m_A3DCullType);
	}
	return true;
}

bool A3DDevice::SetViewport(A3DVIEWPORTPARAM * pViewportParam)
{
	if( m_bHWIDevice )	return true;

	D3DVIEWPORT9 viewport;

	memcpy(&viewport, pViewportParam, sizeof(A3DVIEWPORTPARAM));
	m_pD3DDevice->SetViewport(&viewport);

	return true;
}

bool A3DDevice::GetViewport(A3DVIEWPORTPARAM * pViewportParam)
{
	if( m_bHWIDevice )	
	{
		return true;
	}

	D3DVIEWPORT9 sViewport;
	m_pD3DDevice->GetViewport(&sViewport);

	pViewportParam->X		= sViewport.X;
	pViewportParam->Y		= sViewport.Y;	
	pViewportParam->Width	= sViewport.Width;
	pViewportParam->Height	= sViewport.Height;
	pViewportParam->MinZ	= sViewport.MinZ;
	pViewportParam->MaxZ	= sViewport.MaxZ;

	return true;
}

bool A3DDevice::SetFaceCull(A3DCULLTYPE type)
{
	if (m_bHWIDevice)
		return true;

	m_A3DCullType = type;

	switch (type)
	{
	case A3DCULL_CW:	raw_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);	break;
	case A3DCULL_CCW:	raw_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	break;
	case A3DCULL_NONE:	raw_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	break;
	default:
		ASSERT(0);
		return false;
	}

	return true;
}

bool A3DDevice::SetFillMode(A3DFILLMODE mode)
{
	if( m_bHWIDevice )	return true;

	switch(mode)
	{
	case A3DFILL_POINT:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		break;
	case A3DFILL_WIREFRAME:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case A3DFILL_SOLID:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	default:
		return false;
	}

	return true;
}

bool A3DDevice::DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	AssertMTRender();


	HRESULT hval;
	
	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);


	hval = m_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)Type, 0, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawIndexedPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, int BaseVertexIndex, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;


	AssertMTRender();

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	hval = m_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawIndexedPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitive(A3DPRIMITIVETYPE Type, DWORD StartVertex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	AssertMTRender();

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	hval = m_pD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)Type, StartVertex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawIndexedPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD PrimitiveCount, void* pIndexData, A3DFORMAT IndexDataFormat, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	AssertMTRender();

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	hval = m_pD3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)Type, MinIndex, NumVertices, PrimitiveCount, pIndexData, (D3DFORMAT) IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawIndexedPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD PrimitiveCount, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	AssertMTRender();

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	hval = m_pD3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)Type, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::CopyToBack(int x, int y, int width, int height, IDirect3DSurface9 * pSurface)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	RECT	rect;
	POINT	offset;

	rect.left		= 0;
	rect.top		= 0;
	rect.right		= width;
	rect.bottom		= height;
	offset.x		= x;
	offset.y		= y;

	hval = m_pD3DDevice->UpdateSurface(pSurface, &rect, m_pBackBuffer->m_pD3DSurface, &offset);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::CopyToBack() Fail");
		return false;
	}

	return true;
}

bool A3DDevice::CreateViewport(A3DViewport ** ppViewport, DWORD x, DWORD y, DWORD width, DWORD height, FLOAT minZ, FLOAT maxZ,
		bool bClearColor, bool bClearZ, A3DCOLOR colorClear)
{
	A3DViewport *		pViewport;
	A3DVIEWPORTPARAM	param;

	pViewport = new A3DViewport();
	if( NULL == pViewport )
	{
		g_A3DErrLog.Log("A3DDevice::CreateViewport() Not enough memory!");
		return false;
	}

	ZeroMemory(&param, sizeof(param));
	param.X = x;
	param.Y = y;
	param.Width = width;
	param.Height = height;
	param.MinZ = minZ;
	param.MaxZ = maxZ;

	if( !pViewport->Init(this, &param, bClearColor, bClearZ, colorClear) )
	{
		delete pViewport;
		pViewport = NULL;
		*ppViewport = NULL;
		g_A3DErrLog.Log("A3DDevice::CreateViewport() Viewport Init Fail!");
		return false;
	}

	*ppViewport = pViewport;
	return true;
}

bool A3DDevice::CreateRenderTarget(A3DRenderTarget ** ppRenderTarget, int width, int height,
								   A3DFORMAT fmtTarget, A3DFORMAT fmtDepth, bool bNewTarget, bool bNewDepth)
{
	A3DRenderTarget::RTFMT	devFmt;	
	A3DRenderTarget *	pRenderTarget;

	pRenderTarget = new A3DRenderTarget();
	if( NULL == pRenderTarget )
	{
		g_A3DErrLog.Log("A3DDevice::CreateRenderTarget() Not enough memory!");
		return false;
	}

	ZeroMemory(&devFmt, sizeof(A3DRenderTarget::RTFMT));
	devFmt.iWidth = width;
	devFmt.iHeight = height;
	if( fmtTarget != A3DFMT_UNKNOWN )
		devFmt.fmtTarget = fmtTarget;
	else
		devFmt.fmtTarget = m_A3DDevFmt.fmtTarget;
	if( fmtDepth != A3DFMT_UNKNOWN )
		devFmt.fmtDepth = fmtDepth;
	else
		devFmt.fmtDepth = m_A3DDevFmt.fmtDepth;

	if( !pRenderTarget->Init(this, devFmt, bNewTarget, bNewDepth) )
	{
		delete pRenderTarget;
		pRenderTarget = NULL;
		*ppRenderTarget = NULL;
		g_A3DErrLog.Log("A3DDevice::CreateRenderTarget() RenderTarget Init Fail!");
		return false;
	}

	*ppRenderTarget = pRenderTarget;
	return true;
}


bool A3DDevice::IsDetailMethodSupported()
{
	if( m_bHWIDevice )	return true;

	if( (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
		(m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR) &&
		(m_d3dcaps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR) )
		return true;

	return false;
}

bool A3DDevice::ClearTexture(int nLayer)
{
	if( m_bHWIDevice )	return true;

	return raw_SetTexture(nLayer, NULL) == D3D_OK;
}

//	Set texture factor
bool A3DDevice::SetTextureFactor(DWORD dwValue)
{
	if( m_bHWIDevice )	return true;
	SetDeviceRenderState(D3DRS_TEXTUREFACTOR, dwValue);
	return true;
}

bool A3DDevice::SetFogEnable(bool bEnable)
{
	m_bFogEnable = bEnable;

	if( m_bFogEnable )
	{
		SetDeviceRenderState(D3DRS_FOGENABLE, TRUE);
		SetFogTableEnable(m_bFogTableEnable);
	}
	else
	{
		// We set both fog algorithm to none to allow user defined fog data in LVertex;
		// And we can turn off Vertex's fog data by doint this!
		SetDeviceRenderState(D3DRS_FOGENABLE, FALSE);
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}

	return true;
}

bool A3DDevice::SetFogTableEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_bFogTableEnable = bEnable;
	
	if( m_bFogTableEnable && (m_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, (D3DFOGMODE) m_fogMode);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}
	else
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, (D3DFOGMODE) m_fogMode);
	}

	return true;
}

bool A3DDevice::SetFogVertEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_bFogVertEnable = bEnable;
	
	if( m_bFogVertEnable )
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, (D3DFOGMODE) m_fogMode);
	}
	else
	{
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}

	return true;
}

bool A3DDevice::SetFogMode(A3DFOGMODE fogMode)
{
	m_fogMode = fogMode;

	SetFogTableEnable(m_bFogTableEnable);
	SetFogVertEnable(m_bFogVertEnable);
	return true;
}
	
bool A3DDevice::SetFogColor(A3DCOLOR fogColor)
{
	if( m_bHWIDevice )	return true;

	m_colorFog = fogColor;
	SetDeviceRenderState(D3DRS_FOGCOLOR, m_colorFog);

	return true;
}

bool A3DDevice::SetFogStart(FLOAT vStart)
{
	if( m_bHWIDevice )	return true;

	m_vFogStart = vStart;
	SetDeviceRenderState(D3DRS_FOGSTART, *(DWORD *)&m_vFogStart);

	return true;
}

bool A3DDevice::SetFogEnd(FLOAT vEnd)
{
	if( m_bHWIDevice )	return true;

	m_vFogEnd = vEnd;
	SetDeviceRenderState(D3DRS_FOGEND, *(DWORD *)&m_vFogEnd);

	return true;
}

bool A3DDevice::SetFogDensity(FLOAT vDensity)
{
	if( m_bHWIDevice )	return true;

	m_vFogDensity = vDensity;
	SetDeviceRenderState(D3DRS_FOGDENSITY, *(DWORD *)&m_vFogDensity);

	return true;
}

char * A3DDevice::GetFormatString(A3DFORMAT format)
{
	switch(format)
	{
    case A3DFMT_R8G8B8:
		return "R8G8B8";

    case A3DFMT_A8R8G8B8:
		return "A8R8G8B8";

    case A3DFMT_X8R8G8B8:
		return "X8R8G8B8";

    case A3DFMT_R5G6B5:
		return "R5G6B5";

    case A3DFMT_X1R5G5B5:
		return "X1R5G5B5";

    case A3DFMT_A1R5G5B5:
		return "A1R5G5B5";

	case A3DFMT_A4R4G4B4:
		return "A4R4G4B4";

    case A3DFMT_R3G3B2:
		return "R3G3B2";

    case A3DFMT_A8:
		return "A8";

	case A3DFMT_A8R3G3B2:
		return "A8R3G3B2";

    case A3DFMT_X4R4G4B4:
		return "A8R3G3B2";

    case A3DFMT_A8P8:
		return "A8P8";

    case A3DFMT_P8:
		return "P8";

    case A3DFMT_L8:
		return "L8";

    case A3DFMT_A8L8:
		return "A8L8";

    case A3DFMT_A4L4:
		return "A4L4";

    case A3DFMT_V8U8:
		return "V8U8";

    case A3DFMT_L6V5U5:
		return "L6V5U5";

    case A3DFMT_X8L8V8U8:
		return "X8L8V8U8";

    case A3DFMT_Q8W8V8U8:
		return "Q8W8V8U8";

    case A3DFMT_V16U16:
		return "V16U16";

	case A3DFMT_D16_LOCKABLE:
		return "D16_LOCKABLE";

    case A3DFMT_D32:
		return "D32";

    case A3DFMT_D15S1:
		return "D15S1";

    case A3DFMT_D24S8:
		return "D24S8";

    case A3DFMT_D16:
		return "D16";

    case A3DFMT_D24X8:
		return "D24X8";

    case A3DFMT_D24X4S4:
		return "D24X4S4";
	}
	return "Unkown Format";
}

bool A3DDevice::SetDisplayMode(int nWidth, int nHeight, A3DFORMAT format, bool bWindowed, bool bVSync, DWORD dwFlags)
{
	if( m_bHWIDevice )
	{
		// We only take notes of the new parameters, assuming we have set it already!
		if( dwFlags & SDM_WIDTH )
			m_d3dpp.BackBufferWidth = nWidth;
		if( dwFlags & SDM_HEIGHT )
			m_d3dpp.BackBufferHeight = nHeight;
		if( dwFlags & SDM_FORMAT )
			m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
		if( dwFlags & SDM_WINDOW )
			m_d3dpp.Windowed = bWindowed;
		
		if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		else
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;

		m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;

		m_A3DDevFmt.fmtAdapter = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;

		// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
		m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
		switch(m_A3DDevFmt.fmtTarget)
		{
		case A3DFMT_X1R5G5B5:
		case A3DFMT_R5G6B5:
			m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
			m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
			break;
		case A3DFMT_X8R8G8B8:
		case A3DFMT_A8R8G8B8:
			m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
			m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
			break;
		}
		return true;
	}

	// Now it is a real hardware related device, we have to do normal work;
	if( dwFlags & SDM_WIDTH )
		m_d3dpp.BackBufferWidth = nWidth;
	if( dwFlags & SDM_HEIGHT )
		m_d3dpp.BackBufferHeight = nHeight;
	if( (dwFlags & SDM_FORMAT) && !m_A3DDevFmt.bWindowed )
		m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
	if( dwFlags & SDM_WINDOW )
		m_d3dpp.Windowed = bWindowed;
	if( dwFlags & SDM_VSYNC )
	{
		if( bVSync )
		{
			if (m_d3dpp.Windowed)
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;			
			else 
			{
				// full screen
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
				m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}
		}
		else
		{
			if (m_d3dpp.Windowed)
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
			else 
			{
				// full screen
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
				m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}
		}
	}
	
	A3DFORMAT fmtAdapter;
	if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
	{
		fmtAdapter = m_fmtAdapter16;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
	{
		fmtAdapter = m_fmtAdapter32;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	}

	HRESULT hval;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, m_d3dpp.AutoDepthStencilFormat);
	if (FAILED(hval))
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	bool bval = Reset();
	if (bval)
	{
		// Update Device information;
		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;
		m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;

		m_A3DDevFmt.fmtAdapter = fmtAdapter;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;
	}

	// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	g_A3DErrLog.Log("A3DDevice::SetDisplayMode(), Set to %dx%dx%d", m_A3DDevFmt.nWidth, m_A3DDevFmt.nHeight, (m_A3DDevFmt.fmtTarget == A3DFMT_X8R8G8B8 || m_A3DDevFmt.fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16);
	return bval;
}

bool A3DDevice::Reset()
{
	m_bResetting = true;
	if( m_bHWIDevice ) 
	{
		m_bResetting = false;
		return true;
	}
	
	HRESULT hval;

	// We must test the cooperative level here again to ensure we can call to reset;
	hval = m_pD3DDevice->TestCooperativeLevel();
	if (hval != D3DERR_DEVICENOTRESET && hval != D3D_OK)
	{
		m_bResetting = false;
		return true;
	}

	A3DINTERFACERELEASE(m_pRenderTarget->m_pD3DSurface);
	A3DINTERFACERELEASE(m_pDepthBuffer->m_pD3DSurface);
	A3DINTERFACERELEASE(m_pBackBuffer->m_pD3DSurface);

	//	Handle unmanaged device objects
	ACSWrapper csa(&m_csDevObj);

    m_pRTMan->ClearRentableRenderTargets();
	DevObjTable::iterator it = m_UnmanagedDevObjectTab.begin();
	for (; it != m_UnmanagedDevObjectTab.end(); ++it)
	{
		A3DDevObject* pObject = *it.value();
		pObject->BeforeDeviceReset();
	}

	// confirm desktop bit depth if reset in window mode
	if( m_d3dpp.Windowed /* && m_A3DDevFmt.bWindowed */)
	{
		HDC	hdc = GetDC(NULL);
		int nBitsNow = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);
		if( nBitsNow != m_DesktopMode.nBitsPerPixel )
		{
			m_d3dpp.BackBufferFormat = (D3DFORMAT) (nBitsNow == 16 ? m_fmtTarget16 : m_fmtTarget32);
			m_DesktopMode.nBitsPerPixel = nBitsNow;
			A3DFORMAT fmtAdapter;
			if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
			{
				fmtAdapter = m_fmtAdapter16;
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			}
			else
			{
				fmtAdapter = m_fmtAdapter32;
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
			}

			hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_d3dpp.AutoDepthStencilFormat);
			if (FAILED(hval))
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			m_A3DDevFmt.fmtAdapter = fmtAdapter;
			m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
			m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;
			// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
			m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
			switch(m_A3DDevFmt.fmtTarget)
			{
			case A3DFMT_X1R5G5B5:
			case A3DFMT_R5G6B5:
				m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
				m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
				break;
			case A3DFMT_X8R8G8B8:
			case A3DFMT_A8R8G8B8:
				m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
				m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
				break;
			}
		}
	}

	// clean up memory here first before we reset to avoid small fragments in video memory
	EvictManagedResources();

	if (m_pD3DQuery)
	{
		m_pD3DQuery->Release();
		m_pD3DQuery = NULL;
		m_bQueryIssued = false;
	}

	if (A3DMultiThreadRender::IsMultiThreadRender())
		assert(m_CBRResRecorder.GetUsedNumDWORD() == 0);

	g_A3DErrLog.Log("A3DDevice:Reset() Device Lost, now try to reset it...");

	m_d3dpp.FullScreen_RefreshRateInHz = 0;//must set this to 0 otherwise crash
	hval = m_pD3DDevice->Reset(&m_d3dpp);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Reset Device Failure, ERROR=%x!", hval);
		m_bResetting = false;
		return false;
	}

	g_A3DErrLog.Log("A3DDevice:Reset() Reset Device Succeed.");


/*m_pD3DDevice->SetDepthStencilSurface(m_pIntZRT->GetDepthSurface()->m_pD3DSurface);*/

	//	Handle unmanaged device objects
	it = m_UnmanagedDevObjectTab.begin();
	for (; it != m_UnmanagedDevObjectTab.end(); ++it)
	{
		A3DDevObject* pObject = *it.value();
		pObject->AfterDeviceReset();
	}

	 //Reset IntZ renderTarget
	 if(m_pIntZRT)
	 {
	 	A3DRELEASE(m_pIntZRT);
	 	if(CreateIntZRenderTarget(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight))
	 	{
	 		//SetRenderTarget(m_pIntZRT);
	 		m_pD3DDevice->SetDepthStencilSurface(m_pIntZRT->GetDepthSurface()->m_pD3DSurface);
	 	}
	 }


	csa.Detach(true);

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(0, &m_pRenderTarget->m_pD3DSurface);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get rendertarget pointer!");
		m_bResetting = false;
		return false;
	}

	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer->m_pD3DSurface);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get depth stencil surface pointer!");
		m_bResetting = false;
		return false;
	}

	hval = m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer->m_pD3DSurface);
	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get backbuffer pointer!");
		m_bResetting = false;
		return false;
	}

	ConfigDevice();

	m_pRTMan->Reset();

	// Now set some special parameters;
	SetAmbient(m_colorAmbient);
	SetFogTableEnable(m_bFogTableEnable);
	SetFogColor(m_colorFog);
	SetFogStart(m_vFogStart);
	SetFogEnd(m_vFogEnd);
	SetFogDensity(m_vFogDensity);
	m_pA3DEngine->GetA3DLightMan()->RestoreLightResource();
	m_pA3DEngine->GetA3DTextureMan()->UpdateEffectTexPtr();

	if (D3D_OK != m_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_pD3DQuery))
	{
		m_pD3DQuery = NULL;
		m_bQueryIssued = false;
	}
	else if (m_d3dpp.Windowed)
	{
		m_pD3DQuery->Issue(D3DISSUE_END);
		m_bQueryIssued = true;
	}

	_render_reset_flag = false;
	m_bNeedReset = false;
	m_bResetting = false;
	return true;
}

bool A3DDevice::SetSpecularEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_SPECULARENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetDitherEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_DITHERENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetPointSize(FLOAT vPointSize)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_POINTSIZE, *(DWORD *)&vPointSize);
	return true;
}

bool A3DDevice::SetPointSpriteEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_POINTSPRITEENABLE, bEnable ? TRUE : FALSE);
	return true;	
}

bool A3DDevice::SetSoftwareVertexProcessing(bool bEnable)
{
	if (m_bHWIDevice) return true;

	if (!IsHardwareTL() || TestVertexShaderVersion(1, 1))
		return true;
	
	if (bEnable)
		m_pD3DDevice->SetSoftwareVertexProcessing(TRUE);
	else
		m_pD3DDevice->SetSoftwareVertexProcessing(FALSE);
	
	// for switch to or from software vertex processing in mixed mode, all render states may lost, 
	// so reset them to unset state.
	//m_TextureStageStates.Reset();
	//m_TextureCoordIndex.Reset();
	
	return true;
}

//	Add unmanaged device object
bool A3DDevice::AddUnmanagedDevObject(A3DDevObject* pObject)
{
	if (!pObject)
		return false;

	if (m_bHWIDevice)
		return true;
	
	ACSWrapper csa(&m_csDevObj);
	bool bRet = (DWORD)m_UnmanagedDevObjectTab.put((int)pObject, pObject);
	ASSERT(bRet);	//	Ensure don't add same object twice
	return bRet;
}

//	Remove unmanaged devcie object
void A3DDevice::RemoveUnmanagedDevObject(A3DDevObject* pObject)
{
	if (m_bHWIDevice || !pObject)
		return;

	ACSWrapper csa(&m_csDevObj);
	m_UnmanagedDevObjectTab.erase((int)pObject);
}

bool A3DDevice::GetClipPlane(DWORD Index, float* pPlane)
{
	//return m_pD3DDevice->GetClipPlane(Index, pPlane) == D3D_OK;
	if (Index >= 0 && Index < MAX_CLIP_PLANES)
	{
		A3DCOLORVALUE& c = m_pRSCache->m_aClipPlanes[Index];
		pPlane[0] = c.r;
		pPlane[1] = c.g;
		pPlane[2] = c.b;
		pPlane[3] = c.a;
		return true;
	}
	else
	{
		ASSERT(Index >= 0 && Index < MAX_CLIP_PLANES);
		return false;
	}
}

bool A3DDevice::SetClipPlane(DWORD Index, const float* pPlane)
{
	if (Index < 0 || Index >= MAX_CLIP_PLANES)
	{
		ASSERT(Index >= 0 && Index < MAX_CLIP_PLANES);
		return false;
	}

	return raw_SetClipPlane(Index, pPlane) == D3D_OK;
}

int A3DDevice::GetGammaLevel()
{
	return m_dwGammaLevel;
}

bool A3DDevice::SetGammaLevel(DWORD dwLevel)
{
	m_dwGammaLevel = dwLevel;

	if( m_bHWIDevice ) return true;

	D3DGAMMARAMP gammaRamp;

	float gamma = dwLevel / 100.0f;
	if( gamma == 0.0f )
	{
		memset(&gammaRamp, 0, sizeof(gammaRamp));
	}
	else
	{
		for(int i=0; i<256; i++)
		{
			WORD out = (WORD)(65535 * pow(i / 255.0f, 1.0f / gamma));
			if( i > 0 )
				out = max2(1, out);
			gammaRamp.red[i] = gammaRamp.green[i] = gammaRamp.blue[i] = out;
		}
	}

	m_pD3DDevice->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &gammaRamp);
	return true;
}

void A3DDevice::ShowCursor(bool bShow)
{
	m_bShowCursor = bShow;
	if( m_pA3DCursor )
		m_pA3DCursor->ShowCursor(bShow);
}

void A3DDevice::SetCursor(A3DCursor * pA3DCursor)
{
	m_pA3DCursor = pA3DCursor;
}

bool A3DDevice::ExportColorToFile(char * szFullpath, bool bJPG)
{
	if( m_bHWIDevice ) return true;

	if( !m_pBackBuffer )
		return false;

	HRESULT hval;
	D3DXIMAGE_FILEFORMAT texFmt = bJPG? D3DXIFF_JPG : D3DXIFF_BMP;
	hval = D3DXSaveSurfaceToFileA(szFullpath, texFmt, m_pBackBuffer->m_pD3DSurface, NULL, NULL);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevice::ExportColorToFile(), Can not save color surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DDevice::ExportDepthToFile(char * szFullpath)
{
	if( m_bHWIDevice ) return true;

	// Currently not working;
	if( !m_pDepthBuffer )
		return false;

	HRESULT hval;

	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_pDepthBuffer->m_pD3DSurface, NULL, NULL);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DDevice::ExportDepthToFile(), Can not save depth surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DDevice::SetDeviceWnd(HWND hWnd)
{
	if (!m_d3dpp.Windowed)
	{
		g_A3DErrLog.Log("A3DDevice::SetDeviceWnd(), can not change device window in fullscreen mode!");
		return false;
	}

	m_hDeviceWnd = hWnd;
	m_d3dpp.hDeviceWindow = m_hDeviceWnd;

	if (!Reset())
		return false;
	
	return true;
}

//	indexed vertex matrix blend enable
bool A3DDevice::SetIndexedVertexBlendEnable(bool bEnable)
{
	if (m_bHWIDevice) 
		return true;

	SetDeviceRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, bEnable ? TRUE : FALSE); 
	return true;
}

//	Set blend matrix used in indexed vertex blending
bool A3DDevice::SetIndexedVertexBlendMatrix(int iIndex, const A3DMATRIX4& mat)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(iIndex), (D3DMATRIX*) &mat)))
	{
		g_A3DErrLog.Log("A3DDevice::SetIndexedVertexBlendMatrix, failed to set %d blend matrix", iIndex); 
		return false;
	}
	
	return true;
}

//	Set vertex blend flag
bool A3DDevice::SetVertexBlendFlag(A3DVERTEXBLENDFLAGS Flag)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(SetDeviceRenderState(D3DRS_VERTEXBLEND, (D3DVERTEXBLENDFLAGS)Flag)))
	{
		g_A3DErrLog.Log("A3DDevice::SetVertexBlendFlag, failed to set vertex blend flag"); 
		return false;
	}

	return true;
}

//	Clear vertex stream source
bool A3DDevice::ClearStreamSource(int iStream)
{
	if (m_bHWIDevice) 
		return true;

	m_pD3DDevice->SetStreamSource(iStream, NULL, 0, 0);
	return true;
}

//	Set pixel shader constants
bool A3DDevice::SetPixelShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(raw_SetPixelShaderConstantF(dwStart, (const float*)pData, dwNumConst)))
	{
		g_A3DErrLog.Log("A3DDevice::SetPixelShaderConstants, failed to set pixel shader constants"); 
		return false;
	}

	return true;
}

//	Set vertex shader constants
bool A3DDevice::SetVertexShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst)
{
	if (m_bHWIDevice) 
		return true;

	ASSERT(dwNumConst <= 256);

	if (FAILED(raw_SetVertexShaderConstantF(dwStart, (const float*)pData, dwNumConst)))
	{
		g_A3DErrLog.Log("A3DDevice::SetVertexShaderConstants, failed to set vertex shader constants"); 
		return false;
	}

	return true;
}

/*	Test current pixel shader version with specified version

	Return true if current version >= specified version, otherwise return false.
*/
bool A3DDevice::TestPixelShaderVersion(int iMajor, int iMinor)
{
	return m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(iMajor, iMinor) ? true : false;
}

/*	Test current vertex shader version with specified version

	Return true if current version >= specified version, otherwise return false.
*/
bool A3DDevice::TestVertexShaderVersion(int iMajor, int iMinor)
{
	return m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(iMajor, iMinor) ? true : false;
}

//	Clear current vertex shader
bool A3DDevice::ClearVertexShader()
{
	A3DVertexShader::m_pCurShader = NULL;
	return SUCCEEDED(raw_SetVertexShader(NULL));
	//SetHLSLShader(NULL);
	//return m_pD3DDevice->SetVertexShader(NULL) == D3D_OK;
}

//	Clear current pixel shader
bool A3DDevice::ClearPixelShader()
{
	A3DPixelShader::m_pCurShader = NULL;
	return SUCCEEDED(raw_SetPixelShader(NULL));
	//SetHLSLShader(NULL);
	//return m_pD3DDevice->SetPixelShader(NULL) == D3D_OK;
}

void A3DDevice::EnumDisplayModes(IDirect3D9 * pD3D, A3DFORMAT fmtTarget, DISPLAYMODEARRAY& displaymodes)
{
	displaymodes.RemoveAll();

	int nNumDisplayModes = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, (D3DFORMAT)fmtTarget);

	D3DDISPLAYMODE mode;
	for(int i=0; i<nNumDisplayModes; i++)
	{
		if( D3D_OK != pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, (D3DFORMAT)fmtTarget, i, &mode) )
			break;

		// first filter unregular display modes
		if( mode.Width < 640 )
		{
		}
		if( mode.Format == (D3DFORMAT) fmtTarget || fmtTarget == A3DFMT_UNKNOWN )
		{
			int j;
			// first search if there is this display mode already
			for(j=0; j<displaymodes.GetSize(); j++)
			{
				D3DDISPLAYMODE thisMode = displaymodes[j];
				if( mode.Width == thisMode.Width && mode.Height == thisMode.Height )
					break;
			}

			if( j == displaymodes.GetSize() )
			{
				mode.RefreshRate = 0;
				displaymodes.Add(mode);
			}
		}
	}
}

const D3DDISPLAYMODE& A3DDevice::GetDisplayMode(int nIndex)
{
	if( nIndex >= m_DisplayModes.GetSize() )
		return m_DisplayModes[0];
	else
		return m_DisplayModes[nIndex];
}

bool A3DDevice::SetAutoResetFlag(bool bFlag)
{
	m_bAutoReset = bFlag;
	return true;
}

bool A3DDevice::SetNeedResetFlag(bool bFlag)
{
	m_bNeedReset = bFlag;
	return true;
}

bool A3DDevice::SetDeviceRenderState(DWORD type, DWORD value)
{
	return raw_SetRenderState((D3DRENDERSTATETYPE)type, value) == D3D_OK;
}

DWORD A3DDevice::GetDeviceRenderState(DWORD type)
{
	return m_pRSCache->m_RSMap[(D3DRENDERSTATETYPE)type];
}

bool A3DDevice::SetDeviceTextureStageState(int layer, DWORD type, DWORD value)
{
	if (layer >= 0 && layer < MAX_TEX_LAYERS)
		return raw_SetTextureStageState((DWORD)layer, (D3DTEXTURESTAGESTATETYPE)type, value) == D3D_OK;
	else
	{
		ASSERT(layer >= 0 && layer < MAX_TEX_LAYERS);
		return false;
	}
}

DWORD A3DDevice::GetDeviceTextureStageState(int layer, DWORD type)
{
	ASSERT(layer >= 0 && layer < MAX_TEX_LAYERS);
	return m_pRSCache->m_aTexStages[layer].TSMap[(D3DTEXTURESTAGESTATETYPE)type];
}

bool A3DDevice::SetFVF(DWORD dwHandle)
{
	raw_SetFVF(dwHandle);
	return true;
}

bool A3DDevice::GetCanResetFlag()
{
	HRESULT hval;
	hval = m_pD3DDevice->TestCooperativeLevel();

	if (hval != D3DERR_DEVICENOTRESET && hval != D3D_OK)
		return false;

	return true;
}

DWORD A3DDevice::BitsOfDeviceFormat(const A3DFORMAT eFmt)
{
	switch(eFmt)
	{
	case D3DFMT_DXT1:
		//nPitch = desc.Width >> 1;
		return 4;

	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_L8:
	case D3DFMT_P8:
		//nPitch = desc.Width;
		return 8;

	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_R16F:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_A8L8:
		//nPitch = desc.Width << 1;
		return 16;

	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_G16R16:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
	case D3DFMT_D24S8:
	case D3DFMT_D32:
	case D3DFMT_D24X8:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D24FS8:
	case D3DFMT_D32_LOCKABLE:
		//nPitch = desc.Width << 2;
		return 32;

	case D3DFMT_A16B16G16R16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		//nPitch = desc.Width * 8;
		return 64;

	case D3DFMT_A32B32G32R32F:
		//nPitch = desc.Width * 16;
		return 128;
	default:
		ASSERT(0);
		return 0;
	}
}


bool A3DDevice::CreateIntZRenderTarget(int nWidth, int nHeight)
{
	A3DRenderTarget::RTFMT fmtIntZ;
	memset(&fmtIntZ, 0, sizeof(A3DRenderTarget::RTFMT));
	fmtIntZ.iWidth = nWidth;
	fmtIntZ.iHeight = nHeight;
	fmtIntZ.fmtTarget = (A3DFORMAT)D3DFMT_UNKNOWN;
	fmtIntZ.fmtDepth = (A3DFORMAT)A3DFMT_INTZ;
	m_pIntZRT = new A3DRenderTarget();
	if(!m_pIntZRT->Init(this, fmtIntZ, false, true))
	{
		g_A3DErrLog.Log("Can not create IntZ RenderTarget!");
		A3DRELEASE(m_pIntZRT);
		return false;
	}
	else
		return true;
}

bool A3DDevice::SetClipPlaneProgrammable(DWORD Index, const D3DXPLANE* pClipPlaneInWorld, const A3DMATRIX4* matVP)
{
	A3DMATRIX4 matVP_IT = a3d_InverseTM(*matVP);
	matVP_IT.Transpose();
	D3DXPLANE clipPlaneInClip;
	D3DXPlaneTransform(&clipPlaneInClip, pClipPlaneInWorld,(const D3DXMATRIX*) &matVP_IT);
	return SetClipPlane(Index, (float*)&clipPlaneInClip) == D3D_OK;
}
bool A3DDevice::GetRenderTargetData(IDirect3DSurface9* pRenderTargetSurface, IDirect3DSurface9* pDestSurface)
{
	return m_pD3DDevice->GetRenderTargetData(pRenderTargetSurface, pDestSurface) == S_OK;
}

bool A3DDevice::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, 
							  IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestinationPoint)
{
	return m_pD3DDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestinationPoint) == S_OK;
}

IDirect3DSurface9* A3DDevice::CreateSystemMemSurface(int iWidth, int iHeight, A3DFORMAT fmt)
{
	IDirect3DSurface9* pSurface = NULL;
	if (m_pD3DDevice->CreateOffscreenPlainSurface(iWidth, iHeight, (D3DFORMAT)fmt, D3DPOOL_SYSTEMMEM, &pSurface, NULL) != S_OK)
		return NULL;
	return pSurface;
}
bool A3DDevice::StretchRect( IDirect3DSurface9* pSourceSurface , const RECT* pSourceRect,
							IDirect3DSurface9* pDestSurface, const RECT* pDestRect, A3DTEXTUREFILTERTYPE Filter )
{
	return m_pD3DDevice->StretchRect(pSourceSurface, pSourceRect,
		pDestSurface, pDestRect, (D3DTEXTUREFILTERTYPE)Filter) == S_OK;
}

void A3DDevice::EnableInstancing( int Stream, int num )
{
	if(num)
		m_pD3DDevice->SetStreamSourceFreq(Stream,D3DSTREAMSOURCE_INDEXEDDATA|num);
	else	
		m_pD3DDevice->SetStreamSourceFreq(Stream,D3DSTREAMSOURCE_INSTANCEDATA|1);	
}

void A3DDevice::DisableInstancing( int Stream )
{
	m_pD3DDevice->SetStreamSourceFreq(Stream,1);	
}


HRESULT A3DDevice::raw_SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	HRESULT hr = m_pD3DDevice->SetTransform(State, pMatrix);
	if (hr == D3D_OK)
	{
		if (State == D3DTS_VIEW)
			memcpy(&m_pRSCache->m_matView, pMatrix, sizeof (D3DMATRIX));
		else if (State == D3DTS_PROJECTION)
			memcpy(&m_pRSCache->m_matProj, pMatrix, sizeof (D3DMATRIX));
		else if (State >= D3DTS_TEXTURE0 && State <= D3DTS_TEXTURE7)
		{
			int iIndex = State - D3DTS_TEXTURE0;
			memcpy(&m_pRSCache->m_aTexStages[iIndex].matTrans, pMatrix, sizeof (D3DMATRIX));
		}
		else if (State >= D3DTS_WORLDMATRIX(0))
		{
			int iIndex = State - D3DTS_WORLDMATRIX(0);
			ASSERT(iIndex >= 0 && iIndex < A3DDeviceRSCache::NUM_WORLD_MAT);
			memcpy(&m_pRSCache->m_aWorldMats[iIndex], pMatrix, sizeof (D3DMATRIX));
		}
		else
		{
			ASSERT(0);
		}
	}

	return hr;
}

HRESULT A3DDevice::raw_SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
	AssertMTRender();

	HRESULT hr = m_pD3DDevice->SetMaterial(pMaterial);
	if (hr == D3D_OK)
		memcpy(&m_pRSCache->m_Material, pMaterial, sizeof (D3DMATERIAL9));

	return hr;
}

HRESULT A3DDevice::raw_SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{
	AssertMTRender();
	if (Index >= A3DDeviceRSCache::NUM_LIGHT)
		return D3D_OK;

	HRESULT hr = m_pD3DDevice->SetLight(Index, pLight);
	if (hr == D3D_OK)
		memcpy(&m_pRSCache->m_aLights[Index].data, pLight, sizeof (D3DLIGHT9));

	return hr;
}

HRESULT A3DDevice::raw_LightEnable(DWORD Index, BOOL Enable)
{
	AssertMTRender();

	if (Index >= A3DDeviceRSCache::NUM_LIGHT)
		return D3D_OK;

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_aLights[Index].bEnable == Enable)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->LightEnable(Index, Enable);
	if (hr == D3D_OK)
		m_pRSCache->m_aLights[Index].bEnable = Enable;

	return hr;
}

HRESULT A3DDevice::raw_SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	AssertMTRender();

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_RSMap[State] == Value)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetRenderState(State, Value);
	if (hr == D3D_OK)
		m_pRSCache->m_RSMap[State] = Value;

	return hr;
}

HRESULT A3DDevice::raw_SetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
{
	AssertMTRender();

	ASSERT( (Stage >= 0 && Stage < MAX_SAMPLE_LAYERS) || (Stage >= MIN_VS_SAMPLE_LAYERS && Stage <= MIN_VS_SAMPLE_LAYERS));

#ifdef ENABLE_STATECACHECHECK
	if(Stage >= 0 && Stage < MAX_SAMPLE_LAYERS)
	{
		if (m_pRSCache->m_aTextures[Stage] == pTexture)
			return D3D_OK;
	}
	else if(Stage >= MIN_VS_SAMPLE_LAYERS && Stage <= MIN_VS_SAMPLE_LAYERS)
	{
		if (m_pRSCache->m_aVsTextures[Stage - MIN_VS_SAMPLE_LAYERS] == pTexture)
			return D3D_OK;
	}
#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetTexture(Stage, pTexture);
	if (hr == D3D_OK)
	{
		if(Stage >= 0 && Stage < MAX_SAMPLE_LAYERS)
			m_pRSCache->m_aTextures[Stage] = pTexture;
		else if(Stage >= MIN_VS_SAMPLE_LAYERS && Stage <= MIN_VS_SAMPLE_LAYERS)
			m_pRSCache->m_aVsTextures[Stage - MIN_VS_SAMPLE_LAYERS] = pTexture;
	}
	return hr;
}

HRESULT A3DDevice::raw_SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	AssertMTRender();

	ASSERT(Stage >= 0 && Stage < MAX_TEX_LAYERS);

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_aTexStages[Stage].TSMap[Type] == Value)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetTextureStageState(Stage, Type, Value);
	if (hr == D3D_OK)
		m_pRSCache->m_aTexStages[Stage].TSMap[Type] = Value;

	return hr;
}

HRESULT A3DDevice::raw_SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	AssertMTRender();

	ASSERT((Sampler >= 0 && Sampler < MAX_SAMPLE_LAYERS) || (Sampler >= MIN_VS_SAMPLE_LAYERS && Sampler <= MIN_VS_SAMPLE_LAYERS));

#ifdef ENABLE_STATECACHECHECK
	if(Sampler >= 0 && Sampler < MAX_SAMPLE_LAYERS)
	{
		if (m_pRSCache->m_aSSMaps[Sampler][Type] == Value)
			return D3D_OK;
	}
	else if(Sampler >= MIN_VS_SAMPLE_LAYERS && Sampler <= MIN_VS_SAMPLE_LAYERS)
	{
		if (m_pRSCache->m_aVsSSMaps[Sampler - MIN_VS_SAMPLE_LAYERS][Type] == Value)
			return D3D_OK;
	}
#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetSamplerState(Sampler, Type, Value);
	if (hr == D3D_OK)
	{	
		if (Sampler >= 0 && Sampler < MAX_SAMPLE_LAYERS)
			m_pRSCache->m_aSSMaps[Sampler][Type] = Value;
		else if(Sampler >= MIN_VS_SAMPLE_LAYERS && Sampler <= MIN_VS_SAMPLE_LAYERS)
			m_pRSCache->m_aVsSSMaps[Sampler - MIN_VS_SAMPLE_LAYERS][Type] = Value;
	}
	return hr;
}

HRESULT A3DDevice::raw_SetNPatchMode(FLOAT NumSegments)
{
	AssertMTRender();

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_fNPatchMode == NumSegments)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetNPatchMode(NumSegments);
	if (hr == D3D_OK)
		m_pRSCache->m_fNPatchMode = NumSegments;

	return hr;
}

HRESULT A3DDevice::raw_SetFVF(DWORD FVF)
{
	//	SetFVF and SetVertexDeclaration will effect each other, it's hard
	//	to trace their current value on device. So, we push data to device
	//	directly without cache checking.
	HRESULT hr = m_pD3DDevice->SetFVF(FVF);
	if (hr == D3D_OK)
		m_pRSCache->m_dwFVF = FVF;

	return hr;
}

HRESULT A3DDevice::raw_SetVertexShader(LPDIRECT3DVERTEXSHADER9 pShader)
{
	AssertMTRender();

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_pVS == pShader)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetVertexShader(pShader);
	if (hr == D3D_OK)
		m_pRSCache->m_pVS = pShader;

	return hr;
}

HRESULT A3DDevice::raw_SetVertexShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetVertexShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetVertexShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetVertexShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetVertexShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetVertexShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetPixelShader(LPDIRECT3DPIXELSHADER9 pShader)
{
	AssertMTRender();

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_pPS == pShader)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetPixelShader(pShader);
	if (hr == D3D_OK)
		m_pRSCache->m_pPS = pShader;

	return hr;
}

HRESULT A3DDevice::raw_SetPixelShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetPixelShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetPixelShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetPixelShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetPixelShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
	AssertMTRender();
	return m_pD3DDevice->SetPixelShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}

HRESULT A3DDevice::raw_SetClipPlane(DWORD Index, CONST FLOAT *pPlane)
{
	AssertMTRender();
	ASSERT(Index >= 0 && Index < MAX_CLIP_PLANES);

	HRESULT hr = m_pD3DDevice->SetClipPlane(Index, pPlane);
	if (hr == D3D_OK)
		m_pRSCache->m_aClipPlanes[Index].Set(pPlane[0], pPlane[1], pPlane[2], pPlane[3]);

	return hr;
}

HRESULT A3DDevice::raw_SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	AssertMTRender();
	ASSERT(pDecl);

	//	SetFVF and SetVertexDeclaration will effect each other, it's hard
	//	to trace their current value on device. So, we push data to device
	//	directly without cache checking.
	HRESULT hr = m_pD3DDevice->SetVertexDeclaration(pDecl);
	if (hr == D3D_OK)
		m_pRSCache->m_pVertexDecl = pDecl;

	return hr;
}

HRESULT A3DDevice::raw_SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	AssertMTRender();

	ASSERT(StreamNumber >= 0 && StreamNumber < A3DDeviceRSCache::NUM_STRMSOURCE);
	A3DDeviceRSCache::STREAMSOURCE& ss = m_pRSCache->m_aStrmSrcs[StreamNumber];

#ifdef ENABLE_STATECACHECHECK

	if (ss.pVB == pStreamData && ss.OffsetInBytes == OffsetInBytes && ss.Stride == Stride)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	if (hr == D3D_OK)
	{
		ss.pVB = pStreamData;
		ss.OffsetInBytes = OffsetInBytes;
		ss.Stride = Stride;
	}

	return hr;
}

HRESULT A3DDevice::raw_SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	AssertMTRender();

#ifdef ENABLE_STATECACHECHECK

	if (m_pRSCache->m_pIndices == pIndexData)
		return D3D_OK;

#endif	//	ENABLE_STATECACHECHECK

	HRESULT hr = m_pD3DDevice->SetIndices(pIndexData);
	if (hr == D3D_OK)
		m_pRSCache->m_pIndices = pIndexData;

	return hr;
}

//multiThread Render --- begin
void A3DDevice::InvalidAllCBRecorder()
{
	for( int i = 0; i < 2; i++)
	{
		m_CBRecorder[i].BeginCommandBuffer();
		m_CBRecorder[i].EndCommandBuffer();
		m_bCBValid[i] = false;
	}
}

void A3DDevice::BeginCBRecord() //在Record之前
{
	_render_record_flag = 1;
	if( A3DMultiThreadRender::IsMultiThreadRender())
	{
		m_pD3DDevice = m_pCurCBRecorder;
		m_pCurCBRecorder->BeginCommandBuffer();
	}
}
void A3DDevice::EndCBRecord()
{
	_render_record_flag = 0;
	if( A3DMultiThreadRender::IsMultiThreadRender())
	{
		m_pCurCBRecorder->EndCommandBuffer();
		SetCurCBRecorderValid(true);
		m_pD3DDevice = m_pRealD3DDevice;

	}
}


void A3DDevice::SwitchCBRecorder() //在CBPlay之前调用
{
	if( A3DMultiThreadRender::IsMultiThreadRender())
	{
		m_nCurRecorderIdx = (m_nCurRecorderIdx + 1)%2;
		m_pCurCBRecorder = &m_CBRecorder[m_nCurRecorderIdx];
	}

}

A3DCBRecorder9* A3DDevice::GetCBRecorder()		
{ 
	m_pCurCBRecorder = &m_CBRecorder[m_nCurRecorderIdx];
	return m_pCurCBRecorder;
}

void A3DDevice::CopyRResRec2CBRec()
{
	m_CBRResRecorder.CopyCB2OtherRecorder(m_pCurCBRecorder);
}

void A3DDevice::Playback(bool bCurRecorder)
{
	int nPreRecorderIdx = (m_nCurRecorderIdx + 1)%2;
	if(bCurRecorder)
		nPreRecorderIdx = m_nCurRecorderIdx;

	if(m_bCBValid[nPreRecorderIdx])
	{
		m_pCBPlayer->Playback(m_pRealD3DDevice, &m_ComBuffer[nPreRecorderIdx]);
		m_bCBValid[nPreRecorderIdx] = false;
	}
}

HRESULT A3DDevice::LockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer, DWORD OffsetToLock, DWORD SizeToLock, void** ppbData, DWORD dwFlags)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();

		return m_pCurCBRecorder->LockVertexBuffer(pVertexBuffer, OffsetToLock, SizeToLock, ppbData, dwFlags);
	}
	else
	{
		return pVertexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, dwFlags);
	}
}

HRESULT A3DDevice::UnlockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();

		return m_pCurCBRecorder->UnlockVertexBuffer(pVertexBuffer);
	}
	else
	{
		return pVertexBuffer->Unlock();
	}
}

HRESULT A3DDevice::LockIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer, DWORD OffsetToLock, DWORD SizeToLock, void** ppbData, DWORD dwFlags)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();
		return m_pCurCBRecorder->LockIndexBuffer(pIndexBuffer, OffsetToLock, SizeToLock, ppbData, dwFlags);
	}
	else
	{
		return pIndexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, dwFlags);
	}
}

HRESULT A3DDevice::UnlockIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();
		return m_pCurCBRecorder->UnlockIndexBuffer(pIndexBuffer);
	}
	else
	{
		return pIndexBuffer->Unlock();
	}

}

void A3DDevice::ReleaseResource(IUnknown* pResource)
{
	if( pResource)
	{
		if (A3DMultiThreadRender::IsMultiThreadRender())
		{
			m_CBRResRecorder.ReleaseResource(pResource);
		}
		else
		{
			pResource->Release();
		}
	}
}

bool A3DDevice::SaveSurfaceToFile(const char* szFileName, IDirect3DSurface9* pSurface, D3DXIMAGE_FILEFORMAT fmt)
{
	if( m_bHWIDevice ) return true;

	if( !pSurface )
		return false;

	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		m_pCurCBRecorder->SaveSurface(szFileName, pSurface, fmt);
		return true;
	}
	else
	{
		return SUCCEEDED(D3DXSaveSurfaceToFileA(szFileName, fmt, pSurface, NULL, NULL));
	}
}

HRESULT A3DDevice::LockTexture(IDirect3DTexture9* pTexture, UINT Level, void** ppbData, DWORD SrcPitchWidth, DWORD SrcPitchHeight, 
							   RECT* pRect, DWORD dwFlags, int* pOutPitch)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();

		HRESULT hr = m_pCurCBRecorder->LockTexture(pTexture, Level, ppbData, SrcPitchWidth, SrcPitchHeight, pRect, dwFlags);
		*pOutPitch = SrcPitchWidth;
		return hr;
	}
	else
	{
		D3DLOCKED_RECT rtLock;
		HRESULT hr = pTexture->LockRect(Level, &rtLock, pRect, dwFlags);
		*pOutPitch = rtLock.Pitch;
		*ppbData = rtLock.pBits;
		return hr;
	}
}

HRESULT A3DDevice::UnlockTexture(IDirect3DTexture9* pTexture, UINT Level)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();

		return m_pCurCBRecorder->UnlockTexture(pTexture, Level);
	}
	else
	{
		return pTexture->UnlockRect(Level);
	}
}

HRESULT A3DDevice::LockSurface(IDirect3DSurface9* pSurface, void** ppbData, DWORD SrcPitchWidth, DWORD SrcPitchHeight, 
                               RECT* pRect, DWORD dwFlags, int* pOutPitch)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();

        HRESULT hr = m_pCurCBRecorder->LockSurface(pSurface, ppbData, SrcPitchWidth, SrcPitchHeight, pRect, dwFlags);
        *pOutPitch = SrcPitchWidth;
        return hr;
    }
    else
    {
        D3DLOCKED_RECT rtLock;
        HRESULT hr = pSurface->LockRect(&rtLock, pRect, dwFlags);
        *pOutPitch = rtLock.Pitch;
        *ppbData = rtLock.pBits;
        return hr;
    }
}

HRESULT A3DDevice::UnlockSurface(IDirect3DSurface9* pSurface)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();
        return m_pCurCBRecorder->UnlockSurface(pSurface);
    }
    else
    {
        return pSurface->UnlockRect();
    }
}

HRESULT A3DDevice::CopySurface(IDirect3DSurface9* pDestSurface, const RECT* pDestRect, IDirect3DSurface9* pSrcSurface, const RECT* pSrcRect, DWORD Filter)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();
        return m_pCurCBRecorder->CopySurface(pDestSurface, pDestRect, pSrcSurface, pSrcRect, Filter);
    }
    else
    {
        return D3DXLoadSurfaceFromSurface(pDestSurface, NULL, pDestRect, pSrcSurface, NULL, pSrcRect, Filter, 0);
    }

}

HRESULT A3DDevice::SwapChainPresent(IDirect3DSwapChain9* pSwapChain,
									const RECT *pSourceRect,
									const RECT *pDestRect,
									HWND hDestWindowOverride,
									const RGNDATA *pDirtyRegion,
									DWORD dwFlags)
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();
		return m_pCurCBRecorder->SwapChainPresent(pSwapChain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	}
	else
	{
		return pSwapChain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	}
}

HRESULT A3DDevice::EvictManagedResources()
{
	if (m_pD3DDevice == m_pCurCBRecorder)
	{
		AssertMTRender();

		return m_pCurCBRecorder->EvictManagedResources();
	}
	else
	{
		unsigned int nNumMem = m_pD3DDevice->GetAvailableTextureMem()/(1024 * 1024);
		g_A3DErrLog.Log("A3DDevice:EvictManagedResources  Before AvailableTextureMem : %dMB", nNumMem);
		HRESULT hr = m_pD3DDevice->EvictManagedResources();
		nNumMem = m_pD3DDevice->GetAvailableTextureMem()/(1024 * 1024);
		g_A3DErrLog.Log("A3DDevice:EvictManagedResources  Afater AvailableTextureMem : %dMB", nNumMem);

		if( hr == D3D_OK)
		{	
			g_A3DErrLog.Log("A3DDevice:EvictManagedResources OK!!");		
		}
		else if( hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			g_A3DErrLog.Log("A3DDevice:EvictManagedResources OutOfVideoMemory!!");
		}
		else // if( hr == D3DERR_COMMAND_UNPARSED) //DXSDK bug ?! undefine D3DERR_COMMAND_UNPARSED
		{
			g_A3DErrLog.Log("A3DDevice:EvictManagedResources Command UnParsed!!");
		}
		return hr;
	}
}

bool A3DDevice::BeginOccQuery(IA3DOccQueryImpl* pOccQuery)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();

        m_pCurCBRecorder->OccQuery(pOccQuery, OCC_CMD_BEGIN, 0);
        return true;
    }
    else
    {
        return pOccQuery->BeginQuery();
    }

}

void A3DDevice::EndOccQuery(IA3DOccQueryImpl* pOccQuery, LONG lID)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();

        m_pCurCBRecorder->OccQuery(pOccQuery, OCC_CMD_END, lID);
    }
    else
    {
        pOccQuery->EndQuery(lID);
    }

}

void A3DDevice::GetOccQueryData(IA3DOccQueryImpl* pOccQuery)
{
    if (m_pD3DDevice == m_pCurCBRecorder)
    {
        AssertMTRender();

        m_pCurCBRecorder->OccQuery(pOccQuery, OCC_CMD_GET, 0);
    }
    else
    {
        pOccQuery->GetData();
    }
}

bool A3DDevice::IsActivatedHLSLVS(A3DHLSLCore* pHLSLCore)
{
	return pHLSLCore->m_pVertexShaderInst->pVShader == m_pRSCache->m_pVS;
}

bool A3DDevice::IsActivatedHLSLPS(A3DHLSLCore* pHLSLCore)
{
	return pHLSLCore->m_pPixelShaderInst->pPShader == m_pRSCache->m_pPS;
}

A3DRenderTarget* A3DDevice::GetIntZRenderTarget()
{
	return m_pIntZRT;
}

//multiThread Render --- end