/*
 * FILE: EC_GhostInputFilter.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_GhostInputFilter.h"
#include "EC_InputCtrl.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Utility.h"
#include "EC_MsgDef.h"
#include "EC_Manager.h"

#define new A_DEBUG_NEW

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
//	Implement CECGhostInputFilter
//	
///////////////////////////////////////////////////////////////////////////

CECGhostInputFilter::CECGhostInputFilter(CECInputCtrl* pInputCtrl) : CECInputFilter(pInputCtrl)
{
}

CECGhostInputFilter::~CECGhostInputFilter()
{
}

//	Translate input
bool CECGhostInputFilter::TranslateInput(int iInput)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	
	if (iInput == IT_KEYBOARD)
	{
		int iMove = -1;

		if (m_pInputCtrl->KeyIsBeingPressed('W'))
		{
			if (m_pInputCtrl->KeyIsBeingPressed('A'))
				iMove = 1;
			else if (m_pInputCtrl->KeyIsBeingPressed('D'))
				iMove = 7;
			else
				iMove = 0;
		}
		else if (m_pInputCtrl->KeyIsBeingPressed('S'))
		{
			if (m_pInputCtrl->KeyIsBeingPressed('A'))
				iMove = 3;
			else if (m_pInputCtrl->KeyIsBeingPressed('D'))
				iMove = 5;
			else
				iMove = 4;
		}
		else if (m_pInputCtrl->KeyIsBeingPressed('A'))
			iMove = 2;
		else if (m_pInputCtrl->KeyIsBeingPressed('D'))
			iMove = 6;

		if (iMove >= 0)
			pGameRun->PostMessage(MSG_GST_MOVE, MAN_PLAYER, 0, iMove);

		if (m_pInputCtrl->KeyIsBeingPressed('Q'))
			pGameRun->PostMessage(MSG_GST_MOVEABSUP, MAN_PLAYER, 0, 1);
		else if (m_pInputCtrl->KeyIsBeingPressed('Z'))
			pGameRun->PostMessage(MSG_GST_MOVEABSUP, MAN_PLAYER, 0, 0);
	}
/*	else if (iInput == IT_MOUSE)
	{
		if (m_pInputCtrl->MouBtnIsBeingPressed(CECInputCtrl::MOU_RBUTTON))
		{
			int dx, dy;
			m_pInputCtrl->GetMouseOffset(&dx, &dy);

			if (dx)
			{
				float fDeg = dx * 0.1f; //	fMouseSpeed;
				pGameRun->PostMessage(MSG_GST_YAW, MAN_PLAYER, 0, FLOATTOFIX16(fDeg));
			}

			if (dy)
			{
				float fDeg = dy * 0.1f;	// fMouseSpeed * iMouseVert;
				pGameRun->PostMessage(MSG_GST_PITCH, MAN_PLAYER, 0, FLOATTOFIX16(fDeg));
			}
		}
	}
*/
	return true;
}


