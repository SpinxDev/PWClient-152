/*
 * FILE: A3DSkin.h
 *
 * DESCRIPTION: A3D skin class
 *
 * CREATED BY: duyuxin, 2003/9/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKIN_H_
#define _A3DSKIN_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include "AString.h"
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

class A3DEngine;
class A3DDevice;
class A3DTexture;
class A3DMeshBase;
class A3DSkinMesh;
class A3DRigidMesh;
class A3DMorphSkinMesh;
class A3DMorphRigidMesh;
class A3DSuppleMesh;
class A3DMuscleMesh;
class A3DClothMeshImp;
class A3DMaterial;
class A3DViewport;
class A3DSkinModel;
class A3DSkinMeshMan;
class A3DVertexShader;
class A3DSkeleton;
class AFile;

struct A3DSkinMeshRef
{
	friend class A3DSkin;

	A3DMeshBase*		pMesh;			//	Mesh object
	A3DClothMeshImp*	pClothImp;		//	Mesh's cloth implement
	bool				bSWRender;		//	Software render
	bool				bInvisible;		//	hide flag of this mesh
	float				fTrans;			//	Transparent value
	int					iAlphaWeight;	//	Alpha sort weight, more bigger more closer
	A3DVertexShader*	pvs;			//	Special vertex shader for this skin

	A3DSkinMeshRef()
	{
		pMesh			= NULL;
		pClothImp		= NULL;
		bSWRender		= false;
		bInvisible		= false;
		iAlphaWeight	= 0;
		fTrans			= -1.0f;
		pvs				= NULL;
		pvsBackup		= NULL;
	}
	
protected:
	
	A3DVertexShader*	pvsBackup;		//	Vertex shader backup
};

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

//	Identity and version of skin file
#define	SKINFILE_IDENTIFY	(('A'<<24) | ('S'<<16) | ('K'<<8) | 'I')
#define SKINFILE_VERSION	9
#define SKINFILE_TANGENT_VERSION 101
///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkin
//
///////////////////////////////////////////////////////////////////////////

class A3DSkin : public A3DObject
{
public:		//	Types

	typedef AArray<A3DSkinMeshRef>	MeshRef;

	friend class A3DSkinMan;

public:		//	Constructors and Destructors

	A3DSkin();
	virtual ~A3DSkin();

public:		//	Attributes

public:		//	Operations

	//	Save skin data to file
	bool Save(AFile* pFile, int nVersion, int& nReturnValue);
	bool Save(const char* szFile, int nVersion, int& nReturnValue);
	
	//	Set currently active morph channel
	void SetActiveMorphChannelNum(int iNum);
	//	Change skin texture
	bool ChangeSkinTexture(int iIndex, const char* szTexFile, bool bDelOld=true);
	bool ChangeSkinTexturePtr(int iIndex, A3DTexture* pTexture, bool bDelOld=true);

	//	Set/Get number of bone involved in this skin
	int GetSkinBoneNum();
	//	Set/Get minimum weight limit
	void SetMinWeight(float fMinWeight) { m_fMinWeight = fMinWeight; }
	float GetMinWeight() { return m_fMinWeight; }
	//	Set/Get number of bone of skeleton to which the skin attached
	void SetSkeletonBoneNum(int iNum) { m_iNumSkeBone = iNum; }
	int GetSkeletonBoneNum() { return m_iNumSkeBone; }
	//	Set software skin mesh flag. Set before Load() is called
	void UseSoftwareRender(bool bSoftware) { m_bSoftRender = bSoftware; }
	//	Set transparent value of skin
	void SetTransparent(float fTransparent);
	//	Bind this skin with specify skeleton
	bool BindSkeleton(A3DSkeleton* pSkeleton);

	//	Generate cloth meshes
	bool GenerateClothes(int iNumRigidMesh, int* aRigidMeshes, int iNumSkinMesh, int* aSkinMeshes);
	//	Release cloth meshes
	void ReleaseClothes();
	//	Enable / Disable cloth meshes
	void EnableClothes(bool bEnable) { m_bEnableCloth = bEnable; }
	//	Get cloth enable flag
	bool IsClothesEnable() { return m_bEnableCloth; }

	//	Get skin file name
	const char* GetFileName() { return m_strFileName; }
	//	Get skin's ID
	DWORD GetSkinID() { return m_dwSkinID; }
	//	Get mesh number
	int GetSkinMeshNum() { return m_aSkinMeshes.GetSize(); }
	int GetRigidMeshNum() { return m_aRigidMeshes.GetSize(); }
	int GetMorphSkinMeshNum() { return m_aMorphSkMeshes.GetSize(); }
	int GetMorphRigidMeshNum() { return m_aMorphRdMeshes.GetSize(); }
	int GetSuppleMeshNum() { return m_aSuppleMeshes.GetSize(); }
	int GetMuscleMeshNum() { return m_aMuscleMeshes.GetSize(); }
	int GetClothMeshNum() { return m_aClothMeshes.GetSize(); }
	//	Get texture number
	int GetTextureNum() { return m_aTextures.GetSize(); }
	//	Get material number
	int GetMaterialNum() { return m_aMaterials.GetSize(); }
	//	Get skin mesh
	A3DSkinMeshRef& GetSkinMeshRef(int n) { return m_aSkinMeshes[n]; }
	A3DSkinMeshRef* GetSkinMeshRefByName(const char* szName);
	A3DSkinMesh* GetSkinMesh(int n) { return (A3DSkinMesh*)m_aSkinMeshes[n].pMesh; }
	//	Get rigid mesh
	A3DSkinMeshRef& GetRigidMeshRef(int n) { return m_aRigidMeshes[n]; }
	A3DSkinMeshRef* GetRigidMeshRefByName(const char* szName);
	A3DRigidMesh* GetRigidMesh(int n) { return (A3DRigidMesh*)m_aRigidMeshes[n].pMesh; }
	//	Get morph skin mesh by index
	A3DSkinMeshRef& GetMorphSkinMeshRef(int n) { return m_aMorphSkMeshes[n]; }
	A3DMorphSkinMesh* GetMorphSkinMesh(int n) { return (A3DMorphSkinMesh*)m_aMorphSkMeshes[n].pMesh; }
	//	Get morph rigid mesh by index
	A3DSkinMeshRef& GetMorphRigidMeshRef(int n) { return m_aMorphRdMeshes[n]; }
	A3DMorphRigidMesh* GetMorphRigidMesh(int n) { return (A3DMorphRigidMesh*)m_aMorphRdMeshes[n].pMesh; }
	//	Get supple mesh by index
	A3DSkinMeshRef& GetSuppleMeshRef(int n) { return m_aSuppleMeshes[n]; }
	A3DSuppleMesh* GetSuppleMesh(int n) { return (A3DSuppleMesh*)m_aSuppleMeshes[n].pMesh; }
	//	Get muscle mesh by index
	A3DSkinMeshRef& GetMuscleMeshRef(int n) { return m_aMuscleMeshes[n]; }
	A3DMuscleMesh* GetMuscleMesh(int n) { return (A3DMuscleMesh*)m_aMuscleMeshes[n].pMesh; }
	//	Get cloth mesh by index
	A3DSkinMeshRef& GetClothMeshRef(int n) { return m_aClothMeshes[n]; }
	A3DClothMeshImp* GetClothMeshImp(int n) { return (A3DClothMeshImp*)m_aClothMeshes[n].pMesh; }
	//	Get texture by index
	A3DTexture* GetTexture(int n) { return m_aTextures[n]; }
	//	Get normal map by index
	A3DTexture * GetNormalMap(int n) { return (n < m_aNormalMaps.GetSize() ? m_aNormalMaps[n] : NULL); }
	//	Get material by index
	A3DMaterial* GetMaterial(int n) { return m_aMaterials[n]; }
	//	Get approximate skin data
	int GetDataSize() { return m_iDataSize; }
	//	Get initial aabb of all meshes
	A3DAABB GetInitMeshAABB();
	//	Get skin file version
	DWORD GetFileVersion() { return m_dwLoadVer; }
	//	Map skin bone index to skeleton bone index
	int MapBoneIndex(int iSkinBoneIdx) { return m_aBoneIdxMap[iSkinBoneIdx]; }
	//	Set alpha sort weight
	void SetAlphaSortWeight(int iWeight) { m_iAlphaWeight = iWeight; }
	//	Get alpha sort weight
	int GetAlphaSortWeight() const { return m_iAlphaWeight; }
	
	//	Set vertex shader for all skin meshes and rigid meshes
	void SetVertexShader(A3DVertexShader* pvsSkinMesh, A3DVertexShader* pvsRigidMesh);
	//	Restore vertex shader for all skin meshes and rigid meshes
	void RestoreVertexShader();

	//	Get A3D engine
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	//	Get A3D device interface
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }
	//	Get skin mesh manager
	A3DSkinMeshMan* GetSkinMeshMan() { return m_pMeshMan; }

	//	Below function are used by model editor or exporter !
	//	Get texture array
	APtrArray<A3DTexture*>& GetTextureArray() { return m_aTextures; }
	//	Get normal map array
	APtrArray<A3DTexture*>& GetNormalMapArray() { return m_aNormalMaps; }
	//	Get material array
	APtrArray<A3DMaterial*>& GetMaterialArray() { return m_aMaterials; }
	//	Get bone name array
	APtrArray<AString*>& GetBoneNameArray() { return m_aBoneNames; }
	//	Get mesh array
	MeshRef& GetSkinMeshArray() { return m_aSkinMeshes; }
	MeshRef& GetRigidMeshArray() { return m_aRigidMeshes; }
	MeshRef& GetMorphSkinMeshArray() { return m_aMorphSkMeshes; }
	MeshRef& GetMorphRigidMeshArray() { return m_aMorphRdMeshes; }
	MeshRef& GetSuppleMeshArray() { return m_aSuppleMeshes; }
	MeshRef& GetMuscleMeshArray() { return m_aMuscleMeshes; }
	MeshRef& GetClothMeshArray() { return m_aClothMeshes; }

	inline bool IsBloomMesh() { return m_bBloomMesh;}
	inline void SetBloomMesh(bool bBloomMesh) { m_bBloomMesh = bBloomMesh;}
	bool CheckTangentError();	// 游戏中是多线程加载Effect, 要避免使用这个函数

	void EnableBloom(bool bEnabled) { m_bBloomEnabled = bEnabled; }
	bool IsBloomEnabled() const { return m_bBloomEnabled; }
	void SetBloomColor(const A3DVECTOR4& vBloomCol){ m_vBloomCol = vBloomCol; }
	const A3DVECTOR4& GetBloomColor(){ return m_vBloomCol; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;	//	A3D engine object
	A3DDevice*		m_pA3DDevice;	//	A3D device object
	A3DSkinMeshMan*	m_pMeshMan;		//	Skin mesh manager

	AString		m_strFileName;		//	Skin file name
	float		m_fMinWeight;		//	Minimum weight limit
	int			m_iNumSkeBone;		//	Number of bone of skeleton to which the skin attached
	int			m_iDataSize;		//	Approximate data size
	bool		m_bSoftRender;		//	Force skin mesh to use software render
	bool		m_bEnableCloth;		//	Enable/Disable cloth meshes
	int			m_iAlphaWeight;		//	Alpha sort weight, more bigger more closer

	APtrArray<A3DTexture*>	m_aTextures;	//	Texture array
	APtrArray<A3DTexture*>	m_aNormalMaps;	//	Normal map array
	APtrArray<A3DMaterial*>	m_aMaterials;	//	Material array
	APtrArray<AString*>		m_aBoneNames;	//	Bone name array
	AArray<int, int>		m_aBoneIdxMap;	//	Bone index map

	MeshRef		m_aSkinMeshes;		//	Mesh array
	MeshRef		m_aRigidMeshes;		//	Rigid mesh array
	MeshRef		m_aMorphSkMeshes;	//	Morph skin mesh array
	MeshRef		m_aMorphRdMeshes;	//	Morph rigid mesh array
	MeshRef		m_aSuppleMeshes;	//	Supple mesh array
	MeshRef		m_aMuscleMeshes;	//	Muscle mesh array
	MeshRef		m_aClothMeshes;		//	Cloth mesh array

	DWORD		m_dwSkinID;			//	Skin's ID
	DWORD		m_dwLoadVer;		//	Version stored when Load() is called
	bool		m_bBloomMesh;		//
	bool		m_bBloomEnabled;
	A3DVECTOR4	m_vBloomCol;

protected:	//	Operations

	//	A3DSkin should be created and released through A3DSkinMan, so hide it's
	//	initial and release functions here.
	bool Init(A3DEngine* pA3DEngine);
public:
	void Release();
protected:
	//	Load skin data form file
	bool Load(AFile* pFile);
	bool Load(const char* szFile);

	//	Determine render method
	void DetermineRenderMethod();
	//	Calculate approximate data size
	void CalcDataSize();
	//	Generate cloth meshes
	bool GenerateClothes(MeshRef& aMeshRefs, bool* aFlags);
	//	Create a cloth mesh
	A3DClothMeshImp* CreateClothMesh(A3DMeshBase* pCoreMesh);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSKIN_H_

