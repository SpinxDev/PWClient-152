/*
 * FILE: A3DVSDef.h
 *
 * DESCRIPTION: Vertex shader definition for Angelica 3D engine
 *
 * CREATED BY: duyuxin, 2003/11/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DVSDEF_H_
#define _A3DVSDEF_H_

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Maximum number of hardware morph channel that affect a vertex. This number
//	is limited by number of vertex shader's vertex registers (16) and MAX_FVF_DECL_SIZE
//	(20 in DX8.x), so the maximum value of this number is 4 for hardware rendering.
//	See detail in a3d_MakeMorphVSDeclarator()
#define MAXNUM_HWMORPHCHANNEL	4
#define MAXNUM_SWMORPHCHANNEL	32

//	Vertex shader constant index used by skin model
enum
{
	SMVSC_CONSTANT		= 0,	//	{1.0f, power, 0.0f, 765.01f}
	SMVSC_LIGHTDIR		= 1,	//	Dir_Light's direction in camera space
	SMVSC_PROJECT		= 2,	//	Project matrix (2--5)
	SMVSC_EYEDIR		= 6,	//	Eye direction in world space (not used now)
	SMVSC_AMBIENT		= 7,	//	Env ambient + Mateiral ambient * dir_light ambient
	SMVSC_DIRDIFFUSE	= 8,	//	Material diffuse * dir_light diffuse
	SMVSC_DIRSPECULAR	= 9,	//	Material specular * dir_light specular
	SMVSC_PTLIGHTPOS	= 10,	//	Point_light's position in world space
	SMVSC_PTDIFFUSE		= 11,	//	Material diffuse * point_light diffuse
	SMVSC_PTAMBIENT		= 12,	//	Material ambient * point_light ambient
	SMVSC_PTATTEN		= 13,	//	Point_light's attenuation
	SMVSC_BLENDMAT0		= 10,	//	First blend matrix
	SMVSC_BLENDMAT0_VS2	= 10,	//	First blend matrix
};

//	A3DTerrain2's transform vertex shader
enum
{
	T2VSC_CONSTANT		= 0,	//	{1.0f, power, 0.0f, 765.01f}
	T2VSC_LIGHTDIR		= 1,	//	Dir_Light's direction in world space
	T2VSC_VIEWPROJECT	= 2,	//	View matrix * Project matrix (2--5)
	T2VSC_EYEDIR		= 6,	//	Eye direction in world space
	T2VSC_DIRSPECULAR	= 7,	//	Material specular * dir_light specular
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVSDEF_H_