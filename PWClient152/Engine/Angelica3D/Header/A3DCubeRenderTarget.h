/*
 * FILE: A3DCubeRenderTarget.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/5/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DCUBERENDERTARGET_H_
#define _A3DCUBERENDERTARGET_H_


#include "A3DPlatform.h"
#include "A3DDevObject.h"
#include "A3DTypes.h"
#include "A3DSurface.h"

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

class A3DDevice;
class A3DCubeTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DCubeRenderTarget
//	
///////////////////////////////////////////////////////////////////////////

class A3DCubeRenderTarget : public A3DDevObject
{
public:		//	Types

	friend class A3DRenderTargetMan;

public:		//	Constructor and Destructor

	A3DCubeRenderTarget();
	virtual ~A3DCubeRenderTarget();

public:		//	Attributes

public:		//	Operations

	//	Initialize object.
	bool Init(A3DDevice* pA3DDevice, int iEdgeSize, A3DFORMAT fmtColor);
	//	Release object
	void Release();

	//	Apply as texture
	bool AppearAsTexture(int iLayer);
	//	Apply view to device
	bool ApplyToDevice(int iFace, int iRTIndex=0);
	//	Withdraw view from device
	void WithdrawFromDevice();

	//	Get edge size
	int GetEdgeSize() const { return m_iEdgeSize; }
	//	Get format
	A3DFORMAT GetFormat() const { return m_fmtColor; }
	//	Get face surfaces
	A3DSurfaceProxy* GetFaceSurface(int n);
	//	Get cube texture
	IDirect3DCubeTexture9* GetCubeTexture() { return m_pCubeTex; }

protected:	//	Attributes

	A3DDevice*				m_pA3DDevice;	//	Device object
	IDirect3DCubeTexture9*	m_pCubeTex;		//	Cube texture
	A3DSurfaceProxy			m_aSurfs[6];	//	Face surfaces
	A3DSurfaceProxy			m_DummySurf;	//	Empty dummy surface

	int			m_iEdgeSize;	//	Cube edge size
	A3DFORMAT	m_fmtColor;		//	Color buffer format
	DWORD		m_dwRentTime;	//	Time when the target is rented, 0 means never rented
	int			m_idAppied;		//	id got when render target applied to device

protected:	//	Operations

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Create buffer plain
	bool CreatePlains();
	//	Release buffer plain
	void ReleasePlains();
	int GetRenderTargetSize();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DCUBERENDERTARGET_H_
