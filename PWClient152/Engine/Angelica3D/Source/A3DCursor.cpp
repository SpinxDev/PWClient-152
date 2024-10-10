/*
 * FILE: A3DCursor.cpp
 *
 * DESCRIPTION: A class manipulate the cursor
 *
 * CREATED BY: Hedi, 2002/5/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCursor.h"
#include "A3DSurfaceMan.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DPI.h"

//////////////////////////////////////////////////////////////////////////////////////
// hardware cursor implemented cursors in Angelica
//////////////////////////////////////////////////////////////////////////////////////

A3DHardCursor::A3DHardCursor()
{
	ZeroMemory(&m_ptHotSpot, sizeof(A3DPOINT2));
	ZeroMemory(&m_ptCursorPos, sizeof(A3DPOINT2));

	m_nFrameCount = 0;
	m_nFrame = -1;
}

A3DHardCursor::~A3DHardCursor()
{
	Release();
}

bool A3DHardCursor::Load(A3DDevice * pDevice, const char * szCursorFile, int nWidth, int nHeight, A3DCOLOR colorKey, int nFrameCount, A3DRECT * pRects, A3DPOINT2& ptHotSpot)
{
	if( !A2DSprite::Init(pDevice, szCursorFile, nWidth, nHeight, colorKey, nFrameCount, pRects) )
	{
		g_A3DErrLog.Log("A3DCursor::Init(), Can not init an A2DSprite!");
		return false;
	}

	m_strCursorFile = szCursorFile;
	m_nFrameCount = nFrameCount;
	m_nFrame = -1;

	// For the first time, we set initializing data;
	SetHotSpot(ptHotSpot);
	TickAnimation();
	return true;
}

bool A3DHardCursor::Release()
{
	A2DSprite::Release();
	return true;
}

bool A3DHardCursor::Draw()
{
	POINT ptPos;
	::GetCursorPos(&ptPos);

	m_ptCursorPos.x = ptPos.x;
	m_ptCursorPos.y = ptPos.y;
	
	return A2DSprite::DrawToBack(m_ptCursorPos.x, m_ptCursorPos.y);	
}

bool A3DHardCursor::TickAnimation()
{
	// Animation;
	if( m_nFrame == -1 )
		m_nFrame = 0;
	else
	{
		++ m_nFrame;
		m_nFrame %= m_nFrameCount;
	}
	A2DSprite::SetCurrentItem(m_nFrame);
	return true;
}

bool A3DHardCursor::SetHotSpot(A3DPOINT2& ptHotSpot)
{
	m_ptHotSpot = ptHotSpot;
	if( m_ptHotSpot.x < 0 )
		m_ptHotSpot.x = 0;
	if( m_ptHotSpot.y < 0 )
		m_ptHotSpot.y = 0;
	if( m_ptHotSpot.x >= m_nWidth )
		m_ptHotSpot.x = m_nWidth - 1;
	if( m_ptHotSpot.y >= m_nHeight )
		m_ptHotSpot.y = m_nHeight - 1;

	A2DSprite::SetLocalCenterPos(m_ptHotSpot.x, m_ptHotSpot.y);
	return true;
}

bool A3DHardCursor::ShowCursor(bool bShow)
{
	return true;
}

void A3DHardCursor::OnSysSetCursor()
{
	::SetCursor(NULL);
}


//////////////////////////////////////////////////////////////////////////////////////
// system cursor implemented cursors in Angelica
//////////////////////////////////////////////////////////////////////////////////////


A3DSysCursor::A3DSysCursor()
{
	m_hCursor = NULL;
}

A3DSysCursor::~A3DSysCursor()
{
	if( m_hCursor )
	{
		DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	}
}

bool A3DSysCursor::Load(A3DDevice * pDevice, const char * szCursorFile)
{
	m_hCursor = LoadCursorFromFileA(szCursorFile);
	if( NULL == m_hCursor )
	{
		g_A3DErrLog.Log("A3DSysCursor::Load(), Can not load cursor file [%s]!", szCursorFile);
		return false;
	}

	return true;
}

bool A3DSysCursor::TickAnimation()
{
	return true;
}

bool A3DSysCursor::Draw()
{
	return true;
}

bool A3DSysCursor::ShowCursor(bool bShow)
{
	if( bShow )
		::SetCursor(m_hCursor);
	else
		::SetCursor(NULL);
	return true;
}

void A3DSysCursor::OnSysSetCursor()
{
	::SetCursor(m_hCursor);
}
