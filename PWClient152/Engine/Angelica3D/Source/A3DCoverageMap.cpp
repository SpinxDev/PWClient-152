/*
 * FILE: A3DCoverageMap.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: yaochunhui, 2012/2/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCoverageMap.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DHLSL.h"
#include "A3DCamera.h"
#include "A3DRenderTarget.h"
#include "A3DStream.h"
#include "A3DVertexShader.h"
#include "A3DEnvironment.h"

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

static const D3DVERTEXELEMENT9 s_aVertexDecl[] =
{
    {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},
    {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DCoverageMap::A3DCoverageMap()
{
    m_pDevice = NULL;
    m_pHLSLMan = NULL;
    m_pSmallDepthBuffer = NULL;
    m_nMipmapCount = 0;
    m_pMipmapArray = NULL;
    m_iFinalWidth = 0;
    m_iFinalHeight = 0;
    m_pOffSurface = NULL;
    m_pHLSLMaxDownSample = NULL;
    m_pHLSLWriteDepth = NULL;
    m_pHLSLShowDepth = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    m_pStreamQuad = NULL;
    m_pVertexDecl = NULL;
}

A3DCoverageMap::~A3DCoverageMap()
{
    Release();
}

bool A3DCoverageMap::Init(A3DEngine* pEngine)
{
    Release();

    m_pDevice = pEngine->GetA3DDevice();
    m_pHLSLMan = pEngine->GetA3DHLSLMan();
    
    if (!m_pHLSLMan)
        return false;

    m_pDevice->AddUnmanagedDevObject(this);
    
    AString strShaderFile = "shaders\\2.2\\HLSL\\Occlusion\\CoverageMap.hlsl";
    A3DCCDBinder* pBinder = m_pDevice->GetA3DEngine()->GetA3DEnvironment()->GetCCDBinder();

    m_pHLSLMaxDownSample = m_pHLSLMan->LoadShader(NULL, NULL, strShaderFile, "ps_maxDownSample", "", 0);
    if (!m_pHLSLMaxDownSample)
    {
        g_A3DErrLog.Log("A3DCoverageMap::Init, Failed to load maxDownSample shader!");
        return false;
    }
    m_pHLSLMaxDownSample->BindCommConstData(pBinder);

    m_pHLSLWriteDepth = m_pHLSLMan->LoadShader(NULL, NULL, strShaderFile, "ps_writeDepth", "", 0);
    if (!m_pHLSLWriteDepth)
    {
        g_A3DErrLog.Log("A3DCoverageMap::Init, Failed to load writeDepth shader!");
        return false;
    }
    m_pHLSLWriteDepth->BindCommConstData(pBinder);

    m_pHLSLShowDepth = m_pHLSLMan->LoadShader(NULL, NULL, strShaderFile, "ps_showDepth", "", 0);
    if (!m_pHLSLShowDepth)
    {
        g_A3DErrLog.Log("A3DCoverageMap::Init, Failed to load ps_showDepth shader!");
        return false;
    }
    m_pHLSLShowDepth->BindCommConstData(pBinder);

    if (!CreateStream())
    {
        g_A3DErrLog.Log("A3DCoverageMap::Init, Failed to create quad stream!");
        return false;
    }

    m_pVertexDecl = new A3DVertexDecl;
    if (!m_pVertexDecl->Init(m_pDevice, s_aVertexDecl))
    {
        g_A3DErrLog.Log("A3DCoverageMap::Init, Failed to create vertex declaration!");
        return false;
    }
    return true;
}

void A3DCoverageMap::Release()
{
    if (m_pDevice)
    {
        m_pDevice->RemoveUnmanagedDevObject(this);
    }
    m_pDevice = NULL;

    ReleaseResources();
    if (m_pHLSLMan)
    {
        m_pHLSLMan->ReleaseShader(m_pHLSLMaxDownSample);
        m_pHLSLMan->ReleaseShader(m_pHLSLWriteDepth);
        m_pHLSLMan->ReleaseShader(m_pHLSLShowDepth);
    }
    m_pHLSLWriteDepth = NULL;
    m_pHLSLMaxDownSample = NULL;
    m_pHLSLMan = NULL;
    A3DRELEASE(m_pStreamQuad);
    A3DRELEASE(m_pVertexDecl);
}

bool A3DCoverageMap::BeforeDeviceReset()
{
    //ReleaseResources();
    return true;
}

bool A3DCoverageMap::AfterDeviceReset()
{
    return true;
}

bool A3DCoverageMap::GenerateMipmap(A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture,
                                    bool bWriteDepthBuffer)
{
    // if need re-create resources
    if (pDepthRenderTarget->GetWidth() != m_iWidth || pDepthRenderTarget->GetHeight() != m_iHeight)
    {
        ReleaseResources();
        m_iWidth = pDepthRenderTarget->GetWidth();
        m_iHeight = pDepthRenderTarget->GetHeight();
        CreateResources();
    }

    if (!m_pHLSLMaxDownSample)
        return false;


    bool bAlphaEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) != 0;
    bool bAlphaTestEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ALPHATESTENABLE) != 0;
    bool bZEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ZENABLE) != 0;
    bool bZWriteEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE) != 0;

    m_pDevice->SetAlphaBlendEnable(false);
    m_pDevice->SetAlphaTestEnable(false);
    m_pDevice->SetZTestEnable(false);
    m_pDevice->SetZWriteEnable(false);

    m_pHLSLMaxDownSample->Appear(APPEAR_SETSHADERONLY);

    int iWidth = m_iWidth;
    int iHeight = m_iHeight;
    m_pVertexDecl->Appear();
    for (int i = 0; i < m_nMipmapCount; i++)
    {
        iWidth /= 2;
        iHeight /= 2;
        if (iWidth == 0) iWidth = 1;
        if (iHeight == 0) iHeight = 1;

        float fOffsetX = 0.25f / iWidth;
        float fOffsetY = 0.25f / iHeight;
        float fOffsets[8] = { fOffsetX, fOffsetY, -fOffsetX, fOffsetY, fOffsetX, -fOffsetY, -fOffsetX, -fOffsetY};
        
        m_pHLSLMaxDownSample->SetConstantArrayF("g_PixelOffset", (A3DVECTOR4*)fOffsets, 2);
        
        m_pMipmapArray[i]->ApplyToDevice(0);

        if (i == 0)
            m_pHLSLMaxDownSample->SetTextureFromRT("g_DepthSampler", pDepthRenderTarget, bUseDepthTexture);
        else
            m_pHLSLMaxDownSample->SetTextureFromRT("g_DepthSampler", m_pMipmapArray[i - 1], false);
        
        m_pHLSLMaxDownSample->Commit(m_pDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
        DrawQuad(0, 0, iWidth, iHeight);

        m_pMipmapArray[i]->WithdrawFromDevice();
    }
    
    if (bWriteDepthBuffer)
        WriteSmallDepthBuffer(m_pMipmapArray[m_nMipmapCount - 1]);

    m_pDevice->SetAlphaTestEnable(bAlphaTestEnableOld);
    m_pDevice->SetAlphaBlendEnable(bAlphaEnableOld);
    m_pDevice->SetZTestEnable(bZEnableOld);
    m_pDevice->SetZWriteEnable(bZWriteEnableOld);

    return true;
}

bool A3DCoverageMap::ReadbackSmallBuffer(float* pBuf, int iPitch)
{
//    LPDIRECT3DDEVICE9 pD3DDevice = m_pDevice->GetD3DDevice();

    // Transfer render target surface to system memory surface
    LPDIRECT3DSURFACE9 pSurface = m_pMipmapArray[m_nMipmapCount - 1]->GetTargetSurface()->m_pD3DSurface;

    if (FAILED(m_pDevice->GetRenderTargetData(pSurface, m_pOffSurface)))
    {
        pSurface->Release();
        g_A3DErrLog.Log("A3DCoverageMap::ReadbackSmallBuffer: GetRenderTargetData Error");
        return false;
    }

    // Lock and copy data from system memory surface
    D3DLOCKED_RECT rt;
    m_pOffSurface->LockRect(&rt, NULL, D3DLOCK_READONLY);

    char* pDstBuf = (char*)pBuf;
    char* pSrcBuf = (char*)rt.pBits; 
    for (int iLine = 0; iLine < m_iFinalHeight; iLine++)
    {
        memcpy(pDstBuf, pSrcBuf, m_iFinalWidth * sizeof(float));
        pDstBuf += iPitch;
        pSrcBuf += rt.Pitch;
    }
    m_pOffSurface->UnlockRect();
    return true;
}

void A3DCoverageMap::ShowDepthMap(A3DRenderTarget* pDepthTexture, A3DCameraBase* pCamera, bool bUseDepthTexture)
{
    if (!m_pHLSLShowDepth)
        return;

    m_pHLSLShowDepth->SetTextureFromRT("g_DepthSampler", pDepthTexture, bUseDepthTexture);
    m_pHLSLShowDepth->Appear(m_pDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());

    m_pDevice->SetAlphaBlendEnable(false);
    m_pDevice->SetAlphaTestEnable(false);
    m_pDevice->SetZTestEnable(false);
    m_pDevice->SetZWriteEnable(false);

    float fZFar = pCamera->GetZBack();
    float fZNear = pCamera->GetZFront();

    int iWidth = m_pDevice->GetD3DPP().BackBufferWidth;
    int iHeight = m_pDevice->GetD3DPP().BackBufferHeight;
    m_pVertexDecl->Appear();
    DrawQuad(0, 0, iWidth, iHeight);
}

bool A3DCoverageMap::CreateResources()
{
    int iWidth = m_iWidth;
    int iHeight = m_iHeight;
    m_nMipmapCount = (int)(log(m_iWidth / 129.0) / log(2.0));
    if (m_nMipmapCount < 1) m_nMipmapCount = 1;
    m_pMipmapArray = new A3DRenderTarget*[m_nMipmapCount];

    A3DRenderTarget::RTFMT fmtRT;
    memset(&fmtRT, 0, sizeof(A3DRenderTarget::RTFMT));
    fmtRT.fmtTarget = (A3DFORMAT)D3DFMT_R32F;
    fmtRT.fmtDepth = (A3DFORMAT)D3DFMT_UNKNOWN;

    for (int i = 0; i < m_nMipmapCount; i++)
    {
        iWidth /= 2;
        iHeight /= 2;
        if (iWidth == 0) iWidth = 1;
        if (iHeight == 0) iHeight = 1;
        m_pMipmapArray[i] = new A3DRenderTarget;
        fmtRT.iWidth = iWidth;
        fmtRT.iHeight = iHeight;
        if (!m_pMipmapArray[i]->Init(m_pDevice, fmtRT, true, false))
        {
            g_A3DErrLog.Log("A3DCoverageMap::CreateResources, Failed to create texture!");
            A3DRELEASE(m_pMipmapArray[i]);
        }
    }
    m_iFinalWidth = iWidth;
    m_iFinalHeight = iHeight;


    fmtRT.fmtTarget = A3DFMT_A8R8G8B8;
    fmtRT.fmtDepth = A3DFMT_INTZ;
    m_pSmallDepthBuffer = new A3DRenderTarget;
    if (!m_pSmallDepthBuffer->Init(m_pDevice, fmtRT, true, true))
    {
        g_A3DErrLog.Log("A3DCoverageMap::CreateResources, Failed to create small depth buffer texture!");
        A3DRELEASE(m_pSmallDepthBuffer);
    }

	m_pOffSurface = m_pDevice->CreateSystemMemSurface(m_iFinalWidth, m_iFinalHeight, A3DFMT_R32F);
    if (m_pOffSurface == NULL)
    {
        g_A3DErrLog.Log("A3DCoverageMap::CreateResources, Failed to create off-screen surface!");
    }

    return true;
}

void A3DCoverageMap::ReleaseResources()
{
    A3DRELEASE(m_pSmallDepthBuffer);

    for (int i = 0; i < m_nMipmapCount; i++)
    {
        A3DRELEASE(m_pMipmapArray[i]);
    }
    delete[] m_pMipmapArray;
    m_pMipmapArray = NULL;
    m_nMipmapCount = 0;
    if (m_pOffSurface)
    {
        m_pOffSurface->Release();
    }
    m_pOffSurface = NULL;

    m_iWidth = 0;
    m_iHeight = 0;
}

bool A3DCoverageMap::WriteSmallDepthBuffer(A3DRenderTarget* pDepthTexture)
{
    if (!m_pHLSLWriteDepth)
        return false;
    
    m_pHLSLWriteDepth->SetTextureFromRT("g_DepthSampler", pDepthTexture, false);
    m_pHLSLWriteDepth->Appear(m_pDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());

    m_pSmallDepthBuffer->ApplyToDevice();

    m_pDevice->Clear(D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

    m_pDevice->SetZTestEnable(true);
    m_pDevice->SetZWriteEnable(true);
    m_pDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0);
    
    m_pVertexDecl->Appear();
    DrawQuad(0, 0, m_iFinalWidth, m_iFinalHeight);

    m_pSmallDepthBuffer->WithdrawFromDevice();
    
    m_pDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0xF);
    return true;
}

void A3DCoverageMap::DrawQuad(int x, int y, int w, int h)
{
    RECT rcScreen = {x, y, x + w, y + h};
    struct VERTEX_XYZRHWTEX1
    {
        FLOAT x, y, z;
        FLOAT rhw;
        FLOAT tu, tv;   
    };

    VERTEX_XYZRHWTEX1 pVertices[4] =
    {
        (float) rcScreen.left - 0.5f,  (float) rcScreen.top - 0.5f,    0.5f, 1.0f, 0, 0,
        (float) rcScreen.right - 0.5f, (float) rcScreen.top - 0.5f,    0.5f, 1.0f, 1, 0, 
        (float) rcScreen.right - 0.5f, (float) rcScreen.bottom - 0.5f, 0.5f, 1.0f, 1, 1, 
        (float) rcScreen.left - 0.5f,  (float) rcScreen.bottom - 0.5f, 0.5f, 1.0f, 0, 1,
    };
   
    BYTE* pDst;
    if (!m_pStreamQuad->LockVertexBuffer(0, sizeof(VERTEX_XYZRHWTEX1) * 4, &pDst, 0))
        return;
    memcpy(pDst, pVertices, sizeof(VERTEX_XYZRHWTEX1) * 4);
    m_pStreamQuad->UnlockVertexBuffer();
    m_pStreamQuad->AppearVertexOnly(0, false);
    m_pDevice->DrawPrimitive(A3DPT_TRIANGLEFAN, 0, 2);
}

A3DRenderTarget* A3DCoverageMap::GetSmallDepthBuffer() const
{
    return m_pSmallDepthBuffer;
}

bool A3DCoverageMap::CreateStream()
{
    struct VERTEX_XYZRHWTEX1
    {
        FLOAT x, y, z;
        FLOAT rhw;
        FLOAT tu, tv;   
    };
    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

    m_pStreamQuad = new A3DStream;
    if (!m_pStreamQuad->Init(m_pDevice, sizeof(VERTEX_XYZRHWTEX1), FVF, 4, 0, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
    {
        A3DRELEASE(m_pStreamQuad);
        g_A3DErrLog.Log("A3DCoverageMap::CreateStream, Failed to create quad stream!");
        return false;
    }
    
    return true;
}