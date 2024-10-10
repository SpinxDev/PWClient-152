/*
 * FILE: A3DMeshBase.cpp
 *
 * DESCRIPTION: A3D mesh base class
 *
 * CREATED BY: duyuxin, 2003/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMeshBase.h"
#include "A3DPI.h"
#include "A3DSkin.h"
#include "A3DTexture.h"
#include "A3DEngine.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement of A3DMeshBase
//
///////////////////////////////////////////////////////////////////////////

A3DMeshBase::A3DMeshBase()
{
	m_dwClassID		= A3D_CID_MESHBASE;
	m_pA3DEngine	= NULL;
	m_iNumVert		= 0;
	m_iNumIdx		= 0;
	m_iTexture		= -1;
	m_iMaterial		= 0;
	m_iRefCount		= 1;

	m_aabbInitMesh.Clear();
}

//	Initialize object
bool A3DMeshBase::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	return true;
}

//	Release object
void A3DMeshBase::Release()
{
}

//	Get texture pointer
A3DTexture* A3DMeshBase::GetTexturePtr(A3DSkin* pSkin)
{
	if (pSkin && m_iTexture >= 0)
		return pSkin->GetTexture(m_iTexture);

	return NULL;
}

//	Get normal map pointer
A3DTexture* A3DMeshBase::GetNormalMapPtr(A3DSkin* pSkin)
{
	if (pSkin && m_iTexture >= 0)
		return pSkin->GetNormalMap(m_iTexture);

	return NULL;
}

//	Restore material to original value
const A3DMaterial* A3DMeshBase::GetMaterialPtr(A3DSkin* pSkin)
{
	if (pSkin && m_iMaterial >= 0)
		return pSkin->GetMaterial(m_iMaterial);

	return NULL;
}
