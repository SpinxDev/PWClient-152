/*
 * FILE: EC_UIManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "vector.h"
#include "AAssist.h"

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

class A3DEngine;
class AUIManager;
class CECLoginUIMan;
class CECGameUIMan;
class CECBaseUIMan;
class CECScriptMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECUIManager
//	
///////////////////////////////////////////////////////////////////////////

class CECUIManager
{
public:		//	Types

	//	UI manager index
	enum
	{
		UIMAN_LOGIN = 0,	//	Login UI manager
		UIMAN_INGAME,		//	In game UI manager
	};

public:		//	Constructor and Destructor

	CECUIManager();
	virtual ~CECUIManager();

public:		//	Attributes

public:		//	Operations

	//	Initalize manager
	bool Init(A3DEngine* pA3DEngine, int iInitUIMan);
	//	Release manager
	void Release(void);

	//	Tick routine
	bool Tick();
	//	Render routine
	bool Render();

	//	Change current UI manager
	bool ChangeCurUIManager(int iUIMan);
	//	Deal windows message
	bool DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam);
	//	Filter bad words
	void FilterBadWords(ACString &str);
	bool HasBadWords(ACString &str);

	//	Get current UI manager
	int GetCurrentUIMan() { return m_iCurUIMan; }
	AUIManager* GetCurrentUIManPtr() { return m_pCurUIMan; }
	//	If current UI manager is LOGIN manager, this function return it's interface
	CECLoginUIMan* GetLoginUIMan();
	//	If current UI manager is INGAME manager, this function return it's interface
	CECGameUIMan* GetInGameUIMan();
	CECBaseUIMan * GetBaseUIMan();
	
	//	Get script help manager
	CECScriptMan* GetScriptMan() { return m_pScriptMan; }
	//	Get bad words table
	abase::vector<ACString>& GetBadWords() { return m_vecBadWords; }

	bool UIControlCursor();
	void ShowErrorMsg(const ACHAR *pszMsg, const char *pszName="");
	void ShowLinkBrokenMsg(int iMsg);
	void ShowReconnectMsg(int iMsg);

	static AString GetUIDcf(int type, int theme, bool debug = false);
	int ResetTheme();

protected:	//	Attributes

	A3DEngine*	m_pA3DEngine;	//	A3D engine object
	AUIManager*	m_pCurUIMan;	//	Current UI manager
	int			m_iCurUIMan;	//	Current UI manager inde

	CECScriptMan* m_pScriptMan;	//	help system

	abase::vector<ACString> m_vecBadWords;


protected:	//	Operations

	//	Release current UI manager
	void ReleaseCurUIManager();
	//	Load bad words
	bool LoadBadWords();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

