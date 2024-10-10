/*
 * FILE: A3DLightingEngine.h
 *
 * DESCRIPTION: Soft lighting routines
 *
 * CREATED BY: duyuxin, 2001/12/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DLIGHTINGENGINE_H_
#define _A3DLIGHTINGENGINE_H_

#include "A3DVertex.h"
#include "AList.h"

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

class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DLightingEngine
//
///////////////////////////////////////////////////////////////////////////

class A3DLightingEngine
{
public:		//	Types

	enum
	{
		MAXNUM_LIGHT	= 32		//	Maximum number of lights
	};

	//	Temporary value when do light calculate
	typedef struct _LIGHTVALUE
	{
		A3DCOLORVALUE	Diffuse;
		A3DCOLORVALUE	Ambient;
		A3DCOLORVALUE	Specular;
	
	} LIGHTVALUE, *PLIGHTVALUE;

public:		//	Constructors and Destructors

	A3DLightingEngine();
	virtual ~A3DLightingEngine();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DEngine*	pA3DEngine);	//	Initialize object
	void		Release();		//	Release resources

	bool		BuildFogTable(A3DFOGMODE FogMode, float fNear, float fFar, float fDensity,
							float fViewField, float fStep);	//	Build fog table

	void		UpdateStaticLights();				//	Update static lights
	void		SetDynamicLights(AList* pList);		//	Set dynamic lights
	void		AddDynamicLights(AList* pList);		//	Add dynamic lights
	bool		Illuminate(A3DVERTEX* aSrcVerts, int iNumVert, A3DMATRIX4& mat, 
						A3DMATERIALPARAM* pMaterial, A3DLVERTEX* aDestVerts);	//	Illuminate vertices

	void		SetCameraPos(A3DVECTOR3& vPos)	{	m_vEyePos = vPos;	}

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;				//	A3D engine object

	A3DLIGHTPARAM	m_aLights[MAXNUM_LIGHT];	//	Light array
	LIGHTVALUE		m_aLtValues[MAXNUM_LIGHT];	//	Temporary values
	int				m_iNumStaticLight;			//	Number of static light
	int				m_iNumDynamicLight;			//	Number of dynamic light

	A3DVECTOR3		m_vEyePos;		//	Camera position

	//	Fog table
	BYTE*			m_aFogTab;		//	Fog table
	float			m_fViewField;	//	View field
	float			m_fNear;		//	Near plane
	float			m_fFar;			//	Far plane
	A3DFOGMODE		m_FogMode;		//	Fog mode
	float			m_fStepLen;		//	Step length
	float			m_fInvStepLen;	//	Inverse step length
	float			m_fDensity;		//	Fog's density
	int				m_iNumStep;		//	Number of step

protected:	//	Operations

	inline BYTE		CalculateFogFactor(A3DVECTOR3& vPos);	//	Calculate fog factor for vertex
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Calculate fog factor for vertex.

	Return fog factor clamped to 0 -- 255.

	vPos: vertex's position in world space.
*/	
BYTE A3DLightingEngine::CalculateFogFactor(A3DVECTOR3& vPos)
{
	float dx = vPos.x - m_vEyePos.x;
	float dz = vPos.z - m_vEyePos.z;

	if (dx < 0.0f)	dx = -dx;
	if (dz < 0.0f)	dz = -dz;

	int x = (int)(dx * m_fInvStepLen);
	int z = (int)(dx * m_fInvStepLen);

	if (x >= m_iNumStep || z >= m_iNumStep)
		return 0;

	int iBase = x * (int)m_fInvStepLen;

	dx = dx - (float)iBase;
	dz = dx - z * m_fInvStepLen;

	float f1, f2, f3, f4 = 0.0f;
	bool bFlag = false;

	f1 = (float)m_aFogTab[iBase + z];

	if (x == m_iNumStep-1)
	{
		f2 = 0.0f;
		bFlag = true;
	}
	else
		f2 = (float)m_aFogTab[iBase + m_iNumStep + z];

	if (x == m_iNumStep-1)
	{
		f3 = 0.0f;
		bFlag = true;
	}
	else
		f3 = (float)m_aFogTab[iBase + z + 1];

	if (!bFlag)
		f4 = (float)m_aFogTab[iBase + m_iNumStep + z + 1];

	float fFactor = dx*dz*f1 + (1.0f-dx)*f2 + (1.0f-dz)*f3 + (1.0f-dx)*(1.0f-dz)*f4;

	return (BYTE)(fFactor * 0.25f);
}


#endif	//	_A3DLIGHTINGENGINE_H_

