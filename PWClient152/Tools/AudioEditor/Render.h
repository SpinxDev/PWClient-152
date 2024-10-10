// Render.h : interface of the CRender class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDER_H__3DC67ED2_2E81_482E_BBF7_B2D2AAE02F3B__INCLUDED_)
#define AFX_RENDER_H__3DC67ED2_2E81_482E_BBF7_B2D2AAE02F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DFont;
class A3DGfxEngine;
class A3DGFXExMan;
class A3DRenderTarget;
class AResInterfaceImpl;
class EditorGfxInterface;
class GFXPhysXSceneShell;

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

	//	Get interface
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }

private:	//	Attributes

	A3DEngine*				m_pA3DEngine;		//	A3D engine
	A3DDevice*				m_pA3DDevice;		//	A3D device

	HINSTANCE				m_hInstance;		//	Handle of instance
	HWND					m_hRenderWnd;		//	Render window's handle
	int						m_iRenderWid;		//	Render size
	int						m_iRenderHei;

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
};

extern CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

#endif // AFX_RENDER_H__3DC67ED2_2E81_482E_BBF7_B2D2AAE02F3B__INCLUDED_
