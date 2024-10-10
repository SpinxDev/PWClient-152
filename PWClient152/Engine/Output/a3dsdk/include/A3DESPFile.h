/*
 * FILE: ESPFile.h
 *
 * DESCRIPTION: Common operations for ESP file
 *
 * CREATED BY: Duyuxin, 2001/10/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ESPFILE_H_
#define _ESPFILE_H_

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	First 8 bytes in ESP file
#define	ESP_IDENTIFY			(('A'<<24) | ('E'<<16) | ('S'<<8) | 'P')
#define ESP_VERSION				5	

enum
{
	EFLUMP_LEAVES = 0,		//	Leaves
	EFLUMP_PLANES,			//	Planes
	EFLUMP_VERTICES,		//	Vertice
	EFLUMP_LEAFSIDES,		//	Leaf sides
	EFLUMP_SIDES,			//	Sides
	EFLUMP_WORLDINFO,		//	ESP world info.
	EFLUMP_BRUSHES,			//	Brushes
	EFLUMP_BRUSHPLANES,		//	Planes of brushes
	NUM_ESPLUMP,			//	Number of ESP lump
};

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

struct EFLUMP
{
	LONG		lOff;			//	Offset from file header to lump's data
	LONG		lSize;			//	Size of this lump
};

//	File header
struct ESPFILEHEADER
{
	DWORD		dwIdentify;		//	Identity, must be ESP_IDENTITY
	DWORD		dwVersion;		//	Version, must be ESP_VERSION
	int			iNumLump;		//	Number of lump
	BYTE		aReserved[60];	//	Reserved
};

struct EFWORLDINFO
{
	FLOAT		vMins[3];		//	Cluster's bounding box
	FLOAT		vMaxs[3];
	FLOAT		vAlignMins[3];	//	Align bounding box
	FLOAT		vAlignMaxs[3];
	LONG		vLength[3];		//	Length of each border of leaf
	LONG		vSize[3];		//	Size of world in leaves
};

struct EFPLANE
{
	FLOAT		vNormal[3];
	FLOAT		fDist;
};

struct EFSIDE
{
	LONG		lPlane;			//	Plane index
	LONG		lFirstVert;		//	First vertex in vertex lump
	LONG		lNumVert;		//	Number of vertex
	DWORD		dwMeshProps;	//	Mesh properties
	int			iMtlType;		//	Material type
	LONG		lBrush;			//	Brush index
};

struct EFLEAF
{
	LONG		lFirstLeafSide;	//	First leaf side
	LONG		lNumLeafSides;	//	Number of leaf sides
};

struct EFVERTEX
{
	FLOAT		vPos[3];		//	Position
};

struct EFBRUSH
{
	LONG		lFirstPlane;	//	First brush plane's index
	LONG		lNumPlane;		//	Number of planes of this brush
}; 

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_ESPFILE_H_


