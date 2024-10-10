// Render.cpp : implementation of the CRender class
//
#include "StdAfx.h"
#include "Render.h"
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DMacros.h>
#include <windows.h>

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CRender
//
///////////////////////////////////////////////////////////////////////////

CRender::CRender()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_iRenderWid	= 800;
	m_iRenderHei	= 600;
}

/*	Initialize game

	Return true for success, otherwise return false.

	hInst: handle of instance.
	hWnd: handle of render window.
*/
bool CRender::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInstance		= hInst;
	m_hRenderWnd	= hWnd;

	if (!InitA3DEngine())
	{
		return false;
	}
	return true;
}



//	Release game
void CRender::Release()
{
	ReleaseA3DEngine();
}

/*	Initialize A3D engine

	Return true for success, otherwise return false.
*/
bool CRender::InitA3DEngine()
{
	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine()))
	{
		return false;
	}

	
	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWid = Rect.right;
	m_iRenderHei = Rect.bottom;
	
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

//	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD | D3DCREATE_PUREDEVICE;
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;	//	Hardware render

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	A3DRELEASE(m_pA3DEngine);
	m_pA3DDevice = NULL;
}