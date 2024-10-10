#include "A3DRenderTarget.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DRenderTargetMan.h"
#include "A3DEngine.h"
//#include "A3DDeviceRSCache.h"
#include "A3DDDSFile.h"
#include "A3DDeviceCaps.h"

A3DRenderTarget::A3DRenderTarget()
{
	m_pA3DDevice		= NULL;
	m_pColorTexture		= NULL;
	m_pDepthTexture		= NULL;
	m_pA3DStream		= NULL;
	
	m_mst				= A3DMULTISAMPLE_NONE;
	m_msq				= 0;

	m_iCurWidth			= 0;
	m_iCurHeight		= 0;
	m_fmtCurDepth		= A3DFMT_UNKNOWN;
	m_bUseColor			= false;
	m_bUseDepth			= false;
	m_bUsingTexture		= true;
	m_bHasFilled		= false;
	m_bAutoMipMap		= false;
	m_bIsNullRT			= false;
	m_bInitFinished		= false;
	m_dwRentTime		= 0;
	m_idAppied			= -1;
	
	m_dwClassID			= A3D_CID_RENDERTARGET;
}

A3DRenderTarget::~A3DRenderTarget()
{
}

/*	Check if specified color format is supported

	Return value:

		0: not supported
		0x01: can be created as a texture
		0x02: can be created as a render target surface
*/
DWORD A3DRenderTarget::IsColorFormatSupported(A3DDevice* pA3DDevice, A3DFORMAT fmtColor)
{
	DWORD dwRet = 0;

	//	Try if we can create shading-buffer as a render target
	HRESULT hval = pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
								(D3DDEVTYPE)pA3DDevice->GetDevType(), 
								(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
								D3DUSAGE_RENDERTARGET, 
								D3DRTYPE_SURFACE, 
								(D3DFORMAT)fmtColor);
	if (hval == D3D_OK)
		dwRet |= 2;

	//	Then try if we can create shading-buffer as a texture
	hval = pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
								(D3DDEVTYPE)pA3DDevice->GetDevType(), 
								(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
								D3DUSAGE_RENDERTARGET, 
								D3DRTYPE_TEXTURE, 
								(D3DFORMAT)fmtColor);
	if (hval == D3D_OK)
		dwRet |= 1;

	return dwRet;
}

//	Initialize object. This init() create a texture as render target
//	If rtFmt.iWidth == 0 || rtFmt.iHeight == 0, current device size will be used as render target size
bool A3DRenderTarget::Init(A3DDevice* pA3DDevice, const RTFMT& rtFmt, bool bUseColor, bool bUseDepth,
						bool bAutoMipMap/* false */)
{
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return true;

	ASSERT(pA3DDevice);

	m_pA3DDevice	= pA3DDevice;
	m_bUseColor		= bUseColor;
	m_bUseDepth		= bUseDepth;
	m_bAutoMipMap	= bAutoMipMap;
	m_rtFmt			= rtFmt;
	m_bUsingTexture	= true;

	if (!CreatePlains())
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init(), failed to create plains!");
		return false;
	}

	if (m_ColorSurf.m_pD3DSurface)
	{
		//	Now init an a3dstream for PresentToBack() when m_bUsingTexture == true
		m_pA3DStream = new A3DStream();
		if (!m_pA3DStream || !m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
		{
			g_A3DErrLog.Log("A3DRenderTarget::Init() Not enough memory!");
			return false;
		}

		static const WORD index[] = {0, 1, 2, 2, 1, 3};
		m_pA3DStream->SetIndices((void*)index, 6);
	}

	//	Finish initialization
	FinishInit();

	return true;
}

//	Initialize object. This init() create a surface as render target
//	If rtFmt.iWidth == 0 || rtFmt.iHeight == 0, current device size will be used as render target size
bool A3DRenderTarget::Init(A3DDevice* pA3DDevice, const RTFMT &rtFmt, bool bUseColor, bool bUseDepth,
						A3DMULTISAMPLE_TYPE ms, DWORD msq)
{
	m_pA3DDevice	= pA3DDevice;
	m_bUseColor		= bUseColor;
	m_bUseDepth		= bUseDepth;
	m_rtFmt			= rtFmt;
	m_mst			= ms;
	m_msq			= msq;
	m_bUsingTexture	= false;

	if (!CreatePlains())
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init(), failed to create plains!");
		return false;
	}

	//	Finish initialization
	FinishInit();

	return true;
}

//	Initialize object. This init() create a NULL render target that is supported by some hardware
//	This function will return false if A3DDeviceCaps::SupportATINullRT return false;
bool A3DRenderTarget::InitAsNullRT(A3DDevice* pA3DDevice, const RTFMT& rtFmt, A3DMULTISAMPLE_TYPE ms, DWORD msq)
{
	if (!pA3DDevice->GetA3DDeviceCaps()->SupportNullRT())
		return false;

	m_pA3DDevice	= pA3DDevice;
	m_bUseColor		= false;
	m_bUseDepth		= false;
	m_rtFmt			= rtFmt;
	m_mst			= ms;
	m_msq			= msq;
	m_bUsingTexture	= false;
	m_bIsNullRT		= true;

	if (!CreatePlains())
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init(), failed to create NULL RT !");
		return false;
	}

	//	Finish initialization
	FinishInit();

	return true;
}

//	Do some work to finish initialization
void A3DRenderTarget::FinishInit()
{
	m_pA3DDevice->AddUnmanagedDevObject(this);
	m_bInitFinished = true;
}

bool A3DRenderTarget::Release()
{
	if (m_bInitFinished)
	{
		m_pA3DDevice->RemoveUnmanagedDevObject(this);
		m_bInitFinished = false;
	}

	A3DRELEASE(m_pA3DStream);

	ReleasePlains();

	return true;
}

bool A3DRenderTarget::PresentToBack(int x, int y, int iWidth/* 0 */, int iHeight/* 0 */)
{
	if (!m_pA3DDevice) 
		return true;

	//	Only RT texture can present to back buffer
	if (!m_bUsingTexture || !m_ColorSurf.m_pD3DSurface)
	{
		ASSERT(m_bUsingTexture && m_ColorSurf.m_pD3DSurface);
		return false;
	}

	if (!iWidth || !iHeight)
	{
		iWidth = m_iCurWidth;
		iHeight = m_iCurHeight;
	}

	A3DCOLOR color = A3DCOLORRGBA(255, 255, 255, 255);
	A3DCOLOR specular = A3DCOLORRGBA(0, 0, 0, 255);
	A3DTLVERTEX verts[4];
	float l, t, r, b;

	l = (float)x;
	t = (float)y;
	r = l + (float)iWidth;
	b = t + (float)iHeight;
	
	float fTexOffU = 0.5f / iWidth;
	float fTexOffV = 0.5f / iHeight;

	verts[0] = A3DTLVERTEX(A3DVECTOR4(l, t, 0.0f, 1.0f), color, specular, fTexOffU, fTexOffV);
	verts[1] = A3DTLVERTEX(A3DVECTOR4(r, t, 0.0f, 1.0f), color, specular, 1.0f+fTexOffU, fTexOffV);
	verts[2] = A3DTLVERTEX(A3DVECTOR4(l, b, 0.0f, 1.0f), color, specular, fTexOffU, 1.0f+fTexOffV);
	verts[3] = A3DTLVERTEX(A3DVECTOR4(r, b, 0.0f, 1.0f), color, specular, 1.0f+fTexOffU, 1.0f+fTexOffV);

	m_pA3DStream->SetVertsDynamic((LPBYTE)verts, 4);
	m_pA3DStream->Appear();

	//	Set render state
	bool ztest_enable = m_pA3DDevice->GetZTestEnable();
	bool zwrite_enable = m_pA3DDevice->GetZWriteEnable();
	bool fog_enable = m_pA3DDevice->GetFogEnable();
	bool alpha_blend = m_pA3DDevice->GetAlphaBlendEnable();
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->raw_SetTexture(0, m_pColorTexture);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	//	Do render
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	
	//	Restore render states
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetZTestEnable(ztest_enable);
	m_pA3DDevice->SetZWriteEnable(zwrite_enable);
	m_pA3DDevice->SetFogEnable(fog_enable);
	m_pA3DDevice->SetAlphaBlendEnable(alpha_blend);

	return true;
}

bool A3DRenderTarget::ExportColorToFile(char * szFullpath)
{
	if (!m_pA3DDevice || !m_ColorSurf.m_pD3DSurface || m_bIsNullRT)
		return false;

	HRESULT	hval;
	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_ColorSurf.m_pD3DSurface, NULL, NULL);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DRenderTarget::ExportColorToFile(), Can not save color surface to file [%s]", szFullpath);
		return false;
	}

	return true;
}

bool A3DRenderTarget::ExportDepthToFile(char * szFullpath)
{
	if (!m_pA3DDevice || !m_DepthSurf.m_pD3DSurface || m_bIsNullRT)
		return false;

	HRESULT	hval;
	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_DepthSurf.m_pD3DSurface, NULL, NULL);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DRenderTarget::ExportDepthToFile(), Can not save depth surface to file [%s]", szFullpath);
		return false;
	}

	return true;
}

bool A3DRenderTarget::BeforeDeviceReset()
{
	ReleasePlains();
	return true;
}

bool A3DRenderTarget::AfterDeviceReset()
{
	if (!CreatePlains())
	{
		g_A3DErrLog.Log("A3DRenderTarget::AfterDeviceReset(), failed to create plains!");
		return false;
	}

	return true;
}

bool A3DRenderTarget::CreatePlains()
{
	HRESULT	hval;

	m_iCurWidth = m_rtFmt.iWidth;
	m_iCurHeight = m_rtFmt.iHeight;
	m_fmtCurDepth = m_rtFmt.fmtDepth;

	//	Decide target dimension
	const D3DPRESENT_PARAMETERS& d3dpp = m_pA3DDevice->GetD3DPP();
	if (0 == m_iCurWidth || 0 == m_iCurHeight)
	{
		m_iCurWidth = int(d3dpp.BackBufferWidth);
		m_iCurHeight = int(d3dpp.BackBufferHeight);
	}
	else if (-2 == m_iCurWidth || -2 == m_iCurHeight)
	{
		m_iCurWidth = int(d3dpp.BackBufferWidth) / 2;
		m_iCurHeight = int(d3dpp.BackBufferHeight) / 2;
	}
	else if (-4 == m_iCurWidth || -4 == m_iCurHeight)
	{
		m_iCurWidth = int(d3dpp.BackBufferWidth) / 4;
		m_iCurHeight = int(d3dpp.BackBufferHeight) / 4;
	}
	else if (-8 == m_iCurWidth || -8 == m_iCurHeight)
	{
		m_iCurWidth = int(d3dpp.BackBufferWidth) / 8;
		m_iCurHeight = int(d3dpp.BackBufferHeight) / 8;
	}
	else if (m_iCurWidth < 0 || m_iCurHeight < 0)
	{
		ASSERT(0);
		return false;
	}

	if (m_bIsNullRT)
	{
		hval = m_pA3DDevice->GetD3DDevice()->CreateRenderTarget(m_iCurWidth, m_iCurHeight,
					(D3DFORMAT)A3DFMT_NULL, (D3DMULTISAMPLE_TYPE)m_mst, m_msq,
					FALSE, &m_ColorSurf.m_pD3DSurface, NULL);

		if (D3D_OK != hval)
		{
			g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() Failed to create ATI Null RT Fail !");
			return false;
		}
	}
	else
	{
		if (m_bUseColor)
		{
			if (m_bUsingTexture)
			{
				DWORD dwUsages = D3DUSAGE_RENDERTARGET;
				if (m_bAutoMipMap)
					dwUsages |= D3DUSAGE_AUTOGENMIPMAP;

				hval = m_pA3DDevice->GetD3DDevice()->CreateTexture(m_iCurWidth, m_iCurHeight, 
						1, dwUsages, (D3DFORMAT)m_rtFmt.fmtTarget,
						D3DPOOL_DEFAULT, &m_pColorTexture, NULL);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateTexture[%dx%dx%d] Fail, error=%x!", m_iCurWidth, m_iCurHeight, m_rtFmt.fmtTarget, hval);
					return false;
				}

				hval = m_pColorTexture->GetSurfaceLevel(0, &m_ColorSurf.m_pD3DSurface);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() GetSurfaceLevel Fail, error=%x!",hval);
					return false;
				}
			}
			else
			{
				hval = m_pA3DDevice->GetD3DDevice()->CreateRenderTarget(m_iCurWidth, m_iCurHeight,
					(D3DFORMAT)m_rtFmt.fmtTarget, (D3DMULTISAMPLE_TYPE)m_mst, m_msq,
					FALSE, &m_ColorSurf.m_pD3DSurface, NULL);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateRenderTarget Fail error=%x!", hval);
					return false;
				}
			}
		}

		if (m_bUseDepth)
		{
			//	Decide target's depth format
			if (m_fmtCurDepth == A3DFMT_UNKNOWN)
				m_fmtCurDepth = m_pA3DDevice->GetDepthFormat();

			bool bUsingTexture = m_bUsingTexture;

			if (bUsingTexture)
			{
				//	Check if texture support current depth-stencil format
				hval = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
					(D3DDEVTYPE)m_pA3DDevice->GetDevType(), 
					(D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter, 
					D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, (D3DFORMAT)m_fmtCurDepth);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains(), CheckDeviceFormat fail %dx%d, %s !",
									m_iCurWidth, m_iCurHeight, m_pA3DDevice->GetFormatString(m_fmtCurDepth));
					bUsingTexture = false;
				}
			}

			if (bUsingTexture)
			{
				hval = m_pA3DDevice->GetD3DDevice()->CreateTexture(m_iCurWidth, m_iCurHeight, 
					1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)m_fmtCurDepth,
					D3DPOOL_DEFAULT, &m_pDepthTexture, NULL);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateDepth Fail error=%x!", hval);
					return false;
				}

				hval = m_pDepthTexture->GetSurfaceLevel(0, &m_DepthSurf.m_pD3DSurface);
				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() GetSurfaceLevel Fail!");
					return false;
				}
			}
			else
			{
				hval = m_pA3DDevice->GetD3DDevice()->CreateDepthStencilSurface(m_iCurWidth, m_iCurHeight,
					(D3DFORMAT)m_fmtCurDepth, D3DMULTISAMPLE_NONE, 0, 
					FALSE, &m_DepthSurf.m_pD3DSurface, NULL);

				if (D3D_OK != hval)
				{
					g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateDepth Fail by CreateDepthStencilSurface error=%x!", hval);
					return false;
				}
			}
		}
	}

	m_bHasFilled = false;


	m_pA3DDevice->GetRenderTargetMan()->IncRenderTargetSize(GetRenderTargetSize());

	return true;
}

bool A3DRenderTarget::ReleasePlains()
{

	//	Appied target shouldn't be released
	if (m_idAppied != -1)
	{
		ASSERT(m_idAppied == -1);
		return false;
	}

	m_pA3DDevice->GetRenderTargetMan()->IncRenderTargetSize(-GetRenderTargetSize());

	if (m_pA3DDevice)
	{
		if(m_pA3DDevice->GetNeedResetFlag())
		{

			A3DINTERFACERELEASE(m_ColorSurf.m_pD3DSurface);
			A3DINTERFACERELEASE(m_DepthSurf.m_pD3DSurface);
			A3DINTERFACERELEASE(m_pColorTexture);
			A3DINTERFACERELEASE(m_pDepthTexture);

		}
		else
		{
			//multithread render
			if( m_ColorSurf.m_pD3DSurface)
			{
				m_pA3DDevice->ReleaseResource(m_ColorSurf.m_pD3DSurface);
				m_ColorSurf.m_pD3DSurface = NULL;
			}
			if( m_DepthSurf.m_pD3DSurface)
			{
				m_pA3DDevice->ReleaseResource(m_DepthSurf.m_pD3DSurface);
				m_DepthSurf.m_pD3DSurface = NULL;
			}
			if( m_pColorTexture)
			{
				m_pA3DDevice->ReleaseResource(m_pColorTexture);
				m_pColorTexture = NULL;
			}
			if( m_pDepthTexture)
			{
				m_pA3DDevice->ReleaseResource(m_pDepthTexture);
				m_pDepthTexture = NULL;
			}
		}
	}

	return true;
}

bool A3DRenderTarget::AppearAsTexture(int nLayer,bool bDepthBuffer)
{
	if(!m_pA3DDevice)
		return false;

	if(bDepthBuffer && !m_pDepthTexture)
		return false;

	if( bDepthBuffer && m_pDepthTexture)	
	{
		m_pA3DDevice->raw_SetTexture(nLayer, m_pDepthTexture);
		return true;
	}
	
	if (!m_pColorTexture)
		return false;

	m_pA3DDevice->raw_SetTexture(nLayer, m_pColorTexture);
	return true;

	
	
}

bool A3DRenderTarget::DisappearAsTexture(int nLayer)
{
	if (!m_pA3DDevice || !m_pColorTexture)
		return false;

	m_pA3DDevice->raw_SetTexture(nLayer, NULL);
	return true;
}

//	Apply render target to device
bool A3DRenderTarget::ApplyToDevice(int iRTIndex/* 0 */)
{
	//	Apply twice ?
	if (m_idAppied != -1)
	{
		ASSERT(m_idAppied == -1);
		return false;
	}

	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
	if (!pRTMan->PushRenderTarget(iRTIndex, this))
		return false;
	
	int iStackDepth = pRTMan->GetRTStackDepth(iRTIndex);
	m_idAppied = (iRTIndex << 16) | iStackDepth;

	return true;
}

//	Withdraw render target from device
void A3DRenderTarget::WithdrawFromDevice()
{
	if (m_idAppied != -1)
	{
		A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
		int iStackDepth = m_idAppied & 0xffff;
		int iRTIndex = m_idAppied >> 16;

		if (pRTMan->GetRTStackDepth(iRTIndex) != iStackDepth)
		{
			ASSERT(0 && "Render target stack wrong !");
			return;
		}

		pRTMan->PopRenderTarget(iRTIndex);
		m_idAppied = -1;
	}
	else
	{
		ASSERT(m_idAppied != -1);
	}
}

int A3DRenderTarget::GetRenderTargetSize()
{
	if (m_bIsNullRT)
		return 0;

	int nSize = 0;
	if (m_pColorTexture != NULL)
	{
		nSize += ((A3DDDSFile::BitsPerPixel((D3DFORMAT)(m_rtFmt.fmtTarget)) *
			m_iCurWidth) / 8) * m_iCurHeight;
	}

	if (m_pDepthTexture != NULL)
	{
		nSize += ((A3DDDSFile::BitsPerPixel((D3DFORMAT)m_fmtCurDepth) *
			m_iCurWidth) / 8) * m_iCurHeight;
	}

	return nSize;
}