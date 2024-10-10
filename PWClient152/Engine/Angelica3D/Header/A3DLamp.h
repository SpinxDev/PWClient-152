/*
 * FILE: A3DLamp.h
 *
 * DESCRIPTION: Routines for light effect
 *
 * CREATED BY: duyuxin, 2001/11/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DLAMP_H_
#define _A3DLAMP_H_

#include "A3DTypes.h"
#include "A3DMacros.h"
#include "A3DDecal.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Lamp flags
#define LAMPFLAG_FOLLOWVIEW		0x0001	//	Lamp is following view point
#define	LAMPFLAG_VIEWCONE		0x0002	//	Conside view cone factor
#define	LAMPFLAG_DISTANCE		0x0004	//	Conside distance factor
#define	LAMPFLAG_SPOTLIGHT		0x0008	//	This is a spotlight
#define	LAMPFLAG_BURNOUT		0x0100	//	Burnout effect
#define LAMPFLAG_LENSFLARE		0x0200	//	Lens flare

//	Some combination of special type lamp
#define LAMPSTYLE_UNREAL		(LAMPFLAG_DISTANCE | LAMPFLAG_VIEWCONE)
#define LAMPSTYLE_SUN			(LAMPFLAG_FOLLOWVIEW | LAMPFLAG_VIEWCONE | LAMPFLAG_BURNOUT | LAMPFLAG_LENSFLARE)

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DLensFlare;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DLamp
//
///////////////////////////////////////////////////////////////////////////

class A3DLamp
{
public:		//	Types

	enum
	{
		UPDATE_INTERVAL = 5,	//	Update tick interval
	};

public:		//	Constructors and Destructors

	A3DLamp();
	virtual ~A3DLamp();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, char* szShineTex, DWORD dwLampFlags);	//	Initialize object
	void		Release();		//	Release object

	bool		Render(A3DViewport* pView);		//	Render lamp

	inline void	SetLampParams(FLOAT fWidth, FLOAT fHeight, A3DVECTOR3& vPos, A3DVECTOR3& vDir, A3DCOLOR Color);		//	Set lamp parameters
	inline void	SetViewConeParams(FLOAT fTheta, FLOAT fPhi);	//	Set view cone parameters
	inline void	SetSpotlightParams(FLOAT fTheta, FLOAT fPhi);	//	Set spotlight parameters
	inline void	SetDiatanceParams(FLOAT fNear, FLOAT fFar);		//	Set distance parameters
	void		SetLensFlareParams(char* szBaseName);			//	Set lens flare parameters

	A3DLensFlare*	GetLensFlare()		{	return m_pLensFlare;	}

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;	//	A3D device object
	A3DDecal		m_ShineDecal;	//	Shine decal
	A3DDecal		m_BurnoutDecal;	//	Burnout decal
	A3DLensFlare*	m_pLensFlare;	//	Lens flare
	
	bool			m_bObstruct;	//	Flag indicate whether this lamp is obstructed by scene

	DWORD		m_dwFlags;			//	Flags
	DWORD		m_dwTickOff;		//	Tick offset

	float		m_fWidth;			//	Half size of lamp
	float		m_fHeight;
	A3DVECTOR3	m_vLampPos;			//	Position of lamp
	A3DVECTOR3	m_vLampDir;			//	Direction of lamp
	A3DCOLOR	m_Color;			//	Color of lamp

	float		m_fViewTheta;		//	Inter cone of view cone 
	float		m_fViewPhi;			//	Outer cone of view cone
	float		m_fCosViewTheta;	//	Cosine of m_fViewTheta
	float		m_fCosViewPhi;		//	Cosine of m_fViewPhi

	float		m_fSpotTheta;		//	Inter cone of spotlight
	float		m_fSpotPhi;			//	Outer cone of spotlight
	float		m_fCosSpotTheta;	//	Cosine of m_fSpotTheta
	float		m_fCosSpotPhi;		//	Cosine of m_fSpotPhi

	float		m_fNearDist;		//	The nearest distance
	float		m_fFarDist;			//	The farthest distance

	bool		m_bDrawLamp;		//	Flag used to decide whether draw lamp
	bool		m_bDrawBurnout;		//	Flag used to decide whether draw burnout
	float		m_fCurAlpha;		//	Current alpha should be use
	float		m_fCurSize;			//	Current size should be use

protected:	//	Operations

	void		Update(A3DViewport* pView);		//	Update lamp's state	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Set spotlight parameters.

	fTheta: angle degree of inter cone of spotlight.
	fPhi: angle degree fo outer cone of spotlight.
*/
void A3DLamp::SetSpotlightParams(FLOAT fTheta, FLOAT fPhi)
{	
	m_fSpotTheta	= fTheta;	
	m_fSpotPhi		= fPhi;
	m_fCosSpotTheta	= (float)cos(DEG2RAD(fTheta * 0.5f));
	m_fCosSpotPhi	= (float)cos(DEG2RAD(fPhi * 0.5f));
}

/*	Set view cone parameters.

	fTheta: angle degree of inter view cone
	fPhi: angle degree fo outer view cone.
*/
void A3DLamp::SetViewConeParams(FLOAT fTheta, FLOAT fPhi)
{	
	m_fViewTheta	= fTheta;
	m_fViewPhi		= fPhi;
	m_fCosViewTheta	= (float)cos(DEG2RAD(fTheta * 0.5f));
	m_fCosViewPhi	= (float)cos(DEG2RAD(fPhi * 0.5f));
}

/*	Set lamp parameters.

	fWidth, fHeight: half size of lamp in screen pixels
	vPos: relative or absolute position of lamp.
	vDir: direction of lamp, this parameter is only useful for spotlight
	Color: lamp's color.
*/
void A3DLamp::SetLampParams(FLOAT fWidth, FLOAT fHeight, A3DVECTOR3& vPos, 
							A3DVECTOR3& vDir, A3DCOLOR Color)
{
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;
	m_vLampPos	= vPos;
	m_vLampDir	= vDir;
	m_Color		= Color;
}

/*	Set distance parameters.

	fNear: the distance lamp shine will disappear when view point move to light.
	fFar: the distance lamp shine will disappear when view point leave light
*/
void A3DLamp::SetDiatanceParams(FLOAT fNear, FLOAT fFar)
{
	m_fNearDist	= fNear;
	m_fFarDist	= fFar;
}


#endif	//	_A3DLAMP_H_


