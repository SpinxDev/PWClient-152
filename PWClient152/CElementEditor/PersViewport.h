/*
 * FILE: PersViewport.h
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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

class A3DCamera;
class CStaff;

///////////////////////////////////////////////////////////////////////////
//	
//	Class CPersViewport
//	
///////////////////////////////////////////////////////////////////////////

class CPersViewport : public CViewport
{
public:		//	Types

public:		//	Constructor and Destructor

	CPersViewport();
	virtual ~CPersViewport();

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
	
	// Get camera interface
	A3DCamera* GetCamera(){ return m_pA3DCamera; }

protected:	//	Attributes

	A3DCamera*	m_pA3DCamera;
	POINT		m_ptLastMou;	//	Mouse last position
	bool		m_bRDrag;		//	true, right button is draging

	CStaff*		m_aAxisStaffs[3];	//	Axis staffs

protected:	//	Operations

	//	Adjust camera's position
	void AdjustCameraPos();
	//	Render axis staffs
	bool RenderAxisStaffs();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


