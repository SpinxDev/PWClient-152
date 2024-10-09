/*
 * FILE: Render.h
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/6/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#pragma once

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
class A3DLight;	
class A3DFont;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRender
//
///////////////////////////////////////////////////////////////////////////

class CRender
{
public:		//	Types

public:		//	Constructor and Destructor

	CRender();
	virtual ~CRender() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();

	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender();
	//	End render
	bool EndRender();

	//	Output text
	void TextOut(int x, int y, const ACHAR* szText, DWORD color);
	//	Draw 2D rectangle
	void Draw2DRect(const RECT& rc, DWORD dwCol, bool bFrame);

	//	Set ambient color and diretional light's direction and color
	void ChangeLight(const A3DVECTOR3& vDir, DWORD dwDirCol, DWORD dwAmbient);
	
	//	Get interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() {	return m_pA3DDevice; }
	A3DLight* GetDirectionalLight() { return m_pDirLight; }
	A3DFont* GetSystemFont() { return m_pFont; }

	//	Engine is ready ?
	bool EngineIsReady() { return m_bEngineOK; }

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DLight*		m_pDirLight;		//	Directional light
	A3DFont*		m_pFont;			//	Font object

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;

	bool			m_bEngineOK;		//	Engine is ready

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
	//	Check device
	bool CheckDevice();
};

extern CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

