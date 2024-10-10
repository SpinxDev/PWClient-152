/*
 * FILE: A3DMeshBase.h
 *
 * DESCRIPTION: A3D mesh base class
 *
 * CREATED BY: duyuxin, 2003/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMESHBASE_H_
#define _A3DMESHBASE_H_

#include "A3DGeometry.h"
#include "A3DObject.h"
#include "A3DMaterial.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class AFile;
class A3DViewport;
class A3DSkin;
class A3DTexture;
class A3DSkinModel;
class A3DVertexShader;
class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DMeshBase
//
///////////////////////////////////////////////////////////////////////////

class A3DMeshBase : public A3DObject
{
public:		//	Types

	//	Mesh type
	enum
	{
		ID_SKINMESH = 0,	//	Skin mesh
		ID_RIGIDMESH,		//	Rigid mesh
		ID_FACEMESH,		//	Face mesh
	};

	enum
	{	
		RV_OK = 0,
		RV_UV_MIRROR_ERR = 1,
		RV_UNKNOWN_ERR
	};

public:		//	Constructors and Destructors

	A3DMeshBase();
	virtual ~A3DMeshBase() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion) { return true; }
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile,int nSkinVersion, int& nReturnValue) { return true; }

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin) { return true; }

	//	Get texture pointer
	virtual A3DTexture* GetTexturePtr(A3DSkin* pSkin);
	//	Get normal map pointer
	virtual A3DTexture* GetNormalMapPtr(A3DSkin* pSkin);
	//	Get current material
	virtual const A3DMaterial* GetMaterialPtr(A3DSkin* pSkin);
	//	Get vertex number
	virtual int GetVertexNum() { return m_iNumVert; }
	//	Get index number
	virtual int GetIndexNum() { return m_iNumIdx; }
	//	Get approximate mesh data size
	virtual int GetDataSize() { return 0; }

	//	Get texture index
	int GetTextureIndex() { return m_iTexture; }
	//	Set texture index
	void SetTextureIndex(int iIndex) { m_iTexture = iIndex; }
	//	Get material index
	int GetMaterialIndex() { return m_iMaterial; }
	//	Set material index
	void SetMaterialIndex(int iIndex) { m_iMaterial = iIndex; }
	//	Get mesh center
	const A3DVECTOR3& GetMeshCenter() { return m_aabbInitMesh.Center; }
	//	Get initial mesh AABB
	const A3DAABB& GetInitMeshAABB() { return m_aabbInitMesh; } 

	//	Reference counter operation
	void AddRefCount() { m_iRefCount++; }
	int DecRefCount() { m_iRefCount--; ASSERT(m_iRefCount >= 0); return m_iRefCount; }
	int GetRefCount() { return m_iRefCount; }

	//	Get engine interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }

protected:	//	Attributes

	A3DEngine*	m_pA3DEngine;
	int			m_iRefCount;		//	Reference counter	
	int			m_iTexture;			//	Texture index
	int			m_iMaterial;		//	Material index
	int			m_iNumVert;			//	number of vertex
	int			m_iNumIdx;			//	number of index
	A3DAABB		m_aabbInitMesh;		//	Initial mesh aabb

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMESHBASE_H_

