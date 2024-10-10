/*
* FILE: A3DDeviceCaps.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: Liyi, 2012/2/29
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/

#include "A3DDeviceCaps.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DRenderTarget.h"
#include "A3DConfig.h"

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DDeviceCaps
//	
///////////////////////////////////////////////////////////////////////////

A3DDeviceCaps::A3DDeviceCaps() :
m_pA3DDevice(NULL),
m_bSupRT_R16F(false),
m_bSupRT_R32F(false),
m_bSupRT_GR16F(false),
m_bSupRT_GR32F(false),
m_bSupRT_ABGR16F(false),
m_bSupRT_ABGR32F(false),
m_bSupHWShadowPCF(false),
m_bSupInstances(false),
m_bSupAnisoFilter(false),
m_bSupMinAnisoFilter(false),
m_bSupMagAnisoFilter(false),
m_bSupsRGBRead(false),
m_bSupNullRT(false),
m_bSupLPPGBuf(false),
m_typeDDBA(DDBA_NONE),
m_bSupMRTs(false),
m_bSupDiffBitMRT(false),
m_bSupSimuBloom(false),
m_bSupStreamOffset(false)
{
}

A3DDeviceCaps::~A3DDeviceCaps()
{
}

//	Check caps
void A3DDeviceCaps::CheckCaps(A3DDevice* pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	IDirect3D9* pD3D = m_pA3DDevice->GetA3DEngine()->GetD3D();

	//	Check formats
	DWORD dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_A16B16G16R16F);
	m_bSupRT_ABGR16F = (dwMask & 0x01) ? true : false;
	dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_A32B32G32R32F);
	m_bSupRT_ABGR32F = (dwMask & 0x01) ? true : false;
	dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_G16R16F);
	m_bSupRT_GR16F = (dwMask & 0x01) ? true : false;
	dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_G32R32F);
	m_bSupRT_GR32F = (dwMask & 0x01) ? true : false;
	dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_R16F);
	m_bSupRT_R16F = (dwMask & 0x01) ? true : false;
	dwMask = A3DRenderTarget::IsColorFormatSupported(pA3DDevice, A3DFMT_R32F);
	m_bSupRT_R32F = (dwMask & 0x01) ? true : false;

	m_bSupLPPGBuf = m_bSupRT_ABGR16F;

	HRESULT hr;

	//	Check if hardware shadow map is supportted or not
	hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
		(D3DDEVTYPE)pA3DDevice->GetDevType(), 
		(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
		D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_TEXTURE, 
		D3DFMT_D24S8);

	m_bSupHWShadowPCF = (hr == D3D_OK);
	

	//	Check if dynamic bone blend is supported
	m_bSupInstances = false;
	if (m_pA3DDevice->TestPixelShaderVersion(3, 0) &&
		m_pA3DDevice->TestVertexShaderVersion(3, 0))
	{
		m_bSupInstances = true;
	}

	//	Support anisotropic texture filtering ?
	m_bSupAnisoFilter = m_pA3DDevice->GetD3DCaps().MaxAnisotropy > 1;
	if (m_bSupAnisoFilter)
	{
		DWORD dwCaps = m_pA3DDevice->GetD3DCaps().TextureFilterCaps;
		m_bSupMinAnisoFilter = (dwCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) ? true : false;
		m_bSupMagAnisoFilter = (dwCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) ? true : false;
	}
	else
	{
		m_bSupMinAnisoFilter = false;
		m_bSupMagAnisoFilter = false;
	}

	//	Support sRGB reading for texture ?
	hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
		(D3DDEVTYPE)pA3DDevice->GetDevType(), 
		(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
		D3DUSAGE_QUERY_SRGBREAD, 
		D3DRTYPE_TEXTURE,
		D3DFMT_DXT5);

	m_bSupsRGBRead = (hr == D3D_OK);

	//	Support NULL render target ?
	hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
		(D3DDEVTYPE)pA3DDevice->GetDevType(), 
		(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_SURFACE,
		(D3DFORMAT)A3DFMT_NULL);

	m_bSupNullRT = (hr == D3D_OK);

	//	Check which DDBA type is supported
	m_typeDDBA = DDBA_NONE;
	hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
		(D3DDEVTYPE)pA3DDevice->GetDevType(), 
		(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
		D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_TEXTURE,
		(D3DFORMAT)A3DFMT_INTZ);
	if (hr == D3D_OK)
		m_typeDDBA = DDBA_INTZ;
	else
	{
		hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
			(D3DDEVTYPE)pA3DDevice->GetDevType(), 
			(D3DFORMAT)pA3DDevice->GetDevFormat().fmtAdapter, 
			D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_TEXTURE,
			(D3DFORMAT)A3DFMT_RAWZ);
		if (hr == D3D_OK)
			m_typeDDBA = DDBA_RAWZ;
	}

	//	MRT support
	m_bSupMRTs = m_pA3DDevice->GetD3DCaps().NumSimultaneousRTs > 1;
	if (m_bSupMRTs)
	{
		DWORD dwPrimMiscCaps = m_pA3DDevice->GetD3DCaps().PrimitiveMiscCaps;
		m_bSupDiffBitMRT = (dwPrimMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) ? true : false;
		m_bSupSimuBloom = m_bSupDiffBitMRT && ((dwPrimMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0);
		//	m_bSupSimuBloom = false;
	}
	else
	{
		m_bSupDiffBitMRT = false;
		m_bSupSimuBloom = false;
	}

	//	Stream offset
	m_bSupStreamOffset = false;
	if (m_pA3DDevice->GetD3DCaps().DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)
		m_bSupStreamOffset = true;
}

