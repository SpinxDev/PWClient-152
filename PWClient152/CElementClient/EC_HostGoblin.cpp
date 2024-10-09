#include <A3DTypes.h>
#include <A3DBone.h>
#include <A3DSkinModel.h>
#include <A3DSkinMan.h>

#include "EC_HostGoblin.h"
#include "EC_Model.h"
#include "EC_Viewport.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_IvtrGoblin.h"

#include "EC_Inventory.h"
#include "EC_Skill.h"
#include "EC_GPDataType.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_ElsePlayer.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "DlgQuickBar.h"
#include "EC_ManPlayer.h"
#include "EC_Faction.h"
#include "ExpTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostGoblin
//	
///////////////////////////////////////////////////////////////////////////
CECHostGoblin::CECHostGoblin()
{
	m_iSecurityState	= 0;
	m_pEquipPack		= NULL;
	m_iLevel			= 0;
	m_iVigor			= 0;
	m_iMaxVigor			= 100;
	m_fVigorGen			= 1.0f;
	m_iVigor			= 1;
	m_iGrowDegree		= 0;
	m_iMaxRandomPt		= 100;
	m_iCurrMaxSkillNum	= CECIvtrGoblin::INIT_SKILL_NUM;

	// init skills
	int i;
	for(i=0;i<MAX_SKILLNUM;i++)
	{
		m_aSkills[i] = NULL;
	}

	// init equipment package
 	m_pEquipPack = NULL;
}
CECHostGoblin::~CECHostGoblin()
{
	Release();
}

//	Initialize object
bool CECHostGoblin::Init(int tid, CECIvtrGoblin* pIvtrGoblin, CECPlayer* pPlayer)
{
	CECGoblin::Init(tid, pIvtrGoblin, pPlayer);

	m_iTotalStatusPt	= pIvtrGoblin->GetEssence().data.total_attribute;
	m_iLevel = pIvtrGoblin->GetEssence().data.level;

	m_iSecurityState	= pIvtrGoblin->GetEssence().data.status_value;
	m_iExp				= pIvtrGoblin->GetEssence().data.exp;
	m_iGrowDegree		= pIvtrGoblin->GetGrowDegree();
	m_iMaxRandomPt		= pIvtrGoblin->GetMaxRandomStatusPt();
	m_iCurrMaxSkillNum	= pIvtrGoblin->GetCurrMaxSkillNum();

	int i=0;

	for(i=0;i<5;i++)
		m_aGenius[i] = pIvtrGoblin->GetEssence().data.genius[i];
	
	m_iStrength			= pIvtrGoblin->GetEssence().data.strength;
	m_iAgility			= pIvtrGoblin->GetEssence().data.agility;
	m_iVitality			= pIvtrGoblin->GetEssence().data.vitality;
	m_iEnergy			= pIvtrGoblin->GetEssence().data.energy;

	// Init enhanced props by buff skill
	m_iStrEnhance		= 0;
	m_iAgiEnhance		= 0;
	m_iVitEnhance		= 0;
	m_iEngEnhance		= 0;
	
	m_iTotalGeniusPt	= pIvtrGoblin->GetEssence().data.total_genius;
	m_iStamina			= pIvtrGoblin->GetEssence().data.stamina;
	
	m_iRefineLvl		= pIvtrGoblin->GetEssence().data.refine_level;

	// init skills
	for(i=0; i<pIvtrGoblin->GetEssence().skill_cnt; i++)
	{
		GOBLINSKILL gSkill = pIvtrGoblin->GetSkill(i);

// 		if(m_aSkills[i])
// 		{
// 			delete m_aSkills[i];
// 			m_aSkills[i] = NULL;
//		}
		m_aSkills[i] = ((CECHostPlayer*)m_pPlayer)->GetNormalSkill(gSkill.skill);
		if(m_aSkills[i]) // Skill already exists in CECHostPlayer::m_aGoblinSkill
		{
			m_aSkills[i]->SetLevel(gSkill.level);
		}
		else
		{
			m_aSkills[i] = new CECSkill(gSkill.skill, gSkill.level);
			((CECHostPlayer*)m_pPlayer)->AddGoblinSkill(m_aSkills[i]);
		}		
	}

	// init equipments 
	if( m_pEquipPack )
	{
		m_pEquipPack->Release();
		delete m_pEquipPack;
		m_pEquipPack = NULL;
	}
	if( !(m_pEquipPack = new CECInventory))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECHostGoblin::Init", __LINE__);
		return false;		
	}
	
	if( !(m_pEquipPack->Init(4)))
	{
		a_LogOutput(1, "CECHostGoblin::Init, Failed to initialize inventories");
		return false;	
	}

	for(i=0; i< 4; i++)
	{
		//  Get equipment id at position i 
		int iEquipID = pIvtrGoblin->GetEquip(i);
		if( iEquipID != 0)
		{
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(iEquipID, 0, 1);
			m_pEquipPack->PutItem(i, ((CECIvtrGoblinEquip*)pItem));
		}
	}

	return true;
}

//	Release object
void CECHostGoblin::Release()
{
	if( m_pEquipPack )
	{
		m_pEquipPack->Release();
		delete m_pEquipPack;
		m_pEquipPack = NULL;
	}

	// Delete shortcut of goblin skill first
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	AString dlgName;
	dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET1);
	if(pGameUI && pGameUI->GetDialog(dlgName))
		((CDlgQuickBar*)(pGameUI->GetDialog(dlgName)))->DelGoblinSkillSC();	

// 	int i;
// 	for(i=0; i<MAX_SKILLNUM; i++)
// 	{
// 		if( m_aSkills[i] )
// 		{
// 			delete m_aSkills[i];
// 			m_aSkills[i] = NULL;
// 		}
//	}

	if(m_pEquipPack)
	{
		m_pEquipPack->Release();
		delete m_pEquipPack;
		m_pEquipPack = NULL;
	}
}
//	Tick routine
bool CECHostGoblin::Tick(DWORD dwDeltaTime)
{
	CECGoblin::Tick(dwDeltaTime);
	
//////////////////////////////////////////////////////////////////////////////////// make goblin face target
	if(m_pPlayer->IsFighting())
	{
		//	Face to target
		A3DVECTOR3 vTarget;
		int idTarget = ((CECHostPlayer*)m_pPlayer)->GetSelectedTarget();
		CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 0);
		if (pObject)
		{

			if (ISNPCID(idTarget))
			{
				CECNPC* pNPC = (CECNPC*)pObject;
				vTarget = pNPC->GetPos();
			}
			else if (ISPLAYERID(idTarget))
			{
				CECPlayer* pPlayer = (CECPlayer*)pObject;
				vTarget = pPlayer->GetPos();
			}
			else
			{
				vTarget = m_pPlayer->GetPos();
			}

			A3DVECTOR3 vDir = vTarget - m_pPlayer->GetPos();
			vDir.y = 0.0f;
			vDir.Normalize();

			m_pModel->SetDirAndUp(vDir, A3DVECTOR3(0.0f,1.0f,0.0f));
		}
	}
/////////////////////////////////////////////////////////////////////////////////////
	
	//  Tick Skill
// 	for(int i=0;i<GetSkillNum();i++)
// 	{
// 		m_aSkills[i]->Tick();
// 	}

	return true;
}
//	Render routine
bool CECHostGoblin::Render(CECViewport* pViewport)
{
	CECGoblin::Render(pViewport);
	return true;
}
//  小精灵基础说明文字
const wchar_t* CECHostGoblin::GetBasicDescText()
{
	const wchar_t* szDesc = g_pGame->GetItemExtDesc(m_tid);
	if (!szDesc || !szDesc[0])
		return NULL;

	return szDesc;
}
//  计算小精灵实际获取的经验值， iExp为输入的经验值
//  iLevel为经验丸的等级， > 0 则说明从经验丸注入，为0则为从主角经验值输入
unsigned int CECHostGoblin::InjectExp(unsigned int iExp, int iLevel)
{
	ASSERT(iExp>=0 && iLevel >= 0);

	double factor = 0.0f;
	if(iLevel == 0)
	{
		int iPlayerLvl = ((CECHostPlayer*)m_pPlayer)->GetMaxLevelSofar();
		a_Clamp(iPlayerLvl, 0, 150);
		factor = (double)CECIvtrGoblin::elf_exp_loss_constant[m_iLevel]/(double)CECIvtrGoblin::elf_exp_loss_constant[iPlayerLvl];
	}
	else
		factor = (double)CECIvtrGoblin::elf_exp_loss_constant[m_iLevel]/(double)CECIvtrGoblin::elf_exp_loss_constant[iLevel];
	
	if(factor > 1.0)
		factor = 1.0;
	if(factor < 0.1)
		factor = 0.1;

	return (unsigned int)(factor*iExp);
}
//  计算使小精灵经验增加指定值所需的主角经验值
unsigned int CECHostGoblin::NeedPlayerExp(unsigned int goblin_exp)
{
	ASSERT(goblin_exp>=0);

	int iPlayerLvl = ((CECHostPlayer*)m_pPlayer)->GetMaxLevelSofar();
	a_Clamp(iPlayerLvl, 0, 150);
	double factor = 0.0f;		// 经验注入折损系数
	factor = (double)CECIvtrGoblin::elf_exp_loss_constant[m_iLevel]/(double)CECIvtrGoblin::elf_exp_loss_constant[iPlayerLvl];

	ASSERT(factor <= 1 && factor > 0);
	if(factor > 1.0)
		factor = 1.0;
	if(factor < 0.1)
		factor = 0.1;

	unsigned int iRet = (unsigned int)(goblin_exp/factor);

	if(fabs(goblin_exp/factor - iRet) < 0.00001)
		return iRet;

	return iRet + 1;
}
//  计算使小精灵经验增加指定值所需的等级为iLevel的经验丸中的经验值
unsigned int CECHostGoblin::NeedPillExp(unsigned int goblin_exp, int iLevel)
{
	ASSERT(goblin_exp>=0);

	// 若经验丸等级小于小精灵等级，则100%注入
	if(iLevel <= m_iLevel)
		return goblin_exp;

	double factor = 0.0f;		// 经验注入折损系数
	factor = (double)CECIvtrGoblin::elf_exp_loss_constant[m_iLevel]/(double)CECIvtrGoblin::elf_exp_loss_constant[iLevel];

	ASSERT(factor <= 1 && factor > 0);	
	if(factor > 1.0)
		factor = 1.0;
	if(factor < 0.1)
		factor = 0.1;

	unsigned int iRet = (int)(goblin_exp/factor);

	if(fabs(goblin_exp/factor - iRet) < 0.00001)
		return iRet;

	return iRet + 1;
}
//	获取已学习的技能数量
int CECHostGoblin::GetSkillNum()
{
	int iRet = 0;
	while(m_aSkills[iRet] != NULL && iRet < MAX_SKILLNUM)
		iRet++;

	return iRet;
}
//	获取由id指定的技能
CECSkill* CECHostGoblin::GetSkillByID(int id)
{
	for(int i=0;i<GetSkillNum();i++)
	{
		if(m_aSkills[i]->GetSkillID() == id)
		{
			return m_aSkills[i];
		}
	}

	return NULL;
}	

//  计算小精灵属性点更改需要的点数
//  iBase为当前属性点的值+玩家点击值
//  iNum为1或-1,表示加减一
int CECHostGoblin::NeedStatusPt(int iBase, int iNum)
{
	ASSERT(iNum == 1 || iNum == -1);
	int iRet = 0;

	if(iBase >= 40)
	{
		iRet = (iNum==1) ? 2 : -2;
	}
	else
	{
		iRet = (iNum==1) ? 1 : -1;
	}

	return iRet;
}
//  小精灵当前未使用的属性点数
int CECHostGoblin::UnusedStatusPt()
{
	int iStr = m_iStrength;
	int iAgi = m_iAgility;
	int iVit = m_iVitality;
	int iEng = m_iEnergy;

	if(m_iStrength > 40)
		iStr = (m_iStrength-40)*2 + 40;

	if(m_iAgility > 40)
		iAgi = (m_iAgility-40)*2 + 40;

	if(m_iVitality > 40)
		iVit = (m_iVitality-40)*2 + 40;

	if(m_iEnergy > 40)
		iEng = (m_iEnergy-40)*2 + 40;

	return m_iTotalStatusPt-iStr-iAgi-iVit-iEng; 
}
//  金木水火土属性点，bAddEquip为true表示加上装备的
int CECHostGoblin::GetGenius(int index, bool bAddEquip)
{
	ASSERT(index >= 0&&index < 5);
	int iRet = m_aGenius[index], i;

	if(bAddEquip)		// 加上装备的
	{
		for(i=0;i<4;i++)
		{
			CECIvtrGoblinEquip* pGoblinEquip = (CECIvtrGoblinEquip*)m_pEquipPack->GetItem(i);
			if(pGoblinEquip)
			{
				const GOBLIN_EQUIP_ESSENCE* pEssence = pGoblinEquip->GetDBEssence();
				iRet += pEssence->magic[index];
			}
		}
	}

	return (iRet > 8)?8:iRet;
}
//  精炼时耐力消耗速度
float CECHostGoblin::GetMPCost()
{
	// 精练等级对应表
	const int refine_lvl_data[] = {0, // no use
		10,10,10,10,10,10,10,20,30,40,
		50,60,70,80,90,100,110,120,130,140,
		150,160,170,180,190,200,210,220,230,240,
		250,260,270,280,290,300};

	return refine_lvl_data[m_iRefineLvl]*(((CECHostPlayer*)m_pPlayer)->GetMaxLevelSofar() + 105)/210.0f;
}


//  小精灵当前未使用的天赋点数
int CECHostGoblin::UnusedGeniusPt()
{
	int iSum = 0, i;
	for(i=0;i<5;i++)
		iSum += m_aGenius[i];

	return (m_iTotalGeniusPt - iSum);
}
//	判断小精灵是否可以释放指定的技能
//  0:成功            1:职业不匹配        2:mp不足
//  3:AP不足		  4:天赋点不足		  5:错误ID
//  6:未选择目标      7:非小精灵技能      8:移动环境不满足
//  9:技能个数限制	  10:技能需消耗物品不存在， 11: 战车禁止精灵技能
int CECHostGoblin::CheckSkillCastCondition(CECSkill* pSkill)
{
	GNET::GoblinUseRequirement Info;
	memset(&Info, 0, sizeof (Info));

	CECHostPlayer *pHost = static_cast<CECHostPlayer *>(m_pPlayer);

	if (pHost->GetBattleInfo().IsChariotWar())
		return 11; // 战车 禁止精灵技能

	int idItem = pSkill->GetRequiredItem();
	if (idItem > 0 &&
		pHost->GetPack()->GetItemTotalNum(idItem) <= 0)
	{
		return 10;
	}

//	if(((CECHostPlayer*)m_pPlayer)->GetSelectedTarget() == 0)
//		return 6;

	int i;
	if(GetSkillNum() > GetCurrMaxSkillNum())
	{
		for(i=0;i<GetSkillNum(); i++)
		{
			if(m_aSkills[i]->GetSkillID() == pSkill->GetSkillID())
				break;
		}

		if(i+1>GetCurrMaxSkillNum())
			return 9;
	}

	for(i=0;i<5;i++)
	{
		Info.genius[i] = GetGenius(i);
	}
	
 	Info.profession		= m_pPlayer->GetProfession();
 	Info.mp				= m_iVigor;
 	Info.ap				= m_iStamina;
	Info.level			= m_iLevel;
	Info.move_env		= m_pPlayer->GetMoveEnv();

	return GNET::ElementSkill::GoblinCondition(pSkill->GetSkillID(), Info, pSkill->GetSkillLevel());
}
//	Process message
bool CECHostGoblin::ProcessMessage(const ECMSG& Msg)
{
	using namespace S2C;
	
	switch (Msg.dwParam2)
	{
	case ELF_VIGOR:			OnMsgGoblinVigor(Msg);			break;
	case ELF_ENHANCE:		OnMsgGoblinEnhance(Msg);		break;
	case ELF_STAMINA:		OnMsgGoblinStamina(Msg);		break;
	case ELF_EXP:			OnMsgGoblinExp(Msg);			break;	

	default:
		CECGoblin::ProcessMessage(Msg);
 	}
	return true;
}

//	Cast skill
bool CECHostGoblin::CastSkill(int iIndex, int idTarget, bool bForceAttack)
{
	ASSERT(iIndex >= 0 && iIndex < GetSkillNum());
	if(m_aSkills[iIndex] == NULL)
	{
		ASSERT(0);
		return false;
	}	

	if (m_aSkills[iIndex]->GetType() == CECSkill::TYPE_BLESS ||
		m_aSkills[iIndex]->GetType() == CECSkill::TYPE_NEUTRALBLESS)
	{
		if(ISNPCID(idTarget))
			idTarget = ((CECHostPlayer*)m_pPlayer)->GetPlayerInfo().cid;
	}

	//	Check target when this is a negative skill.
	if (m_aSkills[iIndex]->GetType() == CECSkill::TYPE_ATTACK ||
		m_aSkills[iIndex]->GetType() == CECSkill::TYPE_CURSE)
	{
		if (idTarget && ((CECHostPlayer*)m_pPlayer)->AttackableJudge(idTarget, bForceAttack) != 1)
		{
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETWRONG);	
			return false;
		}
	}
	else if(m_aSkills[iIndex]->GetType() == CECSkill::TYPE_BLESS
		||	m_aSkills[iIndex]->GetType() == CECSkill::TYPE_NEUTRALBLESS)
	{
		int iTargetType = m_aSkills[iIndex]->GetTargetType();

		if (!iTargetType || !ISPLAYERID(idTarget))
			idTarget = m_pPlayer->GetPlayerInfo().cid;
	}

	int iRet = CheckSkillCastCondition(m_aSkills[iIndex]);

	if(iRet)
	{
		switch(iRet)
		{
		case 1:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_WRONG_PROF);		break;
		case 2:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_NEED_MP);			break;
		case 3:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_NEED_AP);			break;
		case 4:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GENIUSPT_WASHED);	break;
		case 6:		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NOTARGET);				break;
		case 10:	g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_NEEDITEM);				break;
		case 8:
		{
			int iEnv = m_aSkills[iIndex]->GetCastEnv();
			ASSERT(iEnv > 0);

			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_GROUND_ONLY + iEnv - 1);			
			break;
		}
		default:
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOBLIN_CAST_SKILL_FAIL);			break;			
		}

		return false;
	}

	// Is target too far away?
	float fDist = 0.0f;
	float fTargetRadius = 0.0f;

	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 1);
	if (!pObject)
		return false;

	bool bIsFarAway = false;

	if (ISNPCID(idTarget))
	{
		CECNPC* pNPC = (CECNPC*)pObject;
		fDist = pNPC->CalcDist(m_pPlayer->GetPos(), true);
		fTargetRadius = pNPC->GetTouchRadius();
	}
	else if (ISPLAYERID(idTarget))
	{
		if(idTarget == ((CECHostPlayer*)m_pPlayer)->GetPlayerInfo().cid)
			fDist = 0.0f;
		else
		{
			CECElsePlayer* pPlayer = (CECElsePlayer*)pObject;
			fDist = pPlayer->CalcDist(m_pPlayer->GetPos(), true);
			fTargetRadius = pPlayer->GetTouchRadius();
		}
	}

 	if( (fDist > m_aSkills[iIndex]->GetCastRange(0.0f, 0.0f) + fTargetRadius) &&
		(idTarget != ((CECHostPlayer*)m_pPlayer)->GetPlayerInfo().cid))
 	{
 		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);	
 		return false;
	}
	
	BYTE byBLSMask = glb_BuildBLSMask();

	if (m_aSkills[iIndex]->GetType() == CECSkill::TYPE_BLESS ||
		m_aSkills[iIndex]->GetType() == CECSkill::TYPE_NEUTRALBLESS)
	{
		if (m_aSkills[iIndex]->GetRangeType() == CECSkill::RANGE_POINT &&
			ISPLAYERID(idTarget) &&
			idTarget != m_pPlayer->GetPlayerInfo().cid)
		{
			if(!m_pPlayer->IsTeamMember(idTarget))
			{
				if(byBLSMask & GP_BLSMASK_SELF)
					idTarget = m_pPlayer->GetPlayerInfo().cid;
				else
				{				
					CECElsePlayer* pPlayer = (CECElsePlayer*)g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idTarget);
					ASSERT(pPlayer);
					
					if(pPlayer->IsInvader() || pPlayer->IsPariah())
					{
						if(byBLSMask & GP_BLSMASK_NORED)
							idTarget = m_pPlayer->GetPlayerInfo().cid;
					}

					CECHostPlayer *pHost = (CECHostPlayer *)m_pPlayer;
					if (!pHost->IsFactionMember(pPlayer->GetFactionID()))
					{
						if(byBLSMask & GP_BLSMASK_NOMAFIA)
							idTarget = m_pPlayer->GetPlayerInfo().cid;
					}

					if (!pHost->IsFactionAllianceMember(pPlayer->GetFactionID()))
					{
						if(byBLSMask & GP_BLSMASK_NOALLIANCE)
							idTarget = m_pPlayer->GetPlayerInfo().cid;
					}

					if (pHost->GetForce() != pPlayer->GetForce())
					{
						if(byBLSMask & GP_BLSMASK_NOFORCE)
							idTarget = m_pPlayer->GetPlayerInfo().cid;
					}
				}
				
				
				//	If host is in duel, bless skill couldn't add to opponent
				if (m_pPlayer->IsInDuel() && ((CECHostPlayer*)m_pPlayer)->GetSelectedTarget() == m_pPlayer->GetPVPInfo().idDuelOpp )
					idTarget = m_pPlayer->GetPlayerInfo().cid;
				
			}
		}
	}

	g_pGame->GetGameSession()->c2s_CmdGoblinCastSkill(m_aSkills[iIndex]->GetSkillID(), glb_BuildPVPMask(bForceAttack), 1,&idTarget);
	return true;
}

void CECHostGoblin::OnMsgGoblinVigor(const ECMSG& Msg)
{
	using namespace S2C;
	
	cmd_elf_vigor* pCmd = (cmd_elf_vigor*)Msg.dwParam1;
	ASSERT(pCmd);

 	m_iVigor = pCmd->vigor;
 	m_iMaxVigor = pCmd->max_vigor;
 	m_fVigorGen = (float)(pCmd->vigor_gen)/100.0f;
}

void CECHostGoblin::OnMsgGoblinExp(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_elf_exp* pCmd = (cmd_elf_exp*)Msg.dwParam1;
	ASSERT(pCmd);

	m_iExp = pCmd->exp;	
}
void CECHostGoblin::OnMsgGoblinEnhance(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_elf_enhance* pCmd = (cmd_elf_enhance*)Msg.dwParam1;
	ASSERT(pCmd);

	m_iStrEnhance = pCmd->str_en;
	m_iAgiEnhance = pCmd->agi_en;
	m_iVitEnhance = pCmd->vit_en;
	m_iEngEnhance = pCmd->eng_en;
	
}
void CECHostGoblin::OnMsgGoblinStamina(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_elf_stamina* pCmd = (cmd_elf_stamina*)Msg.dwParam1;
	ASSERT(pCmd);
	
	m_iStamina = pCmd->stamina;

 	//  更新小精灵物品的耐力值
 	CECIvtrGoblin* pIvtrGoblin = (CECIvtrGoblin*)(((CECHostPlayer*)m_pPlayer)->GetEquipment()->GetItem(EQUIPIVTR_GOBLIN));
	pIvtrGoblin->SetStamina(m_iStamina);
}



