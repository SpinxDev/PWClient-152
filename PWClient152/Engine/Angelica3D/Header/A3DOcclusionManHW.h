/*
 * FILE: A3DOcclusionManHW.h
 *
 * DESCRIPTION: class for object occlusion test using hardware occlusion query
 *
 * CREATED BY: Yaochunhui, 2012/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DOCCLUSIONMAN_HW_H_
#define _A3DOCCLUSIONMAN_HW_H_

#include "A3DOcclusionMan.h"
#include "AArray.h"
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
class A3DOcclusionProxyHW;
class A3DCoverageMap;
class A3DEngine;
class A3DStream;
class A3DCameraBase;
class A3DRenderTarget;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	class A3DOcclusionManHW
//
///////////////////////////////////////////////////////////////////////////

class A3DOcclusionManHW : public A3DOcclusionMan, public A3DDevObject
{
public:
    A3DOcclusionManHW();
    virtual ~A3DOcclusionManHW();

    bool Init(A3DEngine* pEngine);

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

    virtual bool BeforeDeviceReset();
    virtual bool AfterDeviceReset();

protected:
    AArray<A3DOcclusionProxyHW*> m_aQueries;
    int m_iWidth;
    int m_iHeight;
    A3DDevice* m_pDevice;
    A3DCoverageMap* m_pCoverageMap;
    A3DCameraBase* m_pCamera;
    A3DViewport* m_pViewport;
    A3DStream* m_pStreamAABB;
    bool m_bCoverageMapReady;
    CRITICAL_SECTION m_cs;

    int m_nPassed;
    int m_nAvailable;

    DWORD m_nFrameCount;

    void DrawAABB(const A3DAABB& aabb);
    bool CreateAABBStream();
    bool CheckSupport();
    
    friend class A3DOcclusionProxyHW;
};

#endif