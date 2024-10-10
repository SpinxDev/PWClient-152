/*
 * FILE: A3DOcclusionManHW.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: yaochunhui, 2012/2/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DOcclusionManHW.h"
#include "A3DCoverageMap.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "A3DDevObject.h"
#include "A3DStream.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "ACSWrapper.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

class A3DOcclusionProxyHW : public A3DOcclusionProxy
{
public:
    A3DOcclusionProxyHW(A3DOcclusionManHW* pMan);
    ~A3DOcclusionProxyHW();

    virtual void SetAABB(const A3DAABB& aabb);
    virtual bool IsOccluded(A3DViewport* pViewport);
    virtual bool GetEnabled() const { return m_bEnabled; };
    virtual void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

    bool Init(A3DDevice* pDevice);
    void Release();

public:
    A3DAABB m_bbAABB;
    bool m_bEnabled;
    LPDIRECT3DQUERY9 m_pD3DQuery;
    bool m_bNeedQuery;
    bool m_bOccluded;
    bool m_bNeedTest;
    DWORD m_nQueryFrameIndex;
    DWORD m_nResultFrameIndex;
    A3DDevice* m_pDevice;
    A3DOcclusionManHW* m_pMan;
};

A3DOcclusionProxyHW::A3DOcclusionProxyHW(A3DOcclusionManHW* pMan)
{
    m_pDevice = NULL;
    m_pD3DQuery = NULL;
    m_pMan = pMan;
    m_bEnabled = true;
    m_bNeedQuery = true;
    m_bOccluded = false;
    m_bNeedTest = true;
    m_nResultFrameIndex = 0;
    m_nQueryFrameIndex = 0;
}

A3DOcclusionProxyHW::~A3DOcclusionProxyHW()
{
    Release();
}

void A3DOcclusionProxyHW::SetAABB(const A3DAABB& aabb)
{
    m_bbAABB = aabb;
    m_bbAABB.Extents += A3DVECTOR3(0.5f, 0.5f, 0.5f);
    m_bbAABB.CompleteMinsMaxs();
}

bool A3DOcclusionProxyHW::Init(A3DDevice* pDevice)
{
    ACSWrapper csa(&m_pMan->m_cs);

    Release();
    m_pDevice = pDevice;
    HRESULT hr = m_pDevice->GetD3DDevice()->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pD3DQuery);
    if (hr == D3DERR_DEVICELOST)
    {
        return false;
    }
    if (FAILED(hr))
    {
        g_A3DErrLog.Log("A3DOcclusionProxyHW::Init, Failed to create occlusion query");
        return false;
    }
    m_bNeedTest = true;
    return true;
}

void A3DOcclusionProxyHW::Release()
{
    ACSWrapper csa(&m_pMan->m_cs);

    if (m_pD3DQuery)
    {
        m_pD3DQuery->Release();
        m_pD3DQuery = NULL;
    }
}

bool A3DOcclusionProxyHW::IsOccluded(A3DViewport* pViewport)
{
    if (!m_bEnabled || !m_pD3DQuery || pViewport != m_pMan->m_pViewport)
        return false;

    if (!m_bNeedQuery)
    {
        m_bNeedTest = true;
        return m_bOccluded;
    }

    DWORD nCurFrame = m_pMan->m_nFrameCount;

    DWORD dwResult = 0;

    HRESULT hr = m_pD3DQuery->GetData(&dwResult, sizeof(DWORD), D3DGETDATA_FLUSH);
    
    if (hr == S_FALSE)
    {
        if (nCurFrame - m_nResultFrameIndex < 5)
        {
            return m_bOccluded;
        }
        else
        {
            m_bNeedTest = true;
            return false;
        }
    }
    else if (hr == S_OK)
    {
        m_bNeedQuery = false;
        m_bOccluded = (dwResult == 0);
        m_nResultFrameIndex = m_nQueryFrameIndex;
        m_bNeedTest = true;

        return m_bOccluded;
    }
    else if (hr != D3DERR_DEVICELOST)
    {
        g_A3DErrLog.Log("A3DOcclusionProxyHW::IsOccluded, Failed to get occlusion data, hr = %x", hr);
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//
//   A3DOcclusionManHW
//
//////////////////////////////////////////////////////////////////////////

A3DOcclusionManHW::A3DOcclusionManHW()
{
    m_pDevice = NULL;
    m_pCoverageMap = NULL;
    m_pCamera = NULL;
    m_pStreamAABB = NULL;
    m_nPassed = 0;
    m_nAvailable = 0;
    m_pViewport = NULL;
    m_bCoverageMapReady = false;
    m_iWidth = 0;
    m_iHeight = 0;
    m_nFrameCount = 0;

    InitializeCriticalSection(&m_cs);
}

A3DOcclusionManHW::~A3DOcclusionManHW()
{
    Release();

    DeleteCriticalSection(&m_cs);
}

bool A3DOcclusionManHW::Init(A3DEngine* pEngine)
{
    m_pDevice = pEngine->GetA3DDevice();

    if (!CheckSupport())
        return false;

    m_pDevice->AddUnmanagedDevObject(this);
    m_pStreamAABB = new A3DStream;
    m_pCoverageMap = new A3DCoverageMap();
    m_pCoverageMap->Init(pEngine);
    CreateAABBStream();
    m_bCoverageMapReady = false;
    m_nFrameCount = 100;

    return true;
}

void A3DOcclusionManHW::Release()
{
    Clear();
    if (m_pDevice)
    {
        m_pDevice->RemoveUnmanagedDevObject(this);
        m_pDevice = NULL;
    }
    A3DRELEASE(m_pCoverageMap);
    A3DRELEASE(m_pStreamAABB);
}

A3DOcclusionProxy* A3DOcclusionManHW::CreateQuery()
{
    ACSWrapper csa(&m_cs);

    A3DOcclusionProxyHW* pProxy = new A3DOcclusionProxyHW(this);
    pProxy->Init(m_pDevice);
    m_aQueries.Add(pProxy);
    return pProxy;
}

void A3DOcclusionManHW::ReleaseQuery(A3DOcclusionProxy* pProxy)
{
    ACSWrapper csa(&m_cs);

    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        if (pProxy == m_aQueries[i])
        {
            delete m_aQueries[i];
            m_aQueries.RemoveAt(i);
            break;
        }
    }
}

void A3DOcclusionManHW::Clear()
{
    ACSWrapper csa(&m_cs);

    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        delete m_aQueries[i];
    }
    m_aQueries.RemoveAll();
}

bool A3DOcclusionManHW::PrepareCoverageMap(A3DViewport* pViewport, A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture)
{
    m_pViewport = pViewport;
    m_pCamera = pViewport->GetCamera();
    m_bCoverageMapReady = false;
    if (m_pDevice == NULL || m_pCoverageMap == NULL)
        return false;

    int iWidth = pDepthRenderTarget->GetWidth();
    int iHeight = pDepthRenderTarget->GetHeight();

    if (!m_pCoverageMap->GenerateMipmap(pDepthRenderTarget, bUseDepthTexture, true))
        return false;
    
    if (m_iWidth != m_pCoverageMap->GetFinalWidth() || m_iHeight != m_pCoverageMap->GetFinalHeight())
    {
        m_iWidth = m_pCoverageMap->GetFinalWidth();
        m_iHeight = m_pCoverageMap->GetFinalHeight();
    }
    m_bCoverageMapReady = true;
    return true;
}


bool A3DOcclusionManHW::TestAll(int iFlag)
{
    ACSWrapper csa(&m_cs);

    m_nFrameCount++;
    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        if (m_aQueries[i]->m_bNeedTest)
        {
            m_aQueries[i]->m_bNeedQuery = false;
            m_aQueries[i]->m_bOccluded = false;
        }
    }

    if (m_pDevice == NULL || m_pCoverageMap == NULL)
        return false;
    
    if (!m_bCoverageMapReady || m_pCoverageMap->GetSmallDepthBuffer() == NULL)
        return false;

    A3DCULLTYPE cullModeOld = m_pDevice->GetFaceCull();
    m_pDevice->SetFaceCull(A3DCULL_NONE);

    A3DMATRIX4 matProj = m_pCamera->GetProjectionTM();    
    matProj._43 -= 0.10f;

    A3DMATRIX4 matWorldOld = m_pDevice->GetWorldMatrix();
    A3DMATRIX4 matViewOld = m_pDevice->GetViewMatrix();
    A3DMATRIX4 matProjOld = m_pDevice->GetProjectionMatrix();

    m_pDevice->SetWorldMatrix(A3DMATRIX4::IDENTITY);
    m_pDevice->SetViewMatrix(m_pCamera->GetViewTM());
    m_pDevice->SetProjectionMatrix(matProj);

    bool bAlphaEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) != 0;
    bool bAlphaTestEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ALPHATESTENABLE) != 0;
    bool bZEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ZENABLE) != 0;
    bool bZWriteEnableOld = m_pDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE) != 0;

    m_pDevice->SetAlphaBlendEnable(false);
    m_pDevice->SetAlphaTestEnable(false);
    m_pDevice->SetZTestEnable(true);
    m_pDevice->SetZWriteEnable(false);
    m_pDevice->SetZFunc(D3DCMP_LESSEQUAL);
    m_pDevice->ClearVertexShader();
    m_pDevice->ClearPixelShader();
    m_pDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0);

    A3DPLANE* pNearPlane = m_pCamera->GetWorldFrustum()->GetPlane(A3DFrustum::VF_NEAR);
    const A3DVECTOR3& vEye = m_pCamera->GetPos();

    m_pCoverageMap->GetSmallDepthBuffer()->ApplyToDevice(0);

    m_nPassed = 0;
    m_nAvailable = 0;

    m_pStreamAABB->Appear();
    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        A3DOcclusionProxyHW* pQuery = m_aQueries[i];
        if (!pQuery->m_bEnabled || !pQuery->m_pD3DQuery)
            continue;
        
        if (!m_pCamera->AABBInViewFrustum(pQuery->m_bbAABB))
        {
            pQuery->m_bNeedQuery = false;
            pQuery->m_bOccluded = true;
            continue;
        }

        m_nAvailable++;

        if (!pQuery->m_bNeedTest)
            continue;

        if (pQuery->m_bbAABB.IsPointIn(vEye))
        {
            pQuery->m_bNeedQuery = false;
            pQuery->m_bOccluded = false;
            m_nPassed++;
            continue;
        }

        if (pQuery->m_bbAABB.Extents.x > 1.0f && pQuery->m_bbAABB.Extents.y > 1.0f && pQuery->m_bbAABB.Extents.z > 1.0f)
        {
            // if AABB is cross the near plane, and is not flat
            float fPushOut = abs(pNearPlane->vNormal.x * pQuery->m_bbAABB.Extents.x) 
                + abs(pNearPlane->vNormal.y * pQuery->m_bbAABB.Extents.y) 
                + abs(pNearPlane->vNormal.z * pQuery->m_bbAABB.Extents.z);
            float fPlaneDot = DotProduct(pNearPlane->vNormal, pQuery->m_bbAABB.Center) - pNearPlane->fDist;
            if (fPlaneDot < fPushOut)
            {
                pQuery->m_bNeedQuery = false;
                pQuery->m_bOccluded = false;
                m_nPassed++;
                continue;
            }
        }

        pQuery->m_bNeedQuery = true;
        pQuery->m_bNeedTest = false;
        pQuery->m_nQueryFrameIndex = m_nFrameCount;
        if (FAILED(pQuery->m_pD3DQuery->Issue(D3DISSUE_BEGIN)))
        {
            pQuery->m_bNeedQuery = false;
            pQuery->m_bNeedTest = true;
            g_A3DErrLog.Log("A3DOcclusionProxyHW::TestAll, Failed to issue occlusion query");
            continue;
        }
        DrawAABB(pQuery->m_bbAABB);

        if (FAILED(pQuery->m_pD3DQuery->Issue(D3DISSUE_END)))
        {
            pQuery->m_bNeedQuery = false;
            pQuery->m_bNeedTest = true;
        }

    }
    //printf("out frustum = %d, in aabb = %d, cross near = %d\n", nOutFrustum, nInAABB, nCrossNear);
    m_pCoverageMap->GetSmallDepthBuffer()->WithdrawFromDevice();

    m_pDevice->SetWorldMatrix(matWorldOld);
    m_pDevice->SetViewMatrix(matViewOld);
    m_pDevice->SetProjectionMatrix(matProjOld);

    m_pDevice->SetFaceCull(cullModeOld);
    m_pDevice->SetAlphaTestEnable(bAlphaTestEnableOld);
    m_pDevice->SetAlphaBlendEnable(bAlphaEnableOld);
    m_pDevice->SetZTestEnable(bZEnableOld);
    m_pDevice->SetZWriteEnable(bZWriteEnableOld);
    m_pDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0xF);
    return true;
}


void A3DOcclusionManHW::SetState(int iState, int iValue)
{
    if (m_pViewport && m_pCoverageMap && m_pCoverageMap->GetMipmapCount() > 0)
    {
        m_pCoverageMap->ShowDepthMap(m_pCoverageMap->GetSmallDepthBuffer(), m_pViewport->GetCamera(), true);
    }
}

void A3DOcclusionManHW::DrawAABB(const A3DAABB& aabb)
{
    //	Return true if eye is in aabb
    const A3DVECTOR3& vEye = m_pCamera->GetPos();

    A3DVECTOR3 vDelta = aabb.Center - vEye;
    BYTE bySign = 0;
    if (vDelta.x < 0.0f) bySign |= 0x01;
    if (vDelta.y < 0.0f) bySign |= 0x02;
    if (vDelta.z < 0.0f) bySign |= 0x04;

    A3DMATRIX4 matWorld( 
        aabb.Extents.x, 0, 0, 0,
        0, aabb.Extents.y, 0, 0,
        0, 0, aabb.Extents.z, 0,
        aabb.Center.x, aabb.Center.y, aabb.Center.z, 1
        );
    m_pDevice->SetWorldMatrix(matWorld);
    m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLEFAN, 0, 0, 8, bySign * 8, 6);
}

bool A3DOcclusionManHW::CreateAABBStream()
{
    static const WORD aIndicesFan[8*8] = 
    {
        0, 1, 2, 3, 7, 4, 5, 1,	
        //	negative x
        //	negative y
        //	negative z

        1, 2, 3, 0, 4, 5, 6, 2,
        //	positive x
        //	negative y
        //	negative z

        4, 5, 1, 0, 3, 7, 6, 5,
        //	negative x
        //	positive y
        //	negative z

        5, 1, 0, 4, 7, 6, 2, 1,
        //	positive x
        //	positive y
        //	negative z

        3, 2, 1, 0, 4, 7, 6, 2,
        //	negative x
        //	negative y
        //	positive z

        2, 1, 5, 6, 7, 3, 0, 1,
        //	positive x
        //	negative y
        //	positive z

        7, 3, 0, 4, 5, 6, 2, 3,
        //	negative x
        //	positive y
        //	positive z

        6, 5, 4, 7, 3, 2, 1, 5,
        //	positive x
        //	positive y
        //	positive z
    };

    m_pStreamAABB->Init(m_pDevice, sizeof(float) * 3, D3DFVF_XYZ, 8, 64, A3DSTRM_STATIC, A3DSTRM_STATIC);

    // Create a unit box vertex buffer (-1,-1,-1)-(1,1,1)
    A3DVECTOR3* pVerts;
    if (!m_pStreamAABB->LockVertexBuffer(0, sizeof(float) * 3 * 8, (BYTE**)&pVerts, 0))
    {
        g_A3DErrLog.Log("A3DOcclusionManHW::CreateAABBStream, Failed to LockVertexBuffer");
        return false;
    }
    pVerts[0].Set(-1, -1, -1);
    pVerts[1].Set( 1, -1, -1);
    pVerts[2].Set( 1, -1,  1);
    pVerts[3].Set(-1, -1,  1);
    pVerts[4].Set(-1,  1, -1);
    pVerts[5].Set( 1,  1, -1);
    pVerts[6].Set( 1,  1,  1);
    pVerts[7].Set(-1,  1,  1);
    m_pStreamAABB->UnlockVertexBuffer();

    // triangle-fan indices, 8 groups
    WORD* pInds;
    if (!m_pStreamAABB->LockIndexBuffer(0, sizeof(WORD) * 64, (BYTE**)&pInds, 0))
    {
        g_A3DErrLog.Log("OcclusionManHW::CreateAABBStream, Failed to LockIndexBuffer");
        return false;
    }
    memcpy(pInds, aIndicesFan, sizeof(WORD) * 64);
    m_pStreamAABB->UnlockIndexBuffer();

    return true;
}

bool A3DOcclusionManHW::BeforeDeviceReset()
{
    ACSWrapper csa(&m_cs);

    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        m_aQueries[i]->Release();
    }
    return true;
}

bool A3DOcclusionManHW::AfterDeviceReset()
{
    ACSWrapper csa(&m_cs);

    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        m_aQueries[i]->Init(m_pDevice);
    }
    m_nFrameCount = 100;
    return true;
}

int A3DOcclusionManHW::GetState(int iState) const
{
    switch (iState)
    {
    case A3DOS_TOTAL_QUERY:
        return m_aQueries.GetSize();
    case A3DOS_VISIBLE_QUERY:
    {
        int nVisiable = 0;
        for (int i = 0; i < m_aQueries.GetSize(); i++)
            if (m_aQueries[i]->m_bEnabled && (m_aQueries[i]->m_bNeedQuery || !m_aQueries[i]->m_bOccluded))
                nVisiable++;
        return nVisiable;
    }
    case A3DOS_EARLY_PASSED_QUERY:
        return m_nPassed;
    case A3DOS_AVAILID_QUERY:
        return m_nAvailable;
    case A3DOS_MISSED_QUERY:
        {
            int nMissed = 0;
            for (int i = 0; i < m_aQueries.GetSize(); i++)
                if (m_aQueries[i]->m_bEnabled && m_aQueries[i]->m_bNeedQuery)
                    nMissed++;
            return nMissed;
        }
    }
    return -1;
}

static bool IsDepthFormatOk(A3DDevice* pDevice, A3DFORMAT fmtDepth)
{
    A3DFORMAT fmtAdapter = pDevice->GetDevFormat().fmtAdapter;
    A3DFORMAT fmtBackBuffer = pDevice->GetDevFormat().fmtTarget;

    //	Verify that the depth format exists
    //	Note: here we use D3DRTYPE_TEXTURE instead of D3DRTYPE_SURFACE !
    HRESULT hr = pDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
        (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, (D3DFORMAT)fmtDepth);
    if (FAILED(hr))
        return false;

    //	Verify that the depth format is compatible
    hr = pDevice->GetA3DEngine()->GetD3D()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
        (D3DFORMAT)fmtAdapter, (D3DFORMAT)fmtBackBuffer, (D3DFORMAT)fmtDepth);

    return SUCCEEDED(hr);
}

static bool IsRenderTargetFormatOk(A3DDevice* pDevice, A3DFORMAT fmtTarget)
{
    A3DFORMAT fmtAdapter = pDevice->GetDevFormat().fmtAdapter;

    return SUCCEEDED(pDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT,
        (D3DDEVTYPE)pDevice->GetDevType(), (D3DFORMAT)fmtAdapter, D3DUSAGE_RENDERTARGET,
        D3DRTYPE_TEXTURE, (D3DFORMAT)fmtTarget));
}


bool A3DOcclusionManHW::CheckSupport()
{
    if (!IsDepthFormatOk(m_pDevice, A3DFMT_INTZ))
    {
        g_A3DErrLog.Log("OcclusionManHW::CheckSupport, INTZ format depth buffer is not supported");
        return false;
    }
    if (!IsRenderTargetFormatOk(m_pDevice, (A3DFORMAT)D3DFMT_R32F))
    {
        g_A3DErrLog.Log("OcclusionManHW::CheckSupport, R32F format color buffer is not supported");
        return false;
    }
    return true;
}
