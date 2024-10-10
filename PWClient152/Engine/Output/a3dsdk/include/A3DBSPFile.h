/*
 * FILE: BSPFile.h
 *
 * DESCRIPTION: Data and operations for BSP file
 *
 * CREATED BY: Duyuxin, 2001/8/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_BSPFILE_H_
#define _BSPFILE_H_

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	First 8 bytes in BSP file
#define	BSP_IDENTIFY		(('A'<<24) | ('B'<<16) | ('S'<<8) | 'P')
#define BSP_VERSION			1	

//	Size of PVS string's header
#define SIZE_PVSHEADER		8

//	Number of lumps in BSP file
#define NUM_BSPLUMP			5

//	Lumps in BSP file
#define BFLUMP_LEAVES		0		//	Leaves
#define BFLUMP_NODES		1		//	Nodes
#define BFLUMP_PLANES		2		//	Planes
#define BFLUMP_LEAFSURFS	3		//	Leaf surface references
#define BFLUMP_VISIBILITY	4		//	PVS

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

typedef struct _BFLUMP
{
	LONG		lOff;			//	Offset from file header to lump's data
	LONG		lSize;			//	Size of this lump

} BFLUMP, *PBFLUMP;

typedef struct _BSPFILEHEADER
{
	DWORD		dwIdentify;		//	Identity, must be BSP_IDENTITY
	DWORD		dwVersion;		//	Version, must be BSP_VERSION

	BFLUMP		aLumps[NUM_BSPLUMP];	//	Lump's information

} BSPFILEHEADER, *PBSPFILEHEADER;

typedef struct _BFNODE
{
	LONG		lPlane;			//	Plane
	LONG		Children[2];	//	Negative numbers are -(leafs+1), not nodes
	FLOAT		vMins[3];		//	Bound box
	FLOAT		vMaxs[3];

} BFNODE, *PBFNODE;

typedef struct _BFLEAF
{
	LONG		lCluster;
	LONG		lArea;

	FLOAT		vMins[3];			//	Bound box
	FLOAT		vMaxs[3];

	LONG		lFirstLeafSurf;		//	First surface reference
	LONG		lNumLeafSurfs;		//	Number of surface reference

} BFLEAF, *PBFLEAF;

typedef struct _BFPLANE
{
	FLOAT		vNormal[3];
	FLOAT		fDist;

} BFPLANE, *PBFPLANE;

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_BSPFILE_H_

