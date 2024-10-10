/*
 * FILE: A3DLightning.h
 *
 * DESCRIPTION: A electric strike from sky to ground 
 *
 * CREATED BY: Hedi, 2002/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2002 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLIGHTNING_H_
#define _A3DLIGHTNING_H_

#include "A3DTypes.h"
#include "A3DFlash.h"
#include "APerlinNoise1D.h"

class A3DDevice;
class A3DFlash;

class A3DLightning
{
private:
	A3DDevice			* m_pA3DDevice;
	bool				m_bStrike;			// whether it has release its power
	int					m_nTicks;			// ticks of this object

	APerlinNoise1D		m_noiseForFlash;	// noise for flash to use
	APerlinNoise1D		m_noiseForMe;		// noise for lightning stroke path

	A3DMATRIX4			m_matOrient;		// matrix representing the orientation
	A3DVECTOR3			m_vecStart;			// start point of this lightning
	A3DVECTOR3			m_vecDir;			// direction of start point of this lightning
	A3DVECTOR3			m_vecDownward;		// shift vector that each step will follow, such as vertical downward vector
	FLOAT				m_vStrikeSize;		// size of each strike
	FLOAT				m_vStepSize;		// size of each step
	int					m_nMaxLevel;		// max level that brachies;
	A3DCOLOR			m_color;			// color of each strike

	A3DCOLOR			m_colorAmbient;		// color of ambient, after strike we must restore it

	int					m_nMainPathStep;	// step number of main path;

	int					m_nNumCount;		// variables use when build

	int					m_nNumFlashes;		// number of flashes that construct this lightning
	int					m_nNumFlashesToRen;	// number of flashes that should be rendered now
	A3DFlash			** m_ppFlashes;		// array of flash that consist of this lightning
	A3DFlash			m_flashBackground;	// flash use to highlight the background

protected:
	bool AddPath(const A3DMATRIX4& matOrient, const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDir, int nNoisePos, int nLevel, int& nCount);
	bool Build();

public:
	A3DLightning();
	virtual ~A3DLightning();

	// vStrikeSize is the width of each sub path, vStepSize is the length each step moves
	// and nSteps is the total steps moved, nMaxLevel is the maximum forking level of main path
	bool Init(A3DDevice * pDevice, FLOAT vStrikeSize, FLOAT vStepSize, int nSteps, int nMaxLevel=3, A3DCOLOR color=0xfffffff);
	bool Release();

	// vecDownward is not used currently
	bool ReCharge(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecDownward);
	bool Strike();		// Begin strike;

	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();

	inline void SetColor(A3DCOLOR color)	{ m_color = color; }
	inline A3DCOLOR GetColor()				{ return m_color; }

	inline void SetStrikeSize(FLOAT vSize)	{ m_vStrikeSize = vSize; }
	inline FLOAT GetStrikeSize()			{ return m_vStrikeSize; }

	inline void SetStepSize(FLOAT vSize)	{ m_vStepSize = vSize; }
	inline FLOAT GetStepSize()				{ return m_vStepSize; }
																	  
	inline void SetAmbient(A3DCOLOR color)	{ m_colorAmbient = color; }
};	

#endif
