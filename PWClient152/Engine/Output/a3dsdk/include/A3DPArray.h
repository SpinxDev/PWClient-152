/*
 * FILE: A3DPArray.h
 *
 * DESCRIPTION: Object Based Particle System for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPARRAY_H_
#define _A3DPARRAY_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DParticleSystem.h"

enum A3DPARRY_PARTICLE_FORMATION
{
	A3DPARRY_PARTICLE_FORMATION_ENTIRESURFACES = 0,
	A3DPARRY_PARTICLE_FORMATION_VISIBLEEDGES,
	A3DPARRY_PARTICLE_FORMATION_ALLVERTICES,
	A3DPARRY_PARTICLE_FORMATION_DISTINCTPOINTS,
	A3DPARRY_PARTICLE_FORMATION_FACECENTERS
};

class A3DModel;
class A3DMesh;
class A3DFrame;

class A3DPArray : public A3DParticleSystem
{
private:

public:

	enum
	{
		MAXNUM_TRIINBUF	= 32
	};

	struct MESHINFO
	{
		A3DMesh*	pMesh;			//	Mesh address
		A3DFrame*	pFrame;			//	Frame in which mesh layed
		int			idTexture;		//	Texture ID
	};

	struct FRAGBUFFER		//	Fragment buffer
	{
		A3DLVERTEX*	aVerts;			//	Vertices
		WORD		aIndices[MAXNUM_TRIINBUF * 3];	//	Indices, array size should >= iMaxNumTri * 3
		int			iMaxNumTri;		//	Maximum number of triangle buffer can contain
		int			iNumIdx;		//	Number of index
		int			iNumTriangle;	//	Number of triangle has been put into this buffer
		A3DVECTOR3	vNormal;		//	Approximate normal of fragment
		float		fArea;			//	Total area of triangles in buffer
	};

protected:

	A3DPARRY_PARTICLE_FORMATION m_FormationMethod;

	A3DModel	*m_pObjectModel;
	A3DVECTOR3	m_vObjCenter;		//	Object's center

	MESHINFO	m_aMeshes[64];		//	Meshes' address
	int			m_iNumMesh;			//	Number of mesh
	A3DTexture*	m_aTextures[64];	//	Temporary texture array	
	int			m_iNumTexture;		//	Number of texture in m_aTextures

	int			m_iNumGFXs;			//	Number of graphics effects has been generated;

	A3DLVERTEX*	m_aFragVerts;		//	Vertex buffer
	int			m_iNumFragVert;		//	Size of m_aFragVerts in vertex
	int			m_iCurFragVert;		//	Current vertex number in m_aFragVerts
	WORD*		m_aFragIndices;		//	Indices buffer
	int			m_iNumFragIdx;		//	Size of m_aFragIndices in index
	int			m_iCurFragIdx;		//	Current index number in m_aFragIndices

	float		m_fMinFragArea;		//	The minimum fragment area limit
	float		m_fMaxFragArea;		//	The maximum fragment area limit
	int			m_iCurTexID;		//	Current texture ID

	FRAGBUFFER	m_FragBuf;			//	Fragment buffer

	void	AddFrameInfo(A3DFrame* pBaseFrame);
	bool	AddMeshInfo(A3DFrame* pFrame, A3DMesh* pMesh, A3DTexture* pTexture);
	void	InitMeshInfo(A3DModel* pModel);
	void	SplitModel();
	bool	SplitMesh(A3DLVERTEX* aVerts, int iNumVert, WORD* aIndices, int iIndex);
	bool	SplitTriangle(A3DLVERTEX* aVerts, A3DVECTOR3 vNormal, float fAreaVal);
	bool	SplitQuadrangle(A3DLVERTEX* aVerts, A3DVECTOR3 vNormal, float fAreaVal);
	bool	EmitOneFragment(A3DLVERTEX* aVerts, int iNumVert, A3DVECTOR3 vNormal);

	void	InitFragBuffer(A3DLVERTEX* aVerts);		//	Initialize fragment buffer
	bool	PushTriangleIntoFragBuffer(WORD* aIndices, A3DVECTOR3 vNormal, float fArea);	//	Push triangle into fragment buffer
	bool	FlushFragBuffer();		//	Flush frag buffer

public:
	A3DPArray();
	virtual ~A3DPArray();

	bool CreateObjectFragment(A3DModel * pObjectModel);
	bool Release();

	bool EmitParticles();
	bool MakeDead(LPVOID pParticle);

	inline A3DPARRY_PARTICLE_FORMATION GetFormationMethod() { return m_FormationMethod; }
	inline void SetFormationMethod(A3DPARRY_PARTICLE_FORMATION formationMethod) { m_FormationMethod = formationMethod; }

	void	SetMinFragArea(float fArea)	{	m_fMinFragArea = fArea;		}
	float	GetMinFragArea()			{	return m_fMinFragArea;		}
	void	SetMaxFragArea(float fArea)	{	m_fMaxFragArea = fArea;		}
	float	GetMaxFragArea()			{	return m_fMaxFragArea;		}
	void	SetMaxLinkedFrag(int iNum)	{	if (iNum >= 0 && iNum <= MAXNUM_TRIINBUF)	m_FragBuf.iMaxNumTri = iNum;	}
	int		GetMaxLinkedFrag()			{	return m_FragBuf.iMaxNumTri;	}
};

typedef A3DPArray * PA3DPArray;

#endif

