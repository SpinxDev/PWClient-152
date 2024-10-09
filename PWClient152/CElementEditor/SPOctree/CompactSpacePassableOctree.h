 /*
 * FILE: CompactSpacePassableOctree.h
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

#ifndef _COMPACTSPACEPASSABLEOCTREE_H_
#define _COMPACTSPACEPASSABLEOCTREE_H_

#include <stdio.h>
#include <A3DVector.h>

#ifndef NULL
	#define NULL 0
#endif

namespace SPOctree
{

// redefine some types
typedef unsigned int UINT;
typedef unsigned char UCHAR;		// maybe has been defined!?

// 8-3 converter
void Convert8To3(int Input, bool& OutHigh, bool& OutMid, bool& OutLow);
void Convert3To8(bool InHigh, bool InMid, bool InLow, int& Output);


//////////////////////////////////////////////////////////////////////////
// some aux structure
//////////////////////////////////////////////////////////////////////////

// 3D pos of int type!
struct Pos3DInt
{
	int x;
	int y;
	int z;

	Pos3DInt(){};

	inline bool operator ==(const Pos3DInt& AnotherPos)
	{
		return ( x == AnotherPos.x) && ( y == AnotherPos.y) && ( z == AnotherPos.z);
	}

};

// Cube of int type
struct CubeInt
{
	Pos3DInt Center;
	int HalfSize;				// clearly, the minimum halfsize is 1!
	
	inline bool Inside(const Pos3DInt& Pos)
	{
		return  ( Pos.x < Center.x + HalfSize ) && ( Pos.x > Center.x - HalfSize ) &&
				   ( Pos.y < Center.y + HalfSize ) && ( Pos.y > Center.y - HalfSize ) &&
				   ( Pos.z < Center.z + HalfSize ) && ( Pos.z > Center.z - HalfSize );
	}

	inline bool operator == ( const CubeInt& AnotherCube)
	{
		return (HalfSize == AnotherCube.HalfSize) && ( Center == AnotherCube.Center);
	}
};

// traversal node of compact space passable octree!
// to note: for now, each node must be corresponding to a leaf of the octree!
struct CSPOctreeTravNode
{
	// geometric info
	CubeInt ParentCube;				// parent's cube
	CubeInt CurrentCube;			// my cube

	// node info
	UCHAR State;					// state of the node
	UCHAR Level;					// level of the node

	// index info
	UINT FirstSiblingPos;			// the first sibling's pos in Octree's Nodes
	UCHAR OffsetInSibling;			// 0--7

	// which CompactSpacePassableOctree I belong to?  1--88
	UCHAR OctreeID;

	inline bool IsPosInside(const Pos3DInt& Pos)
	{
		return CurrentCube.Inside(Pos);
	}

	// to test whether the tNode is neighbor sibling of mine
	inline bool IsNodeNeighborSibling(const CSPOctreeTravNode& tNode)
	{
		// Note: for now, we don't consider the case that two nodes
		// lie in different octree!

		// in the identical octree?
		if(OctreeID != tNode.OctreeID)
			return false;

		// have the same parent?
		if (FirstSiblingPos!=tNode.FirstSiblingPos)
			return false;
		
		// are neighbor
		UCHAR ucRes = OffsetInSibling ^ tNode.OffsetInSibling;
		if(ucRes == 1 || ucRes == 2 || ucRes == 4)
			return true;
		else
			return false;
	}

	inline bool operator == ( const CSPOctreeTravNode& AnotherNode)
	{
		// only need test whether these two members are identical
		return  ( OctreeID == AnotherNode.OctreeID) &&
				( FirstSiblingPos == AnotherNode.FirstSiblingPos) &&
				( OffsetInSibling == AnotherNode.OffsetInSibling);
	}
};


//////////////////////////////////////////////////////////////////////////
// Main class of the file
//////////////////////////////////////////////////////////////////////////

// define a function pointer for traverse the whole octree
typedef void (*PtrFuncVisitNode)(void *pPara, const CubeInt& Cube, UCHAR state);

class CCompactSpacePassableOctree
{
	friend class CSpacePassableOctree;

public:
	
	// the minimum cube halfsize we can support
	static const float MIN_CUBE_HALFSIZE;
	
	// state of each CCompactSpacePassableOctree's node
	enum
	{
		Free=0,					// fully empty
		Blocked,				// fully blocked
		CHBorder,				// on the border of convex hull
	};

	CCompactSpacePassableOctree()
	{
		m_Nodes = NULL;
		m_iNodesNum = 0;

		m_OctreeID = 0;			// Init to 0
	}
	
	~CCompactSpacePassableOctree()
	{
		Release();
	}

	void Release()
	{
		if(m_Nodes)
		{
			delete [] m_Nodes;
			m_Nodes = NULL;
			m_iNodesNum =0;
		}
	}
	
	// set and get m_OctreeID;
	void SetOctreeID(UCHAR id)
	{
		m_OctreeID = id;
	}

	UCHAR GetOctreeID()
	{
		return m_OctreeID;
	}
	
	int GetLeafNodeSize()
	{
		return m_iLeafNodeSize;
	}

	// transfer the position of octree
	void Transfer(int OffsetX,int OffsetY,int OffsetZ)
	{
		m_Cube.Center.x += OffsetX;
		m_Cube.Center.y += OffsetY;
		m_Cube.Center.z += OffsetZ;
	}

	// Traverse all the nodes of the compact octree!
	// pfVN:  pointer of visit function
	// pPara: parameter of pfVN besides the geometric information and state information
	// bOnlyLeaves: visit only leaves!
	void Traverse(PtrFuncVisitNode pfVN, void *pPara, bool bOnlyLeaves = true);
	
	// true if we just hit the pRefTNode, else, we return false
	bool GetTraversalNode(const Pos3DInt& Pos, CSPOctreeTravNode& tNode, CSPOctreeTravNode* pRefTNode = NULL);
	
	// Load and save interface
	bool Load(const char* szFile);
	bool Load(FILE* FileToLoad);

	bool Save(const char* szFile);
	bool Save(FILE* FileToSave);

protected:
	
	inline UINT GetChildenAddr(UINT uiNode)
	{
		return ( uiNode & 0x03ffffff);
	}

	inline UCHAR GetState(UINT uiNode)
	{
		return (uiNode>>30);
	}

protected:
	
	// ******** Noted by wenfeng, 05-9-27
	// How about the case of instance map?
	// octree id, corresponding to each map of the world!
	// id range: 1 -- 88 ?
	UCHAR m_OctreeID;

	// geometric information of the initial space (an AABB), 
	// for speed consideration, we use the data type "int"
	CubeInt m_Cube;

	// ******** Added by wenfeng, 05-9-29
	// the lowest-level node (leaf node)'s halfsize which should be >= MIN_CUBE_HALFSIZE and always the power of 2
	int m_iLeafNodeSize;

	// nodes of the octree	
	//---------------------------------------------------------------------------------------
	// node bits representation:
	// xx . xxxx . xxxxxxxxxxxxxxxxxxxxxxxxxx  (32 bits)
	//
	// ^^: 2 bits state
	//
	//      ^^^^: 4 bits reserved
	//
	//             ^^^^^^^^^^^^^^^^^^^^^^^^^^: 26 bits children start address
	//---------------------------------------------------------------------------------------
	UINT m_iNodesNum;
	UINT* m_Nodes;
};

};	// end of namespace

#endif