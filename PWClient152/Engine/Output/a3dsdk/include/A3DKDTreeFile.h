/*
 * FILE: A3DKDTreeFile.h
 *
 * DESCRIPTION: Data used for KD-Tree file
 *
 * CREATED BY: Duyuxin, 2003/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DKDTREEFILE_H_
#define _A3DKDTREEFILE_H_

#include "ABaseDef.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	First 8 bytes in portal file
#define	KDTFILE_IDENTIFY	(('A'<<24) | ('K'<<16) | ('D'<<8) | 'T')
#define KDTFILE_VERSION		2	

//	Lumps in portal file file
enum
{
	KDTLUMP_VERTICES = 0,	//	Vertices
	KDTLUMP_PLANES,			//	Planes
	KDTLUMP_NODES,			//	Nodes
	KDTLUMP_LEAFSIDES,		//	Leaf side references
	KDTLUMP_SIDES,			//	Sides
	KDTLUMP_BRUSHES,		//	Brushes
	KDTLUMP_BRUSHPLANES,	//	Planes of brushes
	NUM_KDTLUMP,
};

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

struct KDTFLUMP
{
	DWORD	dwOff;			//	Offset from file header to lump's data
	DWORD	dwSize;			//	Size of this lump
};

struct KDTFILEHEADER
{
	DWORD	dwIdentify;		//	Identity, must be BSP_IDENTITY
	DWORD	dwVersion;		//	Version, must be BSP_VERSION
	int		iNumLump;		//	Number of lump
	BYTE	aReserved[60];	//	Reserved
};

//	Plane lump
struct KDTFPLANE
{
	float	vNormal[3];
	float	fDist;
};

struct KDTFNODE
{
	int		iPlane;			//	Plane index. -1 means leaf
	int		iParent;		//	iParent node
	int		aChildren[2];	//	Children nodes
	int		iFirstSide;		//	First side reference
	int		iNumSide;		//	Number side reference
	float	vMins[3];		//	Node's aabb
	float	vMaxs[3];
};

struct KDTFVERTEX
{
	float	vPos[3];		//	Position
};

struct KDTFSIDE
{
	int		iPlane;			//	Plane index
	int		iFirstVert;		//	First vertex in vertex lump
	int		iNumVert;		//	Number of vertex
	DWORD	dwMeshProps;	//	Mesh properties
	int		iMtlType;		//	Material type
	int		iBrush;			//	Brush index
};

struct KDTFBRUSH
{
	int		iFirstPlane;	//	First brush plane's index
	int		iNumPlane;		//	Number of planes of this brush
}; 

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_A3DKDTREEFILE_H_

