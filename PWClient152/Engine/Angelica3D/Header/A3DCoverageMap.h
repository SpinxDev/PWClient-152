/*
 * FILE: A3DCoverageMap.h
 *
 * DESCRIPTION: Class for generating small depth buffer with max mip-maping
 *
 * CREATED BY: Yaochunhui, 2012/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DCoverageMap_H_
#define _A3DCoverageMap_H_

#include "A3DTypes.h"
#include "A3DDevObject.h"


/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////
class A3DEngine;
class A3DDevice;
class A3DCameraBase;
class A3DHLSL;
class A3DHLSLMan;
class A3DRenderTarget;
class A3DTexture;
class A3DStream;
class A3DVertexDecl;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DCoverageMap
//
///////////////////////////////////////////////////////////////////////////
class A3DCoverageMap : public A3DDevObject
{
public:
    A3DCoverageMap();
    ~A3DCoverageMap();

    bool Init(A3DEngine* pEngine);
    void Release();
    
    // Get mip-map depth buffer
    A3DRenderTarget* GetSmallDepthBuffer() const;

    // Get size of small depth buffer   
    int GetFinalWidth() const  { return m_iFinalWidth; }
    int GetFinalHeight() const { return m_iFinalHeight; }

    // Generate max filtered mip-map
    // if bUseDepthTexture = true, depth texture of render target is used, otherwise we use color texture.
    // if bWriteDepthBuffer = true, depth will be filled into small depth buffer which can be retrieved by GetSmallDepthBuffer(),
    // otherwise depth is only available from ReadbackSmallBuffer()
    bool GenerateMipmap(A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture, bool bWriteDepthBuffer);
    
    // Read back mip-map depth buffer to pBuf, iPitch is bytes per line in pBuf
    bool ReadbackSmallBuffer(float* pBuf, int iPitch);
    
    // for debug
    void ShowDepthMap(A3DRenderTarget* pDepthTexture, A3DCameraBase* pCamera, bool bUseDepthTexture);
    // for debug
    int GetMipmapCount() const { return m_nMipmapCount; }
    // for debug
    A3DRenderTarget* GetMipmap(int i) const { return m_pMipmapArray[i]; }

protected:
    bool CreateResources();
    void ReleaseResources();

    bool CreateStream();
    // generate depth-stencil buffer containing mip-mapped depth buffer
    bool WriteSmallDepthBuffer(A3DRenderTarget* pDepthTexture);
    void DrawQuad(int x, int y, int w, int h);

    // A3DDevObject
    virtual bool BeforeDeviceReset();
    virtual bool AfterDeviceReset();

protected:                  
    A3DRenderTarget*        m_pSmallDepthBuffer;
    A3DRenderTarget**       m_pMipmapArray;
    A3DHLSL*                m_pHLSLMaxDownSample;
    A3DHLSL*                m_pHLSLWriteDepth;
    A3DHLSL*                m_pHLSLShowDepth;
    A3DDevice*              m_pDevice;
    A3DHLSLMan*             m_pHLSLMan;  
    A3DStream*              m_pStreamQuad;
    A3DVertexDecl*          m_pVertexDecl;
    IDirect3DSurface9*      m_pOffSurface;

    int                     m_nMipmapCount;
    int                     m_iFinalWidth;
    int                     m_iFinalHeight;
    int                     m_iWidth;
    int                     m_iHeight;
};

#endif