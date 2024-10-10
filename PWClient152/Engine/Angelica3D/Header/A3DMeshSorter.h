/*
 * FILE: A3DMeshSorter.h
 *
 * DESCRIPTION: Routines for sorting meshes
 *
 * CREATED BY: duyuxin, 2001/12/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMeshSorterER_H_
#define _A3DMeshSorterER_H_

#include "A3DTypes.h"
#include "AList2.h"
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

class A3DMesh;
class A3DLight;
class A3DDevice;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DMeshSorter
//
///////////////////////////////////////////////////////////////////////////

class A3DMeshSorter
{
public:		//	Types

	struct MESHNODE
	{
		A3DMesh*			pMesh;			//	Mesh's address
		int					iCurFrame;		//	Mesh's current frame
		A3DMATRIX4			matTrans;		//	Mesh's translate matrix
		float				fWeight;		//	Mesh's weight used for sorting
		A3DIBLLIGHTPARAM 	iblLightParam;	//	parameter describe current ibl light
	};

public:		//	Constructors and Destructors

	A3DMeshSorter();
	virtual ~A3DMeshSorter();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice);	//	Initialize object
	void		Release();					//	Release objbect

	bool		InsertMesh(A3DMATRIX4& matTrans, A3DIBLLIGHTPARAM& iblLightParam, int iCurFrame, A3DMesh* pMesh);	//	Insert a mesh
	void		RemoveAllMeshes();			//	Remove all meshes from list

	void		SetCameraPos(const A3DVECTOR3& vPos) { m_vCameraPos = vPos; }
	void		SetSortOrder(bool bIncrease) { m_bIncrease = bIncrease;	}

	bool		Render(A3DViewport* pCurViewport);	//	Render sorted meshes

protected:	//	Attributes

	A3DDevice*	m_pDevice;			//	A3DDevice

	bool		m_bIncrease;		//	true, increase sorting
	A3DVECTOR3	m_vCameraPos;		//	Camera position
	int			m_iCurNode;			//	Current node in m_aMeshNodes

	APtrArray<MESHNODE*>	m_aMeshNodes;	//	Mesh node buffer
	APtrList<MESHNODE*>		m_MeshList;		//	Mesh list

	// IBL Light sections;
	A3DLight	* m_pIBLStaticLight;//	a static light used for IBL scene
	A3DLight	* m_pIBLDynamicLight;// a dynamic light used for IBL scene
protected:	//	Operations

public:
	inline void SetIBLLight(A3DLight * pStaticLight, A3DLight * pDynamicLight) 
	{ m_pIBLStaticLight = pStaticLight; m_pIBLDynamicLight = pDynamicLight; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMeshSorterER_H_


