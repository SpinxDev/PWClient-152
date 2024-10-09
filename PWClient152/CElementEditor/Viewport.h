/*
 * FILE: Viewport.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
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

class CViewFrame;
class A3DViewport;
class A3DCameraBase;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CViewport
//	
///////////////////////////////////////////////////////////////////////////

class CViewport
{
public:		//	Types

public:		//	Constructor and Destructor

	CViewport();
	virtual ~CViewport();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(CViewFrame* pViewFrame, int iWidth, int iHeight);
	//	Release object
	virtual void Release();

	//	Resize viewport
	virtual bool Resize(int cx, int cy);
	//	Tick routine
	virtual bool FrameMove(DWORD dwDeltaTime) { return true; }
	//	Render rontine
	virtual bool Render(A3DViewport* pA3DViewport) { return true; }

	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags) { return true; }

	//	Keyboard handler.
	virtual void OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) {}
	virtual void OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) {}

	//	Reset camera position
	virtual void ResetCameraPos(float x, float y, float z);
	//	Get A3D camera object
	A3DCameraBase* GetA3DCamera() { return m_pA3DCameraBase; }

protected:	//	Attributes

	CViewFrame*		m_pViewFrame;		//	View frame
	A3DCameraBase*	m_pA3DCameraBase;	//	A3D camera object
	DWORD			m_dwLastMove;		//	Last camera move time

	int		m_iWidth;		//	Viewport size
	int		m_iHeight;	

protected:	//	Operations

	//	Move camera forward / backward
	void MoveCameraForward(bool bForward, float fDeltaTime);
	//	Move camera up / down
	void MoveCameraUp(bool bUp, float fDeltaTime);
	//	Move camera right / left
	void MoveCameraRight(bool bRight, float fDeltaTime);
	//	Move camera absolute up / down
	void MoveCameraAbsUp(bool bUp, float fDeltaTime);
	//	Get frame time
	float GetFrameTime();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

