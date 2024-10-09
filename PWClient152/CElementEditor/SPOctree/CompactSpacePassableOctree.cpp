/*
 * FILE: CompactSpacePassableOctree.cpp
 *
 * DESCRIPTION:   a class which describe the space passable information. 
 *                It's implemented by an octree while using a compact data structure!
 *				  In the class, each node of the octree take up only 4 bytes to store all the information.
 *
 *				  This class will be used in the game server for full 3D space path-finding!
 *				  So the class can't employ any function and class of MFC or Windows!
 *
 * CREATED BY: He wenfeng, 2005/7/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "CompactSpacePassableOctree.h"
#include <stack>

namespace SPOctree
{

// some free functions
void Convert8To3(int Input, bool& OutHigh, bool& OutMid, bool& OutLow)
{
	// for speed, we don't validate the Input here!
	OutHigh = Input & 4 /* ...0100 */;
	OutMid = Input & 2 /* ...0010 */;
	OutLow = Input & 1 /* ...0001 */;
}

void Convert3To8(bool InHigh, bool InMid, bool InLow, int& Output)
{
	Output = 0;
	if(InHigh) Output |= 4 /* ...0100 */;
	if(InMid) Output  |= 2 /* ...0010 */;
	if(InLow) Output |= 1 /* ...0001 */;
}





// implement of class functions
const float CCompactSpacePassableOctree::MIN_CUBE_HALFSIZE = 1.0f;

// file version
const UINT uiCSPOctreeFileVer = 0xcc000001; 

// using for traverse method, a local structure
struct s_NodeInfo
{
	// geometric info
	CubeInt Cube;

	// children info
	UINT uiNode;
};

void CCompactSpacePassableOctree::Traverse(PtrFuncVisitNode pfVN, void *pPara, bool bOnlyLeaves)
{
	if( !m_Nodes ) return;

	// here we should use a depth-first traverse method 
	// so that we can get the geometric information of each node!
	// To do so, we employ a stack to store the history info and handle the back traverse!
	
	std::stack<s_NodeInfo>	stackNodes;
	bool bIncreX, bIncreY, bIncreZ;

	// first, we push all eight one-level children into the stack
	s_NodeInfo curNode;
	curNode.Cube.HalfSize = m_Cube.HalfSize>>1;
	for(int i =0; i<8; ++i)
	{
		Convert8To3(i, bIncreX, bIncreY, bIncreZ);
		curNode.Cube.Center.x = (bIncreX)?(m_Cube.Center.x + curNode.Cube.HalfSize ):(m_Cube.Center.x - curNode.Cube.HalfSize );
		curNode.Cube.Center.y = (bIncreY)?(m_Cube.Center.y + curNode.Cube.HalfSize ):(m_Cube.Center.y - curNode.Cube.HalfSize );
		curNode.Cube.Center.z = (bIncreZ)?(m_Cube.Center.z + curNode.Cube.HalfSize ):(m_Cube.Center.z - curNode.Cube.HalfSize );
		curNode.uiNode = m_Nodes[i];

		stackNodes.push(curNode);
	}

	while( !stackNodes.empty() )
	{
		curNode = stackNodes.top();
		stackNodes.pop();
		
		// visit current node!
		if(GetChildenAddr(curNode.uiNode) ==NULL)
		{
			// leaf node!
			// visit the leaf node
			pfVN(pPara, curNode.Cube, GetState(curNode.uiNode));
		}
		else
		{
			// non-leaf node
			if(!bOnlyLeaves)
				pfVN(pPara, curNode.Cube, GetState(curNode.uiNode));

			// push children of current node
			s_NodeInfo childNode;

			//////////////////////////////////////////////////////////////////////////
			// Note : problems maybe occur here!
			//			Since when Cube.HalfSize == 1, after the operation, it becomes 0! but what we expect
			//           is 0.5f, so, the minimum cube size we can handle by these code is 2m!
			//////////////////////////////////////////////////////////////////////////
			childNode.Cube.HalfSize = curNode.Cube.HalfSize >> 1;
			for(int i =0; i<8; ++i)
			{
				Convert8To3(i, bIncreX, bIncreY, bIncreZ);
				childNode.Cube.Center.x = (bIncreX)?(curNode.Cube.Center.x + childNode.Cube.HalfSize):(curNode.Cube.Center.x - childNode.Cube.HalfSize);
				childNode.Cube.Center.y = (bIncreY)?(curNode.Cube.Center.y + childNode.Cube.HalfSize):(curNode.Cube.Center.y - childNode.Cube.HalfSize);
				childNode.Cube.Center.z = (bIncreZ)?(curNode.Cube.Center.z + childNode.Cube.HalfSize):(curNode.Cube.Center.z - childNode.Cube.HalfSize);
				UINT uiChildAddr = (GetChildenAddr(curNode.uiNode) << 3) + i;
				childNode.uiNode = m_Nodes[uiChildAddr];

				stackNodes.push(childNode);
			}			
		}
	}

}

bool CCompactSpacePassableOctree::GetTraversalNode(const Pos3DInt& Pos, CSPOctreeTravNode& tNode, CSPOctreeTravNode* pRefTNode)
{

	if(!m_Nodes) return false;

	if(pRefTNode && pRefTNode->IsPosInside(Pos))
	{
		tNode = *pRefTNode;
		return true;
	}
	
	int x = Pos.x;
	int y = Pos.y;
	int z = Pos.z;
	
	bool bIncreX, bIncreY, bIncreZ;
	int ChildID;
	UINT uiChildAddr;
	bool bHitRefNode;

	// tNode's OctreeID set to m_OctreeID
	tNode.OctreeID = m_OctreeID;

	if(pRefTNode && pRefTNode->ParentCube.Inside(Pos))
	{
		// 	Pos lies in the subtree of the pRefTNode, so we can search it from the pRefTNode
		tNode.ParentCube = pRefTNode->ParentCube;
		tNode.Level = pRefTNode->Level;
		uiChildAddr = pRefTNode->FirstSiblingPos;
		bHitRefNode = true;
	}
	else
	{
		// search the Pos 's corresponding node from the root
		tNode.ParentCube = m_Cube;
		tNode.Level = 1;
		uiChildAddr = 0;
		bHitRefNode = false;
	}

	
	bIncreX = x > tNode.ParentCube.Center.x;
	bIncreY = y > tNode.ParentCube.Center.y;
	bIncreZ = z > tNode.ParentCube.Center.z;

	tNode.CurrentCube.HalfSize = tNode.ParentCube.HalfSize >> 1;
	tNode.CurrentCube.Center.x = (bIncreX)?( tNode.ParentCube.Center.x +tNode.CurrentCube.HalfSize ) : ( tNode.ParentCube.Center.x - tNode.CurrentCube.HalfSize );
	tNode.CurrentCube.Center.y = (bIncreY)?( tNode.ParentCube.Center.y + tNode.CurrentCube.HalfSize ) : ( tNode.ParentCube.Center.y - tNode.CurrentCube.HalfSize );
	tNode.CurrentCube.Center.z = (bIncreZ)?( tNode.ParentCube.Center.z + tNode.CurrentCube.HalfSize ) : ( tNode.ParentCube.Center.z - tNode.CurrentCube.HalfSize );

	Convert3To8(bIncreX,bIncreY,bIncreZ, ChildID);
	tNode.State = GetState(m_Nodes[ (uiChildAddr << 3) + ChildID]);
	
	tNode.FirstSiblingPos = uiChildAddr;
	tNode.OffsetInSibling = ChildID;

	// update the child address
	uiChildAddr=GetChildenAddr(m_Nodes[(uiChildAddr << 3) + ChildID]);		


	while(uiChildAddr!=NULL)
	{
		// not the leaf yet!
		tNode.ParentCube = tNode.CurrentCube;

		bIncreX = x > tNode.CurrentCube.Center.x;
		bIncreY = y > tNode.CurrentCube.Center.y;
		bIncreZ = z > tNode.CurrentCube.Center.z;

		//////////////////////////////////////////////////////////////////////////
		// Note : problems maybe occur here!
		//		Since when Cube.HalfSize == 1, after the operation, it becomes 0! but what we expect
		//      is 0.5f, so, the minimum cube size we can handle by these code is 2m!
		//////////////////////////////////////////////////////////////////////////
		tNode.CurrentCube.HalfSize  >>= 1;

		tNode.CurrentCube.Center.x = (bIncreX)?( tNode.CurrentCube.Center.x + tNode.CurrentCube.HalfSize ) : ( tNode.CurrentCube.Center.x - tNode.CurrentCube.HalfSize );
		tNode.CurrentCube.Center.y = (bIncreY)?( tNode.CurrentCube.Center.y + tNode.CurrentCube.HalfSize ) : ( tNode.CurrentCube.Center.y - tNode.CurrentCube.HalfSize );
		tNode.CurrentCube.Center.z = (bIncreZ)?( tNode.CurrentCube.Center.z + tNode.CurrentCube.HalfSize ) : ( tNode.CurrentCube.Center.z - tNode.CurrentCube.HalfSize );
		tNode.Level ++;

		Convert3To8(bIncreX,bIncreY,bIncreZ, ChildID);
		tNode.State = GetState(m_Nodes[(uiChildAddr<<3) + ChildID]);

		tNode.FirstSiblingPos = uiChildAddr;
		tNode.OffsetInSibling = ChildID;

		uiChildAddr=GetChildenAddr(m_Nodes[(uiChildAddr<<3) + ChildID]);		
	}

	return bHitRefNode;
}

bool CCompactSpacePassableOctree::Load(const char* szFile)
{
	FILE * FileToLoad = fopen(szFile, "rb");
	if(FileToLoad)
	{
		bool bLoaded = Load(FileToLoad);
		fclose(FileToLoad);
		return bLoaded;
	}
	else
		return false;
}

bool CCompactSpacePassableOctree::Load(FILE* FileToLoad)
{
	if(!FileToLoad) return false;
	size_t NumRead;

	// first, we read and test the file version
	UINT uiVer;
	NumRead = fread(&uiVer,sizeof(UINT),1,FileToLoad);
	if(NumRead!=1)
		return false;
	if(uiVer != uiCSPOctreeFileVer)
		return false;					// wrong file version

	// read the Octree ID
	NumRead = fread(&m_OctreeID,sizeof(UCHAR),1,FileToLoad);
	if(NumRead!=1)
		return false;

	// read the Cube information
	NumRead = fread(&m_Cube,sizeof(CubeInt),1,FileToLoad);
	if(NumRead!=1)
		return false;

	// read the minimum half size
	NumRead = fread(&m_iLeafNodeSize,sizeof(int),1,FileToLoad);
	if(NumRead!=1)
		return false;
	
	// read the number of nodes
	NumRead = fread(&m_iNodesNum,sizeof(UINT),1,FileToLoad);
	if(NumRead!=1)
		return false;

	// release the current data and new some data
	Release();
	m_Nodes = new UINT[m_iNodesNum];

	// read all the nodes
	NumRead = fread(m_Nodes,sizeof(UINT),m_iNodesNum,FileToLoad);
	if(NumRead!=m_iNodesNum)
		return false;
	
	return true;
}

bool CCompactSpacePassableOctree::Save(const char* szFile)
{
	FILE *FileToSave = fopen(szFile, "wb");
	if(FileToSave)
	{
		bool bSaved = Save(FileToSave);
		fclose(FileToSave);
		return bSaved;
	}
	else
		return false;
}

bool CCompactSpacePassableOctree::Save(FILE* FileToSave)
{
	if(!FileToSave) return false;
	
	size_t NumWrite;
	// first, we write the file version
	NumWrite = fwrite(&uiCSPOctreeFileVer,sizeof(UINT),1,FileToSave);
	if(NumWrite!=1)
		return false;

	// write the Octree ID
	NumWrite = fwrite(&m_OctreeID,sizeof(UCHAR),1,FileToSave);
	if(NumWrite!=1)
		return false;

	// write the Cube information
	NumWrite = fwrite(&m_Cube,sizeof(CubeInt),1,FileToSave);
	if(NumWrite!=1)
		return false;
	
	// write the minimum half size
	NumWrite = fwrite(&m_iLeafNodeSize,sizeof(int),1,FileToSave);
	if(NumWrite!=1)
		return false;
	
	// write the number of nodes
	NumWrite = fwrite(&m_iNodesNum,sizeof(UINT),1,FileToSave);
	if(NumWrite!=1)
		return false;

	// write all the nodes
	NumWrite = fwrite(m_Nodes,sizeof(UINT),m_iNodesNum,FileToSave);
	if(NumWrite!=m_iNodesNum)
		return false;
	
	return true;
}

};	// end of namespace