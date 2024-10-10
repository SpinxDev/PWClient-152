/*
 * FILE: A3DPortal.h
 *
 * DESCRIPTION: Scene visible solution using portal algorithm.
 *
 * CREATED BY: duyuxin, 2003/6/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DPORTAL_H_
#define _A3DPORTAL_H_

#include "A3DGeometry.h"
#include "A3DPortalFile.h"
#include "A3DSceneVisible.h"
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

class A3DCameraBase;
class A3DViewport;
class A3DFrustum;
class AFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DPortal
//
///////////////////////////////////////////////////////////////////////////

class A3DPortal : public A3DSceneVisible
{
public:		//	Types

	struct SECTOR;

	struct FRECT
	{
		float	left;
		float	top;
		float	right;
		float	bottom;
	};

	struct PORTAL
	{
		char		szName[PTNAME_LENGTH];	//	Name

		DWORD		dwID;			//	Portal's ID
		A3DSPLANE*	pPlane;			//	Plane on which portal exists
		SECTOR*		pFront;			//	Sector this portal face to
		SECTOR*		pBack;			//	Sector this portal back to
		PORTAL*		pOpposite;		//	Opposite portal
		A3DVECTOR3*	aVerts;			//	Portal vertices
		int			iNumVert;		//	Number of vertex
		bool		bDynamic;		//	true, the portal is generated dynamicly
		bool		bOpen;			//	true, the portal is open
		A3DVECTOR3	vMins;			//	AABB of sector part
		A3DVECTOR3	vMaxs;
		int			iIndex;			//	Sector's portal
		FRECT		rcPass;			//	Pass area
	};

	struct SECTORPART
	{
		A3DAABB		aabb;			//	AABB of sector part
		int*		aSurfs;			//	Draw surface indices
		int			iNumSurf;		//	Number of surface belonging to this sector part
		DWORD		dwTraceCnt;		//	Trace counter
	};

	struct SECTOR
	{
		A3DAABB		aabb;			//	AABB of sector
		SECTORPART*	aParts;			//	Sector parts
		int			iNumPart;		//	Number of sector part
		int			iIndex;			//	Sector's index
		bool		bOnTrail;		//	true, being on trail
		DWORD		dwTraceCnt;		//	Trace counter

		APtrArray<A3DSPLANE*>	aPlanes;	//	Bounding planes of sector
		APtrArray<PORTAL*>		aPortals;	//	Portals
	};

	struct BSPNODE
	{
		A3DSPLANE*	pPlane;			//	Plane index. NULL means leaf
		SECTOR*		pSector;		//	Only valid for leaf, sector belongs to this leaf
		BSPNODE*	pParent;		//	Parent node
		BSPNODE*	aChildren[2];	//	Children nodes
	};

	//	Intersect information of AABB and BSP leaves
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

public:		//	Constructors and Destructors

	A3DPortal();
	virtual ~A3DPortal() {}

public:		//	Attributes

#ifdef _DEBUG

	AArray<int, int>	m_aPVSector;		//	Potential visible sector
	int					m_iNumPVSector;

#endif	

public:		//	Operations

	//	Initialize object
	bool Load(const char* szFile);
	//	Release objects
	virtual void Release();

	//	Get visible surfaces
	virtual DWORD GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf);
	//	Force to recalculate visible set this frame
	virtual void RecalcVisibleSet() { m_bCompRecord = false; }

	//	Check whether specified AABB is visible or not at current frame
	virtual bool AABBIsVisible(const A3DAABB& aabb);
	//	Check whether specified point is visible or not at current frame
	virtual bool PointIsVisible(const A3DVECTOR3& vPos);
	
	//	Calculate sectors occupied by specified aabb
	void CalcOccupiedSectors(const A3DAABB& aabb, BYTE* aOccupiedFlags);

	//	Set AABB sector flag
	void SetAABBSectorFlag(bool bAABB) { m_bAABBSector = bAABB; }
	//	Get AABB sector flag
	bool GetAABBSectorFlag() { return m_bAABBSector; }
	//	Set check parts flag
	void SetCheckPartFlag(bool bCheck) { m_bCheckParts = bCheck; }
	//	Get check parts flag
	bool GetCheckPartFlag() { return m_bCheckParts; }

	//	Get sector
	SECTOR* GetSector(int iIndex) { return m_aSectors[iIndex]; }
	//	Get portal
	PORTAL* GetPortal(int iIndex) { return m_aPortals[iIndex]; }
	PORTAL* GetPortal(const char* szName, int* piIndex=NULL);
	//	Get sector which specified point is in
	SECTOR* SearchSector(const A3DVECTOR3& vPos);
	//	Sector is visible ?
	bool IsSectorVisible(int iIndex) { return (m_aSectors[iIndex]->dwTraceCnt == m_dwTraceCnt) ? true : false; }
	//	Open / Close portal
	void OpenPortal(int iIndex, bool bOpen) { m_aPortals[iIndex]->bOpen = bOpen; }
	
	//	Get sector number
	int GetSectorNum() { return m_aSectors.GetSize(); }
	//	Get portal number
	int GetPortalNum() { return m_aPortals.GetSize(); }

protected:	//	Attributes

	APtrArray<SECTOR*>	m_aSectors;			//	Sectors
	APtrArray<PORTAL*>	m_aPortals;			//	Portals

	A3DSPLANE*	m_aPlanes;			//	Planes
	SECTORPART*	m_aSectParts;		//	Sector parts
	int*		m_aSectPlaneRefs;	//	Sector plane references
	int*		m_aSectPtlRefs;		//	Sector portal references
	int*		m_aPartSurfRefs;	//	Sector part draw surface references
	A3DVECTOR3*	m_aVertices;		//	Portal vertices
	BSPNODE*	m_aBSPNodes;		//	BSP nodes

	int			m_iNumPlanes;		//	Number of planes
	int			m_iNumSectPart;		//	Number of sector part
	int			m_iNumSectPlaneRef;	//	Number of sector plane reference
	int			m_iNumSectPtlRef;	//	Number of sector portal reference
	int			m_iNumPartSurfRef;	//	Number of sector part draw surface reference
	int			m_iNumVertex;		//	Number of portal vertex
	int			m_iNumBSPNodes;		//	Number of BSP nodes

	bool		m_bPortalLoaded;	//	true, portal file has been loaded
	DWORD		m_dwTraceCnt;		//	Trace counter
	AABBLEAF	m_AABBLeaf;			//	AABB and BSP leaves intersect information

	bool		m_bCompRecord;		//	Compare last frame's record
	int*		m_aVisSurfs;		//	Visible surface's indices
	BYTE*		m_aVisSurfFlags;	//	Visible surfaces' flags
	BYTE*		m_aVisSurfFlags2;	//	Last frame's Visible surfaces' flags
	int			m_iNumDrawSurf;		//	Number of draw surfaces
	int			m_iNumVisSurf;		//	Number of visible surface
	A3DMATRIX4	m_matView;			//	World to view matrix
	DWORD*		m_aSectorCnts;		//	Temporary sector couters
	DWORD		m_dwSectorCnt;		//	Current sector counter value

	bool		m_bAABBSector;		//	true, all sectors are AABB
	bool		m_bCheckParts;		//	true, check parts when judge AABB or point is visible

protected:	//	Operations

	//	Lump reading routines
	int	ReadLump(AFile* pFile, PTFLUMP* aLumps, int iLump, void** ppBuf, DWORD dwSize);
	bool ReadSectorAndPortalLumps(AFile* pFile, PTFLUMP* aLumps);
	bool ReadPlaneLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadSectorPlaneRefLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadSectorPartLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadPartSurfRefLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadVertexLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadSectorPortalRefLump(AFile* pFile, PTFLUMP* aLumps);
	bool ReadBSPNodesLump(AFile* pFile, PTFLUMP* aLumps);

	//	Create necessary buffers
	bool CreateBuffers();
	//	Get visible surfaces
	bool GetVisibleSurfaces(A3DViewport* pView);
	//	Go through sector
	bool GoThroughSector(A3DCameraBase* pCamera, A3DFrustum* pFrustum, FRECT* pPtRect, PORTAL* pPortalFrom, SECTOR* pSector);
	//	Project portal to identity view space and calculate it's 2D bounding box
	void ProjectPortal(PORTAL* pPortal, FRECT* pfRect);
	//	Release resources used by portal and sector
	void ReleasePortalResources();

	//	Collision check between AABB and sector
	bool AABBCheckWithSector(const A3DAABB& aabb, SECTOR* pSector);
	//	Intersection of AABB and leaves
	void AABBIntersectLeaves(BSPNODE* pNode);

	//	Intersect float rectangle
	bool IntersectFRect(FRECT* pDest, FRECT* prc1, FRECT* prc2)
	{
		//	Intersect rectangle
		if (prc1->right < prc2->left || prc1->left > prc2->right ||
			prc1->top < prc2->bottom || prc1->bottom > prc2->top)
			return false;	//	No intersection
	
		pDest->left		= prc1->left > prc2->left ? prc1->left : prc2->left;
		pDest->right	= prc1->right < prc2->right ? prc1->right : prc2->right;
		pDest->top		= prc1->top < prc2->top ? prc1->top : prc2->top;
		pDest->bottom	= prc1->bottom > prc2->bottom ? prc1->bottom : prc2->bottom;
		return true;
	}
	
	//	Union float rectangle
	void UnionFRect(FRECT* pDest, FRECT* prc1, FRECT* prc2)
	{
		pDest->left		= prc1->left < prc2->left ? prc1->left : prc2->left;
		pDest->right	= prc1->right > prc2->right ? prc1->right : prc2->right;
		pDest->top		= prc1->top > prc2->top ? prc1->top : prc2->top;
		pDest->bottom	= prc1->bottom < prc2->bottom ? prc1->bottom : prc2->bottom;
	}
	
	//	Float rectangle 1 complete in 2 ?
	bool FRectInFRect(FRECT* prc1, FRECT* prc2)
	{
		if (prc1->left >= prc2->left && prc1->left <= prc2->right &&
			prc1->right >= prc2->left && prc1->right <= prc2->right &&
			prc1->top >= prc2->bottom && prc1->top <= prc2->top &&
			prc1->bottom >= prc2->bottom && prc1->bottom <= prc2->top)
			return true;
	
		return false;
	}
};
	
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DPortal_H_

