/*
 * FILE: EC_UIManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "ECScriptManInGame.h"
#include "EC_Viewport.h"

#include <A3DEngine.h>
#include <AWScriptFile.h>

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
//	Implement CECUIManager
//	
///////////////////////////////////////////////////////////////////////////

CECUIManager::CECUIManager()
{
	m_pA3DEngine	= NULL;
	m_pCurUIMan		= NULL;
	m_iCurUIMan		= -1;
	m_pScriptMan	= NULL;
}

CECUIManager::~CECUIManager()
{
}

//	Initialize manager
bool CECUIManager::Init(A3DEngine* pA3DEngine, int iInitUIMan)
{
	m_pA3DEngine	= pA3DEngine;
	m_iCurUIMan		= iInitUIMan;

	//	Load bad words
	if (!LoadBadWords())
	{
		a_LogOutput(1, "CECUIManager::Init, Failed to load bad words");
		return false;
	}

	if (!ChangeCurUIManager(iInitUIMan))
	{
		a_LogOutput(1, "CECUIManager::Init, Failed to create initial UI manager");
		return false;
	}

	return true;
}

//	Release manager
void CECUIManager::Release(void)
{
	m_vecBadWords.clear();

	ReleaseCurUIManager();
}

//	Load bad words
bool CECUIManager::LoadBadWords()
{
	AWScriptFile s;
	if (!s.Open("Configs\\BadWords.txt"))
		return AUI_ReportError(__LINE__, __FILE__);

	while (s.GetNextToken(true))
	{
		wcslwr(s.m_szToken);
		m_vecBadWords.push_back(s.m_szToken);
	}

	s.Close();

	return true;
}

AString CECUIManager::GetUIDcf(int type, int theme, bool debug)
{
	//  迷你客户端强行使用version01界面
	if( g_pGame->GetConfigs()->IsMiniClient() )
		theme = 1;

	if(type == CECUIManager::UIMAN_LOGIN)
	{
		if(debug && theme > 1)
		{
			return AString().Format("loginui-v%d.dcf", theme);
		}
		// for safe reason we only use these two known name
		return theme == 0 ? "loginui.dcf" : "loginui-v1.dcf";
	}
	else if(type == CECUIManager::UIMAN_INGAME)
	{
		if(debug && theme > 1)
		{
			return AString().Format("ingame-v%d.dcf", theme);
		}

		// for safe reason we only use these two known name
		return theme == 0 ? "ingame.dcf" : "ingame-v1.dcf";
	}

	ASSERT(FALSE);
	return "";
}

int CECUIManager::ResetTheme()
{
	int iTheme = -1;

	CECBaseUIMan* pUI = dynamic_cast<CECBaseUIMan*>(GetCurrentUIManPtr());
	if(pUI)
	{
		iTheme = g_pGame->GetConfigs()->GetSystemSettings().iTheme;
		pUI->ChangeLayout(GetUIDcf(GetCurrentUIMan(), iTheme));
	}

	return iTheme;
}

//	Change current UI manager
bool CECUIManager::ChangeCurUIManager(int iUIMan)
{
	if (iUIMan != m_iCurUIMan)
		ReleaseCurUIManager();

	if (iUIMan < 0)
		return true;

	AString strDcf = GetUIDcf(iUIMan, g_pGame->GetConfigs()->GetSystemSettings().iTheme);

	if (iUIMan == UIMAN_LOGIN)
	{
		CECLoginUIMan* pLoginUI = new CECLoginUIMan;
		if (!pLoginUI)
		{
			glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECUIManager::ChangeCurUIManager", __LINE__);
			return false;
		}

		if (!pLoginUI->Init(m_pA3DEngine, m_pA3DEngine->GetA3DDevice(), strDcf))
		{
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECUIManager::ChangeCurUIManager", __LINE__);
			return false;
		}

		m_pCurUIMan = pLoginUI;
	}
	else if (iUIMan == UIMAN_INGAME)
	{
		CECGameUIMan* pInGameUI = new CECGameUIMan;
		if (!pInGameUI)
		{
			glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECUIManager::ChangeCurUIManager", __LINE__);
			return false;
		}

		if (!pInGameUI->Init(m_pA3DEngine, m_pA3DEngine->GetA3DDevice(), strDcf))
		{
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECUIManager::ChangeCurUIManager", __LINE__);
			return false;
		}

		m_pCurUIMan = pInGameUI;
	}
	else
	{
		ASSERT(0);
		return false;
	}	
	
	m_iCurUIMan	= iUIMan;

	
	// init help system
	if (iUIMan == UIMAN_LOGIN)
	{
		ASSERT(!m_pScriptMan);
		// create help system for login ui
		// not used now
	}
	else if (iUIMan == UIMAN_INGAME)
	{
		ASSERT(!m_pScriptMan);
		
		if (!(m_pScriptMan = new CECScriptManInGame))
		{
			glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECUIManager::ChangeCurUIManager", __LINE__);
			return false;
		}
		
		if (!m_pScriptMan->Init())
		{
			delete m_pScriptMan;
			m_pScriptMan = NULL;
			a_LogOutput(1, "CECUIManager::ChangeCurUIManager, Failed to initialize Help system for game.");
			return false;
		}
	}
	
	A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
	A3DRECT rcNew(param.X, param.Y, param.X+param.Width, param.Y+param.Height);
	m_pCurUIMan->RearrangeWindows(m_pCurUIMan->GetRect(), rcNew);

	return true;
}

//	Release current UI manager
void CECUIManager::ReleaseCurUIManager()
{
	if (m_iCurUIMan < 0)
	{
		ASSERT(!m_pCurUIMan);
		return;
	}

	A3DRELEASE(m_pScriptMan);

	ASSERT(m_pCurUIMan);
	m_pCurUIMan->Release();
	delete m_pCurUIMan;
	m_pCurUIMan = NULL;
	m_iCurUIMan = -1;
}

//	Deal windows message
bool CECUIManager::DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_pCurUIMan)
		return false;

	if( m_pCurUIMan->DealWindowsMessage(message, wParam, lParam))
	{
		return true;
	}
    if( GetBaseUIMan() &&
		GetBaseUIMan()->IsCustomizeCharacter()&& 
		GetBaseUIMan()->GetCustomizeMgr()->CustomizeDealWindowsMessage(message,wParam,lParam) )
	{
		return true;
	}

    return false;
}

//	Tick routine
bool CECUIManager::Tick()
{
	if (m_pCurUIMan)
	{
		m_pCurUIMan->Tick(g_pGame->GetTickTime());

		if( GetBaseUIMan()->IsCustomizeCharacter() )
		{
			GetBaseUIMan()->GetCustomizeMgr()->Tick();
		}
	}
	// help system
	if (m_pScriptMan)
		m_pScriptMan->Tick(0);

	return true;
}

//	Render routine
bool CECUIManager::Render()
{
	if (m_pCurUIMan)
		m_pCurUIMan->Render();

	return true;
}

//	If current UI manager is LOGIN manager, this function return it's interface
CECLoginUIMan* CECUIManager::GetLoginUIMan()
{
	if (m_pCurUIMan && m_iCurUIMan == UIMAN_LOGIN)
		return (CECLoginUIMan*)m_pCurUIMan;
	else
		return NULL;
}

//	If current UI manager is INGAME manager, this function return it's interface
CECGameUIMan* CECUIManager::GetInGameUIMan()
{
	if (m_pCurUIMan && m_iCurUIMan == UIMAN_INGAME)
		return (CECGameUIMan*)m_pCurUIMan;
	else
		return NULL;
}

CECBaseUIMan * CECUIManager::GetBaseUIMan()
{
	return dynamic_cast<CECBaseUIMan *>(m_pCurUIMan);
}

bool CECUIManager::UIControlCursor()
{
	if( m_pCurUIMan )
	{
		if( m_iCurUIMan == UIMAN_LOGIN )
			return ((CECLoginUIMan *)m_pCurUIMan)->UIControlCursor();
		else if( m_iCurUIMan == UIMAN_INGAME )
			return ((CECGameUIMan *)m_pCurUIMan)->UIControlCursor();
	}
	return false;
}

void CECUIManager::ShowErrorMsg(const ACHAR *pszMsg, const char *pszName/* "" */)
{
	if( m_pCurUIMan )
	{
		if( m_iCurUIMan == UIMAN_LOGIN )
			((CECLoginUIMan *)m_pCurUIMan)->ShowErrorMsg2(pszMsg, pszName);
		else if( m_iCurUIMan == UIMAN_INGAME )
			((CECGameUIMan *)m_pCurUIMan)->ShowErrorMsg(pszMsg, pszName);
	}
}

void CECUIManager::ShowLinkBrokenMsg(int iMsg){
	const static char* szDlgName = "MsgBox_LinkBroken";
	if (CECBaseUIMan *pBaseUIMan = GetBaseUIMan()){
		if (!pBaseUIMan->GetDialog(szDlgName)){
			ShowErrorMsg(g_pGame->GetFixedMsgTab()->GetWideString(iMsg), szDlgName);
		}
	}
}

void CECUIManager::ShowReconnectMsg(int iMsg){
	const static char* szDlgName = "MsgBox_RemindReconnect";
	if (CECBaseUIMan *pBaseUIMan = GetBaseUIMan()){
		if (!pBaseUIMan->GetDialog(szDlgName)){
			pBaseUIMan->ShowReconnectMsg(g_pGame->GetFixedMsgTab()->GetWideString(iMsg), szDlgName);
		}
	}
}

void CECUIManager::FilterBadWords(ACString &str)
{
	int i, j, nPos;

	ACString strLwr((const ACHAR *)str);
	strLwr.MakeLower();
	for( i = 0; i < (int)m_vecBadWords.size(); i++ )
	{
		nPos = 0;
		while( (nPos = strLwr.Find(m_vecBadWords[i], nPos)) >= 0 )
		{
			for( j = 0; j < m_vecBadWords[i].GetLength(); j++ )
			{
				str[nPos] = '*';
				nPos ++;
			}
		} 
	}
}

bool CECUIManager::HasBadWords(ACString &str)
{
	bool bRet(false);

	ACString strLwr = str;
	strLwr.MakeLower();
	for(int i = 0; i < (int)m_vecBadWords.size(); i++ )
	{
		if (strLwr.Find(m_vecBadWords[i]) >= 0)
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}
