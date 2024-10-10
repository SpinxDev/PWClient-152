/*
 * FILE: A3DOcclusionMan.h
 *
 * DESCRIPTION: Interface for object occlusion test
 *
 * CREATED BY: Yaochunhui, 2012/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DOCCLUSIONMAN_H_
#define _A3DOCCLUSIONMAN_H_

#include "A3DTypes.h"
#include "A3DPlatform.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

enum A3DOcclusionState
{
    A3DOS_TOTAL_QUERY = 0,      // Get number of total queries
    A3DOS_AVAILID_QUERY,        // Get number of availed queries (exclude disabled and out of frustum)
    A3DOS_VISIBLE_QUERY,        // Get number of visible queries
    A3DOS_EARLY_PASSED_QUERY,   // Get number of queries that are early passed by algorithm
    A3DOS_MISSED_QUERY,         // Get number of queries that are failed to get query result
    A3DOS_END,
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////
class A3DAABB;
class A3DCamera;
class A3DEngine;
class A3DViewport;
class A3DRenderTarget;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Interface A3DOcclusionProxy
//
///////////////////////////////////////////////////////////////////////////

// Proxy interface for occlusion query
// Each object should have an individual proxy object
class A3DOcclusionProxy
{
protected:
    // Deleting directly is not allowed
    virtual ~A3DOcclusionProxy() {}

public:
    // Set AABB of object
    virtual void SetAABB(const A3DAABB& aabb) = 0;

    // Get result of occlusion query.
    // Should be called after A3DOcclusionMan::TestAll()
    // If query process is not finished, this method returns false
    virtual bool IsOccluded(A3DViewport* pViewport) = 0;
    
    // Get & set whether it is enabled
    virtual bool GetEnabled() const = 0;
    virtual void SetEnabled(bool bEnabled) = 0;
};

///////////////////////////////////////////////////////////////////////////
//
//	Interface A3DOcclusionMan
//
///////////////////////////////////////////////////////////////////////////

class A3DOcclusionMan
{
public:
    virtual ~A3DOcclusionMan() {}
    virtual bool Init(A3DEngine* pEngine) = 0;

    // Release
    virtual void Release() = 0;
    
    // Create a query proxy
    virtual A3DOcclusionProxy* CreateQuery() = 0;
    
    // Release a query proxy. 
    virtual void ReleaseQuery(A3DOcclusionProxy* pProxy) = 0;
    
    // Clear all created proxy objects
    virtual void Clear() = 0;
    
    // Set depth buffer for occlusion testing
    // pCamera: current camera
    // pDepthTexture: texture containing z buffer
    // if bUseDepthTexture = true, depth texture of render target is used, otherwise we use color texture.
    virtual bool PrepareCoverageMap(A3DViewport* pViewport, A3DRenderTarget* pDepthRenderTarget, bool bUseDepthTexture) = 0;

    // Test all enabled queries, iFlag is not used now
    virtual bool TestAll(int iFlag) = 0;

    virtual void SetState(int iState, int iValue) = 0;

    virtual int GetState(int iState) const = 0;
};

#endif