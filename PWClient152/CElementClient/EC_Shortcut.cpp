/*
 * FILE: EC_Shortcut.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Shortcut.h"
#include "EC_IvtrItem.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_GameSession.h"
#include "EC_IvtrTypes.h"
#include "EC_Resource.h"
#include "EC_Skill.h"
#include "EC_PetCorral.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "AUIDialog.h"
#include "A3DMacros.h"
#include "EC_Utility.h"
#include "EC_HostGoblin.h"
#include "DlgInventory.h"
#include "DlgSystem2.h"
#include "DlgHost.h"
#include "EC_UIConfigs.h"
#include "EC_CrossServer.h"
#include "EC_World.h"
#include "DlgAutoHPMP.h"
#include "EC_ComputerAid.h"
#include "EC_ActionSwitcher.h"

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
//	Implement CECShortcut
//	
///////////////////////////////////////////////////////////////////////////

CECShortcut::CECShortcut(const CECShortcut& src)
{
	m_iSCType = src.m_iSCType;
}

//	Clone shortcut
CECShortcut* CECShortcut::Clone()
{
	return new CECShortcut(*this);
}

//	Get icon file
const char* CECShortcut::GetIconFile()
{
	//	Return a default icon file name
	static const char szFile[] = "Iconset\\铲.bmp";
	return szFile;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCSkill
//	
///////////////////////////////////////////////////////////////////////////

CECSCSkill::CECSCSkill()
{
	m_iSCType	= SCT_SKILL;
	m_pSkill	= NULL;
}

CECSCSkill::CECSCSkill(const CECSCSkill& src) : CECShortcut(src)
{
	m_pSkill = src.m_pSkill;
}

CECSCSkill::~CECSCSkill()
{
}

//	Clone shortcut
CECShortcut* CECSCSkill::Clone()
{
	return new CECSCSkill(*this);
}

//	Initialize object
bool CECSCSkill::Init(CECSkill* pSkill)
{
	m_pSkill = pSkill;
	return true;
}

//	Execute shortcut
bool CECSCSkill::Execute()
{
	if (!m_pSkill)
		return false;
	
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	ASSERT(pHost);

	if (GNET::ElementSkill::IsGoblinSkill(m_pSkill->GetSkillID()))
	{
		int idSelected = pHost->GetSelectedTarget();
		bool bForctAttack = glb_GetForceAttackFlag(NULL);
		CECHostGoblin* pHostGoblin= (CECHostGoblin *)pHost->GetGoblinModel();
		int i;
		for (i = 0 ; i < pHostGoblin->GetSkillNum(); i++)
		{
			if (m_pSkill == pHostGoblin->GetSkill(i))
			{
				break;
			}	
		}
		if (i < pHostGoblin->GetSkillNum())
		{
			pHostGoblin->CastSkill(i, idSelected, bForctAttack);
		}
	}
	else
	{
		// 骑乘时，要放技能, 执行 action switcher，否则 ApplySkillShortcut
		if (!pHost->GetActionSwitcher() || !pHost->GetActionSwitcher()->OnRideToSkillAction(m_pSkill->GetSkillID(),false,0,-1))		
			pHost->ApplySkillShortcut(m_pSkill->GetSkillID());
	}
	return true;
}

//	Get icon file
const char* CECSCSkill::GetIconFile()
{
	return m_pSkill ? m_pSkill->GetIconFile() : "";
}

//	Get shortcut description text
const wchar_t* CECSCSkill::GetDesc()
{
	return m_pSkill ? m_pSkill->GetDesc() : _AL("");
}

//	Get item cool time
int CECSCSkill::GetCoolTime(int* piMax/* NULL */)
{
	if (!m_pSkill)
		return 0;

	if (piMax)
		*piMax = m_pSkill->GetCoolingTime();

	return m_pSkill->GetCoolingCnt();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCItem
//	
///////////////////////////////////////////////////////////////////////////

CECSCItem::CECSCItem()
{
	m_iSCType		= SCT_ITEM;
	m_iIvtr			= 0;
	m_iSlot			= 0;
	m_tidItem		= 0;
	m_bAutoFind		= false;
}

CECSCItem::CECSCItem(const CECSCItem& src) : CECShortcut(src)
{
	m_iIvtr			= src.m_iIvtr;
	m_iSlot			= src.m_iSlot;
	m_tidItem		= src.m_tidItem;
	m_strIconFile	= src.m_strIconFile;
	m_bAutoFind		= src.m_bAutoFind;
}

CECSCItem::~CECSCItem()
{
}

//	Clone shortcut
CECShortcut* CECSCItem::Clone()
{
	return new CECSCItem(*this);
}

//	Initialize object
bool CECSCItem::Init(int iIvtr, int iSlot, CECIvtrItem* pItem)
{
	m_iIvtr	= iIvtr;
	m_iSlot	= iSlot;

	if (!pItem->IsEquipment() || pItem->GetClassID() == CECIvtrItem::ICID_ARROW)
		m_bAutoFind = true;

	m_tidItem		= pItem->GetTemplateID();
	m_strIconFile	= pItem->GetIconFile();

	return true;
}

//	Get item object
CECIvtrItem* CECSCItem::GetItem()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost)
	{
		ASSERT(pHost);
		return NULL;
	}

	//	Try to get item at m_iSlot at first
	CECInventory* pPack = pHost->GetPack(m_iIvtr);
	if (!pPack)
	{
		ASSERT(pPack);
		return NULL;
	}

	return pPack->GetItem(m_iSlot);
}

//	Execute shortcut
bool CECSCItem::Execute()
{
	if (!m_tidItem)
	{
		ASSERT(0);
		return false;
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	ASSERT(pHost);

	//	Try to get item at m_iSlot at first
	CECIvtrItem* pItem = GetItem();

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgInventory *pDlgInventory = (CDlgInventory *)(pGameUI->GetDialog("Win_Inventory"));

	if (!m_bAutoFind)
	{
		// Equipment take off (or exchange) validation
		if (!pDlgInventory->ValidateEquipmentShortcut(pItem))
			return true;

		if( pItem && pItem->IsEquipment() && pHost->CanUseEquipment(((CECIvtrEquip*)pItem)) &&
			(pItem->GetProcType() & CECIvtrItem::PROC_BIND) )
		{
			PAUIDIALOG pMsgBox;
			CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->MessageBox("Game_EquipBind2", pGameUI->GetStringFromTable(872), 
				MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_iSlot);
		}
		else
			pHost->ApplyItemShortcut(m_iIvtr, m_iSlot);
		return true;
	}

	if (!pItem || pItem->GetTemplateID() != m_tidItem)
	{
		CECInventory* pPack = pHost->GetPack(m_iIvtr);

		int i, iCandidate = -1;

		//	Are there some same kind items exists ? Some items with different
		//	id but have the same effect. For example, when we counldn't find
		//	middle heal bottle, we can use small or large heal bottles as candidates.
		for (i=0; i < pPack->GetSize(); i++)
		{
			CECIvtrItem* pTempItem = pPack->GetItem(i);
			if (!pTempItem)
				continue;

			int idSlotItem = pTempItem->GetTemplateID();
			if (idSlotItem == m_tidItem)
			{
				m_iSlot = i;
				break;
			}
			else if (iCandidate < 0 && CECIvtrItem::IsCandidate(m_tidItem, pTempItem))
				iCandidate = i;
		}

		if (i >= pPack->GetSize())
		{
			if (iCandidate >= 0)
			{
				m_iSlot = iCandidate;
				UpdateItemData();
			}
			else
			{
				//	Couldn't find proper item, this shortcut should has been removed
				ASSERT(0);
				return false;
			}
		}

		// Update pItem for validation
		pItem = GetItem();
	}

	// Equipment take off (or exchange) validation
	if (!pDlgInventory->ValidateEquipmentShortcut(pItem))
		return true;

	pHost->ApplyItemShortcut(m_iIvtr, m_iSlot);

	return true;
}

//	Set new position of item referenced by this shortcut
void CECSCItem::MoveItem(int iIvtr, int iSlot)
{
	m_iIvtr = iIvtr;
	m_iSlot = iSlot;

	//	Update item associated data
	UpdateItemData();
}

//	Update item associated data after m_iIvtr or m_iSlot changed
void CECSCItem::UpdateItemData()
{
	//	Update shortcut icon file if necessary
	CECIvtrItem* pItem = GetItem();
	if (pItem)
	{
		//	If item id changes
		if (pItem->GetTemplateID() != m_tidItem)
		{
			m_tidItem		= pItem->GetTemplateID();
			m_strIconFile	= pItem->GetIconFile();
		}
	}
	else
	{
		m_tidItem = 0;
		m_strIconFile.Empty();
	}
}

//	Get shortcut description text
const wchar_t* CECSCItem::GetDesc()
{
	CECIvtrItem* pItem = GetItem();
	if (!pItem)
		return _AL("");

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	const wchar_t * pDesc = pItem->GetDesc(CECIvtrItem::DESC_NORMAL, pHost->GetEquipment());
	if (!pDesc)
	{
		pHost->GetIvtrItemDetailData(m_iIvtr, m_iSlot);
		return _AL("");
	}

	return pDesc;
}

//	Get item cool time
int CECSCItem::GetCoolTime(int* piMax/* NULL */)
{
	CECIvtrItem* pItem = GetItem();
	return pItem ? pItem->GetCoolTime(piMax) : 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCCommand
//	
///////////////////////////////////////////////////////////////////////////

CECSCCommand::CECSCCommand(int iCommand)
{
	m_iSCType	= SCT_COMMAND;
	m_iCommand	= iCommand;
	m_dwParam	= 0;
}

CECSCCommand::CECSCCommand(const CECSCCommand& src) : CECShortcut(src)
{
	m_iCommand	= src.m_iCommand;
	m_dwParam	= src.m_dwParam;
}

CECSCCommand::~CECSCCommand()
{
}

//	Clone shortcut
CECShortcut* CECSCCommand::Clone()
{
	return new CECSCCommand(*this);
}

//	Execute shortcut
bool CECSCCommand::Execute()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost || !pHost->IsAllResReady())
		return false;

	if (GetCoolTime())
	{
		bool bForbidCmd = true;
		if (m_iCommand == CMD_RUSHFLY && pHost->GetRushFlyFlag())
			bForbidCmd = false;

		if (bForbidCmd)
		{
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
			return false;
		}
	}

	switch (m_iCommand)
	{
	case CMD_SITDOWN:			pHost->CmdSitDown(!pHost->IsSitting());			break;
	case CMD_WALKRUN:			pHost->CmdWalkRun(!pHost->GetWalkRunFlag());	break;
	case CMD_NORMALATTACK:		pHost->CmdNormalAttack();						break;
	case CMD_FINDTARGET:		pHost->CmdFindTarget();							break;
	case CMD_ASSISTATTACK:		pHost->CmdAssistAttack();						break;
	case CMD_INVITETOTEAM:		pHost->CmdInviteToTeam();						break;
	case CMD_LEAVETEAM:			pHost->CmdLeaveTeam();							break;
	case CMD_KICKTEAMMEM:		pHost->CmdKickTeamMember();						break;
	case CMD_FINDTEAM:			pHost->CmdFindTeam();							break;
	case CMD_STARTTRADE:		pHost->CmdStartTrade();							break;
	case CMD_SELLBOOTH:			pHost->CmdSellBooth();							break;
	case CMD_BUYBOOTH:			pHost->CmdBuyBooth();							break;
	case CMD_PLAYPOSE:			pHost->CmdStartPose((int)m_dwParam);			break;
	case CMD_INVITETOFACTION:	pHost->CmdInviteToFaction();					break;
	case CMD_FLY:			
		{
			// 如果骑乘要飞行，则这些 action switcher，否则 CmdFly
			if(!pHost->GetActionSwitcher() || !pHost->GetActionSwitcher()->OnRideToFlyAction())
				pHost->CmdFly();
			break;
		}
	case CMD_PICKUP:			pHost->CmdPickup();								break;
	case CMD_GATHER:			pHost->CmdGather();								break;
	case CMD_RUSHFLY:			pHost->CmdRushFly();							break;
	case CMD_BINDBUDDY:			pHost->CmdBindBuddy(pHost->GetSelectedTarget());break;
	default:
		return false;
	}
	
	return true;
}

//	Get icon file
const char* CECSCCommand::GetIconFile()
{
	const char* szIconFile = "";

	switch (m_iCommand)
	{
	case CMD_SITDOWN:			szIconFile = res_IconFile(RES_ICON_CMD_SITDOWN);			break;
	case CMD_WALKRUN:			szIconFile = res_IconFile(RES_ICON_CMD_WALKRUN);			break;
	case CMD_NORMALATTACK:		szIconFile = res_IconFile(RES_ICON_CMD_NORMALATTACK);		break;
	case CMD_FINDTARGET:		szIconFile = res_IconFile(RES_ICON_CMD_FINDTARGET);			break;		
	case CMD_ASSISTATTACK:		szIconFile = res_IconFile(RES_ICON_CMD_ASSISTATTACK);		break;
	case CMD_INVITETOTEAM:		szIconFile = res_IconFile(RES_ICON_CMD_INVITETOTEAM);		break;
	case CMD_LEAVETEAM:			szIconFile = res_IconFile(RES_ICON_CMD_LEAVETEAM);			break;
	case CMD_KICKTEAMMEM:		szIconFile = res_IconFile(RES_ICON_CMD_KICKTEAMMEM);		break;
	case CMD_FINDTEAM:			szIconFile = res_IconFile(RES_ICON_CMD_FINDTEAM);			break;
	case CMD_STARTTRADE:		szIconFile = res_IconFile(RES_ICON_CMD_STARTTRADE);			break;
	case CMD_SELLBOOTH:			szIconFile = res_IconFile(RES_ICON_CMD_SELLBOOTH);			break;
	case CMD_BUYBOOTH:			szIconFile = res_IconFile(RES_ICON_CMD_BUYBOOTH);			break;
	case CMD_INVITETOFACTION:	szIconFile = res_IconFile(RES_ICON_CMD_INVITETOFACTION);	break;
	case CMD_FLY:				szIconFile = res_IconFile(RES_ICON_CMD_FLY);				break;
	case CMD_PICKUP:			szIconFile = res_IconFile(RES_ICON_CMD_PICKUP);				break;
	case CMD_GATHER:			szIconFile = res_IconFile(RES_ICON_CMD_GATHER);				break;
	case CMD_RUSHFLY:			szIconFile = res_IconFile(RES_ICON_CMD_RUSHFLY);			break;
	case CMD_BINDBUDDY:			szIconFile = res_IconFile(RES_ICON_CMD_BINDBUDDY);			break;

	case CMD_PLAYPOSE:
	{
		switch (m_dwParam)
		{
		case ROLEEXP_WAVE:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_WAVE);			break;
		case ROLEEXP_NOD:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_NOD);			break;
		case ROLEEXP_SHAKEHEAD:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_SHAKEHEAD);		break;
		case ROLEEXP_SHRUG:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_SHRUG);			break;
		case ROLEEXP_LAUGH:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_LAUGH);			break;
		case ROLEEXP_ANGRY:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_ANGRY);			break;
		case ROLEEXP_STUN:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_STUN);			break;
		case ROLEEXP_DEPRESSED:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_DEPRESSED);		break;
		case ROLEEXP_KISSHAND:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_KISSHAND);		break;
		case ROLEEXP_SHY:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_SHY);			break;
		case ROLEEXP_SALUTE:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_SALUTE);			break;
		case ROLEEXP_SITDOWN:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_SITDOWN);		break;
		case ROLEEXP_ASSAULT:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_ASSAULT);		break;
		case ROLEEXP_THINK:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_THINK);			break;
		case ROLEEXP_DEFIANCE:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_DEFIANCE);		break;
		case ROLEEXP_VICTORY:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_VICTORY);		break;
		case ROLEEXP_GAPE:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_GAPE);			break;
		case ROLEEXP_KISS:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_KISS);			break;
		case ROLEEXP_FIGHT:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_FIGHT);			break;
		case ROLEEXP_ATTACK1:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_ATTACK1);		break;
		case ROLEEXP_ATTACK2:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_ATTACK2);		break;
		case ROLEEXP_ATTACK3:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_ATTACK3);		break;
		case ROLEEXP_ATTACK4:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_ATTACK4);		break;
		case ROLEEXP_DEFENCE:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_DEFENCE);		break;	
		case ROLEEXP_FALL:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_FALL);			break;
		case ROLEEXP_FALLONGROUND:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_FALLONGROUND);	break;
		case ROLEEXP_LOOKAROUND:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_LOOKAROUND);		break;	
		case ROLEEXP_DANCE:		szIconFile = res_IconFile(RES_ICON_CMD_EXP_DANCE);			break;
		case ROLEEXP_FASHIONWEAPON:	szIconFile = res_IconFile(RES_ICON_CMD_EXP_FASHIONWEAPON);		break;
		default:
			ASSERT(0);
			break;
		}

		break;
	}
	default:
		return "";
	}

	return szIconFile;
}

//	Get shortcut description text
const wchar_t* CECSCCommand::GetDesc()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	const wchar_t* szDesc = _AL("");

	switch (m_iCommand)
	{
	case CMD_SITDOWN:			szDesc = pDescTab->GetWideString(CMDDESC_SITDOWN);			break;
	case CMD_WALKRUN:			szDesc = pDescTab->GetWideString(CMDDESC_WALKRUN);			break;
	case CMD_NORMALATTACK:		szDesc = pDescTab->GetWideString(CMDDESC_NORMALATTACK);		break;
	case CMD_FINDTARGET:		szDesc = pDescTab->GetWideString(CMDDESC_FINDTARGET);		break;
	case CMD_ASSISTATTACK:		szDesc = pDescTab->GetWideString(CMDDESC_ASSISTATTACK);		break;
	case CMD_INVITETOTEAM:		szDesc = pDescTab->GetWideString(CMDDESC_INVITETOTEAM);		break;
	case CMD_LEAVETEAM:			szDesc = pDescTab->GetWideString(CMDDESC_LEAVETEAM);		break;
	case CMD_KICKTEAMMEM:		szDesc = pDescTab->GetWideString(CMDDESC_KICKTEAMMEM);		break;
	case CMD_FINDTEAM:			szDesc = pDescTab->GetWideString(CMDDESC_FINDTEAM);			break;
	case CMD_STARTTRADE:		szDesc = pDescTab->GetWideString(CMDDESC_STARTTRADE);		break;
	case CMD_SELLBOOTH:			szDesc = pDescTab->GetWideString(CMDDESC_SELLBOOTH);		break;
	case CMD_BUYBOOTH:			szDesc = pDescTab->GetWideString(CMDDESC_BUYBOOTH);			break;
	case CMD_INVITETOFACTION:	szDesc = pDescTab->GetWideString(CMDDESC_INVITETOFACTION);	break;
	case CMD_FLY:				szDesc = pDescTab->GetWideString(CMDDESC_FLY);				break;
	case CMD_PICKUP:			szDesc = pDescTab->GetWideString(CMDDESC_PICKUP);			break;
	case CMD_GATHER:			szDesc = pDescTab->GetWideString(CMDDESC_GATHER);			break;
	case CMD_RUSHFLY:			szDesc = pDescTab->GetWideString(CMDDESC_RUSHFLY);			break;
	case CMD_BINDBUDDY:			szDesc = pDescTab->GetWideString(CMDDESC_BINDBUDDY);			break;

	case CMD_PLAYPOSE:
	{
		switch (m_dwParam)
		{
		case ROLEEXP_WAVE:			szDesc = pDescTab->GetWideString(FACEDESC_WAVEHAND);	break;
		case ROLEEXP_NOD:			szDesc = pDescTab->GetWideString(FACEDESC_NOD);			break;		
		case ROLEEXP_SHAKEHEAD:		szDesc = pDescTab->GetWideString(FACEDESC_SHADEHEAD);	break;	
		case ROLEEXP_SHRUG:			szDesc = pDescTab->GetWideString(FACEDESC_SHRUG);		break;	
		case ROLEEXP_LAUGH:			szDesc = pDescTab->GetWideString(FACEDESC_LAUGH);		break;		
		case ROLEEXP_ANGRY:			szDesc = pDescTab->GetWideString(FACEDESC_ANGRY);		break;		
		case ROLEEXP_STUN:			szDesc = pDescTab->GetWideString(FACEDESC_FAINT);		break;		
		case ROLEEXP_DEPRESSED:		szDesc = pDescTab->GetWideString(FACEDESC_SAD);			break;		
		case ROLEEXP_KISSHAND:		szDesc = pDescTab->GetWideString(FACEDESC_KISSHAND);	break;		
		case ROLEEXP_SHY:			szDesc = pDescTab->GetWideString(FACEDESC_SHY);			break;		
		case ROLEEXP_SALUTE:		szDesc = pDescTab->GetWideString(FACEDESC_SALUTE);		break;	
		case ROLEEXP_SITDOWN:		szDesc = pDescTab->GetWideString(FACEDESC_SITDOWN);		break;	
		case ROLEEXP_ASSAULT:		szDesc = pDescTab->GetWideString(FACEDESC_CHARGE);		break;	
		case ROLEEXP_THINK:			szDesc = pDescTab->GetWideString(FACEDESC_THINK);		break;		
		case ROLEEXP_DEFIANCE:		szDesc = pDescTab->GetWideString(FACEDESC_CHALLENGE);	break;	
		case ROLEEXP_VICTORY:		szDesc = pDescTab->GetWideString(FACEDESC_WIN);			break;	
		case ROLEEXP_GAPE:			szDesc = pDescTab->GetWideString(FACEDESC_GAPE);		break;
		case ROLEEXP_KISS:			szDesc = pDescTab->GetWideString(FACEDESC_KISS);		break;
		case ROLEEXP_FIGHT:			szDesc = pDescTab->GetWideString(FACEDESC_FIGHT);		break;
		case ROLEEXP_ATTACK1:		szDesc = pDescTab->GetWideString(FACEDESC_ATTACK1);		break;
		case ROLEEXP_ATTACK2:		szDesc = pDescTab->GetWideString(FACEDESC_ATTACK2);		break;
		case ROLEEXP_ATTACK3:		szDesc = pDescTab->GetWideString(FACEDESC_ATTACK3);		break;
		case ROLEEXP_ATTACK4:		szDesc = pDescTab->GetWideString(FACEDESC_ATTACK4);		break;
		case ROLEEXP_DEFENCE:		szDesc = pDescTab->GetWideString(FACEDESC_DEFENCE);		break;	
		case ROLEEXP_FALL:			szDesc = pDescTab->GetWideString(FACEDESC_FALL);		break;
		case ROLEEXP_FALLONGROUND:	szDesc = pDescTab->GetWideString(FACEDESC_FALLONGROUND);break;
		case ROLEEXP_LOOKAROUND:	szDesc = pDescTab->GetWideString(FACEDESC_LOOKAROUND);	break;	
		case ROLEEXP_DANCE:			szDesc = pDescTab->GetWideString(FACEDESC_DANCE);		break;
		case ROLEEXP_FASHIONWEAPON:	szDesc = pDescTab->GetWideString(FACEDESC_FASHIONWEAPON);break;
		default:
			ASSERT(0);
			break;
		}

		break;
	}
	default:
		return _AL("");
	}
	
	return szDesc;
}

//	Get item cool time
int CECSCCommand::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int iTime = 0;

	switch (m_iCommand)
	{
	case CMD_RUSHFLY:

		iTime = pHost->GetCoolTime(GP_CT_FLY_RUSH, piMax);
		break;

	case CMD_PLAYPOSE:

		iTime = pHost->GetCoolTime(GP_CT_EMOTE, piMax);
		break;

	case CMD_BINDBUDDY:
	{
		CECCounter& cnt = pHost->GetBindCmdCoolCnt();
		iTime = cnt.GetPeriod() - cnt.GetCounter();

		if (piMax)
			*piMax = cnt.GetPeriod();

		break;
	}
	default:

		if (piMax)	*piMax = 0;
		break;
	}

	return iTime;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCSkillGrp
//	
///////////////////////////////////////////////////////////////////////////

CECSCSkillGrp::CECSCSkillGrp()
{ 
	m_iSCType	= SCT_SKILLGRP;
	m_iGroupIdx	= -1;
}

CECSCSkillGrp::CECSCSkillGrp(const CECSCSkillGrp& src)
{
	m_iGroupIdx	= src.m_iGroupIdx;
	m_strDesc	= src.m_strDesc;
}

//	Initialize object
bool CECSCSkillGrp::Init(int iGroupIdx)
{
	m_iGroupIdx = iGroupIdx;

	CECStringTab* pStrTab = g_pGame->GetItemDesc();
	m_strDesc.Format(pStrTab->GetWideString(CMDDESC_SKILLGROUP), m_iGroupIdx);
	return true;
}

//	Clone shortcut
CECShortcut* CECSCSkillGrp::Clone()
{
	return new CECSCSkillGrp(*this);
}

//	Execute shortcut
bool CECSCSkillGrp::Execute()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	pHost->ApplyComboSkill(m_iGroupIdx);
	return true;
}

//	Get icon file
const char* CECSCSkillGrp::GetIconFile()
{
	return "";
}

//	Get shortcut description text
const wchar_t* CECSCSkillGrp::GetDesc()
{
	return m_strDesc;
}

//	Get item cool time
int CECSCSkillGrp::GetCoolTime(int* piMax/* NULL */)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCPet
//	
///////////////////////////////////////////////////////////////////////////

CECSCPet::CECSCPet()
{ 
	m_iSCType		= SCT_PET;
	m_iPetIndex		= -1;
	m_pPetEssence	= NULL;
}

CECSCPet::CECSCPet(const CECSCPet& src)
{
	m_iPetIndex		= src.m_iPetIndex;
	m_pPetEssence	= src.m_pPetEssence;
}

//	Initialize object
bool CECSCPet::Init(int iPetIndex)
{
	CECPetCorral* pPetCorral = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral();
	CECPetData* pPet = pPetCorral->GetPetData(iPetIndex);
	if (!pPet)
		return false;

	DATA_TYPE DataType;
	m_pPetEssence = (PET_ESSENCE*)g_pGame->GetElementDataMan()->get_data_ptr(pPet->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if (!m_pPetEssence)
		return false;

	m_iPetIndex = iPetIndex;

	return true;
}

//	Clone shortcut
CECShortcut* CECSCPet::Clone()
{
	return new CECSCPet(*this);
}

//	Execute shortcut
bool CECSCPet::Execute()
{
	if (m_iPetIndex < 0)
		return false;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECPetCorral* pPetCorral = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral();
	
	if (pPetCorral->GetActivePetIndex() == m_iPetIndex)
	{
		//	If this pet is the active one, deactivate it
		pHost->RecallPet();
	}
	else
	{
		// 飞行-》骑乘
		if (pHost->GetActionSwitcher() && pHost->GetActionSwitcher()->OnFlyToRideAction(m_iPetIndex))			
			return true;
		
		//	force to active this pet.
		pHost->SummonPet(m_iPetIndex);
	}

	return true;
}

//	Get icon file
const char* CECSCPet::GetIconFile()
{
	return m_pPetEssence->file_icon;
}

//	Get shortcut description text
const wchar_t* CECSCPet::GetDesc()
{
	return m_pPetEssence->name;
}

//	This pet is the active one ?
bool CECSCPet::IsActivePet()
{
	CECPetCorral* pPetCorral = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral();
	return pPetCorral->GetActivePetIndex() == m_iPetIndex;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCAutoFashion
//	
///////////////////////////////////////////////////////////////////////////

CECSCAutoFashion::CECSCAutoFashion()
{ 
	m_iSCType		= SCT_AUTOFASHION;

	m_iFashionIdx	= -1;
	strcpy(m_IconFile, "Iconset\\1.dds");
}

CECSCAutoFashion::CECSCAutoFashion(const CECSCAutoFashion& src)
{
	m_iFashionIdx	= src.m_iFashionIdx;
	strcpy(m_IconFile, src.m_IconFile);
}

//	Initialize object
bool CECSCAutoFashion::Init(int iFashionIdx)
{
	m_iFashionIdx = iFashionIdx;
	
	char temp[10];
	itoa(iFashionIdx+1, temp, 10);
	sprintf(m_IconFile, "Iconset\\%s.dds", temp);
	
	return true;
}

//	Clone shortcut
CECShortcut* CECSCAutoFashion::Clone()
{
	return new CECSCAutoFashion(*this);
}

//	Execute shortcut
bool CECSCAutoFashion::Execute()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int iRet = pHost->CheckAutoFashionCondition();
	if(iRet == 0)
	{
		pHost->EquipFashionBySuitID(m_iFashionIdx);
		return true;
	}

	return false;
}

//	Get icon file
const char* CECSCAutoFashion::GetIconFile()
{
	return m_IconFile;
}

//	Get shortcut description text
const wchar_t* CECSCAutoFashion::GetDesc()
{
	return _AL("");
}



///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSCSysModule
//	
///////////////////////////////////////////////////////////////////////////

const int CECSCSysModule::g_SysIndexMap[FM_NUM] = {0,1,2,3,4,5,6,7,28}; // EC_FixMsg.h里关于系统功能的序号被隔开了，所有映射一下

CECSCSysModule::CECSCSysModule()
{ 
	m_iSCType		= SCT_SYSMODULE;
	m_iSystem		= FM_NONE;
}

CECSCSysModule::CECSCSysModule(const CECSCSysModule& src)
{
	m_iSCType = src.m_iSCType;
	m_strDesc = src.m_strDesc;
	m_iSystem = src.m_iSystem;
}

//	Initialize object
bool CECSCSysModule::Init(int iSys)
{	
	m_iSystem = iSys;
	
	CECStringTab* pStrTab = g_pGame->GetItemDesc();

	const wchar_t* pStr = pStrTab->GetWideString(ITEMDESC_SYSMODULE_FM_GT + g_SysIndexMap[iSys]);
	if(pStr)
		m_strDesc.Format(pStr, 0);

	return true;
}

//	Clone shortcut
CECShortcut* CECSCSysModule::Clone()
{
	return new CECSCSysModule(*this);
}

//	Execute shortcut
bool CECSCSysModule::Execute()
{
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();


	switch (m_iSystem)
	{
	case FM_NONE:
		break;
	case FM_MATCH:
		{
			CDlgSystem2* pDlg = (CDlgSystem2*)pGameUI->GetDialog("Win_Main2");
			if (pDlg)
				pDlg->OnCommandMatch(NULL);
		}
		break;
	case FM_TOUCH:
		{
			if (!CECUIConfig::Instance().GetGameUI().bEnableTouch)
			{
				pGameUI->MessageBox("", pGameUI->GetStringFromTable(10931), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return false;
			}

			bool bShow = false;
			if (!CECCrossServer::Instance().IsOnSpecialServer() &&
				!g_pGame->GetConfigs()->IsMiniClient()){
				int idInstance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
				if (CECUIConfig::Instance().GetGameUI().GetCanShowTouchShop(idInstance)){
					bShow = true;
				}	
			}
			if (bShow)
			{
				AUIDialog* pTouch = pGameUI->GetDialog("Win_touchshop");
				pTouch->Show(!pTouch->IsShow());
			}
			else
			{
				pGameUI->MessageBox("", pGameUI->GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}
		}
		break;
	case FM_GT:
		{
			CDlgHost* pHostDlg = (CDlgHost*)pGameUI->GetDialog("Win_Hpmpxp");
			if(pHostDlg)
				pHostDlg->OnCommand_GT(NULL);
		}
		break;
	case FM_ROBOT:
		{
			CDlgSystem4* pDlg = (CDlgSystem4*)pGameUI->GetDialog("Win_Main4");
			if (pDlg)
				pDlg->OnCommandAutoRobot(NULL);
		}
		break;
	case FM_OFFLINESHOP:
		{
			CDlgSystem4* pDlg = (CDlgSystem4*)pGameUI->GetDialog("Win_Main4");
			if (pDlg)
				pDlg->OnCommandOfflineShop(NULL);
		}
		break;
	case FM_ADDEXP: // 聚灵
		{
			CDlgSystem5* pDlg = (CDlgSystem5*)pGameUI->GetDialog("Win_Main5");
			if (pDlg)
				pDlg->OnCommandAddExp(NULL);
		}
		break;
	case FM_WIKI:
		{
			CDlgSystem4* pDlg = (CDlgSystem4*)pGameUI->GetDialog("Win_Main4");
			if (pDlg)
				pDlg->OnCommandWiki(NULL);
		}
		break;
	case FM_BORADCAST: // 完美服务
		{
			CDlgSystem4* pDlg = (CDlgSystem4*)pGameUI->GetDialog("Win_Main4");
			if (pDlg)
				pDlg->OnCommandBroadcast(NULL);
		}
		break;
	case FM_AUTOHPMP: // 自动喝药辅助功能
		{
			if (CEComputerAid::Instance().CanUse())
			{
				CDlgAutoHPMP* pDlg = (CDlgAutoHPMP*)pGameUI->GetDialog("Win_AutoHPMP");
				if(pDlg)
					pDlg->SwitchShow();
			}			
		}
		break;
	}
	
	return false;
}

//	Get shortcut description text
const wchar_t* CECSCSysModule::GetDesc()
{
	return m_strDesc;
}

//////////////////////////////////////////////////////////////////////////