/*
 * FILE: A3DPI.cpp
 *
 * DESCRIPTION: private interface functions or variables for Angelica 3D engine
 *
 * CREATED BY: duyuxin, 2003/10/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DFuncs.h"
#include "AAssist.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

ALog	g_A3DErrLog;

namespace A3D
{
	const A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
	const A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
	const A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);
	const A3DVECTOR3	g_vOrigin(0.0f);
	const A3DMATRIX4	g_matIdentity(A3DMATRIX4::IDENTITY);
}

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement some class declared in A3DTypes.h
//
///////////////////////////////////////////////////////////////////////////

A3DCOLORVALUE::A3DCOLORVALUE(A3DCOLOR Color)
{
	static float fTemp = 1.0f / 255.0f;
	a = A3DCOLOR_GETALPHA(Color) * fTemp;
	r = A3DCOLOR_GETRED(Color) * fTemp;
	g = A3DCOLOR_GETGREEN(Color) * fTemp;
	b = A3DCOLOR_GETBLUE(Color) * fTemp;
}

const A3DCOLORVALUE& A3DCOLORVALUE::operator = (A3DCOLOR Color)
{
	static float fTemp = 1.0f / 255.0f;
	a = A3DCOLOR_GETALPHA(Color) * fTemp;
	r = A3DCOLOR_GETRED(Color) * fTemp;
	g = A3DCOLOR_GETGREEN(Color) * fTemp;
	b = A3DCOLOR_GETBLUE(Color) * fTemp;
	return *this;
}

//	Convert color value to A3DCOLOR
A3DCOLOR A3DCOLORVALUE::ToRGBAColor()
{
	int _a = (int)(a * 255);
	int _r = (int)(r * 255);
	int _g = (int)(g * 255);
	int _b = (int)(b * 255);
	return A3DCOLORRGBA(min2(_r, 255), min2(_g, 255), min2(_b, 255), min2(_a, 255));
}



