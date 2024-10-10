/*
 * FILE: A3DSkySphere.cpp
 *
 * DESCRIPTION: Class that standing for the sphere sky method in A3D Engine
 *
 * CREATED BY: Hedi, 2003/1/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */
 
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DSkySphere.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DStream.h"
#include "A3DCamera.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "AMemory.h"
#include "A3DHLSL.h"
#include "A3DVertexShader.h"

static const D3DVERTEXELEMENT9 s_aVertexDecl[] =
{
    {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    D3DDECL_END()
};


#define new A_DEBUG_NEW

A3DSkySphere::ANIM_PROP::ANIM_PROP()
{
	fCapTile		= 8.0f;
	fCapSpeedU		= 0.018f;
	fCapSpeedV		= 0.018f;
	fCapRotSpeed	= 0.0f;
	fCapOffsetU		= 0.0f;
	fCapOffsetV		= 0.0f;
	fCapRotate		= 0.0f;
}

A3DSkySphere::A3DSkySphere() :
m_tmCurCapCloud(A3DMATRIX4::IDENTITY),
m_tmDstCapCloud(A3DMATRIX4::IDENTITY)
{
	m_pA3DDevice		= NULL;
	m_pCamera			= NULL;

	m_pStreamSurround	= NULL;
	m_pStreamCap		= NULL;

	m_pTextureSurroundF = NULL;
	m_pTextureSurroundB = NULL;
	m_pTextureCap		= NULL;

	m_pTextureSurroundFDest = NULL;
	m_pTextureSurroundBDest = NULL;
	m_pTextureCapDest = NULL;

	m_bTransToDest		= true;
	m_nTransTime		= 0;
	m_nTransTickLeft	= 0;

	m_bTextureLoading	= false;

	m_nSegmentCount		= 6;
	m_vPitchRange		= DEG2RAD(17.5f);
	m_fRadius			= 800.0f;

    m_pHLSLCap          = NULL;
    m_pHLSLCapBlend     = NULL;
    m_pHLSLSurround     = NULL;
    m_pHLSLSurroundBlend = NULL;
    m_pVertDecl         = NULL;
}

A3DSkySphere::~A3DSkySphere()
{
}

bool A3DSkySphere::Init(A3DDevice* pDevice, A3DCamera* pCamera, const char* szCap, 
						const char* szSurroundF, const char* szSurroundB)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
	
	m_pA3DDevice		= pDevice;
	m_pCamera			= pCamera;
	m_nTransTime		= 0;
	m_nTransTickLeft	= 0;

	// First Create Surround Cloud;
	if( !CreateSurround() )
		return false;

	if( !CreateCap() )
		return false;

	//Load the textures;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szCap, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSurroundF, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSurroundB, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder("trans.tga", "Textures\\Sky", &m_pTextureTrans, A3DTF_MIPLEVEL, 1) )
		return false;

    A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
    AString strShaderFile = "shaders\\2.2\\HLSL\\Sky\\SkySphere.hlsl";
    m_pHLSLCap = pHLSLMan->LoadShader(strShaderFile, "vs_main", strShaderFile, "ps_main", "_TRANSFORM_", 0);
    if (!m_pHLSLCap)
    {
        g_A3DErrLog.Log("A3DSkySphere::Init, Failed to load cap shader!");
        return false;
    }
    m_pHLSLCapBlend = pHLSLMan->LoadShader(strShaderFile, "vs_main", strShaderFile, "ps_main", "_TRANSFORM_;_BLEND_", 0);
    if (!m_pHLSLCapBlend)
    {
        g_A3DErrLog.Log("A3DSkySphere::Init, Failed to load cap blend shader!");
        return false;
    }
    m_pHLSLSurround = pHLSLMan->LoadShader(strShaderFile, "vs_main", strShaderFile, "ps_main", "", 0);
    if (!m_pHLSLSurround)
    {
        g_A3DErrLog.Log("A3DSkySphere::Init, Failed to load surround shader!");
        return false;
    }
    m_pHLSLSurroundBlend = pHLSLMan->LoadShader(strShaderFile, "vs_main", strShaderFile, "ps_main", "_BLEND_", 0);
    if (!m_pHLSLSurroundBlend)
    {
        g_A3DErrLog.Log("A3DSkySphere::Init, Failed to load surround blend shader!");
        return false;
    }
    m_pVertDecl = new A3DVertexDecl;
    if (!m_pVertDecl->Init(m_pA3DDevice, s_aVertexDecl))
    {
        g_A3DErrLog.Log("A3DSkySphere::Init, Failed to create vertex declaration!");
        return false;
    }
	return true;
}

bool A3DSkySphere::Release()
{
	while( m_bTextureLoading )
		Sleep(5);

	if( m_pTextureTrans )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureTrans);
		m_pTextureTrans = NULL;
	}
	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
		m_pTextureCap = NULL;
	}
	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
		m_pTextureSurroundF = NULL;
	}
	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
		m_pTextureSurroundB = NULL;
	}
	if( m_pTextureCapDest )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCapDest);
		m_pTextureCapDest = NULL;
	}
	if( m_pTextureSurroundFDest )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundFDest);
		m_pTextureSurroundFDest = NULL;
	}
	if( m_pTextureSurroundBDest )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundBDest);
		m_pTextureSurroundBDest = NULL;
	}
	if( m_pStreamSurround )
	{
		m_pStreamSurround->Release();
		delete m_pStreamSurround;
		m_pStreamSurround = NULL;
	}
	if( m_pStreamCap )
	{
		m_pStreamCap->Release();
		delete m_pStreamCap;
		m_pStreamCap = NULL;
	}

    if (m_pA3DDevice)
    {
        A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
        if (m_pHLSLCap)
            pHLSLMan->ReleaseShader(m_pHLSLCap);
        if (m_pHLSLCapBlend)
            pHLSLMan->ReleaseShader(m_pHLSLCapBlend);
        if (m_pHLSLSurround)
            pHLSLMan->ReleaseShader(m_pHLSLSurround);
        if (m_pHLSLSurroundBlend)
            pHLSLMan->ReleaseShader(m_pHLSLSurroundBlend);
    }
    m_pHLSLCap           = NULL;
    m_pHLSLCapBlend      = NULL;
    m_pHLSLSurround      = NULL;
    m_pHLSLSurroundBlend = NULL;

    A3DRELEASE(m_pVertDecl);
	return true;
}

bool A3DSkySphere::RenderCapHLSL(A3DTexture * pTextureCur, A3DTexture * pTextureDst, float fAlpha)
{
    if (!m_pHLSLCap || !m_pHLSLCapBlend)
        return false;

    m_pStreamCap->Appear(0, false);
    
    A3DMATRIX4 matCap;
    matCap = m_WorldMatrix;
    matCap._42 -= m_fRadius * (float)cos(m_vPitchRange);
    //A3DMATRIX4 matVP = m_pA3DDevice->GetViewMatrix() * m_pA3DDevice->GetProjectionMatrix();
	A3DMATRIX4 matVP = m_pCamera->GetVPTM();
    A3DMATRIX4 matWVP = matCap *matVP;
    
    A3DHLSL* pHLSL = fAlpha == 0 ? m_pHLSLCap : m_pHLSLCapBlend;
    pHLSL->SetTexture("g_CurSampler", SafeTexture(pTextureCur));
    pHLSL->SetTexture("g_DestSampler", SafeTexture(pTextureDst));
    pHLSL->SetValue1f("g_fTexIn", fAlpha);
    pHLSL->SetConstantMatrix("g_matWVP", matWVP);
    pHLSL->SetConstantMatrix("g_matCurCapCloud", m_tmCurCapCloud);
    pHLSL->SetConstantMatrix("g_matDstCapCloud", m_tmDstCapCloud);
    pHLSL->Appear();

    m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
    if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountC, 0, m_nIndexCountC / 3))
    {
        g_A3DErrLog.Log("A3DSky::RenderCapHLSL DrawIndexPrimitive Fail!");
        return false;
    }

    return true;
}

bool A3DSkySphere::RenderSurroundHLSL(A3DTexture * pTextureCurF, A3DTexture * pTextureDstF, 
                                      A3DTexture * pTextureCurB, A3DTexture * pTextureDstB,
                                      float fAlpha)
{

    if (!m_pHLSLSurround || !m_pHLSLSurroundBlend)
        return false;
    
    m_pStreamSurround->Appear(0, false);
    //A3DMATRIX4 matVP = m_pA3DDevice->GetViewMatrix() * m_pA3DDevice->GetProjectionMatrix();
	A3DMATRIX4 matVP = m_pCamera->GetVPTM();
    A3DMATRIX4 matWVP = m_WorldMatrix * matVP;
    A3DHLSL* pHLSL = fAlpha == 0 ? m_pHLSLSurround : m_pHLSLSurroundBlend;
    pHLSL->SetTexture("g_CurSampler", SafeTexture(pTextureCurF));
    pHLSL->SetTexture("g_DestSampler", SafeTexture(pTextureDstF));
    pHLSL->SetValue1f("g_fTexIn", fAlpha);
    pHLSL->SetConstantMatrix("g_matWVP", matWVP);
    pHLSL->Appear();
    
    m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3))
    {
        g_A3DErrLog.Log("A3DSky::RenderSurroundHLSL DrawIndexPrimitive Fail!");
        return false;
    }

    A3DMATRIX4 matRotate180 = a3d_IdentityMatrix();
    matRotate180._11 = -1.0f;
    matRotate180._33 = -1.0f;
    matWVP = matRotate180 * matWVP;
    pHLSL->SetTexture("g_CurSampler", SafeTexture(pTextureCurB));
    pHLSL->SetTexture("g_DestSampler", SafeTexture(pTextureDstB));
    pHLSL->SetValue1f("g_fTexIn", fAlpha);
    pHLSL->SetConstantMatrix("g_matWVP", matWVP);
    
    //pHLSL->Commit();

    if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3))
    {
        g_A3DErrLog.Log("A3DSkySphere::RenderSurroundHLSL DrawIndexPrimitive Fail!");
        return false;
    }

    m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);

    return true;
}


bool A3DSkySphere::Render()
{	
	if( !m_pA3DDevice ) return true;

	float fZNear, fZFar;
	// Update World Matrix First;
	if( m_pCamera )
	{
		A3DVECTOR3 vecPos;
		if( m_pCamera->IsMirrored() )
		{
			vecPos = m_pCamera->GetMirrorCamera()->GetPos();
			A3DMATRIX4 matMirror = m_pCamera->GetMirrorMatrix();
			vecPos = vecPos * InverseTM(matMirror);
		}
		else
		{
			vecPos = m_pCamera->GetPos();
		}
		m_WorldMatrix = a3d_Translate(vecPos.x, vecPos.y, vecPos.z);

		fZNear = m_pCamera->GetZFront();
		fZFar = m_pCamera->GetZBack();
		m_pCamera->SetZFrontAndBack(0.1f, 1000.f);
		m_pA3DDevice->SetProjectionMatrix(m_pCamera->GetProjectionTM());
	}
	
    DWORD dwClipState;
    dwClipState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
    m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);

	m_pA3DDevice->SetLightingEnable(false);

    // Set render states
    m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(true);

    RenderHLSL();

    // Restore render states
    m_pA3DDevice->SetZWriteEnable(TRUE);
	m_pA3DDevice->SetZTestEnable(TRUE);
	m_pA3DDevice->SetLightingEnable(TRUE);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
    m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, dwClipState);

	if(m_pCamera)
	{
		m_pCamera->SetZFrontAndBack(fZNear, fZFar);
		m_pA3DDevice->SetProjectionMatrix(m_pCamera->GetProjectionTM());
	}

	return true;
}

bool A3DSkySphere::TickAnimation()
{
	if( !m_pA3DDevice ) return true;

	UpdateCapTexTM(m_tmCurCapCloud, m_CurAnimProp);

	if( m_nTransTickLeft && !m_bTextureLoading )
	{
		UpdateCapTexTM(m_tmDstCapCloud, m_DstAnimProp);

		if( m_bTransToDest )
		{
			m_nTransTickLeft -= 33;
			if( m_nTransTickLeft <= 0 )
			{
				// transition stops here
				m_nTransTickLeft = 0;

				SwitchClouds();
			}
		}
		else
		{
			m_nTransTickLeft += 33;
			if( m_nTransTickLeft >= m_nTransTime )
			{
				// transition stops here
				m_nTransTickLeft = 0;

				SwitchClouds();
			}
		}
	}
	return true;
}

//	Update cap texture tm
void A3DSkySphere::UpdateCapTexTM(A3DMATRIX4& matTM, ANIM_PROP& props)
{
	A3DMATRIX3 mat3x3(A3DMATRIX3::IDENTITY);

//	float fTime = dwDeltaTime * 0.001f;
	float fTime = 1.0f / 30.0f;

	//	Rotate
	if (props.fCapRotSpeed)
	{
		//	u -= 0.5f, v -= 0.5f
		mat3x3._31 -= 0.5f;
		mat3x3._32 -= 0.5f;
		//	Rotate
		props.fCapRotate += props.fCapRotSpeed * fTime;
		mat3x3 = a3d_Matrix3Rotate(mat3x3, props.fCapRotate);
		//	u += 0.5f, v += 0.5f
		mat3x3 = a3d_Matrix3Translate(mat3x3, 0.5f, 0.5f);
	}

	//	Tile
	if (props.fCapTile != 1.0f)
	{
		A3DMATRIX3 matScale(A3DMATRIX3::IDENTITY);
		matScale._11 = props.fCapTile;
		matScale._22 = props.fCapTile;
		mat3x3 *= matScale;
	}

	//	Scroll
	if (props.fCapSpeedU || props.fCapSpeedV)
	{
		props.fCapOffsetU += props.fCapSpeedU * fTime;
		props.fCapOffsetV += props.fCapSpeedV * fTime;
		mat3x3 = a3d_Matrix3Translate(mat3x3, props.fCapOffsetU, props.fCapOffsetV);
	}

	//	Apply matrix to effect
	matTM.Identity();
	matTM.SetRow(0, mat3x3.GetRow(0));
	matTM.SetRow(1, mat3x3.GetRow(1));
	matTM.SetRow(2, mat3x3.GetRow(2));
}

bool A3DSkySphere::SetCamera(A3DCamera * pCamera)
{
	if( !m_pA3DDevice ) return true;

	//If the new set camera is Null, then the old camera is retained;
	if( NULL == pCamera )
		return true;

	m_pCamera = pCamera;
	return true;
}

bool A3DSkySphere::SetTextureSurroundF(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::SetTextureSurroundF(), Can't load texture [%s]", szTextureFile);
		return false;
	}
	
	return true;
}

bool A3DSkySphere::SetTextureSurroundB(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}
	
	return true;
}

bool A3DSkySphere::SetTextureCap(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	return true;
}

bool A3DSkySphere::CreateSurround()
{
	bool bval;

	const int iLatitude = 5;

	m_nVertCountS = (m_nSegmentCount + 1) * iLatitude * 2;
	m_nIndexCountS = m_nSegmentCount * (iLatitude-1) * 6 * 2;

	A3DSKYVERTEX *	pVerts;
	WORD *			pIndices;

	m_pStreamSurround = new A3DStream();
	if (!m_pStreamSurround)
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Not enough memory!");
		return false;
	}
	
	if (!m_pStreamSurround->Init(m_pA3DDevice, sizeof(A3DSKYVERTEX), A3DSKYVERT_FVF, m_nVertCountS, m_nIndexCountS, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Stream Surround Init Fail!");
		return false;
	}

	bval = m_pStreamSurround->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Stream Surround Lock Verts Fail!");
		return false;
	}
	bval = m_pStreamSurround->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL);
	if( !bval )
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Stream Surround Lock Indices Fail!");
		return false;
	}

	// Create a half-angle cylinder to be used as surround(front and back) cloud
	WORD i, j;
	FLOAT vHeight;
	FLOAT alpha;
	int   nVertIndex = 0;
	int   nFaceIndex = 0;
	int   is = (m_nSegmentCount + 1) * 2;
	float h = m_fRadius * 0.7854f;

	float aHeiFactors[iLatitude] = {1.0f, 0.5f, 0.167f, 0.0f, -0.27f};

	for(j=0; j < iLatitude; j++)
	{
		vHeight = h * aHeiFactors[j];
			
		//Left Quadro Cynlinde,r;
		for(i=0; i<=m_nSegmentCount; i++)
		{
			alpha = A3D_PI / 2.0f + A3D_PI / 2.0f * i / m_nSegmentCount;

			pVerts[nVertIndex].x = m_fRadius * (float)cos(alpha);
			pVerts[nVertIndex].y = vHeight;
			pVerts[nVertIndex].z = m_fRadius * (float)sin(alpha);

			if( j != 0 )
				pVerts[nVertIndex].diffuse = 0xffffffff;
			else
				pVerts[nVertIndex].diffuse = 0x00ffffff;

			pVerts[nVertIndex].u1 = 1.0f - 1.0f * i / m_nSegmentCount; 
			if( j != iLatitude-1 )
				pVerts[nVertIndex].v1 = (1.0f - vHeight / h) * 0.5f * 0.9f + 0.05f;
			else
				pVerts[nVertIndex].v1 = 0.5f;

			pVerts[nVertIndex].u2 = 0.0f;
			pVerts[nVertIndex].v2 = 0.0f;

			
			if( i != m_nSegmentCount && j != iLatitude-1 )
			{
				pIndices[nFaceIndex * 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 1] = nVertIndex + is;
				pIndices[nFaceIndex * 3 + 2] = nVertIndex + is + 1;

				pIndices[nFaceIndex * 3 + 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 4] = nVertIndex + is + 1;
				pIndices[nFaceIndex * 3 + 5] = nVertIndex + 1;

				nFaceIndex += 2;
			}

			nVertIndex ++;
		}

		//Right Quadro Sphere;
		for( i=0; i<=m_nSegmentCount; i++ )
		{
			alpha = A3D_PI / 2.0f * i / m_nSegmentCount;

			pVerts[nVertIndex].x = m_fRadius * (float)cos(alpha);
			pVerts[nVertIndex].y = vHeight;
			pVerts[nVertIndex].z = m_fRadius * (float)sin(alpha);
			if( j != 0 )
				pVerts[nVertIndex].diffuse = 0xffffffff;
			else
				pVerts[nVertIndex].diffuse = 0x00ffffff;

			pVerts[nVertIndex].u1 = 1.0f - 1.0f * i / m_nSegmentCount; 
			if( j != iLatitude-1 )
				pVerts[nVertIndex].v1 = (0.5f + vHeight / h * 0.5f) * 0.9f + 0.05f;
			else
				pVerts[nVertIndex].v1 = 0.5f;

			pVerts[nVertIndex].u2 = 0.0f;
			pVerts[nVertIndex].v2 = 0.0f;
			
			if( i != m_nSegmentCount && j != iLatitude-1 )
			{
				pIndices[nFaceIndex * 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 1] = nVertIndex + is;
				pIndices[nFaceIndex * 3 + 2] = nVertIndex + is + 1;

				pIndices[nFaceIndex * 3 + 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 4] = nVertIndex + is + 1;
				pIndices[nFaceIndex * 3 + 5] = nVertIndex + 1;

				nFaceIndex += 2;
			}

			nVertIndex ++;
		}
	}

	m_pStreamSurround->UnlockVertexBuffer();
	m_pStreamSurround->UnlockIndexBuffer();

	return true;
}

bool A3DSkySphere::CreateCap()
{
	// Now calculate the face count and vert count of the hemisphere sky cone part;
	m_nVertCountC = 1 + m_nSegmentCount * (4 * m_nSegmentCount);
	m_nIndexCountC = (4 * m_nSegmentCount + (m_nSegmentCount - 1) * (4 * m_nSegmentCount) * 2) * 3;

	// Then create stream for rendering usage
	m_pStreamCap = new A3DStream();
	if (!m_pStreamCap)
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Not enough memory!");
		return false;
	}
	if (!m_pStreamCap->Init(m_pA3DDevice, sizeof(A3DSKYVERTEX), A3DSKYVERT_FVF, m_nVertCountC, m_nIndexCountC,	A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkySphere::Init(), Init the A3DStream fail!");
		return false;
	}

	A3DSKYVERTEX *	pVerts;
	WORD *			pIndices;
	
	// Fill verts;
	if( !m_pStreamCap->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL) )
		return false;
	
	float r = m_fRadius * (float)sin(m_vPitchRange);
	float h = m_fRadius - m_fRadius * (float)cos(m_vPitchRange);

	// First it is my cap center vertex;
	pVerts[0].x			= 0.0f;
	pVerts[0].y			= m_fRadius;
	pVerts[0].z			= 0.0f;
	pVerts[0].diffuse	= 0xffffffff;

	// Then hemisphere middle part;
	int i, ix, iy;
	int is = m_nSegmentCount * 4; // vert count of each segment
	int nVertIndex = 1;
	for(iy=0; iy<m_nSegmentCount; iy++)
	{
		for(ix=0; ix<4 * m_nSegmentCount; ix++)
		{
			float alpha = A3D_PI * 2.0f * ix / is;
			float pitch = A3D_PI / 2.0f - (iy + 1) * m_vPitchRange / m_nSegmentCount;
			
			float vX = float(m_fRadius * cos(alpha) * cos(pitch));
			float vY = float(m_fRadius * sin(pitch));
			float vZ = float(m_fRadius * sin(alpha) * cos(pitch));

			pVerts[nVertIndex].x = vX;
			pVerts[nVertIndex].y = vY;
			pVerts[nVertIndex].z = vZ;
			pVerts[nVertIndex].diffuse = 0xffffffff;
			
			nVertIndex ++;
		}
	}

	// Now we apply a plain texture maping to it;
	for(i=0; i<m_nVertCountC; i++)
	{
		pVerts[i].u1 = (pVerts[i].x / r + 1.0f) * 0.5f;
		pVerts[i].v1 = (pVerts[i].z / r + 1.0f) * 0.5f;
		pVerts[i].u2 = 0.0f;
		pVerts[i].v2 = 0.0f;
	}

	m_pStreamCap->UnlockVertexBuffer();

	// Fill the indcies;
	if( !m_pStreamCap->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL) )
		return false;
	
	// First conic cap;
	int nFaceIndex = 0;
	for(i=0; i<is; i++)
	{
		pIndices[nFaceIndex * 3] = 0;
		pIndices[nFaceIndex * 3 + 1] = 1 + i;
		pIndices[nFaceIndex * 3 + 2] = 1 + ((i + 1) % is);

		nFaceIndex ++;
	}

	// Then middle part;
	for(iy=0; iy<m_nSegmentCount-1; iy++)
	{
		for(ix=0; ix<is; ix++)
		{
			int index0 = 1 + iy * is + ix;
			int index1 = 1 + iy * is + ((ix + 1) % is);
			int index2 = index0 + is;
			int index3 = index1 + is;

			pIndices[nFaceIndex * 3] = index0;
			pIndices[nFaceIndex * 3 + 1] = index2;
			pIndices[nFaceIndex * 3 + 2] = index3;

			pIndices[nFaceIndex * 3 + 3] = index0;
			pIndices[nFaceIndex * 3 + 4] = index3;
			pIndices[nFaceIndex * 3 + 5] = index1;

			nFaceIndex += 2;
		}
	}
	m_pStreamCap->UnlockIndexBuffer();

	return true;
}

bool A3DSkySphere::NeedTrans(const char * szCap, const char * szSurroundF, const char * szSurroundB, const ANIM_PROP* pDstAnim/* NULL */)
{
	if( IsTransing() || m_bTextureLoading )
		return false;

	char		szCapMapFile[MAX_PATH];
	char		szSurroundFMapFile[MAX_PATH];
	char		szSurroundBMapFile[MAX_PATH];

	sprintf(szCapMapFile, "Textures\\Sky\\%s", szCap);
	sprintf(szSurroundFMapFile, "Textures\\Sky\\%s", szSurroundF);
	sprintf(szSurroundBMapFile, "Textures\\Sky\\%s", szSurroundB);
	// see if trans back to current sky map
	if( m_pTextureCap && m_pTextureSurroundF && m_pTextureSurroundB )
	{
		if (pDstAnim)
		{
			if (m_CurAnimProp.fCapTile != pDstAnim->fCapTile ||
				m_CurAnimProp.fCapSpeedU != pDstAnim->fCapSpeedU ||
				m_CurAnimProp.fCapSpeedV != pDstAnim->fCapSpeedV ||
				m_CurAnimProp.fCapRotSpeed != pDstAnim->fCapRotSpeed)
				return true;
		}

		if( _stricmp(szCapMapFile, m_pTextureCap->GetMapFile()) == 0 &&
			_stricmp(szSurroundFMapFile, m_pTextureSurroundF->GetMapFile()) == 0 &&
			_stricmp(szSurroundBMapFile, m_pTextureSurroundB->GetMapFile()) == 0 )
		{
			return false;
		}
	}

	return true;
}

DWORD WINAPI SkyLoadTextures(LPVOID pArg)
{
	A3DSkySphere * pSky = (A3DSkySphere *) pArg;
	
	if( pSky->m_pTextureCapDest )
	{
		pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSky->m_pTextureCapDest);
	}

	if( !pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(pSky->m_szCapToLoad, "Textures\\Sky", &pSky->m_pTextureCapDest, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::TransSky(), Can't load texture [%s]", pSky->m_szCapToLoad);
		goto ERREXIT;
	}

	if( pSky->m_pTextureSurroundFDest )
	{
		pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSky->m_pTextureSurroundFDest);
	}

	if( !pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(pSky->m_szSurroundFToLoad, "Textures\\Sky", &pSky->m_pTextureSurroundFDest, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::TransSky(), Can't load texture [%s]", pSky->m_szSurroundFToLoad);
		goto ERREXIT;
	}
	
	if( pSky->m_pTextureSurroundBDest )
	{
		pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSky->m_pTextureSurroundBDest);
	}

	if( !pSky->m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(pSky->m_szSurroundBToLoad, "Textures\\Sky", &pSky->m_pTextureSurroundBDest, A3DTF_MIPLEVEL, 1) )
	{
		g_A3DErrLog.Log("A3DSkySphere::TransSky(), Can't load texture [%s]", pSky->m_szSurroundBToLoad);
		goto ERREXIT;
	}
	
	pSky->m_bTextureLoading = false;
	return 0;

ERREXIT:
	pSky->m_bTextureLoading = false;
	return -1;
}

bool A3DSkySphere::TransSky(const char * szCap, const char * szSurroundF, const char * szSurroundB, int nTransTime/*in millisecond*/, const ANIM_PROP* pDstAnim/* NULL */)
{
	if( !m_pA3DDevice || m_bTextureLoading ) return true;

	m_bTransToDest = true;

	char szCapMapFile[MAX_PATH];
	char szSurroundFMapFile[MAX_PATH];
	char szSurroundBMapFile[MAX_PATH];

	sprintf(szCapMapFile, "Textures\\Sky\\%s", szCap);
	sprintf(szSurroundFMapFile, "Textures\\Sky\\%s", szSurroundF);
	sprintf(szSurroundBMapFile, "Textures\\Sky\\%s", szSurroundB);
	// see if trans back to current sky map
	if( m_pTextureCap && m_pTextureSurroundF && m_pTextureSurroundB )
	{
		if( _stricmp(szCapMapFile, m_pTextureCap->GetMapFile()) == 0 &&
			_stricmp(szSurroundFMapFile, m_pTextureSurroundF->GetMapFile()) == 0 &&
			_stricmp(szSurroundBMapFile, m_pTextureSurroundB->GetMapFile()) == 0 )
		{
			if (pDstAnim)
				m_CurAnimProp = *pDstAnim;

			m_bTransToDest = false;
			return true;
		}
	}

	if( m_pTextureCapDest && m_pTextureSurroundFDest && m_pTextureSurroundBDest )
	{
		if( _stricmp(szCapMapFile, m_pTextureCapDest->GetMapFile()) == 0 &&
			_stricmp(szSurroundFMapFile, m_pTextureSurroundFDest->GetMapFile()) == 0 &&
			_stricmp(szSurroundBMapFile, m_pTextureSurroundBDest->GetMapFile()) == 0 )
		{
			if (pDstAnim)
				m_DstAnimProp = *pDstAnim;

			// continue with current dest map
			m_bTransToDest = true;
			return true;
		}
	}

	if (pDstAnim)
		m_DstAnimProp = *pDstAnim;
	else
		m_DstAnimProp = m_CurAnimProp;

	strncpy(m_szCapToLoad, szCap, MAX_PATH);
	strncpy(m_szSurroundFToLoad, szSurroundF, MAX_PATH);
	strncpy(m_szSurroundBToLoad, szSurroundB, MAX_PATH);

	m_nTransTime = max2(nTransTime, 0);
	m_nTransTickLeft = m_nTransTime;

	// now we should load sky textures in a thread
	if( m_nTransTime == 0 )
	{
		m_bTextureLoading = true;
		if( -1 == SkyLoadTextures(this) )
			return false;
		SwitchClouds();
	}
	else
	{
		HANDLE hThread;
		DWORD  dwThreadID;
		m_bTextureLoading = true;
		hThread = CreateThread(NULL, 0, SkyLoadTextures, this, 0, &dwThreadID);
	}

	return true;
}

bool A3DSkySphere::SetTimePassed(int nDeltaTime)
{
	if( m_nTransTickLeft == 0 )
		return true;

	m_nTransTickLeft -= nDeltaTime;

	if( m_nTransTickLeft <= 0 )
		m_nTransTickLeft = 1; // don't set it to zero, because we want tickanimation to do switchclouds.

	TickAnimation();
	return true;
}

bool A3DSkySphere::SwitchClouds()
{
	if( m_bTransToDest )
	{
		// first release old textures;
		if( m_pTextureCap )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
			m_pTextureCap = NULL;
		}
		if( m_pTextureSurroundF )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
			m_pTextureSurroundF = NULL;
		}
		if( m_pTextureSurroundB )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
			m_pTextureSurroundB = NULL;
		}

		// then take down new textures;
		m_pTextureCap = m_pTextureCapDest;
		m_pTextureSurroundF = m_pTextureSurroundFDest;
		m_pTextureSurroundB = m_pTextureSurroundBDest;
		m_pTextureCapDest = NULL;
		m_pTextureSurroundFDest = NULL;
		m_pTextureSurroundBDest = NULL;

		m_CurAnimProp = m_DstAnimProp;
		m_tmCurCapCloud = m_tmDstCapCloud;
	}
	else
	{
		// release current trans dest maps
		if( m_pTextureCapDest )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCapDest);
			m_pTextureCapDest = NULL;
		}
		if( m_pTextureSurroundFDest )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundFDest);
			m_pTextureSurroundFDest = NULL;
		}
		if( m_pTextureSurroundBDest )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundBDest);
			m_pTextureSurroundBDest = NULL;
		}
	}

	return true;
}

void A3DSkySphere::RenderHLSL()
{
    if (!m_pVertDecl)
        return;
    m_pVertDecl->Appear();
    if (m_nTransTickLeft && !m_bTextureLoading)
    {
        float fAlpha = (1.0f - m_nTransTickLeft * 1.0f /  m_nTransTime);
        RenderCapHLSL(m_pTextureCap, m_pTextureCapDest, fAlpha);
        RenderSurroundHLSL(m_pTextureSurroundF, m_pTextureSurroundFDest, m_pTextureSurroundB, m_pTextureSurroundBDest, fAlpha);
    }
    else
    {
        RenderCapHLSL(m_pTextureCap, m_pTextureCap, 0);
        RenderSurroundHLSL(m_pTextureSurroundF, m_pTextureSurroundF, m_pTextureSurroundB, m_pTextureSurroundB, 0);
    }
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->ClearVertexShader();
}

A3DTexture* A3DSkySphere::SafeTexture(A3DTexture* pTexture)
{
    if (pTexture && pTexture->GetD3DTexture())
        return pTexture;
    else
        return m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->GetErrorTexture();
}