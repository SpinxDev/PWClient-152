/*
 * FILE: A3DFlash.h
 *
 * DESCRIPTION: A electric flash between two point, 
				from start point rushing to end point and have enough detail random
				when it running
 *
 * CREATED BY: Hedi, 2002/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2002 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFLASH_H_
#define _A3DFLASH_H_

#include "A3DTypes.h"
#include "A3DVertex.h"

class A3DTexture;
class A3DDevice;
class A3DViewport;
class APerlinNoise1D;

#define A3DFLASH_MAX_SEGEMENT		256
class A3DFlash
{
private:
	A3DDevice			* m_pA3DDevice;
	APerlinNoise1D		* m_pPerlinNoise;

	A3DCOLOR			m_color;			// color of this flash
	A3DVECTOR3			m_vecStart;			// start point of this flash
	A3DVECTOR3			m_vecEnd;			// end point of this flash
	float				m_vSize;			// width of the flash segment

	A3DVECTOR3			* m_pVecStrikePath;	// all vert position of each segment;
	A3DTexture			* m_pTexture;		// texture used by this flash
	int					m_nNumSegments;		// how many segment this flash is diveded into
	A3DLVERTEX			* m_pVertexBuffer;  // vertex buffer used for rendering
	WORD				* m_pIndexBuffer;   // index buffer used for rendering

protected:
	bool BuildStrikePath();	// generate the random walk strike path

public:
	A3DFlash();
	~A3DFlash();

	bool Init(A3DDevice * pDevice, APerlinNoise1D * pNoise, float vSize, int nSteps, A3DCOLOR color=0xffffffff);
	bool Release();

	bool RebuildStrikePath(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecEnd);

	bool Render(A3DViewport * pCurrentViewport);	// function that render this flash
	bool TickAnimation();							// function that make this flash animated

	bool SetTextureMap(char * szTextureMap);		// set the texture of this flash
	inline char * GetTextureMap();					// get the name of texture map

	inline void SetColor(A3DCOLOR color)	{ m_color = color; }
	inline A3DCOLOR GetColor()				{ return m_color; }

	inline void SetSize(FLOAT vSize)		{ m_vSize = vSize; }
	inline FLOAT GetSize()					{ return m_vSize; }
};	

#endif
