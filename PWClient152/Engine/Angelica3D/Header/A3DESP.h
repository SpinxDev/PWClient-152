/*
 * FILE: A3DESP.h
 *
 * DESCRIPTION: Routines for ESP algorithm and file
 *
 * CREATED BY: duyuxin, 2001/10/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DESP_H_
#define _A3DESP_H_

#include "A3DVertex.h"
#include "A3DSceneHull.h"
#include "A3DESPFile.h"

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
//	Class A3DESP
//
///////////////////////////////////////////////////////////////////////////

class A3DESP : public A3DSceneHull
{
public:		//	Types

	struct ESPBRUSH
	{
		A3DSPLANE**	aPlanes;		//	Plane pointer array
		int			iNumPlane;		//	Number of plane reference
	};

	struct ESPSIDE
	{
		A3DSPLANE*	pPlane;			//	Plane
		A3DVECTOR3*	aVerts;			//	Vertex array of mesh to which this side belong
		int			iNumVert;		//	Number of vertex in aVerts, this in fact is mesh's vertex number
		ESPBRUSH*	pBrush;			//	Brush
		A3DMESHPROP	MeshProps;		//	Mesh properties

		A3DVECTOR3	vCenter;		//	Side's center
		A3DVECTOR3	vExtents;		//	Bounding box's extents
		A3DVECTOR3	vMins;			//	Bounding box
		A3DVECTOR3	vMaxs;

		DWORD		dwTraceCnt;		//	Trace count
	};
	
	struct ESPLEAF
	{
		int*		aSideRefs;		//	Side reference array
		int			iNumRefs;		//	Number of side reference
	};

	struct ESPWORLD
	{
		A3DVECTOR3	vCenter;		//	Cluster's center
		A3DVECTOR3	vExtents;		//	AABB's extents
		A3DVECTOR3	vMins;			//	Cluster's accurate AABB
		A3DVECTOR3	vMaxs;
		A3DVECTOR3	vAlignMins;		//	Align bounding box
		A3DVECTOR3	vAlignMaxs;
		int			vLength[3];		//	Length of each border of leaf
		int			vSize[3];		//	Size of world in leaves
		A3DVECTOR3	vInvLength;		//	Reciprocal of lengths
		
		int			iNumLeaves;		//	Number of leaves
		int			vSpans[3];		//	Spans when x, y, z increase
		DWORD		dwTraceCnt;		//	Trace count
	};

	//	Structure for 3DDDA algorithm
	struct THREEDDDA
	{
		int			iMajor;			//	Major axis
		int			iNumSteps;		//	Steps need to go
		int			vStart[3];		//	Start voxel
		int			vEnd[3];		//	End voxel
		int			sx, sy, sz;		//	Sign delta
		float		ex, ey, ez;		//	Errors' initial value
		float		ix, iy, iz;		//	Increments
		float		dx, dy, dz;		//	Deltas to next voxel
		float		cx, cy, cz;		//	Compare value
	};

	//	Internal structure for AABB trace
	struct AABBINFO
	{
		AABBTRACEINFO*	pInfo;		//	AABB trace information
		float			fFraction;	//	Collision fraction
		ESPSIDE*		pSide;		//	Nearest side
		A3DVECTOR3		vNormal;	//	Side's normal
	};

	//	Internal structure for ray trace
	struct RAYINFO
	{
		A3DVECTOR3	vStart;			//	Start point of ray segment
		A3DVECTOR3	vEnd;			//	end point of ray segment
		A3DVECTOR3	vDir;			//	Unnormalized direction (vEnd - vStart)
		int			iAxial;			//	-1: non-axial, 0, 1, 2: x, y, z

		A3DVECTOR3	vPoint;			//	Hit point
		FLOAT		fFraction;		//	Adjusted hit fraction (0 start point, 1 end point)
		ESPSIDE*	pSide;			//	Nearest side
		A3DVECTOR3	vNormal;		//	Side's normal
		FLOAT		fHitFrac;		//	Real hit fraction
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
		ESPSIDE*	pSide;			//	Current side used to split mark
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

	A3DESP();
	virtual ~A3DESP();

public:		//	Attributes

public:		//	Operations

	//	Load ESP data from file and initialize object
	bool Load(const char* szFileName);
	//	Release all resources
	virtual void Release();

	//	Do ray trace
	virtual bool RayTrace(RAYTRACERT* pTrace, const A3DVECTOR3& vStart, const A3DVECTOR3& vVelocity, FLOAT fTime);
	//	Do AABB trace
	virtual bool AABBTrace(AABBTRACERT* pTrace, AABBTRACEINFO* pInfo);

	//	Split explosion mark
	virtual bool SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
						   bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale=0.2f);

	const TRACERT& GetRayTraceResult() { return m_RayTraceRt; }
	const TRACERT& GetAABBTraceResult() { return m_AABBTraceRt; }

protected:	//	Attributes

	A3DSPLANE*	m_aPlanes;			//	Plane array
	ESPSIDE*	m_aSides;			//	Side array
	ESPLEAF*	m_aLeaves;			//	Leaf array
	ESPWORLD	m_World;			//	ESP world
	A3DVECTOR3*	m_aVerts;			//	Vertex array
	int*		m_aSideRefs;		//	Side reference array
	ESPBRUSH*	m_aBrushes;			//	Brush array
	A3DSPLANE**	m_aPlaneRefs;		//	Plane reference array

	int			m_iNumPlane;		//	Number of plane
	int			m_iNumSide;			//	Number of side
	int			m_iNumLeaf;			//	Number of leaf
	int			m_iNumVert;			//	Number of vertex
	int			m_iNumSideRef;		//	Number of side reference
	int			m_iNumBrush;		//	Number of brush
	int			m_iNumPlaneRef;		//	Number of plane reference of brushes

	DWORD		m_dwTraceCnt;		//	Trace count
	RAYINFO		m_Ray;				//	Ray trace information
	AABBINFO	m_AABB;				//	AABB trace information
	TRACERT		m_RayTraceRt;		//	Result of last ray trace
	TRACERT		m_AABBTraceRt;		//	Result of last aabb trace

	THREEDDDA	m_3DDDA;			//	3DDDA algorithm variables
	ESPLEAF*	m_pEverHitLeaf;		//	Hit ever occures in this leaf

protected:	//	Operations

	//	Lump read functions
	int			ReadLump(AFile* pFile, EFLUMP* aLumps, int iLump, void** ppBuf, int iSize);	//	read lump
	bool		ReadPlaneLump(AFile* pFile, EFLUMP* aLumps);		//	Read plane lump
	bool		ReadSideLump(AFile* pFile, EFLUMP* aLumps);			//	Read side lump
	bool		ReadLeafLump(AFile* pFile, EFLUMP* aLumps);			//	Read leaf lump
	bool		ReadWorldInfoLump(AFile* pFile, EFLUMP* aLumps);	//	Read world info. lump
	bool		ReadVertexLump(AFile* pFile, EFLUMP* aLumps);		//	Read vertex lump
	bool		ReadSideRefLump(AFile* pFile, EFLUMP* aLumps);		//	Read side reference lump
	bool		ReadBrushLump(AFile* pFile, EFLUMP* aLumps);		//	Read brush lump
	bool		ReadBrushPlaneLump(AFile* pFile, EFLUMP* aLumps);	//	Read brush plane reference lump

	//	Ray tracing
	bool		RayToWorld();								//	Trace a ray to world
	void		Init3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1);	//	Initialize variables for tracing ray in cluster
	bool		TraceRayInWorld();							//	Trace ray in ESP world
	bool		TraceRayInLeaf(ESPLEAF* pLeaf);				//	Trace ray in leaf
	bool		RayToSide(ESPSIDE* pSide, ESPLEAF* pLeaf);	//	Ray-side intersection routine
	void		AxialInit3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1);		//	Initialize variable for axial ray tracing in cluster
	bool		AxialTraceRayInWorld();						//	Axial trace ray in ESP world

	bool		TraceAABBInWorld();					//	Trace AABB in ESP world
	bool		AABBMoveToSide(ESPSIDE* pSide);		//	Collision routine for moving AABB and side

	void		SplitMarkInWorld(MARKSPLIT* pMarkSplit);	//	Split mark in a cluster
	bool		SplitMarkBySide(MARKSPLIT* pMarkSplit);		//	Split mark using a side
};


#endif	//	_A3DESP_H_

