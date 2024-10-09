  /*
 * FILE: SpacePassableOctree.h
 *
 * DESCRIPTION:   a set of classes which describe the space passable information. 
 *        		  It's implemented by an octree! When the octree is generated,it can
 *				  export the data to CompactSpacePassableOctree for server!
 *
 * CREATED BY: He wenfeng, 2005/7/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SPACEPASSABLEOCTREE_H_
#define _SPACEPASSABLEOCTREE_H_

#include <memory.h>
#include <A3DVector.h>
#include "CompactSpacePassableOctree.h"

#include "vector.h"

#include "ConvexHullData.h"
using namespace CHBasedCD;

class A3DWireCollector;

namespace SPOctree
{

// ****** Noted by wenfeng, 05-9-27
// The following interface has been laid in CSceneObjectManager!
// For now, we only implement the default case, that is we ignore the trees' convex hulls
// void GetAllConvexHulls(abase::vector<CConvexHullData *>& ConvexHulls, bool bIgnoreTrees = true);

class CSpacePassableOctreeNode
{
	friend class CSpacePassableOctree;
	
public:
	
	// Default constructor and deconstructor
	CSpacePassableOctreeNode()
	{
		m_pParent = NULL;
		memset(m_ChildrenPtr, 0, 8 * sizeof(CSpacePassableOctreeNode *));
	}
	
	~CSpacePassableOctreeNode()
	{
		// Note: the release of parent and children shouldn't be laid here.
	}

	// Release the subtree of this node! This is a self-recursive function.
	// Note: here we don't delete current node
	void ReleaseChildren();	

	//////////////////////////////////////////////////////////////////////////
	// The main function of the class: To build the sub-tree of current node!
	//////////////////////////////////////////////////////////////////////////
	void BuildSubTree(int& iLeafCount, int& iNodeCount);


	// Render interface
	void RenderAABB(A3DWireCollector* pWC);
	void RenderSubTree(A3DWireCollector* pWC);

	// Set and Get method
	UCHAR GetState()
	{
		return m_State;
	}

	void SetState(UCHAR state)
	{
		m_State = state;
	}

	CSpacePassableOctreeNode* GetChild(int ChildID)
	{
		// for speed, we don't validate the ChildID here!
		return m_ChildrenPtr[ChildID];
	}

	CSpacePassableOctreeNode* GetParent()
	{
		return m_pParent;
	}
	
	UCHAR GetLevel()
	{
		return m_Level;
	}

	void SetLevel(UCHAR level)
	{
		m_Level = level;
	}

	void SetCenter(const A3DVECTOR3& vCenter)
	{
		m_vCenter = vCenter;
	}

	void SetHalfSize(float fHalfSize)
	{
		m_fHalfSize = fHalfSize;
	}

	bool IsLeaf()
	{
		return (m_ChildrenPtr[0]==NULL);
	}

	void SetCHList(const abase::vector<CConvexHullData *>& CHList)
	{
		m_CHList = CHList;
	}

	abase::vector<CConvexHullData *>& GetCHList()
	{
		return m_CHList;
	}

	static void SetMinHalfSize(float fMinHalfSize)
	{
		s_fMinHalfSize = fMinHalfSize;
	}

private:
	// Expand the current node and new its' children nodes! 
	void ExpandChildren();

	// Compute my state and compute my convex hull list!
	void ComputeState();

private:
	// the state to store
	UCHAR m_State;

	// geometric information of the cube corresponding to the node
	A3DVECTOR3 m_vCenter;
	float m_fHalfSize;
	
	// hiberarchy information
	CSpacePassableOctreeNode * m_pParent;
	CSpacePassableOctreeNode * m_ChildrenPtr[8];
	UCHAR m_Level;
	
	// Convex hulls which are fully inside the node's AABB
	abase::vector<CConvexHullData *> m_CHList;

	// the minimum half-size of the cell
	static float s_fMinHalfSize;
	
};


class CSpacePassableOctree
{

public:

	CSpacePassableOctree() 
	{
		m_pRoot = NULL;
	}

	~CSpacePassableOctree() 
	{
		Release();
	}

	// release the whole octree
	void Release()
	{
		if(m_pRoot)
		{
			m_pRoot->ReleaseChildren();
			delete m_pRoot;
			m_pRoot = NULL;
		}
	}

	void Init(const abase::vector<CConvexHullData *>& CHList, const A3DVECTOR3& vCenter=A3DVECTOR3(0.0f, 512.0f, 0.0f), float fHalfSize = 512.0f, float fMinVoxelSize = 2.0f);

	bool BuildTree();

	void Render(A3DWireCollector* pWC);

	bool Export(CCompactSpacePassableOctree& compSPOctree, UCHAR MapID, float fXOffset,float fZOffset, float fYOffset = 0 );


	// Set and Get functions
	int GetLeafCount() { return m_iLeafCount;}
	int GetNodeCount() { return m_iNodeCount;}
private:

	CSpacePassableOctreeNode* m_pRoot;	
	
	int m_iLeafCount;
	int m_iNodeCount;
};

}	// end of namespace

#endif