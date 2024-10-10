/*
 * FILE: A3DOcclusionManSW.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: yaochunhui, 2012/2/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DOcclusionManSW.h"
#include "A3DCoverageMap.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "A3DDevObject.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"

// SSE intrinsics
#include <xmmintrin.h>
// SSE2 intrinsics
#include <emmintrin.h>

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

inline int iround(float x)
{
#ifdef _WIN64
    return (int)(x + 0.5f);
#else
    int t;
    __asm
    {
        fld x
        fistp t
    }
    return t;
#endif
}

template <class T> 
inline T a_Min(T x, T y, T z, T w)
{
    return a_Min(a_Min(x, y), a_Min(z, w));
}

template <class T> 
inline T a_Max(T x, T y, T z, T w)
{
    return a_Max(a_Max(x, y), a_Max(z, w));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//   A3DOcclusionProxySW : Occlusion proxy for software occlusion manager
//
//////////////////////////////////////////////////////////////////////////

class A3DOcclusionProxySW : public A3DOcclusionProxy
{
public:
    A3DOcclusionProxySW(A3DOcclusionManSW* pMan);
    ~A3DOcclusionProxySW();

    virtual void SetAABB(const A3DAABB& aabb);
    virtual bool IsOccluded(A3DViewport* pViewport);
    virtual bool GetEnabled() const { return m_bEnabled; };
    virtual void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

public:
    A3DAABB m_bbAABB;
    bool m_bEnabled;
    bool m_bOccluded;
    A3DOcclusionManSW* m_pMan;
};

A3DOcclusionProxySW::A3DOcclusionProxySW(A3DOcclusionManSW* pMan)
{
    m_bEnabled = true;
    m_bOccluded = false;
    m_pMan = pMan;
}

A3DOcclusionProxySW::~A3DOcclusionProxySW()
{
}

void A3DOcclusionProxySW::SetAABB(const A3DAABB& aabb)
{
    m_bbAABB = aabb;
}

bool A3DOcclusionProxySW::IsOccluded(A3DViewport* pViewport)
{
    if (!m_bEnabled || pViewport != m_pMan->m_pViewport)
        return false;

    return m_bOccluded;
}

//////////////////////////////////////////////////////////////////////////
//
//   A3DMipmapBuffer : software generating max mip-mapping
//
//////////////////////////////////////////////////////////////////////////


A3DMipmapBuffer::A3DMipmapBuffer()
{
    m_iWidth = 0;
    m_iHeight = 0;
}

A3DMipmapBuffer::~A3DMipmapBuffer()
{
    Release();
}

void A3DMipmapBuffer::Init(int iWidth, int iHeight)
{
    Release();
    int iMipWidth = iWidth;
    int iMipHeight = iHeight;

    // Generate mip-maps information
    int iShift = 0;
    MipmapRec* rec;
    do
    {
        // reduce by 4x4
        iMipWidth >>= 2;
        iMipHeight >>= 2;
        iShift += 2;
        if (iMipWidth < 1) iMipWidth = 1;
        if (iMipHeight < 1) iMipHeight = 1;
        rec = new MipmapRec;
        rec->iWidth = iMipWidth;
        rec->iHeight = iMipHeight;
        rec->iShift = iShift;
        rec->pBuf = new float[iMipWidth * iMipHeight];
        m_arrMipmaps.Add(rec);
    } while (iMipWidth > 1 || iMipHeight > 1);

    m_iWidth = iWidth;
    m_iHeight = iHeight;
}

void A3DMipmapBuffer::Release()
{
    for (int i = 0; i < m_arrMipmaps.GetSize(); i++)
    {
        delete m_arrMipmaps[i]->pBuf;
        m_arrMipmaps[i]->pBuf = NULL;
        delete m_arrMipmaps[i];
    }
    m_arrMipmaps.RemoveAll();
    m_iWidth = 0;
    m_iHeight = 0;
}

const A3DMipmapBuffer::MipmapRec* A3DMipmapBuffer::GetMipmap(int iIndex) const
{
    return m_arrMipmaps[iIndex];
}

void A3DMipmapBuffer::UpdateMipmaps(float* pBuf, int iPitch)
{
    // Generate Max-filter mip-maps
    for (int i = 0; i < m_arrMipmaps.GetSize(); i++)
    {
        // At first level read from original buffer, otherwise use previous level
        int iLastWidth = i == 0 ? m_iWidth : m_arrMipmaps[i - 1]->iWidth;
        int iLastHeight = i == 0 ? m_iHeight : m_arrMipmaps[i - 1]->iHeight;
        int iLastPitch = i == 0 ? iPitch : m_arrMipmaps[i - 1]->iWidth * sizeof(float);
        float* pSrcBuf = i == 0 ? pBuf : m_arrMipmaps[i - 1]->pBuf;
        
        // Current buffer
        float* pResultBuf = m_arrMipmaps[i]->pBuf;

        // Position in previous buffer
        int iLastX = 0;
        int iLastY = 0;
        
        for (int iY = 0; iY < m_arrMipmaps[i]->iHeight; iY++)
        {
            // Number of pixels to collect in previous buffer in y
            // if it is at bottom border, collect all left, otherwise 4 
            int nYPixels = iY == m_arrMipmaps[i]->iHeight - 1 ? iLastHeight - iLastY : 4;
            
            iLastX = 0;
            float* pResultBufLine = pResultBuf;

            for (int iX = 0; iX < m_arrMipmaps[i]->iWidth; iX++)
            {
                float fMaxZ = 0;

                // Number of pixels to collect in previous buffer in x
                int nXPixels = iX == m_arrMipmaps[i]->iWidth - 1 ? iLastWidth - iLastX : 4;

                float* pBuf = pSrcBuf + iLastX;
                // Find max value
                for (int iYPixel = 0; iYPixel < nYPixels; iYPixel++)
                {
                    float* pBufLine = pBuf;
                    for (int iXPixel = 0; iXPixel < nXPixels; iXPixel++, pBufLine++)
                    {
                        if (fMaxZ < *pBufLine) fMaxZ = *pBufLine;
                    }
                    pBuf += iLastWidth;
                }
                // Write max value to current buffer
                *pResultBufLine = fMaxZ;
                pResultBufLine++;
                iLastX += 4;
            }
            pResultBuf += m_arrMipmaps[i]->iWidth;
            iLastY += 4;
            (char*&)pSrcBuf += iLastPitch * 4;
        }
    }
}

void A3DMipmapBuffer::GetCoordInLevel(int iLevel, int iX, int iY, int& iOutX, int& iOutY) const
{
    iOutX = iX >> m_arrMipmaps[iLevel]->iShift;
    iOutY = iY >> m_arrMipmaps[iLevel]->iShift;
    iOutX = a_Min(iOutX, m_arrMipmaps[iLevel]->iWidth - 1);
    iOutY = a_Min(iOutY, m_arrMipmaps[iLevel]->iHeight - 1);
}

//////////////////////////////////////////////////////////////////////////
//
//   A3DOcclusionManHW
//
//////////////////////////////////////////////////////////////////////////

A3DOcclusionManSW::A3DOcclusionManSW()
{
    m_pCoverageMap = NULL;
    m_pMipmapBuffer = NULL;
    m_pCamera = NULL;
    m_pDevice = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    m_bUseHierachy = true;
    m_bUseSSE = true;
    m_nVisible = 0;
    m_nPassed = 0;
    m_nAvailable = 0;
    m_bCoverageMapReady = false;
    m_pViewport = NULL;
}

A3DOcclusionManSW::~A3DOcclusionManSW()
{
    Release();
}

bool A3DOcclusionManSW::Init(A3DEngine* pEngine)
{
    m_pDevice = pEngine->GetA3DDevice();

    if (!CheckSupport())
        return false;

    m_pCoverageMap = new A3DCoverageMap;
    m_pCoverageMap->Init(pEngine);
    m_pMipmapBuffer = new A3DMipmapBuffer;
    m_bCoverageMapReady = false;
    return true;
}

void A3DOcclusionManSW::Release()
{
    Clear();
    A3DRELEASE(m_pCoverageMap);
    A3DRELEASE(m_pMipmapBuffer);
}

A3DOcclusionProxy* A3DOcclusionManSW::CreateQuery()
{
    A3DOcclusionProxySW* pProxy = new A3DOcclusionProxySW(this);
    m_aQueries.Add(pProxy);
    return pProxy;
}

void A3DOcclusionManSW::ReleaseQuery(A3DOcclusionProxy* pProxy)
{
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

void A3DOcclusionManSW::Clear()
{
    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        delete m_aQueries[i];
    }
    m_aQueries.RemoveAll();
}

bool A3DOcclusionManSW::PrepareCoverageMap(A3DViewport* pViewport, A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture)
{
    m_bCoverageMapReady = false;

    m_pViewport = pViewport;
    m_pCamera = pViewport->GetCamera();
    m_matWVP = m_pCamera->GetVPTM();
    m_matWVP._43 -= 0.01f; 

    int iWidth = pDepthRenderTarget->GetWidth();
    int iHeight = pDepthRenderTarget->GetHeight();

    if (!m_pCoverageMap->GenerateMipmap(pDepthRenderTarget, bUseDepthTexture, false))
        return false;

    // Reset buffers
    if (m_iWidth != m_pCoverageMap->GetFinalWidth() || m_iHeight != m_pCoverageMap->GetFinalHeight())
    {
        m_iWidth = m_pCoverageMap->GetFinalWidth();
        m_iHeight = m_pCoverageMap->GetFinalHeight();
        m_aAlignedZBuffer.Init(m_iWidth, m_iHeight, 16, 8 * sizeof(float), 8 * sizeof(float));
        memset(m_aAlignedZBuffer.GetData(), 0, m_aAlignedZBuffer.GetWidthBytes() * m_aAlignedZBuffer.GetHeightBytes());
        m_pMipmapBuffer->Init(m_iWidth, m_iHeight);
    }

    if (!m_pCoverageMap->ReadbackSmallBuffer(m_aAlignedZBuffer.GetData(), m_aAlignedZBuffer.GetWidthBytes()))
        return false;

    if (m_bUseHierachy)
        m_pMipmapBuffer->UpdateMipmaps(m_aAlignedZBuffer.GetData(), m_aAlignedZBuffer.GetWidthBytes());

    m_bCoverageMapReady = true;

    return true;
}

bool A3DOcclusionManSW::TestAll(int iFlag)
{
    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        m_aQueries[i]->m_bOccluded = false;
    }

    if (m_pCoverageMap == NULL || m_pMipmapBuffer == NULL || !m_bCoverageMapReady)
        return false;

    const A3DVECTOR3& vEye = m_pCamera->GetPos();

    m_nVisible = 0;
    m_nPassed = 0;
    m_nAvailable = 0;

    for (int i = 0; i < m_aQueries.GetSize(); i++)
    {
        A3DOcclusionProxySW* pQuery = m_aQueries[i];
        if (!pQuery->m_bEnabled)
            continue;

        if (!m_pCamera->AABBInViewFrustum(pQuery->m_bbAABB))
        {
            pQuery->m_bOccluded = true;
            continue;
        }
        m_nAvailable++;

        if (pQuery->m_bbAABB.IsPointIn(vEye))
        {
            pQuery->m_bOccluded = false;
            m_nVisible++;
            continue;
        }

        pQuery->m_bOccluded = !TestAABBQuad(pQuery->m_bbAABB);
        if (!pQuery->m_bOccluded)
            m_nVisible++;
    }


    return true;
}

bool A3DOcclusionManSW::TestAABBQuad(const A3DAABB& aabb)
{
    //	Return true if eye is in aabb
    const A3DVECTOR3& vEye = m_pCamera->GetPos();

    if (aabb.IsPointIn(vEye))
    {
        m_nPassed++;
        return true;
    }

    m_nWrite = 0;

    A3DVECTOR3 vDelta = aabb.Center - vEye;
    BYTE bySign = 0;
    if (vDelta.x < 0.0f) bySign |= 0x01;
    if (vDelta.y < 0.0f) bySign |= 0x02;
    if (vDelta.z < 0.0f) bySign |= 0x04;

    static const WORD aIndices[8][12] = 
    {
        {
            0, 4, 7, 3, 	//	negative x
            0, 3, 2, 1, 	//	negative y
            0, 1, 5, 4,	    //	negative z
        },

        {
            1, 2, 6, 5,	    //	positive x
            0, 3, 2, 1, 	//	negative y
            0, 1, 5, 4, 	//	negative z
        },

        {
            0, 4, 7, 3, 	//	negative x
            7, 4, 5, 6, 	//	positive y
            0, 1, 5, 4,	    //	negative z
        },

        {
            1, 2, 6, 5, 	//	positive x
            7, 4, 5, 6,	    //	positive y
            0, 1, 5, 4, 	//	negative z
        },

        {
            0, 4, 7, 3, 	//	negative x
            0, 3, 2, 1, 	//	negative y
            2, 3, 7, 6,	    //	positive z
        },

        {
            1, 2, 6, 5, 	//	positive x
            0, 3, 2, 1, 	//	negative y
            2, 3, 7, 6,	    //	positive z
        },

        {
            0, 4, 7, 3,     //	negative x
            7, 4, 5, 6,	    //	positive y
            2, 3, 7, 6,	    //	positive z
        },

        {
            1, 2, 6, 5, 	//	positive x
            7, 4, 5, 6,	    //	positive y
            2, 3, 7, 6,	    //	positive z
        },
    };

    const WORD* pIndices = &aIndices[bySign][0];

    A3DVECTOR3 aVerts[8];
    aVerts[0].Set(aabb.Mins.x, aabb.Mins.y, aabb.Mins.z);
    aVerts[1].Set(aabb.Maxs.x, aabb.Mins.y, aabb.Mins.z);
    aVerts[2].Set(aabb.Maxs.x, aabb.Mins.y, aabb.Maxs.z);
    aVerts[3].Set(aabb.Mins.x, aabb.Mins.y, aabb.Maxs.z);

    aVerts[4].Set(aabb.Mins.x, aabb.Maxs.y, aabb.Mins.z);
    aVerts[5].Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Mins.z);
    aVerts[6].Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);
    aVerts[7].Set(aabb.Mins.x, aabb.Maxs.y, aabb.Maxs.z);

    m_nBlocked = 0;
    // draw quads
    if (m_bUseSSE)
        DrawIndexedQuads_SSE(aVerts, 8, pIndices, 3);
    else
        DrawIndexedQuads(aVerts, 8, pIndices, 3);

    if (m_nBlocked == 3)
        m_nPassed++;

    return m_nWrite > 0;
}

void A3DOcclusionManSW::TransformVertices(const A3DVECTOR3* pVert, const int nVertNum)
{
    const A3DMATRIX4& mat = m_matWVP;

    if (m_aTransVerts.GetSize() < (int)((nVertNum + 1) * sizeof(A3DVECTOR4)))
        m_aTransVerts.SetSize((nVertNum + 1) * sizeof(A3DVECTOR4), 16);
    A3DVECTOR4* pVert4 = m_aTransVerts.GetData();
    for(int i = 0; i < nVertNum; i++)
    {
        const A3DVECTOR3& v = pVert[i];
        pVert4[i].x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
        pVert4[i].y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
        pVert4[i].z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
        pVert4[i].w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44;
    }
}

void A3DOcclusionManSW::FillQuad_HalfEdge(const A3DVECTOR4 &v1, const A3DVECTOR4 &v2, const A3DVECTOR4 &v3, const A3DVECTOR4 &v4)
{
    // 转化为 28.4 定点数 
    const int Y1 = iround(16.0f * v1.y); 
    const int Y2 = iround(16.0f * v2.y); 
    const int Y3 = iround(16.0f * v3.y); 
    const int Y4 = iround(16.0f * v4.y); 

    const int X1 = iround(16.0f * v1.x); 
    const int X2 = iround(16.0f * v2.x); 
    const int X3 = iround(16.0f * v3.x); 
    const int X4 = iround(16.0f * v4.x); 

    // 预先计算Delta 

    const int DX12 = X1 - X2; 
    const int DX23 = X2 - X3; 
    const int DX34 = X3 - X4; 
    const int DX41 = X4 - X1; 
    const int DX31 = X3 - X1; 

    const int DY12 = Y1 - Y2; 
    const int DY23 = Y2 - Y3; 
    const int DY34 = Y3 - Y4; 
    const int DY41 = Y4 - Y1; 
    const int DY31 = Y3 - Y1; 

    const float DZ12 = v1.z - v2.z; 
    const float DZ23 = v2.z - v3.z; 
    const float DZ34 = v3.z - v4.z; 
    const float DZ41 = v4.z - v1.z; 
    const float DZ31 = v3.z - v1.z; 


   // 剔除后向面
    if (DX12 * DY31 - DX31 * DY12 < 0)
    {
        m_nWrite++;
        return;
    }

    // 高精度Delta 

    const int FDX12 = DX12 << 4; 
    const int FDX23 = DX23 << 4; 
    const int FDX34 = DX34 << 4; 
    const int FDX41 = DX41 << 4; 

    const int FDY12 = DY12 << 4; 
    const int FDY23 = DY23 << 4; 
    const int FDY34 = DY34 << 4; 
    const int FDY41 = DY41 << 4; 

    // 包围矩形 

    int minx = (a_Min(X1, X2, X3, X4) + 0xF) >> 4; 
    int maxx = (a_Max(X1, X2, X3, X4) + 0xF) >> 4; 
    int miny = (a_Min(Y1, Y2, Y3, Y4) + 0xF) >> 4; 
    int maxy = (a_Max(Y1, Y2, Y3, Y4) + 0xF) >> 4; 

    // 将范围限定在屏幕内
    minx = a_Max(minx, 0);
    maxx = a_Min(maxx, m_iWidth);
    miny = a_Max(miny, 0);
    maxy = a_Min(maxy, m_iHeight);

    // 8x8 block (must be power of two) 
    const int q = 8; 

    // 8x8 对齐 
    minx &= ~(q - 1); 
    miny &= ~(q - 1); 
    int iPitch = m_aAlignedZBuffer.GetWidthBytes();
    char* pZBuffer = (char*)m_aAlignedZBuffer.GetData();
    pZBuffer += miny * iPitch; 

    // Half-edge constants 
    int C1 = DY12 * X1 - DX12 * Y1; 
    int C2 = DY23 * X2 - DX23 * Y2; 
    int C3 = DY34 * X3 - DX34 * Y3; 
    int C4 = DY41 * X4 - DX41 * Y4; 


    // Correct for fill convention 
    if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++; 
    if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++; 
    if(DY34 < 0 || (DY34 == 0 && DX34 > 0)) C3++; 
    if(DY41 < 0 || (DY41 == 0 && DX41 > 0)) C4++; 

    // 计算平面方程 A x + B y - C z = 0
    const float fA = DZ12 * DY31 - DZ31 * DY12;
    const float fB = DX12 * DZ31 - DX31 * DZ12;
    const int fC = -(DY12 * DX31 - DY31 * DX12);

    const float dzdx = fA / fC;         // dz/dx
    const float dzdy = fB / fC;         // dz/dx
    const float DZDX = dzdx * 16;       // 和定点数<<4匹配
    const float DZDY = dzdy * 16;

    // 计算左上角的z
    float fz0 = v1.z + ((minx << 4) - X1) * dzdx + ((miny << 4) - Y1) * dzdy;

    for(int y = miny; y < maxy; y += q) 
    { 
        float fzLine0 = fz0;
        for(int x = minx; x < maxx; x += q) 
        { 
            // Corners of block 
            int x0 = x << 4; 
            int x1 = (x + q - 1) << 4; 
            int y0 = y << 4; 
            int y1 = (y + q - 1) << 4; 
            // fz0 = v1.z + (x0 - X1) * dzdx + (y0 - Y1) * dzdy;

            // 计算角点的 half-space 值 

            bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0; 
            bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0; 
            bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0; 
            bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0; 
            int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3); 

            bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0; 
            bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0; 
            bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0; 
            bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0; 
            int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3); 

            bool c00 = C3 + DX34 * y0 - DY34 * x0 > 0; 
            bool c10 = C3 + DX34 * y0 - DY34 * x1 > 0; 
            bool c01 = C3 + DX34 * y1 - DY34 * x0 > 0; 
            bool c11 = C3 + DX34 * y1 - DY34 * x1 > 0; 
            int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3); 

            bool d00 = C4 + DX41 * y0 - DY41 * x0 > 0; 
            bool d10 = C4 + DX41 * y0 - DY41 * x1 > 0; 
            bool d01 = C4 + DX41 * y1 - DY41 * x0 > 0; 
            bool d11 = C4 + DX41 * y1 - DY41 * x1 > 0; 
            int d = (d00 << 0) | (d10 << 1) | (d01 << 2) | (d11 << 3); 

            // 如果block完全在某条边外部，跳过
            if (a == 0x0 || b == 0x0 || c == 0x0 || d == 0x0)
            {
                fzLine0 += q * DZDX; // continue 会跳过
                continue; 
            }

            float *buffer = (float*)pZBuffer; 

            // 如果block全在内部，内部点不用判断
            if ((a == 0xF && b == 0xF && c == 0xF && d == 0xF)) 
            { 
                float fz = fzLine0;
                for(int iy = 0; iy < q; iy++) 
                { 
                    float fzLine = fz;
                    for(int ix = x; ix < x + q; ix++) 
                    { 
                        if (buffer[ix] >= fzLine)
                        {
                            m_nWrite++;
                            return;
                        }
                        fzLine += DZDX; 
                    } 
                    fz += DZDY;
                    (char*&)buffer += iPitch; 
                } 
            } 
            else // 部分在内，每个点都需要判断
            { 
                int CY1 = C1 + DX12 * y0 - DY12 * x0; 
                int CY2 = C2 + DX23 * y0 - DY23 * x0; 
                int CY3 = C3 + DX34 * y0 - DY34 * x0; 
                int CY4 = C4 + DX41 * y0 - DY41 * x0; 

                float fz = fzLine0;

                for(int iy = y; iy < y + q; iy++) 
                { 
                    int CX1 = CY1; 
                    int CX2 = CY2; 
                    int CX3 = CY3; 
                    int CX4 = CY4; 
                    float fzLine = fz;

                    for(int ix = x; ix < x + q; ix++) 
                    { 
                        if(CX1 > 0 && CX2 > 0 && CX3 > 0 && CX4 > 0) 
                        { 
                            if (buffer[ix] >= fzLine)
                            {
                                m_nWrite++;
                                return;
                            }
                        } 
                        CX1 -= FDY12; 
                        CX2 -= FDY23; 
                        CX3 -= FDY34; 
                        CX4 -= FDY41; 
                        fzLine += DZDX; 
                    } 

                    CY1 += FDX12; 
                    CY2 += FDX23; 
                    CY3 += FDX34; 
                    CY4 += FDX41;
                    fz += DZDY;
                    (char*&)buffer += iPitch; 
                } 
            } 
            fzLine0 += q * DZDX;
        } 
        fz0 += q * DZDY;
        pZBuffer += q * iPitch; 
    } 
}

void A3DOcclusionManSW::DrawIndexedQuads(const A3DVECTOR3* pVertices, int nVertNum, const WORD* pIndices, int nFaceNum)
{
    TransformVertices(pVertices, nVertNum);

    for (int i = 0; i < nVertNum; i++)
    {
        if (m_aTransVerts[i].z <= 0 || m_aTransVerts[i].w <= 0)
        {
            m_nWrite++;
            return;
        }
        m_aTransVerts[i].x /= m_aTransVerts[i].w; 
        m_aTransVerts[i].y /= m_aTransVerts[i].w; 
        m_aTransVerts[i].z /= m_aTransVerts[i].w; 
        m_aTransVerts[i].x = (m_aTransVerts[i].x + 1) / 2 * m_iWidth + 0.5f;
        m_aTransVerts[i].y = (-m_aTransVerts[i].y + 1) / 2 * m_iHeight + 0.5f;
    }
    const WORD* pInds = pIndices;
    A3DVECTOR4 pVerts[4];
    for (int i = 0; i < nFaceNum; i++)
    {
        pVerts[0] = m_aTransVerts[*pInds++];
        pVerts[1] = m_aTransVerts[*pInds++];
        pVerts[2] = m_aTransVerts[*pInds++];
        pVerts[3] = m_aTransVerts[*pInds++];

        bool bBlocked = false;

        // Check hierarchically
        if (m_bUseHierachy)
        {
            int iVertX0 = iround(pVerts[0].x - 0.5f);
            int iVertY0 = iround(pVerts[0].y - 0.5f);
            int iVertX1 = iround(pVerts[1].x - 0.5f);
            int iVertY1 = iround(pVerts[1].y - 0.5f);
            int iVertX2 = iround(pVerts[2].x - 0.5f);
            int iVertY2 = iround(pVerts[2].y - 0.5f);
            int iVertX3 = iround(pVerts[3].x - 0.5f);
            int iVertY3 = iround(pVerts[3].y - 0.5f);

            a_Clamp(iVertX0, 0, m_iWidth - 1);
            a_Clamp(iVertY0, 0, m_iHeight - 1);
            a_Clamp(iVertX1, 0, m_iWidth - 1);
            a_Clamp(iVertY1, 0, m_iHeight - 1);
            a_Clamp(iVertX2, 0, m_iWidth - 1);
            a_Clamp(iVertY2, 0, m_iHeight - 1);
            a_Clamp(iVertX3, 0, m_iWidth - 1);
            a_Clamp(iVertY3, 0, m_iHeight - 1);

            float fMinZ = a_Min(pVerts[0].z, pVerts[1].z, pVerts[2].z, pVerts[3].z);

            for (int iMip = m_pMipmapBuffer->GetMipmapCount() - 1; iMip >= 0; iMip--)
            {
                int iMipX0, iMipY0;
                int iMipX1, iMipY1;
                int iMipX2, iMipY2;
                int iMipX3, iMipY3;
                m_pMipmapBuffer->GetCoordInLevel(iMip, iVertX0, iVertY0, iMipX0, iMipY0);
                m_pMipmapBuffer->GetCoordInLevel(iMip, iVertX1, iVertY1, iMipX1, iMipY1);
                m_pMipmapBuffer->GetCoordInLevel(iMip, iVertX2, iVertY2, iMipX2, iMipY2);
                m_pMipmapBuffer->GetCoordInLevel(iMip, iVertX3, iVertY3, iMipX3, iMipY3);

                // if all vertices are in the same pixel
                if (iMipX0 == iMipX1 && iMipX1 == iMipX2 && iMipX2 == iMipX3 
                    && iMipY0 == iMipY1 && iMipY1 == iMipY2 && iMipY2 == iMipY3)
                {
                    const A3DMipmapBuffer::MipmapRec* rec = m_pMipmapBuffer->GetMipmap(iMip);
                    float fVal = rec->pBuf[iMipX0 + iMipY0 * rec->iWidth];
                    if (fMinZ > fVal)
                    {
                        // all z greater than buffer Z, this primitive is occluded
                        bBlocked = true;
                        m_nBlocked++;
                        break;
                    }
                }
                else
                {
                    // vertices can't be in the same pixel in above levels 
                    break;
                }

            }
        }

        if (!bBlocked)
        {
            // passed early check, do filling
            FillQuad_HalfEdge(pVerts[0], pVerts[1], pVerts[2], pVerts[3]);
        }
    }
}

// multiply of __m128i
static inline __m128i MultiplyM128i(const __m128i &a, const __m128i &b)
{
    __m128i tmp1 = _mm_mul_epu32(a,b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
}

void A3DOcclusionManSW::FillQuad_HalfEdge_SSE(const A3DVECTOR4 &v1, const A3DVECTOR4 &v2, const A3DVECTOR4 &v3, const A3DVECTOR4 &v4)
{
    // 转化为 28.4 定点数 
    const int Y1 = iround(16.0f * v1.y); 
    const int Y2 = iround(16.0f * v2.y); 
    const int Y3 = iround(16.0f * v3.y); 
    const int Y4 = iround(16.0f * v4.y); 

    const int X1 = iround(16.0f * v1.x); 
    const int X2 = iround(16.0f * v2.x); 
    const int X3 = iround(16.0f * v3.x); 
    const int X4 = iround(16.0f * v4.x); 

    const int DX12 = X1 - X2; 
    const int DX31 = X3 - X1; 
    const int DY31 = Y3 - Y1; 
    const int DY12 = Y1 - Y2; 

    // 剔除后向面
    if (DX12 * DY31 - DX31 * DY12 < 0)
    {
        m_nWrite++;
        return;
    }

    __m128i emmY = _mm_set_epi32(Y1, Y2, Y3, Y4);
    __m128i emmX = _mm_set_epi32(X1, X2, X3, X4);

    // 预先计算Delta 
    __m128i emmDX = _mm_sub_epi32(emmX, _mm_shuffle_epi32(emmX, _MM_SHUFFLE(2, 1, 0, 3)));
    //const int DX12 = X1 - X2; 
    //const int DX23 = X2 - X3; 
    //const int DX34 = X3 - X4; 
    //const int DX41 = X4 - X1; 

    __m128i emmDY = _mm_sub_epi32(emmY, _mm_shuffle_epi32(emmY, _MM_SHUFFLE(2, 1, 0, 3)));
    //const int DY12 = Y1 - Y2; 
    //const int DY23 = Y2 - Y3; 
    //const int DY34 = Y3 - Y4; 
    //const int DY41 = Y4 - Y1; 

    const float DZ12 = v1.z - v2.z; 
    const float DZ23 = v2.z - v3.z; 
    const float DZ34 = v3.z - v4.z; 
    const float DZ41 = v4.z - v1.z; 
    const float DZ31 = v3.z - v1.z; 



    // 高精度Delta 
    __m128i emmFDX = _mm_slli_epi32(emmDX, 4);
    //const int FDX12 = DX12 << 4; 
    //const int FDX23 = DX23 << 4; 
    //const int FDX34 = DX34 << 4; 
    //const int FDX41 = DX41 << 4; 

    __m128i emmFDY = _mm_slli_epi32(emmDY, 4);
    //const int FDY12 = DY12 << 4; 
    //const int FDY23 = DY23 << 4; 
    //const int FDY34 = DY34 << 4; 
    //const int FDY41 = DY41 << 4; 

    // 包围矩形 

    int minx = (a_Min(X1, X2, X3, X4) + 0xF) >> 4; 
    int maxx = (a_Max(X1, X2, X3, X4) + 0xF) >> 4; 
    int miny = (a_Min(Y1, Y2, Y3, Y4) + 0xF) >> 4; 
    int maxy = (a_Max(Y1, Y2, Y3, Y4) + 0xF) >> 4; 

    // 将范围限定在屏幕内
    minx = a_Max(minx, 0);
    maxx = a_Min(maxx, m_iWidth);
    miny = a_Max(miny, 0);
    maxy = a_Min(maxy, m_iHeight);


    const int q = 8; 

    minx &= ~(q - 1); 
    miny &= ~(q - 1); 
    int iPitch = m_aAlignedZBuffer.GetWidthBytes();
    char* pZBuffer = (char*)m_aAlignedZBuffer.GetData();
    pZBuffer += miny * iPitch; 

    __m128i emmC = _mm_sub_epi32(MultiplyM128i(emmX, emmDY), MultiplyM128i(emmY, emmDX));
    //int C1 = DY12 * X1 - DX12 * Y1; 
    //int C2 = DY23 * X2 - DX23 * Y2; 
    //int C3 = DY34 * X3 - DX34 * Y3; 
    //int C4 = DY41 * X4 - DX41 * Y4; 

    // Correct for fill convention 
    __m128i emmZero = {0};
    __m128i emmFillCond1 = _mm_cmplt_epi32(emmDY, emmZero); // DY < 0
    __m128i emmFillCond2 = _mm_cmpeq_epi32(emmDY, emmZero); // DY == 0
    __m128i emmFillCond3 = _mm_cmpgt_epi32(emmDX, emmZero); // DX > 0
    __m128i emmFillCond = _mm_or_si128(emmFillCond1, _mm_and_si128(emmFillCond2, emmFillCond3));
    emmFillCond = _mm_and_si128(emmFillCond, _mm_set1_epi32(1)); // convert fffffffff to 1
    emmC = _mm_add_epi32(emmC, emmFillCond);
    
    //if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++; 
    //if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++; 
    //if(DY34 < 0 || (DY34 == 0 && DX34 > 0)) C3++; 
    //if(DY41 < 0 || (DY41 == 0 && DX41 > 0)) C4++; 


    // 计算平面方程 A x + B y - C z = 0
    const float fA = DZ12 * DY31 - DZ31 * DY12;
    const float fB = DX12 * DZ31 - DX31 * DZ12;
    const int fC = -(DY12 * DX31 - DY31 * DX12);

    const float dzdx = fA / fC;         // dz/dx
    const float dzdy = fB / fC;         // dz/dx
    const float DZDX = dzdx * 16;       // 和定点数匹配
    const float DZDY = dzdy * 16;

    const float DZDX4 = DZDX * 4;

    __m128 xmmDZDX = _mm_set1_ps(DZDX);
    __m128 xmmDZDY = _mm_set1_ps(DZDY);
    __m128 xmmDZDXStep = {0, DZDX, DZDX * 2, DZDX * 3};

    // 计算左上角的z
    float fz0 = v1.z + ((minx << 4) - X1) * dzdx + ((miny << 4) - Y1) * dzdy;

    for(int y = miny; y < maxy; y += q) 
    { 
        float fzLine0 = fz0;
        for(int x = minx; x < maxx; x += q) 
        { 
            // Corners of block 
            int x0 = x << 4; 
            int x1 = (x + q - 1) << 4; 
            int y0 = y << 4; 
            int y1 = (y + q - 1) << 4; 
            //fz0 = v1.z + (x0 - X1) * dzdx + (y0 - Y1) * dzdy;

            // half-space functions 
            __m128i emmCornerX0 = _mm_set1_epi32(x0);
            __m128i emmCornerX1 = _mm_set1_epi32(x1);
            __m128i emmCornerY0 = _mm_set1_epi32(y0);
            __m128i emmCornerY1 = _mm_set1_epi32(y1);
            __m128i emmCornerCond00 = _mm_cmpgt_epi32(
                _mm_sub_epi32(
                    _mm_add_epi32(emmC, MultiplyM128i(emmDX, emmCornerY0)),
                    MultiplyM128i(emmDY, emmCornerX0)
                ), emmZero
            );
            //bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0; 
            //bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0; 
            //bool c00 = C3 + DX34 * y0 - DY34 * x0 > 0; 
            //bool d00 = C4 + DX41 * y0 - DY41 * x0 > 0; 
            
            __m128i emmCornerCond10 = _mm_cmpgt_epi32(
                _mm_sub_epi32(
                    _mm_add_epi32(emmC, MultiplyM128i(emmDX, emmCornerY0)),
                    MultiplyM128i(emmDY, emmCornerX1)
                ), emmZero
            );
            //bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0; 
            //bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0; 
            //bool c10 = C3 + DX34 * y0 - DY34 * x1 > 0; 
            //bool d10 = C4 + DX41 * y0 - DY41 * x1 > 0; 

            __m128i emmCornerCond01 = _mm_cmpgt_epi32(
                _mm_sub_epi32(
                    _mm_add_epi32(emmC, MultiplyM128i(emmDX, emmCornerY1)),
                    MultiplyM128i(emmDY, emmCornerX0)
                ), emmZero
            );
            //bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0; 
            //bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0; 
            //bool c01 = C3 + DX34 * y1 - DY34 * x0 > 0; 
            //bool d01 = C4 + DX41 * y1 - DY41 * x0 > 0; 

            __m128i emmCornerCond11 = _mm_cmpgt_epi32(
                _mm_sub_epi32(
                    _mm_add_epi32(emmC, MultiplyM128i(emmDX, emmCornerY1)),
                 MultiplyM128i(emmDY, emmCornerX1)
                ), emmZero
            );
            //bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0; 
            //bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0; 
            //bool c11 = C3 + DX34 * y1 - DY34 * x1 > 0; 
            //bool d11 = C4 + DX41 * y1 - DY41 * x1 > 0; 
            
            __m128i emmCornerCondAnd = _mm_and_si128(
                _mm_and_si128(emmCornerCond00, emmCornerCond10),
                _mm_and_si128(emmCornerCond01, emmCornerCond11));
            __m128i emmCornerCondOr = _mm_or_si128(
                _mm_or_si128(emmCornerCond00, emmCornerCond10),
                _mm_or_si128(emmCornerCond01, emmCornerCond11));
            //int andA = a00 & a10 & a01 & a11; 
            //int andB = b00 & b10 & b01 & b11; 
            //int andC = c00 & c10 & c01 & c11; 
            //int andD = d00 & d10 & d01 & d11; 
            //int orA = a00 | a10 | a01 | a11; 
            //int orB = b00 | b10 | b01 | b11; 
            //int orC = c00 | c10 | c01 | c11; 
            //int orD = d00 | d10 | d01 | d11; 

            // 完全在一条边外面
            if (emmCornerCondOr.m128i_i32[0] == 0 || emmCornerCondOr.m128i_i32[1] == 0 
                || emmCornerCondOr.m128i_i32[2] == 0 ||emmCornerCondOr.m128i_i32[3] == 0)
            {
                fzLine0 += q * DZDX; // continue 会跳过
                continue; 
            }
            //if(orA == 0x0 || orB == 0x0 || orC == 0x0 || orD == 0x0){
            //    fzLine0 += q * DZDX; // continue 会跳过
            //    continue; 
            //}

            float *buffer = (float*)pZBuffer; 

            // 内部
            if ((emmCornerCondAnd.m128i_i32[0] && emmCornerCondAnd.m128i_i32[1] 
                && emmCornerCondAnd.m128i_i32[2] && emmCornerCondAnd.m128i_i32[3]))
            //if ((andA && andB && andC && andD)) 
            { 
                __m128 xmmFZ = _mm_set1_ps(fzLine0);
                xmmFZ = _mm_add_ps(xmmFZ, xmmDZDXStep);
                //float fz = fzLine0;
                for(int iy = 0; iy < q; iy++) 
                { 
                    __m128 xmmFZLine = xmmFZ;
                    //float fzLine = fz;
                    for(int ix = x; ix < x + q; ix += 4) 
                    { 
                        __m128 xmmFZBuf = _mm_load_ps(buffer + ix);
                        __m128 xmmCMPRes = _mm_cmpge_ps(xmmFZBuf, xmmFZLine);
                        if (xmmCMPRes.m128_i32[0] || xmmCMPRes.m128_i32[1] || xmmCMPRes.m128_i32[2] || xmmCMPRes.m128_i32[3])
                        //if (buffer[ix] >= fzLine)
                        {
                            m_nWrite++;
                            return;
                        }
                        xmmFZLine = _mm_add_ps(xmmFZLine, xmmDZDX);
                        //fzLine += DZDX; 
                    } 
                    xmmFZ = _mm_add_ps(xmmFZ, xmmDZDY);
                    //fz += DZDY;
                    (char*&)buffer += iPitch; 
                } 
            } 
            else // Partially covered block 
            { 
                
                __m128i emmCY = _mm_sub_epi32(
                    _mm_add_epi32(emmC, MultiplyM128i(emmDX, emmCornerY0)),
                    MultiplyM128i(emmDY, emmCornerX0)
                    ); 
                //int CY1 = C1 + DX12 * y0 - DY12 * x0; 
                //int CY2 = C2 + DX23 * y0 - DY23 * x0; 
                //int CY3 = C3 + DX34 * y0 - DY34 * x0; 
                //int CY4 = C4 + DX41 * y0 - DY41 * x0; 

                float fz = fzLine0;

                for(int iy = y; iy < y + q; iy++) 
                { 
                    __m128i emmCX = emmCY;
                    //int CX1 = CY1; 
                    //int CX2 = CY2; 
                    //int CX3 = CY3; 
                    //int CX4 = CY4; 
                    float fzLine = fz;

                    for(int ix = x; ix < x + q; ix++) 
                    { 
                        if(emmCX.m128i_i32[0] > 0 && emmCX.m128i_i32[1] > 0 && emmCX.m128i_i32[2] > 0 && emmCX.m128i_i32[3] > 0)
                        { 
                            if (buffer[ix] >= fzLine)
                            {
                                m_nWrite++;
                                return;
                            }
                        }
                        emmCX = _mm_sub_epi32(emmCX, emmFDY);
                        //CX1 -= FDY12; 
                        //CX2 -= FDY23; 
                        //CX3 -= FDY34; 
                        //CX4 -= FDY41; 
                        fzLine += DZDX; 
                    } 
                    emmCY = _mm_add_epi32(emmCY, emmFDX);
                    //CY1 += FDX12; 
                    //CY2 += FDX23; 
                    //CY3 += FDX34; 
                    //CY4 += FDX41;
                    fz += DZDY;
                    (char*&)buffer += iPitch; 
                } 
            } 
            fzLine0 += q * DZDX;
        } 
        fz0 += q * DZDY;
        pZBuffer += q * iPitch; 
    } 
}

void A3DOcclusionManSW::DrawIndexedQuads_SSE(const A3DVECTOR3* pVertices, int nVertNum, const WORD* pIndices, int nFaceNum)
{
    TransformVertices(pVertices, nVertNum);

    for (int i = 0; i < nVertNum; i++)
    {
        if (m_aTransVerts[i].z <= 0 || m_aTransVerts[i].w <= 0)
        {
            m_nWrite++;
            return;
        }
        m_aTransVerts[i].x /= m_aTransVerts[i].w; 
        m_aTransVerts[i].y /= m_aTransVerts[i].w; 
        m_aTransVerts[i].z /= m_aTransVerts[i].w; 
        m_aTransVerts[i].x = (m_aTransVerts[i].x + 1) / 2 * m_iWidth + 0.5f;
        m_aTransVerts[i].y = (-m_aTransVerts[i].y + 1) / 2 * m_iHeight + 0.5f;
    }
    const WORD* pInds = pIndices;
    A3DVECTOR4 pVerts[4];
    __m128i emmZero = _mm_set1_epi32(0);
    __m128i emmWidth = _mm_set1_epi32(m_iWidth - 1);
    __m128i emmHeight = _mm_set1_epi32(m_iHeight - 1);
    for (int i = 0; i < nFaceNum; i++)
    {
        pVerts[0] = m_aTransVerts[*pInds++];
        pVerts[1] = m_aTransVerts[*pInds++];
        pVerts[2] = m_aTransVerts[*pInds++];
        pVerts[3] = m_aTransVerts[*pInds++];

        bool bBlocked = false;

        // Check hierarchically
        if (m_bUseHierachy)
        {
            int iVertX0 = iround(pVerts[0].x - 0.5f);
            int iVertY0 = iround(pVerts[0].y - 0.5f);
            int iVertX1 = iround(pVerts[1].x - 0.5f);
            int iVertY1 = iround(pVerts[1].y - 0.5f);
            int iVertX2 = iround(pVerts[2].x - 0.5f);
            int iVertY2 = iround(pVerts[2].y - 0.5f);
            int iVertX3 = iround(pVerts[3].x - 0.5f);
            int iVertY3 = iround(pVerts[3].y - 0.5f);

            __m128i emmVertX = _mm_set_epi32(iVertX0, iVertX1, iVertX2, iVertX3);
            __m128i emmVertY = _mm_set_epi32(iVertY0, iVertY1, iVertY2, iVertY3);
            emmVertX = _mm_max_epi16(emmVertX, emmZero);
            emmVertX = _mm_min_epi16(emmVertX, emmWidth);
            emmVertY = _mm_max_epi16(emmVertY, emmZero);
            emmVertY = _mm_min_epi16(emmVertY, emmHeight);
            //a_Clamp(iVertX0, 0, m_iWidth - 1);
            //a_Clamp(iVertY0, 0, m_iHeight - 1);
            //a_Clamp(iVertX1, 0, m_iWidth - 1);
            //a_Clamp(iVertY1, 0, m_iHeight - 1);
            //a_Clamp(iVertX2, 0, m_iWidth - 1);
            //a_Clamp(iVertY2, 0, m_iHeight - 1);
            //a_Clamp(iVertX3, 0, m_iWidth - 1);
            //a_Clamp(iVertY3, 0, m_iHeight - 1);

            float fMinZ = a_Min(pVerts[0].z, pVerts[1].z, pVerts[2].z, pVerts[3].z);

            for (int iMip = m_pMipmapBuffer->GetMipmapCount() - 1; iMip >= 0; iMip--)
            {
                const A3DMipmapBuffer::MipmapRec* rec = m_pMipmapBuffer->GetMipmap(iMip);
                __m128i emmMipVertX = _mm_srli_epi32(emmVertX, rec->iShift);
                __m128i emmMipVertY = _mm_srli_epi32(emmVertY, rec->iShift);
                __m128i emmMipWidth = _mm_set1_epi32(rec->iWidth - 1);
                __m128i emmMipHeight = _mm_set1_epi32(rec->iHeight - 1);
                emmMipVertX = _mm_min_epi16(emmMipVertX, emmMipWidth);
                emmMipVertY = _mm_min_epi16(emmMipVertY, emmMipHeight);
                //int iMipX0, iMipY0;
                //int iMipX1, iMipY1;s
                //int iMipX2, iMipY2;
                //int iMipX3, iMipY3;
                //m_pMipmap->GetCoordInLevel(iMip, iVertX0, iVertY0, iMipX0, iMipY0);
                //m_pMipmap->GetCoordInLevel(iMip, iVertX1, iVertY1, iMipX1, iMipY1);
                //m_pMipmap->GetCoordInLevel(iMip, iVertX2, iVertY2, iMipX2, iMipY2);
                //m_pMipmap->GetCoordInLevel(iMip, iVertX3, iVertY3, iMipX3, iMipY3);

                // if all vertices are in the same pixel
                if (emmMipVertX.m128i_i32[0] == emmMipVertX.m128i_i32[1] 
                    && emmMipVertX.m128i_i32[1] == emmMipVertX.m128i_i32[2] 
                    && emmMipVertX.m128i_i32[2] == emmMipVertX.m128i_i32[3] 
                    && emmMipVertY.m128i_i32[0] == emmMipVertY.m128i_i32[1] 
                    && emmMipVertY.m128i_i32[1] == emmMipVertY.m128i_i32[2] 
                    && emmMipVertY.m128i_i32[2] == emmMipVertY.m128i_i32[3])
                {
                    float fVal = rec->pBuf[emmMipVertX.m128i_i32[0] + emmMipVertY.m128i_i32[0] * rec->iWidth];
                    if (fMinZ > fVal)
                    {
                        // all z greater than buffer Z, this primitive is occluded
                        bBlocked = true;
                        m_nBlocked++;
                        break;
                    }
                }
                else
                {
                    // vertices can't be in the same pixel in above levels 
                    break;
                }
            }
        }

        if (!bBlocked)
        {
            // passed early check, do filling
            FillQuad_HalfEdge_SSE(pVerts[0], pVerts[1], pVerts[2], pVerts[3]);
        }
    }
}

int A3DOcclusionManSW::GetState(int iState) const
{
    switch (iState)
    {
    case A3DOS_TOTAL_QUERY:
        return m_aQueries.GetSize();
    case A3DOS_VISIBLE_QUERY:
        return m_nVisible;
    case A3DOS_EARLY_PASSED_QUERY:
        return m_nPassed;
    case A3DOS_MISSED_QUERY:
        return 0;
    case A3DOS_AVAILID_QUERY:
        return m_nAvailable;
    case A3DOS_SW_HIERARCHY:
        return m_bUseHierachy;
    case A3DOS_SW_SSE2:
        return m_bUseSSE;

    }
    return -1;
}

void A3DOcclusionManSW::SetState(int iState, int iValue)
{
    switch (iState)
    {
    case A3DOS_SW_HIERARCHY:
        m_bUseHierachy = iValue != 0;
    case A3DOS_SW_SSE2:
        m_bUseSSE = iValue != 0;
    }
}

static bool IsCPUSupportSSE2()
{
    // cpuid结果：edx 第25位SSE，第26位SSE2
    unsigned int iEDX;
    static const unsigned int iMask = (1 << 25) | (1 << 26);
#ifdef _WIN64
    return true; // x64 不支持内嵌汇编，假设都支持
#else
    __asm
    {
        mov eax, 1
        cpuid
        mov iEDX, edx
    }

    return (iEDX & iMask) == iMask;
#endif
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


bool A3DOcclusionManSW::CheckSupport()
{
    if (!IsDepthFormatOk(m_pDevice, A3DFMT_INTZ))
    {
        g_A3DErrLog.Log("OcclusionManSW::CheckSupport, INTZ format depth buffer is not supported");
        return false;
    }
    if (!IsRenderTargetFormatOk(m_pDevice, (A3DFORMAT)D3DFMT_R32F))
    {
        g_A3DErrLog.Log("OcclusionManSW::CheckSupport, R32F format color buffer is not supported");
        return false;
    }
    if (!IsCPUSupportSSE2())
    {
        g_A3DErrLog.Log("OcclusionManSW::CheckSupport, SSE2 is not supported");
        return false;
    }
    return true;
}