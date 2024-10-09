/*
 * FILE: SpacePassableOctree.cpp
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

#include "Global.h"

#include "SpacePassableOctree.h"
#include "A3DCamera.h"
#include "A3DWireCollector.h"
#include "MainFrm.h"
#include "Viewport.h"

//#include "Viewport.h"
//#include "ViewFrame.h"

#include <queue>
using namespace std;

namespace SPOctree
{

//////////////////////////////////////////////////////////////////////////
// Some utilities
//////////////////////////////////////////////////////////////////////////
A3DVECTOR3 GetCameraPos()
{
	A3DCamera * pCamera = (A3DCamera*) (AUX_GetMainFrame()->GetViewFrame()->GetActiveViewport()->GetA3DCamera());
	return pCamera->GetPos();
}

/*
void GetAllConvexHulls(abase::vector<CConvexHullData *>& ConvexHulls, bool bIgnoreTrees)
{
	ConvexHulls.clear();
}
*/

//////////////////////////////////////////////////////////////////////////
// Member functions of CSpacePassableOctreeNode
//////////////////////////////////////////////////////////////////////////

float CSpacePassableOctreeNode::s_fMinHalfSize =  1.0f;

void CSpacePassableOctreeNode::ReleaseChildren()
{
	if(IsLeaf()) return;
	
	for(int i=0;i<8;i++)
	{ 
		if(m_ChildrenPtr[i]->IsLeaf())
			delete m_ChildrenPtr[i];
		else
		{
			m_ChildrenPtr[i]->ReleaseChildren();		// a recursive call
			delete m_ChildrenPtr[i];
		}
	}
	
}

void CSpacePassableOctreeNode::ExpandChildren()
{
	bool bIncreX, bIncreY, bIncreZ;
	for(int i=0;i<8;++i)
	{
		SPOctree::Convert8To3(i, bIncreX, bIncreY, bIncreZ);
		
		CSpacePassableOctreeNode* pChild = new CSpacePassableOctreeNode;
		
		pChild->m_pParent = this;
		pChild->m_Level = m_Level+1;
		
		// compute the geometric information
		pChild->m_fHalfSize = 0.5f * m_fHalfSize;
		pChild->m_vCenter.x = (bIncreX)?(m_vCenter.x+pChild->m_fHalfSize):(m_vCenter.x-pChild->m_fHalfSize);
		pChild->m_vCenter.y = (bIncreY)?(m_vCenter.y+pChild->m_fHalfSize):(m_vCenter.y-pChild->m_fHalfSize);
		pChild->m_vCenter.z = (bIncreZ)?(m_vCenter.z+pChild->m_fHalfSize):(m_vCenter.z-pChild->m_fHalfSize);

		m_ChildrenPtr[i] = pChild;
	}
	
}

void CSpacePassableOctreeNode::ComputeState()
{
	if( m_pParent == NULL)		
	{
		// If current node is a root node, we set the state to CCompactSpacePassableOctree::CHBorder,
		// so, we can enter the BuildSubTree() function.
		m_State = CCompactSpacePassableOctree::CHBorder;
		return;
	}
		
	// compute the cube's convex hull
	A3DAABB aabb;
	aabb.Center = m_vCenter;
	aabb.Extents = A3DVECTOR3(m_fHalfSize);
	aabb.CompleteMinsMaxs();
	
	A3DOBB obb;
	obb.Build(aabb);

	CConvexHullData CHCube;
	CHCube.Import(&obb);

	abase::vector<CConvexHullData *>& ParentCHList = m_pParent->GetCHList();
	int iRes;
	bool bOnBorder = false;
	for(int i=0; i< ParentCHList.size(); ++i)
	{
		//////////////////////////////////////////////////
		// Test if two convex hull overlap.
		// Return value:
		//		0: no overlap
		//		1: overlap partly
		//		2: this CH is fully inside Another CH
		//		3: Another CH is fully inside this CH
		//////////////////////////////////////////////////		
		iRes = CHCube.ConvexHullOverlapTest( *(ParentCHList[i]) );
		if(iRes == 2)
		{
			// the cube is fully inside some convex hull
			m_State = CCompactSpacePassableOctree::Blocked;
			return;
		}

		if(iRes==1 || iRes==3)
		{
			m_CHList.push_back(ParentCHList[i]);
			bOnBorder = true;
		}
	}

	if(bOnBorder)
		m_State = CCompactSpacePassableOctree::CHBorder;
	else
		m_State = CCompactSpacePassableOctree::Free;
	
}

void CSpacePassableOctreeNode::BuildSubTree(int& iLeafCount, int& iNodeCount)
{
	// first, we compute the state of this node
	ComputeState();
		
	if( m_State == CCompactSpacePassableOctree::CHBorder && m_fHalfSize > s_fMinHalfSize)
	{
		// need to expand this node and build the subtree of its' children
		// so, this is really a recursive procedure.
		iNodeCount++;
		ExpandChildren();
		for(int i=0; i < 8; ++i)
		{
			m_ChildrenPtr[i]->BuildSubTree(iLeafCount, iNodeCount);
		}
	}
	else
	{
		// indicate this node is a leaf
		iNodeCount++;
		iLeafCount++;
		return;
	}

}

void CSpacePassableOctreeNode::RenderAABB(A3DWireCollector* pWC)
{
	// for now, we only render the CCompactSpacePassableOctree::CHBorder node
	if(m_State == CCompactSpacePassableOctree::CHBorder)
	{
		A3DAABB aabb;
		aabb.Center = m_vCenter;
		aabb.Extents = A3DVECTOR3(m_fHalfSize);
		pWC->AddAABB(aabb, 0xffff0000);				// in red color
	}
}

void CSpacePassableOctreeNode::RenderSubTree(A3DWireCollector* pWC)
{
	//////////////////////////////////////////////////////////////////////////
	// Multi-resolution render control:
	//		1. m_fHalfSize > 100 m, render if I'm a leaf node
	//		2. m_fHalfSize < 100 m && m_fHalfSize > 10 m
	//			2.1 Distance > 400 m, render myself
	//			2.2 Distance < 400 m, render my children
	//		3. m_fHalfSize < 10 m
	//			3.1 Distance > 100 m, render myself
	//			3.2 Distance < 100 m, render my children
	//////////////////////////////////////////////////////////////////////////
	
	if(m_fHalfSize < 100.0f)
	{
		float fSqrDist = (GetCameraPos() - m_vCenter).SquaredMagnitude();

		if(m_fHalfSize < 10.0f )
		{
			if( fSqrDist > 10000.0f)	// Distance > 100 m
			{
				RenderAABB(pWC);
				return;
			}
		}
		else
			if(fSqrDist > 160000.0f)	// Distance > 400m
			{
				RenderAABB(pWC);
				return;
			}
	}
	
	if( IsLeaf() )
		RenderAABB(pWC);
	else
	{
		for(int i=7; i>=0; --i)
			m_ChildrenPtr[i]->RenderSubTree(pWC);
	}	
}

//////////////////////////////////////////////////////////////////////////
// member function of CSpacePassableBuildOctree
//////////////////////////////////////////////////////////////////////////
void CSpacePassableOctree::Init(const abase::vector<CConvexHullData *>& CHList,const A3DVECTOR3& vCenter, float fHalfSize, float fMinVoxelSize )
{
	m_pRoot = new CSpacePassableOctreeNode;
	m_pRoot->SetCenter(vCenter);
	m_pRoot->SetHalfSize(fHalfSize);
	m_pRoot->SetLevel(0);
	m_pRoot->SetCHList(CHList);

	CSpacePassableOctreeNode::SetMinHalfSize(fMinVoxelSize*0.5f);
}

bool CSpacePassableOctree::BuildTree()
{
	m_iLeafCount = 0;
	m_iNodeCount = 0;

	if(m_pRoot)
	{
		m_pRoot->BuildSubTree(m_iLeafCount, m_iNodeCount);
		return true;
	}
	else
		return false;
}

bool CSpacePassableOctree::Export(CCompactSpacePassableOctree& compSPOctree, UCHAR MapID, float fXOffset,float fZOffset, float fYOffset)
{
	// *************** Revised by wenfeng, 05-9-29
	// We should assure CSpacePassableOctreeNode::s_fMinHalfSize, while not m_pRoot->m_fHalfSize is less than  CCompactSpacePassableOctree::MIN_CUBE_HALFSIZE!
	if( (!m_pRoot) || CSpacePassableOctreeNode::s_fMinHalfSize < CCompactSpacePassableOctree::MIN_CUBE_HALFSIZE)
		return false;
	
	// do not include the root!
	compSPOctree.Release();

	compSPOctree.m_iNodesNum = GetNodeCount()-1;
	compSPOctree.m_Nodes = new UINT[compSPOctree.m_iNodesNum];

	compSPOctree.m_Cube.Center.x = (int)(m_pRoot->m_vCenter.x);
	compSPOctree.m_Cube.Center.y = (int)(m_pRoot->m_vCenter.y);
	compSPOctree.m_Cube.Center.z = (int)(m_pRoot->m_vCenter.z);
	compSPOctree.m_Cube.HalfSize = (int)(m_pRoot->m_fHalfSize);

	// ***************** Added by wenfeng, 05-9-28
	// Revise the compSPOctree's absolute position by add the offsets to its' center
	// And fill the compSPOctree's m_OctreeID!
	compSPOctree.SetOctreeID(MapID);
	compSPOctree.m_Cube.Center.x += (int)fXOffset;
	compSPOctree.m_Cube.Center.y += (int)fYOffset;
	compSPOctree.m_Cube.Center.z += (int)fZOffset;
	// set the minimum half size
	compSPOctree.m_iLeafNodeSize = 2 * (int) CSpacePassableOctreeNode::s_fMinHalfSize;

	// now, we start insert all nodes to compSPOctree
	// we use Breadth-First traverse.
	UINT uiNodeCur=0;
	UINT uiChildPosCur = 0;
	
	UINT uiState;
	UINT uiChildAddr;
	
	queue <CSpacePassableOctreeNode* > queNodes;
	for(int i=0; i<8; ++i)
		queNodes.push(m_pRoot->GetChild(i));
	CSpacePassableOctreeNode* pCurNode;

	while(!queNodes.empty())
	{
		// popup
		pCurNode = queNodes.front();
		queNodes.pop();

		// push the children into the queue
		if(!pCurNode->IsLeaf())
		{
			++uiChildPosCur;
			for(int i=0; i<8; ++i)
				queNodes.push(pCurNode->GetChild(i));
			
			uiChildAddr = uiChildPosCur;
		}
		else
			uiChildAddr = NULL;			// indicate current node is a leaf
		
		// insert the information to the compSPOctree
		uiState = pCurNode->GetState();
		uiState<<= 30;
		compSPOctree.m_Nodes[uiNodeCur] = uiState | uiChildAddr;

		++uiNodeCur;
	}
	
	return true;
}

void CSpacePassableOctree::Render(A3DWireCollector* pWC)
{
	if(m_pRoot) m_pRoot->RenderSubTree(pWC);
}

}	// end of namespace