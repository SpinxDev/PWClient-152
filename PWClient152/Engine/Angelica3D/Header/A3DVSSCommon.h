/*
 * FILE: A3DVSSCommon.h
 *
 * DESCRIPTION: Internal header files.
 *
 * CREATED BY: duyuxin, 2002/7/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVSSCOMMON_H_
#define _A3DVSSCOMMON_H_

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Define this to use compressed vss
#define COMPRESSED_VSS

//	File's version and flag
#define	VSSFILE_VERSION		4
#define VSSFILE_IDENTIFY	(('V'<<24) | ('S'<<16) | ('S'<<8) | 'F')

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

typedef struct _VSSFILEHEADER
{
	DWORD	dwVersion;		//	File version
	DWORD	dwIdentify;		//	File identity
	DWORD	dwRouteID;		//	Route ID
	int		iNumSection;	//	Number of section
	int		iVisBufLen;		//	Visible buffer length
	int		iNumObject;		//	Number of scene object, Ver >= 3
	int		iNumFace;		//	Number of scene face, Ver >= 4

} VSSFILEHEADER, *PVSSFILEHEADER;

//	Parameters used to process visible surface set
typedef struct _VSSETPARAMS
{
	int		iMaxCount;		//	Maximum sampling count
	int		iMaxSurf;		//	Maximum number of surfaces can be rendered together
	int		iMinSecSample;	//	Minimum number of sampling in a section
	int		iMaxSecSample;	//	Maximum number of sampling in a section

} VSSETPARAMS, *PVSSETPARAMS;

//	Section information in file
typedef struct _VSSSECTIONINFO
{
	int		iStartCnt;		//	Start sampling count of this section
	int		iEndCnt;		//	End sampling count of this section
	int		iNumVisible;	//	Number of visible surface
	DWORD	dwOff;			//	Visible Set's offset in file
	int		iVisLen;		//	Length (in bytes) of compressed visible set
	int		iNumVisObj;		//	Number of visible scene object, Ver >= 3

} VSSSECTIONINFO, *PVSSSECTIONINFO;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVSSCOMMON_H_

