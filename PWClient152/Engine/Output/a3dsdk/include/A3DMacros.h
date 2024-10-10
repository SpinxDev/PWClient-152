/*
 * FILE: A3DMacros.h
 *
 * DESCRIPTION: Fundermental macros for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMACROS_H_
#define _A3DMACROS_H_

#include "A3DTypes.h"

#define A3D_PI		3.1415926535f
#define A3D_2PI		6.2831853072f

//Warning: you must supply byte values as r, g and b or the result may be undetermined
#define A3DCOLORRGB(r, g, b) ((A3DCOLOR) (0xff000000 | ((r) << 16) | ((g) << 8) | (b)))					
//Warning: you must supply byte values as r, g, b and a, or the result may be undetermined
#define A3DCOLORRGBA(r, g, b, a) ((A3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

#define A3DCOLOR_GETRED(color) ((BYTE)(((color) & 0x00ff0000) >> 16))
#define A3DCOLOR_GETGREEN(color) ((BYTE)(((color) & 0x0000ff00) >> 8))
#define A3DCOLOR_GETBLUE(color) ((BYTE)(((color) & 0x000000ff)))
#define A3DCOLOR_GETALPHA(color) ((BYTE)(((color) & 0xff000000) >> 24))

#define DEG2RAD(deg) ((deg) * A3D_PI / 180.0f)
#define RAD2DEG(rad) ((rad) * 180.0f / A3D_PI)

#define FLOATISZERO(x)	((x) > -1e-7f && (x) < 1e-7f)

//	Release A3D objects
#define A3DRELEASE(p) \
{ \
	if (p) \
	{ \
		(p)->Release(); \
		delete (p); \
		(p) = NULL; \
	} \
}

//	Release A3D interfaces
#define A3DINTERFACERELEASE(p) \
{ \
	if (p) \
	{ \
	(p)->Release(); \
	(p) = NULL; \
	} \
}

#define MEMBER_OFFSET(_CLS, _MEMBER)   ((size_t)(&(((_CLS*)0)->_MEMBER)))

#endif

