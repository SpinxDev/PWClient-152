/*
 * FILE: A3DLensFlare.h
 *
 * DESCRIPTION: Routines for Lens Flare effect
 *
 * CREATED BY: duyuxin, 2001/11/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLENSFLARE_H_
#define _A3DLENSFLARE_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDecal;
class A3DViewport;
class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DLensFlare
//
///////////////////////////////////////////////////////////////////////////

class A3DLensFlare
{
public:		//	Types

	typedef struct _FLARE
	{
		float		fSize;			//	Relative size of flare
		float		fPos;			//	Position relative to center point
		BYTE		r, g, b;		//	Color

		A3DDecal*	pDecal;			//	Decal
	
	} FLARE, *PFLARE;

public:		//	Constructors and Destructors

	A3DLensFlare();
	virtual ~A3DLensFlare();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, char* szBaseName);	//	Initialize object
	void		Release();		//	Release object

	void		SetFlareCol(int iIndex, A3DCOLOR Color);	//	Set flare's color
	void		Update(A3DViewport* pView);					//	Update flares with current viewport
	void		Update(A3DViewport* pView, A3DVECTOR3& vLightPos, FLOAT fCosAngle, 
					   FLOAT fCosInter, FLOAT fCosOuter, bool bObstruct);

	void		SetFlare(int iIndex, FLOAT fSize, FLOAT fPos, A3DCOLOR Color);	//	Set flare's property

	void		SetLightPos(A3DVECTOR3 vPos)	{	m_vLightPos = vPos;		}	//	Set position for light source, the position is relative to camera
//	void		OpenBurnout(bool bBurnout)		{	m_bBurnout = bBurnout;	}

	bool		Render(A3DViewport* pView);		//	Render flares

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;	//	A3D device object
	int			m_iNumFlares;	//	Number of flares
	PFLARE		m_aFlares;		//	Flares

	A3DVECTOR3	m_vLightPos;	//	Position for light source
	A3DVECTOR3	m_vRayDir;		//	Normalized direction from center point to light

	bool		m_bUpdateOK;	//	Update ok flag
	bool		m_bBurnout;		//	true, do burnout effect
	bool		m_bDrawFlares;	//	true, draw flares
	bool		m_bDrawBurnout;	//	true, draw burnout

	float		m_fRealAlpha;	//	Real alpha

protected:	//	Operations

	bool		CreateDecals(A3DDevice* pDevice, char* szBaseName);		//	Create decals
};



#endif	//	_A3DLENSFLARE_H_
