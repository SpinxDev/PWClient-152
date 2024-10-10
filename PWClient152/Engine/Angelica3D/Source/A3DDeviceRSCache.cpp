/*
 * FILE: A3DDeviceRSCache.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/9/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "A3DDeviceRSCache.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DRenderTarget.h"
#include "A3DConfig.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DDeviceRSCache
//	
///////////////////////////////////////////////////////////////////////////

A3DDeviceRSCache::A3DDeviceRSCache()
{
	m_pA3DDevice = NULL;
}

A3DDeviceRSCache::~A3DDeviceRSCache()
{
}

//	Reset state cache
bool A3DDeviceRSCache::Reset(A3DDevice* pA3DDevice)
{
	m_pA3DDevice	= pA3DDevice;
	m_fNPatchMode	= 0.0f;
	m_dwFVF			= 0;
	m_pVS			= NULL;
	m_pPS			= NULL;
	m_pVertexDecl	= NULL;
	m_pIndices		= NULL;

	m_matView.Identity();
	m_matProj.Identity();

	memset(&m_Material, 0, sizeof (m_Material));
	memset(m_aClipPlanes, 0, sizeof (m_aClipPlanes));
	memset(m_aTextures, 0, sizeof (m_aTextures));
	memset(m_aVsTextures, 0, sizeof (m_aVsTextures));
	memset(&m_aStrmSrcs, 0, sizeof (m_aStrmSrcs));

	int i;

	//	Initialize world matrices
	for (i=0; i < NUM_WORLD_MAT; i++)
		m_aWorldMats[i].Identity();
	
	//	For light data, we must give a correct default value, the value may be
	//	applied to device in A3DEffectStateMan::RestoreStates(). If bad
	//	light value is set, some rendering artifacts may occur.
	for (i=0; i < NUM_LIGHT; i++)
	{
		LIGHTDATA& light = m_aLights[i];
		memset(&light, 0, sizeof (light));

		light.data.Type	= D3DLIGHT_DIRECTIONAL;
		light.data.Diffuse.r = 1.0f;
		light.data.Diffuse.g = 1.0f;
		light.data.Diffuse.b = 1.0f;
		light.data.Direction.x = 0.0f;
		light.data.Direction.y = 0.0f;
		light.data.Direction.z = 1.0f;
	}

	//	Initialize texture stage stage
	for (i=0; i < MAX_TEX_LAYERS; i++)
	{
		TEXTUREDATA& tex = m_aTexStages[i];

		tex.matTrans.Identity();

		tex.TSMap[D3DTSS_COLOROP] = D3DTOP_DISABLE;
		tex.TSMap[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
		tex.TSMap[D3DTSS_COLORARG2] = D3DTA_CURRENT;
		tex.TSMap[D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
		tex.TSMap[D3DTSS_ALPHAARG1]	= D3DTA_TEXTURE;
		tex.TSMap[D3DTSS_ALPHAARG2]	= D3DTA_CURRENT;
		tex.TSMap[D3DTSS_BUMPENVMAT00] = 0;
		tex.TSMap[D3DTSS_BUMPENVMAT01] = 0;
		tex.TSMap[D3DTSS_BUMPENVMAT10] = 0;
		tex.TSMap[D3DTSS_BUMPENVMAT11] = 0;
		tex.TSMap[D3DTSS_TEXCOORDINDEX] = i;
		tex.TSMap[D3DTSS_BUMPENVLSCALE]	= 0;
		tex.TSMap[D3DTSS_BUMPENVLOFFSET] = 0;
		tex.TSMap[D3DTSS_TEXTURETRANSFORMFLAGS] = D3DTTFF_DISABLE;
		tex.TSMap[D3DTSS_COLORARG0]	= D3DTA_CURRENT;
		tex.TSMap[D3DTSS_ALPHAARG0]	= D3DTA_CURRENT;
		tex.TSMap[D3DTSS_RESULTARG]	= D3DTA_CURRENT;
		tex.TSMap[D3DTSS_CONSTANT] = D3DTA_CONSTANT;
	}

	//	Initialize sampler stage state
	for (i=0; i < MAX_SAMPLE_LAYERS; i++)
	{
		SamplerStateMap& SSMap = m_aSSMaps[i];

		SSMap[D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_BORDERCOLOR] = 0;
		SSMap[D3DSAMP_MAGFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MINFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MIPFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MIPMAPLODBIAS] = 0;
		SSMap[D3DSAMP_MAXMIPLEVEL] = 0;
		SSMap[D3DSAMP_MAXANISOTROPY] = 1;
		SSMap[D3DSAMP_SRGBTEXTURE] = 0;
		SSMap[D3DSAMP_ELEMENTINDEX] = 0;
		SSMap[D3DSAMP_DMAPOFFSET] = 0;
	}

	for(i = 0; i < VS_SAMPLE_LAYERS_NUM; i++)
	{
		SamplerStateMap& SSMap = m_aVsSSMaps[i];

		SSMap[D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
		SSMap[D3DSAMP_BORDERCOLOR] = 0;
		SSMap[D3DSAMP_MAGFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MINFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MIPFILTER] = D3DTEXF_LINEAR;	//	Different to D3D's default
		SSMap[D3DSAMP_MIPMAPLODBIAS] = 0;
		SSMap[D3DSAMP_MAXMIPLEVEL] = 0;
		SSMap[D3DSAMP_MAXANISOTROPY] = 1;
		SSMap[D3DSAMP_SRGBTEXTURE] = 0;
		SSMap[D3DSAMP_ELEMENTINDEX] = 0;
		SSMap[D3DSAMP_DMAPOFFSET] = 0;
	}

	const float f1 = 1.0f;
	const float f64 = 64.0f;

	//	Initialize render state
	m_RSMap[D3DRS_ZENABLE] = D3DZB_TRUE;
	m_RSMap[D3DRS_FILLMODE] = D3DFILL_SOLID;
	m_RSMap[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	m_RSMap[D3DRS_ZWRITEENABLE] = TRUE;
	m_RSMap[D3DRS_ALPHATESTENABLE] = FALSE;
	m_RSMap[D3DRS_LASTPIXEL] = TRUE;
	m_RSMap[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	m_RSMap[D3DRS_DESTBLEND] = D3DBLEND_ZERO;
	m_RSMap[D3DRS_CULLMODE] = D3DCULL_CCW;
	m_RSMap[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
	m_RSMap[D3DRS_ALPHAREF] = 0;
	m_RSMap[D3DRS_ALPHAFUNC] = D3DCMP_ALWAYS;
	m_RSMap[D3DRS_DITHERENABLE] = FALSE;
	m_RSMap[D3DRS_ALPHABLENDENABLE] = FALSE;
	m_RSMap[D3DRS_FOGENABLE] = FALSE;
	m_RSMap[D3DRS_SPECULARENABLE] = FALSE;
	m_RSMap[D3DRS_FOGCOLOR] = 0;
	m_RSMap[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
	m_RSMap[D3DRS_FOGSTART] = 0;
	m_RSMap[D3DRS_FOGEND] = *((DWORD*)&f1);
	m_RSMap[D3DRS_FOGDENSITY] = *((DWORD*)&f1);
	m_RSMap[D3DRS_RANGEFOGENABLE] = FALSE;
	m_RSMap[D3DRS_STENCILENABLE] = FALSE;
	m_RSMap[D3DRS_STENCILFAIL] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_STENCILZFAIL] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
	m_RSMap[D3DRS_STENCILREF] = 0;
	m_RSMap[D3DRS_STENCILMASK] = 0xFFFFFFFF;
	m_RSMap[D3DRS_STENCILWRITEMASK] = 0xFFFFFFFF;
	m_RSMap[D3DRS_TEXTUREFACTOR] = 0xFFFFFFFF;
	m_RSMap[D3DRS_WRAP0] = 0;
	m_RSMap[D3DRS_WRAP1] = 0;
	m_RSMap[D3DRS_WRAP2] = 0;
	m_RSMap[D3DRS_WRAP3] = 0;
	m_RSMap[D3DRS_WRAP4] = 0;
	m_RSMap[D3DRS_WRAP5] = 0;
	m_RSMap[D3DRS_WRAP6] = 0;
	m_RSMap[D3DRS_WRAP7] = 0;
	m_RSMap[D3DRS_CLIPPING] = TRUE;
	m_RSMap[D3DRS_LIGHTING] = TRUE;
	m_RSMap[D3DRS_AMBIENT] = 0;
	m_RSMap[D3DRS_FOGVERTEXMODE] = D3DFOG_NONE;
	m_RSMap[D3DRS_COLORVERTEX] = TRUE;
	m_RSMap[D3DRS_LOCALVIEWER] = TRUE;
	m_RSMap[D3DRS_NORMALIZENORMALS] = FALSE;
	m_RSMap[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
	m_RSMap[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
	m_RSMap[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_MATERIAL;
	m_RSMap[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
	m_RSMap[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
	m_RSMap[D3DRS_CLIPPLANEENABLE] = 0;
	m_RSMap[D3DRS_POINTSIZE] = *((DWORD*)&f64);
	m_RSMap[D3DRS_POINTSIZE_MIN] = *((DWORD*)&f1);
	m_RSMap[D3DRS_POINTSPRITEENABLE] = FALSE;
	m_RSMap[D3DRS_POINTSCALEENABLE] = FALSE;
	m_RSMap[D3DRS_POINTSCALE_A] = *((DWORD*)&f1);
	m_RSMap[D3DRS_POINTSCALE_B] = 0;
	m_RSMap[D3DRS_POINTSCALE_C] = 0;
	m_RSMap[D3DRS_MULTISAMPLEANTIALIAS] = TRUE;
	m_RSMap[D3DRS_MULTISAMPLEMASK] = 0xFFFFFFFF;
	m_RSMap[D3DRS_PATCHEDGESTYLE] = D3DPATCHEDGE_DISCRETE;
	m_RSMap[D3DRS_DEBUGMONITORTOKEN] = D3DDMT_ENABLE;
	m_RSMap[D3DRS_POINTSIZE_MAX] = *((DWORD*)&f64);
	m_RSMap[D3DRS_INDEXEDVERTEXBLENDENABLE] = FALSE;
	m_RSMap[D3DRS_COLORWRITEENABLE] = 0x0000000F;
	m_RSMap[D3DRS_TWEENFACTOR] = 0;
	m_RSMap[D3DRS_BLENDOP] = D3DBLENDOP_ADD;
	m_RSMap[D3DRS_POSITIONDEGREE] = D3DDEGREE_CUBIC;
	m_RSMap[D3DRS_NORMALDEGREE] = D3DDEGREE_LINEAR;
	m_RSMap[D3DRS_SCISSORTESTENABLE] = FALSE;
	m_RSMap[D3DRS_SLOPESCALEDEPTHBIAS] = 0;
	m_RSMap[D3DRS_ANTIALIASEDLINEENABLE] = FALSE;
	m_RSMap[D3DRS_MINTESSELLATIONLEVEL] = *((DWORD*)&f1);
	m_RSMap[D3DRS_MAXTESSELLATIONLEVEL] = *((DWORD*)&f1);
	m_RSMap[D3DRS_ADAPTIVETESS_X] = 0;
	m_RSMap[D3DRS_ADAPTIVETESS_Y] = 0;
	m_RSMap[D3DRS_ADAPTIVETESS_Z] = *((DWORD*)&f1);
	m_RSMap[D3DRS_ADAPTIVETESS_W] = 0;
	m_RSMap[D3DRS_ENABLEADAPTIVETESSELLATION] = FALSE;
	m_RSMap[D3DRS_TWOSIDEDSTENCILMODE] = FALSE;
	m_RSMap[D3DRS_CCW_STENCILFAIL] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_CCW_STENCILZFAIL] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_CCW_STENCILPASS] = D3DSTENCILOP_KEEP;
	m_RSMap[D3DRS_CCW_STENCILFUNC] = D3DCMP_ALWAYS;
	m_RSMap[D3DRS_COLORWRITEENABLE1] = 0x0000000F;
	m_RSMap[D3DRS_COLORWRITEENABLE2] = 0x0000000F;
	m_RSMap[D3DRS_COLORWRITEENABLE3] = 0x0000000F;
	m_RSMap[D3DRS_BLENDFACTOR] = 0xFFFFFFFF;
	m_RSMap[D3DRS_SRGBWRITEENABLE] = 0;
	m_RSMap[D3DRS_DEPTHBIAS] = 0;
	m_RSMap[D3DRS_WRAP8] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP9] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP10] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP11] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP12] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP13] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP14] = D3DRS_WRAP0;
	m_RSMap[D3DRS_WRAP15] = D3DRS_WRAP0;
	m_RSMap[D3DRS_SEPARATEALPHABLENDENABLE] = FALSE;
	m_RSMap[D3DRS_SRCBLENDALPHA] = D3DBLEND_ONE;
	m_RSMap[D3DRS_DESTBLENDALPHA] = D3DBLEND_ZERO;
	m_RSMap[D3DRS_BLENDOPALPHA] = D3DBLENDOP_ADD;

	//	Apply all states to device
	ApplyAllStates();

	return true;
}

//	Apply all states to device
void A3DDeviceRSCache::ApplyAllStates()
{
	ASSERT(m_pA3DDevice);

	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	int i;

	//	For light data, we must give a correct default value, the value may be
	//	applied to device in A3DEffectStateMan::RestoreStates(). If bad
	//	light value is set, some rendering artifacts may occur.
	for (i=0; i < NUM_LIGHT; i++)
	{
		pD3DDevice->SetLight(i, &m_aLights[i].data);
	}

	//	Texture stage stage
	for (i=0; i < MAX_TEX_LAYERS; i++)
	{
		TEXTUREDATA& tex = m_aTexStages[i];
        
		pD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(i + D3DTS_TEXTURE0), (D3DMATRIX*) &tex.matTrans);

		TextureStateMap::iterator it = tex.TSMap.begin();
		for (; it != tex.TSMap.end(); ++it)
		{
			D3DTEXTURESTAGESTATETYPE type = it->first;
			DWORD value = it->second;
			pD3DDevice->SetTextureStageState(i, type, value);
		}
	}
	
	//	Sampler stage state
	for (i=0; i < MAX_SAMPLE_LAYERS; i++)
	{
		SamplerStateMap& SSMap = m_aSSMaps[i];

		SamplerStateMap::iterator it = SSMap.begin();
		for (; it != SSMap.end(); ++it)
		{
			D3DSAMPLERSTATETYPE type = it->first;
			DWORD value = it->second;
			pD3DDevice->SetSamplerState(i, type, value);
		}
	}

	for(i = 0; i < VS_SAMPLE_LAYERS_NUM; i++)
	{
		SamplerStateMap& SSMap = m_aVsSSMaps[i];

		SamplerStateMap::iterator it = SSMap.begin();
		for (; it != SSMap.end(); ++it)
		{
			D3DSAMPLERSTATETYPE type = it->first;
			DWORD value = it->second;
			pD3DDevice->SetSamplerState(i, type, value);
		}
	}

	//	Render state
	RenderStateMap::iterator it = m_RSMap.begin();
	for (; it != m_RSMap.end(); ++it)
	{
		D3DRENDERSTATETYPE type = it->first;
		DWORD value = it->second;
		pD3DDevice->SetRenderState(type, value);
	}
}
