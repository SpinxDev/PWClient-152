/*
 * FILE: A3DSkinMesh.h
 *
 * DESCRIPTION: A3D skin mesh class
 *
 * CREATED BY: duyuxin, 2003/9/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINMESH_H_
#define _A3DSKINMESH_H_

#include "A3DMeshBase.h"
#include "A3DVertex.h"
#include "AArray.h"
#include "A3DConfig.h"
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

class A3DStream;
class A3DTexture;
class A3DMaterial;

struct s_PROCSUBMESH;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

#define SKIN_FVF_VERTEX_TANGENT (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE4(1) )
struct SKIN_VERTEX_TANGENT
{
	FLOAT vPos[3];			//	Position
	FLOAT aWeights[3];
	DWORD dwMatIndices;		//	Matrix indices
	FLOAT vNormal[3];
	FLOAT tu, tv;			//	Texture coordinates
	A3DVECTOR4 tangents;	//	tangent axis
};

#define SKIN_FVF_VERTEX (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
struct SKIN_VERTEX
{
	FLOAT vPos[3];			//	Position
	FLOAT aWeights[3];
	DWORD dwMatIndices;		//	Matrix indices
	FLOAT vNormal[3];
	FLOAT tu, tv;			//	Texture coordinates
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinMesh
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinMesh : public A3DMeshBase
{
public:		//	Types

	//	Struct used to save and load skin mesh
	struct SKINMESHDATA
	{
		int		iTexture;		//	Texture index
		int		iMaterial;		//	Material index
		int		iNumVert;		//	Number of vertex
		int		iNumIdx;		//	Number of index
	};

	//	Sub mesh set
	struct SUBMESH
	{
		int*		aMatIndices;	//	Matrix indices
		int			iMatCount;		//	Number of valid matrix in aMatIndices
		A3DStream*	pA3DStream;		//	A3D stream
		A3DStream*	pA3DStreamTan;
	};

public:		//	Constructors and Destructors

	A3DSkinMesh();
	virtual ~A3DSkinMesh();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile, int nSkinVersion, int& nReturnValue);

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);
	//	Software rendering
	virtual bool RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Create original data buffers
	bool CreateOriginalBuffers(int iNumVert, int iNumIdx, int nSkinVersion);

	//	Get vertex buffer which contains original vertex data
	SKIN_VERTEX* GetOriginVertexBuf() { return m_aVerts; }
	SKIN_VERTEX_TANGENT* GetOriginTanVertexBuf() { return m_aTangentVerts; }
	//	Get index buffer which contains original index data
	WORD* GetOriginIndexBuf() { return m_aIndices; }
	//	Get sub mesh number
	int GetSubMeshNum() { return m_aSubMeshes.GetSize(); }
	//	Get splited flag
	bool GetSplitedFlag() { return m_bSplited; }
	//	Use specified blend matrices to calculate vertices' positions and normals through software rendering method
	//	One of aVerts and aNormals can be NULL
	bool GetBlendedVertices(A3DSkin* pSkin, const A3DMATRIX4* aBlendMats, A3DVECTOR3* aVerts, A3DVECTOR3* aNormals);
	bool GetBlendedVertices(A3DSkin* pSkin, const A3DMATRIX4* aBlendMats, A3DVECTOR3* aVerts, A3DVECTOR3* aNormals, A3DVECTOR3* aTangents);
	inline bool IsEnableExtMaterial();
protected:	//	Attributes

	SKIN_VERTEX*	m_aVerts;			//	Original vertex data buffer
	WORD*			m_aIndices;			//	Original indices data buffer
	A3DStream*		m_pVSStream;		//	if m_bSplited is force, this is the entire mesh stream
	A3DStream*		m_pVSStreamTan;
	A3DStream*		m_pSRStream;		//	Stream used to software or IVM rendering
	int				m_iNumVSBlendMat;	//	Number of blend matrix can be used in vertex shader
	bool			m_bSplited;			//	Split flag
	int				m_iRenderMethod;	//	Render method

	APtrArray<SUBMESH*>	m_aSubMeshes;	//	Sub meshes
	SKIN_VERTEX_TANGENT* m_aTangentVerts;

protected:	//	Operations

	//	Calculate number of blend matrix can be used in vertex shader
	virtual void CalcVSBlendMatNum();
	//	Load mesh data form file
	virtual bool LoadData(AFile* pFile, int nSkinVersion);
	//	Create sub meshes
	virtual bool CreateSubMeshes(A3DSkin* pSkin);

	//	Calculate initial mesh aabb
	void CalcInitMeshAABB();
	//	Release original data buffers
	void ReleaseOriginalBuffers();
	//	Create the only one stream for this skin mesh
	bool CreateVSStream();
	//	Create sub meshes for indexed blend matrix rendering
	bool CreateIndexedBlendMatSubMeshes();
	//	Create sub meshes for vertex shader rendering
	bool CreateVertexShaderSubMeshes(A3DSkin* pSkin);
	//	Create sub meshes for software rendering
	bool CreateSoftwareSubMeshes();
	//	Release all sub meshes
	void ReleaseSubMeshes();
	//	Add face to processed sub-mesh
	bool AddFaceToProcessedSubMesh(APtrArray<s_PROCSUBMESH*>& aSubMeshes, int iFace, int* aFaceMats, int iNumFaceMat);
	//	Add vertex to processed sub-mesh
	bool AddVertexToProcessedSubMesh(s_PROCSUBMESH* pProcSubMesh, int iIndex);
	//	Convert processed sub mesh to real sub mesh
	bool ConvertProcessedSubMesh(s_PROCSUBMESH* pProcSubMesh);

	//	Apply vertex shader blend matrices
	bool ApplyVSBlendMatrices(A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DViewport* pViewport, int iStartConst);
	//	Apply indexed vertex blend matrices
	bool ApplyIndexedBlendMatrices(A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DViewport* pViewport);

	void GenerateTangentBinormal(A3DVECTOR4* pTangents);

	bool CheckTangentsUVMirrorError(A3DVECTOR4* pTangents);
	void SetTangentW(A3DVECTOR4* pTangents);

#ifdef SKIN_BUMP_ENABLE
	bool BuildTangents();
#endif 
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////

inline bool A3DSkinMesh::IsEnableExtMaterial()
{
	return (g_pA3DConfig->GetFlagHLSLEffectEnable() && (m_aTangentVerts != NULL));
}
#endif	//	_A3DSKINMESH_H_

