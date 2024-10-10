/*
 * FILE: A3DKDTree.h
 *
 * DESCRIPTION: Routines for KD-Tree algorithm
 *
 * CREATED BY: duyuxin, 2003/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DKDTREE_H_
#define _A3DKDTREE_H_

#include "A3DVertex.h"
#include "A3DKDTreeFile.h"
#include "A3DSceneHull.h"

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

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DKDTree
//
///////////////////////////////////////////////////////////////////////////

class A3DKDTree : public A3DSceneHull
{
public:		//	Types

	//	Trace type
	enum
	{
		TRACE_RAY = 0,
		TRACE_AABB,
		TRACE_SPHERE,
	};

	struct BRUSH
	{
		A3DSPLANE**	aPlanes;		//	Plane pointer array
		int			iNumPlane;		//	Number of plane reference
	};

	struct SIDE
	{
		A3DSPLANE*	pPlane;			//	Plane
		A3DVECTOR3*	aVerts;			//	Vertex array of mesh to which this side belong
		int			iNumVert;		//	Number of vertex in aVerts, this in fact is mesh's vertex number
		BRUSH*		pBrush;			//	Brush
		A3DMESHPROP	MeshProps;		//	Mesh properties

		A3DVECTOR3	vCenter;		//	Side's center
		A3DVECTOR3	vExtents;		//	Bounding box's extents
		A3DVECTOR3	vMins;			//	Bounding box
		A3DVECTOR3	vMaxs;

		DWORD		dwTraceCnt;		//	Trace count
	};
	
	struct NODE
	{
		A3DSPLANE*	pPlane;			//	Plane index. NULL means leaf
		NODE*		pParent;		//	iParent node
		NODE*		aChildren[2];	//	Children nodes
		int*		aSideRefs;		//	Side reference array
		int			iNumSideRef;	//	Number of side reference
		A3DAABB		aabb;			//	Node's AABB
	};

	//	Internal structure for trace
	struct TRACEINFO
	{
		//	In
		int			iTrace;			//	Trace type
		A3DVECTOR3	vStart;			//	Start point of ray segment
		A3DVECTOR3	vEnd;			//	end point of ray segment
		A3DVECTOR3	vDelta;			//	Moving delta = vEnd - vStart

		//	Out
		float		fFraction;		//	Adjusted collision fraction
		float		fHitFrac;		//	Real hit fraction
		SIDE*		pSide;			//	Nearest side
		A3DVECTOR3	vNormal;		//	Side's normal
		A3DVECTOR3	vPoint;			//	Hit point, only for ray trace
	};

	//	Intersect information of AABB and leaves
	struct AABBLEAF
	{
		//	In
		A3DVECTOR3	vMins;			//	AABB information
		A3DVECTOR3	vMaxs;

		//	Out
		int*		aLeaves;		//	Point to leaf array
		int			iNumLeaf;		//	Number of leaf in aLeaves
		int			iMaxLeafNum;	//	Maximum leaf number can be hold in aLeaves
		int			iTopNode;		//	Top node of leaves
	};

	//	Internal structure for mark spliting
	struct MARKSPLIT
	{
		A3DAABB*	paabb;			//	Explosion box
		A3DVECTOR3*	pvNormal;		//	Normal of plane explosion is on
		A3DLVERTEX* aVerts;			//	Vertex buffer
		WORD*		aIndices;		//	Index buffer
		int			iNumVert;		//	Number of vertex in aVerts
		int			iNumIdx;		//	Number of index in aIndices
		float		fRadius;		//	Affect radius
		SIDE*		pSide;			//	Current side used to split mark
		bool		bJog;			//	true, push mark out a little
		A3DVECTOR3	vSumDir;		//	Sum direction of sides
	};

	//	Ray trace or aabb trace result
	struct TRACERT
	{
		A3DMESHPROP	MeshProperty;
		int			iNumCheckedSide;	//	Number of checked side, this value can be used for debug
	};

public:		//	Constructors and Destructors

	A3DKDTree();
	virtual ~A3DKDTree();

public:		//	Attributes

public:		//	Operations

	//	Load ESP data from file and initialize object
	bool Load(const char* szFileName);
	//	Release all resources
	virtual void Release();

	//	Do ray trace
	virtual bool RayTrace(RAYTRACERT* pTraceRt, const A3DVECTOR3& vStart, const A3DVECTOR3& vVelocity, FLOAT fTime);
	//	Do AABB trace
	virtual bool AABBTrace(AABBTRACERT* pTraceRt, AABBTRACEINFO* pInfo);
	//	Do sphere trace
	virtual bool SphereTrace(SPHERETRACERT* pTraceRt, SPHERETRACEINFO* pInfo);
	//	Split explosion mark
	virtual bool SplitMark(A3DAABB& aabb, A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
						   bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale=0.2f);

	const TRACERT& GetRayTraceResult() { return m_RayTraceRt; }
	const TRACERT& GetAABBTraceResult() { return m_AABBTraceRt; }
	const TRACERT& GetSphereTraceResult() { return m_SphereTraceRt; }

protected:	//	Attributes

	A3DSPLANE*	m_aPlanes;			//	Plane array
	SIDE*		m_aSides;			//	Side array
	NODE*		m_aNodes;			//	Node and leaf array
	A3DVECTOR3*	m_aVerts;			//	Vertex array
	int*		m_aSideRefs;		//	Side reference array
	BRUSH*		m_aBrushes;			//	Brush array
	A3DSPLANE**	m_aPlaneRefs;		//	Plane reference array

	int			m_iNumPlane;		//	Number of plane
	int			m_iNumSide;			//	Number of side
	int			m_iNumNode;			//	Number of node + leaf
	int			m_iNumVert;			//	Number of vertex
	int			m_iNumSideRef;		//	Number of side reference
	int			m_iNumBrush;		//	Number of brush
	int			m_iNumPlaneRef;		//	Number of plane reference of brushes

	DWORD		m_dwTraceCnt;		//	Trace count
	TRACEINFO	m_Trace;			//	Trace information
	AABBLEAF	m_AABBLeaf;			//	AABB leaf intersection information
	TRACERT		m_RayTraceRt;		//	Result of last ray trace
	TRACERT		m_AABBTraceRt;		//	Result of last aabb trace
	TRACERT		m_SphereTraceRt;	//	Result of last sphere trace

	AABBTRACEINFO*		m_pAABBTraceInfo;		//	AABB trace information
	SPHERETRACEINFO*	m_pSphereTraceInfo;		//	Sphere trace information

protected:	//	Operations

	//	Lump read functions
	int	ReadLump(AFile* pFile, KDTFLUMP* aLumps, int iLump, void** ppBuf, DWORD dwSize);	//	read lump
	bool ReadPlaneLump(AFile* pFile, KDTFLUMP* aLumps);			//	Read plane lump
	bool ReadSideLump(AFile* pFile, KDTFLUMP* aLumps);			//	Read side lump
	bool ReadNodeLump(AFile* pFile, KDTFLUMP* aLumps);			//	Read node lump
	bool ReadVertexLump(AFile* pFile, KDTFLUMP* aLumps);		//	Read vertex lump
	bool ReadSideRefLump(AFile* pFile, KDTFLUMP* aLumps);		//	Read side reference lump
	bool ReadBrushLump(AFile* pFile, KDTFLUMP* aLumps);			//	Read brush lump
	bool ReadBrushPlaneLump(AFile* pFile, KDTFLUMP* aLumps);	//	Read brush plane reference lump

	//	Recursive trace checking
	void RecursiveTraceCheck(NODE* pNode, float f1, float f2, const A3DVECTOR3& vP1, const A3DVECTOR3& vP2);
	//	Trace ray in leaf
	bool TraceRayInLeaf(NODE* pLeaf);
	//	Trace AABB in leaf
	bool TraceAABBInLeaf(NODE* pLeaf);
	//	Trace sphere in leaf
	bool TraceSphereInLeaf(NODE* pLeaf);
	//	Ray-side intersection routine
	bool RayToSide(SIDE* pSide);
	//	Collision routine for moving AABB and side
	bool AABBToSide(SIDE* pSide);
	//	Collision routine for moving sphere and side
	bool SphereToSide(SIDE* pSide);
	//	Intersection of AABB and leaves
	void AABBIntersectLeaves(NODE* pNode);
	//	Split mark using a side
	bool SplitMarkBySide(MARKSPLIT* pMarkSplit);
};


#endif	//	_A3DKDTREE_H_

