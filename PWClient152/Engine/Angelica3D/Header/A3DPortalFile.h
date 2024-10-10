/*
 * FILE: A3DPortalFile.h
 *
 * DESCRIPTION: Data used for portal file
 *
 *	File format:
 *
 *	Ver < 2:	PTFHEADER2 structure
 *				lump data ...
 *
 *	Ver >= 3:	PTFHEADER3 structure	
 *				number of lump, len = 4 bytes
 *				lump info, len = number of lump * sizeof (PTFLUMP)
 *				lump data ...
 *
 * CREATED BY: Duyuxin, 2003/6/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DPORTALFILE_H_
#define _A3DPORTALFILE_H_

#include "ABaseDef.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	First 8 bytes in portal file
#define	PTFILE_IDENTIFY		(('A'<<24) | ('P'<<16) | ('T'<<8) | 'L')
#define PTFILE_VERSION		3	

//	Name length
#define PTNAME_LENGTH		32

//	Lumps in portal file file
enum
{
	PTLUMP_VERTEX = 0,			//	Portal vertices
	PTLUMP_PLANES,				//	Planes
	PTLUMP_SECTOR,				//	Sector
	PTLUMP_SECTPLANEREF,		//	Sector plane reference
	PTLUMP_SECTPORTALREF,		//	Sector portal reference
	PTLUMP_SECTORPART,			//	Sector part
	PTLUMP_PARTSURFACEREF,		//	Sector part surface reference
	PTLUMP_PORTAL,				//	Portal
	PTLUMP_BSPNODES,			//	BSP Nodes
	NUM_PTLUMP,
};

#define NUM_PTLUMP2		8		//	Number of lump, ver <= 2

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

struct PTFLUMP
{
	DWORD	dwOff;			//	Offset from file header to lump's data
	DWORD	dwSize;			//	Size of this lump
};

/*	File header ver <= 2
struct PTFHEADER
{
	DWORD	dwIdentify;		//	Identity, must be BSP_IDENTITY
	DWORD	dwVersion;		//	Version, must be BSP_VERSION

	PTFLUMP	aLumps[NUM_PTLUMP2];	//	Lump's information
};
*/

//	File header ver > 3
struct PTFHEADER
{
	DWORD	dwIdentify;		//	Identity, must be BSP_IDENTITY
	DWORD	dwVersion;		//	Version, must be BSP_VERSION
	int		iNumLump;		//	Number of lump
	BYTE	aReserved[60];	//	Reserved
};

//	Plane lump
struct PTFPLANE
{
	float	vNormal[3];
	float	fDist;
};

//	Sector lump
struct PTFSECTOR
{
	float	vMins[3];		//	AABB of sector
	float	vMaxs[3];
	int		iFirstPlane;	//	First plane index in PTLUMP_SECTPLANEREF lump
	int		iNumPlane;		//	Number of plane
	int		iFirstPortal;	//	First portal index in PTLUMP_POTRALVERTREF lump
	int		iNumPortal;		//	Number of portal
	int		iFirstPart;		//	First part index in PTLUMP_SECTPARTREF lump
	int		iNumPart;		//	Number of sector part
};

//	Sector part
struct PTFSECTORPART
{
	float	vMins[3];		//	AABB of sector part
	float	vMaxs[3];
	int		iFirstSurf;		//	First surface index in PTLUMP_PARTSURFACEREF lump
	int		iNumSurf;		//	Number of surface belonging to this sector part
};

//	Portal lump
struct PTFPORTAL
{
	char	szName[PTNAME_LENGTH];	//	Name

	int		iPlane;			//	Index of plane on which portal exists
	int		iFront;			//	Sector this portal face to
	int		iBack;			//	Sector this portal back to
	int		iOpposite;		//	Opposite portal's index
	int		iFirstVert;		//	First vertex index in PTLUMP_VERTEX lump
	int		iNumVert;		//	Number of vertex
};

//	BSP nodes (exists when version > 2)
struct PTFBSPNODE
{
	int		iPlane;			//	Plane index. -1 means leaf
	int		iSector;		//	Only valid for leaf, sector belongs to this leaf
	int		iParent;		//	iParent node
	int		aChildren[2];	//	Children nodes
};

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_A3DPORTALFILE_H_

