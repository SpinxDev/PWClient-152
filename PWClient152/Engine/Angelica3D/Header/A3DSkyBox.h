/*
 * FILE: A3DSkyBox.h
 *
 * DESCRIPTION: Class that standing for the sky box in A3D Engine
 *
 * CREATED BY: Hedi, 2002/5/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKYBOX_H_
#define _A3DSKYBOX_H_

#include "A3DSky.h"
#include "A3DTextureMan.h"

class A3DTexture;
class A3DStream;

class A3DSkyBox : public A3DSky
{
private:
	// Six maps that generated with a 90 degree camera;
	A3DTexture *	m_pMapTexture[6];
	A3DStream *		m_pSkyFace; // One rectangle face for sky box rendering

protected:
public:
	A3DSkyBox();
	virtual ~A3DSkyBox();

	bool Init(A3DDevice* pA3DDevice, A3DCameraBase* pCamera, const char* szRight, const char* szLeft, 
			  const char* szTop, const char* szBottom, const char* szFront, const char* szBack);
	bool Release();

	bool Render();
	bool TickAnimation();

	bool SetMapTexture(int index, char * szMapTexture);
};

typedef A3DSkyBox * PA3DSkyBox;
#endif//_A3DSKYBOX_H_
