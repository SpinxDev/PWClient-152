/*
 * FILE: A3DStars.h
 *
 * DESCRIPTION: Class that manipulating the stars in A3D Engine
 *
 * CREATED BY: Hedi, 2002/5/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSTARS_H_
#define _A3DSTARS_H_

#include "A3DVertex.h"
#include "AString.h"

class A3DTexture;
class A3DDevice;
class A3DViewport;

class A3DStars
{
private:
	A3DDevice *		m_pA3DDevice;

	// Point sprite's texture;
	AString 		m_strStarTexture;
	A3DTexture *	m_pStarTexture;

	FLOAT			m_vPointSize;
	bool			m_bUsePointSize; // Flag indicate whether point sprite's size can be used

	bool			m_bShow;

	int				m_nNumStars;
	A3DLVERTEX *	m_pVertexBuffer;

	// Parameters that controls the generation of the stars;
	A3DCOLOR		m_color;
	DWORD			m_dwRandSeed;
	DWORD			m_dwBlinkInterval;
	DWORD			m_dwBlinkDistance;
	DWORD			m_dwBlinkAmplitudeMax;
	DWORD			m_dwBlinkAmplitudeMin;

protected:
	bool GenerateStars();

public:
	A3DStars();
	virtual ~A3DStars();

	bool Init(A3DDevice * pA3DDevice, int nNumStars=1000, A3DCOLOR color=0xffffffff, DWORD dwRandSeed=0x12345678, 
		DWORD dwBlinkInterval=0, DWORD dwBlinkDistance=0, DWORD dwBlinkAmplitudeMin=0, DWORD dwBlinkAmplitudeMax=255,
		FLOAT vPointSize=1.0f, char * szStarTexture=NULL);
	bool Release();

	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();

	bool RegenerateStars();
public:
	inline const char* GetTexture() { return m_strStarTexture; }
	bool SetStarTexture(const char* szStarTexture);

	inline FLOAT GetPointSize() { return m_vPointSize; }
	inline void SetPointSize(FLOAT vPointSize) { m_vPointSize = vPointSize; }

	inline void SetNumStars(int nNumStars) { m_nNumStars = nNumStars; }
	inline void SetColor(A3DCOLOR color) { m_color = color; }
	inline void SetRandSeed(DWORD dwRandSeed) { m_dwRandSeed = dwRandSeed; }
	inline void SetBlinkInterval(DWORD dwBlinkInterval) { m_dwBlinkInterval = dwBlinkInterval; }
	inline void SetBlinkDistance(DWORD dwBlinkDistance) { m_dwBlinkDistance = dwBlinkDistance; }
	inline void SetBlinkAmplitudeMax(DWORD dwBlinkAmplitudeMax) { m_dwBlinkAmplitudeMax = dwBlinkAmplitudeMax; }
	inline void SetBlinkAmplitudeMin(DWORD dwBlinkAmplitudeMin) { m_dwBlinkAmplitudeMin = dwBlinkAmplitudeMin; }
	inline bool GetShowFlag() { return m_bShow; }
	inline void SetShowFlag(bool bShow) { m_bShow = bShow; }
};

#endif//_A3DSTARS_H_