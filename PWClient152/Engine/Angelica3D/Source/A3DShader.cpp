 /*
 * FILE: A3DShader.cpp
 *
 * DESCRIPTION: Routines for shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DShader.h"
#include "A3DPI.h"
#include "A3DMathUtility.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DTextureMan.h"
#include "A3DShaderMan.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DPixelShader.h"
#include "AScriptFile.h"
#include "AFileImage.h"
#include "ATime.h"
#include "A3DHLSL.h"

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

//	Default general properties
const SHADERGENERAL A3DShader::m_DefGeneral;
//	Default stage properties
const SHADERSTAGE A3DShader::m_DefStage;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement SHADERGENERAL
//
///////////////////////////////////////////////////////////////////////////

SHADERGENERAL::SHADERGENERAL()
{
	dwMask			= 0;
	dwZFunc			= A3DCMP_LESSEQUAL;
	bZEnable		= true;
	bZWriteEnable	= true;
	dwAlphaFunc		= A3DCMP_ALWAYS;
	iAlphaRef		= 128;
	dwCullMode		= A3DCULL_CCW;
	dwSrcBlend		= A3DBLEND_SRCALPHA;
	dwDstBlend		= A3DBLEND_INVSRCALPHA;
	pPShader		= NULL;

	memset(&rgbGen, 0, sizeof (rgbGen));
	memset(&AlphaGen, 0, sizeof (AlphaGen));

	dwTFactor		= 0xffffffff;
	dwTTemp			= 0;

	iTransTextureIndex = 0;
	iTransChannel = 3;

	memset(aPSConsts, 0, sizeof (aPSConsts));
}

SHADERGENERAL::SHADERGENERAL(const SHADERGENERAL& s)
{
	Duplicate(s);
}

//	= operator
const SHADERGENERAL& SHADERGENERAL::operator = (const SHADERGENERAL& s)
{
	Duplicate(s);
	return *this;
}

void SHADERGENERAL::Duplicate(const SHADERGENERAL& s)
{
	dwMask				= s.dwMask;
	dwZFunc				= s.dwZFunc;
	bZEnable			= s.bZEnable;
	bZWriteEnable		= s.bZWriteEnable;
	dwAlphaFunc			= s.dwAlphaFunc;
	iAlphaRef			= s.iAlphaRef;
	dwCullMode			= s.dwCullMode;
	dwSrcBlend			= s.dwSrcBlend;
	dwDstBlend			= s.dwDstBlend;
	pPShader			= s.pPShader;
	rgbGen				= s.rgbGen;
	AlphaGen			= s.AlphaGen;
	dwTFactor			= s.dwTFactor;
	dwTTemp				= s.dwTTemp;
	iTransTextureIndex	= s.iTransTextureIndex;
	iTransChannel		= s.iTransChannel;

	memcpy(aPSConsts, s.aPSConsts, sizeof (aPSConsts));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement SHADERSTAGE
//
///////////////////////////////////////////////////////////////////////////

SHADERSTAGE::SHADERSTAGE()
{
	//	Set default value
	dwMask		= 0;
	dwStates	= 0;
	iTexType	= A3DSDTEX_NONE;
	TexFormat	= A3DFMT_UNKNOWN;

	dwFrameTime = 0;
	dwFilter	= A3DTEXF_LINEAR;
	dwAddress	= A3DTADDR_WRAP;
	
	dwColorOp	= A3DTOP_DISABLE;
	dwColorArg1 = A3DTA_TEXTURE;
	dwColorArg2 = A3DTA_CURRENT;
	dwAlphaOp	= A3DTOP_DISABLE;
	dwAlphaArg1 = A3DTA_TEXTURE;
	dwAlphaArg2 = A3DTA_CURRENT;
	r = g = b = a = 0.0f;
	dwAlphaFunc = A3DCMP_ALWAYS;
	iAlphaRef	= 128;
	dwResultArg = A3DTA_CURRENT;
	dwTexCoord	= 0;
	dwTTFF		= A3DTTFF_DISABLE;
	
	memset(matTexTrans, 0, sizeof (matTexTrans));

	fBumpEnvMat00	= 0.0f;
	fBumpEnvMat01	= 0.0f;
	fBumpEnvMat10	= 0.0f;
	fBumpEnvMat11	= 0.0f;
	fBumpEnvScale	= 0.0f;
	fBumpEnvOffset	= 0.0f;
	
	iUVGenFunc	= 0;
	iProTexFunc = 0;
	iNumtcMod	= 0;
	
	memset(atcMods, 0, sizeof (atcMods));

	dwColorOPOld	= A3DTA_TEXTURE;
	dwAlphaOPOld	= A3DTA_CURRENT;
}

SHADERSTAGE::SHADERSTAGE(const SHADERSTAGE& s)
{
	Duplicate(s);
}

//	= operator
const SHADERSTAGE& SHADERSTAGE::operator = (const SHADERSTAGE& s)
{
	Duplicate(s);
	return *this;
}

void SHADERSTAGE::Duplicate(const SHADERSTAGE& s)
{
	dwMask		= s.dwMask;
	dwStates	= s.dwStates;
	iTexType	= s.iTexType;
	TexFormat	= s.TexFormat;
	
	int i;

	//	TODO: maybe we should clone textures
	aTextures.SetSize(s.aTextures.GetSize(), 4);
	for (i=0; i < s.aTextures.GetSize(); i++)
		aTextures[i] = s.aTextures[i];

	dwFrameTime = s.dwFrameTime;
	dwFilter	= s.dwFilter;
	dwAddress	= s.dwAddress;
	dwColorOp	= s.dwColorOp;
	dwColorArg1 = s.dwColorArg1;
	dwColorArg2 = s.dwColorArg2;
	dwAlphaOp	= s.dwAlphaOp;
	dwAlphaArg1 = s.dwAlphaArg1;
	dwAlphaArg2 = s.dwAlphaArg2;

	r = s.r;
	g = s.g;
	b = s.b;
	a = s.a;

	dwAlphaFunc = s.dwAlphaFunc;
	iAlphaRef	= s.iAlphaRef;
	dwResultArg = s.dwResultArg;
	dwTexCoord	= s.dwTexCoord;
	dwTTFF		= s.dwTTFF;

	for (i=0; i < 16; i++)
		matTexTrans[i] = s.matTexTrans[i];

	fBumpEnvMat00	= s.fBumpEnvMat00;
	fBumpEnvMat01	= s.fBumpEnvMat01;
	fBumpEnvMat10	= s.fBumpEnvMat10;
	fBumpEnvMat11	= s.fBumpEnvMat11;
	fBumpEnvScale	= s.fBumpEnvScale;
	fBumpEnvOffset	= s.fBumpEnvOffset;

	iUVGenFunc	= s.iUVGenFunc;
	iProTexFunc	= s.iProTexFunc;
	iNumtcMod	= s.iNumtcMod;

	for (i=0; i < 4; i++)
		atcMods[i] = s.atcMods[i];

	dwColorOPOld	= s.dwColorOPOld;
	dwAlphaOPOld	= s.dwAlphaOPOld;
}

static bool ComputeFactor(DWORD dwOp, bool bSwapArg, A3DVECTOR4& vFactor)
{
    if (dwOp == A3DTOP_DISABLE)
    {
        vFactor.Set(0, 1, 0, 0);
    }
    else
    {
        switch (dwOp)
        {
        case A3DTOP_SELECTARG1  : vFactor.Set(1, 0, 0, 0); break;
        case A3DTOP_SELECTARG2  : vFactor.Set(1, 0, 0, 0); break;
        case A3DTOP_PREMODULATE : vFactor.Set(1, 0, 0, 0); break;
        case A3DTOP_MODULATE    : vFactor.Set(0, 0, 1, 0); break;
        case A3DTOP_MODULATE2X  : vFactor.Set(0, 0, 2, 0); break;
        case A3DTOP_MODULATE4X  : vFactor.Set(0, 0, 4, 0); break;
        case A3DTOP_ADD         : vFactor.Set(1, 1, 0, 0); break;
        case A3DTOP_ADDSIGNED   : vFactor.Set(1, 1, 0, -0.5f); break;
        case A3DTOP_ADDSIGNED2X : vFactor.Set(2, 2, 0, -1); break;
        case A3DTOP_SUBTRACT    : vFactor.Set(1, -1, 0, 0); break;
        case A3DTOP_ADDSMOOTH   : vFactor.Set(1, 1, -1, 0); break;
        default: return false;
        }
        if (bSwapArg)
        {
            a_Swap(vFactor.x, vFactor.y);
        }
    }
    return true;

}

bool SHADERSTAGE::ComputeHLSLParams()
{
    bool bSupport = true;
    if (dwMask & SSMF_RESULTARG)
        bSupport = false;

    vColorFactor.Set(0, 0, 1, 0);

    if (dwMask & SSMF_COLOROP)
    {
        bool bSwapArg = dwColorArg1 == A3DTA_CURRENT && dwColorArg2 == A3DTA_TEXTURE;

        if (!bSwapArg && 
            dwColorOp != A3DTOP_SELECTARG1 &&
            dwColorOp != A3DTOP_SELECTARG2 &&
            (dwColorArg2 != A3DTA_CURRENT || dwColorArg1 != A3DTA_TEXTURE))
            bSupport = false;

        if (!ComputeFactor(dwColorOp, bSwapArg, vColorFactor))
            bSupport = false;
    }

    vAlphaFactor.Set(0, 0, 1, 0);

    if (dwMask & SSMF_ALPHAOP)
    {
        bool bSwapArg = dwAlphaArg1 == A3DTA_CURRENT && dwAlphaArg2 == A3DTA_TEXTURE;

        if (!bSwapArg && 
            dwAlphaOp != A3DTOP_SELECTARG1 &&
            dwAlphaOp != A3DTOP_SELECTARG2 &&
            (dwAlphaArg2 != A3DTA_CURRENT || dwAlphaArg1 != A3DTA_TEXTURE))
            bSupport = false;

        if (!ComputeFactor(dwAlphaOp, bSwapArg, vAlphaFactor))
            bSupport = false;
    }

    return bSupport;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DShader
//
///////////////////////////////////////////////////////////////////////////

A3DShader::A3DShader() : A3DTexture()
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_iNumStage			= 0;
	m_dwStartTime		= 0;
	m_iMaxNumStage		= 0;
	m_bAlphaTexture		= false;
	m_bShaderTexture	= true;
	m_pBaseTexture		= NULL;

	m_dwClassID			= A3D_CID_SHADER;

	m_bOnlyActiveStages	= false;

	memset(m_aFuncVal, 0, sizeof (m_aFuncVal));
	memset(m_aCurFrame, 0, sizeof (m_aCurFrame));
	m_pTransMaskTexture	= NULL;
}

A3DShader::~A3DShader()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pA3DEngine: address of A3DEngine object
*/
bool A3DShader::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);

	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWITexture = true;
		return true;
	}

	m_iNumStage			= 0;
	m_dwStartTime		= 0;
	m_bAlphaTexture		= false;
	m_bShaderTexture	= true;
	m_iVersion			= VERSION;

	m_iMaxNumStage = m_pA3DDevice->GetD3DCaps().MaxTextureBlendStages;//GetMaxSimultaneousTextures();

	return true;
}

//	Release object
bool A3DShader::Release()
{
	if( m_bHWITexture ) return true;

	ReleasePixelShader();

	for (int i=0; i < m_iNumStage; i++)
	{
		//	Release texture
		ReleaseStageTexture(i);
	}
	
	m_pBaseTexture = NULL;
	return true;
}

/*	Apply shader.

	iLayer: parameter of CA3DTexture::Appear, we don't use it in shader
*/
bool A3DShader::Appear(int iLayer/* 0 */)
{
	if( m_bHWITexture ) return true;

	//	Apply general properties
	ApplyGeneral();

	if (!m_iNumStage)
	{
		m_pA3DDevice->ClearTexture(0);
		return true;
	}

	//	Apply stage properties
	int i, iNumStage = 0;

	for (i=0; i < m_iNumStage; i++)
	{
		if (StageIsVisible(i))
			ApplyStage(iNumStage++, i);

		if (iNumStage >= m_iMaxNumStage)
			break;
	}

	if( !m_bOnlyActiveStages )
	{
		//	Disable unused stages
		if (iNumStage && iNumStage < MAXNUM_STAGE)
			m_pA3DDevice->SetTextureColorOP(iNumStage, A3DTOP_DISABLE);
	}

	return true;
}

/*	Abolish shader.

	iLayer: parameter of CA3DTexture::Disappear, we don't use it in shader
*/
bool A3DShader::Disappear(int iLayer/* 0 */)
{
	if( m_bHWITexture ) return true;

	//	Apply default general properties
	ApplyDefGeneral();

	//	Apply default stage properties
	int i, iNumStage = 0;

	for (i=0; i < m_iNumStage; i++)
	{
		if (StageIsVisible(i))
			RestoreStage(iNumStage++, i);

		if (iNumStage >= m_iMaxNumStage)
			break;
	}

	return true;
}

//	Apply general properties
void A3DShader::ApplyGeneral()
{
	if (m_General.dwMask & SGMF_ZFUNC)
		m_pA3DDevice->SetZFunc(m_General.dwZFunc);

	if (m_General.dwMask & SGMF_ZENABLE)
		m_pA3DDevice->SetZTestEnable(m_General.bZEnable);

	if (m_General.dwMask & SGMF_ZWENABLE)
		m_pA3DDevice->SetZWriteEnable(m_General.bZWriteEnable);

	if (m_General.dwMask & SGMF_ALPHACOMP)
	{
		m_pA3DDevice->SetAlphaTestEnable(true);
		m_pA3DDevice->SetAlphaFunction((A3DCMPFUNC)m_General.dwAlphaFunc);
		m_pA3DDevice->SetAlphaRef(m_General.iAlphaRef);
	}

	if (m_General.dwMask & SGMF_CULLMODE)
		m_pA3DDevice->SetFaceCull((A3DCULLTYPE)m_General.dwCullMode);

	if (m_General.dwMask & SGMF_BLEND)
	{
		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetSourceAlpha((A3DBLEND)m_General.dwSrcBlend);
		m_pA3DDevice->SetDestAlpha((A3DBLEND)m_General.dwDstBlend);
	}

	//	colorgen and alphagen aren't applied here
//	if (m_General.dwMask & SGMF_COLORGEN)
//	if (m_General.dwMask & SGMF_ALPHAGEN)

	if (m_General.dwMask & SGMF_TFACTOR)
		m_pA3DDevice->SetTextureFactor(m_General.dwTFactor);

	if (m_General.pPShader)
	{
		if (m_General.dwMask & SGMF_TFACTOR)
		{
			//	Use constant 0 to store texture factor
			A3DCOLORVALUE col(m_General.dwTFactor);
			m_pA3DDevice->SetPixelShaderConstants(0, &col, 1);
		}

		if (m_General.dwMask & SGMF_TTEMP)
		{
			//	Use constant 0 to store texture factor
			A3DCOLORVALUE col(m_General.dwTTemp);
			m_pA3DDevice->SetPixelShaderConstants(1, &col, 1);
		}

		//	Apply c2-c6 constant registers
		DWORD dwConstMask = SGMF_PSC2;
		int i;
		for (i=0; i < 6; i++)
		{
			if (m_General.dwMask & dwConstMask)
				m_pA3DDevice->SetPixelShaderConstants(2+i, &m_General.aPSConsts[i], 1);

			dwConstMask <<= 1;
		}

		if(m_General.pPShader->GetClassID() == A3D_CID_PIXELSHADER)
			((A3DPixelShader*)m_General.pPShader)->Appear();
		else if(m_General.pPShader->GetClassID() == A3D_CID_HLSL)
			((A3DHLSL*)m_General.pPShader)->Appear();
	}
}

/*	Apply one stage

	iStage: destination stage level of D3D to which specified stage will applied to
	iIndex: index of stage in m_aStages.

	Note: usurally, iStage == iIndex, but when we hide one or more stages, they
		  may be not equal.
*/
void A3DShader::ApplyStage(int iStage, int iIndex)
{
	SHADERSTAGE* pStage = &m_aStages[iIndex];

	m_pA3DDevice->SetTextureCoordIndex(iStage, 0);

	int iNumTex = pStage->aTextures.GetSize();
	if (iNumTex)
	{
		if (pStage->iTexType == A3DSDTEX_NORMAL)
		{
			A3DTexture* pTexture = (A3DTexture*)pStage->aTextures[0];
			if (pTexture)
				pTexture->Appear(iStage);
		}
		else if (pStage->iTexType == A3DSDTEX_ANIMATION)
		{
			int iCurFrame = m_aCurFrame[iIndex].iCurFrame;
			A3DTexture* pTexture = (A3DTexture*)pStage->aTextures[iCurFrame];
			if (pTexture)
				pTexture->Appear(iStage);
		}
		else if (pStage->iTexType == A3DSDTEX_D3DTEX)
		{
			A3DTextureProxy* pTexProxy = (A3DTextureProxy*)pStage->aTextures[0];
			if (pTexProxy)
				pTexProxy->Appear(iStage);
		}
	}

	if (pStage->dwMask & SSMF_FILTER)
		m_pA3DDevice->SetTextureFilterType(iStage, (A3DTEXTUREFILTERTYPE)pStage->dwFilter);

	if (pStage->dwMask & SSMF_ADDRESS)
		m_pA3DDevice->SetTextureAddress(iStage, (A3DTEXTUREADDR)pStage->dwAddress, (A3DTEXTUREADDR)pStage->dwAddress);

	if (pStage->dwMask & SSMF_BUMPMAT)
	{
		m_pA3DDevice->SetTextureBumpMat(iStage, pStage->fBumpEnvMat00, pStage->fBumpEnvMat01,
										pStage->fBumpEnvMat10, pStage->fBumpEnvMat11);
	}

	if (pStage->dwMask & SSMF_BUMPSCALE)
		m_pA3DDevice->SetTextureBumpScale(iStage, pStage->fBumpEnvScale);

	if (pStage->dwMask & SSMF_BUMPOFFSET)
		m_pA3DDevice->SetTextureBumpOffset(iStage, pStage->fBumpEnvOffset);

	if (pStage->dwMask & SSMF_TEXTRANS)
	{
		// ×¢ÊÍµô²âÊÔ
		// ±¨´í: Direct3D9: (ERROR) :Number of coordinates specified by texture transform flags is unsupported.
		//m_pA3DDevice->SetTextureTransformFlags(iStage, (A3DTEXTURETRANSFLAGS)pStage->dwTTFF);
		//m_pA3DDevice->SetTextureMatrix(iStage, A3DMATRIX4(pStage->matTexTrans));
	}

	if (pStage->dwMask & SSMF_TEXCOORD)
		m_pA3DDevice->SetTextureCoordIndex(iStage, (int)pStage->dwTexCoord);
	
	//	TODO: handle these...
//	if (pStage->dwMask & SSMF_COLORGEN)
//	if (pStage->dwMask & SSMF_ALPHAGEN)
//	if (pStage->dwMask & SSMF_ALPHACOMP)
//	Stage.iProTexFunc
//	Stage.iUVGenFunc

	//	Texture Coordinates Modify
	if (pStage->iNumtcMod)
	{
		A3DMATRIX4 mat = GetTextureMatrix(iIndex);
		m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(iStage, mat);
	}

	if (!m_General.pPShader)
	{
		//	Use legacy pixel pipeline
		pStage->dwColorOPOld = m_pA3DDevice->GetTextureColorOP(iStage);
		if (pStage->dwMask & SSMF_COLOROP)
		{
			m_pA3DDevice->SetTextureColorOP(iStage, (A3DTEXTUREOP)pStage->dwColorOp);
			m_pA3DDevice->SetTextureColorArgs(iStage, pStage->dwColorArg1, pStage->dwColorArg2);
		}
		else	//	Enable this stage
		{
			m_pA3DDevice->SetTextureColorOP(iStage, A3DTOP_MODULATE);
			m_pA3DDevice->SetTextureColorArgs(iStage, A3DTA_TEXTURE, A3DTA_CURRENT);
		}
		
		pStage->dwAlphaOPOld = m_pA3DDevice->GetTextureAlphaOP(iStage);
		if (pStage->dwMask & SSMF_ALPHAOP)
		{
			m_pA3DDevice->SetTextureAlphaOP(iStage, (A3DTEXTUREOP)pStage->dwAlphaOp);
			m_pA3DDevice->SetTextureAlphaArgs(iStage, pStage->dwAlphaArg1, pStage->dwAlphaArg2);
		}
		else
			m_pA3DDevice->SetTextureAlphaOP(iStage, A3DTOP_DISABLE);

		if (pStage->dwMask & SSMF_RESULTARG)
			m_pA3DDevice->SetTextureResultArg(iStage, pStage->dwResultArg);
	}
}

//	Apply default general properties
void A3DShader::ApplyDefGeneral()
{
	if (m_General.dwMask & SGMF_ZFUNC)
		m_pA3DDevice->SetZFunc(m_DefGeneral.dwZFunc);

	if (m_General.dwMask & SGMF_ZENABLE)
		m_pA3DDevice->SetZTestEnable(m_DefGeneral.bZEnable);

	if (m_General.dwMask & SGMF_ZWENABLE)
		m_pA3DDevice->SetZWriteEnable(m_DefGeneral.bZWriteEnable);

	if (m_General.dwMask & SGMF_ALPHACOMP)
	{
		m_pA3DDevice->SetAlphaTestEnable(false);
	//	m_pA3DDevice->SetAlphaFunction((A3DCMPFUNC)m_DefGeneral.dwAlphaFunc);
	//	m_pA3DDevice->SetAlphaRef(m_DefGeneral.iAlphaRef);
	}

	if (m_General.dwMask & SGMF_CULLMODE)
		m_pA3DDevice->SetFaceCull((A3DCULLTYPE)m_DefGeneral.dwCullMode);

	if (m_General.dwMask & SGMF_BLEND)
	{
		m_pA3DDevice->SetAlphaBlendEnable(true);	//	Our default value is enable
		m_pA3DDevice->SetSourceAlpha((A3DBLEND)m_DefGeneral.dwSrcBlend);
		m_pA3DDevice->SetDestAlpha((A3DBLEND)m_DefGeneral.dwDstBlend);
	}

	if (m_General.pPShader)
	{
		if(m_General.pPShader->GetClassID() == A3D_CID_PIXELSHADER)
			((A3DPixelShader*)m_General.pPShader)->Disappear();
		else if(m_General.pPShader->GetClassID() == A3D_CID_HLSL)
			((A3DHLSL*)m_General.pPShader)->Disappear();
	}
}

/*	Apply default stage properties for a stage

	iStage: destination stage level of D3D which will be applied to default data
	iIndex: index of stage in m_aStages.

	Note: usurally, iStage == iIndex, but when we hide one or more stages, they
		  may not be equal.
*/
void A3DShader::RestoreStage(int iStage, int iIndex)
{
	SHADERSTAGE* pStage = &m_aStages[iIndex];

	if (pStage->aTextures.GetSize())
	{
		A3DTexture* pTexture = (A3DTexture*)pStage->aTextures[0];
		if (pTexture)
			m_pA3DDevice->ClearTexture(iStage);
	}
	
	if (pStage->dwMask & SSMF_FILTER)
		m_pA3DDevice->SetTextureFilterType(iStage, (A3DTEXTUREFILTERTYPE)m_DefStage.dwFilter);

	if (pStage->dwMask & SSMF_ADDRESS)
		m_pA3DDevice->SetTextureAddress(iStage, (A3DTEXTUREADDR)m_DefStage.dwAddress, (A3DTEXTUREADDR)m_DefStage.dwAddress);

	if (pStage->dwMask & SSMF_BUMPMAT)
	{
		m_pA3DDevice->SetTextureBumpMat(iStage, m_DefStage.fBumpEnvMat00, m_DefStage.fBumpEnvMat01,
										m_DefStage.fBumpEnvMat10, m_DefStage.fBumpEnvMat11);
	}

	if (pStage->dwMask & SSMF_BUMPSCALE)
		m_pA3DDevice->SetTextureBumpScale(iStage, m_DefStage.fBumpEnvScale);

	if (pStage->dwMask & SSMF_BUMPOFFSET)
		m_pA3DDevice->SetTextureBumpOffset(iStage, m_DefStage.fBumpEnvOffset);

	if (pStage->dwMask & SSMF_TEXTRANS)
	{
		m_pA3DDevice->SetTextureTransformFlags(iStage, (A3DTEXTURETRANSFLAGS)m_DefStage.dwTTFF);
		m_pA3DDevice->SetTextureMatrix(iStage, A3D::g_matIdentity);
	}

	//if (pStage->dwMask & SSMF_TEXCOORD)
	m_pA3DDevice->SetTextureCoordIndex(iStage, iStage);
	
	//	Reset texture matrix
	if (pStage->iNumtcMod)
		m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_DISABLE);

	if (!m_General.pPShader)
	{
		//	Use legacy pixel pipeline
		m_pA3DDevice->SetTextureColorOP(iStage, (A3DTEXTUREOP) pStage->dwColorOPOld);
		if (pStage->dwMask & SSMF_COLOROP)
			m_pA3DDevice->SetTextureColorArgs(iStage, m_DefStage.dwColorArg1, m_DefStage.dwColorArg2);
		
		m_pA3DDevice->SetTextureAlphaOP(iStage, (A3DTEXTUREOP) pStage->dwAlphaOPOld);
		if (pStage->dwMask & SSMF_ALPHAOP)
			m_pA3DDevice->SetTextureAlphaArgs(iStage, m_DefStage.dwAlphaArg1, m_DefStage.dwAlphaArg2);
		
		if (pStage->dwMask & SSMF_RESULTARG)
			m_pA3DDevice->SetTextureResultArg(iStage, A3DTA_CURRENT);
	}
}

/*	Insert a stage.

	Return true for success, otherwise return false.

	iIndex: position of new stage, -1 means add stage at tail
	Stage: stage data
*/
bool A3DShader::InsertStage(int iIndex, SHADERSTAGE& Stage)
{
	if (m_iNumStage >= MAXNUM_STAGE || iIndex < -1)
		return false;

	if (iIndex == -1 || iIndex >= m_iNumStage)	//	Add at tail
		m_aStages[m_iNumStage] = Stage;
	else
	{
		int i;
		for (i=m_iNumStage-1; i >= iIndex; i--)
			m_aStages[i+1] = m_aStages[i];

		m_aStages[iIndex] = Stage;
	}

	m_iNumStage++;

	return true;
}

/*	Remove a stage

	iIndex: 0 base position of stage which will be removed
*/
void A3DShader::RemoveStage(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumStage)
		return;

	//	Release texture
	ReleaseStageTexture(iIndex);

	int i;
	for (i=iIndex; i < m_iNumStage-1; i++)
		m_aStages[i] = m_aStages[i+1];

	m_iNumStage--;
}

//	Release specified stage's texture
void A3DShader::ReleaseStageTexture(int iStage)
{
	if (!m_pA3DDevice)
		return;

	SHADERSTAGE& stage = m_aStages[iStage];
	int iNumTex = stage.aTextures.GetSize();

	if (iNumTex)
	{
		if (stage.iTexType != A3DSDTEX_D3DTEX)
		{
			for (int i=0; i < stage.aTextures.GetSize(); i++)
			{
				A3DTexture* pTexture = (A3DTexture*)stage.aTextures[i];
				if (pTexture)
					m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(pTexture);
			}
		}
		else	//	stage.iTexType == A3DSDTEX_D3DTEX
		{
			A3DTextureProxy* pTexture = (A3DTextureProxy*)stage.aTextures[0];
			//	D3D textures are always set by outside at run-time, so we 
			//	don't really release them here
		}

		stage.aTextures.RemoveAll(false);
	}
}

//	Release pixel shader
void A3DShader::ReleasePixelShader()
{
	if (!m_General.pPShader || !m_pA3DDevice)
		return;

	if(m_General.pPShader->GetClassID() == A3D_CID_PIXELSHADER)
		m_pA3DEngine->GetA3DShaderMan()->ReleasePixelShader((A3DPixelShader**)&m_General.pPShader);
	else if(m_General.pPShader->GetClassID() == A3D_CID_HLSL)
	{
		m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader((A3DHLSL*)m_General.pPShader);
	}
	m_General.pPShader = NULL;
}

//	Set general properties
void A3DShader::SetGeneralProps(SHADERGENERAL& Props)
{
	if(m_General.pPShader != Props.pPShader )
		ReleasePixelShader();
	m_General = Props; 
}

/*	Set a stage's data

	Return true for success, otherwise return false.

	iStage: stage's index
	Stage: set a stage's data
*/
bool A3DShader::SetStage(int iStage, SHADERSTAGE& Stage)
{
	if (iStage < 0 || iStage >= m_iNumStage)
		return false;

	m_aStages[iStage] = Stage;

	return true;
}

/*	Change a stage's texture.

	Return true for success, otherwise return false.

	iStage: stage's index
	iType: texture type
	dwTex: for different texture type, this parameter has different means

		A3DSDTEX_NORMAL: dwTex is address of texture map file string.
		A3DSDTEX_ANIMATION: dwTex is address of texture map file string.
		A3DSDTEX_LIGHTMAP:
		A3DSDTEX_BUMPMAP:
		A3DSDTEX_PROCESS:
		A3DSDTEX_DETAIL:

	Fmt: specified which format texture should be created as. A3DFMT_UNKNOWN means
		to use default texture format
*/
bool A3DShader::ChangeStageTexture(int iStage, A3DSHADERTEXTYPE iType, DWORD dwTex, 
							A3DFORMAT Fmt/* A3DFMT_UNKNOWN */, int iNumAnimTex/* 1 */)
{
	if (iStage < 0)
		return false;

	//	Release old texture
	ReleaseStageTexture(iStage);

	A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();
	SHADERSTAGE* pStage = &m_aStages[iStage];

	pStage->iTexType	= iType;
	pStage->TexFormat	= Fmt;

	//	Load new map file
	if (iType == A3DSDTEX_NORMAL)
	{
		char* szMapFile = (char*)dwTex;
		bool bRet;

		A3DTexture* pTexture = NULL;
		if (Fmt == A3DFMT_UNKNOWN)
			bRet = pTexMan->LoadTextureFromFile(szMapFile, &pTexture);
		else
			bRet = pTexMan->LoadTextureFromFile(szMapFile, 0, 0, Fmt, 0, &pTexture);

		if (!bRet)
		{
			g_A3DErrLog.Log("A3DShader::ChangeStageTexture, failed to load map file %s", szMapFile);
			return false;
		}

		pStage->aTextures.Add(pTexture);
		m_pBaseTexture = pTexture;
		
		if((m_General.dwMask & SGMF_TRANSPARENTTEXCHANNEL) && iStage == m_General.iTransTextureIndex)//Added by PanYupin
			m_pTransMaskTexture = pTexture;
	}
	else if (iType == A3DSDTEX_ANIMATION)
	{
		char szPre[MAX_PATH], szSuf[16], szMapFile[MAX_PATH];
		char* pTemp, *szFile = (char*)dwTex;

		if (!(pTemp = strrchr(szFile, '.')))
		{
			g_A3DErrLog.Log("A3DShader::ChangeStageTexture, invalid map file %s", szFile);
			return false;
		}

		*pTemp = '\0';
		strcpy(szPre, szFile);
		sprintf(szSuf, ".%s", pTemp+1);

		A3DTexture* pTexture = NULL;
		bool bRet;

		for (int i=0; i < iNumAnimTex; i++)
		{
			sprintf(szMapFile, "%s%02d%s", szPre, i, szSuf);

			if (Fmt == A3DFMT_UNKNOWN)
				bRet = pTexMan->LoadTextureFromFile(szMapFile, &pTexture);
			else
				bRet = pTexMan->LoadTextureFromFile(szMapFile, 0, 0, Fmt, 0, &pTexture);

			if (!bRet)
			{
				g_A3DErrLog.Log("A3DShader::ChangeStageTexture, failed to load map file %s", szMapFile);
				return false;
			}
			
			pStage->aTextures.Add(pTexture);
		}
	}
	else
	{
		ASSERT(0);
		return false;
	}

	return true;
}

bool A3DShader::ChangeStageTexture_D3DTex(int iStage, A3DTextureProxy* pTexProxy)
{
	if (iStage < 0)
		return false;

	//	Release old texture
	ReleaseStageTexture(iStage);

	SHADERSTAGE* pStage = &m_aStages[iStage];

	pStage->iTexType	= A3DSDTEX_D3DTEX;
	pStage->TexFormat	= A3DFMT_UNKNOWN;

	pStage->aTextures.Add(pTexProxy);
	return true;
}

/*	Change pixel shader

	Return true for success, otherwise return false.

	szFileName: shader's file name. NULL means clear pixel shader
*/
bool A3DShader::ChangePixelShader(const char* szFileName)
{
	//	Release old pixel shader if it exists
	if (m_General.pPShader)
		ReleasePixelShader();

	if (!szFileName)
		return true;

	if(strcmpi(szFileName + strlen(szFileName) - 5, ".hlsl") == 0)
	{
		A3DHLSL* pNewHLSL = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(NULL, NULL, szFileName, "ps_main", NULL, NULL);
		m_General.pPShader = pNewHLSL;
	}
	else
	{
		A3DPixelShader* pNewShader = m_pA3DEngine->GetA3DShaderMan()->LoadPixelShader(szFileName, false);
		if (!pNewShader)
		{
#ifdef _DEBUG
			g_A3DErrLog.Log("A3DShader::ChangePixelShader, failed to load pixel shader from file %s", szFileName);
#endif
			return false;
		}

		m_General.pPShader = pNewShader;
	}

	return true;
}

//	Load shader from disk file
bool A3DShader::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DShader::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

/*	Load shader from file

	Return true for success, otherwise return false.

	pFile: file which contain shader's information. This file should be open as
		   script file.
*/
bool A3DShader::Load(AFile* pFile)
{
	if (m_bHWITexture)
		return true;

	AScriptFile ScriptFile;

	if (!ScriptFile.Open(pFile))
	{
		g_A3DErrLog.Log("A3DShader::Load, Failed to open script file.");
		return false;
	}

	m_strMapFile	= pFile->GetRelativeName();
	m_dwTextureID	= (DWORD) this;//a_MakeIDFromLowString(m_strMapFile);
	
    g_A3DErrLog.Log("A3DShader::Load, %s.", m_strMapFile);

    //	Clear all stage information
	if (m_General.dwMask || m_iNumStage)
		Release();

	m_iNumStage	= 0;

	char* szToken = ScriptFile.m_szToken;

	ScriptFile.ResetScriptFile();

	//	Load version
	if (!ScriptFile.GetNextToken(true))
	{
		g_A3DErrLog.Log("A3DShader::Load, Failed to read shader's version");
		goto Error;
	}

	if (!_stricmp(szToken, "version"))
	{
		//	Get version number
		ScriptFile.GetNextToken(false);
		m_iVersion = atoi(szToken);

		//	Get shader's name
		if (!ScriptFile.GetNextToken(true))
		{
			g_A3DErrLog.Log("A3DShader::Load, Failed to read shader's name");
			goto Error;
		}
	}
	else
		m_iVersion = 1;

	SetName(szToken);

	//	Search token "{"
	if (!ScriptFile.MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DShader::Load, Failed get shader's body");
		goto Error;
	}

	while (ScriptFile.PeekNextToken(true))
	{
		if (!_stricmp(szToken, "}"))		//	End of shader body
		{
			//	Skip '}'
			ScriptFile.GetNextToken(true);
			break;
		}

		if (!_stricmp(szToken, "{"))		//	Is a stage
		{
			if (!LoadStage(&ScriptFile))
			{
				g_A3DErrLog.Log("A3DShader::Load, Failed parse %d stage", m_iNumStage);
				goto Error;
			}
		}
		else 	//	Is a general properties
		{
			if (!LoadGeneralProperty(&ScriptFile))
				goto Error;
		}
	}

	ScriptFile.Close();
	return true;

Error:

	ScriptFile.Close();
	return false;
}

/*	Load a stage.

	Return true for success, otherwise return false

	pFile: shader script file
*/
bool A3DShader::LoadStage(AScriptFile* pFile)
{
	if (m_iNumStage >= MAXNUM_STAGE)
	{
		g_A3DErrLog.Log("A3DShader::LoadStage, Too many stages");
		return false;
	}

	m_aStages[m_iNumStage].dwMask = 0;

	//	Search '{'
	if (!pFile->MatchToken("{", false))
		return false;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			//	Skip '}'
			pFile->GetNextToken(true);
			break;
		}

		if (!LoadStageProperty(pFile))
			return false;
	}
    
    if (!m_aStages[m_iNumStage].ComputeHLSLParams())
    {
        g_A3DErrLog.Log("A3DShader::LoadStage, Stage properties are not supported: %s.", m_strMapFile);
        //return false;
    }
	m_iNumStage++;

	return true;
}

/*	Load a stage property.

	Return true for success, otherwise return false.

	pFile: shader script file
*/
bool A3DShader::LoadStageProperty(AScriptFile* pFile)
{
	char* szToken = pFile->m_szToken;
	bool bValueOK = true;
	char szKey[100];

	SHADERSTAGE* pStage = &m_aStages[m_iNumStage];

	//	Get property keyword
	if (!pFile->GetNextToken(true))
		return false;

	strcpy(szKey, szToken);

	if (!_stricmp(szKey, "texture"))
	{
		//	Get texture type
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TEX_NORMAL"))
				pStage->iTexType = A3DSDTEX_NORMAL;
			else if (!_stricmp(szToken, "TEX_LIGHTMAP"))
				pStage->iTexType = A3DSDTEX_LIGHTMAP;
			else if (!_stricmp(szToken, "TEX_BUMPMAP"))
				pStage->iTexType = A3DSDTEX_BUMPMAP;
			else if (!_stricmp(szToken, "TEX_PROCESS"))
				pStage->iTexType = A3DSDTEX_PROCESS;
			else if (!_stricmp(szToken, "TEX_DETAIL"))
				pStage->iTexType = A3DSDTEX_DETAIL;
			else if (!_stricmp(szToken, "TEX_ANIMATION"))
				pStage->iTexType = A3DSDTEX_ANIMATION;
			else if (!_stricmp(szToken, "TEX_D3DTEX"))
				pStage->iTexType = A3DSDTEX_D3DTEX;
			else
				bValueOK = false;
		}

		if (!bValueOK)
			goto EndTexture;

		//	Get texture format
		if (m_iVersion >= 3)
			pStage->TexFormat = (A3DFORMAT)pFile->GetNextTokenAsInt(false);
		else
			pStage->TexFormat = A3DFMT_UNKNOWN;

		//	Get texture parameter
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			//	Load map file here
			char szTexFile[MAX_PATH];

			if (pStage->iTexType == A3DSDTEX_NORMAL)
			{
				if (m_iVersion == 1)
					sprintf(szTexFile, "Shaders\\Textures\\%s", szToken);
				else
					strcpy(szTexFile, szToken);

				ChangeStageTexture(m_iNumStage, pStage->iTexType, (DWORD)&szTexFile[0], pStage->TexFormat);
			}
			else if (pStage->iTexType == A3DSDTEX_ANIMATION)
			{
				int iNumTex = atoi(szToken);
				a_ClampFloor(iNumTex, 0);

				//	Frame time
				pFile->GetNextToken(false);
				pStage->dwFrameTime = atoi(szToken);

				//	File name
				pFile->GetNextToken(false);

				if (m_iVersion == 1)
					sprintf(szTexFile, "Shaders\\Textures\\%s", szToken);
				else
					strcpy(szTexFile, szToken);

				ChangeStageTexture(m_iNumStage, pStage->iTexType, (DWORD)&szTexFile[0], pStage->TexFormat, iNumTex);
			}
			else if (pStage->iTexType == A3DSDTEX_D3DTEX)
			{
				ASSERT(m_iVersion >= 7);
				//	D3D texture will be set at run-time
			}
			else
			{
				//	TODO: handle parameter of other type texture
				ASSERT(0);
			}
		}

EndTexture:;

	}
	else if (!_stricmp(szKey, "filter"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TEXF_NONE"))
				pStage->dwFilter = A3DTEXF_NONE;
			else if (!_stricmp(szToken, "TEXF_POINT"))
				pStage->dwFilter = A3DTEXF_POINT;
			else if (!_stricmp(szToken, "TEXF_LINEAR"))
				pStage->dwFilter = A3DTEXF_LINEAR;
			else if (!_stricmp(szToken, "TEXF_ANISOTROPIC"))
				pStage->dwFilter = A3DTEXF_ANISOTROPIC;
			else if (!_stricmp(szToken, "TEXF_FLATCUBIC"))
				pStage->dwFilter = A3DTEXF_FLATCUBIC;
			else if (!_stricmp(szToken, "TEXF_GAUSSIANCUBIC"))
				pStage->dwFilter = A3DTEXF_GAUSSIANCUBIC;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= SSMF_FILTER;
		}
	}
	else if (!_stricmp(szKey, "address"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TADDR_WRAP"))
				pStage->dwAddress = A3DTADDR_WRAP;
			else if (!_stricmp(szToken, "TADDR_MIRROR"))
				pStage->dwAddress = A3DTADDR_MIRROR;
			else if (!_stricmp(szToken, "TADDR_CLAMP"))
				pStage->dwAddress = A3DTADDR_CLAMP;
			else if (!_stricmp(szToken, "TADDR_BORDER"))
				pStage->dwAddress = A3DTADDR_BORDER;
			else if (!_stricmp(szToken, "TADDR_MIRRORONCE"))
				pStage->dwAddress = A3DTADDR_MIRRORONCE;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= SSMF_ADDRESS;
		}
	}
	else if (!_stricmp(szKey, "colorop") || !_stricmp(szKey, "alphaop"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue, dwMask;
			if (!_stricmp(szKey, "colorop"))
			{
				pValue = &pStage->dwColorOp;
				dwMask = SSMF_COLOROP;
			}
			else
			{
				pValue = &pStage->dwAlphaOp;
				dwMask = SSMF_ALPHAOP;
			}

			if (!_stricmp(szToken, "TOP_DISABLE"))
				*pValue = A3DTOP_DISABLE;
			else if (!_stricmp(szToken, "TOP_SELECTARG1"))
				*pValue = A3DTOP_SELECTARG1;
			else if (!_stricmp(szToken, "TOP_SELECTARG2"))
				*pValue = A3DTOP_SELECTARG2;
			else if (!_stricmp(szToken, "TOP_MODULATE"))
				*pValue = A3DTOP_MODULATE;
			else if (!_stricmp(szToken, "TOP_MODULATE2X"))
				*pValue = A3DTOP_MODULATE2X;
			else if (!_stricmp(szToken, "TOP_MODULATE4X"))
				*pValue = A3DTOP_MODULATE4X;
			else if (!_stricmp(szToken, "TOP_ADD"))
				*pValue = A3DTOP_ADD;
			else if (!_stricmp(szToken, "TOP_ADDSIGNED"))
				*pValue = A3DTOP_ADDSIGNED;
			else if (!_stricmp(szToken, "TOP_ADDSIGNED2X"))
				*pValue = A3DTOP_ADDSIGNED2X;
			else if (!_stricmp(szToken, "TOP_SUBTRACT"))
				*pValue = A3DTOP_SUBTRACT;
			else if (!_stricmp(szToken, "TOP_ADDSMOOTH"))
				*pValue = A3DTOP_ADDSMOOTH;
			else if (!_stricmp(szToken, "TOP_BLENDDIFFUSEALPHA"))
				*pValue = A3DTOP_BLENDDIFFUSEALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDTEXTUREALPHA"))
				*pValue = A3DTOP_BLENDTEXTUREALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDFACTORALPHA"))
				*pValue = A3DTOP_BLENDFACTORALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDTEXTUREALPHAPM"))
				*pValue = A3DTOP_BLENDTEXTUREALPHAPM;
			else if (!_stricmp(szToken, "TOP_BLENDCURRENTALPHA"))
				*pValue = A3DTOP_BLENDCURRENTALPHA;
			else if (!_stricmp(szToken, "TOP_PREMODULATE"))
				*pValue = A3DTOP_PREMODULATE;
			else if (!_stricmp(szToken, "TOP_MODULATEALPHA_ADDCOLOR"))
				*pValue = A3DTOP_MODULATEALPHA_ADDCOLOR;
			else if (!_stricmp(szToken, "TOP_MODULATECOLOR_ADDALPHA"))
				*pValue = A3DTOP_MODULATECOLOR_ADDALPHA;
			else if (!_stricmp(szToken, "TOP_MODULATEINVALPHA_ADDCOLOR"))
				*pValue = A3DTOP_MODULATEINVALPHA_ADDCOLOR;
			else if (!_stricmp(szToken, "TOP_MODULATEINVCOLOR_ADDALPHA"))
				*pValue = A3DTOP_MODULATEINVCOLOR_ADDALPHA;
			else if (!_stricmp(szToken, "TOP_BUMPENVMAP"))
				*pValue = A3DTOP_BUMPENVMAP;
			else if (!_stricmp(szToken, "TOP_BUMPENVMAPLUMINANCE"))
				*pValue = A3DTOP_BUMPENVMAPLUMINANCE;
			else if (!_stricmp(szToken, "TOP_DOTPRODUCT3"))
				*pValue = A3DTOP_DOTPRODUCT3;
			else if (!_stricmp(szToken, "TOP_MULTIPLYADD"))
				*pValue = A3DTOP_MULTIPLYADD;
			else if (!_stricmp(szToken, "TOP_LERP"))
				*pValue = A3DTOP_LERP;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= dwMask;
		}
	}
	else if (!_stricmp(szKey, "colorarg1") || !_stricmp(szKey, "colorarg2") ||
			 !_stricmp(szKey, "alphaarg1") || !_stricmp(szKey, "alphaarg2"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue;
			if (!_stricmp(szKey, "colorarg1"))
				pValue = &pStage->dwColorArg1;
			else if (!_stricmp(szKey, "colorarg2"))
				pValue = &pStage->dwColorArg2;
			else if (!_stricmp(szKey, "alphaarg1"))
				pValue = &pStage->dwAlphaArg1;
			else
				pValue = &pStage->dwAlphaArg2;

			//	Argument
			if (!_stricmp(szToken, "TA_CURRENT"))
				*pValue = A3DTA_CURRENT;
			else if (!_stricmp(szToken, "TA_DIFFUSE"))
				*pValue = A3DTA_DIFFUSE;
			else if (!_stricmp(szToken, "TA_SELECTMASK"))
				*pValue = A3DTA_SELECTMASK;
			else if (!_stricmp(szToken, "TA_SPECULAR"))
				*pValue = A3DTA_SPECULAR;
			else if (!_stricmp(szToken, "TA_TEMP"))
				*pValue = A3DTA_TEMP;
			else if (!_stricmp(szToken, "TA_TEXTURE"))
				*pValue = A3DTA_TEXTURE;
			else if (!_stricmp(szToken, "TA_TFACTOR"))
				*pValue = A3DTA_TFACTOR;
			else
				bValueOK = false;

			//	Modifier is optional
			if (pFile->GetNextToken(false))
			{
				if (!_stricmp(szToken, "TA_COMPLEMENT"))
					*pValue |= A3DTA_COMPLEMENT;
				else if (!_stricmp(szToken, "TA_ALPHAREPLICATE"))
					*pValue |= A3DTA_ALPHAREPLICATE;
			}
		}
	}
	else if (!_stricmp(szKey, "colorgen") || !_stricmp(szKey, "alphagen"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "alphacomp"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "bumpmat"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvMat00 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat01 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat10 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat11 = (float)atof(szToken);

			pStage->dwMask |= SSMF_BUMPMAT;
		}
	}
	else if (!_stricmp(szKey, "bumpscale"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvScale = (float)atof(szToken);
			pStage->dwMask |= SSMF_BUMPSCALE;
		}
	}
	else if (!_stricmp(szKey, "bumpoff"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvOffset = (float)atof(szToken);
			pStage->dwMask |= SSMF_BUMPOFFSET;
		}
	}
	else if (!_stricmp(szKey, "uvgenfunc"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "protexfunc"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "tcscale"))
	{
		if (!LoadTCMod(pFile, SWO_TCSCALE, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcscroll"))
	{
		if (!LoadTCMod(pFile, SWO_TCSCROLL, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcrotate"))
	{
		if (!LoadTCMod(pFile, SWO_TCROTATE, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcstretch"))
	{
		if (!LoadTCMod(pFile, SWO_TCSTRETCH, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcturb"))
	{
		if (!LoadTCMod(pFile, SWO_TCTURB, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "resultarg"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			//	Argument
			if (!_stricmp(szToken, "TA_CURRENT"))
				pStage->dwResultArg = A3DTA_CURRENT;
			else if (!_stricmp(szToken, "TA_TEMP"))
				pStage->dwResultArg = A3DTA_TEMP;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= SSMF_RESULTARG;
		}
	}
	else if (!_stricmp(szKey, "textrans"))
	{
		pStage->dwTTFF = (DWORD)pFile->GetNextTokenAsInt(false);
		pStage->matTexTrans[0] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[1] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[2] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[3] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[4] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[5] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[6] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[7] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[8] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[9] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[10] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[11] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[12] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[13] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[14] = pFile->GetNextTokenAsFloat(false);
		pStage->matTexTrans[15] = pFile->GetNextTokenAsFloat(false);

		pStage->dwMask |= SSMF_TEXTRANS;
	}
	else if (!_stricmp(szKey, "texcoord"))
	{
		pStage->dwTexCoord = (DWORD)pFile->GetNextTokenAsInt(false);
		pFile->GetNextToken(false);
		//	Argument
		if (!_stricmp(szToken, "TCI_PASSTHRU"))
			pStage->dwTexCoord |= A3DTCI_PASSTHRU;
		else if (!_stricmp(szToken, "TCI_CAMERASPACENORMAL"))
			pStage->dwTexCoord |= A3DTCI_CAMERASPACENORMAL;
		else if (!_stricmp(szToken, "TCI_CAMERASPACEPOSITION"))
			pStage->dwTexCoord |= A3DTCI_CAMERASPACEPOSITION;
		else if (!_stricmp(szToken, "TCI_CAMERASPACEREFLECTIONVECTOR"))
			pStage->dwTexCoord |= A3DTCI_CAMERASPACEREFLECTIONVECTOR;

		pStage->dwMask |= SSMF_TEXCOORD;
	}
	else	//	Unknown property keyword
	{
		g_A3DErrLog.Log("A3DShader::LoadStageProperty, shader '%s': unknown stage property: '%s' was ignored", m_strName, szKey);
	}

	if (!bValueOK)
		g_A3DErrLog.Log("A3DShader::LoadStageProperty, shader '%s': invalid value for stage property '%s' ", m_strName, szKey);

	//	Skip this line
	pFile->SkipLine();

	return true;
}

/*	Load a general property.

	Return true for success, otherwise return false

	pFile: shader script file
*/
bool A3DShader::LoadGeneralProperty(AScriptFile* pFile)
{
	char* szToken = pFile->m_szToken;
	bool bValueOK = true;
	char szKey[100];

	//	Get property keyword
	if (!pFile->GetNextToken(true))
		return false;

	strcpy(szKey, szToken);

	if (!_stricmp(szToken, "depthfunc") || !_stricmp(szToken, "alphacomp"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue, dwMask;
			if (!_stricmp(szKey, "depthfunc"))
			{
				pValue = &m_General.dwZFunc;
				dwMask = SGMF_ZFUNC;
			}
			else
			{
				pValue = &m_General.dwAlphaFunc;
				dwMask = SGMF_ALPHACOMP;
			}

			if (!_stricmp(szToken, "CMP_NEVER"))
				*pValue = A3DCMP_NEVER;
			else if (!_stricmp(szToken, "CMP_LESS"))
				*pValue = A3DCMP_LESS;
			else if (!_stricmp(szToken, "CMP_EQUAL"))
				*pValue = A3DCMP_EQUAL;
			else if (!_stricmp(szToken, "CMP_LESSEQUAL"))
				*pValue = A3DCMP_LESSEQUAL;
			else if (!_stricmp(szToken, "CMP_GREATER"))
				*pValue = A3DCMP_GREATER;
			else if (!_stricmp(szToken, "CMP_NOTEQUAL"))
				*pValue = A3DCMP_NOTEQUAL;
			else if (!_stricmp(szToken, "CMP_GREATEREQUAL"))
				*pValue = A3DCMP_GREATEREQUAL;
			else if (!_stricmp(szToken, "CMP_ALWAYS"))
				*pValue = A3DCMP_ALWAYS;
			else
				bValueOK = false;

			if (dwMask == SGMF_ALPHACOMP)
			{
				//	Read reference value
				if (!pFile->GetNextToken(false))
					m_General.iAlphaRef = 128;	//	Default value
				else
					m_General.iAlphaRef = atoi(szToken);
			}

			if (bValueOK)
				m_General.dwMask |= dwMask;
		}
	}
	else if (!_stricmp(szToken, "zenable"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			int iValue = atoi(szToken);
			m_General.bZEnable = iValue ? true : false;
			m_General.dwMask  |= SGMF_ZENABLE;	
		}
	}
	else if (!_stricmp(szToken, "zwriteenable"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			int iValue = atoi(szToken);
			m_General.bZWriteEnable = iValue ? true : false;
			m_General.dwMask |= SGMF_ZWENABLE;
		}
	}
	else if (!_stricmp(szToken, "cullmode"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "CULL_NONE"))
				m_General.dwCullMode = A3DCULL_NONE;
			else if (!_stricmp(szToken, "CULL_CW"))
				m_General.dwCullMode = A3DCULL_CW;
			else if (!_stricmp(szToken, "CULL_CCW"))
				m_General.dwCullMode = A3DCULL_CCW;
			else
				bValueOK = false;

			if (bValueOK)
				m_General.dwMask |= SGMF_CULLMODE;
		}
	}
	else if (!_stricmp(szToken, "blend"))
	{
		//	Get value
		for (int i=0; i < 2; i++)
		{
			if (!pFile->GetNextToken(false))
			{
				bValueOK = false;
				break;
			}

			DWORD* pValue;
			if (!i)
				pValue = &m_General.dwSrcBlend;
			else
				pValue = &m_General.dwDstBlend;

			if (!_stricmp(szToken, "BLEND_ZERO"))
				*pValue = A3DBLEND_ZERO;
			else if (!_stricmp(szToken, "BLEND_ONE"))
				*pValue = A3DBLEND_ONE;
			else if (!_stricmp(szToken, "BLEND_SRCCOLOR"))
				*pValue = A3DBLEND_SRCCOLOR;
			else if (!_stricmp(szToken, "BLEND_INVSRCCOLOR"))
				*pValue = A3DBLEND_INVSRCCOLOR;
			else if (!_stricmp(szToken, "BLEND_SRCALPHA"))
				*pValue = A3DBLEND_SRCALPHA;
			else if (!_stricmp(szToken, "BLEND_INVSRCALPHA"))
				*pValue = A3DBLEND_INVSRCALPHA;
			else if (!_stricmp(szToken, "BLEND_DESTALPHA"))
				*pValue = A3DBLEND_DESTALPHA;
			else if (!_stricmp(szToken, "BLEND_INVDESTALPHA"))
				*pValue = A3DBLEND_INVDESTALPHA;
			else if (!_stricmp(szToken, "BLEND_DESTCOLOR"))
				*pValue = A3DBLEND_DESTCOLOR;
			else if (!_stricmp(szToken, "BLEND_INVDESTCOLOR"))
				*pValue = A3DBLEND_INVDESTCOLOR;
			else if (!_stricmp(szToken, "BLEND_SRCALPHASAT"))
				*pValue = A3DBLEND_SRCALPHASAT;
			else if (!_stricmp(szToken, "BLEND_BOTHSRCALPHA"))
				*pValue = A3DBLEND_BOTHSRCALPHA;
			else if (!_stricmp(szToken, "BLEND_BOTHINVSRCALPHA"))
				*pValue = A3DBLEND_BOTHINVSRCALPHA;
			else
				bValueOK = false;
		}

		if (bValueOK)
		{
			m_General.dwMask |= SGMF_BLEND;
			m_bAlphaTexture	  = true;
		}
	}
	else if (!_stricmp(szToken, "colorgen"))
	{
		//	Load wave
		if (!LoadWave(pFile, &m_General.rgbGen))
			bValueOK = false;
		else
		{
			m_General.rgbGen.iType = SWO_COLORGEN;
			m_General.dwMask |= SGMF_COLORGEN;
		}
	}
	else if (!_stricmp(szToken, "alphagen"))
	{
		//	Load wave
		if (!LoadWave(pFile, &m_General.AlphaGen))
			bValueOK = false;
		else
		{
			m_General.rgbGen.iType = SWO_ALPHAGEN;
			m_General.dwMask |= SGMF_ALPHAGEN;
		}
	}
	else if (!_stricmp(szToken, "texturefactor"))
	{
		A3DCOLORVALUE col;
		col.r = pFile->GetNextTokenAsFloat(false);
		col.g = pFile->GetNextTokenAsFloat(false);
		col.b = pFile->GetNextTokenAsFloat(false);
		col.a = pFile->GetNextTokenAsFloat(false);

		m_General.dwTFactor = col.ToRGBAColor();
		m_General.dwMask |= SGMF_TFACTOR;
	}
	else if (!_stricmp(szToken, "pixelshader"))
	{
		//	Get pixel shader's file name
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else if (!ChangePixelShader(szToken))
		{
			//	In poor display card, bValueOK = false here will case too many
			//	logs output.
		//	bValueOK = false;
		}
	}
	else if (!_stricmp(szToken, "alphatexflag"))
	{
		int iVal = pFile->GetNextTokenAsInt(false);
		m_bAlphaTexture = iVal ? true : false;
	}
	else if (!_stricmp(szToken, "ps_const"))
	{
		int iIndex = pFile->GetNextTokenAsInt(false);
		ASSERT(iIndex >= 2 && iIndex <= 8);
		A3DCOLORVALUE col;
		col.r = pFile->GetNextTokenAsFloat(false);
		col.g = pFile->GetNextTokenAsFloat(false);
		col.b = pFile->GetNextTokenAsFloat(false);
		col.a = pFile->GetNextTokenAsFloat(false);
		m_General.aPSConsts[iIndex-2] = col;
		DWORD dwConstMask = SGMF_PSC2 << (iIndex-2);
		m_General.dwMask |= dwConstMask;
	}
	else if (!stricmp(szToken, "transtexchannel"))//Added by PanYupin
	{
		int iTexIndex = pFile->GetNextTokenAsInt(false);
		m_General.iTransTextureIndex = iTexIndex;
		int iChannelIndex = pFile->GetNextTokenAsInt(false);
		m_General.iTransChannel = iChannelIndex;
		m_General.dwMask |= SGMF_TRANSPARENTTEXCHANNEL;
	}
	else	//	Unknown property keyword
	{
		g_A3DErrLog.Log("A3DShader::LoadGeneralProperty, shader '%s': unknown general property: '%s' was ignored", m_strName, szKey);
	}

	if (!bValueOK)
		g_A3DErrLog.Log("A3DShader::LoadGeneralProperty, shader '%s': invalid value for general property '%s' ", m_strName, szKey);

	//	Skip this line
	pFile->SkipLine();

	return true;
}

/*	Load a tc modifier property.

	Return true for success, otherwise return false.

	pFile: script file pointer.
	iType: tc modifier type
	pStage: current stage.	
*/
bool A3DShader::LoadTCMod(AScriptFile* pFile, int iType, SHADERSTAGE* pStage)
{
	char* szToken = pFile->m_szToken;
	int i;

	//	Check whether this property has existed
	for (i=0; i < pStage->iNumtcMod; i++)
	{
		if (pStage->atcMods[i].iType == iType)
			break;
	}

	if (i < pStage->iNumtcMod)	//	The property has existed, overwrite it
		g_A3DErrLog.Log("A3DShader::LoadTCMod, shader '%s': tc modifier overwrite.", m_strName);
	else if (pStage->iNumtcMod >= MAXNUM_TCMOD)
	{
		g_A3DErrLog.Log("A3DShader::LoadTCMod, shader '%s': too much tcmods.", m_strName);
		return false;
	}

	SHADERWAVE* pWave = &pStage->atcMods[i];

	switch (iType)
	{
	case SWO_TCSCALE:
	case SWO_TCSCROLL:

		//	u scale or scroll
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fBase = (float)atof(szToken);

		//	v scale or scroll
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fAmp	= (float)atof(szToken);
		break;

	case SWO_TCROTATE:

		//	Rotate value (degree per second)
		if (!pFile->GetNextToken(false))
			return false;

		//	Convert degree to radian
		pWave->fBase = DEG2RAD((float)atof(szToken));
		break;

	case SWO_TCSTRETCH:

		if (!LoadWave(pFile, &pStage->atcMods[pStage->iNumtcMod]))
			return false;
		
		break;

	case SWO_TCTURB:

		//	Amplitude
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fAmp = (float)atof(szToken);

		//	Phase
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fPhase = (float)atof(szToken);

		//	Period
		if (!pFile->GetNextToken(false))
			return false;

		pWave->iPeriod = atoi(szToken);
		break;

	default:
		return false;
	}

	pWave->iType = iType;

	if (i == pStage->iNumtcMod)
		pStage->iNumtcMod++;

	return true;
}

/*	Load a wave function

	Return true for success, otherwise return false.

	pFile: script file pointer.
	pWave: used to receive wave data.
*/
bool A3DShader::LoadWave(AScriptFile* pFile, SHADERWAVE* pWave)
{
	char* szToken = pFile->m_szToken;

	//	Wave function -------------------------
	if (!pFile->GetNextToken(false))
		return false;

	if (!_stricmp(szToken, "W_SIN"))
		pWave->iFunc = SWFT_SIN;
	else if (!_stricmp(szToken, "W_TRIANGLE"))
		pWave->iFunc = SWFT_TRIANGLE;
	else if (!_stricmp(szToken, "W_SQUARE"))
		pWave->iFunc = SWFT_SQUARE;
	else if (!_stricmp(szToken, "W_SAWTOOTH"))
		pWave->iFunc = SWFT_SAWTOOTH;
	else if (!_stricmp(szToken, "W_INVSAWTOOTH"))
		pWave->iFunc = SWFT_INVSAWTOOTH;
	else
	{
		g_A3DErrLog.Log("A3DShader::LoadWave, unknown wave function: %s", szToken);
		return false;
	}

	//	Baseline value ------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fBase = (float)atof(szToken);

	//	Amplitude -----------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fAmp = (float)atof(szToken);

	//	Initial phase -------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fPhase = (float)atof(szToken);

	//	Frequency -----------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->iPeriod = atoi(szToken);

	return true;
}

//	Tick animation
bool A3DShader::TickAnimation()
{
	if( m_bHWITexture ) return true;

	DWORD dwTime, dwCurTime = a_GetTime();

	if (!m_dwStartTime)
	{
		m_dwStartTime = dwCurTime;
		dwTime = 0;
	}
	else
		dwTime = dwCurTime - m_dwStartTime;

	//	Color generate
	if (m_General.dwMask & SGMF_COLORGEN)
		m_aFuncVal[VAL_GENCOLORGEN] = CaluWaveValue(m_General.rgbGen, dwTime);

	//	Alpha generate
	if (m_General.dwMask & SGMF_ALPHAGEN)
		m_aFuncVal[VAL_GENALPHAGEN] = CaluWaveValue(m_General.AlphaGen, dwTime);

	int i, j;
	SHADERWAVE* pWave;
	float x = dwTime * 0.001f;

	for (i=0; i < m_iNumStage; i++)
	{
		SHADERSTAGE& stage = m_aStages[i];

		//	Texture animation
		int iNumTex = stage.aTextures.GetSize();
		if (stage.iTexType == A3DSDTEX_ANIMATION && stage.dwFrameTime && iNumTex)
		{
			ANIMFRAME& frame = m_aCurFrame[i];
			
			if (!frame.dwTimeCnt) // initialize to be first frame
			{
				frame.dwTimeCnt = dwCurTime;
				frame.iCurFrame = frame.iCurStartFrame;
			}
			else
			{
				int iFrameDelta = (dwCurTime - frame.dwTimeCnt) / stage.dwFrameTime;
				if (iFrameDelta > 0)
				{
					if( frame.iCurStartFrame <= frame.iCurEndFrame && frame.iCurEndFrame > 0 )
					{
						// range loop
						frame.iCurFrame = frame.iCurFrame + iFrameDelta;
						if( frame.iCurFrame > frame.iCurEndFrame )
						{
							if( frame.bLoop )
								frame.iCurFrame = frame.iCurStartFrame;
							else
								frame.iCurFrame = frame.iCurEndFrame;
						}
					}
					else
					{
						// entire loop
						frame.iCurFrame = (frame.iCurFrame + iFrameDelta) % iNumTex;
					}
					frame.dwTimeCnt += iFrameDelta * stage.dwFrameTime;
				}
			}
		}
			
		for (j=0; j < stage.iNumtcMod; j++)
		{
			pWave = &stage.atcMods[j];

			switch (pWave->iType)
			{
			case SWO_TCSTRETCH:		//	tcmod stretch for every stage

				m_aFuncVal[VAL_TCMODSTRETCH0 + i] = CaluWaveValue(*pWave, dwTime);
				break;

			case SWO_TCSCROLL:

				m_aFuncVal[VAL_TCMODUSCROLL0 + i] = x * pWave->fBase;
				m_aFuncVal[VAL_TCMODVSCROLL0 + i] = x * pWave->fAmp;
				break;

			case SWO_TCROTATE:

				m_aFuncVal[VAL_TCMODROTATE0 + i] = x * pWave->fBase;
				break;

			case SWO_TCTURB:

				CaluTurbulence(*pWave, dwTime, i);
				break; 

			default:
				continue;
			}
		}
	}

	return true;
}

/*	Calculate wave function's value

	Return result.

	Wave: wave function parameters
	dwTime: time in ms
*/
float A3DShader::CaluWaveValue(SHADERWAVE& Wave, DWORD dwTime)
{
	float x, y;

	x  = (float)(dwTime % Wave.iPeriod) / Wave.iPeriod + Wave.fPhase;
	x -= (float)floor(x);		//	Normalize x to 0.0f -- 1.0f

	switch (Wave.iFunc)
	{
	case SWFT_SIN:

		y = g_A3DMathUtility.SIN(x * 360.0f);
		break;

	case SWFT_TRIANGLE:

		y = (x <= 0.5f) ? x * 2.0f : (1.0f - x) * 2.0f;
		break;

	case SWFT_SQUARE:

		y = (x <= 0.5f) ? 1.0f : -1.0f;
		break;

	case SWFT_INVSAWTOOTH:

		y = 1.0f - x;
		break;

	case SWFT_SAWTOOTH:
	default:

		y = x;
		break;
	}

	return y * Wave.fAmp + Wave.fBase;
}

/*	Calculate turbulence value

	Wave: wave function parameters
	dwTime: time in ms
	iStage: index of stage
*/
void A3DShader::CaluTurbulence(SHADERWAVE& Wave, DWORD dwTime, int iStage)
{
	float x1, fSin1, x2, fSin2;

	x1  = (float)(dwTime % Wave.iPeriod) / Wave.iPeriod + Wave.fPhase;
	x1 -= (float)floor(x1);		//	Normalize x to 0.0f -- 1.0f

	fSin1 = g_A3DMathUtility.SIN(x1 * 360.0f) * Wave.fAmp;

	int iPeriod = (int)(Wave.iPeriod * 0.99f);
	x2  = (float)(dwTime % iPeriod) / iPeriod + Wave.fPhase;
	x2 -= (float)floor(x2);		//	Normalize x to 0.0f -- 1.0f

	fSin2 = g_A3DMathUtility.SIN(x2 * 360.0f) * Wave.fAmp;

	m_aFuncVal[VAL_TCMODOUTURB0 + iStage] = -fSin1;
	m_aFuncVal[VAL_TCMODOVTURB0 + iStage] = fSin2;
	m_aFuncVal[VAL_TCMODSUTURB0 + iStage] = 1.0f - fSin1;
	m_aFuncVal[VAL_TCMODSVTURB0 + iStage] = 1.0f + fSin1;
}

/*	Get current value of general color generate.

	Return true if value in pfVal is valid, otherwise return false.

	pfVal (out): used to receive general colorgen value if true is returned.
*/
bool A3DShader::GetCurGenColorGenerate(float* pfVal)
{
	if (m_General.dwMask & SGMF_COLORGEN)
	{
		*pfVal = m_aFuncVal[VAL_GENCOLORGEN];
		return true;
	}

	return false;
}

/*	Get current value of general alpha generate.

	Return true if value in pfVal is valid, otherwise return false.

	pfVal (out): used to receive general alphagen value if true is returned.
*/
bool A3DShader::GetCurGenAlphaGenerate(float* pfVal)
{
	//	Alpha generate
	if (m_General.dwMask & SGMF_ALPHAGEN)
	{
		*pfVal = m_aFuncVal[VAL_GENALPHAGEN];
		return true;
	}

	return false;
}

//	Get current frame of stage's animation texture
int A3DShader::GetCurAnimTexFrame(int iStage)
{
	if (iStage < 0 || iStage >= MAXNUM_STAGE)
		return -1;
	else
		return m_aCurFrame[iStage].iCurFrame;
}

//	Set current frame of stage's animation texture
void A3DShader::SetCurAnimTexFrame(int iStage, int iFrame)
{
	if (iStage >= 0 || iStage < MAXNUM_STAGE)
	{
		const SHADERSTAGE& stage = m_aStages[iStage];
		int iNumTex = stage.aTextures.GetSize();

		if (stage.iTexType == A3DSDTEX_ANIMATION && iNumTex)
		{
			m_aCurFrame[iStage].iCurFrame = iFrame % iNumTex;
			m_aCurFrame[iStage].dwTimeCnt = a_GetTime();
		}
	}
}

//	Step animation texture of specified texture
int A3DShader::StepAnimTexFrame(int iStage, int iStep/* 1 */)
{
	if (iStage < 0 || iStage >= MAXNUM_STAGE)
		return -1;

	const SHADERSTAGE& stage = m_aStages[iStage];
	int iNumTex = stage.aTextures.GetSize();

	if (stage.iTexType == A3DSDTEX_ANIMATION && iNumTex)
	{
		ANIMFRAME& frame = m_aCurFrame[iStage];
		frame.iCurFrame += iStep;

		if (frame.iCurFrame >= 0)
		{
			frame.iCurFrame %= iNumTex;
		}
		else
		{
			int iInvFrame = (-frame.iCurFrame) % iNumTex;
			frame.iCurFrame = iInvFrame ? iNumTex - iInvFrame : 0;
		}

		frame.dwTimeCnt = a_GetTime();
		return frame.iCurFrame;
	}

	return -1;
}

void A3DShader::SetCurAnimInfo(int iStage, int iStartFrame, int iEndFrame, bool bLoop, int iFrameTime)
{
	if (iStage >= 0 || iStage < MAXNUM_STAGE)
	{
		SHADERSTAGE& stage = m_aStages[iStage];
		int iNumTex = stage.aTextures.GetSize();

		if (stage.iTexType == A3DSDTEX_ANIMATION && iNumTex)
		{
			m_aCurFrame[iStage].iCurStartFrame = iStartFrame % iNumTex;
			m_aCurFrame[iStage].iCurEndFrame = iEndFrame % iNumTex;
			m_aCurFrame[iStage].bLoop = bLoop;
			m_aCurFrame[iStage].dwTimeCnt = a_GetTime();
			stage.dwFrameTime = iFrameTime;
		}
	}
}

A3DMATRIX4 A3DShader::GetTextureMatrix(int iIndex)
{
    //	Texture Coordinates Modify
    SHADERSTAGE* pStage = &m_aStages[iIndex];
    A3DMATRIX4 mat = a3d_IdentityMatrix();

    if (pStage->iNumtcMod)
    {
        int i;
        SHADERWAVE* pWave;

        for (i=0; i < pStage->iNumtcMod; i++)
        {
            pWave = &pStage->atcMods[i];
            switch (pWave->iType)
            {
            case SWO_TCSCALE:

                //	Scale u and v
                mat._11 *= pWave->fBase;	mat._21 *= pWave->fBase;
                mat._31 *= pWave->fBase;	
                mat._12 *= pWave->fAmp;		mat._22 *= pWave->fAmp;
                mat._32 *= pWave->fAmp;
                break;

            case SWO_TCSCROLL:

                mat._31 += m_aFuncVal[VAL_TCMODUSCROLL0 + iIndex];
                mat._32 += m_aFuncVal[VAL_TCMODVSCROLL0 + iIndex];
                break;

            case SWO_TCROTATE:

                //	u -= 0.5f, v -= 0.5f
                mat._31 -= 0.5f;	mat._32 -= 0.5f;

                mat = a3d_RotateZ(mat, m_aFuncVal[VAL_TCMODROTATE0 + iIndex]);

                //	u += 0.5f, v += 0.5f
                mat._31 += 0.5f;	mat._32 += 0.5f;
                break;

            case SWO_TCSTRETCH:
                {
                    //	u -= 0.5f, v -= 0.5f
                    mat._31 -= 0.5f;	mat._32 -= 0.5f;

                    //	Scale u and v
                    float fVal = m_aFuncVal[VAL_TCMODSTRETCH0 + iIndex];

                    mat._11 *= fVal;	mat._21 *= fVal;	mat._31 *= fVal;
                    mat._12 *= fVal;	mat._22 *= fVal;	mat._32 *= fVal;

                    //	u += 0.5f, v += 0.5f
                    mat._31 += 0.5f;	mat._32 += 0.5f;
                    break;
                }
            case SWO_TCTURB:
                {
                    mat._31 += m_aFuncVal[VAL_TCMODOUTURB0 + iIndex];
                    mat._32 += m_aFuncVal[VAL_TCMODOVTURB0 + iIndex];

                    //	Scale u and v
                    float fu = m_aFuncVal[VAL_TCMODSUTURB0 + iIndex];
                    float fv = m_aFuncVal[VAL_TCMODSVTURB0 + iIndex];

                    mat._11 *= fu;	mat._21 *= fu;	mat._31 *= fu;
                    mat._12 *= fv;	mat._22 *= fv;	mat._32 *= fv;
                    break;
                }
            }
        }
    }

    return mat;
}