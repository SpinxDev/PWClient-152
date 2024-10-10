#include "StdAfx.h"
#include "A3DGFXSurfaceRender.h"
#include "A3DStream.h"
#include "A3DOrthoCamera.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"

#ifdef _ANGELICA21
static const D3DVERTEXELEMENT9 s_aVertexDecl[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
};
#elif defined(_ANGELICA22)
#include "A3DHLSL.h"
#include "A3DEnvironment.h"
#include "A3DVertexShader.h"
static const D3DVERTEXELEMENT9 s_aVertexDecl[] =
{
    {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    D3DDECL_END()
};
#endif

A3DGFXSurfaceRender::A3DGFXSurfaceRender()
{
	m_bInit		= false;
	m_pViewport	= 0;
	m_pCamera	= 0;
	m_pStream	= 0;
    m_pNoTex    = NULL;
	m_pVS		= NULL;
	m_pPS		= NULL;
#ifdef _ANGELICA22
    m_pHLSL     = NULL;
    m_pVertDecl = NULL;
#endif
}

A3DGFXSurfaceRender::~A3DGFXSurfaceRender()
{
}

bool A3DGFXSurfaceRender::Init()
{
	A3DDevice* pDevice = AfxGetA3DDevice();
	m_pCamera = new A3DOrthoCamera();

	if (!m_pCamera->Init(pDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f))
		return false;

	RECT rc;
	::GetClientRect(pDevice->GetDeviceWnd(), &rc);
	DWORD w = (rc.right - rc.left);
	DWORD h = (rc.bottom - rc.top);

	if (!pDevice->CreateViewport(&m_pViewport, 0, 0, w, h, 0.0f, 1.0f, true, true, 0))
		return false;

	m_pViewport->SetCamera(m_pCamera);
	m_pCamera->SetDirAndUp(-_unit_y, _unit_z);
	m_pStream = new A3DStream;

	if (!m_pStream->Init(pDevice, sizeof(GFX_SURFACEVERTEX), GFXSURFACEVERT_FVF, _max_verts, _max_indices, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
		return false;

#ifdef _ANGELICA21
	m_pVS = pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\hlsl\\gfxsurface.hlslv", false);
	m_pVS->SetDecl(s_aVertexDecl);
	m_pPS = pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfxsurface.hlslp", false);
#elif defined(_ANGELICA22)
    m_pHLSL = pDevice->GetA3DEngine()->GetA3DHLSLMan()->LoadShader(SHADERS_ROOT_DIR"HLSL\\gfx\\decal.hlsl", SHADERS_ROOT_DIR"HLSL\\gfx\\decal.hlsl", "_FOG_");
    A3DEnvironment* pEnvironment = pDevice->GetA3DEngine()->GetA3DEnvironment();	// 取环境对象
    A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();			// 取环境对象的绑定对象
    if (m_pHLSL) 
        m_pHLSL->BindCommConstData(pBinder);
    else
        a_LogOutput(1, "A3DGFXSurfaceRender::Init, Failed to load shader!");
    m_pVertDecl = new A3DVertexDecl;
    if (!m_pVertDecl || !m_pVertDecl->Init(pDevice, s_aVertexDecl))
    {
        a_LogOutput(1, "A3DGFXSurfaceRender::Init, Failed to create vertex declaration!");
        return false;
    }
#endif

    m_pNoTex = A3DTexture::CreateColorTexture(pDevice, 4, 4, A3DCOLORRGB(255, 255, 255));
	m_bInit = true;
	return true;
}

void A3DGFXSurfaceRender::Release()
{
	m_bInit = false;
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pStream);
    A3DRELEASE(m_pNoTex);

	if (m_pVS)
	{
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pVS);
		m_pVS = NULL;
	}

	if (m_pPS)
	{
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pPS);
		m_pPS = NULL;
	}

#ifdef _ANGELICA22
    if (m_pHLSL)
    {
        AfxGetA3DDevice()->GetA3DEngine()->GetA3DHLSLMan()->ReleaseShader(m_pHLSL);
        m_pHLSL = NULL;
    }
    A3DRELEASE(m_pVertDecl);
#endif
}

bool A3DGFXSurfaceRender::SetupSteam(A3DVECTOR3* pVerts, int nVertCount, WORD* pIndices, int nIndexCount, A3DCOLOR diffuse)
{
	GFX_SURFACEVERTEX* pv;
	WORD* pi;
	if (!m_pStream->LockVertexBufferDynamic(0, 0, (BYTE**)&pv, 0))
		return false;

    int i;

	for (i = 0; i < nVertCount; i++)
	{
		GFX_SURFACEVERTEX& vert = pv[i];
		vert.pos = pVerts[i];
		vert.pos.y += .02f;
		vert.diffuse = diffuse;
	}

	m_pStream->UnlockVertexBufferDynamic();

	if (!m_pStream->LockIndexBufferDynamic(0, 0, (BYTE**)&pi, 0))
		return false;

	for (i = 0; i < nIndexCount; i++)
		pi[i] = pIndices[i];

    m_pStream->UnlockIndexBufferDynamic();

    return true;
}

bool A3DGFXSurfaceRender::Render(
	A3DViewport* pViewport,
	A3DVECTOR3* pVerts,
	int nVertCount,
	WORD* pIndices,
	int nIndexCount,
	const A3DVECTOR3& vCenter,
	float fHalfWidth,
	float fHalfHeight,
	float fRadius,
	A3DTexture* pTex,
	A3DBLEND SrcBlend,
	A3DBLEND DestBlend,
	A3DCOLOR diffuse,
	const A3DVECTOR3& vDir,
	bool bUReverse, 
	bool bVReverse, 
	bool bUVInterChange)
{
	if (!m_bInit)
		return false;

	if (!SetupSteam(pVerts, nVertCount, pIndices, nIndexCount, diffuse))
		return false;

	A3DMATRIX4 matProjectedView;
	A3DMATRIX4 matScale = a3d_IdentityMatrix();
	// 默认以前都为False
	matScale._11 = bUReverse ? -0.5f : 0.5f;	// 这里原本就是0.5f, 而_22则原本是-0.5f，导致了UV两种Reverse的解释不一致了
	matScale._22 = bVReverse ? 0.5f : -0.5f;	// 原本这里就是-0.5f，那么当加入对VReverse的支持时，只能把0.5作为reverse了
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;

	if (bUVInterChange)
	{
		A3DMATRIX4 matInterChangeUV = a3d_ZeroMatrix();
		matInterChangeUV._12 = 1.0f;
		matInterChangeUV._21 = 1.0f;
		matInterChangeUV._33 = 1.0f;
		matInterChangeUV._44 = 1.0f;
		matScale = matScale * matInterChangeUV;
	}

	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	m_pCamera->SetProjectionParam(-fHalfWidth, fHalfWidth, -fHalfHeight, fHalfHeight, -100.0f, 100.0f);
	m_pCamera->SetPos(vCenter + A3DVECTOR3(0, fRadius * 2.0f, 0));
	m_pCamera->SetDirAndUp(-_unit_y, vDir);

	pViewport->Active();
	bool bFog = pA3DDevice->GetFogEnable();
	A3DCULLTYPE cull = pA3DDevice->GetFaceCull();
	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetFogEnable(false);
	pA3DDevice->SetFaceCull(A3DCULL_NONE);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetWorldMatrix(_identity_mat4);
	pA3DDevice->SetSourceAlpha(SrcBlend);
	pA3DDevice->SetDestAlpha(DestBlend);
	bool bAlphaBlend = pA3DDevice->GetAlphaBlendEnable();
	pA3DDevice->SetAlphaBlendEnable(true);

	if (AfxGetGFXExMan()->UsingZBias())
		pA3DDevice->SetZBias(3);

	pA3DDevice->SetTextureAddress(0, A3DTADDR_BORDER, A3DTADDR_BORDER);

#ifdef _ANGELICA21

	pA3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
    m_pStream->Appear(0, false);
	A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
	matVP.Transpose();
	pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);
	A3DMATRIX4 matProj = m_pCamera->GetVPTM() * matScale;
	matProj.Transpose();
	pA3DDevice->SetVertexShaderConstants(4, &matProj, 4);
	m_pVS->Appear();
	m_pPS->Appear();

#elif defined(_ANGELICA22)
	A3DCameraBase * pHostCamera = pViewport->GetCamera();
	matProjectedView = InverseTM(pHostCamera->GetViewTM()) * m_pCamera->GetVPTM() * matScale;
	pA3DDevice->SetTextureCoordIndex(0, D3DTSS_TCI_CAMERASPACEPOSITION);
	pA3DDevice->SetTextureTransformFlags(0, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	pA3DDevice->SetTextureMatrix(0, matProjectedView);
	pA3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
    m_pStream->Appear(0, false);
    m_pVertDecl->Appear();
    if (m_pHLSL)
	{
		pA3DDevice->SetTextureCoordIndex(0, 0); //为了避免D3D Debug模式爆出Error
		ApplyHLSL(pViewport, SrcBlend, DestBlend, matScale);
	}
#endif
    if (pTex)
        pTex->Appear();
    else if (m_pNoTex)
        m_pNoTex->Appear();

	pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nIndexCount / 3);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->ClearTexture(0);
	pA3DDevice->SetFaceCull(cull);
	pA3DDevice->SetFogEnable(bFog);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);

	if (AfxGetGFXExMan()->UsingZBias())
		pA3DDevice->SetZBias(0);

#ifdef _ANGELICA21
	pA3DDevice->ClearVertexShader();
	pA3DDevice->ClearPixelShader();
#elif defined(_ANGELICA22)
	pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureCoordIndex(0, 0);
    if (m_pHLSL)
        m_pHLSL->Disappear();
#endif
	return true;
}

#ifdef _ANGELICA22
bool A3DGFXSurfaceRender::ApplyHLSL(A3DViewport* pViewport, A3DBLEND SrcBlend, A3DBLEND DestBlend, const A3DMATRIX4& matScale)
{
    A3DMATRIX4 matProjectedView = matProjectedView = m_pCamera->GetVPTM() * matScale;
    A3DDevice* pA3DDevice = AfxGetA3DDevice();
    pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
    A3DMATRIX4 matViewProj = pA3DDevice->GetViewMatrix() * pA3DDevice->GetProjectionMatrix();
    m_pHLSL->SetConstantMatrix("g_matViewProj", matViewProj);
    m_pHLSL->SetConstantMatrix("g_matTexture", matProjectedView);
    m_pHLSL->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());
    A3DEnvironment* pEnv = pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
    A3DEnvironment::FOG oldFog = *pEnv->GetFogParam();

    bool bIsAdditive = DestBlend == A3DBLEND_ONE || (SrcBlend == A3DBLEND_ONE && DestBlend != A3DBLEND_ZERO);
    if (bIsAdditive)
    {
        A3DEnvironment::FOG fog = *pEnv->GetFogParam();
        fog.crFog1 &= 0xFF000000;
        fog.crFog2 &= 0xFF000000;
        if (pEnv->GetIsUnderWater())
            pEnv->SetFogParamUnderWater(&fog);
        else
            pEnv->SetFogParamAboveWater(&fog);
    }
    m_pHLSL->Appear(pEnv->GetCommonConstTable());
    if (bIsAdditive)
    {
        if (pEnv->GetIsUnderWater())
            pEnv->SetFogParamUnderWater(&oldFog);
        else
            pEnv->SetFogParamAboveWater(&oldFog);
    }

    return true;
}
#endif