/*
 * FILE: A3DOcclusionManSW.h
 *
 * DESCRIPTION: class for object occlusion test using software rasterization
 *
 * CREATED BY: Yaochunhui, 2012/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DOCCLUSIONMAN_SW_H_
#define _A3DOCCLUSIONMAN_SW_H_

#include "A3DOcclusionMan.h"
#include "AArray.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////
enum A3DOcclusionStateSW
{
    A3DOS_SW_HIERARCHY = A3DOS_END + 1,     // Get/set whether use hierarchy optimization
    A3DOS_SW_SSE2,                          // Get/set whether use SSE2 optimization
};
///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////
class A3DOcclusionProxySW;
class A3DCoverageMap;
class A3DEngine;
class A3DStream;
class A3DDevice;
class A3DCameraBase;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Template AAlignedBuffer
//
///////////////////////////////////////////////////////////////////////////

template<class T>
class AAlignedBuffer
{
public:		//	Types

public:		//	Constructors and Destructors
    AAlignedBuffer();
    ~AAlignedBuffer();

public:		//	Attributes

public:		//	Operations

    // Initializing with sizes. Calling for multiple times is safe.
    // Params:
    //      iWidth, iHeight: element count per row and per column
    //      iAlignDataBytes: bytes of required memory alignment
    //      iAlignWidthBytes: bytes of required row alignment
    //      iAlignHeightBytes: bytes of required column alignment
    void Init(int iWidth, int iHeight, int iAlignDataBytes, int iAlignWidthBytes, int iAlignHeightBytes);

    void Release();

    int GetWidth() const { return m_iWidth; }

    int GetHeight() const { return m_iWidth; }

    int GetWidthBytes() const { return m_iWidthBytes; }

    int GetHeightBytes() const { return m_iHeightBytes; }

    T* GetData() const { return m_pBuf; }

    T* GetLineData(int i) const { return (T*)((char*) m_pBuf + m_iWidthBytes * i); }

private:
    char* m_pOrgBuf;
    T* m_pBuf;
    int m_iWidthBytes;
    int m_iHeightBytes;
    int m_iWidth;
    int m_iHeight;

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    AAlignedBuffer(const AAlignedBuffer&);
    AAlignedBuffer& operator=(const AAlignedBuffer&);
};


///////////////////////////////////////////////////////////////////////////
//
//	Implement AAlignedBuffer
//
///////////////////////////////////////////////////////////////////////////

template<class T>
AAlignedBuffer<T>::AAlignedBuffer()
{
    m_pOrgBuf = NULL;
    m_pBuf = NULL;
    m_iWidthBytes = 0;
    m_iHeightBytes = 0;
    m_iWidth = 0;
    m_iHeight = 0;
}

template<class T>
AAlignedBuffer<T>::~AAlignedBuffer()
{
    Release();
}


template<class T>
void AAlignedBuffer<T>::Init(int iWidth, int iHeight, int iAlignDataBytes, int iAlignWidthBytes, int iAlignHeightBytes)
{
    Release();

    m_iWidth = iWidth;
    m_iHeight = iHeight;

    m_iWidthBytes = (m_iWidth * sizeof(T) + iAlignWidthBytes - 1) / iAlignWidthBytes * iAlignWidthBytes;
    m_iHeightBytes = (m_iHeight * sizeof(T) + iAlignHeightBytes - 1) / iAlignHeightBytes * iAlignHeightBytes;

    int nTotalBytes = m_iWidthBytes * m_iHeightBytes + iAlignDataBytes;

    m_pOrgBuf = new char[nTotalBytes];

    m_pBuf = (T*)((LONG_PTR)(m_pOrgBuf + iAlignDataBytes - 1) / iAlignDataBytes * iAlignDataBytes);
}

template<class T>
void AAlignedBuffer<T>::Release()
{
    delete m_pOrgBuf;
    m_pOrgBuf = NULL;
    m_pBuf = NULL;
    m_iWidthBytes = 0;
    m_iHeightBytes = 0;
    m_iWidth = 0;
    m_iHeight = 0;
}

//////////////////////////////////////////////////////////////////////////
//
//   A3DMipmapBuffer : software generating max mip-mapping
//
//////////////////////////////////////////////////////////////////////////

class A3DMipmapBuffer
{
public:
    // information for each mip-map level
    struct MipmapRec
    {
        int iWidth;  // width of level
        int iHeight; // height of level
        int iShift;  // bits to shift right from top level
        float* pBuf; // data buffer
    };

    A3DMipmapBuffer();
    ~A3DMipmapBuffer();

    void Init(int iWidth, int iHeight);
    void Release();

    int GetWidth() const { return m_iWidth; }
    int GetHeight() const { return m_iHeight; }

    int GetMipmapCount() const { return m_arrMipmaps.GetSize(); }
    const MipmapRec* GetMipmap(int iIndex) const;

    void UpdateMipmaps(float* pBuf, int iPitch);
    void GetCoordInLevel(int iLevel, int iX, int iY, int& iOutX, int& iOutY) const;

private:
    int m_iWidth;
    int m_iHeight;
    AArray<MipmapRec*> m_arrMipmaps;
};

///////////////////////////////////////////////////////////////////////////
//
//	class A3DOcclusionManSW
//
///////////////////////////////////////////////////////////////////////////

class A3DOcclusionManSW : public A3DOcclusionMan
{
public:
    A3DOcclusionManSW();
    virtual ~A3DOcclusionManSW();

    virtual bool Init(A3DEngine* pEngine);

    // Release
    virtual void Release();

    // Create a query proxy
    virtual A3DOcclusionProxy* CreateQuery();

    // Release a query proxy. 
    virtual void ReleaseQuery(A3DOcclusionProxy* pProxy);

    // Clear all created proxy objects
    virtual void Clear();

    // Set depth buffer for occlusion testing
    // pCamera: current camera
    // pDepthTexture: texture containing z buffer
    virtual bool PrepareCoverageMap(A3DViewport* pViewport, A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture);

    // Test all enabled queries, iFlag is not used now
    virtual bool TestAll(int iFlag);


    virtual void SetState(int iState, int iValue);
    virtual int GetState(int iState) const;

protected:
    bool TestAABBQuad(const A3DAABB& aabb);
    void TransformVertices(const A3DVECTOR3* pVert, const int nVertNum);

    void FillQuad_HalfEdge(const A3DVECTOR4 &v1, const A3DVECTOR4 &v2, const A3DVECTOR4 &v3, const A3DVECTOR4 &v4);
    void DrawIndexedQuads(const A3DVECTOR3* pVertices, int nVertNum, const WORD* pIndices, int nFaceNum);

    void FillQuad_HalfEdge_SSE(const A3DVECTOR4 &v1, const A3DVECTOR4 &v2, const A3DVECTOR4 &v3, const A3DVECTOR4 &v4);
    void DrawIndexedQuads_SSE(const A3DVECTOR3* pVertices, int nVertNum, const WORD* pIndices, int nFaceNum);
    bool CheckSupport();

protected:
    AArray<A3DOcclusionProxySW*> m_aQueries;
    AAlignedBuffer<float> m_aAlignedZBuffer;
    AArray<A3DVECTOR4> m_aTransVerts;

    int m_iWidth;
    int m_iHeight;
    A3DCoverageMap* m_pCoverageMap;
    A3DCameraBase* m_pCamera;
    A3DViewport* m_pViewport;
    A3DDevice* m_pDevice;
    A3DMipmapBuffer* m_pMipmapBuffer;
    A3DMATRIX4 m_matWVP;

    int m_nWrite;
    bool m_bUseHierachy;
    bool m_bUseSSE;
    bool m_bCoverageMapReady;

    int m_nPassed;
    int m_nAvailable;
    int m_nVisible;
    int m_nBlocked;
    
    friend class A3DOcclusionProxySW;
};

#endif