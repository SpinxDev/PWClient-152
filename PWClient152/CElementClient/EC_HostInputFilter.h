/*
 * FILE: EC_HostInputFilter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "APoint.h"
#include "EC_InputFilter.h"
#include "AAssist.h"
#include <hashmap.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Logic keys
enum
{
	LKEY_CANCEL = 1,		//	Cancel action etc.
	LKEY_JUMP,				//	Jump
	LKEY_PLAYTRICK,			//	Play trick action
	LKEY_PUSHCAMERA,		//	Push camera
	LKEY_PULLCAMERA,		//	Pull camera

	LKEY_UI_BEGIN,

	LKEY_UI_SHOW_ALL = LKEY_UI_BEGIN,
	LKEY_UI_SHOW_MAP,
	LKEY_UI_SHOW_CHARACTER,
	LKEY_UI_SHOW_INVENTORY,
	LKEY_UI_SHOW_SKILL,
	LKEY_UI_SHOW_PET,
	LKEY_UI_SHOW_QUEST,
	LKEY_UI_SHOW_QSHOP,
	LKEY_UI_SHOW_ACTION,
	LKEY_UI_SHOW_TEAM,
	LKEY_UI_SHOW_FRIEND,
	LKEY_UI_SHOW_FACTION,
	LKEY_UI_SHOW_BBS,
	LKEY_UI_SHOW_HELP,
	LKEY_UI_SHOW_CONSOLE,
	LKEY_UI_SHOW_GM,
	LKEY_UI_SHOW_SYS,
	LKEY_UI_SHOW_CAMERA,
	LKEY_UI_SHOW_QUICKKEY,

	LKEY_UI_QUICK9_CHANGECONTENT,
	LKEY_UI_QUICK9_SC1,
	LKEY_UI_QUICK9_SC2,
	LKEY_UI_QUICK9_SC3,
	LKEY_UI_QUICK9_SC4,
	LKEY_UI_QUICK9_SC5,
	LKEY_UI_QUICK9_SC6,
	LKEY_UI_QUICK9_SC7,
	LKEY_UI_QUICK9_SC8,
	LKEY_UI_QUICK9_SC9,

	LKEY_UI_QUICK8_CHANGECONTENT,
	LKEY_UI_QUICK8_SC1,
	LKEY_UI_QUICK8_SC2,
	LKEY_UI_QUICK8_SC3,
	LKEY_UI_QUICK8_SC4,
	LKEY_UI_QUICK8_SC5,
	LKEY_UI_QUICK8_SC6,
	LKEY_UI_QUICK8_SC7,
	LKEY_UI_QUICK8_SC8,

	LKEY_UI_CHAT_CLEAR,
	LKEY_UI_CHAT_REPLY,

	LKEY_UI_TEAM_SELECT1,
	LKEY_UI_TEAM_SELECT2,
	LKEY_UI_TEAM_SELECT3,
	LKEY_UI_TEAM_SELECT4,
	LKEY_UI_TEAM_SELECT5,
	LKEY_UI_TEAM_SELECT6,
	LKEY_UI_TEAM_SELECT7,
	LKEY_UI_TEAM_SELECT8,
	LKEY_UI_TEAM_SELECT9,

	LKEY_UI_PET_ATTACK,
	LKEY_UI_PET_SKILL1,
	LKEY_UI_PET_SKILL2,
	LKEY_UI_PET_SKILL3,
	LKEY_UI_PET_SKILL4,
	LKEY_UI_PET_SKILL5,
	LKEY_UI_PET_SKILL6,
	LKEY_UI_PET_SKILL7,

	LKEY_UI_SWITCH_SPEEDSETTING,
	LKEY_UI_MONSTER_AUTOSELECT,

	LKEY_UI_GOBLIN_SKILL1,
	LKEY_UI_GOBLIN_SKILL2,
	LKEY_UI_GOBLIN_SKILL3,
	LKEY_UI_GOBLIN_SKILL4,
	LKEY_UI_GOBLIN_SKILL5,
	LKEY_UI_GOBLIN_SKILL6,
	LKEY_UI_GOBLIN_SKILL7,
	LKEY_UI_GOBLIN_SKILL8,

	LKEY_UI_SHOW_AUTOHPMP,
	
	LKEY_UI_SHOW_AUTOHELPPOP,
	LKEY_UI_SHOW_AUTOPOLICY,
	LKEY_UI_SWITCH_AUTOPOLICY,
	LKEY_UI_SYSMODEULE_SC1,
	LKEY_UI_SYSMODEULE_SC2,
	LKEY_UI_SYSMODEULE_SC3,
	LKEY_UI_SYSMODEULE_SC4,


	LKEY_UI_END = LKEY_UI_SYSMODEULE_SC4,

	LKEY_MOVE_LEFT,
	LKEY_MOVE_RIGHT,
	LKEY_MOVE_FORWARD,
	LKEY_MOVE_BACKWARD,
	LKEY_MOVE_UP,
	LKEY_MOVE_DOWN,
	LKEY_MOVE_AUTOFORWARD,
	LKEY_MOVE_FOLLOW,
	LKEY_MOVE_DOWN2,

	NUM_LKEY,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECHostPlayer;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostInputFilter
//	
///////////////////////////////////////////////////////////////////////////
class CECHostInputFilter : public CECInputFilter
{
public:		//	Types

	//	Mouse press information
	struct PRESS
	{
		bool	bPressed;		//	Pressed flag
		DWORD	dwPressTime;	//	Start pressing time stamp
		APointI	ptPress;		//	Press position
		bool	bTurnCamera;	//	Turn camera flag
	};

	//	Key map group
	enum KEYMAP
	{
		KM_NORMAL = 0,
		KM_CTRL,
		KM_SHIFT,
		NUM_KEYMAP,
	};

public:		//	Constructor and Destructor

	CECHostInputFilter(CECInputCtrl* pInputCtrl);
	virtual ~CECHostInputFilter();

public:		//	Attributes

public:		//	Operations

	//	Translate input
	virtual bool TranslateInput(int iInput);

	const PRESS& GetLBtnPressinfo() { return m_LBPress; }
	const PRESS& GetRBtnPressInfo() { return m_RBPress; }

	// Hot key file
	void LoadHotKey();
	void SaveHotKey();

	// Hot key redefinition
	bool IsUsageCustomizable(int iUsage);
	bool IsHotKeyValid(int nHotKey, DWORD dwModifier);
	bool IsHotKeyUsed(int nHotKey, DWORD dwModifier, int *iUsage=NULL);

	void SetCurrentCustomize(int iUsage, bool discardWhenFinish=false);
	bool CustomizeHotKey(int iUsage, int nHotKey, DWORD dwModifier);
	bool IsExistEmptyCustomizeHotKey();
	bool IsCustomizeHotKeyChanged();
	bool IsInCustomize();
	void DeleteHotKeyCustomize(int iUsage);
	void DefaultHotKey();

	bool IsMoveUsagePressed();

	// Hot key display
	bool GetUsageShowDescription(int iUsage, ACString &strDesc, bool simple);
	static ACString GetHotKeyDescription(int nHotKey, DWORD dwModifier, bool simple);

protected:	//	Attributes

	APointI		m_TurnOffset;		//	Camera turning offset
	APointI		m_ptTurnStart;		//	Start position to turn camera

	PRESS		m_LBPress;			//	Left button press information
	PRESS		m_RBPress;			//	Right button press information

	typedef abase::hash_map<int, DWORD> HotKeySettings;
	// Find user hot key settings(with modifiers) for given game functionality such as finding nearest monster
	HotKeySettings m_hotKeySettings;

	typedef abase::hash_map<DWORD, int> HotKeyUsages;
	// Find game functionality such as finding nearest monster with current key stroke(with modifiers)
	HotKeyUsages m_hotKeyUsages;

	int          m_iUsageCustomize; // usage customized currently
	abase::vector<int> m_backupUsageCustomize;    // usage and hot key backup for canceling customize
	abase::vector<DWORD> m_backupHotKeyCustomize;
	
protected:	//	Operations

	//	Translate keyboard input
	bool TranslateKeyboard(CECHostPlayer* pHost);
	void TranslateKeyboardMove(CECHostPlayer *pHost);
	//	Translate mouse input
	bool TranslateMouse(CECHostPlayer* pHost);

	//	Key message handlers
	void OnKeyDown(CECHostPlayer* pHost, int iKey, DWORD dwFlags);
	void OnKeyUp(CECHostPlayer* pHost, int iKey, DWORD dwFlags);

	//	Mouse message handlers
	void OnLButtonDown(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnLButtonUp(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnLButtonDblClk(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnRButtonDown(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnRButtonUp(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnRButtonDblClk(CECHostPlayer* pHost, int x, int y, DWORD dwFlags);
	void OnMouseMove(CECHostPlayer* pHost, int x, int y, DWORD dwFlags, WPARAM wParam);
	void OnMouseWheel(CECHostPlayer* pHost, int x, int y, int iDelta, DWORD dwFlags);

	//  Hot key related
	void  DefaultHotKeyImpl(bool bReservedOnly, bool bReset);
	DWORD CompressHotKey(int nHotKey, DWORD dwModifier);
	void  DecompressHotKey(DWORD dwHotKey, int &nHotKey, DWORD &dwModifyer);
	bool  RegisterHotKey(int iUsage, int nHotKey, DWORD dwModifier);
	void  DeleteHotKey(int iUsage);

	int   FindHotKeyUsageReserved(int nHotKey, DWORD dwModifyer);
	bool  FindUsageHotKeyReserved(int iUsage, int &nHotKey, DWORD &dwModifier);
	int   FindHotKeyUsage(int nHotKey, DWORD dwModifyer);
	bool  FindUsageHotKey(int iUsage, int &nHotKey, DWORD &dwModifier);

	bool  IsHotKeyPressed(int nHotKey, DWORD dwModifier);
	bool  IsUsagePressed(int iUsage);

	// Backup and restore customize usage
	void  BackupCustomize();
	void  RestoreCustomizeBackup();
	void  ClearCustomizeBackup();
	
	bool  GetHotKeyFileName(AString &strFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


