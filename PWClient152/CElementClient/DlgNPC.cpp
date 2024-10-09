// Filename	: DlgNPC.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "DlgNPC.h"
#include "DlgShop.h"
#include "DlgProduce.h"
#include "DlgInventory.h"
#include "DlgWorldMap.h"
#include "DlgFittingRoom.h"
#include "DlgGuildCreate.h"
#include "DlgGuildMan.h"
#include "DlgFaceLift.h"
#include "DlgMailList.h"
#include "DlgDamageRep.h"
#include "DlgDoubleExp.h"
#include "DlgAuctionBuyList.h"
#include "DlgEarthBagRank.h"
#include "DlgEquipRefine.h"
#include "DlgEquipDye.h"
#include "DlgEquipSlot.h"
#include "DlgEquipMark.h"
#include "DlgRefineTrans.h"
#include "DlgPetHatch.h"
#include "DlgPetRec.h"
#include "DlgPetList.h"
#include "DlgGuildMap.h"
#include "DlgGuildMapStatus.h"
#include "DlgGodEvilConvert.h"
#include "DlgGoldTrade.h"
#include "DlgWebViewProduct.h"
#include "DlgAutoLock.h"
#include "DlgWedding.h"
#include "DlgWeddingInvite.h"
#include "DlgStoneChange.h"
#include "DlgKingAuction.h"
#include "DlgKingElection.h"
#include "DlgAward.h"
#include "DlgGiftCard.h"
#include "DlgReincarnation.h"
#include "DlgGeneralCardRespawn.h"
#include "DlgELFXiDian.h"
#include "DlgELFRetrain.h"
#include "DlgELFRefine.h"
#include "DlgELFToPill.h"
#include "DlgELFGeniusReset.h"
#include "DlgELFRefineTrans.h"
#include "DlgELFEquipRemove.h"
#include "DlgELFLearn.h"
#include "DlgPetDye.h"
#include "DlgPlayerRename.h"
#include "DlgFashionSplit.h"
#include "EC_OfflineShopCtrl.h"
#include "DlgChariot.h"
#include "DlgFlySwordImprove.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "DlgTask.h"
#include "DlgSimpleCostItemService.h"

#include "EC_Global.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_TaskInterface.h"
#include "EC_Skill.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Faction.h"
#include "EC_ManNPC.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_PetCorral.h"
#include "EL_Precinct.h"
#include "EC_ForceMgr.h"
#include "EC_Team.h"
#include "EC_DomainGuild.h"
#include "EC_UIConfigs.h"
#include "EC_Utility.h"
#include "EC_UIHelper.h"
#include "EC_ProfConfigs.h"

#include "TaskTemplMan.h"
#include "Network\\ids.hxx"
#include "elementdataman.h"
#include "AUICTranslate.h"
#include "globaldataman.h"
#include "EC_CrossServer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgNPC, CDlgBase)

AUI_ON_COMMAND("back",			OnCommand_back)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgNPC, CDlgBase)

AUI_ON_EVENT("Lst_Main",		WM_LBUTTONDOWN,		OnEventLButtonDown_Lst_Main)
AUI_ON_EVENT("Lst_Main",		WM_LBUTTONUP,		OnEventLButtonUp_Lst_Main)

AUI_END_EVENT_MAP()

enum
{
	CDLGNPC_FORGET			= 0xFFFFF01,
	CDLGNPC_FACTION_CREATE	= 0xFFFFF02,
	CDLGNPC_FACTION_UPGRADE	= 0xFFFFF03,
	CDLGNPC_FACTION_DISMISS	= 0xFFFFF04,
	CDLGNPC_FACE			= 0xFFFFF05,
	CDLGNPC_MAIL			= 0xFFFFF06,
	CDLGNPC_AUCTION			= 0xFFFFF07,
	CDLGNPC_DOUBLEEXP		= 0xFFFFF08,
	CDLGNPC_PETHATCH		= 0xFFFFF09,
	CDLGNPC_PETREC			= 0xFFFFF0A,
	CDLGNPC_BATTLECHALLENGE	= 0xFFFFF0B,
	CDLGNPC_BATTLEENTER		= 0xFFFFF0C,
	CDLGNPC_BATTLELEAVE		= 0xFFFFF0D,
	CDLGNPC_GOLDTRADE		= 0xFFFFF0E,
	CDLGNPC_EQUIPREFINE		= 0xFFFFF10,
	CDLGNPC_EQUIPDYE		= 0xFFFFF11,
	CDLGNPC_REFINETRANS		= 0xFFFFF12,
	CDLGNPC_EQUIPSLOT		= 0xFFFFF13,
	CDLGNPC_ELFPRORESET	    = 0xFFFFF14,
	CDLGNPC_ELFGENIUSRESET   = 0xFFFFF15,
	CDLGNPC_ELFFORGET       = 0xFFFFF16,
	CDLGNPC_ELFREFINE       = 0xFFFFF17,
	CDLGNPC_ELFREFINETRANS  = 0xFFFFF18,
	CDLGNPC_ELFTOPILL       = 0xFFFFF19,
	CDLGNPC_ELFEQUIPOFF		= 0xFFFFF1A,
	CDLGNPC_EQUIPREPAIR     = 0xFFFFF1B,
	CDLGNPC_WEBTRADE		= 0xFFFFF1C,
	CDLGNPC_GODEVILCONVERT	= 0xFFFFF1D,
	CDLGNPC_WEDDING_BOOK	= 0xFFFFF1E,
	CDLGNPC_WEDDING_INVITE	= 0xFFFFF1F,
	CGLDNPC_FORTRESS_CREATE	= 0xFFFFF20,
	CGLDNPC_FORTRESS_WAR	= 0xFFFFF21,
	CGLDNPC_FORTRESS_UPGRADE= 0xFFFFF22,
	CGLDNPC_FORTRESS_HANDIN_MATERIAL	= 0xFFFFF23,
	CGLDNPC_FORTRESS_HANDIN_CONTRIB		= 0xFFFFF24,
	CGLDNPC_FORTRESS_EXCHANGE_MATERIAL	= 0xFFFFF25,
	CGLDNPC_FORTRESS_ENTER	= 0xFFFFF26,
	CGLDNPC_FORTRESS_WARLIST= 0xFFFFF27,
	CGLDNPC_FORTRESS_LEAVE	= 0xFFFFF28,
	CDLGNPC_PETDYE = 0xFFFFF29,
	CDLGNPC_VIEW_TRASHBOX = 0xFFFFF30,
	CDLGNPC_VIEW_DPS_DPH_RANK = 0xFFFFF31,
	CDLGNPC_FORCE_JOIN		= 0xFFFFF32,
	CDLGNPC_FORCE_QUIT		= 0xFFFFF33,
	CDLGNPC_JOIN_COUNTRY		= 0xFFFFF34,
	CDLGNPC_QUIT_COUNTRY		= 0xFFFFF35,
	CDLGNPC_LEAVE_COUNTRY_WAR	= 0xFFFFF36,
	CDLGNPC_EQUIP_MARK	=	0xFFFFF37,
	CDLGNPC_GOTO_SPECIALSERVER = 0xFFFFF38,
	CDLGNPC_GOTO_ORIGINALSERVER = 0xFFFFF39,
	CDLGNPC_PLAYER_RENAME = 0xFFFFF40,

	CDLGNPC_STONE_CHANGE = 0xFFFFF41,
	CDLGNPC_KING_SERVICE = 0xFFFFF42,
	CDLGNPC_SPLIT_FASHION = 0xFFFFF43,
	CDLGNPC_OFFLINESHOP_SETTING = 0xFFFFF44,	
	CDLGNPC_OFFLINESHOP_SELLBUY = 0xFFFFF45,
	CDLGNPC_REINCARNATION	= 0xFFFFF46,
	CDLGNPC_GIFTCARD		= 0xFFFFF47,
	CDLGNPC_TRICKBATTLE		= 0xFFFFF48, // 跨服活动, 战车
	CDLGNPC_CARDRESPAWN		= 0xFFFFF49, // 卡牌转生
	CDLGNPC_QUERYCHARIOTAMOUNT = 0xFFFFF50, // 战车数量
	CDLGNPC_FLYSWORDIMPROVE	= 0xFFFFF51, // 飞剑强化
	CDLGNPC_OPEN_FACTION_PVP= 0xFFFFF52, // 开启帮派掠夺	
	CDLGNPC_FACTION_RENAME	= 0xFFFFF53,
	CDLGNPC_GOLD_SHOP		= 0xFFFFF54,
	CDLGNPC_PLAYER_CHANGE_GENDER = 0xFFFFF55,	//	修改性别
};

CDlgNPC::CDlgNPC()
{
}

CDlgNPC::~CDlgNPC()
{
}

bool CDlgNPC::OnInitDialog()
{
	m_pTxt_npc = (PAUILABEL)GetDlgItem("Txt_npc");
	m_pLst_Main = (PAUILISTBOX)GetDlgItem("Lst_Main");
	m_pTxt_Content = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	m_pBtn_Back = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Back");
	
	m_lastNPCID = 0;
	m_bLButtonDown = false;

	m_iTracedTaskNPCID = 0;
	m_iTracedTaskID = 0;
	return true;
}

void CDlgNPC::OnCommand_back(const char * szCommand)
{
	NPC_ESSENCE *pCurNPCEssence = GetGameUIMan()->m_pCurNPCEssence;

	if( GetData() == NPC_DIALOG_TALK ||
		GetData() == NPC_DIALOG_TASK_TALK )
	{
		int i, j, nIndex;
		ACString strText;
		talk_proc *pTalk = (talk_proc *)GetDataPtr("ptr_talk_proc");
		unsigned int id = (unsigned int)m_pTxt_Content->GetData();
		CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

		if( 0xFFFFFFFF == id )		// No any more parent window.
		{
			if( GetData() == NPC_DIALOG_TALK )
				PopupNPCDialog(pCurNPCEssence);
			else if( pCurNPCEssence )
			{
				//	从任务谈话回到任务列表界面
				
				//	先弹欢迎界面，看欢迎界面是否就是任务列表界面
				PopupNPCDialog(pCurNPCEssence);
				if (GetData() != NPC_DIALOG_TASK_LIST)
				{
					//	欢迎界面不是任务列表界面，有其它内容
					//	重新弹出任务列表界面
					PopupTaskDialog(false);
					m_pLst_Main->AddString(GetStringFromTable(503));
				}
			}
		}
		else
		{
			for( i = 0; i < pTalk->num_window; i++ )
			{
				if( id != pTalk->windows[i].id ) continue;

				m_pTxt_Content->SetText(pTask->FormatTaskTalk(pTalk->windows[i].talk_text));
				m_pTxt_Content->SetData(pTalk->windows[i].id_parent);

				m_pLst_Main->ResetContent();
				for( j = 0; j < pTalk->windows[i].num_option; j++ )
				{
					strText = GetStringFromTable(249);
					strText += pTalk->windows[i].options[j].text;
					m_pLst_Main->AddString(strText);
					nIndex = m_pLst_Main->GetCount() - 1;
					m_pLst_Main->SetItemData(nIndex, pTalk->windows[i].options[j].id);
					m_pLst_Main->SetItemDataPtr(nIndex, &pTalk->windows[i].options[j]);
				}

				break;
			}
		}
	}
	else if( GetData() == NPC_DIALOG_TASK_LIST )
	{
		PopupNPCDialog(pCurNPCEssence);

		if (GetData() == NPC_DIALOG_TASK_LIST)
		{
			//	如果当前NPC只有任务相关服务，则返回等同于退出NPC服务
			Show(false);
			GetGameUIMan()->EndNPCService();
		}
	}
	else if( GetData() == NPC_DIALOG_ESSENCE )
	{
		Show(false);
		GetGameUIMan()->EndNPCService();
	}
}

void CDlgNPC::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();

	int idCurFinishTask = GetGameUIMan()->m_idCurFinishTask;
	if( GetData() == NPC_DIALOG_TASK_TALK && idCurFinishTask > 0 )
	{
		GetHostPlayer()->GetTaskInterface()->OnUIDialogEnd(idCurFinishTask);
		GetGameUIMan()->m_idCurFinishTask = -1;
	}
}

void CDlgNPC::PopupNPCDialog(const NPC_ESSENCE *pEssence)
{
	int i;
	const void *pData;
	DATA_TYPE DataType;
	ACString strText = GetStringFromTable(249);
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	
	Show(true);

	//test
//	pEssence->id_goblin_skill_service = 1;
	unsigned int a_uiService[] =
	{
		pEssence->id_talk_service,			// 交谈的服务ID
		pEssence->id_sell_service,			// 出售商品的服务ID
		pEssence->id_buy_service,			// 收购品的服务ID
		pEssence->id_install_service,		// 安装镶嵌品的服务ID
		pEssence->id_uninstall_service,		// 拆除镶嵌品的服务ID
		pEssence->id_skill_service,			// 教授技能的服务ID
		pEssence->id_heal_service,			// 治疗的服务ID
		pEssence->id_transmit_service,		// 传送的服务ID
		pEssence->id_transport_service,		// 运输的服务ID
		pEssence->id_proxy_service,			// 代售的服务ID
		pEssence->id_storage_service,		// 仓库的服务ID
		pEssence->id_make_service,			// 生产的服务ID
		pEssence->id_decompose_service,		// 分解的服务ID
		pEssence->id_identify_service,		// 鉴定的服务ID
		pEssence->id_war_towerbuild_service,// 建造箭塔的服务ID
		pEssence->id_resetprop_service,		// 洗点的服务ID
		pEssence->id_petname_service,		// 宠物改名服务
		pEssence->id_petlearnskill_service,	// 宠物学习技能服务
		pEssence->id_petforgetskill_service,// 宠物遗忘技能服务
		pEssence->id_equipbind_service,		// 装备绑定服务
		pEssence->id_equipdestroy_service,	// 装备销毁服务
		pEssence->id_equipundestroy_service,// 装备解除销毁服务
		pEssence->id_goblin_skill_service,	// 小精灵学习服务
		pEssence->id_engrave_service,		//	镌刻服务
		pEssence->id_randprop_service,		//	随机属性
		pEssence->id_force_service,			//	势力属性
	};
	
	GetGameUIMan()->m_pCurNPCEssence = (NPC_ESSENCE *)pEssence;

	SetData(NPC_DIALOG_ESSENCE);
	m_pTxt_npc->SetText(pEssence->name);
	m_pTxt_Content->SetText(pEssence->hello_msg);
	
	m_pLst_Main->SetHOver(true);
	m_pLst_Main->ResetContent();

	
	//add for test by czx
//	int old = pEssence->combined_services | 0xFFFE0000;
//	((NPC_ESSENCE *)pEssence)->combined_services = old;

	if( pEssence->combined_services & 0x01 )	// Add "forget" skill.
	{
		CECSkill *pSkill;

		for( i = 0; i < GetHostPlayer()->GetPassiveSkillNum(); i++ )
		{
			pSkill = GetHostPlayer()->GetPassiveSkillByIndex(i);
			if( pSkill->GetType() == CECSkill::TYPE_PRODUCE )
			{
				m_pLst_Main->AddString(strText + GetStringFromTable(558));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FORGET);
				m_pLst_Main->SetItemDataPtr(m_pLst_Main->GetCount() - 1, (void *)pSkill);
				break;
			}
		}
	}
	if( pEssence->combined_services & 0x02 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(885));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_EQUIPSLOT);
	}
	if( pEssence->combined_services & 0x10 )	// Add "faction" service.
	{
		if( GetHostPlayer()->GetFactionID() <= 0 )
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(592));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FACTION_CREATE);
		}
		else if( GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER )
		{
			int idFaction = GetHostPlayer()->GetFactionID();
			CECFactionMan *pFMan = GetGame()->GetFactionMan();
			Faction_Info *pInfo = pFMan->GetFaction(idFaction);

			if( pInfo && pInfo->GetLevel() <= 1 )
			{
				m_pLst_Main->AddString(strText + GetStringFromTable(593));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FACTION_UPGRADE);
			}

			m_pLst_Main->AddString(strText + GetStringFromTable(11436));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FACTION_RENAME);

			m_pLst_Main->AddString(strText + GetStringFromTable(594));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FACTION_DISMISS);
		}
	}
	if( pEssence->combined_services & 0x20 &&
		!GetHostPlayer()->HasEffectType(CECPlayer::EFF_FACEPILL) )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(595));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_FACE);
	}

	if( pEssence->combined_services & 0x40 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(634));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_MAIL);
	}

	if( pEssence->combined_services & 0x80 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(644));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_AUCTION);
	}
	
//	if( pEssence->combined_services & 0x100 )
//	{
//		m_pLst_Main->AddString(strText + GetStringFromTable(683));
//		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_DOUBLEEXP);
//	}

	if( pEssence->combined_services & 0x200 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(691));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_PETHATCH);
	}

	if( pEssence->combined_services & 0x400 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(692));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_PETREC);
	}

	if( pEssence->combined_services & 0x2000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(734));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_GOLDTRADE);
	}


	if( pEssence->combined_services & 0x4000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(769));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_EQUIPREFINE);
	}

	if( pEssence->combined_services & 0x8000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(874));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_EQUIPDYE);
		
		m_pLst_Main->AddString(strText + GetStringFromTable(9323));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_PETDYE);
	}
	
	if( pEssence->combined_services & 0x10000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(878));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_REFINETRANS);
	}

	if (pEssence->combined_services & 0x800000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7921));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_EQUIPREPAIR);
	}

	if (pEssence->combined_services & 0x1000000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(8600));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_WEBTRADE);
	}
	
	if (pEssence->combined_services & 0x2000000)
	{
		if (GetHostPlayer()->GetBasicProps().iLevel2 >= 20)
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(8601));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_GODEVILCONVERT);
		}
	}

	if (pEssence->combined_services & 0x4000000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(8740));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_WEDDING_BOOK);
		m_pLst_Main->AddString(strText + GetStringFromTable(8741));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_WEDDING_INVITE);
	}

	bool bFortressOK = false;
	while (true)
	{
		int idFaction = GetHostPlayer()->GetFactionID();
		if (idFaction <= 0 ||
			GetGame()->GetFactionMan()->GetFaction(idFaction) == NULL)
		{
			//	无帮派、或者帮派信息尚未查询到
			break;
		}

		if (!GetHostPlayer()->GetFactionFortressConfig())
		{
			//	没有基地配置表
			break;
		}

		bFortressOK = true;
		break;
	}

	bool bMaster(false), bViceMaster(false), bBodyGuard(false);
	CECHostPlayer *pHost = GetHostPlayer();
	if (pHost->GetFactionID() > 0){
		switch (pHost->GetFRoleID())
		{
		case GNET::_R_MASTER:
			bMaster = true;
			break;
		case GNET::_R_VICEMASTER:
			bViceMaster = true;
			break;
		case GNET::_R_BODYGUARD:
			bBodyGuard = true;
			break;
		}
	}
	bool bInFortressNow = pHost->IsInFortress();

	if (bFortressOK)
	{
		//	帮派基地相关

		if (pEssence->combined_services & 0x8000000)
		{
			if (bMaster)
			{
				//	帮主可以创建基地
				m_pLst_Main->AddString(strText + GetStringFromTable(9101));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_CREATE);
			}
// 			if (bMaster || bViceMaster)
// 			{
// 				//	帮主、副帮主可以处理战斗事宜
// 				m_pLst_Main->AddString(strText + GetStringFromTable(9102));
// 				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_WAR);
// 			}

			if (!bInFortressNow)
			{
				//	帮众随时可进入我方基地
				m_pLst_Main->AddString(strText + GetStringFromTable(9107));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_ENTER);
			}

			//	帮众随时可查看对战表
// 			m_pLst_Main->AddString(strText + GetStringFromTable(9108));
// 			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_WARLIST);
		}
		
		if (pEssence->combined_services & 0x10000000)
		{
			if (bMaster)
			{
				if (!GetHostPlayer()->IsInFortressWar())
				{
					//	帮主非战时可以升级基地设施
					m_pLst_Main->AddString(strText + GetStringFromTable(9103));
					m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_UPGRADE);
				}
			}

			//	帮众可以缴纳材料
			m_pLst_Main->AddString(strText + GetStringFromTable(9104));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_HANDIN_MATERIAL);

			//	帮众可以缴纳贡献度
			m_pLst_Main->AddString(strText + GetStringFromTable(9105));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_HANDIN_CONTRIB);

			if (bInFortressNow)
			{
				//	帮众可以离开基地
				m_pLst_Main->AddString(strText + GetStringFromTable(9109));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_LEAVE);
			}
		}

		if (pEssence->combined_services & 0x20000000)
		{
			if (bMaster)
			{
				//	帮主可以兑换材料
				m_pLst_Main->AddString(strText + GetStringFromTable(9106));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_EXCHANGE_MATERIAL);
			}
		}
	}

	if (pEssence->combined_services & 0x8000000)
	{
		if (g_bEnableFortressDeclareWar)
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(9102));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CGLDNPC_FORTRESS_WAR);
		}
	}

	// check the storage without password
	if(pEssence->combined_services & 0x40000000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(9299));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_VIEW_TRASHBOX);
	}

	if (pEssence->combined_services & 0x80000000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(9340));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_VIEW_DPS_DPH_RANK);
	}

	if (pEssence->combined_services2 & 0x00000001)
	{
		bool bHasCountry = GetHostPlayer()->GetCountry() > 0;
		m_pLst_Main->AddString(strText + GetStringFromTable(bHasCountry ? 9763 : 9762 ));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, bHasCountry ? CDLGNPC_QUIT_COUNTRY : CDLGNPC_JOIN_COUNTRY);
	}
	
	if (pEssence->combined_services2 & 0x00000002)
	{
		if (GetHostPlayer()->IsInCountryWar())
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(9914));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_LEAVE_COUNTRY_WAR);
		}
	}
	
	if (pEssence->combined_services2 & 0x00000004)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(9994));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_EQUIP_MARK);
	}
	
	if (pEssence->combined_services2 & 0x00000008)
	{
		if (!CECCrossServer::Instance().IsOnSpecialServer())
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(10132));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_GOTO_SPECIALSERVER);
		}
	}
	
	if (pEssence->combined_services2 & 0x00000010)
	{
		if (CECCrossServer::Instance().IsOnSpecialServer())
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(10133));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_GOTO_ORIGINALSERVER);
		}
	}
	// 改名服务
	if (pEssence->combined_services2 & 0x00000020)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10150));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_PLAYER_RENAME);
	}
	// 宝石转换
	bool bHasStoneChange = (pEssence->combined_services2 & 0x00000040) != 0;
	if (bHasStoneChange)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10172));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount()-1, CDLGNPC_STONE_CHANGE);
	}	
	// 国王相关
	if (pEssence->combined_services2 & 0x00000080)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10304));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_KING_SERVICE);
	}
	// 寄卖服务
	if (pEssence->combined_services2 & 0x00000100)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10508));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_OFFLINESHOP_SETTING);

		m_pLst_Main->AddString(strText + GetStringFromTable(10513));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_OFFLINESHOP_SELLBUY);
	}
	//	时装拆分
	if (pEssence->combined_services2 & 0x00000200)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10430));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_SPLIT_FASHION);
	}
	if (pEssence->combined_services2 & 0x00000400)
	{
		// 转生版本上限
		if (GetHostPlayer()->GetReincarnationCount() < MAX_REINCARNATION) {
			m_pLst_Main->AddString(strText + GetStringFromTable(10800));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_REINCARNATION);
		}
	}
	if (pEssence->combined_services2 & 0x00000800)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10820));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_GIFTCARD);		
	}
	if (pEssence->combined_services2 & 0x00001000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10890));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_TRICKBATTLE);		
	}
	if (pEssence->combined_services2 & 0x00002000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(11000));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_CARDRESPAWN);	
	}
	if (pEssence->combined_services2 & 0x00001000) // 战车开启npc 自动带有改服务器，（纯客户端）
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(10912));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_QUERYCHARIOTAMOUNT);			
	}
	if(pEssence->combined_services2 & 0x00004000)
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(11193));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount()-1,CDLGNPC_FLYSWORDIMPROVE);
	}
	if(pEssence->combined_services2 & 0x00008000)
	{
		if (!pHost->IsInFactionPVP() && (bMaster || bViceMaster || bBodyGuard)){
			m_pLst_Main->AddString(strText + GetStringFromTable(11270));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount()-1,CDLGNPC_OPEN_FACTION_PVP);
		}
	}
	if((pEssence->combined_services2 & 0x00010000) ||
		pEssence->combined_services2 & 0x00020000){
		m_pLst_Main->AddString(strText + GetStringFromTable(11437));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount()-1, CDLGNPC_GOLD_SHOP);
	}

	if (pEssence->combined_services2 & 0x00040000){
		if (CECProfConfig::Instance().IsExist(GetHostPlayer()->GetProfession(), CECProfConfig::Instance().GetCounterpartGender(GetHostPlayer()->GetGender()))){
			m_pLst_Main->AddString(strText + GetStringFromTable(11438));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount()-1, CDLGNPC_PLAYER_CHANGE_GENDER);
		}
	}
	
	for( i = 0; i < sizeof(a_uiService) / sizeof(unsigned int); i++ )
	{
		if( !a_uiService[i] ) continue;

		pData = pDataMan->get_data_ptr(a_uiService[i], ID_SPACE_ESSENCE, DataType);

		if( DataType == DT_NPC_TALK_SERVICE )
		{
			NPC_TALK_SERVICE *pService = (NPC_TALK_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_SELL_SERVICE )
		{
			NPC_SELL_SERVICE *pService = (NPC_SELL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_BUY_SERVICE )
		{
			continue;
		}
		else if( DataType == DT_NPC_REPAIR_SERVICE )
		{
			NPC_REPAIR_SERVICE *pService = (NPC_REPAIR_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_INSTALL_SERVICE )
		{
			NPC_INSTALL_SERVICE *pService = (NPC_INSTALL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_UNINSTALL_SERVICE )
		{
			NPC_UNINSTALL_SERVICE *pService = (NPC_UNINSTALL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_SKILL_SERVICE )
		{
			NPC_SKILL_SERVICE *pService = (NPC_SKILL_SERVICE *)pData;
			if (i >= 18)
			{
				m_pLst_Main->AddString(strText + GetStringFromTable(7107));
			}
			else
				m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_HEAL_SERVICE )
		{
			NPC_HEAL_SERVICE *pService = (NPC_HEAL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_TRANSMIT_SERVICE )
		{
			NPC_TRANSMIT_SERVICE *pService = (NPC_TRANSMIT_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_TRANSPORT_SERVICE )
		{
			NPC_TRANSPORT_SERVICE *pService = (NPC_TRANSPORT_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_PROXY_SERVICE )
		{
			NPC_PROXY_SERVICE *pService = (NPC_PROXY_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_STORAGE_SERVICE )
		{
			NPC_STORAGE_SERVICE *pService = (NPC_STORAGE_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, a_uiService[i]);
			m_pLst_Main->AddString(strText + GetStringFromTable(253));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, a_uiService[i]);
			m_pLst_Main->AddString(strText + GetStringFromTable(8080));
		}
		else if( DataType == DT_NPC_MAKE_SERVICE )
		{
			NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_DECOMPOSE_SERVICE )
		{
			NPC_DECOMPOSE_SERVICE *pService = (NPC_DECOMPOSE_SERVICE *)pData;
			CECHostPlayer *pHost = GetHostPlayer();
			CECSkill *pSkill;
			int j(0);
			for(j = 0; j < pHost->GetPassiveSkillNum(); j++ )
			{
				pSkill = pHost->GetPassiveSkillByIndex(j);
				if( (pSkill->GetType() == CECSkill::TYPE_LIVE ||
					pSkill->GetType() == CECSkill::TYPE_PRODUCE) &&
					(int)pService->id_decompose_skill == pSkill->GetSkillID() )
				{
					m_pLst_Main->AddString(strText + pService->name);
					break;
				}
			}
			if( j == pHost->GetPassiveSkillNum() )
				continue;
		}
		else if( DataType == DT_NPC_IDENTIFY_SERVICE )
		{
			NPC_IDENTIFY_SERVICE *pService = (NPC_IDENTIFY_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_WAR_TOWERBUILD_SERVICE )
		{
			NPC_WAR_TOWERBUILD_SERVICE *pService = (NPC_WAR_TOWERBUILD_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_RESETPROP_SERVICE )
		{
			NPC_RESETPROP_SERVICE *pService = (NPC_RESETPROP_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_PETNAME_SERVICE )
		{
			NPC_PETNAME_SERVICE *pService = (NPC_PETNAME_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_PETLEARNSKILL_SERVICE )
		{
			NPC_PETLEARNSKILL_SERVICE *pService = (NPC_PETLEARNSKILL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_PETFORGETSKILL_SERVICE )
		{
			NPC_PETFORGETSKILL_SERVICE *pService = (NPC_PETFORGETSKILL_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_EQUIPBIND_SERVICE )
		{
			NPC_EQUIPBIND_SERVICE *pService = (NPC_EQUIPBIND_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_EQUIPDESTROY_SERVICE )
		{
			NPC_EQUIPDESTROY_SERVICE *pService = (NPC_EQUIPDESTROY_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if( DataType == DT_NPC_EQUIPUNDESTROY_SERVICE )
		{
			NPC_EQUIPUNDESTROY_SERVICE *pService = (NPC_EQUIPUNDESTROY_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if (DataType == DT_NPC_ENGRAVE_SERVICE)
		{
			NPC_ENGRAVE_SERVICE *pService  = (NPC_ENGRAVE_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if (DataType == DT_NPC_RANDPROP_SERVICE)
		{
			NPC_RANDPROP_SERVICE *pService  = (NPC_RANDPROP_SERVICE *)pData;
			m_pLst_Main->AddString(strText + pService->name);
		}
		else if (DataType == DT_NPC_FORCE_SERVICE)
		{
			NPC_FORCE_SERVICE *pService  = (NPC_FORCE_SERVICE *)pData;
			int npcForce = pService->force_id;
			int playerForce = GetHostPlayer()->GetForce();
			const FORCE_CONFIG* pConfig = CECForceMgr::GetForceData(npcForce);
			if(pConfig && (npcForce == playerForce || !playerForce) )
			{
				ACString strForce;
				strForce.Format(GetStringFromTable(playerForce ? 9401:9400), pConfig->name);
				m_pLst_Main->AddString(strText + strForce);
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, playerForce ? CDLGNPC_FORCE_QUIT:CDLGNPC_FORCE_JOIN);
				m_pLst_Main->SetItemDataPtr(m_pLst_Main->GetCount() - 1, (void *)pData);
			}
			
			// hide the invalid force option
			continue;
		}
		else
		{
			//	未识别的服务
			continue;
		}

		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, a_uiService[i]);
		m_pLst_Main->SetItemDataPtr(m_pLst_Main->GetCount() - 1, (void *)pData);
	}
	//add for goblin by czx
	if( pEssence->combined_services & 0x400000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7106));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFEQUIPOFF);
	}
	if( pEssence->combined_services & 0x20000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7100));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFPRORESET);
	}
	if( pEssence->combined_services & 0x40000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7101));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFGENIUSRESET);
	}
	if( pEssence->combined_services & 0x80000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7102));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFFORGET);
	}
	if( pEssence->combined_services & 0x200000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7105));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFTOPILL);
	}
	if( pEssence->combined_services & 0x100000 )
	{
		m_pLst_Main->AddString(strText + GetStringFromTable(7103));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFREFINE);
		m_pLst_Main->AddString(strText + GetStringFromTable(7104));
		m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_ELFREFINETRANS);
	}

	if( GetGameSession()->GetServerAttr().battle == 1 )
	{
		if( GetGameUIMan()->m_pDlgGuildMap->IsBattleOpen() && 
			(pEssence->combined_services & 0x800) )
		{
			long stime = GetGame()->GetServerGMTTime();
			stime -= GetGame()->GetTimeZoneBias() * 60; // localtime = UTC - bias, in which bias is minute
			tm *gtime = gmtime(&stime);
//			if( gtime->tm_wday == 1 && gtime->tm_hour >= 19 )
			if( GetGameUIMan()->m_pDlgGuildMap->IsBattleChallengeOpen() )
//				&& GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER )
			{
				m_pLst_Main->AddString(strText + GetStringFromTable(699));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_BATTLECHALLENGE);
			}
//			if( gtime->tm_wday == 5 && gtime->tm_hour >= 18 || 
//				gtime->tm_wday == 6 ||
//				gtime->tm_wday == 0 )
			if( GetHostPlayer()->GetFRoleID() != GNET::_R_UNMEMBER )
			{
				m_pLst_Main->AddString(strText + GetStringFromTable(722));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_BATTLEENTER);
			}
		}

		if( pEssence->combined_services & 0x1000 )
		{
			m_pLst_Main->AddString(strText + GetStringFromTable(724));
			m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, CDLGNPC_BATTLELEAVE);
		}
	}	

	//	任务相关末尾添加，以判断是以“任务相关”显示在列表里，还是直接显示任务列表
	CheckTaskService(pEssence);

	if( pEssence->domain_related == 1 )
	{
		if( GetHostPlayer()->GetFRoleID() == GNET::_R_UNMEMBER )
			m_pLst_Main->ResetContent();
		else
		{
			CECNPCMan *pNPCMan = GetWorld()->GetNPCMan();
			CECNPC *pNPC = pNPCMan->GetNPC(GetHostPlayer()->GetCurServiceNPC());
			if( pNPC )
			{
				A3DVECTOR3 pos = pNPC->GetServerPos();
				CECWorld *pWorld = GetWorld();
				CELPrecinct* pPrecinct = pWorld->GetPrecinctSet()->IsPointIn(
					pos.x, pos.z, pWorld->GetInstanceID());
				if( pPrecinct )
				{
					int idDomain = pPrecinct->GetDomainID();
					const DOMAIN_INFO *pInfo = CECDomainGuildInfo::Instance().Find(idDomain);
					int idFaction = pInfo ? pInfo->id_owner : -1;

					if( GetHostPlayer()->GetFactionID() != idFaction )
						m_pLst_Main->ResetContent();
				}
			}
		}
	}

	// Add "EXIT" choice.
	m_pLst_Main->AddString(GetStringFromTable(503));

	// Update list height for diffent service count
	if (pEssence->id != m_lastNPCID)
	{
		m_lastNPCID = pEssence->id;

		// Compute new list height
		const float fScale = GetGameUIMan()->GetWindowScale();
		const int c_heightPerItem = int(18 * fScale);
		const int c_minListHeight = int(101 * fScale);
		const int c_minTextHeight = int(135 * fScale);
		int itemCount = m_pLst_Main->GetCount();
		int newListHeight = itemCount*c_heightPerItem + 1;
		newListHeight = max(newListHeight, c_minListHeight);
		SIZE sizeText = m_pTxt_Content->GetSize();
		SIZE sizeList = m_pLst_Main->GetSize();
		if (sizeText.cy-(newListHeight-sizeList.cy) < c_minTextHeight)
			newListHeight = sizeText.cy-c_minTextHeight+sizeList.cy;

		// Compute list height change
		int deltaListHeight = newListHeight-sizeList.cy;

		// Adjust controls according to height change
		if (deltaListHeight)
		{
			POINT pos = m_pLst_Main->GetPos(true);
			m_pLst_Main->SetPos(pos.x, pos.y-deltaListHeight);
			m_pLst_Main->SetSize(sizeList.cx, sizeList.cy+deltaListHeight);			
			m_pTxt_Content->SetSize(sizeText.cx, sizeText.cy-deltaListHeight);
		}
	}

	GetGameUIMan()->m_bShowAllPanels = true;
}

void CDlgNPC::PopupNPCDialog(talk_proc *pTalk)
{
	if( pTalk->num_window == 0 ) return;

	int i, nIndex;
	ACString strText;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	m_pTxt_Content->SetText(pTask->FormatTaskTalk(pTalk->windows[0].talk_text));
	m_pTxt_npc->SetText(_AL(""));

	m_pLst_Main->ResetContent();
	for( i = 0; i < pTalk->windows[0].num_option; i++ )
	{
		strText = GetStringFromTable(249);
		strText += pTalk->windows[0].options[i].text;
		m_pLst_Main->AddString(strText);
		nIndex = m_pLst_Main->GetCount() - 1;
		m_pLst_Main->SetItemData(nIndex, pTalk->windows[0].options[i].id);
		m_pLst_Main->SetItemDataPtr(nIndex, &pTalk->windows[0].options[i]);
	}

	SetDataPtr(pTalk,"ptr_talk_proc");
	if( !IsShow() ) Show(true);
}

void CDlgNPC::PopupTaskDialog(bool bTaskNPC)
{
	NPC_ESSENCE *pCurNPCEssence = GetGameUIMan()->m_pCurNPCEssence;
	NPC_ESSENCE *pEssence = pCurNPCEssence;
	if( !pEssence ) return;

	int i, j, idTask;
	const void *pData;
	DATA_TYPE DataType;
	unsigned int *a_idTask;
	unsigned int a_uiService[] =
	{
		pEssence->id_task_out_service,		// 任务相关的服务ID: 发放任务服务
		pEssence->id_task_in_service,		// 任务相关的服务ID: 验证完成任务服务
		pEssence->id_task_matter_service	// 任务相关的服务ID: 发放任务物品服务
	};
	int idLastTask = 0, nLastTaskTime = 0, nFinishTime;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	abase::vector<TASK_ITEM> taskIn, taskOut, taskMatter;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	for( i = 0; i < sizeof(a_uiService) / sizeof(unsigned int); i++ )
	{
		pData = pDataMan->get_data_ptr(a_uiService[i], ID_SPACE_ESSENCE, DataType);

		if( DataType == DT_NPC_TASK_IN_SERVICE ||
			DataType == DT_NPC_TASK_OUT_SERVICE )
		{
			int total_count = 0;

			if( DataType == DT_NPC_TASK_IN_SERVICE )
			{
				NPC_TASK_IN_SERVICE *pService = (NPC_TASK_IN_SERVICE *)pData;
				a_idTask = pService->id_tasks;
				total_count = sizeof(pService->id_tasks) / sizeof(pService->id_tasks[0]);
			}
			else
			{
				NPC_TASK_OUT_SERVICE *pService = (NPC_TASK_OUT_SERVICE *)pData;

				// if storage task not exists
				if(pService->storage_id == 0 || pService->storage_open_item == 0)
				{
					a_idTask = pService->id_tasks;
					total_count = sizeof(pService->id_tasks) / sizeof(pService->id_tasks[0]);
				}
			}

			for( j = 0; j < total_count; j++ )
			{
				idTask = a_idTask[j];
				if( idTask <= 0 ) continue;

				if( DataType == DT_NPC_TASK_IN_SERVICE )
					taskIn.push_back(TASK_ITEM(idTask, a_uiService[i]));
				else
					taskOut.push_back(TASK_ITEM(idTask, a_uiService[i]));

				nFinishTime = (int)pTask->GetTaskFinishedTime(idTask);
				if( nFinishTime > nLastTaskTime )
				{
					idLastTask = idTask;
					nLastTaskTime = nFinishTime;
				}
			}
		}
		else if( DataType == DT_NPC_TASK_MATTER_SERVICE )
		{
			NPC_TASK_MATTER_SERVICE *pService = (NPC_TASK_MATTER_SERVICE *)pData;

			for( j = 0; j < 16; j++ )
			{
				idTask = pService->tasks[j].id_task;
				if( idTask > 0 )
				{
					taskMatter.push_back(TASK_ITEM(idTask, a_uiService[i]));
				}
			}
		}
	}

	A3DCOLOR color;
	ACString strText;
	const talk_proc *pTalk;
	ATaskTempl *pTemp, *pTempRoot;
	int nIndex, nNumTasks = 0;
	int nHostLevel = GetHostPlayer()->GetBasicProps().iLevel;
	ATaskTemplMan *pTempMan = g_pGame->GetTaskTemplateMan();

	m_pLst_Main->ResetContent();
	abase::vector<TASK_ITEM>* a_uiTasks[] = { &taskIn, &taskOut, &taskMatter };
	for( int idx = 0; idx < sizeof(a_uiTasks) / sizeof(a_uiTasks[0]); idx++ )
	{
		abase::vector<TASK_ITEM>::iterator it;
		for( it = a_uiTasks[idx]->begin(); it != a_uiTasks[idx]->end(); ++it )
		{
			idTask = it->task_id;

			color = 0;
			bool bNeedSetSpecialColor = false;
			pTemp = NULL;
			pTalk = NULL;
			color = m_pLst_Main->GetColor();
			strText = GetStringFromTable(249);

			if( idx == 1 )
			{
				if( pTask->HasTask(idTask) )
				{
					if( !pTask->CanFinishTask(idTask) )
					{
						pTemp = pTempMan->GetTaskTemplByID(idTask);
						pTalk = pTemp->GetUnfinishedTalk();
					}
				}
				else if( pTask->CanShowTask(idTask) )
				{
					pTemp = pTempMan->GetTaskTemplByID(idTask);

					if( 0 == pTask->CanDeliverTask(idTask) )
						pTalk = pTemp->GetDeliverTaskTalk();
					else
					{
						pTalk = pTemp->GetUnqualifiedTalk();
						color = A3DCOLORRGB(128, 128, 128);
						bNeedSetSpecialColor = true;
					}
				}
			}

			if( idx == 0 && pTask->HasTask(idTask)
				&& pTask->CanFinishTask(idTask) && !pTalk )
			{
				pTemp = pTempMan->GetTaskTemplByID(idTask);
				pTalk = pTemp->GetAwardTalk();
				strText = GetStringFromTable(302);
			}

			if( pTemp && pTalk && (pTalk->num_window > 1 || pTalk->num_window == 1 && wcslen(pTalk->windows[0].talk_text)))
			{
				if( pTemp->IsKeyTask() ) {
					color = A3DCOLORRGB(255, 162, 0);
					bNeedSetSpecialColor = true;
				}
				else if( color != 0 )
				{
					// 去掉根据人物和任务级别差来改变颜色的逻辑
					/*
					nLevel = pTemp->GetSuitableLevel();
					if( nHostLevel <= nLevel - 2 )
						color = A3DCOLORRGB(255, 54, 0);
					else if( nHostLevel >= nLevel + 3 )
						color = A3DCOLORRGB(22, 142, 54);
					*/
				}
				

				pTempRoot = (ATaskTempl *)pTemp->GetTopTask();
				if( pTemp != pTempRoot )
				{
					if (bNeedSetSpecialColor) {
						strText += CDlgTask::GetTaskNameWithOutColor(pTempRoot);
					} else {
						strText += CDlgTask::GetTaskNameWithColor(pTempRoot);
					}	
					strText += _AL(" - ");
				}
				if (bNeedSetSpecialColor) {
					strText += CDlgTask::GetTaskNameWithOutColor(pTemp);
				} else {
					strText += CDlgTask::GetTaskNameWithColor(pTemp);
				}				
	 			m_pLst_Main->AddString(strText);
	 			nIndex = m_pLst_Main->GetCount() - 1;
	 			m_pLst_Main->SetItemData(nIndex, it->service);			// Service ID.
	 			m_pLst_Main->SetItemDataPtr(nIndex, (void *)pTalk);		// Talk data.
				m_pLst_Main->SetItemData64(nIndex,pTemp->GetID(),0,"TaskID");

				if (bNeedSetSpecialColor) {
					m_pLst_Main->SetItemTextColor(nIndex, color);
				}
				nNumTasks++;
			}
		}
	}

	if( idLastTask > 0 )
	{
		pTemp = pTempMan->GetTaskTemplByID(idLastTask);
		if( a_strlen(pTemp->GetTribute()) > 0 )
			m_pTxt_Content->SetText(pTemp->GetTribute());
		else
		{
			if( nNumTasks > 0 )
				m_pTxt_Content->SetText(GetStringFromTable(502));
			else
				m_pTxt_Content->SetText(GetStringFromTable(501));
		}
	}
	else if( bTaskNPC )
		m_pTxt_Content->SetText(pCurNPCEssence->hello_msg);
	else
	{
		if( nNumTasks > 0 )
			m_pTxt_Content->SetText(GetStringFromTable(502));
		else
			m_pTxt_Content->SetText(GetStringFromTable(501));
	}

	SetData(NPC_DIALOG_TASK_LIST);
}

void CDlgNPC::PopupTaskFinishDialog(int idTask, talk_proc *pTalk)
{
	PopupNPCDialog(pTalk);
	SetData(NPC_DIALOG_TASK_TALK);
	GetGameUIMan()->m_idCurFinishTask = idTask;
}

bool CDlgNPC::PopupCorrespondingServiceDialog(
	int idFunction, int iService, void *pData)
{
	PAUIDIALOG pShow1 = NULL, pShow2 = NULL;
	NPC_ESSENCE *pCurNPCEssence = GetGameUIMan()->m_pCurNPCEssence;

	if( idFunction == NPC_SELL || idFunction == NPC_BUY )
	{
		PAUISTILLIMAGEBUTTON pButton;

		pShow1 = m_pAUIManager->GetDialog("Win_Shop");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");

		GetHostPlayer()->PrepareNPCService(iService);
		GetGameUIMan()->m_pDlgShop->UpdateShop(1);

		if( pCurNPCEssence && pCurNPCEssence->id_repair_service )
		{
			pShow1->GetDlgItem("Btn_Repair")->Show(true);
			pShow1->GetDlgItem("Btn_RepairAll")->Show(true);
		}
		else
		{
			pShow1->GetDlgItem("Btn_Repair")->Show(false);
			pShow1->GetDlgItem("Btn_RepairAll")->Show(false);
		}
		
		pButton = (PAUISTILLIMAGEBUTTON)pShow2->GetDlgItem("Btn_NormalItem");
		pButton->SetPushed(true);
		pButton = (PAUISTILLIMAGEBUTTON)pShow2->GetDlgItem("Btn_QuestItem");
		pButton->SetPushed(false);
		GetGameUIMan()->m_pDlgInventory->SetShowItem(CDlgInventory::INVENTORY_ITEM_NORMAL);
	}
	else if( idFunction == NPC_INSTALL )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_Enchase");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if( idFunction == NPC_UNINSTALL )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_Disenchase");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if( idFunction == NPC_HEAL )
	{
		GetGameSession()->c2s_CmdNPCSevHeal();
		GetGameUIMan()->EndNPCService();
	}
	else if( idFunction == NPC_TRANSMIT )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_WorldMapTravel");
		((CDlgWorldMap*)pShow1)->BuildTravelMap(DT_NPC_TRANSMIT_SERVICE, pData);
	}
	else if( idFunction == NPC_SKILL )
	{
		ACString strText = m_pLst_Main->GetText(m_pLst_Main->GetCurSel());
		ACString strHead = GetStringFromTable(249);
		ACString strComp = ACString(strHead + GetStringFromTable(7107));
		if( 0 == a_stricmp(strText, strComp) )
		{
			pShow1 = m_pAUIManager->GetDialog("Win_ELFLearn");
			((CDlgELFLearn *)pShow1)->SetNPCName(pCurNPCEssence->name);
			pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
			GetHostPlayer()->PrepareNPCService(iService);
			pShow1->SetData(DT_NPC_SKILL_SERVICE);
		}
		else
		{
			pShow1 = m_pAUIManager->GetDialog("Win_Teach");
			GetHostPlayer()->PrepareNPCService(iService);
			pShow1->SetData(DT_NPC_SKILL_SERVICE);
			GetGameUIMan()->UpdateTeach(0);
		}
	}
	else if( idFunction == NPC_MAKE )
	{
		NPC_MAKE_SERVICE *pMake = (NPC_MAKE_SERVICE *)pData;
		if( pMake->produce_type == 2 )
			pShow1 = m_pAUIManager->GetDialog("Win_Produce1");
		else
			pShow1 = m_pAUIManager->GetDialog("Win_Produce");
		GetGameUIMan()->m_pDlgProduce = (CDlgProduce*)pShow1;
		GetHostPlayer()->PrepareNPCService(iService);
		pShow1->SetDataPtr(pMake,"ptr_NPC_MAKE_SERVICE");
		if( pMake->produce_type == 1 ||
			pMake->produce_type == 3 ||
			pMake->produce_type == 4 ||
			pMake->produce_type == 5)
			GetGameUIMan()->m_pDlgProduce->ClearMaterial();
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
		GetGameUIMan()->m_pDlgProduce->UpdateProduce(1, 0);
	}
	else if( idFunction == NPC_DECOMPOSE )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_Split");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
		pShow1->SetDataPtr(pData,"ptr_NPC_DECOMPOSE_SERVICE");

		PAUIPROGRESS pProgress;
		PAUIIMAGEPICTURE pImage;

		pImage = (PAUIIMAGEPICTURE)pShow1->GetDlgItem("Item_a");
		pImage->ClearCover();
		pImage->SetDataPtr(NULL);

		pImage = (PAUIIMAGEPICTURE)pShow1->GetDlgItem("Item_b");
		pImage->ClearCover();
		pImage->SetDataPtr(NULL);

		pProgress = (PAUIPROGRESS)pShow1->GetDlgItem("Prgs_1");
		pProgress->SetProgress(0);

		pShow1->GetDlgItem("Btn_Start")->Enable(false);
		pShow1->GetDlgItem("Btn_Cancel")->Enable(true);
		pShow1->GetDlgItem("Txt_no1")->SetText(_AL(""));
		pShow1->GetDlgItem("Txt_no2")->SetText(_AL(""));
		pShow1->GetDlgItem("Txt_Gold")->SetText(_AL(""));
		pShow1->GetDlgItem("Txt_SkillName")->SetText(_AL(""));
		pShow1->GetDlgItem("Txt_SkillLevel")->SetText(_AL(""));
	}
	else if( idFunction == NPC_WAR_TOWERBUILD )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_WarTower");
	}
	else if( idFunction == NPC_RESETPROP )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_ResetProp");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if( idFunction == NPC_PETNAME )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_PetRename");
		pShow2 = m_pAUIManager->GetDialog("Win_PetList");
	}
	else if( idFunction == NPC_PETLEARNSKILL )
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetActivePet();
		if( !pPet )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(814), MB_OK, A3DCOLORRGB(255, 255, 255));
			GetGameUIMan()->EndNPCService();
		}
		else
		{
			pShow1 = m_pAUIManager->GetDialog("Win_Teach");
			GetHostPlayer()->PrepareNPCService(iService);
			pShow1->SetData(DT_NPC_PETLEARNSKILL_SERVICE);
			GetGameUIMan()->UpdateTeach(0);
		}
	}
	else if( idFunction == NPC_PETFORGETSKILL )
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetActivePet();
		if( !pPet )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(814), MB_OK, A3DCOLORRGB(255, 255, 255));
			GetGameUIMan()->EndNPCService();
		}
		else
			pShow1 = m_pAUIManager->GetDialog("Win_PetRetrain");
	}
	else if( idFunction == NPC_EQUIPBIND )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_EquipBind");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
		pShow1->SetData((DWORD)pData, "ptr_NPC_EQUIPBIND_SERVICE");
	}
	else if( idFunction == NPC_EQUIPDESTROY )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_EquipDestroy");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if( idFunction == NPC_EQUIPUNDESTROY )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_EquipUndestroy");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if( idFunction == NPC_IDENTIFY )
	{
		PAUIIMAGEPICTURE pImage;

		pShow1 = m_pAUIManager->GetDialog("Win_Identify");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
		pShow1->SetDataPtr(pData,"ptr_NPC_IDENTIFY_SERVICE");

		pImage = (PAUIIMAGEPICTURE)pShow1->GetDlgItem("Item");
		pImage->ClearCover();
		pImage->SetDataPtr(NULL);

		pShow1->GetDlgItem("Txt")->SetText(_AL(""));
		pShow1->GetDlgItem("Txt_Gold")->SetText(_AL(""));
		pShow1->GetDlgItem("Btn_Confirm")->Enable(false);
		pShow1->GetDlgItem("Btn_Cancel")->Enable(true);
	}
	else if( idFunction == NPC_GIVE_TASK )
	{
		talk_proc::option *opt = (talk_proc::option *)pData;
		int idTask = opt->param;
		if( g_pGame->GetConfigs()->IsMiniClient() && CECUIConfig::Instance().GetGameUI().IsTaskDisabledInMiniClient(idTask) )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(10714), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			GetGameSession()->c2s_CmdNPCSevAcceptTask(idTask,0,0);
		}
		GetGameUIMan()->EndNPCService();
	}
	else if( idFunction == NPC_COMPLETE_TASK )
	{
		AWARD_DATA ad;
		talk_proc::option *opt = (talk_proc::option *)pData;
		CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

		pTask->GetAwardCandidates(opt->param, &ad);
		if( ad.m_ulCandItems > 1 )
		{
			pShow1 = m_pAUIManager->GetDialog("Win_Award");
			CDlgAward* pAward = dynamic_cast<CDlgAward*>(pShow1);
			if (pAward) pAward->UpdateAwardItem(opt->param, true);
		}
		else
		{
			GetGameSession()->c2s_CmdNPCSevReturnTask(opt->param, 0);
			GetGameUIMan()->EndNPCService();
		}
	}
	else if( idFunction == NPC_GIVE_TASK_MATTER )
	{
		talk_proc::option *opt = (talk_proc::option *)pData;
		GetGameSession()->c2s_CmdNPCSevTaskMatter(opt->param);
		GetGameUIMan()->EndNPCService();
	}
	else if( idFunction == NPC_STORAGE )
	{
 		if( GetHostPlayer()->TrashBoxHasPsw() )
		{
			pShow1 = m_pAUIManager->GetDialog("Win_InputString");
			pShow1->GetDlgItem("DEFAULT_Txt_Input")->SetText(_AL(""));
		}
		else
			g_pGame->GetGameSession()->c2s_CmdNPCSevOpenTrash("");
	}
	else if( idFunction == NPC_STORAGE_PASSWORD )
	{
		pShow1 = m_pAUIManager->GetDialog("Win_InputString3");
	}
	else if (idFunction == NPC_ACCOUNT_STORAGE)
	{
		if (CECCrossServer::Instance().IsOnSpecialServer())
		{
			GetGameUIMan()->ShowErrorMsg(10130);
			GetGameUIMan()->EndNPCService();
		}
		else g_pGame->GetGameSession()->c2s_CmdNPCSevOpenAccountBox();
	}
	else if (idFunction == NPC_ENGRAVE)
	{
		pShow1 = m_pAUIManager->GetDialog("Win_Engrave");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
	}
	else if (idFunction == NPC_RANDPROP)
	{
		pShow1 = m_pAUIManager->GetDialog("Win_RandProp");
		pShow2 = m_pAUIManager->GetDialog("Win_Inventory");
		GetHostPlayer()->PrepareNPCService(iService);
		pShow1->SetDataPtr(pData, "ptr_NPC_RANDPROP_SERVICE");
	}
	else if( idFunction == TALK_RETURN )
	{
		OnCommand_back("back");
		return true;		// To avoid to close NPC dialog.
	}
	else if( idFunction == TALK_EXIT )
	{
		GetGameUIMan()->EndNPCService();

		int idCurFinishTask = GetGameUIMan()->m_idCurFinishTask;
		if( GetData() == CDlgNPC::NPC_DIALOG_TASK_TALK && idCurFinishTask > 0 )
		{
			GetHostPlayer()->GetTaskInterface()->OnUIDialogEnd(idCurFinishTask);
			GetGameUIMan()->m_idCurFinishTask = -1;
		}
	}
	else if( idFunction == TALK_GIVEUP_TASK)
	{
		talk_proc::option *opt = (talk_proc::option *)pData;
		GetHostPlayer()->GetTaskInterface()->GiveUpTask(opt->param);
		GetGameUIMan()->EndNPCService();
	}
	else
	{
		GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
		GetGameUIMan()->EndNPCService();
	}

	if( pShow1 )
	{
		DATA_TYPE DataType;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();

		pDataMan->get_data_ptr(iService, ID_SPACE_ESSENCE, DataType);

		pShow1->Show(true);

		if( pShow2 )
		{
			POINT ptPos = pShow1->GetPos();
			if( ptPos.x == 0 && ptPos.y == 0 )
			{
				SIZE s1 = pShow1->GetSize();
				SIZE s2 = pShow2->GetSize();
				A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
				int x, y = (p->Height - max(s1.cy, s2.cy)) / 2;

				x = (p->Width - s1.cx - s2.cx) / 2;
				
				// old : pShow1->SetPos(x, y);
				pShow1->SetPosEx(x, y);

				x += s1.cx;
				
				// old : pShow2->SetPos(x, y);
				pShow2->SetPosEx(x, y);
			}

			pShow2->Show(true);
		}
	}
	Show(false);

	return true;
}

void CDlgNPC::OnEventLButtonDown_Lst_Main(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_bLButtonDown = true;
}

void CDlgNPC::OnEventLButtonUp_Lst_Main(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!m_bLButtonDown)
		return;
	m_bLButtonDown = false;
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	if( m_pLst_Main->GetHitArea(x, y) != AUILISTBOX_RECT_TEXT )
		return;

	SelectListItem(m_pLst_Main->GetCurSel());
}

void CDlgNPC::SelectListItem(int index)
{
	m_pLst_Main->SetCurSel(index);
	int nCurSel = m_pLst_Main->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pLst_Main->GetCount() )
		return;

	int i, j, nIndex;
	ACString strText;
	DATA_TYPE DataType;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	if( GetData() == NPC_DIALOG_TALK ||
		GetData() == NPC_DIALOG_TASK_TALK )
	{
		unsigned int id = m_pLst_Main->GetItemData(nCurSel);
		talk_proc *pTalk = (talk_proc *)GetDataPtr("ptr_talk_proc");

		if( TALKPROC_IS_TERMINAL(id) )			// Ternimal.
		{
			for( i = 0; i < pTalk->num_window; i++ )
			{
				if( id != pTalk->windows[i].id ) continue;

				GetGameUIMan()->MessageBox("", pTask->FormatTaskTalk(pTalk->windows[i].talk_text),
					MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				GetGameUIMan()->EndNPCService();
				Show(false);

				break;
			}

		}
		else if( TALKPROC_IS_FUNCTION(id) )		// Function.
		{
			if( GetGameUIMan()->m_pCurNPCEssence )
			{
				NPC_ESSENCE *pEssence = GetGameUIMan()->m_pCurNPCEssence;
				int nIndex = TALKPROC_GET_FUNCTION_ID(id);
				unsigned int a_uiService[] =
				{
					pEssence->id_talk_service,			// 交谈的服务ID
					pEssence->id_sell_service,			// 出售商品的服务ID
					pEssence->id_buy_service,			// 收购品的服务ID
					pEssence->id_repair_service,		// 修理商品的服务ID
					pEssence->id_install_service,		// 安装镶嵌品的服务ID
					pEssence->id_uninstall_service,		// 拆除镶嵌品的服务ID
					pEssence->id_task_out_service,		// 任务相关的服务ID: 发放任务服务
					pEssence->id_task_in_service,		// 任务相关的服务ID: 验证完成任务服务
					pEssence->id_task_matter_service,	// 任务相关的服务ID: 发放任务物品服务
					pEssence->id_skill_service,			// 教授技能的服务ID
					pEssence->id_heal_service,			// 治疗的服务ID
					pEssence->id_transmit_service,		// 传送的服务ID
					pEssence->id_transport_service,		// 运输的服务ID
					pEssence->id_proxy_service,			// 代售的服务ID
					pEssence->id_storage_service,		// 仓库的服务ID
					pEssence->id_make_service,			// 生产的服务ID
					pEssence->id_decompose_service,		// 分解的服务ID
					pEssence->id_identify_service,		// 鉴定的服务ID
					0,									// Talk return.
					0,									// Talk exit.
					0									// Storage password.
				};

				PopupCorrespondingServiceDialog(id,
					a_uiService[nIndex], m_pLst_Main->GetItemDataPtr(nCurSel));
			}
			else
			{
				PopupCorrespondingServiceDialog(
					id, 0, m_pLst_Main->GetItemDataPtr(nCurSel));
			}
		}
		else					// Window.
		{
			for( i = 0; i < pTalk->num_window; i++ )
			{
				if( id != pTalk->windows[i].id ) continue;

				m_pTxt_Content->SetText(pTask->FormatTaskTalk(pTalk->windows[i].talk_text));
				m_pTxt_Content->SetData(pTalk->windows[i].id_parent);

				m_pLst_Main->ResetContent();
				for( j = 0; j < pTalk->windows[i].num_option; j++ )
				{
					strText = GetStringFromTable(249);
					strText += pTalk->windows[i].options[j].text;
					m_pLst_Main->AddString(strText);
					nIndex = m_pLst_Main->GetCount() - 1;
					m_pLst_Main->SetItemData(nIndex, pTalk->windows[i].options[j].id);
					m_pLst_Main->SetItemDataPtr(nIndex, &pTalk->windows[i].options[j]);
				}

				break;
			}
		}
	}
	else if( GetData() == NPC_DIALOG_TASK_LIST )
	{
		if( nCurSel == m_pLst_Main->GetCount() - 1 )
		{
			GetGameUIMan()->EndNPCService();
			Show(false);
		}
		else
		{
			if (CDlgAutoHelp::IsAutoHelp())
			{
				UINT64 taskid = m_pLst_Main->GetItemData64(nCurSel,0,"TaskID");
				CDlgWikiShortcut::PopQuestWiki(GetGameUIMan(),(int)taskid);
			}
			else
			{
				talk_proc *pTalk = (talk_proc *)m_pLst_Main->GetItemDataPtr(nCurSel);
				
				GetGameUIMan()->PopupNPCDialog(pTalk);
				m_pTxt_Content->SetData(0xFFFFFFFF);
				SetData(NPC_DIALOG_TASK_TALK);
			}
		}
	}
	else if( GetData() == NPC_DIALOG_ESSENCE )
	{
		if( nCurSel == m_pLst_Main->GetCount() - 1 )
		{
			GetGameUIMan()->EndNPCService();
			Show(false);
			return;
		}

		int iService = (int)m_pLst_Main->GetItemData(nCurSel);
		NPC_ESSENCE *pEssence = GetGameUIMan()->m_pCurNPCEssence;
		if( pEssence )
		{
			if( iService == CDLGNPC_FORGET && pEssence->combined_services & 0x01 )
			{
				PAUIDIALOG pMsgBox;

				GetGameUIMan()->MessageBox("Game_ForgetSkill", GetStringFromTable(559),
					MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetDataPtr(m_pLst_Main->GetItemDataPtr(nCurSel));
				GetGameUIMan()->EndNPCService();
				Show(false);
				return;
			}
			else if( iService == CDLGNPC_EQUIPSLOT && pEssence->combined_services & 0x02 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				GetGameUIMan()->m_pDlgEquipSlot->Show(true);
				return;
			}
			else if( pEssence->combined_services & 0x10
				&& (iService == CDLGNPC_FACTION_CREATE
				|| iService == CDLGNPC_FACTION_UPGRADE
				|| iService == CDLGNPC_FACTION_RENAME
				|| iService == CDLGNPC_FACTION_DISMISS) )
			{
				if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					GetGameUIMan()->ShowErrorMsg(10130);
				}			
				else if( iService == CDLGNPC_FACTION_CREATE )
				{
					if( GetHostPlayer()->GetBasicProps().iLevel < 20 ||
						GetHostPlayer()->GetMoneyAmount() < 100000 )
					{
						GetGameUIMan()->MessageBox("", GetStringFromTable(607),
							MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					}
					else
						GetGameUIMan()->m_pDlgGuildCreate->Show(true);
				}
				else if( iService == CDLGNPC_FACTION_UPGRADE )
				{
					int idFaction = GetHostPlayer()->GetFactionID();
					CECFactionMan *pFMan = GetGame()->GetFactionMan();
					Faction_Info *pInfo = pFMan->GetFaction(idFaction);

					if( pInfo )
					{
						if( pInfo->GetLevel() == 0 )
						{
							GetGameUIMan()->MessageBox(
								"Faction_Upgrade", GetStringFromTable(608),
								MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
						}
						else if( pInfo->GetLevel() == 1 )
						{
							GetGameUIMan()->MessageBox(
								"Faction_Upgrade", GetStringFromTable(690),
								MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
						}
						else if( pInfo->GetLevel() == 2 )
						{
							GetGameUIMan()->MessageBox("", GetStringFromTable(609),
								MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
						}
						else
						{
							GetGameUIMan()->MessageBox("", GetStringFromTable(610),
								MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
						}
					}
				}
				else if (iService == CDLGNPC_FACTION_RENAME)
				{
					CDlgModifyName *pDlgModifyName = GetGameUIMan()->m_pDlgModifyName;
					if (!pDlgModifyName->IsShow()){
						pDlgModifyName->SetPolicy(new CDlgFactionRenamePolicy());
						pDlgModifyName->Show(true);
					}
				}
				else if( iService == CDLGNPC_FACTION_DISMISS )
				{
					GetGameUIMan()->MessageBox("Faction_Dismiss", GetStringFromTable(596),
						MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
				}
				GetGameUIMan()->EndNPCService();
				Show(false);
				return;
			}
			else if( iService == CDLGNPC_FACE && pEssence->combined_services & 0x20 )
			{
				if( GetHostPlayer()->IsFlying() ||
					GetHostPlayer()->IsSitting() ||
					GetHostPlayer()->IsShapeChanged() )
				{
					GetGameUIMan()->MessageBox("", GetStringFromTable(603),
						MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				}
				else
				{
					GetGameUIMan()->m_pDlgInventory->Show(true);
					GetGameUIMan()->m_pDlgFaceLift->Show(true);
					GetGameUIMan()->EndNPCService();
					Show(false);
				}
				return;
			}
			else if( iService == CDLGNPC_MAIL && pEssence->combined_services & 0x40 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgMailList->Show(true);
				return;
			}
			else if( iService == CDLGNPC_AUCTION && pEssence->combined_services & 0x80)
			{
				Show(false);
				if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					GetGameUIMan()->ShowErrorMsg(10130);
					GetGameUIMan()->EndNPCService();
				}
				else
				{
					GetGameSession()->auction_AttendList();
					GetGameUIMan()->m_pDlgAuctionBuyList->Show(true);
					GetGameUIMan()->m_pDlgAuctionBuyList->m_nIDNext = 0;
					GetGameUIMan()->m_pDlgAuctionBuyList->m_nPageNext = 0;
					ACString szText;
					szText.Format(GetStringFromTable(3501), GetStringFromTable(3509));
					GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_RefreshStatus->SetText(szText);
					GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_Search->
						SetText(GetStringFromTable(3509));
				}
				return;
			}
			else if( iService == CDLGNPC_DOUBLEEXP && pEssence->combined_services & 0x100)
			{
				Show(false);
				GetGameUIMan()->m_pDlgDoubleExp->Show(true);
				return;
			}
			else if( iService == CDLGNPC_PETHATCH && pEssence->combined_services & 0x200)
			{
				Show(false);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				GetGameUIMan()->m_pDlgPetHatch->Show(true);
				return;
			}
			else if( iService == CDLGNPC_PETREC && pEssence->combined_services & 0x400)
			{
				Show(false);
				GetGameUIMan()->m_pDlgPetList->Show(true);
				GetGameUIMan()->m_pDlgPetRec->Show(true);
				return;
			}
			else if( iService == CDLGNPC_BATTLECHALLENGE && pEssence->combined_services & 0x800)
			{
				Show(false);
				GetGameUIMan()->m_pDlgGuildMap->SetType(CDlgGuildMap::FACTIONMAP_TYPE_CHALLENGE);
				GetGameUIMan()->m_pDlgGuildMap->Show(true);
				GetGameUIMan()->m_pDlgGMapStatus2->Show(true);
				GetGameUIMan()->m_pDlgGMapStatus2->SetCanMove(false);
				return;
			}
			else if( iService == CDLGNPC_BATTLEENTER && pEssence->combined_services & 0x800)
			{
				Show(false);
				GetGameUIMan()->m_pDlgGuildMap->SetType(CDlgGuildMap::FACTIONMAP_TYPE_ENTER);
				GetGameUIMan()->m_pDlgGuildMap->Show(true);
				GetGameUIMan()->m_pDlgGMapStatus1->Show(true);
				GetGameUIMan()->m_pDlgGMapStatus1->SetCanMove(false);
				return;
			}
			else if( iService == CDLGNPC_BATTLELEAVE && pEssence->combined_services & 0x1000 )
			{
				Show(false);
				GetGameUIMan()->MessageBox("Game_BattleLeave", GetStringFromTable(725),
					MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
			else if( iService == CDLGNPC_GOLDTRADE && pEssence->combined_services & 0x2000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgGoldTrade->Show(true);
				return;
			}
			else if( iService == CDLGNPC_EQUIPREFINE && pEssence->combined_services & 0x4000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgEquipRefine->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_EQUIPDYE && pEssence->combined_services & 0x8000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgEquipDye->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				GetGameUIMan()->m_pDlgFittingRoom->Show(true);
				return;
			}
			else if( iService == CDLGNPC_PETDYE && pEssence->combined_services & 0x8000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgPetDye->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				GetGameUIMan()->m_pDlgPetList->Show(true);
				return;
			}
			else if( iService == CDLGNPC_REFINETRANS && pEssence->combined_services & 0x10000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgRefineTrans->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFPRORESET && pEssence->combined_services & 0x20000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFXiDian->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFGENIUSRESET && pEssence->combined_services & 0x40000 )
			{
				//新的对话框
				Show(false);
				GetGameUIMan()->m_pDlgELFGeniusReset->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFFORGET && pEssence->combined_services & 0x80000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFRetrain->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFREFINE && pEssence->combined_services & 0x100000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFRefine->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFREFINETRANS && pEssence->combined_services & 0x100000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFRefineTrans->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFTOPILL && pEssence->combined_services & 0x200000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFToPill->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if( iService == CDLGNPC_ELFEQUIPOFF && pEssence->combined_services & 0x400000 )
			{
				Show(false);
				GetGameUIMan()->m_pDlgELFEquipRemove->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_EQUIPREPAIR && pEssence->combined_services & 0x800000)
			{
				Show(false);
				GetGameUIMan()->m_pDlgDamageRep->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_WEBTRADE && pEssence->combined_services & 0x1000000)
			{
				Show(false);
				if(GetGameUIMan()->m_pDlgAutoLock->IsLocked())
				{
					GetGameUIMan()->AddChatMessage(GetStringFromTable(5520), GP_CHAT_MISC);
					GetGameUIMan()->EndNPCService();
				}
				else if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					GetGameUIMan()->ShowErrorMsg(10130);
					GetGameUIMan()->EndNPCService();
				}
				else
				{
					GetGameUIMan()->m_pDlgWebViewProduct->Show(true);
				}
				return;
			}
			else if (iService == CDLGNPC_GODEVILCONVERT && pEssence->combined_services & 0x2000000)
			{
				Show(false);
				GetGameUIMan()->m_pDlgGodEvilConvert->Show(true);
				return;
			}
			else if (iService == CDLGNPC_WEDDING_BOOK && pEssence->combined_services & 0x4000000)
			{
				Show(false);
				GetGameUIMan()->m_pDlgWedding->Show(true);
				return;
			}
			else if (iService == CDLGNPC_WEDDING_INVITE && pEssence->combined_services & 0x4000000)
			{
				Show(false);
				GetGameUIMan()->m_pDlgWeddingInvite->Show(true);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_CREATE && pEssence->combined_services & 0x8000000)
			{
				//	创建基地
				CECHostPlayer *pHost = GetHostPlayer();

				int idFaction = pHost->GetFactionID();
				CECFactionMan *pFMan = GetGame()->GetFactionMan();
				Faction_Info *pInfo = pFMan->GetFaction(idFaction);	
				
				const FACTION_FORTRESS_CONFIG *pConfig = pHost->GetFactionFortressConfig();

				if (pInfo && pConfig)
				{
					ACString strMsg = GetStringFromTable(9114);
					ACString strTemp;
					ACString strColorRed = _AL("^FF0000");
					ACString strColorWhite = _AL("^FFFFFF");
					ACString strNextLine = _AL("\r");

					bool bOK(true);

					//	帮派等级
					if (pInfo->GetLevel() < pConfig->require_level)
						bOK = false;

					strTemp.Format(GetStringFromTable(9112)
						, (pInfo->GetLevel() < pConfig->require_level) ? strColorRed : strColorWhite
						, pConfig->require_level+1);

					strMsg += strNextLine;
					strMsg += strTemp;

					//	道具需求
					CECInventory *pPack = pHost->GetPack();
					int nItems = sizeof(pConfig->require_item)/sizeof(pConfig->require_item[0]);
					for (int i = 0; i < nItems; ++ i)
					{
						if (pConfig->require_item[i].id && pConfig->require_item[i].count > 0)
						{
							int count = pPack->GetItemTotalNum(pConfig->require_item[i].id);
							if (count < pConfig->require_item[i].count)
								bOK = false;

							CECIvtrItem *pItem = CECIvtrItem::CreateItem(pConfig->require_item[i].id, 0, 1);
							strTemp.Format(GetStringFromTable(9113)
								, (count < pConfig->require_item[i].count) ? strColorRed : strColorWhite
								, pItem->GetName()
								, pConfig->require_item[i].count);
							delete pItem;
							
							strMsg += strNextLine;
							strMsg += strTemp;
						}
					}

					strMsg += strNextLine;
					strMsg += GetStringFromTable(bOK ? 9110 : 9111);

					if (!bOK)
					{
						//	条件不满足，显示具体信息
						GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
						GetGameUIMan()->EndNPCService();
					}
					else
					{						
						//	条件满足，弹出对话框确认
						GetGameUIMan()->MessageBox("Fortress_Create", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
					}
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}

				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_WAR && pEssence->combined_services & 0x8000000)
			{
				//	基地战
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressWar");
				if (pDlg1)
				{
					pDlg1->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_ENTER && pEssence->combined_services & 0x8000000)
			{
				//	进入基地
				GetGameSession()->factionFortress_Enter(GetHostPlayer()->GetFactionID());
				GetGameUIMan()->EndNPCService();
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_WARLIST && pEssence->combined_services & 0x8000000)
			{
				//	基地对战表
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressWarList");
				if (pDlg1)
				{
					pDlg1->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_UPGRADE && pEssence->combined_services & 0x10000000)
			{
				//	基地升级
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressInfo");
				PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_FortressBuild");
				if (pDlg1 && pDlg2)
				{
					pDlg1->Show(true);
					pDlg2->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_HANDIN_MATERIAL && pEssence->combined_services & 0x10000000)
			{
				//	缴纳材料
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressInfo");
				PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_FortressMaterial");
				if (pDlg1 && pDlg2)
				{
					pDlg1->Show(true);
					pDlg2->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_HANDIN_CONTRIB && pEssence->combined_services & 0x10000000)
			{
				//	缴纳贡献度
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressInfo");
				PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_FortressContrib");
				if (pDlg1 && pDlg2)
				{
					pDlg1->Show(true);
					pDlg2->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_LEAVE && pEssence->combined_services & 0x10000000)
			{
				//	离开基地
				GetGameSession()->c2s_CmdNPCSevFactionFortressLeave();
				GetGameUIMan()->EndNPCService();
				Show(false);
				return;
			}
			else if (iService == CGLDNPC_FORTRESS_EXCHANGE_MATERIAL && pEssence->combined_services & 0x20000000)
			{
				//	兑换材料
				PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_FortressInfo");
				PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_FortressExchange");
				if (pDlg1 && pDlg2)
				{
					pDlg1->Show(true);
					pDlg2->Show(true);
				}
				else
				{
					GetGameUIMan()->EndNPCService();
				}
				Show(false);
				return;
			}
			else if(iService == CDLGNPC_VIEW_TRASHBOX && pEssence->combined_services & 0x40000000)
			{
				GetGameSession()->c2s_CmdNPCSevViewTrash();
				Show(false);
				return;
			}
			else if(iService == CDLGNPC_VIEW_DPS_DPH_RANK && pEssence->combined_services & 0x80000000)
			{
				GetGameUIMan()->m_pDlgEarthBagRank->Show(true);
				Show(false);
				return;
			}
			else if((iService == CDLGNPC_FORCE_QUIT || iService == CDLGNPC_FORCE_JOIN) && pEssence->id_force_service > 0)
			{
				void *pData = m_pLst_Main->GetItemDataPtr(nCurSel);
				NPC_FORCE_SERVICE *pService = (NPC_FORCE_SERVICE *)pData;
				int npcForce = pService->force_id;

				PAUIDIALOG pDlg = NULL;
				if(iService == CDLGNPC_FORCE_QUIT) pDlg = GetGameUIMan()->GetDialog("Win_ForceQuit");
				else if(iService == CDLGNPC_FORCE_JOIN) pDlg = GetGameUIMan()->GetDialog("Win_ForceJoin");
				if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					GetGameUIMan()->ShowErrorMsg(10130);
					GetGameUIMan()->EndNPCService();
				}
				else if(pDlg)
				{
					pDlg->SetData(npcForce);
					pDlg->Show(true);
				}

				Show(false);
				return;
			}
			else if ((iService == CDLGNPC_JOIN_COUNTRY || iService == CDLGNPC_QUIT_COUNTRY) && pEssence->combined_services2 & 0x00000001)
			{
				if (iService == CDLGNPC_JOIN_COUNTRY)
				{
					CECHostPlayer *pHost = GetHostPlayer();
					ACString strMsg = GetStringFromTable(9765);
					ACString strTemp;
					ACString strColorRed = _AL("^FF0000");
					ACString strColorWhite = _AL("^FFFFFF");
					ACString strNextLine = _AL("\r");
					
					bool bOK(true);
					
					//	玩家等级
					const int REQUIRE_LEVEL = CECUIConfig::Instance().GetGameUI().nCountryWarEnterLevel;
					int iLevel = GetHostPlayer()->GetMaxLevelSofar();
					if (iLevel < REQUIRE_LEVEL) bOK = false;
					
					strTemp.Format(GetStringFromTable(9766)
						, (iLevel < REQUIRE_LEVEL) ? strColorRed : strColorWhite
						, REQUIRE_LEVEL);
					
					strMsg += strNextLine;
					strMsg += strTemp;
					
					//	道具需求
					const int REQUIRE_ITEM = CECUIConfig::Instance().GetGameUI().nCountryWarEnterItem;
					const int REQUIRE_COUNT = CECUIConfig::Instance().GetGameUI().nCountryWarEnterItemCount;
					CECInventory *pPack = pHost->GetPack();
					int count = pPack->GetItemTotalNum(REQUIRE_ITEM);
					if (count < REQUIRE_COUNT) bOK = false;
					
					CECIvtrItem *pItem = CECIvtrItem::CreateItem(REQUIRE_ITEM, 0, 1);
					strTemp.Format(GetStringFromTable(9767)
						, (count < REQUIRE_COUNT) ? strColorRed : strColorWhite
						, pItem->GetName()
						, REQUIRE_COUNT);
					delete pItem;
					
					strMsg += strNextLine;
					strMsg += strTemp;

					//	组队要求是队长
					CECTeam *pTeam = pHost->GetTeam();
					if (pTeam != NULL)
					{
						bool bLeader = (pTeam->GetLeaderID() == pHost->GetCharacterID());
						if (!bLeader) bOK = false;
						strTemp.Format(GetStringFromTable(9771)
							, (!bLeader) ? strColorRed : strColorWhite
							, GetStringFromTable(bLeader ? 9772 : 9773));
						strMsg += strNextLine;
						strMsg += strTemp;
					}
					
					//	冷却检查
					bool bCoolDown = pHost->GetCoolTime(GP_CT_COUNTRY_BATTLE_APPLY) > 0;
					if (bCoolDown)	bOK = false;
					strTemp.Format(GetStringFromTable(9774)
						, bCoolDown ? strColorRed : strColorWhite
						, GetStringFromTable(bCoolDown ? 9772 : 9773));
					strMsg += strNextLine;
					strMsg += strTemp;
					
					strMsg += strNextLine;
					strMsg += GetStringFromTable(bOK ? 9768 : 9769);
					
					if (!bOK)
					{
						//	条件不满足，显示具体信息
						GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
						GetGameUIMan()->EndNPCService();
					}
					else
					{						
						//	条件满足，弹出对话框确认
						PAUIDIALOG pDlgMsgBox = NULL;
						GetGameUIMan()->MessageBox("Country_JoinLeave", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pDlgMsgBox);
						pDlgMsgBox->SetData(1);
					}
				}
				else
				{
					//	离开确定
					PAUIDIALOG pDlgMsgBox = NULL;
					ACString strMsg = GetStringFromTable(9770);
					GetGameUIMan()->MessageBox("Country_JoinLeave", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pDlgMsgBox);
					pDlgMsgBox->SetData(2);
				}
				Show(false);
				return;
			}
			else if (iService == CDLGNPC_LEAVE_COUNTRY_WAR && pEssence->combined_services2 & 0x00000002)
			{
				//	离开确定
				ACString strMsg = GetStringFromTable(9915);
				GetGameUIMan()->MessageBox("Country_LeaveWar", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
				Show(false);
				return;
			}
			else if (iService == CDLGNPC_EQUIP_MARK && pEssence->combined_services2 & 0x00000004)
			{
				Show(false);
				GetGameUIMan()->m_pDlgEquipMark->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_GOTO_SPECIALSERVER && pEssence->combined_services2 & 0x00000008)
			{
				if (!CECCrossServer::Instance().IsOnSpecialServer())
				{
					CECHostPlayer *pHost = GetHostPlayer();
					ACString strMsg = GetStringFromTable(10134);
					ACString strTemp;
					ACString strColorRed = _AL("^FF0000");
					ACString strColorWhite = _AL("^FFFFFF");
					ACString strNextLine = _AL("\r");
					
					bool bOK(true);
					
					//	玩家等级
					const int REQUIRE_LEVEL = CECUIConfig::Instance().GetGameUI().nCrossServerEnterLevel;
					int iLevel = GetHostPlayer()->GetMaxLevelSofar();
					if (iLevel < REQUIRE_LEVEL) bOK = false;
					
					strTemp.Format(GetStringFromTable(9766)
						, (iLevel < REQUIRE_LEVEL) ? strColorRed : strColorWhite
						, REQUIRE_LEVEL);
					
					strMsg += strNextLine;
					strMsg += strTemp;
					
					//	修真要求
					const int REQUIRE_LEVEL2 = CECUIConfig::Instance().GetGameUI().nCrossServerEnterLevel2;
					int iLevel2 = GetHostPlayer()->GetBasicProps().iLevel2;
					if (iLevel2 < REQUIRE_LEVEL2) bOK = false;

					strTemp.Format(GetStringFromTable(10135)
						, (iLevel2 < REQUIRE_LEVEL2) ? strColorRed : strColorWhite
						, GetGameRun()->GetLevel2NameForCondition(REQUIRE_LEVEL2));
										
					strMsg += strNextLine;
					strMsg += strTemp;
					
					//	冷却检查
					bool bCoolDown = pHost->GetCoolTime(GP_CT_CROSS_SERVER_APPLY) > 0;
					if (bCoolDown)	bOK = false;
					strTemp.Format(GetStringFromTable(9774)
						, bCoolDown ? strColorRed : strColorWhite
						, GetStringFromTable(bCoolDown ? 9772 : 9773));

					strMsg += strNextLine;
					strMsg += strTemp;
					
					strMsg += strNextLine;
					strMsg += GetStringFromTable(bOK ? 10136 : 10137);
					
					if (!bOK)
					{
						//	条件不满足，显示具体信息
						GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
						GetGameUIMan()->EndNPCService();
					}
					else
					{						
						//	条件满足，弹出对话框确认
						GetGameUIMan()->MessageBox("CrossServer_GetIn", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
					}
				}
				Show(false);
				return;
			}
			else if (iService == CDLGNPC_GOTO_ORIGINALSERVER && pEssence->combined_services2 & 0x00000010)
			{
				if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					ACString strMsg = GetStringFromTable(10138);
					GetGameUIMan()->MessageBox("CrossServer_GetOut", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
				}
				Show(false);
				return;
			}
			else if (iService == CDLGNPC_PLAYER_RENAME && pEssence->combined_services2 & 0x00000020)
			{// 改名服务
				if (CECCrossServer::Instance().IsOnSpecialServer())
				{
					GetGameUIMan()->ShowErrorMsg(10130);
					GetGameUIMan()->EndNPCService();
				}
				else if (!CECUIConfig::Instance().GetGameUI().bEnablePlayerRename)
				{
					// 未开启改名服务则不显示该对话框
					GetGameUIMan()->ShowErrorMsg(10152);
					GetGameUIMan()->EndNPCService();
				}
				else
				{
					CDlgModifyName *pDlgModifyName = GetGameUIMan()->m_pDlgModifyName;
					if (!pDlgModifyName->IsShow()){
						pDlgModifyName->SetPolicy(new CDlgPlayerRenamePolicy());
						pDlgModifyName->Show(true);
					}
				}
				Show(false);
				return;
			}
			else if (iService == CDLGNPC_STONE_CHANGE && (pEssence->combined_services2 & 0x00000040))
			{
				Show(false);
				GetGameUIMan()->m_pDlgStoneChange->SetType(CDlgStoneChange::EM_ALL);
				GetGameUIMan()->m_pDlgStoneChange->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_KING_SERVICE && (pEssence->combined_services2 & 0x00000080))
			{
				Show(false);
				GetGameSession()->king_GetCandidateStatus();
				GetHostPlayer()->SetInKingService(true);
				return;
			}
			else if (iService == CDLGNPC_OFFLINESHOP_SETTING && (pEssence->combined_services2 & 0x00000100) )
			{
				Show(false);
				if( g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient() )
				{
					GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					return;
				}

				// Get shop info
				GetGameSession()->OffShop_GetMyShopInfo();
				GetHostPlayer()->GetOfflineShopCtrl()->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_SETTING);
				return;
			}
			else if (iService == CDLGNPC_OFFLINESHOP_SELLBUY && (pEssence->combined_services2 & 0x00000100))
			{
				Show(false);
				GetGameUIMan()->EndNPCService();
				if( g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient() )
				{
					GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					return;
				}

				if(GetGameUIMan()->m_pDlgAutoLock->IsLocked())
				{
					GetGameUIMan()->AddChatMessage(GetStringFromTable(10524), GP_CHAT_MISC);
				}
				else
				{
					// Get shop info
					GetGameSession()->OffShop_QueryShops(COfflineShopCtrl::OSTM_ALL);
					GetHostPlayer()->GetOfflineShopCtrl()->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_SELLBUY);
				}
				
				return;
			}
			else if (iService == CDLGNPC_SPLIT_FASHION && (pEssence->combined_services2 & 0x00000200))
			{
				Show(false);
				GetGameUIMan()->m_pDlgFashionSplit->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_REINCARNATION && (pEssence->combined_services2 & 0x00000400))
			{
				Show(false);
				GetGameUIMan()->m_pDlgReincarnation->Show(true);
				return;
			}
			else if(iService == CDLGNPC_GIFTCARD && (pEssence->combined_services2 & 0x00000800))
			{
				Show(false);
				GetGameUIMan()->m_pDlgGiftCard->Show(true);
				return;
			}
			else if(iService == CDLGNPC_TRICKBATTLE && (pEssence->combined_services2 & 0x00001000))
			{
				Show(false);
				GetGameUIMan()->m_pDlgChariotWarApply->Show(true);
				return;
			}
			else if(iService == CDLGNPC_CARDRESPAWN && (pEssence->combined_services2 & 0x00002000))
			{
				Show(false);
				GetGameUIMan()->m_pDlgGenCardRespawn->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}
			else if (iService == CDLGNPC_QUERYCHARIOTAMOUNT && (pEssence->combined_services2 & 0x00001000))
			{
				Show(false);
				GetGameSession()->tankBattle_GetRank();
				GetGameUIMan()->m_pDlgChariotRankList->SetNPCSevFlag(true);
				if(!GetGameUIMan()->m_pDlgChariotRankList->IsShow())
					GetGameUIMan()->m_pDlgChariotRankList->Show(true);
				return;
			}
			else if(iService == CDLGNPC_FLYSWORDIMPROVE && (pEssence->combined_services2 & 0x00004000))
			{
				Show(false);
				GetGameUIMan()->m_pDlgFlySwordImprove->Show(true);
				GetGameUIMan()->m_pDlgInventory->Show(true);
				return;
			}else if (iService == CDLGNPC_OPEN_FACTION_PVP && (pEssence->combined_services2 & 0x00008000)){
				Show(false);
				ACString strMsg = GetStringFromTable(11271);
				GetGameUIMan()->MessageBox("Faction_PVP_Open", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}else if (iService == CDLGNPC_GOLD_SHOP && ((pEssence->combined_services2 & 0x00010000) || (pEssence->combined_services2 & 0x00020000))){
				Show(false);
				bool bEnableQShop = (pEssence->combined_services2 & 0x00010000);
				bool bEnableBackShop = (pEssence->combined_services2 & 0x00020000);
				if (bEnableQShop){
					CECUIHelper::ShowQShop(pEssence->id, bEnableBackShop);
				}else if (bEnableBackShop){
					CECUIHelper::ShowBackShop(pEssence->id, bEnableQShop);
				}
				return;
			}else if (iService == CDLGNPC_PLAYER_CHANGE_GENDER && pEssence->combined_services2 & 0x00040000){
				if (GetGameUIMan()->m_pDlgAutoLock->IsLocked()){
					GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(10524), GP_CHAT_MISC);
					GetGameUIMan()->EndNPCService();
				}else if (CECCrossServer::Instance().IsOnSpecialServer()){
					GetGameUIMan()->ShowErrorMsg(10130);
					GetGameUIMan()->EndNPCService();
				}else if (!CECUIConfig::Instance().GetGameUI().bEnablePlayerChangeGender){
					GetGameUIMan()->ShowErrorMsg(10152);
					GetGameUIMan()->EndNPCService();
				}else{
					CDlgSimpleCostItemService *pDlgSimpleCostItemService = GetGameUIMan()->m_pDlgSimpleCostItemService;
					if (!pDlgSimpleCostItemService->IsShow()){
						GetHostPlayer()->StopAutoFashion();
						pDlgSimpleCostItemService->SetPolicy(new CDlgChangeGenderPolicy());
						pDlgSimpleCostItemService->Show(true);
					}
				}
				Show(false);
				return;
			}
		}

		int idFunction, id_dialog = 0;
		void *pData = m_pLst_Main->GetItemDataPtr(nCurSel);

		g_pGame->GetElementDataMan()->get_data_ptr(iService, ID_SPACE_ESSENCE, DataType);

		if( DataType == DT_NPC_TASK_IN_SERVICE ||
			DataType == DT_NPC_TASK_OUT_SERVICE ||
			DataType == DT_NPC_TASK_MATTER_SERVICE )
		{
			// pop-up storage task dialog
			if(DataType == DT_NPC_TASK_OUT_SERVICE)
			{
				NPC_TASK_OUT_SERVICE *pService = (NPC_TASK_OUT_SERVICE *)pData;
				if( !(pService->storage_id == 0 || pService->storage_open_item == 0) )
				{
					PopupStorageTaskDialog(pService, false);
					return;
				}
			}

			PopupTaskDialog(false);
			m_pLst_Main->AddString(GetStringFromTable(503));
			return;
		}
		else if( DataType == DT_NPC_TALK_SERVICE )
		{
			NPC_TALK_SERVICE *pService = (NPC_TALK_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_TALK;
		}
		else if( DataType == DT_NPC_SELL_SERVICE )
		{
			NPC_SELL_SERVICE *pService = (NPC_SELL_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_SELL;
		}
		else if( DataType == DT_NPC_BUY_SERVICE )
		{
			NPC_BUY_SERVICE *pService = (NPC_BUY_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_BUY;
		}
		else if( DataType == DT_NPC_REPAIR_SERVICE )
		{
			NPC_REPAIR_SERVICE *pService = (NPC_REPAIR_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_REPAIR;
		}
		else if( DataType == DT_NPC_INSTALL_SERVICE )
		{
			NPC_INSTALL_SERVICE *pService = (NPC_INSTALL_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_INSTALL;
		}
		else if( DataType == DT_NPC_UNINSTALL_SERVICE )
		{
			NPC_UNINSTALL_SERVICE *pService = (NPC_UNINSTALL_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_UNINSTALL;
		}
		else if( DataType == DT_NPC_SKILL_SERVICE )
		{
			NPC_SKILL_SERVICE *pService = (NPC_SKILL_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_SKILL;
		}
		else if( DataType == DT_NPC_HEAL_SERVICE )
		{
			NPC_HEAL_SERVICE *pService = (NPC_HEAL_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_HEAL;
		}
		else if( DataType == DT_NPC_TRANSMIT_SERVICE )
		{
			NPC_TRANSMIT_SERVICE *pService = (NPC_TRANSMIT_SERVICE *)pData;
			idFunction = NPC_TRANSMIT;
		}
		else if( DataType == DT_NPC_TRANSPORT_SERVICE )
		{
			NPC_TRANSPORT_SERVICE *pService = (NPC_TRANSPORT_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_TRANSPORT;
		}
		else if( DataType == DT_NPC_PROXY_SERVICE )
		{
			NPC_PROXY_SERVICE *pService = (NPC_PROXY_SERVICE *)pData;
			id_dialog = pService->id_dialog;
			idFunction = NPC_PROXY;
		}
		else if( DataType == DT_NPC_STORAGE_SERVICE )
		{
			NPC_STORAGE_SERVICE *pService = (NPC_STORAGE_SERVICE *)pData;
			ACString strText = m_pLst_Main->GetText(nCurSel);
			ACString strComp1 = ACString(GetStringFromTable(249)) + ACString(GetStringFromTable(253));
			ACString strComp2 = ACString(GetStringFromTable(249)) + ACString(GetStringFromTable(8080));

			if( 0 == a_stricmp(strText, strComp1) )
				idFunction = NPC_STORAGE_PASSWORD;
			else if (0 == a_stricmp(strText, strComp2))
				idFunction = NPC_ACCOUNT_STORAGE;
			else
				idFunction = NPC_STORAGE;
		}
		else if( DataType == DT_NPC_MAKE_SERVICE )
		{
			NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)pData;
			idFunction = NPC_MAKE;
		}
		else if( DataType == DT_NPC_DECOMPOSE_SERVICE )
		{
			NPC_DECOMPOSE_SERVICE *pService = (NPC_DECOMPOSE_SERVICE *)pData;
			idFunction = NPC_DECOMPOSE;
		}
		else if( DataType == DT_NPC_IDENTIFY_SERVICE )
		{
			NPC_IDENTIFY_SERVICE *pService = (NPC_IDENTIFY_SERVICE *)pData;
			idFunction = NPC_IDENTIFY;
		}
		else if( DataType == DT_NPC_WAR_TOWERBUILD_SERVICE)
		{
			NPC_WAR_TOWERBUILD_SERVICE *pService = (NPC_WAR_TOWERBUILD_SERVICE *)pData;
			idFunction = NPC_WAR_TOWERBUILD;
		}
		else if( DataType == DT_NPC_RESETPROP_SERVICE)
		{
			NPC_RESETPROP_SERVICE *pService = (NPC_RESETPROP_SERVICE *)pData;
			idFunction = NPC_RESETPROP;
		}
		else if( DataType == DT_NPC_PETNAME_SERVICE )
		{
			NPC_PETNAME_SERVICE *pService = (NPC_PETNAME_SERVICE *)pData;
			idFunction = NPC_PETNAME;
		}
		else if( DataType == DT_NPC_PETLEARNSKILL_SERVICE )
		{
			NPC_PETLEARNSKILL_SERVICE *pService = (NPC_PETLEARNSKILL_SERVICE *)pData;
			idFunction = NPC_PETLEARNSKILL;
		}
		else if( DataType == DT_NPC_PETFORGETSKILL_SERVICE )
		{
			NPC_PETFORGETSKILL_SERVICE *pService = (NPC_PETFORGETSKILL_SERVICE *)pData;
			idFunction = NPC_PETFORGETSKILL;
		}
		else if( DataType == DT_NPC_EQUIPBIND_SERVICE )
		{
			NPC_EQUIPBIND_SERVICE *pService = (NPC_EQUIPBIND_SERVICE *)pData;
			idFunction = NPC_EQUIPBIND;
		}
		else if( DataType == DT_NPC_EQUIPDESTROY_SERVICE )
		{
			NPC_EQUIPDESTROY_SERVICE *pService = (NPC_EQUIPDESTROY_SERVICE *)pData;
			idFunction = NPC_EQUIPDESTROY;
		}
		else if( DataType == DT_NPC_EQUIPUNDESTROY_SERVICE )
		{
			NPC_EQUIPUNDESTROY_SERVICE *pService = (NPC_EQUIPUNDESTROY_SERVICE *)pData;
			idFunction = NPC_EQUIPUNDESTROY;
		}
		else if (DataType == DT_NPC_ENGRAVE_SERVICE)
		{
			NPC_ENGRAVE_SERVICE *pService = (NPC_ENGRAVE_SERVICE *)pData;
			idFunction = NPC_ENGRAVE;
		}
		else if(DataType == DT_NPC_RANDPROP_SERVICE)
		{
			NPC_RANDPROP_SERVICE *pService = (NPC_RANDPROP_SERVICE *)pData;
			idFunction = NPC_RANDPROP;
		}

		if( id_dialog > 0 )
		{
			talk_proc *pTalk = (talk_proc *)g_pGame->GetElementDataMan()
				->get_data_ptr(id_dialog, ID_SPACE_TALK, DataType);

			if( pTalk )
			{
				GetGameUIMan()->PopupNPCDialog(pTalk);
				m_pTxt_Content->SetData(0xFFFFFFFF);
				SetData(NPC_DIALOG_TALK);
			}
		}
		else
		{
			void *pData = m_pLst_Main->GetItemDataPtr(nCurSel);
			PopupCorrespondingServiceDialog(idFunction, iService, pData);
		}
	}
}

void CDlgNPC::PopupStorageTaskDialog(NPC_TASK_OUT_SERVICE *pService, bool bTaskNPC)
{
	NPC_ESSENCE *pCurNPCEssence = GetGameUIMan()->m_pCurNPCEssence;

	if(GetHostPlayer()->GetPack()->GetItemTotalNum(pService->storage_open_item) > 0)
	{
		Show(false);
		
		PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_QuestList");
		if(pDlg)
		{
			pDlg->Show(true);
		} 
		else // in case the dialog is missing or name changed...
		{
			ASSERT(false);
			GetGameUIMan()->EndNPCService();
		}
	}
	else
	{
		CECIvtrItem* pItem = CECIvtrItem::CreateItem(pService->storage_open_item, 0, 1);
		ACString szMsg;
		// cannot open storage task
		m_pLst_Main->ResetContent();
		m_pTxt_Content->SetText( bTaskNPC ? pCurNPCEssence->hello_msg : szMsg.Format(GetStringFromTable(984), pItem->GetName()));
		delete pItem;
		SetData(NPC_DIALOG_TASK_LIST);
	}
}

void CDlgNPC::CheckTaskService(const NPC_ESSENCE *pEssence)
{
	unsigned int a_uiService2[] = 
	{
		pEssence->id_task_out_service,		// 任务相关的服务ID: 发放任务服务
		pEssence->id_task_in_service,		// 任务相关的服务ID: 验证完成任务服务
		pEssence->id_task_matter_service,	// 任务相关的服务ID: 发放任务物品服务
	};

	const void *pData;
	DATA_TYPE DataType;
	ACString strText = GetStringFromTable(249);
	elementdataman *pDataMan = g_pGame->GetElementDataMan();

	// flag if this NPC contains task related service
	unsigned int validTaskService = 0;

	// flag if we only got storage task service
	bool isStorageTaskOnly = true; 

	// flag if storage task exists
	NPC_TASK_OUT_SERVICE *pStorageService = NULL;
	
	//
	// this loop will set values to these flags by checking current NPC data
	//
	for(int i = 0; i < sizeof(a_uiService2) / sizeof(unsigned int); i++ )
	{
		if( !a_uiService2[i] ) continue;
		
		pData = pDataMan->get_data_ptr(a_uiService2[i], ID_SPACE_ESSENCE, DataType);
		
		// ignore other service
		if( DataType != DT_NPC_TASK_IN_SERVICE &&
			DataType != DT_NPC_TASK_OUT_SERVICE &&
			DataType != DT_NPC_TASK_MATTER_SERVICE )
		{
			continue;
		}

		if(DataType != DT_NPC_TASK_OUT_SERVICE)
		{
			validTaskService = a_uiService2[i];
			isStorageTaskOnly = false;
		}
		else
		{
			// do not update if we got other task service already
			if( !validTaskService ) validTaskService = a_uiService2[i];
			
			// check whether this is a task storage
			NPC_TASK_OUT_SERVICE *pService = (NPC_TASK_OUT_SERVICE *)pData;
			if(pService->storage_id == 0 || pService->storage_open_item == 0)
			{
				isStorageTaskOnly = false;
			} else {
				pStorageService = pService;
			}	
		}
	}
	// make sure this flag is not conflicted with others
	isStorageTaskOnly = isStorageTaskOnly && validTaskService && pStorageService;
	

	// setup the menu item based on these flags
	if(validTaskService)
	{
		bool isEmptyMenu = (m_pLst_Main->GetCount() <= 0);
		
		if( isEmptyMenu && !pStorageService)
		{
			//	没有非任务选项，直接弹出任务内容
			PopupTaskDialog(true);
		}
		else 
		{
			if(pStorageService)
			{
				// add a menu item for task storage
				m_pLst_Main->AddString(strText + _AL("^FF0000") + pStorageService->name);
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, pEssence->id_task_out_service);
				m_pLst_Main->SetItemDataPtr(m_pLst_Main->GetCount() - 1, (void *)pStorageService);
			}

			isEmptyMenu = (m_pLst_Main->GetCount() <= 0);
			
			if(!isEmptyMenu && !isStorageTaskOnly)
			{
				pData = pDataMan->get_data_ptr(validTaskService, ID_SPACE_ESSENCE, DataType);

				//	有其它非任务内容，把所有任务相关集中放在“任务相关”栏里
				m_pLst_Main->AddString(strText + GetStringFromTable(244));
				m_pLst_Main->SetItemData(m_pLst_Main->GetCount() - 1, validTaskService);
				m_pLst_Main->SetItemDataPtr(m_pLst_Main->GetCount() - 1, (void *)pData);
			}
		}
	}
}

void CDlgNPC::SetTraceTaskInfo( int tid, int taskid )
{
	m_iTracedTaskNPCID = tid;
	m_iTracedTaskID = taskid;
	m_tracedTaskTimer.Reset(3000);
}

void CDlgNPC::ResetTraceTaskInfo()
{
	m_iTracedTaskNPCID = 0;
	m_iTracedTaskID = 0;
	m_tracedTaskTimer.Reset();
}

bool CDlgNPC::PopupTracedTaskDialog( const NPC_ESSENCE *pEssence )
{
	PopupNPCDialog(pEssence);
	if (GetData() == NPC_DIALOG_ESSENCE){
		for (int i(0); i < m_pLst_Main->GetCount()-1; ++ i){
			void *pData = m_pLst_Main->GetItemDataPtr(i);			
			int iService = (int)m_pLst_Main->GetItemData(i);
			DATA_TYPE DataType = DT_INVALID;
			g_pGame->GetElementDataMan()->get_data_ptr(iService, ID_SPACE_ESSENCE, DataType);			
			if( DataType == DT_NPC_TASK_IN_SERVICE ||
				DataType == DT_NPC_TASK_OUT_SERVICE ||
				DataType == DT_NPC_TASK_MATTER_SERVICE ){
				SelectListItem(i);
				break;
			}
		}
	}
	if (GetData() == NPC_DIALOG_TASK_LIST){
		for (int i(0); i < m_pLst_Main->GetCount()-1; ++ i){			
			UINT64 taskid = m_pLst_Main->GetItemData64(i,0,"TaskID");
			if (taskid == m_iTracedTaskID){
				SelectListItem(i);
				return GetData() == NPC_DIALOG_TASK_TALK;
			}
		}
	}
	return false;
}

void CDlgNPC::PopupDialog( const NPC_ESSENCE *pEssence )
{
	if (HasTracedTask()){
		if (m_tracedTaskTimer.IsTimeArrived()){
			ResetTraceTaskInfo();
		}
	}
	if (HasTracedTask()){
		CECNPC* pNPC = dynamic_cast<CECNPC*>(g_pGame->GetGameRun()->GetWorld()->GetObject(m_iTracedTaskNPCID, 0));
		if (pNPC){
			if (pEssence->id == pNPC->GetTemplateID()){
				if (!PopupTracedTaskDialog(pEssence)){
					if (!IsShow()){
						ResetTraceTaskInfo();
						return;
					}
					PopupNPCDialog(pEssence);
				}
			}
		}else{
			GetGameUIMan()->EndNPCService();
		}
		ResetTraceTaskInfo();
	}else{
		PopupNPCDialog(pEssence);
	}
}

bool CDlgNPC::HasTracedTask(){
	return m_iTracedTaskID > 0;
}