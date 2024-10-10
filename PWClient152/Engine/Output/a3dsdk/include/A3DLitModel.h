/*
 * FILE: A3DLitModel.h
 *
 * DESCRIPTION: Lit static model in Angelica Engine, usually used for building
 *
 * CREATED BY: Hedi, 2004/11/4
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLITMODEL_H_
#define _A3DLITMODEL_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include <AArray.h>
#include "A3DMaterial.h"

class A3DTexture;
class A3DDevice;
class A3DEngine;
class A3DViewport;
class A3DStream;
class A3DPixelShader;
class A3DOcclusionProxy;
class A3DLitMeshSharedData;
class A3DLitModelSharedData;

//////////////////////////////////////////////////////////////////////////
// class A3DLitMesh
//	
//		A mesh that was pre-lit
//////////////////////////////////////////////////////////////////////////

struct A3DLMVERTEX
{
	A3DLMVERTEX() {}
	
	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	A3DCOLOR			diffuse;

	float				u;
	float				v;
};

//Litmodel with lightmap
struct A3DLMLMVERTEX
{
	A3DLMLMVERTEX() {}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	A3DCOLOR			diffuse;

	float				u;
	float				v;

	float				lm_u;
	float				lm_v;
};

struct A3DLMVERTEX_WITHOUTNORMAL
{
	A3DVECTOR3			pos;
	//A3DVECTOR3			normal;
	A3DCOLOR			diffuse;

	float				u;
	float				v;
};

struct A3DLMVERTEX_POS_NORMAL_DIFFUSE_UV
{
	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	A3DCOLOR			diffuse;

	float				u;
	float				v;
};

#define A3DLMVERTEX_WITHOUTNORMAL_FVF D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1

struct A3DLMVERTEX_LIGHTMAP_NOVERT
{
    A3DVECTOR3			pos;
	//A3DVECTOR3			normal;
    float				u;
    float				v;

    float				lm_u;
    float				lm_v;
};

struct A3DLMVERTEX_POS_NORMAL_UV_UV
{
	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	float				u;
	float				v;

	float				lm_u;
	float				lm_v;
};

#define A3DLMVERTEX_LIGHTMAP_NOVERT_FVF D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_TEX2

enum A3DLITMESH_VERTEX_FORMAT
{
	A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV,
	A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2,
};


#define A3DLITMESH_VERSION		0x10000006
#define A3DLITMESH_SUPPORT_LIGHTMAP_VERSION 0x10000101
#define A3DLITMESH_SUPPORT_LIGHTMAP_VERSION_01 (A3DLITMESH_SUPPORT_LIGHTMAP_VERSION + 1)

#define LIGHT_MAP_VERSION "LITMAPX2_"
#define ERROR_LIGHT_MAP "shaders\\textures\\ErrLM.dds"

class A3DLitMesh : public A3DObject
{
	friend class A3DLitModel;
	friend class A3DLitModelRender;

private:
	A3DDevice *					m_pA3DDevice;		// A3DDevice object pointer
	A3DLitModel *				m_pParentModel;		// Model that owns this mesh

	char						m_szTextureMap[256];// texture name of this mesh
	A3DTexture *				m_pA3DTexture;		// texture of this mesh
	char						m_szReflectTextureMap[256];// reflect texture name of this mesh
	A3DTexture *				m_pA3DReflectTexture;// texture of fake reflection
	A3DMaterial 				m_Material;			// material of this mesh

	float						m_vDisToCam;		// distance from this mesh's center to camera

	A3DLMVERTEX *				m_pVerts;			// vertex of this mesh
	WORD *						m_pIndices;			// indices of this mesh

	A3DVECTOR3 *				m_pNormals;			// normals of this mesh, for lighting calculation usage only

	A3DCOLOR *					m_pDayColors;		// day colors of this mesh
	A3DCOLOR *					m_pNightColors;		// night colors of this mesh	

	bool						m_bHasExtraColors;
	A3DCOLOR *					m_pDayColorsExtra;	// extra day colors of this mesh
	A3DCOLOR *					m_pNightColorsExtra;// extra night colors of this mesh	 

	float						m_fDNFactor;		// current day night factor in m_pVerts

	A3DAABB						m_aabb;				// aabb of this mesh

	int							m_nVertCount;		// number of vertex
	int							m_nFaceCount;		// number of faces

	bool						m_bVisible;			// visibility flag of this mesh
	int							m_nAlphaValue;		// alpha value of this mesh	0 ~ 255

	bool						m_bSupportLM;		//Is support light map
	bool						m_bUseLM;
	A3DLIGHTMAPCOORD*			m_pLMCoords;		//Light map coords;
    A3DStream*                  m_pStream;          //Stream
    //DWORD                       m_dwStreamFVF;
	A3DLITMESH_VERTEX_FORMAT	m_vertFormat;

    AString                     m_strDetailMap;
    A3DTexture*                 m_pDetailTexture;
    float                       m_fDetailTile;
    float                       m_fDetailHardness;
	
public:
	inline int GetNumVerts()			{ return m_nVertCount; }
	inline int GetNumFaces()			{ return m_nFaceCount; }
	inline A3DLMVERTEX * GetVerts()		{ return m_pVerts; }
	inline WORD * GetIndices()			{ return m_pIndices; }
	inline A3DVECTOR3 * GetNormals()	{ return m_pNormals; }
	inline A3DCOLOR * GetDayColors()	{ return m_pDayColors; }
	inline A3DCOLOR * GetNightColors()	{ return m_pNightColors; }

	bool HasExtraColors() const { return m_bHasExtraColors; }
	void SetExtraColors(bool bApplay);
	inline A3DCOLOR * GetDayColorsExtra()	{ return m_pDayColorsExtra; }
	inline A3DCOLOR * GetNightColorsExtra()	{ return m_pNightColorsExtra; }


	inline const A3DAABB& GetAABB()		{ return m_aabb; }
	inline void SetAABB(const A3DAABB& aabb) { m_aabb = aabb; }

	inline const char * GetTextureMap()	{ return m_szTextureMap; }
	inline A3DTexture * GetTexturePtr()	{ return m_pA3DTexture; }
	inline A3DTexture * GetReflectTexturePtr() { return m_pA3DReflectTexture; }
	inline A3DMaterial& GetMaterial() { return m_Material; }
	inline A3DLitModel* GetParentModel() { return m_pParentModel;}

	inline bool IsSupportLightMap() { return m_bSupportLM;}
	void		SetUseLightMap(bool bUseLM);
	inline bool		IsUseLightMap() { return m_bUseLM;}
	inline A3DLIGHTMAPCOORD* GetLightMapCoords() { return m_pLMCoords;}
	A3DTexture*  GetLightMap();
	A3DTexture*	 GetNightLightMap();
	const char*  GetLightMapName();
	const char*	 GetNightLightMapName();
    inline A3DStream* GetStream() const { return m_pStream; }
	inline A3DLITMESH_VERTEX_FORMAT GetVertexFormat() const { return m_vertFormat; }

    void UpdateColorByDayNight();
	bool SetTexturePtr(A3DTexture* pNewTexture);

	inline bool IsVisible()				{ return m_bVisible; }
	inline void SetVisible(bool bFlag)	{ m_bVisible = bFlag; }

	inline int GetAlphaValue()			{ return m_nAlphaValue; }
	inline void SetAlphaValue(int value){ m_nAlphaValue = value; }

	inline float GetDisToCam()			{ return m_vDisToCam; }

    A3DTexture* GetDetailTexture() const { return m_pDetailTexture; }
    const char* GetDetailTextureFileName() const { return m_strDetailMap; }
    void SetDetailTexture(const char* szDetailMap);

    float GetDetailTile() const { return m_fDetailTile; }
    void SetDetailTile(float fTile) { m_fDetailTile = fTile; }

    float GetDetailHardness() const { return m_fDetailHardness; }
    void SetDetailHardness(float fHard) { m_fDetailHardness = fHard; }

protected:
	bool CreateVertexBuffers();
	bool ReleaseVertexBuffers();

	bool CreateLMCoords();
	bool ReleaseLMCoords();

	bool LoadTexture();
	bool ReleaseTexture();

	bool BuildTangents();

	bool RegisterForRender(A3DViewport * pViewport);
	//bool RegisterForRenderShadowMap();

    bool CreateStream();
    bool UpdateStream(bool bDynamic);
    void ReleaseStream();

public:
	A3DLitMesh(A3DLitModel * pLitModel);
	~A3DLitMesh();

	// Create/Release a lit mesh
	bool Create(A3DDevice * pA3DDevice, int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLM = false);
	bool Release();

	bool MergeMesh(A3DLitMesh * pMeshToBeMerged);

	// Ray trace method
	bool RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdFace=NULL);

	// Save and load
	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	// dummy load for skipping the litmodel part in the file
	bool DummyLoad(AFile * pFileToLoad);

	bool UpdateColors();

	// Change texture path
	bool ChangeTextureMap(const char * szTextureMap);
	
	//Get Parent Model
	const A3DMATRIX4& GetAbsMatrix();

    A3DLitMeshSharedData* ExportSharedData() const;
};

//////////////////////////////////////////////////////////////////////////
// class A3DLitModel
//		
//		A model that was pre-lit
//////////////////////////////////////////////////////////////////////////

#define A3DLITMODEL_VERSION		0x10000002
#define A3DLITMODEL_LIGHTMAP_VERSION		0x10000101
#define A3DLITMODEL_SHARED_VERSION		0x10000200

// version since 2013.02.05, merge vertex lit and light map and shared data
#define A3DLITMODEL_VERSION_EX	0x10000300

class A3DLitModel : public A3DObject
{
	friend class A3DLitMesh;

public:		//	Types

private:
	A3DDevice *					m_pA3DDevice;		// A3DDevice object pointer
    A3DEngine *                 m_pA3DEngine;		// A3DEngine object pointer
	A3DAABB						m_ModelAABB;		// auto build model aabb which will be updated while setposition and setdirection

	bool						m_bNoTransform;		// no transform has been applied to this model, so m_matAbsoluteTM is Identity matrix

	A3DVECTOR3					m_vecScale;			// scale factor
	A3DVECTOR3					m_vecPos;			// location of the model
	A3DVECTOR3					m_vecDir;			// direction of the model
	A3DVECTOR3					m_vecUp;			// up direction of the model

	A3DMATRIX4					m_matNormalizedTM;	// the absolute tm when NormalizeTransforms, 
	A3DMATRIX4					m_matAbsoluteTM;	// a transform matrix from model space to world space

	int							m_nVertCount;		// vertex count of all meshes in this model
	int							m_nFaceCount;		// face count of all meshes in this model
	
	APtrArray<A3DLitMesh *>		m_Meshes;			// all meshes in this model
	bool						m_bVisible;			// visibility flag of this model

	char						m_szLightMap[MAX_PATH];			//light map name;
	A3DTexture*					m_pLMTexture;					//Light map texture;
	char						m_szNightLightMap[MAX_PATH];	//night light map name;
	A3DTexture*					m_pNightLMTexture;				//night light map texture;
    A3DOcclusionProxy*          m_pOccProxy;        //Proxy for occlusion culling

public:
	inline const A3DVECTOR3& GetPos()			{ return m_vecPos; }
	inline const A3DVECTOR3& GetDir()			{ return m_vecDir; }
	inline const A3DVECTOR3& GetUp()			{ return m_vecUp; }
	inline const A3DVECTOR3& GetScale()			{ return m_vecScale; }
	inline const A3DMATRIX4& GetAbsoluteTM()	{ return m_matAbsoluteTM; }

	inline bool GetVisibility()					{ return m_bVisible; }

	inline const A3DAABB& GetModelAABB()		{ return m_ModelAABB; }
	
	inline int GetNumMeshes()					{ return m_Meshes.GetSize(); }
	inline A3DLitMesh * GetMesh(int index)		{ return m_Meshes[index]; }

	//light map
	inline A3DTexture*  GetLightMap() { return m_pLMTexture;}
	inline A3DTexture*	GetNightLightMap() { return m_pNightLMTexture;}
	inline const char*  GetLightMapName() { return m_szLightMap;}
	inline const char*	GetNightLightMapName() { return m_szNightLightMap;}

	bool SetLightMap(const char* szLightMap);
	bool SetNightLightMap(const char* szNightLightMap);

	bool ReleaseLightMap(bool bDay = true, bool bNight = true);
	bool LoadLightMap(bool bDay = true, bool bNight = true);
    bool IsNoTransform() const { return m_bNoTransform; }

protected:
	bool UpdateAbsoluteTM();
	bool AddMeshFromFrame(A3DFrame * pFrame);

public:
	// Constructor and Destructor
	A3DLitModel();
	~A3DLitModel();

	// Adjust scale factor
	bool SetScale(float vScaleX, float vScaleY, float vScaleZ);

	// apply the accumulated transforms to the verts and normals to make the m_matAbsoluteTM identitied;
	bool NormalizeTransforms();

	// Initialize and finalize
	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	// Render method
	bool Render(A3DViewport * pViewport, bool bUpdateShadowMap = false);

	// Visibility Adjustion
	void SetVisibility(bool bVisible);

	// Direct Set the position and orientation methods
	bool SetPos(const A3DVECTOR3& vecPos);
	bool SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp);

	// Relatively adjust orientation and position methods
	bool Move(const A3DVECTOR3& vecDeltaPos);
	bool RotateX(float vDeltaRad);
	bool RotateY(float vDeltaRad);
	bool RotateZ(float vDeltaRad);

	// Ray trace methods.
	// return true if hit, false if not.
	bool RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);
	bool RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdMesh=NULL, int * pIdFace=NULL);

	// Load and Save
	bool Save(const char * szFileName);
	bool Load(A3DDevice * pA3DDevice, const char * szFileName);

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

    bool SaveWithSharedFormat(AFile * pFileToSave, const char* szSharedFileName);
    bool SaveWithSharedFormat(const char* szFileName, const char* szSharedFileName);

	// dummy load for skipping the litmodel part in the file
	bool DummyLoad(AFile * pFileToLoad);

	bool LoadFromMox(A3DDevice * pA3DDevice, const char * szMoxFile);

	// mesh manipulation
	A3DLitMesh * AddNewMesh(int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLightMap = false);
	bool DeleteMesh(A3DLitMesh * pMesh);

	// Change litmesh texture
	bool ChangeLitmeshTexture(const char* szMeshName, const char* szTexFile);

    A3DLitModelSharedData* ExportSharedData() const;
};

#endif//_A3DLITMODEL_H_