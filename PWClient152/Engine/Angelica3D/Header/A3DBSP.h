/*
 * FILE: A3DBSP.h
 *
 * DESCRIPTION: Routines for BSP algorithm and file
 *
 * CREATED BY: duyuxin, 2002/4/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBSP_H_
#define _A3DBSP_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DBSPFile.h"
#include "A3DSceneVisible.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Set and get bit in a PVS stream
#define SETBSPPVSBIT(b, i)	((b)[(i) >> 3] |= (1 << ((i) & 7)))
#define GETBSPPVSBIT(b, i)	((b)[(i) >> 3] & (1 << ((i) & 7)))

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DFrustum;
class AFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DBSP
//
///////////////////////////////////////////////////////////////////////////

class A3DBSP : public A3DSceneVisible
{
public:		//	Types

	typedef struct _BSPNODE
	{
		_BSPNODE*	pParent;		//	Parent node
		DWORD		dwFrameCnt;		//	Visible frame counter
		A3DSPLANE*	pPlane;			//	Plane
		int			aChildren[2];	//	Negative numbers are -(leafs+1), not nodes
		A3DVECTOR3	vMins;			//	Bound box
		A3DVECTOR3	vMaxs;
		bool		bHasurf;		//	Whether this node has draw surface
	
	} BSPNODE, *PBSPNODE;

	typedef struct _BSPLEAF
	{
		PBSPNODE	pParent;		//	Parent node
		DWORD		dwFrameCnt;		//	Visible frame counter
		int			iCluster;		//	-1: opaque leaf; -2: node
		int			iArea;
		A3DVECTOR3	vMins;			//	Bound box
		A3DVECTOR3	vMaxs;
		int*		aSurfRefs;		//	First surface reference
		int			iNumSurfRef;	//	Number of surface reference
		BYTE*		aPVS;			//	PVS stream of this leaf
	
	} BSPLEAF, *PBSPLEAF;

public:		//	Constructors and Destructors

	A3DBSP();
	virtual ~A3DBSP();

public:		//	Attributes

public:		//	Operations

	//	Load ESP data from file and initialize object
	bool Load(char* szFileName);
	//	Release all resources
	virtual void Release();

	//	Get visible surfaces
	virtual DWORD GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf);

	//	Check whether specified point is visible or not at current frame
	virtual bool PointIsVisible(const A3DVECTOR3& vPos)
	{
		if (m_iViewLeaf < 0)
			return true;

		return CanBeSeenFromLeaf(vPos, m_iViewLeaf);
	}

	//	Search the leaf in which specified point exists
	bool PointInLeaf(const A3DVECTOR3& vPos, int* piLeafID);
	//	Check whether a position can be seen from specified leaf
	bool CanBeSeenFromLeaf(const A3DVECTOR3& vPos, int iLeaf);

	//	Check whether a position is out of BSP space
	inline bool	IsOutBSPSpace(A3DVECTOR3& vPos);
	//	Use which method when call GetVisibleSurfs().
	inline void RecursiveNodes(bool bTrue);

	//	Below functions can be used to debug PVS ...

	//	Get the first visible cluster from specified poistion
	int	GetFirstVisibleCluster(A3DVECTOR3& vPos, int* piLeafID);
	//	Get next visible cluster
	int GetNextVisibleCluster();
	//	Get cluster's bounding box
	void GetClusterAABB(int iCluster, A3DVECTOR3& vMins, A3DVECTOR3& vMaxs);

	//	Get leaf's index in which the camera is
	int GetViewLeaf() { return m_iViewLeaf; } 

protected:	//	Attributes

	PBSPNODE	m_aNodes;			//	BSP nodes
	PBSPLEAF	m_aLeaves;			//	BSP leaves
	A3DSPLANE*	m_aPlanes;			//	BSP planes
	int*		m_aSurfRefs;		//	Draw surface reference
	BYTE*		m_aPVS;				//	PVS stream
	PBSPLEAF*	m_aClusters;		//	Clusters

	int			m_iNumNode;			//	Number of nodes
	int			m_iNumLeaf;			//	Number of leaf
	int			m_iNumPlane;		//	Number of plane
	int			m_iNumSurfRef;		//	Number of draw surface reference
	int			m_iPVSSize;			//	Size of PVS stream in bytes
	int			m_iLeafPVSSize;		//	PVS size (in bytes) for each cluster
	int			m_iNumCluster;		//	Number of cluster

	int			m_iViewLeaf;		//	Index of leaf viewpoint is in
	A3DFrustum*	m_pFrustum;			//	Current view frustum
	bool		m_bRecursive;		//	Use quake's recursive mothod when call GetVisibleSurfs()
	BYTE*		m_aSRFlags;			//	Surface rendered flag
	int*		m_aRSurfs;			//	Index's of surface will be rendered
	int			m_iNumDrawSurf;		//	Number of draw surfaces
	int			m_iNumVisSurf;		//	Number of visible surfaces every frame

	//	Used by quake's recursive mothod when call GetVisibleSurfs()
	DWORD		m_dwFrameCnt;		//	Visible frame counter
	A3DVECTOR3	m_vViewPos;			//	Current view position

	//	Used by enumerate method when call GetVisibleSurfs()
	BYTE*		m_aFCFlags;			//	Flags of clusters which have draw surfaces
	PBSPLEAF*	m_aRClusters;		//	Clusters will be rendered
	int			m_iNumRCluster;		//	Number of clusters in m_aRClusters

	//	Used by GetFirstVisibleCluster() and GetNextVisibleCluster()
	PBSPLEAF	_m_pCurCluster;		//	Current cluster
	int			_m_iPVSCnt;			//	PVS cluster counter

protected:	//	Operations

	//	Lump read functions
	int			ReadLump(AFile* pFile, PBSPFILEHEADER pHeader, int iLump, void** ppBuf, int iSize);	//	read lump
	bool		ReadPlaneLump(AFile* pFile, PBSPFILEHEADER pHeader);	//	Read plane lump
	bool		ReadNodeLump(AFile* pFile, PBSPFILEHEADER pHeader);		//	Read node lump
	bool		ReadLeafLump(AFile* pFile, PBSPFILEHEADER pHeader);		//	Read leaf lump
	bool		ReadSurfRefLump(AFile* pFile, PBSPFILEHEADER pHeader);	//	Read draw surface reference lump
	bool		ReadPVSLump(AFile* pFile, PBSPFILEHEADER pHeader);		//	Read PVS lump

	bool		InitOthers();		//	Initialize all other buffers
	void		MarkLeaves(int iViewCluster);							//	Mark leaves can be seen
	void		RecursiveWorldNode(BSPNODE* pNode, bool bNeedClip);		//	Record all surfaces will be drawn

	bool		E_GetVisibleSurfs(int iLeaf);	//	Get visible surfaces use enumerate method
	bool		R_GetVisibleSurfs(int iLeaf);	//	Get visible surfaces use recursive method
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DBSP
//
///////////////////////////////////////////////////////////////////////////

/*	Check whether a position is out of BSP space

	Return true if specified position is out of BSP space, otherwise return false.

	vPos: specified position.
*/
bool A3DBSP::IsOutBSPSpace(A3DVECTOR3& vPos)
{
	PBSPNODE pRoot = &m_aNodes[0];

	if (vPos.x < pRoot->vMins.x || vPos.x > pRoot->vMaxs.x ||
		vPos.y < pRoot->vMins.y || vPos.y > pRoot->vMaxs.y ||
		vPos.z < pRoot->vMins.z || vPos.z > pRoot->vMaxs.z)
		return true;

	return false;
}

/*	Use which method when call GetVisibleSurfs().

	bTrue: true, use quake's recursive method. false, use enumerate method
*/
void A3DBSP::RecursiveNodes(bool bTrue)
{
	m_iViewLeaf	 = -1;
	m_bRecursive = bTrue;
}


#endif	//	_A3DBSP_H_

