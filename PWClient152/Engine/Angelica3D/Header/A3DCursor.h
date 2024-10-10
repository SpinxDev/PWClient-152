/*
 * FILE: A3DCursor.h
 *
 * DESCRIPTION: A class manipulate the cursor
 *
 * CREATED BY: Hedi, 2002/5/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCURSOR_H_
#define _A3DCURSOR_H_

#include "A3DTypes.h"
#include "A2DSprite.h"
#include "AString.h"

class A3DDevice;

//////////////////////////////////////////////////////////////////////////////////////
// base class for cursors in Angelica
//////////////////////////////////////////////////////////////////////////////////////
class A3DCursor
{
protected:
public:
	virtual ~A3DCursor() {}

	// Update the cursor pos information and make the cursor animated as possible;
	virtual bool TickAnimation() = 0;
	virtual bool Draw() = 0;
	virtual void OnSysSetCursor() = 0;
	virtual bool ShowCursor(bool bShow) = 0;
};


//////////////////////////////////////////////////////////////////////////////////////
// hardware implemented cursors in Angelica
//////////////////////////////////////////////////////////////////////////////////////

class A3DHardCursor : public A3DCursor, A2DSprite
{
private:
	AString			m_strCursorFile;

	A3DPOINT2		m_ptHotSpot;
	A3DPOINT2		m_ptCursorPos;

	int				m_nFrameCount;
	int				m_nFrame;

private:
	bool SetHotSpot(A3DPOINT2& ptHotSpot);
	bool Release();

public:
	A3DHardCursor();
	virtual ~A3DHardCursor();

	bool Load(A3DDevice * pDevice, const char * szCursorFile, int nWidth, int nHeight, A3DCOLOR colorKey, int nFrameCount, A3DRECT * pRects, A3DPOINT2& ptHotSpot);
	
	// Update the cursor pos information and make the cursor animated as possible;
	virtual bool TickAnimation();
	virtual bool Draw();
	virtual void OnSysSetCursor();
	virtual bool ShowCursor(bool bShow);
};


//////////////////////////////////////////////////////////////////////////////////////
// system cursor implemented cursors in Angelica
//////////////////////////////////////////////////////////////////////////////////////

class A3DSysCursor : public A3DCursor
{
private:
	HCURSOR		m_hCursor;

public:
	A3DSysCursor();
	virtual ~A3DSysCursor();

	bool Load(A3DDevice * pDevice, const char * szCursorFile);
	
	// Update the cursor pos information and make the cursor animated as possible;
	virtual bool TickAnimation();
	virtual bool Draw();
	virtual void OnSysSetCursor();
	virtual bool ShowCursor(bool bShow);
};

#endif//_A3DCURSOR_H_
