  /*
 * FILE: EC_HostInputFilter.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

//	For WM_MOUSEWHEEL message
#define _WIN32_WINDOWS	0x0500

#include "EC_Global.h"
#include "EC_HostInputFilter.h"
#include "EC_InputCtrl.h"
#include "EC_CameraCtrl.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Utility.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "EC_Manager.h"
#include "EC_World.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include <windowsx.h>
#include <winuser.h>
#include "EC_AutoPolicy.h"
#include "EC_TimeSafeChecker.h"
#include "EC_ComboSkillState.h"
#include "DlgComboSkillShortCut.h"
#include "EC_Skill.h"
#include "EC_HPWorkSpell.h"

#include "AC.h"
#include "A3DDevice.h"
#include <AIniFile.h>
#include "roleinfo"
#include <AFI.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define CLICK_DELAY		200
#define HOTKEYFILE_VERSION		2

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

static DWORD _JumpTime = 0;

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
//	Implement CECHostInputFilter
//	
///////////////////////////////////////////////////////////////////////////

CECHostInputFilter::CECHostInputFilter(CECInputCtrl* pInputCtrl) :
CECInputFilter(pInputCtrl)
{
	m_LBPress.bPressed		= false;
	m_LBPress.bTurnCamera	= false;
	m_RBPress.bPressed		= false;
	m_RBPress.bTurnCamera	= false;

	m_iUsageCustomize = 0;
}

CECHostInputFilter::~CECHostInputFilter()
{
}

bool CECHostInputFilter::GetHotKeyFileName(AString &strFile)
{
	// Get save/read hot key file name per character
	//
	bool result(false);	
	
	// Load from file
	while (true)
	{
		const GNET::RoleInfo &info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
		int idHost = info.roleid;
		if (!idHost)
		{
			ASSERT(false);
			break;
		}
		strFile.Format("userdata\\layout\\%d.key", idHost);
		result = true;
		break;
	}

	return result;
}

void CECHostInputFilter::LoadHotKey()
{
	// Load hot key, from file or given default value
	//

	// Use ingame.stf to store hot key name (also description)
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (!pGameUIMan)
		return;

	// Load from file
	AString strFile;
	if (!GetHotKeyFileName(strFile))
	{
		a_LogOutput(1, "CECHostInputFilter::LoadHotKey, failed to make hotkey file name");

		// Load all default hot key
		DefaultHotKey();
		return;
	}

	bool loaded(false);
	
	if (af_IsFileExist(strFile))
	{
		AIniFile file;
		if (file.Open(strFile))
		{
			while (true)
			{
				//	Read infos
				AString strSect = "Info";
				int iVersion = file.GetValueAsInt(strSect, "version", 0);
				if (iVersion > HOTKEYFILE_VERSION)
					break;
				
				// Clear and set default hot key that cannot customize
				DefaultHotKeyImpl(true, true);
				
				//	Read key map
				strSect = "Map";
				AString strKey;
				int aVals[1];
				DWORD dwHotKey(0);
				int nHotKey(0);
				DWORD dwModifier(0);
				
				//	Remap all logic key maps
				for (int iUsage=1; iUsage<NUM_LKEY; iUsage++)
				{
					if (!IsUsageCustomizable(iUsage))
						continue;
					
					strKey = AC2AS(pGameUIMan->GetStringFromTable(8350+iUsage-1));
					if (strKey.IsEmpty())
						continue;
					
					if (!file.GetValueAsIntArray(strSect, strKey, sizeof(aVals)/sizeof(aVals[0]), aVals))
						continue;
					
					dwHotKey = aVals[0];
					DecompressHotKey(dwHotKey, nHotKey, dwModifier);
					RegisterHotKey(iUsage, nHotKey, dwModifier);
				}
				
				loaded = true;
				break;
			}
			file.Close();
		}
		if (!loaded)
			a_LogOutput(1, "CECHostInputFilter::LoadHotKey, failed to load hotkey from file [%s]", (const char *)strFile);
	}

	if (!loaded)
	{
		// Load all default hot key
		DefaultHotKey();
	}
	else
	{
		// Register usage missed in .key file (typically when new usage is added)
		DefaultHotKeyImpl(false, false);
	}
}

void CECHostInputFilter::SaveHotKey()
{
	// Save hot key to file
	//
	
	// Use ingame.stf to store hot key name (also description)
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (!pGameUIMan)
		return;

	AString strFile;
	if (!GetHotKeyFileName(strFile))
		return;

	AIniFile file;
	
	//	Write info section
	AString strSect = "Info";
	file.WriteIntValue(strSect, "version", HOTKEYFILE_VERSION);
	
	//	Write key map section
	strSect = "Map";
	int nHotKey(0);
	DWORD dwModifier(0);
	DWORD dwHotKey(0);
	AString strKey, strValue;
	for (int iUsage=1; iUsage<NUM_LKEY; iUsage++)
	{
		if (!IsUsageCustomizable(iUsage))
			continue;

		strKey = AC2AS(pGameUIMan->GetStringFromTable(8350+iUsage-1));
		if (strKey.IsEmpty())
			continue;

		if (!FindUsageHotKey(iUsage, nHotKey, dwModifier))
			continue;

		dwHotKey = CompressHotKey(nHotKey, dwModifier);
		strValue.Format("%d", dwHotKey);

		file.WriteStringValue(strSect, strKey, strValue);
	}
	
	if (!file.Save(strFile))
		a_LogOutput(1, "CECHostInputFilter::SaveHotKey, failed to save hotkey to file [%s]", (const char *)strFile);

	file.Close();
}

void CECHostInputFilter::DefaultHotKey()
{
	DefaultHotKeyImpl(false, true);
}

void CECHostInputFilter::DefaultHotKeyImpl(bool bReservedOnly, bool bReset)
{
	// Load default hot key
	// bReservedOnly=true, touch only usage not customizable
	// bReservedOnly=false, touch all usage
	// bReset=true, clear all old settings
	// bReset=false, register empty usage setting(when hot key is valid)
	
	if (bReset)
	{
		// Clear old
		m_hotKeySettings.clear();
		m_hotKeyUsages.clear();
	}

	int   nHotKey(0);
	DWORD dwModifier(0);
	
#define REGISTER_HOT_KEY(usage, key, modifier) \
	if ((!bReservedOnly || !IsUsageCustomizable(usage)) && (bReset || !FindUsageHotKey(usage, nHotKey, dwModifier)))\
		RegisterHotKey((usage), (key), (modifier))

	// Register default
	REGISTER_HOT_KEY(LKEY_PUSHCAMERA, VK_SUBTRACT, 0);
	REGISTER_HOT_KEY(LKEY_PULLCAMERA, VK_ADD, 0);
	REGISTER_HOT_KEY(LKEY_PLAYTRICK, 'X', 0);

	REGISTER_HOT_KEY(LKEY_UI_SHOW_ALL, 'H', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_MAP, 'M', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_CHARACTER, 'C', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_INVENTORY, 'B', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_SKILL, 'R', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_PET, 'P', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_QUEST, 'Q', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_QSHOP, 'O', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_ACTION, 'E', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_TEAM, 'T', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_FRIEND, 'F', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_FACTION, 'G', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_HELP, 'L', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_CONSOLE, VK_OEM_3, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_GM, 'G', EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_SYS, 'U', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_CAMERA, VK_F9, 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_QUICKKEY, 'K', 0);
	
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_CHANGECONTENT, VK_OEM_3, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC1, '1', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC2, '2', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC3, '3', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC4, '4', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC5, '5', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC6, '6', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC7, '7', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC8, '8', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK9_SC9, '9', 0);
	
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_CHANGECONTENT, 'V', 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC1, VK_F1, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC2, VK_F2, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC3, VK_F3, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC4, VK_F4, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC5, VK_F5, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC6, VK_F6, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC7, VK_F7, 0);
	REGISTER_HOT_KEY(LKEY_UI_QUICK8_SC8, VK_F8, 0);
	
	REGISTER_HOT_KEY(LKEY_UI_CHAT_CLEAR, 'D', EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_CHAT_REPLY, 'Z', EC_KSF_CONTROL|EC_KSF_ALT);
	
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT1, '1', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT2, '2', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT3, '3', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT4, '4', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT5, '5', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT6, '6', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT7, '7', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT8, '8', EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_TEAM_SELECT9, '9', EC_KSF_SHIFT);
	
	REGISTER_HOT_KEY(LKEY_UI_PET_ATTACK, '1', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL1, '2', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL2, '3', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL3, '4', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL4, '5', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL5, '6', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL6, '7', EC_KSF_ALT);
	REGISTER_HOT_KEY(LKEY_UI_PET_SKILL7, '8', EC_KSF_ALT);
	
	REGISTER_HOT_KEY(LKEY_UI_SWITCH_SPEEDSETTING, VK_F9, EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_MONSTER_AUTOSELECT, VK_TAB, 0);
	
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL1, VK_F1, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL2, VK_F2, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL3, VK_F3, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL4, VK_F4, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL5, VK_F5, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL6, VK_F6, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL7, VK_F7, EC_KSF_SHIFT);
	REGISTER_HOT_KEY(LKEY_UI_GOBLIN_SKILL8, VK_F8, EC_KSF_SHIFT);

	REGISTER_HOT_KEY(LKEY_UI_SHOW_AUTOHPMP, 'H', 0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_AUTOHELPPOP, 'I',0);
	REGISTER_HOT_KEY(LKEY_UI_SHOW_AUTOPOLICY, VK_F12, EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_SWITCH_AUTOPOLICY, VK_F12, 0);
	
	REGISTER_HOT_KEY(LKEY_UI_SYSMODEULE_SC1,'1', EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_SYSMODEULE_SC2,'2', EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_SYSMODEULE_SC3,'3', EC_KSF_CONTROL);
	REGISTER_HOT_KEY(LKEY_UI_SYSMODEULE_SC4,'4', EC_KSF_CONTROL);
	

	REGISTER_HOT_KEY(LKEY_MOVE_LEFT, 'A', 0);
	REGISTER_HOT_KEY(LKEY_MOVE_RIGHT, 'D', 0);
	REGISTER_HOT_KEY(LKEY_MOVE_FORWARD, 'W', 0);
	REGISTER_HOT_KEY(LKEY_MOVE_BACKWARD, 'S', 0);
	REGISTER_HOT_KEY(LKEY_MOVE_UP, VK_UP, 0);
	REGISTER_HOT_KEY(LKEY_MOVE_DOWN, VK_DOWN, 0);
	REGISTER_HOT_KEY(LKEY_MOVE_DOWN2, 'Z', 0);

#undef REGISTER_HOT_KEY
}

bool CECHostInputFilter::RegisterHotKey(int iUsage, int nHotKey, DWORD dwModifier)
{
	if (IsHotKeyValid(nHotKey, dwModifier) && !IsHotKeyUsed(nHotKey, dwModifier))
	{
		// Delete old usage to keep consistence in both map
		DeleteHotKey(iUsage);

		// Set new usage
		DWORD dwHotKey = CompressHotKey(nHotKey, dwModifier);
		m_hotKeySettings[iUsage] = dwHotKey;
		m_hotKeyUsages[dwHotKey] = iUsage;
		return true;
	}
	return false;
}

bool CECHostInputFilter::CustomizeHotKey(int iUsage, int nHotKey, DWORD dwModifier)
{
	// Set new usage hot key by user
	// Demand usage is customizable
	// Demand hot key is not used
	//
	return IsUsageCustomizable(iUsage)
		&& RegisterHotKey(iUsage, nHotKey, dwModifier);
}

bool CECHostInputFilter::IsExistEmptyCustomizeHotKey()
{
	bool result(false);
	
	ACString strDesc;
	for (int iUsage=1; iUsage<NUM_LKEY; ++iUsage)
	{
		if (IsUsageCustomizable(iUsage))
		{
			GetUsageShowDescription(iUsage, strDesc, false);
			if (strDesc.IsEmpty())
			{
				result = true;
				break;
			}
		}
	}
	
	return result;
}

bool CECHostInputFilter::IsCustomizeHotKeyChanged()
{
	bool result(false);
	if (IsInCustomize())
	{
		int nHotKey(0);
		DWORD dwModifier(0);

		// Get current usage and modifier into an array
		//
		abase::vector<int> usageArray = m_backupUsageCustomize;
		abase::vector<DWORD> modifierArray = m_backupHotKeyCustomize;
		BackupCustomize();
		usageArray.swap(m_backupUsageCustomize);
		modifierArray.swap(m_backupHotKeyCustomize);

		// Compare both result
		if (usageArray.size() != m_backupUsageCustomize.size())
		{
			result = true;
		}
		else
		{
			size_t count = usageArray.size();
			size_t i(0);
			for (i = 0; i<count; ++i)
			{
				if (m_backupUsageCustomize[i] != usageArray[i] ||
					m_backupHotKeyCustomize[i] != modifierArray[i])
				{
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

void CECHostInputFilter::DeleteHotKey(int iUsage)
{
	HotKeySettings::iterator it = m_hotKeySettings.find(iUsage);
	if (it != m_hotKeySettings.end())
	{
		DWORD dwHotKey = it->second;
		m_hotKeySettings.erase(it);
		
		HotKeyUsages::iterator it2 = m_hotKeyUsages.find(dwHotKey);
		if (it2 != m_hotKeyUsages.end())
			m_hotKeyUsages.erase(it2);
		else
			ASSERT(false);
	}
}

void CECHostInputFilter::DeleteHotKeyCustomize(int iUsage)
{
	// Delete hot key customizable
	//
	if (IsInCustomize() && IsUsageCustomizable(iUsage))
		DeleteHotKey(iUsage);
}

void CECHostInputFilter::SetCurrentCustomize(int iUsage, bool discardWhenFinish/* =false */)
{
	// Begin/Set/Finish customize
	// Parameter iUsage set to NON-Zero to begin customize, start next customize, set to zero to finish
	// Parameter discardWhenFinish used to cancel customize option when user canceled
	//
	if (!iUsage)
	{
		// Finish customize
		if (m_iUsageCustomize)
		{
			// Customize may happened

			if (discardWhenFinish)
			{
				// Cancel customize since begin
				RestoreCustomizeBackup();
			}
			else
			{
				// Save customize to file
				SaveHotKey();
			}
			
			// Reset customize state to NONE
			m_iUsageCustomize = 0;
			ClearCustomizeBackup();
		}
		return;
	}

	if (!IsUsageCustomizable(iUsage))
		return;

	if (!m_iUsageCustomize)
	{
		// First time enter

		// Begin customize state
		BackupCustomize();
	}

	m_iUsageCustomize = iUsage;
}

bool CECHostInputFilter::IsInCustomize()
{
	return m_iUsageCustomize != 0;
}

void CECHostInputFilter::BackupCustomize()
{
	m_backupUsageCustomize.clear();
	m_backupHotKeyCustomize.clear();

	HotKeySettings::iterator it = m_hotKeySettings.begin();
	while (it != m_hotKeySettings.end())
	{
		if (IsUsageCustomizable(it->first))
		{
			m_backupUsageCustomize.push_back(it->first);
			m_backupHotKeyCustomize.push_back(it->second);
		}
		++ it;
	}
}

void CECHostInputFilter::RestoreCustomizeBackup()
{
	int iUsage(0);

	// Clear all customizable usage
	for (iUsage=1; iUsage<NUM_LKEY; ++iUsage)
		DeleteHotKeyCustomize(iUsage);

	// Restore old backup
	DWORD dwHotKey(0);
	int nHotKey(0);
	DWORD dwModifier(0);
	int nBackup = (int)m_backupUsageCustomize.size();
	for (int i=0; i<nBackup; ++i)
	{
		iUsage = m_backupUsageCustomize[i];
		dwHotKey = m_backupHotKeyCustomize[i];
		DecompressHotKey(dwHotKey, nHotKey, dwModifier);
		CustomizeHotKey(iUsage, nHotKey, dwModifier);
	}
}

void CECHostInputFilter::ClearCustomizeBackup()
{
	abase::vector<int> temp;
	m_backupUsageCustomize.swap(temp);

	abase::vector<DWORD> temp2;
	m_backupHotKeyCustomize.swap(temp2);
}

bool CECHostInputFilter::IsHotKeyValid(int nHotKey, DWORD dwModifier)
{
	// Some hot keys cannot be used for any usage
	//
	return nHotKey != 0
		&& nHotKey != VK_ESCAPE
		&& nHotKey != VK_SPACE
		&& nHotKey != VK_CAPITAL
		&& nHotKey != VK_BACK
		&& nHotKey != VK_PRINT
		&& nHotKey != VK_INSERT
		&& nHotKey != VK_DELETE
		&& nHotKey != VK_PAUSE
		&& nHotKey != VK_SCROLL
		&& nHotKey != VK_NUMLOCK
		&& nHotKey != VK_RETURN
		&& nHotKey != VK_CONTROL
		&& nHotKey != VK_SHIFT
		&& nHotKey != VK_MENU
		&& (nHotKey != 'Y' || (dwModifier&EC_KSF_CTRLKEYS)!=0)
		&& (nHotKey != 'N' || (dwModifier&EC_KSF_CTRLKEYS)!=0);
}

bool CECHostInputFilter::IsHotKeyUsed(int nHotKey, DWORD dwModifier, int *iUsage /*=NULL*/)
{
	// Find whether given hot key is occupied by any usage
	//
	int usage = FindHotKeyUsage(nHotKey, dwModifier);
	if (!usage)
		usage = FindHotKeyUsageReserved(nHotKey, dwModifier);
	if (iUsage)
		*iUsage = usage;
	return usage!=0;
}

DWORD CECHostInputFilter::CompressHotKey(int nHotKey, DWORD dwModifier)
{
	return (nHotKey << 16) | (dwModifier & EC_KSF_CTRLKEYS);
}

void CECHostInputFilter::DecompressHotKey(DWORD dwHotKey, int &nHotKey, DWORD &dwModifyer)
{
	nHotKey = (int)(dwHotKey>>16);
	dwModifyer = (dwHotKey & EC_KSF_CTRLKEYS);
}

int CECHostInputFilter::FindHotKeyUsage(int nHotKey, DWORD dwModifyer)
{
	// Find hot key usage set by user

	int iUsage(0);
	
	DWORD dwHotKey = CompressHotKey(nHotKey, dwModifyer);
	HotKeyUsages::iterator it = m_hotKeyUsages.find(dwHotKey);
	if (it != m_hotKeyUsages.end())
		iUsage = it->second;

	return iUsage;
}

int CECHostInputFilter::FindHotKeyUsageReserved(int nHotKey, DWORD dwModifyer)
{
	// Find hot key usage reserved by the system
	//
	int iUsage(0);
	
	switch (nHotKey)
	{
	case VK_SPACE:
		iUsage = LKEY_JUMP;
		break;
		
	case VK_ESCAPE:
		iUsage = LKEY_CANCEL;
		break;
	}

	return iUsage;
}

bool CECHostInputFilter::FindUsageHotKey(int iUsage, int &nHotKey, DWORD &dwModifier)
{
	// Get user settings for given usage
	//
	bool result(false);
	if (iUsage)
	{
		HotKeySettings::iterator it = m_hotKeySettings.find(iUsage);
		if (it != m_hotKeySettings.end())
		{
			DWORD dwHotKey = it->second;
			DecompressHotKey(dwHotKey, nHotKey, dwModifier);
			result = nHotKey>0;
		}
	}
	return result;
}

bool CECHostInputFilter::FindUsageHotKeyReserved(int iUsage, int &nHotKey, DWORD &dwModifier)
{
	bool result(false);

	switch (iUsage)
	{
	case LKEY_JUMP:
		nHotKey = VK_SPACE;
		dwModifier = 0;
		result = true;
		break;
		
	case LKEY_CANCEL:
		nHotKey = VK_ESCAPE;
		dwModifier = 0;
		result = true;
		break;
	}

	return result;
}

bool CECHostInputFilter::IsUsagePressed(int iUsage)
{
	int nHotKey(0);
	DWORD dwModifier(0);
	return FindUsageHotKey(iUsage, nHotKey, dwModifier)
		&& IsHotKeyPressed(nHotKey, dwModifier)
		|| FindUsageHotKeyReserved(iUsage, nHotKey, dwModifier)
		&& IsHotKeyPressed(nHotKey, dwModifier);
}

bool CECHostInputFilter::IsMoveUsagePressed(){
	return IsUsagePressed(LKEY_MOVE_LEFT)
		|| IsUsagePressed(LKEY_MOVE_RIGHT)
		|| IsUsagePressed(LKEY_MOVE_FORWARD)
		|| IsUsagePressed(LKEY_MOVE_BACKWARD)
		|| IsUsagePressed(LKEY_MOVE_UP)
		|| IsUsagePressed(LKEY_MOVE_DOWN)
		|| IsUsagePressed(LKEY_MOVE_DOWN2)
		|| IsHotKeyPressed(VK_SPACE, 0);
}

bool CECHostInputFilter::IsHotKeyPressed(int nHotKey, DWORD dwModifier)
{
	bool pressed(false);

	while (nHotKey)
	{
		// Test hot key
		if (!m_pInputCtrl->KeyIsBeingPressed(nHotKey))
			break;

		// Test modifier
		if (dwModifier & EC_KSF_CONTROL)
		{
			if (!m_pInputCtrl->KeyIsBeingPressed(VK_CONTROL))
				break;
		}
		if (dwModifier & EC_KSF_SHIFT)
		{
			if (!m_pInputCtrl->KeyIsBeingPressed(VK_SHIFT))
				break;
		}
		if (dwModifier & EC_KSF_ALT)
		{
			if (!m_pInputCtrl->KeyIsBeingPressed(VK_MENU))
				break;
		}

		pressed = true;
		break;
	}

	return pressed;
}

bool CECHostInputFilter::IsUsageCustomizable(int iUsage)
{
	// Some usage reserved for system
	// Other usage reserved for special usage
	//
	bool result = false;
	
	while (true)
	{
		if (iUsage<=0 || iUsage>=NUM_LKEY)
			break;
		
		// Check reserved key
		if (iUsage == LKEY_CANCEL ||
			iUsage == LKEY_JUMP)
			break;
		
		if (iUsage == LKEY_UI_SHOW_BBS ||
			iUsage == LKEY_UI_SHOW_CONSOLE ||
			iUsage == LKEY_UI_SHOW_GM)
			break;

		if (iUsage == LKEY_MOVE_AUTOFORWARD ||
			iUsage == LKEY_MOVE_FOLLOW)
			break;
		
		if (iUsage == LKEY_MOVE_UP ||
			iUsage == LKEY_MOVE_DOWN)
			break;

		result = true;
		break;
	}

	return result;
}

bool CECHostInputFilter::GetUsageShowDescription(int iUsage, ACString &strDesc, bool simple)
{
	// Get customizable usage description
	//
	bool result = false;

	while (true)
	{
		if (!IsUsageCustomizable(iUsage))
			break;

		// Set default to empty because some usage may not be registered yet
		strDesc.Empty();

		// Search
		int nHotKey(0);
		DWORD dwModifier(0);
		if (FindUsageHotKey(iUsage, nHotKey, dwModifier))
			strDesc = GetHotKeyDescription(nHotKey, dwModifier, simple);

		result = true;
		break;
	}

	return result;
}

ACString CECHostInputFilter::GetHotKeyDescription(int nHotKey, DWORD dwModifier, bool simple)
{
	ACString strCtrl;
	
	if (dwModifier & EC_KSF_SHIFT)
		strCtrl += simple ? _AL("S+") : _AL("Shift+");
	if (dwModifier & EC_KSF_CONTROL)
		strCtrl += simple ? _AL("C+") : _AL("Ctrl+");
	if (dwModifier & EC_KSF_ALT)
		strCtrl += simple ? _AL("A+") : _AL("Alt+");

	ACString strText;
	if( nHotKey >= 'A' && nHotKey <= 'Z' || nHotKey >= '0' && nHotKey <= '9')
		strText.Format(_AL("%c"), nHotKey);
	else if( nHotKey >= VK_F1 && nHotKey <= VK_F24 )
		strText.Format(_AL("F%d"), nHotKey - VK_F1 + 1);
	else if( nHotKey >= VK_NUMPAD0 && nHotKey <= VK_NUMPAD9 )
		strText.Format(_AL("N%d"), nHotKey - VK_NUMPAD0);
	{
		switch(nHotKey)
		{
		case VK_BACK:
			strText = simple ? _AL("BK") : _AL("Bac");
			break;

		case VK_TAB:
			strText = simple ? _AL("TB") : _AL("Tab");
			break;

		case VK_CAPITAL:
			strText = simple ? _AL("CP") : _AL("Cap");
			break;

		case VK_RETURN:
			strText = simple ? _AL("ET") : _AL("Ent");
			break;

		case VK_PAUSE:
			strText = simple ? _AL("PA") : _AL("Pau");
			break;

		case VK_SPACE:
			strText = simple ? _AL("SP") : _AL("Spa");
			break;

		case VK_PRIOR:
			strText = _AL("PU");
			break;

		case VK_NEXT:
			strText = _AL("PD");
			break;

		case VK_END:
			strText = simple ? _AL("ED") : _AL("End");
			break;

		case VK_HOME:
			strText = simple ? _AL("HM") : _AL("Hom");
			break;

		case VK_LEFT:
			strText = _AL("←");
			break;

		case VK_UP:
			strText = _AL("↑");
			break;

		case VK_RIGHT:
			strText = _AL("→");
			break;

		case VK_DOWN:
			strText = _AL("↓");
			break;

		case VK_SCROLL:
			strText = simple ? _AL("SC") : _AL("Scr");
			break;

		case VK_SNAPSHOT:
			strText = simple ? _AL("PR") : _AL("Pri");
			break;

		case VK_INSERT:
			strText = simple ? _AL("IN") : _AL("Ins");
			break;

		case VK_DELETE:
			strText = simple ? _AL("DL") : _AL("Del");
			break;

		case VK_NUMLOCK:
			strText = _AL("NL");
			break;

		case VK_MULTIPLY:
			strText = _AL("N*");
			break;

		case VK_ADD:
			strText = _AL("N+");
			break;

		case VK_SEPARATOR:
			strText = _AL("NE");
			break;

		case VK_SUBTRACT:
			strText = _AL("N-");
			break;

		case VK_DECIMAL:
			strText = _AL("N.");
			break;

		case VK_DIVIDE:
			strText = _AL("N/");
			break;

		case VK_OEM_1:
			strText = _AL(";");
			break;

		case VK_OEM_PLUS:
			strText = _AL("=");
			break;

		case VK_OEM_COMMA:
			strText = _AL(",");
			break;

		case VK_OEM_MINUS:
			strText = _AL("-");
			break;

		case VK_OEM_PERIOD:
			strText = _AL(".");
			break;

		case VK_OEM_2:
			strText = _AL("/");
			break;

		case VK_OEM_3:
			strText = _AL("`");
			break;

		case VK_OEM_4:
			strText = _AL("[");
			break;

		case VK_OEM_5:
			strText = _AL("\\");
			break;

		case VK_OEM_6:
			strText = _AL("]");
			break;

		case VK_OEM_7:
			strText = _AL("\"");
			break;

		}
	}

	return strCtrl+strText;
}

//	Translate input
bool CECHostInputFilter::TranslateInput(int iInput)
{
	if( !g_pGame->IsActive() )
		return true;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost || !pHost->HostIsReady())
		return false;

	if (iInput == IT_KEYBOARD)
		return TranslateKeyboard(pHost);
	else if (iInput == IT_MOUSE)
		return TranslateMouse(pHost);

	return false;
}

//	Translate keyboard input
bool CECHostInputFilter::TranslateKeyboard(CECHostPlayer* pHost)
{
	if (m_iUsageCustomize)
	{
		// Customize settings
		int iNumMsg = m_pInputCtrl->GetWinKeyMsgNum();
		for (int i=0; i < iNumMsg; i++)
		{
			const CECInputCtrl::WINMSG& Msg = m_pInputCtrl->GetWinKeyMsg(i);
			if (Msg.uMsg == WM_KEYDOWN || Msg.uMsg == WM_SYSKEYDOWN)
			{
				int nHotKey = Msg.wParam;
				DWORD dwModifier = (Msg.dwCtrlkeys & EC_KSF_CTRLKEYS);

				CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUIMan && pGameUIMan->CustomizeHotKey(m_iUsageCustomize, nHotKey, dwModifier))
					break;
			}
		}
		return true;
	}

	TranslateKeyboardMove(pHost);

	int iNumMsg = m_pInputCtrl->GetWinKeyMsgNum();
	for (int i=0; i < iNumMsg; i++)
	{
		const CECInputCtrl::WINMSG& Msg = m_pInputCtrl->GetWinKeyMsg(i);
		if (Msg.uMsg == WM_KEYDOWN || Msg.uMsg == WM_SYSKEYDOWN)
			OnKeyDown(pHost, Msg.wParam, Msg.dwCtrlkeys);
		else
			OnKeyUp(pHost, Msg.wParam, Msg.dwCtrlkeys);
	}

	return true;
}

void CECHostInputFilter::TranslateKeyboardMove(CECHostPlayer *pHost)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	
	int iMove1=-1, iMove2=-1;
	int iYaw = 0;
	
	if (IsUsagePressed(LKEY_MOVE_UP))
		iMove1 = 8;
	else if (IsUsagePressed(LKEY_MOVE_DOWN) || IsUsagePressed(LKEY_MOVE_DOWN2))
		iMove1 = 9;
	
	if (pHost->IsUnderWater() || pHost->IsFlying())
	{
		if (IsUsagePressed(LKEY_MOVE_UP) || IsUsagePressed(LKEY_JUMP))
		{
			if (pHost->IsFlying())
				iMove1 = 8;
			else if (!pHost->CanTakeOffWater())
				iMove1 = 8;
		}
		else
		{
			_JumpTime = 0;
			
			if (IsUsagePressed(LKEY_MOVE_DOWN) || IsUsagePressed(LKEY_MOVE_DOWN2))
				iMove1 = 9;
		}
	}
	
	if (!g_pGame->GetConfigs()->GetGameSettings().bTurnaround)
	{
		if (IsUsagePressed(LKEY_MOVE_FORWARD))
		{
			if (IsUsagePressed(LKEY_MOVE_LEFT))
				iMove2 = 1;
			else if (IsUsagePressed(LKEY_MOVE_RIGHT))
				iMove2 = 7;
			else
				iMove2 = 0;
		}
		else if (IsUsagePressed(LKEY_MOVE_BACKWARD))
		{
			if (IsUsagePressed(LKEY_MOVE_LEFT))
				iMove2 = 3;
			else if (IsUsagePressed(LKEY_MOVE_RIGHT))
				iMove2 = 5;
			else
				iMove2 = 4;
		}
		else if (IsUsagePressed(LKEY_MOVE_LEFT))
			iMove2 = 2;
		else if (IsUsagePressed(LKEY_MOVE_RIGHT))
			iMove2 = 6;
	}
	else
	{
		if (IsUsagePressed(LKEY_MOVE_FORWARD))
			iMove2 = 0;
		else if (IsUsagePressed(LKEY_MOVE_BACKWARD))
			iMove2 = 4;
		
		if (IsUsagePressed(LKEY_MOVE_LEFT))
			iYaw = 1;
		else if (IsUsagePressed(LKEY_MOVE_RIGHT))
			iYaw = -1;
	}
	
	if (iMove1 >= 0)
		pGameRun->PostMessage(MSG_HST_PUSHMOVE, MAN_PLAYER, 0, iMove1);
	
	if (iMove2 >= 0)
		pGameRun->PostMessage(MSG_HST_PUSHMOVE, MAN_PLAYER, 0, iMove2);
	
	if (iYaw)
	{
		int iOffset = (int)(g_pGame->GetRealTickTime() * 0.001f * 400.0f * iYaw);
		pGameRun->PostMessage(MSG_HST_YAW, MAN_PLAYER, 0, iOffset);
	}

	if( iMove1 >= 0 || iMove2 >= 0 || iYaw )
	{
		if( CECAutoPolicy::GetInstance().IsAutoPolicyEnabled() )
			CECAutoPolicy::GetInstance().StopPolicy();
	}
}

void CECHostInputFilter::OnKeyDown(CECHostPlayer* pHost, int iKey, DWORD dwFlags)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	// 连续技需要占用Q、E、R三个键位
	CECGameUIMan *pGameUIMan = pGameRun->GetUIManager()->GetInGameUIMan();
	if (pGameUIMan) {
		if ('Q' == iKey || 'E' == iKey || 'R' == iKey) {
			CDlgComboShortCutBar* pComboSkill = (CDlgComboShortCutBar*)pGameUIMan->GetDialog("Win_ComboSkill");
			if (pComboSkill->IsUsingQuickKey()) {
				if (pComboSkill->IsShow()) {
					pComboSkill->CastSkill(iKey);
				}
				return;
			} else {
				CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
				CECHPWorkSpell* pWorkSpell = dynamic_cast<CECHPWorkSpell*>(pHost->GetWorkMan()->GetRunningWork(CECHPWork::WORK_SPELLOBJECT));
				if (pWorkSpell && pWorkSpell->GetSkill()) {
					if (CECComboSkillState::Instance().IsComboPreSkill(pWorkSpell->GetSkill()->GetSkillID())) {
						return;
					}
				}	
			}
		}
	}	


	// Translate to logic keys
	int iUsage = FindHotKeyUsageReserved(iKey, dwFlags);
	if (!iUsage)
		iUsage = FindHotKeyUsage(iKey, dwFlags);
	if (!iUsage)
		return;

	bool bFirstPress = m_pInputCtrl->IsFirstPressed(dwFlags);

	// Apply usage for UI
	if (iUsage>=LKEY_UI_BEGIN && iUsage<=LKEY_UI_END)
	{
		CECGameUIMan *pGameUIMan = pGameRun->GetUIManager()->GetInGameUIMan();
		if (pGameUIMan)
			pGameUIMan->OnHotKeyDown(iUsage, bFirstPress);
		return;
	}

	bool bInAutoMode = CECAutoPolicy::GetInstance().IsAutoPolicyEnabled();

	// Apply usage for others
	switch (iUsage)
	{
	case LKEY_CANCEL:
		if( bInAutoMode ) break;
		if (bFirstPress)
			pGameRun->PostMessage(MSG_HST_PRESSCANCEL, MAN_PLAYER, 0);
		break;

	case LKEY_JUMP:
		if( bInAutoMode ) break;
		if (pHost->IsJumpInWater() || pHost->IsFlying())
			break;

		if (pHost->IsUnderWater())
		{
			if (!pHost->CanTakeOffWater())
				break;
			else if (_JumpTime == 0)
			{
				_JumpTime = ::GetTickCount();
				break;
			}
			else if (CECTimeSafeChecker::ElapsedTimeFor(_JumpTime) < 1000)
				break;
			else
				_JumpTime = 0;
		}

		pGameRun->PostMessage(MSG_HST_JUMP, MAN_PLAYER, 0);
		break;

	case LKEY_PLAYTRICK:
		if( bInAutoMode ) break;
		pGameRun->PostMessage(MSG_HST_PLAYTRICK, MAN_PLAYER, 0);
		break;

	case LKEY_PUSHCAMERA:
	case LKEY_PULLCAMERA:
		{
			int iDelta = (int)(g_pGame->GetRealTickTime() * 0.001f * 120.0f);
			pGameRun->PostMessage(MSG_HST_WHEELCAM, MAN_PLAYER, 0, iUsage == LKEY_PUSHCAMERA ? -iDelta : iDelta);
			break;
		}
	}
}

void CECHostInputFilter::OnKeyUp(CECHostPlayer* pHost, int iKey, DWORD dwFlags)
{
	// Translate to logic keys
	int iUsage = FindHotKeyUsageReserved(iKey, dwFlags);
	if (!iUsage)
		iUsage = FindHotKeyUsage(iKey, dwFlags);
	if (!iUsage)
		return;

	// Apply usage
}

//	Translate mouse input
bool CECHostInputFilter::TranslateMouse(CECHostPlayer* pHost)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	int i, iNumMsg = m_pInputCtrl->GetWinMouseMsgNum();

	for (i=0; i < iNumMsg; i++)
	{
		const CECInputCtrl::WINMSG& Msg = m_pInputCtrl->GetWinMouseMsg(i);
		int x = GET_X_LPARAM(Msg.lParam);
		int y = GET_Y_LPARAM(Msg.lParam);
		DWORD dwFlags = Msg.dwCtrlkeys;

		switch (Msg.uMsg)
		{
		case WM_LBUTTONDOWN:	OnLButtonDown(pHost, x, y, dwFlags);	break;
		case WM_LBUTTONDBLCLK:	OnLButtonDblClk(pHost, x, y, dwFlags);	break;
		case WM_LBUTTONUP:		OnLButtonUp(pHost, x, y, dwFlags);		break;
		case WM_RBUTTONDOWN:	OnRButtonDown(pHost, x, y, dwFlags);	break;
		case WM_RBUTTONDBLCLK:	OnRButtonDblClk(pHost, x, y, dwFlags);	break;
		case WM_RBUTTONUP:		OnRButtonUp(pHost, x, y, dwFlags);		break;
		case WM_MOUSEMOVE:		OnMouseMove(pHost, x, y, dwFlags, Msg.wParam);	break;

		case WM_MOUSEWHEEL:
		{
			int iDelta = ((short)HIWORD(Msg.wParam));
			dwFlags = LOWORD(Msg.wParam);
			OnMouseWheel(pHost, x, y, iDelta, dwFlags);
			break;
		}
		}
	}

	if (m_LBPress.bTurnCamera || m_RBPress.bTurnCamera)
	{
		int iTurn = m_LBPress.bTurnCamera ? 1 : 0;

		if (!pHost->IsChangingFace())
		{
			if (m_TurnOffset.x) pGameRun->PostMessage(MSG_HST_YAW, MAN_PLAYER, 0, -m_TurnOffset.x, 0, iTurn);
			if (m_TurnOffset.y) pGameRun->PostMessage(MSG_HST_PITCH, MAN_PLAYER, 0, -m_TurnOffset.y, 0, iTurn);
		}

		m_TurnOffset.Set(0, 0);

		if( g_pGame->GetA3DDevice()->GetShowCursor() )
			g_pGame->ShowCursor(false);
	}
	else
	{
		if( !g_pGame->GetA3DDevice()->GetShowCursor() )
			g_pGame->ShowCursor(true);
	}

//	if (m_LBPress.bTurnCamera && m_RBPress.bTurnCamera)
//		pGameRun->PostMessage(MSG_HST_PUSHMOVE, MAN_PLAYER, 0, 0);

	return true;
}

void CECHostInputFilter::OnLButtonDown(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed		= true;
	m_LBPress.dwPressTime	= a_GetTime();
	m_LBPress.bTurnCamera	= false;
	m_LBPress.ptPress.Set(x, y);

	//	TODO: If we enable mouse mode 2, below lines should be moved
	//	to OnLButtonUp.
	if (!pHost->IsChangingFace())
		g_pGame->GetGameRun()->PostMessage(MSG_HST_LBTNCLICK, MAN_PLAYER, 0, x, y, dwFlags, 0);
}

void CECHostInputFilter::OnLButtonDblClk(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed		= true;
	m_LBPress.dwPressTime	= a_GetTime();
	m_LBPress.bTurnCamera	= false;
	m_LBPress.ptPress.Set(x, y);

	//	TODO: If we enable mouse mode 2, below lines should be moved
	//	to OnLButtonUp.
	if (!pHost->IsChangingFace())
		g_pGame->GetGameRun()->PostMessage(MSG_HST_LBTNCLICK, MAN_PLAYER, 0, x, y, dwFlags, 1);
}

void CECHostInputFilter::OnLButtonUp(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed = false;

	if (m_LBPress.bTurnCamera)
	{
		m_LBPress.bTurnCamera = false;
		m_pInputCtrl->CaptureMouse(false);
	}

	if (!pHost->IsChangingFace())
		g_pGame->GetGameRun()->PostMessage(MSG_HST_TURNCAM, MAN_PLAYER, 0, 0, 1);
}

void CECHostInputFilter::OnRButtonDown(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	if (m_pInputCtrl->IsAltPressed(dwFlags))
	{
		if (!pHost->IsChangingFace())
			g_pGame->GetGameRun()->PostMessage(MSG_HST_RBTNCLICK, MAN_PLAYER, 0, x, y, dwFlags);
	}
	else
	{
		m_ptTurnStart.Set(x, y);
		m_TurnOffset.Set(0, 0);
		m_pInputCtrl->CaptureMouse(true);

		m_RBPress.bTurnCamera = true;

		if (!pHost->IsChangingFace())
			g_pGame->GetGameRun()->PostMessage(MSG_HST_TURNCAM, MAN_PLAYER, 0, 1);
	}
}

void CECHostInputFilter::OnRButtonDblClk(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	OnRButtonDown(pHost, x, y, dwFlags);
}

void CECHostInputFilter::OnRButtonUp(CECHostPlayer* pHost, int x, int y, DWORD dwFlags)
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	if (m_RBPress.bTurnCamera)
	{
		if (!m_LBPress.bTurnCamera)
		{
			if (!pHost->IsChangingFace())
			{
				if (m_TurnOffset.x) pGameRun->PostMessage(MSG_HST_YAW, MAN_PLAYER, 0, -m_TurnOffset.x, 0);
				if (m_TurnOffset.y) pGameRun->PostMessage(MSG_HST_PITCH, MAN_PLAYER, 0, -m_TurnOffset.y, 0);
			}

			m_TurnOffset.Set(0, 0);
		}

		m_pInputCtrl->CaptureMouse(false);

		m_RBPress.bTurnCamera = false;

		if (!pHost->IsChangingFace())
			pGameRun->PostMessage(MSG_HST_TURNCAM, MAN_PLAYER, 0, 0);
	}
}

void CECHostInputFilter::OnMouseMove(CECHostPlayer* pHost, int x, int y, DWORD dwFlags, WPARAM wParam)
{
	if (m_LBPress.bTurnCamera || m_RBPress.bTurnCamera)
	{
		m_TurnOffset.x += x - m_ptTurnStart.x;
		m_TurnOffset.y += y - m_ptTurnStart.y;

		//	Restore cursor position to start position
		POINT pt = {m_ptTurnStart.x, m_ptTurnStart.y};
		::ClientToScreen(g_pGame->GetGameInit().hWnd, &pt);
		 ::SetCursorPos(pt.x, pt.y);
		 
		 //	处理鼠标 UP 与 DOWN 不同步、导致鼠标指针不显示的情况
		 if (m_LBPress.bTurnCamera && !(wParam & MK_LBUTTON)){
			 OnLButtonUp(pHost, x, y, dwFlags);
		 }
		 if (m_RBPress.bTurnCamera && !(wParam & MK_RBUTTON)){
			 OnRButtonUp(pHost, x, y, dwFlags);
		 }
	}
}

void CECHostInputFilter::OnMouseWheel(CECHostPlayer* pHost, int x, int y, int iDelta, DWORD dwFlags)
{
	if (g_pGame->GetConfigs()->GetGameSettings().bReverseWheel)
		iDelta = -iDelta;

	if (!pHost->IsChangingFace())
		g_pGame->GetGameRun()->PostMessage(MSG_HST_WHEELCAM, MAN_PLAYER, 0, iDelta);
}



