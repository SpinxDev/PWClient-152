/*
 * FILE: A3DViewport.h
 *
 * DESCRIPTION: Class standing for a viewport
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DVIEWPORT_H_
#define _A3DVIEWPORT_H_

#include "A3DTypes.h"
#include "A3DObject.h"

class A3DDevice;
class A3DCameraBase;

class A3DViewport : public A3DObject
{
private:
	bool				m_bClearColor;
	bool				m_bClearZ;
	A3DCOLOR			m_colorClear;
	A3DVIEWPORTPARAM	m_ViewportParam;
	A3DDevice*			m_pA3DDevice;
	A3DCameraBase*		m_pA3DCamera;

	A3DMATRIX4			m_matViewScale;
	A3DMATRIX4			m_matInvViewScale;
	
protected:
public:
	A3DViewport();
	~A3DViewport();

	bool Init(A3DDevice * pA3DDevice, A3DVIEWPORTPARAM * pParam, bool bClearColor, bool bClearZ, A3DCOLOR colorClear);
	bool Release();

	// Set Viewport's parameters;
	bool SetParam(A3DVIEWPORTPARAM * pParam);

	// Transform a world space point to screen coordinates, vecOut is a screen point;
	bool Transform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);
	// Transform a screen point to world space coordinates, vecIn is a screen point;
	bool InvTransform(const A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);

	bool SetCamera(A3DCameraBase* pA3DCamera);
	
	bool Active(); //This call will rebuild the transform matrix, so careful to use.

	bool ClearDevice();

	inline A3DVIEWPORTPARAM * GetParam() { return &m_ViewportParam; }
	inline A3DCameraBase* GetCamera() { return m_pA3DCamera; }
	inline void SetClearColor(A3DCOLOR color) { m_colorClear = color; }
	inline A3DCOLOR GetClearColor() { return m_colorClear; }
	inline const A3DMATRIX4 GetViewScale() { return m_matViewScale; }
	inline A3DDevice* GetA3DDevice() { return m_pA3DDevice; }
};

typedef A3DViewport * PA3DViewport;
#endif