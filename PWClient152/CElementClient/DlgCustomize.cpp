/*
 * FILE: DlgCustomize.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgCustomize.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Player.h"
#include "EC_Face.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_Viewport.h"

AUI_BEGIN_COMMAND_MAP(CDlgCustomize, CDlgCustomizeBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandOnCancel)
AUI_ON_COMMAND("IDOK", OnCommandOnOK)

AUI_END_COMMAND_MAP()

//----------------------------------------------------------
CDlgCustomize::CDlgCustomize()
{
	
}

//----------------------------------------------------------
CDlgCustomize::~CDlgCustomize()
{

}

//----------------------------------------------------------
void CDlgCustomize::OnCommandOnCancel(const char* szCommand)
{
	if (!GetPlayer()){
		assert(false);
		return;
	}
	GetCustomizeMan()->OnCancel();
}

//----------------------------------------------------------
void CDlgCustomize::OnCommandOnOK(const char* szCommand)
{
	GetCustomizeMan()->OnOK();
}

//----------------------------------------------------------------------
void CDlgCustomize::OnTick()
{
	RandomExpression();
}

void CDlgCustomize::RandomExpression(void)
{
	int nRand = rand()%(FACE_ANIMATION_MAX + 500);
	if( nRand < FACE_ANIMATION_MAX)
	{		
		GetFace()->PlayAnimation((FACE_ANIMATION)nRand);
	}
}