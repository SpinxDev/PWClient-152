/*
 * FILE: A3DSkinMeshMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/4/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINMESHMAN_H_
#define _A3DSKINMESHMAN_H_

#include "AArray.h"
#include "AString.h"
#include "hashtab.h"

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

class A3DEngine;
class A3DDevice;
class A3DMeshBase;
class A3DSkinMesh;
class A3DRigidMesh;
class A3DMorphSkinMesh;
class A3DMorphRigidMesh;
class A3DSuppleMesh;
class A3DMuscleMesh;
class A3DSkin;
class A3DStream;
class AFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinMeshMan
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinMeshMan
{
public:		//	Types

	//	Skin node
	struct SKIN
	{
		AString		strSkinFile;	//	Skin file name
		SKIN*		pNext;			//	Next skin node

		APtrArray<A3DSkinMesh*>			aSkinMeshes;		//	Mesh array
		APtrArray<A3DRigidMesh*>		aRigidMeshes;		//	Rigid mesh array
		APtrArray<A3DMorphSkinMesh*>	aMorphSkMeshes;		//	Morph skin mesh array
		APtrArray<A3DMorphRigidMesh*>	aMorphRdMeshes;		//	Morph rigid mesh array
		APtrArray<A3DSuppleMesh*>		aSuppleMeshes;		//	Supple mesh array
		APtrArray<A3DMuscleMesh*>		aMuscleMeshes;		//	Muscle mesh array
	};

	typedef abase::hashtab<SKIN*, int, abase::_hash_function>	SkinTable;

public:		//	Constructor and Destructor

	A3DSkinMeshMan();
	virtual ~A3DSkinMeshMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Tick routine
	void Tick(DWORD dwDeltaTime);

	//	Load mesh object from file
	A3DMeshBase* LoadMeshObject(AFile* pFile, A3DSkin* pSkin, int iMeshClassID, int iMeshIdx, int iMaxMeshNum, int nSkinVersion);
	//	Release all mesh object of specified skin
	void ReleaseAllSkinMesh(A3DSkin* pSkin);

	//	Calculate approximate data size of all meshes
	int CalcDataSize();

	//	Request cloth stream
	A3DStream* RequestClothStream(int iVertNum, int iIndexNum);

protected:	//	Attributes

	A3DEngine*			m_pA3DEngine;
	A3DDevice*			m_pA3DDevice;
	SkinTable			m_SkinTab;			//	Skin table
	CRITICAL_SECTION	m_cs;				//	Skin list lock
	DWORD				m_dwTimeCnt;		//	Garbage collect time counter
	A3DStream*			m_pClothStream;		//	Cloth mesh shared stream

protected:	//	Operations

	//	Get skin node with specified ID and file name
	SKIN* GetSkin(int idSkin, const char* szFileName);
	//	Load mesh object from file
	A3DMeshBase* LoadMeshObject(AFile* pFile, A3DSkin* pSkin, int iMeshClassID, int nSkinVersion);
	//	Check whether a skin's all meshes have been released
	bool IsGarbageSkin(SKIN* pSkinNode);
	//	Release skin node
	void ReleaseSkinNode(SKIN* pSkinNode);
	//	Calcuate data size of skin node
	int CalculateSkinSize(SKIN* pSkinNode);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINMESHMAN_H_
