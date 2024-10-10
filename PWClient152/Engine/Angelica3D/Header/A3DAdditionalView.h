/*
 * FILE: A3DAdditionalView.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/2/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DADDITIONALVIEW_H_
#define _A3DADDITIONALVIEW_H_

#include "A3DPlatform.h"
#include "A3DDevObject.h"
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

class A3DEngine;
class A3DDevice;
class A3DViewport;
class A3DCameraBase;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DAdditionalView
//	
///////////////////////////////////////////////////////////////////////////

class  A3DAdditionalView : public A3DDevObject
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DAdditionalView();
	virtual ~A3DAdditionalView();

public:		//	Attributes

public:		//	Operations

	//	Initiaalize object
	bool Init(A3DEngine* pA3DEngine, HWND hWnd, bool bDepthBuffer);
	//	Release object
	void Release();

	//	Present
	bool Present();

	//	Apply view to device
	bool ApplyToDevice();
	//	Withdraw view from device
	void WithdrawFromDevice();

	//	Set/Get active viewport
	void SetActiveViewport(A3DViewport* pViewport);
	A3DViewport* GetActiveViewport() const { return m_pCurViewport; }
	//	Set/Get active camera
	void SetActiveCamera(A3DCameraBase* pCamera);
	A3DCameraBase* GetActiveCamera() const { return m_pCurCamera; }

	//	Resize view when window's size changed
	bool OnSize();
	//	Get size of view
	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	//	Set/Get horizontal major flag
	void SetHoriMajor(bool bHoriMajor) { m_bHoriMajor = bHoriMajor; }
	bool GetHoriMajor() const { return m_bHoriMajor; }

	//	Get interfaces
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	IDirect3DSwapChain9* GetD3DSwapChain() const { return m_pD3DSwapChain; }
	A3DSurfaceProxy* GetBackBuffer() { return &m_BackBuf; }
	A3DSurfaceProxy* GetDepthBuffer() { return &m_DepthBuf; }

protected:	//	Attributes

	A3DEngine*				m_pA3DEngine;		//	Engine object
	A3DDevice*				m_pA3DDevice;		//	Device object
	A3DViewport*			m_pCurViewport;		//	Active viewport
	A3DCameraBase*			m_pCurCamera;		//	Active camera

	IDirect3DSwapChain9*	m_pD3DSwapChain;	//	D3D swap chain interface
	A3DSurfaceProxy			m_BackBuf;			//	Back buffer surface
	A3DSurfaceProxy			m_DepthBuf;			//	Depth-stencil buffer surface

	HWND	m_hWnd;			//	Handle of window
	int		m_iWidth;		//	Width of view
	int		m_iHeight;		//	Height of view
	bool	m_bDepthBuf;	//	true, create depth-stencil buffer
	int		m_idAppied;		//	id got when this view is appied to device
	bool	m_bHoriMajor;	//	true, horizontal major when adjust a orthogonal camera
	bool	m_bNeedReApply;	//	when device reset, do we need reapply in the after device reset callback

protected:	//	Operations

	//	Is applied on device
	bool IsAppliedToDevice() const { return m_idAppied != -1; }
	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Create swap chain
	bool CreateSwapChain(HWND hWnd, bool bDepthBuffer);
	//	Release swap chain
	void ReleaseSwapChain();

	//	Update viewport when view size changed
	void UpdateViewportWhenResize();
	//	Update camera when view size changed
	void UpdateCameraWhenResize();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DADDITIONALVIEW_H_
