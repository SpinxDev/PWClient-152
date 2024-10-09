/*
 * FILE: IconButton.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include "IconButton.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CIconButton
//	
///////////////////////////////////////////////////////////////////////////

CIconButton::CIconButton()
{
	m_hIcon = NULL;
}

CIconButton::~CIconButton()
{
}

BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Set icon for button
bool CIconButton::SetButtonIcon(int iSize, int iIconID)
{
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	if (!(m_hIcon = LoadIcon(hInst, MAKEINTRESOURCE(iIconID))))
		return false;

	m_iSize = iSize;

	return true;
}

void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	UINT uStyle = DFCS_BUTTONPUSH;
	
	//	If drawing selected, add the pushed style to DrawFrameControl.
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uStyle |= DFCS_PUSHED;

	//	Draw the button frame.
	::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem,
						DFC_BUTTON, uStyle);
	if (!m_hIcon)
		return;

	//	Draw icon at center
	RECT rcClient;
	GetClientRect(&rcClient);

	int x = (rcClient.right - m_iSize) / 2;
	int y = (rcClient.bottom - m_iSize) / 2;

	::DrawIconEx(lpDrawItemStruct->hDC, x, y, m_hIcon, m_iSize, m_iSize, 0, NULL, DI_NORMAL);
}

