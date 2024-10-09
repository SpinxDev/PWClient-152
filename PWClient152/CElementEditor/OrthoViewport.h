/*
 * FILE: OrthoViewport.h
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

#include "Viewport.h"

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

class A3DOrthoCamera;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class COrthoViewport
//	
///////////////////////////////////////////////////////////////////////////

class COrthoViewport : public CViewport
{
public:		//	Types

	//	Viewport type
	enum
	{
		TYPE_XZ = 0,
		TYPE_XY,
		TYPE_YZ,
	};

public:		//	Constructor and Destructor

	COrthoViewport(int iType);
	virtual ~COrthoViewport();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(CViewFrame* pViewFrame, int iWidth, int iHeight);
	//	Release object
	virtual void Release();

	//	Resize viewport
	virtual bool Resize(int cx, int cy);
	//	Tick routine
	virtual bool FrameMove(DWORD dwDeltaTime);
	//	Render rontine
	virtual bool Render(A3DViewport* pA3DViewport);

	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags);

	//	Keyboard handler.
	virtual void OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
	virtual void OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);

	inline int GetViewPortWidth(){ return m_iWidth; }
	inline int GetViewPortHeight(){ return m_iHeight; }

protected:	//	Attributes

	A3DOrthoCamera*	m_pA3DCamera;

	int		m_iType;		//	View type
	POINT	m_ptLastMou;	//	Mouse last position
	bool	m_bRDrag;		//	true, right button is draging

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


