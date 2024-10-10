/*
 * FILE: A3DMirrorRender.h
 *
 * DESCRIPTION: Object Based Particle System for Angelica 3D Engine
 *
 * CREATED BY: liyi 2012, 5, 24
 *
 * HISTORY:
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.	
 */


#ifndef _A3D_MIRROR_RENDER_H_
#define _A3D_MIRROR_RENDER_H_

#include "A3DRenderTarget.h"

class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DrenderTarget;
class A3DPLANE;

typedef void (*LPRENDERFORMIRROR)(A3DViewport * pViewport, void* pArg);

class A3DMirrorRender
{
public:
	A3DMirrorRender();
	~A3DMirrorRender();

	bool Init(A3DDevice* pA3DDevice);
	void Release();

	A3DRenderTarget* GetMirrorRT() { return m_pMirrorRT;}
	bool Render(A3DViewport* pCurViewport, A3DPLANE* pMirrorPlane, LPRENDERFORMIRROR pFuncReflectCallBack = NULL, void* pArg = NULL);

private:
	A3DDevice*			m_pA3DDevice;

	A3DCamera*			m_pMirrorCamera;
	A3DViewport*		m_pViewport;

	A3DRenderTarget*	m_pMirrorRT;

	bool				m_bInit;
};

#endif //_A3D_MIRROR_RENDER_H_