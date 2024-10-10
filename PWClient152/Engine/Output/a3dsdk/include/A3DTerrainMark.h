/*
 * FILE: A3DTerrainMark.h
 *
 * DESCRIPTION: Terrain mark is a square patch that lies on the terrain
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTERRAINMARK_H_
#define _A3DTERRAINMARK_H_

#include "A3DTypes.h"

class A3DDevice;
class A3DStream;
class A3DTexture;
class A3DTerrain;
class A3DViewport;

class A3DTerrainMark
{
private:
	A3DDevice *			m_pA3DDevice;
	A3DTerrain *		m_pA3DTerrain;
	A3DTexture *		m_pA3DTexture;
	A3DStream *			m_pA3DStream;

	A3DMATRIX4			m_matMyWorldTM;

public:
	A3DTerrainMark();
	~A3DTerrainMark();

	bool Init(A3DDevice * pA3DDevice, char * szTexture);
	bool Release();

	bool Render(A3DViewport * pCurrentViewport);
	bool SetAllParameters(int x, int y, A3DCOLOR color, FLOAT vSize);
};

typedef A3DTerrainMark * PA3DTerrainMark;

#endif