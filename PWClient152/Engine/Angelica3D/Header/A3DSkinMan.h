/*
 * FILE: A3DSkinMan.h
 *
 * DESCRIPTION: A3D skin manager class
 *
 * CREATED BY: duyuxin, 2003/9/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINMAN_H_
#define _A3DSKINMAN_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "hashtab.h"
#include "AArray.h"
#include "AString.h"

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
class A3DSkin;
class A3DVertexShader;
class A3DLight;
class A3DSkinMeshMan;
class A3DSkeleton;
class AFile;
class A3DTexture;
class A3DObject;
class AIniFile;
class A3DHLSLMan;
class A3DShaderMan;
class A3DCCDBinder;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinMan
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinMan
{
public:		//	Types

	//	Vertex shader type
	enum VertexShaderType
	{
		VST_SPEC = 0,			//	Specular
		VST_NONSPEC,			//	Non-specular
		VST_SPEC_DLT,			//	Sepcular + dynamic light
		VST_NONSPEC_DLT,		//	Non-Specular + dynamic light
		//VST_NORMALMAP,			//	normal map supported, without light calculation in vs
		VST_NUM,
	};

	//	Vertex shader index
	enum VertexShaderIdx
	{
		VS_SKINMESH4 = 0,		//	Skin mesh, 4 blend matrices
		VS_SKINMESH4TAN,		//	Skin mesh, 4 blend matrices with tangent
		VS_RIGIDMESH,			//	Rigid mesh
		VS_RIGIDMESHTAN,		//	Rigid mesh, with tangent
		VS_MORPHSKINMESH0,		//	Morph skin mesh, 0 morph channel
		VS_MORPHSKINMESH1,		//	Morph skin mesh, 1 morph channel
		VS_MORPHSKINMESH2,		//	Morph skin mesh, 2 morph channel
		VS_MORPHSKINMESH3,		//	Morph skin mesh, 3 morph channel
		VS_MORPHSKINMESH4,		//	Morph skin mesh, 4 morph channel
		VS_MORPHRIGIDMESH0,		//	Morph rigid mesh, 0 morph channel
		VS_MORPHRIGIDMESH1,		//	Morph rigid mesh, 1 morph channel
		VS_MORPHRIGIDMESH2,		//	Morph rigid mesh, 2 morph channel
		VS_MORPHRIGIDMESH3,		//	Morph rigid mesh, 3 morph channel
		VS_MORPHRIGIDMESH4,		//	Morph rigid mesh, 4 morph channel
		VS_NUM,					//	Vertex shader number
	};

	//	Core skeleton node
	struct SKELETONNODE
	{
		APtrArray<A3DSkeleton*>	aSkeletons;
	};

	//	Shader replace struct
	struct SHADERREPLACE 
	{
		AString		strFile;			// file of the replace .sdr
		DWORD		dwFlags;			// flags of this shader
	};

	enum
	{
		SHADERREPLACE_NORMALMAPNEEDED = 0x1,	// whether or not mesh using this shader will take use of normal map
		SHADERREPLACE_REFLECTPREFIX = 0x2,		// filename must begin with "r_"
		SHADERREPLACE_USERDEFINE = 0x4,			// user defined procedure to replace the shader
	};

	friend class A3DSkinRenderBase;
	friend class A3DSkinRender;
	friend class A3DSkinRenderWithEffect;
	friend class A3DSkinModel;

	typedef abase::hashtab<SKELETONNODE*, int, abase::_hash_function> SkeletonTable;
	typedef abase::hashtab<SHADERREPLACE, int, abase::_hash_function> ShaderReplaceTable;
	typedef void (*UserReplaceShaderProc)(A3DSkin * pSkin);

public:		//	Constructors and Destructors

	A3DSkinMan();
	virtual ~A3DSkinMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();
	//	Reset object
	bool Reset();

	//	Set current skin render
	void SetCurSkinRender(A3DSkinRenderBase* psr) { m_psr = psr; }
	//	Get current skin render
	A3DSkinRenderBase* GetCurSkinRender() { return m_psr; }
	//	Get default skin render
	A3DSkinRenderBase* GetDefSkinRender() { return m_pdsr; }

	//	Create skin from file
	A3DSkin* LoadSkinFile(const char* szFile);
	A3DSkin* LoadSkinFile(AFile* pFile);
	//	Release skin object
	void ReleaseSkin(A3DSkin** ppSkin);

	//	Create skeleton from file
	A3DSkeleton* LoadSkeletonFile(const char* szFile);
	A3DSkeleton* LoadSkeletonFile(AFile* pFile);
	//	Release skeleton object
	void ReleaseSkeleton(A3DSkeleton** ppSkeleton);

	//	Load skin model vertex shaders
	bool LoadSkinModelVertexShaders(const char* szDescFile);
	//	Set / Get vertex shader
	A3DObject* SetVertexShader(int iVSType, int iIndex, A3DVertexShader* pNewShader);
	A3DObject* GetVertexShader(int iVSType, int iIndex);

	//	Set / Get current replace shader file name
	bool SetReplaceShaderFile(const char * szShaderFile, DWORD dwFlags); 
	const char* GetReplaceShaderFile(DWORD * pdwFlags);

	//	Set / Get directional light
	void SetDirLight(A3DLight* pLight) { m_pDirLight = pLight; }
	A3DLight* GetDirLight() { return m_pDirLight; }
	//	Set / Get dynamic point light
	void SetDPointLight(A3DLight* pLight) { m_pDPointLight = pLight; }
	A3DLight* GetDPointLight() { return m_pDPointLight; }
	//	Set / Get world center
	void SetWorldCenter(const A3DVECTOR3& vPos) { m_vWorldCenter = vPos; }
	const A3DVECTOR3& GetWorldCenter() { return m_vWorldCenter; }

	//	Get A3D engine object
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	//	Get A3D device object
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }
	//	Get skin mesh manager
	A3DSkinMeshMan* GetSkinMeshMan() { return m_pMeshMan; }
	//	Get specular map
	A3DTexture * GetSpecularMap() { return m_pSpecularMap; }

	//	Apply light information to skin model
	void ApplyModelLightTo(A3DSkinModel* pSkinModel);
	//	Get mesh vs by vs type and index
	inline A3DObject* GetMeshVS(VertexShaderType eType, VertexShaderIdx eIdx);

protected:	//	Attributes
	
	A3DEngine*			m_pA3DEngine;			//	A3D engine object
	A3DDevice*			m_pA3DDevice;			//	A3D device object
	A3DSkinRenderBase*	m_psr;					//	Current A3D Skin render
	A3DSkinRenderBase*	m_pdsr;					//	Default A3D skin render
	A3DSkinMeshMan*		m_pMeshMan;				//	Mesh manager

	A3DObject*			m_aVS[VST_NUM][VS_NUM];	//	Vertex shaders
	A3DLight*			m_pDirLight;			//	Directional light
	A3DLight*			m_pDPointLight;			//	Dynamic point light
	ShaderReplaceTable	m_ShaderReplaceTab;		//	Shader file name with which we replace current texture
	SkeletonTable		m_SkeletonTab;			//	Skeleton table
	CRITICAL_SECTION	m_csSkeleton;			//	Skeleton table lock
	A3DVECTOR3			m_vWorldCenter;			//	World center
	A3DTexture *		m_pSpecularMap;			//	Specular look up map

protected:	//	Operations

	//	Create new skin from file data
	A3DSkin* CreateNewSkin(void* pData, bool bFileName);
	//	Create a new skeleton from file data
	A3DSkeleton* CreateNewSkeleton(void* pData, bool bFileName);
	//	Search a skeleton by it's file name
	SKELETONNODE* SearchSkeleton(const char* szFile, DWORD dwSkeletonID);
	//	Allocate a skeleton node
	SKELETONNODE* AllocSkeletonNode();
	//	Release skeleton node
	void FreeSkeletonNode(SKELETONNODE* pNode);
	//	Release all resources
	void ReleaseAllResources();

	A3DObject* LoadShaderFromProfile(AIniFile& IniFile, A3DHLSLMan* pHLSLMan, A3DShaderMan* pShaderMan, A3DCCDBinder* pBinder, 
		bool bBinary, const char* szSect, const char* szKey, DWORD dwFVF);

	//	Get skin mesh vertex shader
	inline A3DObject* GetSkinMeshVS(bool bSpecular, bool bDynLight, bool bTangent);
	//	Get rigid mesh vertex shader
	inline A3DObject* GetRigidMeshVS(bool bSpecular, bool bDynLight);

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////

//	Get skin mesh vertex shader
A3DObject* A3DSkinMan::GetSkinMeshVS(bool bSpecular, bool bDynLight, bool bTangent)
{
	//	Select proper vertex shader
	int iVSType = bSpecular ? 0 : 1;
	if (bDynLight)
		iVSType += 2;

	return bTangent ? m_aVS[iVSType][VS_SKINMESH4TAN] : m_aVS[iVSType][VS_SKINMESH4];
}

//	Get rigid mesh vertex shader
A3DObject* A3DSkinMan::GetRigidMeshVS(bool bSpecular, bool bDynLight)
{
	//	Select proper vertex shader
	int iVSType = bSpecular ? 0 : 1;
	if (bDynLight)
		iVSType += 2;

	return m_aVS[iVSType][VS_RIGIDMESH];
}

A3DObject* A3DSkinMan::GetMeshVS(VertexShaderType eType, VertexShaderIdx eIdx)
{
	return m_aVS[eType][eIdx];
}
#endif	//	_A3DSKINMAN_H_

