/*
 * FILE: A3DSuppleMesh.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/3/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSUPPLEMESH_H_
#define _A3DSUPPLEMESH_H_

#include "A3DMeshBase.h"
#include "A3DVertex.h"
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

class A3DStream;
class A3DTexture;
class A3DMaterial;
class A3DSkinModel;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSuppleMesh
//	
///////////////////////////////////////////////////////////////////////////

class A3DSuppleMesh : public A3DMeshBase
{
public:		//	Types

	//	Vertex linked by springs 
	struct SVERTEX
	{
		A3DVECTOR3	vPos;			//	Position
		float		aWeights[3];	//	Bone weight
		DWORD		dwMatIndices;	//	Bone matrix indices
		float		fBoneFactor;	//	Bone effect factor
	};

	//	Vertex used to render mesh
	struct RVERTEX
	{
		int			iSVertex;		//	Index of SVERTEX
		A3DVECTOR3	vNormal;		//	Vertex normal
		float		tu, tv;			//	Texture coordinates
	};

	//	Spring information
	struct SPRING
	{
		int			v1;				//	Spring v1's index
		int			v2;				//	Spring v2's index
		float		fCoefficient;	//	Spring coefficient
		float		fIdleLen;		//	Idle length
	};
	
public:		//	Constructor and Destructor

	A3DSuppleMesh();
	virtual ~A3DSuppleMesh();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile, int nSkinVersion,int& nReturnValue);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Create original data buffers
	bool CreateOriginalBuffers(int iNumSVert, int iNumRVert, int iNumIdx, int iNumSpring);

	//	Get spring vertex number
	int GetSpringVertexNum() { return m_iNumSVert; }
	//	Get spring vertex buffer
	SVERTEX* GetOriginSpringVertexBuf() { return m_aSVerts; }
	//	Get render vertex buffer
	RVERTEX* GetOriginRenderVertexBuf() { return m_aRVerts; }
	//	Get index buffer which contains original index data
	WORD* GetOriginIndexBuf() { return m_aIndices; }
	//	Get spring buffer
	SPRING* GetSpringBuf() { return m_aSprings; }
	//	Get spring number
	int GetSpringNum() { return m_iNumSpring; }
	//	Set / Get vertex gravity factor
	void SetGravityFactor(float fFactor) { m_fGravityFactor = fFactor; }
	float GetGravityFactor() { return m_fGravityFactor; }
	//	Set / Get air resistance factor
	void SetAirResistance(float fFactor) { m_fAirResist = fFactor; }
	float GetAirResistance() { return m_fAirResist; }
	//	Set / Get vertex weight
	void SetVertWeight(float fWeight) { m_fVertWeight = fWeight; }
	float GetVertWeight() { return m_fVertWeight; }

protected:	//	Attributes

	SVERTEX*	m_aSVerts;			//	Original spring vertex data buffer
	RVERTEX*	m_aRVerts;			//	Original render vertex data buffer
	WORD*		m_aIndices;			//	Original indices data buffer
	SPRING*		m_aSprings;			//	Springs
	int			m_iNumSVert;		//	Number of spring vertex
	int			m_iNumSpring;		//	Number of springs
	float		m_fGravityFactor;	//	Vertex gravity factor
	float		m_fAirResist;		//	Air resistance factor
	float		m_fVertWeight;		//	Vertex weight

protected:	//	Operations

	//	Calculate initial mesh aabb
	void CalcInitMeshAABB();
	//	Release original data buffers
	void ReleaseOriginalBuffers();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSuppleMeshImp
//	
///////////////////////////////////////////////////////////////////////////

class A3DSuppleMeshImp : public A3DMeshBase
{
public:		//	Types

	//	Physical properties
	struct PHYSICALPROP
	{
		A3DVECTOR3	vTransPos;		//	Transformed positino
		A3DVECTOR3	vTransNormal;	//	Transformed normal	
		A3DVECTOR3	vPos;			//	Position
		A3DVECTOR3	vOldPos;		//	Old position
		A3DVECTOR3	vForce;			//	Force
	};

	//	External force
	struct FORCE
	{
		A3DVECTOR3	vForceDir;		//	Normalized force
		float		fForce;			//	Force magnitude
		float		fDisturb;		//	Force disturb
	};

public:		//	Constructor and Destructor

	A3DSuppleMeshImp(A3DSuppleMesh* pCoreMesh, A3DSkinModel* pSkinModel);
	virtual ~A3DSuppleMeshImp();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Update mesh
	bool Update(int iDeltaTime, A3DSkinModel* pSkinModel);
	//	Add external force
	void AddForce(const A3DVECTOR3& vForce, float fDisturb);

	//	Erase the force generated by position change of current frame
	void ErasePosEffect() { m_bErasePosEffect = true; }

protected:	//	Attributes

	A3DSkinModel*	m_pSkinModel;		//	Skin model this mesh belongs to
	A3DStream*		m_pA3DStream;		//	Stream used in hardware rendering
	A3DSuppleMesh*	m_pCoreMesh;		//	Core mesh object
	PHYSICALPROP*	m_aPhysicalProps;	//	Physical properties
	int				m_iInterateTime;	//	Spring interate time
	bool			m_bErasePosEffect;	//	Erase position effect of current frame

	AArray<FORCE, FORCE&>	m_aForces;		//	External force of every frame
	AArray<BYTE, BYTE>		m_aFixFlags;	//	Used when m_bErasePosEffect = true

protected:	//	Operations

	//	Create A3D stream
	bool CreateStream();
	//	Create physical properties
	bool CreatePhysicalProperties();
	//	Calculate forces which effect vertices
	void CalculateForces(int iDeltaTime, A3DSkinModel* pSkinModel);
	//	Calculate vertices positions
	void CalculateVertices(int iDeltaTime, A3DSkinModel* pSkinModel);

	//	Set vertex fixed flag
	void SetFixedFlag(int n) { m_aFixFlags[n >> 3] |= (1 << (n & 7)); }
	//	Get vertex fixed flag
	BYTE GetFixedFlag(int n) { return m_aFixFlags[n >> 3] & (1 << (n & 7)); }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSUPPLEMESH_H_
