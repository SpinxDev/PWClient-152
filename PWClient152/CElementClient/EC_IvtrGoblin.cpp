/*
 * FILE: EC_IvtrGoblin.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Sunxuewei, 2008/11/04
 */

#include "EC_Global.h"
#include "EC_IvtrGoblin.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_RTDebug.h"
#include "elementdataman.h"
#include "ElementSkill.h"
#include "EC_Inventory.h"
#include "EC_Skill.h"
#include "EC_Configs.h"

#define new A_DEBUG_NEW
#define INITIAL_MAX_VIGOR	100  // initial max vigor of goblin

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
//	Implement CECIvtrGoblin
//	
///////////////////////////////////////////////////////////////////////////

// Refine effect of goblin
int CECIvtrGoblin::m_iRefineEffect[MAX_ELF_REFINE_LEVEL][3] = 
{	
	{45,0,0},
	{100,0,0},
	{165,0,0},
	{240,0,0},
	{325,0,0},
	
	{420,0,0},
	{420,1,0},
	{420,2,0},
	{420,2,1},
	{420,2,2},		// 10
	
	{420,3,3},
	{420,4,4},
	{420,6,5},
	{420,9,6},
	{420,12,7},

	{420,16,8},
	{420,20,9},
	{420,24,11},
	{420,28,13},
	{420,32,15},	// 20

	{420,36,17},
	{420,41,19},
	{420,46,21},
	{420,51,24},
	{420,56,27},

	{420,61,30},
	{420,66,33},
	{420,72,36},
	{420,78,39},
	{420,84,42},	// 30
	
	{420,90,45},
	{420,96,48},
	{420,102,51},
	{420,108,54},
	{420,114,57},

	{420,120,60}
};

//给小精灵注入经验时的折损常数，小精灵等级对应的折损常数/人物对应的折损常数=注入经验时的有效比率
int CECIvtrGoblin::elf_exp_loss_constant[151] = {0, //no use
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
		31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
		51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
		61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
		71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
		81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
		91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
		100, 100, 100, 100, 100,		//105
		
		//  为避免内服测试出错，将等级提升到150
		100, 100, 100, 100,	100,	//110
		100, 100, 100, 100, 100,
		100, 100, 100, 100, 100,	//120
		100, 100, 100, 100, 100,
		100, 100, 100, 100, 100,	//130
		100, 100, 100, 100, 100,
		100, 100, 100, 100, 100,	//140
		100, 100, 100, 100, 100,
		100, 100, 100, 100, 100,	//150
};

//不使用rmb材料目标精炼等级对应的成功率
float CECIvtrGoblin::elf_refine_succ_prob_ticket0[MAX_ELF_REFINE_LEVEL+1] = { 
		1.0f, 0.4f, 0.315789474f, 0.338983051f, 0.377358491f, 0.417754569f, 0.463208685f, 0.510769231f, 0.55f, 0.55f,
		0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 
		0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 
		0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 
		0.0f};	// [36]: cannot refine any more
//使用玲珑丹(提高成功率，失败等级归0)目标精炼等级对应的成功率
float CECIvtrGoblin::elf_refine_succ_prob_ticket1[MAX_ELF_REFINE_LEVEL+1] = {
		1.0f, 0.6f, 0.368421053f, 0.355932203f, 0.383647799f, 0.420365535f, 0.464414958f, 0.511384615f, 0.557001027f, 0.60032861f, 
		0.640836732f, 0.678052261f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 
		0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 
		0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f,
		0.0f};
//使用神韵丹(降成功率，失败等级降1)目标精炼等级对应的成功率
float CECIvtrGoblin::elf_refine_succ_prob_ticket2[MAX_ELF_REFINE_LEVEL+1] = {
		1.0f, 0.6f, 0.333333333f, 0.296296296f, 0.3f, 0.314814815f, 0.337313433f, 0.360708535f, 0.381453155f, 0.4f, 
		0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 
		0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f,
		0.0f};
//使用梦幻丹(成功率由使用个数决定，有个数上限，失败等级不败)目标精炼等级对应的最大使用量，成功率=道具数量/可以放的最大数量
int CECIvtrGoblin::elf_refine_max_use_ticket3[MAX_ELF_REFINE_LEVEL+1] = {
		1 , 2 , 7 , 20 , 50 , 112 , 223 , 398 , 648 , 974 , 
		1366 , 1806 , 2271 , 2739 , 3191 , 3612 , 3993 , 4331 , 4623 , 4873 , 
		5084 , 5260 , 5405 , 5525 , 5623 , 5702 , 5767 , 5819 , 5861 , 5895 , 
		5923 , 5945 , 5962 , 5977 , 5988 , 5997,
		100000000};

//  小精灵精炼所消耗的乾坤石的数量
int CECIvtrGoblin::m_iRefineMaterial[MAX_ELF_REFINE_LEVEL] = 
{
	1, 2, 7, 20, 50, 110, 220, 390, 500, 550,	// 10 
	600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050, // 20
	1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, // 30 
	1600, 1650, 1700, 1750, 1800, 1850,
};
float CECIvtrGoblin::GetRefineSuccProb0(int iLevel)
{
	ASSERT(iLevel >= 0 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return elf_refine_succ_prob_ticket0[iLevel];
}
float CECIvtrGoblin::GetRefineSuccProb1(int iLevel)
{
	ASSERT(iLevel >= 0 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return elf_refine_succ_prob_ticket1[iLevel];
}
float CECIvtrGoblin::GetRefineSuccProb2(int iLevel)
{
	ASSERT(iLevel >= 0 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return elf_refine_succ_prob_ticket2[iLevel];
}
float CECIvtrGoblin::GetRefineSuccProb3(int iLevel, int iNum)
{
	ASSERT(iLevel >= 0 && iLevel <= MAX_ELF_REFINE_LEVEL);
	if(iNum >= elf_refine_max_use_ticket3[iLevel])
		return 1.0f;
	if(iLevel == MAX_ELF_REFINE_LEVEL)
		return 0.0f;
	
	return (float)(iNum)/(float)elf_refine_max_use_ticket3[iLevel];
}
int CECIvtrGoblin::GetRefineLife(int iLevel)
{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ELF_REFINE_LEVEL);	
	return m_iRefineEffect[iLevel-1][0];
}

int CECIvtrGoblin::GetRefineAtkLvl(int iLevel)
{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return m_iRefineEffect[iLevel-1][1];
}

int CECIvtrGoblin::GetRefineDfsLvl(int iLevel)
{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return m_iRefineEffect[iLevel-1][2];
}

int CECIvtrGoblin::GetRefineMaterial(int iLevel)
{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ELF_REFINE_LEVEL);
	return m_iRefineMaterial[iLevel-1];
}

CECIvtrGoblin::CECIvtrGoblin(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_GOBLIN;
	m_iCurEndurance	= 1;
	m_iMaxEndurance	= 1;

	memset(&m_Essence, 0, sizeof (m_Essence));

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (GOBLIN_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;

	m_bNeedUpdate	= false;	
	m_i64EquipMask	= EQUIP_MASK64_GOBLIN;
}

CECIvtrGoblin::CECIvtrGoblin(const CECIvtrGoblin& s) : CECIvtrEquip(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_Essence		= s.m_Essence;

	//	Copy skills
	int i, iNumSkill = s.m_aSkills.GetSize();
	m_aSkills.SetSize(iNumSkill, 4);

	for (i=0; i < iNumSkill; i++)
		m_aSkills[i] = s.m_aSkills[i];

	//  Copy equipments
	int iNumEquip = s.m_aEquipID.GetSize();
	m_aEquipID.SetSize(iNumEquip, 4);

	for(i=0; i< iNumEquip; i++)
		m_aEquipID[i] = s.m_aEquipID[i];
}

CECIvtrGoblin::~CECIvtrGoblin()
{
}

//	Set item detail information
bool CECIvtrGoblin::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		// Set _GOBLIN_DATA info
		int iGoblinDataSize = sizeof(IVTR_ESSENCE_GOBLIN::_GOBLIN_DATA);
 		m_Essence.data = *(IVTR_ESSENCE_GOBLIN::_GOBLIN_DATA*)dr.Read_Data(iGoblinDataSize);

		// Change proc type
		if(m_Essence.data.status_value < 0)
		{
			m_iProcType |= PROC_SELLABLE;
			m_iProcType |= PROC_TRADEABLE;
		}
		else
		{
			m_iProcType &= ~PROC_SELLABLE;
			m_iProcType &= ~PROC_TRADEABLE;
		}

		// Set equipment info
		int i, iEquipCnt = dr.Read_int();
		ASSERT(iEquipCnt >= 0);
		m_Essence.equip_cnt = iEquipCnt;

		m_aEquipID.SetSize(iEquipCnt, 4);
		for(i=0; i< iEquipCnt; i++)
		{
			m_aEquipID[i] = dr.Read_int();
		}

		// Set skill info
		int iSkillCnt = dr.Read_int();
		ASSERT(iSkillCnt >= 0);
		m_Essence.skill_cnt = iSkillCnt;

		m_aSkills.SetSize(iSkillCnt, 4);
		for(i=0; i< iSkillCnt; i++)
		{
			GOBLINSKILL& s = m_aSkills[i];
			s.skill = dr.Read_short();
			s.level = dr.Read_short();
		}
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrGoblin::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrGoblin::GetIconFile()
{
	return m_pDBEssence->file_icon1;	// Need to be confirmed
}

//	Get item name
const wchar_t* CECIvtrGoblin::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrGoblin::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	m_strDesc = _AL("");
	
	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	int white = ITEMDESC_COL_WHITE;
	int yellow = ITEMDESC_COL_YELLOW;
	int green = ITEMDESC_COL_GREEN;
	int color = white;
	int i;
	int namecol = DecideNameCol();

	//	Item name: always use the name in template	
	if(m_Essence.data.refine_level != 0)
	{
		AddDescText(namecol, false, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name);
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_REFINE_LEVEL), m_Essence.data.refine_level);
	}
	else
	{
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name);
	}
	
	AddIDDescText();

	//  Goblin level
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_LEVEL), m_Essence.data.level);

	// Strength, Agility, Vitality, Energy + gained from equipment
	int iEquipStrength = 0;
	int iEquipAgility = 0;
	int iEquipTili = 0;
	int iEquipEnergy = 0;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	GOBLIN_EQUIP_ESSENCE* pDBEssence;

	for(i=0; i< m_aEquipID.GetSize(); i++)
	{
		pDBEssence = (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(m_aEquipID[i], ID_SPACE_ESSENCE, DataType);
		iEquipStrength += pDBEssence->strength;
		iEquipAgility += pDBEssence->agility;
		iEquipTili += pDBEssence->tili;
		iEquipEnergy += pDBEssence->energy;
	}

	color = (iEquipStrength == 0) ? white : green;
	AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), iEquipStrength + m_Essence.data.strength + m_pDBEssence->init_strength);
	color = (iEquipAgility == 0) ? white : green;
	AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), iEquipAgility + m_Essence.data.agility + m_pDBEssence->init_agility);
	color = (iEquipTili == 0) ? white : green;
	AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), iEquipTili + m_Essence.data.vitality + m_pDBEssence->init_tili);
	color = (iEquipEnergy == 0) ? white : green;
	AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), iEquipEnergy + m_Essence.data.energy + m_pDBEssence->init_energy);

	// Grow degree
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_GROW_DEGREE), GetGrowDegree());

	// Random status point
	int iMaxRandPt = GetMaxRandomStatusPt();
	if(iMaxRandPt != 0)
	{
		int iRandPt = GetRandomStatusPt();

		int iGrowDegree = GetGrowDegree();
		int iRandomPt	= GetRandomStatusPt();
		int iMaxPt= GetMaxStautsPt();

		if(iRandomPt <= iGrowDegree*0.8)
			color = ITEMDESC_COL_GREEN;
		else if(iRandomPt <= iGrowDegree)
			color = ITEMDESC_COL_LIGHTBLUE;
		else if(iRandomPt <= (iMaxPt - iGrowDegree)*0.2 + iGrowDegree)
			color = ITEMDESC_COL_PURPLE;
		else if(iRandomPt < iMaxPt)
			color = ITEMDESC_COL_YELLOW;
		else if(iRandomPt == iMaxPt)
			color = ITEMDESC_COL_DARKGOLD;

		AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_RANDPT), iRandPt, iMaxRandPt);
	}

	// Energy
	int iEnergy = INITIAL_MAX_VIGOR + GetBasicProp(2);
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_ENERGY), iEnergy);

	// Energy restore speed
	float fRestoreSpeed = 1.0f + GetBasicProp(3)*0.02f;
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_ENERGY_RESTORE), fRestoreSpeed);

	// Current stamina
	int iCurrStamina = m_Essence.data.stamina;
	if(iCurrStamina > 999999)
		iCurrStamina = 999999;
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_STAMINA), iCurrStamina);

	// Current refine effect
	if(m_Essence.data.refine_level != 0)
	{
		int iAttackLevel, iDefenceLevel, iLife;
		iLife = GetRefineLife(m_Essence.data.refine_level);
		iAttackLevel = GetRefineAtkLvl(m_Essence.data.refine_level);
		iDefenceLevel = GetRefineDfsLvl(m_Essence.data.refine_level);
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_REFINE_EFFECT), iLife, iAttackLevel, iDefenceLevel);
	}

	// Trade state
	int iTradeState = m_Essence.data.status_value;
	if(iTradeState == 0)
		AddDescText(white,true, pDescTab->GetWideString(ITEMDESC_GOBLIN_TRADE_PROTECT));
	else if(iTradeState > 0)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_TRADE_UNPROTECT));
	else if(iTradeState < 0)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLIN_CANTRADE));

	//	Price
	AddPriceDesc(white, bRepair);
	
	// Equipment info
	if(m_aEquipID.GetSize() != 0)
		m_strDesc += _AL("\\r");
	for(i=0; i< m_aEquipID.GetSize(); i++)
	{
		pDBEssence = (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(m_aEquipID[i], ID_SPACE_ESSENCE, DataType);
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_GOBLINEQUIP_POS_1 + pDBEssence->equip_type));
		AddDescText(white, true, _AL(" %s"), pDBEssence->name);
	}

	//	Skill list
	if(m_aSkills.GetSize() != 0 && m_aEquipID.GetSize() == 0)
		m_strDesc += _AL("\\r");
	for (i=0; i < m_aSkills.GetSize(); i++)
	{
		const GOBLINSKILL& gSkill = m_aSkills[i];
		const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(gSkill.skill * 10);
		if(i==m_aSkills.GetSize()-1)
			AddDescText(ITEMDESC_COL_YELLOW, false, pDescTab->GetWideString(ITEMDESC_PETSKILL), szName ? szName : _AL(""), gSkill.level);
		else
			AddDescText(ITEMDESC_COL_YELLOW, true, pDescTab->GetWideString(ITEMDESC_PETSKILL), szName ? szName : _AL(""), gSkill.level);
	}

	AddExtDescText();

	return m_strDesc;
}
//  Get the experience when destroy the goblin
unsigned int CECIvtrGoblin::GetDestroyExp()
{
	double dRetExp = 0;	
	int iLevel = m_Essence.data.level;
	unsigned int iCurrExp = m_Essence.data.exp;
	double factor = 0.0f;

	for(int i=1;i<iLevel;i++)
	{
		unsigned int iPlayerLvlUpExp = (unsigned int)(g_pGame->GetGameRun()->GetHostPlayer()->GetLevelUpExp(i) * m_pDBEssence->exp_factor);
		factor = (double)elf_exp_loss_constant[i]/(double)elf_exp_loss_constant[m_Essence.data.level];
		ASSERT(factor <= 1 && factor > 0);
		dRetExp += (((double)iPlayerLvlUpExp * 0.1) / factor);
	}
	
	dRetExp += ((double)iCurrExp * 0.1);
	return (dRetExp>4200000000.0) ? 4200000000 : (unsigned int)dRetExp;
}
//  Check whether goblin can trade
bool CECIvtrGoblin::IsTradeable() const
{
	return (m_iProcType & PROC_TRADEABLE)?true:false;
}
//	Get drop model for shown
const char* CECIvtrGoblin::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
//  Get Equipment id
unsigned int CECIvtrGoblin::GetEquip(int id)
{
	ASSERT(id>=0 && id<4);

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	GOBLIN_EQUIP_ESSENCE* pDBEssence;
	
	for(int i=0;i<m_aEquipID.GetSize();i++)
	{	
		pDBEssence = (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(m_aEquipID[i], ID_SPACE_ESSENCE, DataType);
		
		int iEquipPos = pDBEssence->equip_type;
		if(iEquipPos == id)
			return m_aEquipID[i]; 
	}
	return 0;
}
//  Get grow degree
int CECIvtrGoblin::GetGrowDegree()
{
	float fGrowDegree = 0.0f;
	for(int i=0;i<10;i++)
	{
		fGrowDegree += m_pDBEssence->rand_prop[i].rand_num * m_pDBEssence->rand_prop[i].rand_rate;
	}

	return (int)(fGrowDegree*10 + 0.5f);
}
//  Current random status point
int CECIvtrGoblin::GetRandomStatusPt()
{
	return m_Essence.data.total_attribute - (m_Essence.data.level-1);
}
//  Currently max random status point that can get
int CECIvtrGoblin::GetMaxRandomStatusPt()
{
	return (GetMaxStautsPt()/10) * (m_Essence.data.level/10);
}
//  Max random status points that this goblin can get, depending on player's RP value
int CECIvtrGoblin::GetMaxStautsPt()
{
	int iMaxPt = 0;
	for(int i= 0;i<10;i++)
	{
		if(iMaxPt < m_pDBEssence->rand_prop[i].rand_num)
			iMaxPt = m_pDBEssence->rand_prop[i].rand_num;
	}

	return iMaxPt * 10;
}

//	Check whether goblin can learn specified skill
// 0:成功            1:主人SP不足        2:技能天赋点不足
// 3:满级            4:技能个数限制		 5:错误ID
// 6:金钱不足		 7:非小精灵技能		 8:没有技能书
// 9:等级不足       10:能量上限不足      11:职业不匹配
// 12:能量上限不足，且职业不匹配
int CECIvtrGoblin::CheckSkillLearnCondition(int idSkill, bool bCheckBook)
{
	int iLevel = 1, i;
	for(i=0;i<m_aSkills.GetSize();i++)
	{
		if(m_aSkills[i].skill == idSkill)
		{
			iLevel = m_aSkills[i].level + 1;
			break;
		}
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (iLevel == 1 && bCheckBook)
	{
		//	Do we have the skill book ?
		int idBook = GNET::ElementSkill::GetRequiredBook(idSkill, iLevel);
		if (idBook && pHost->GetPack()->FindItem(idBook) < 0)
			return 8;
	}

	//	Build player information
	GNET::GoblinRequirement Info;
	memset(&Info, 0, sizeof (Info));

	for(i=0;i<5;i++)
	{
		Info.genius[i] = GetGenius(i);
	}
	Info.profession	= pHost->GetProfession();
	Info.sp			= pHost->GetBasicProps().iSP;
	Info.money		= pHost->GetMoneyAmount();
	Info.level		= m_Essence.data.level;
	Info.mp			= INITIAL_MAX_VIGOR + GetBasicProp(2);

	int iRet = GNET::ElementSkill::GoblinLearn(idSkill, Info, iLevel);

	if(iRet == 0) // success
	{
		if((GetSkillNum() > GetCurrMaxSkillNum()) ||
		   (GetSkillNum() == GetCurrMaxSkillNum() && iLevel == 1))
			return 4;
	}

	return iRet;
}
//	Check whether goblin can cast specified skill
bool CECIvtrGoblin::CheckSkillCastCondition(int index)
{
	if(index<0 || index >= m_aSkills.GetSize())
	{
		ASSERT(0);
		return false;
	}
	if (g_pGame->GetGameRun()->GetHostPlayer()->GetBattleInfo().IsChariotWar())
		return false;  // 战场战车 禁止精灵技能
		

	GNET::GoblinUseRequirement Info;
	memset(&Info, 0, sizeof(Info));

	for(int i=0;i<5;i++)
	{
		Info.genius[i] = GetGenius(i);
	}
	
	Info.level = m_Essence.data.level;
	Info.move_env = g_pGame->GetGameRun()->GetHostPlayer()->GetMoveEnv();

	GOBLINSKILL pSkill = GetSkill(index);
	int ret = GNET::ElementSkill::GoblinCondition(pSkill.skill, Info, pSkill.level);

	if(ret == 4)
		return false;
	return true;
}

//  Get basic property
int CECIvtrGoblin::GetBasicProp(int iIndex)
{
	ASSERT(iIndex>=0 && iIndex<4);
	int i, iRet = 0;

	// Strength, Agility, Vitality, Energy + gained from equipment
	int iEquipStrength = 0;
	int iEquipAgility = 0;
	int iEquipTili = 0;
	int iEquipEnergy = 0;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	GOBLIN_EQUIP_ESSENCE* pDBEssence;

	for(i=0; i< m_aEquipID.GetSize(); i++)
	{
		pDBEssence = (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(m_aEquipID[i], ID_SPACE_ESSENCE, DataType);
		iEquipStrength += pDBEssence->strength;
		iEquipAgility += pDBEssence->agility;
		iEquipTili += pDBEssence->tili;
		iEquipEnergy += pDBEssence->energy;
	}

	if(iIndex == 0)
		iRet = iEquipStrength + m_Essence.data.strength + m_pDBEssence->init_strength;
	else if(iIndex == 1)
		iRet = iEquipAgility + m_Essence.data.agility + m_pDBEssence->init_agility;
	else if(iIndex == 2)
		iRet = iEquipTili + m_Essence.data.vitality + m_pDBEssence->init_tili;
	else if(iIndex == 3)
		iRet = iEquipEnergy + m_Essence.data.energy + m_pDBEssence->init_energy;

	return iRet;
}
//  Get genius point
int CECIvtrGoblin::GetGenius(int iIndex)
{
	ASSERT(iIndex>=0&&iIndex<5);
	int i, iRet = m_Essence.data.genius[iIndex];

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	GOBLIN_EQUIP_ESSENCE* pDBEssence;

	for(i=0; i< m_aEquipID.GetSize(); i++)
	{
		pDBEssence = (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(m_aEquipID[i], ID_SPACE_ESSENCE, DataType);
		iRet += pDBEssence->magic[iIndex];
	}

	return (iRet > 8) ? 8 : iRet;
}

//  Get model level
int CECIvtrGoblin::GetModelLevel()
{
	int iGrowDegree	= GetGrowDegree();
	int iRandomPt	= GetRandomStatusPt();
	int iMaxPt= GetMaxStautsPt();
	int iModelLvl = 0;

	// 成长度、随机属性与几档模型的关系，如更新需告知服务器同步更新
	if(iRandomPt <= iGrowDegree*0.8)
		iModelLvl = 1;
	else if(iRandomPt <= iGrowDegree)
		iModelLvl = 2;
	else if(iRandomPt <= (iMaxPt - iGrowDegree)*0.2 + iGrowDegree)
		iModelLvl = 3;
	else 
		iModelLvl = 4;

	return iModelLvl;
} 

//  Get current max skill number
int CECIvtrGoblin::GetCurrMaxSkillNum()
{
	int iRandPt = GetRandomStatusPt();

	if(iRandPt > 90)
		return 8;
	else if(iRandPt > 80)
		return 7;
	else if(iRandPt > 70)
		return 6;
	else if(iRandPt > 50)
		return 5;

	return INIT_SKILL_NUM;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrGoblinEquip
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrGoblinEquip::CECIvtrGoblinEquip(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_GOBLIN_EQUIP;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (GOBLIN_EQUIP_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	
	m_bNeedUpdate	= false;
}

CECIvtrGoblinEquip::CECIvtrGoblinEquip(const CECIvtrGoblinEquip& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}
CECIvtrGoblinEquip::~CECIvtrGoblinEquip()
{

}

//	Set item detail information
bool CECIvtrGoblinEquip::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrGoblinEquip::GetIconFile()
{
	return m_pDBEssence->file_icon;
}
//	Get item name
const wchar_t* CECIvtrGoblinEquip::GetName()
{
	return m_pDBEssence->name;
}
//	Get drop model for shown
const char * CECIvtrGoblinEquip::GetDropModel()
{
	return m_pDBEssence->file_model;
}
//  Get goblin equipment type
int CECIvtrGoblinEquip::GetGoblinEquipType() const
{
	return m_pDBEssence->equip_type; 
}

//	Can this item be equipped to specified position ?
bool CECIvtrGoblinEquip::CanEquippedTo(int iSlot) const
{
	return (iSlot == GetGoblinEquipType()) ? true : false; 
}
//	Get item description text
const wchar_t* CECIvtrGoblinEquip::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	int white = ITEMDESC_COL_WHITE;
	int color = white;
	int namecol = DecideNameCol();

	//  Item name
	AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name);

	AddIDDescText();
	
	//  Equip pos
	int iEquipPos = m_pDBEssence->equip_type;
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLINEQUIP_POS),pDescTab->GetWideString(ITEMDESC_GOBLINEQUIP_POS_1 + iEquipPos));

	//  Bind state
	AddBindDescText();

	//  Level require
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_GOBLINEQUIP_LEVELREQ), m_pDBEssence->req_goblin_level);

	//  Price
	AddPriceDesc(white, false);

	//  4 basic prop
	if(m_pDBEssence->strength != 0)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_STRENGTH), m_pDBEssence->strength);	
	}
	if(m_pDBEssence->agility != 0)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_AGILITY), m_pDBEssence->agility);
	}
	if(m_pDBEssence->energy != 0)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_ENERGY), m_pDBEssence->energy);
	}
	if(m_pDBEssence->tili != 0)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_VITALITY), m_pDBEssence->tili);
	}

	//  5 genius prop
	for(int i=0;i<5;i++)
	{
		if(m_pDBEssence->magic[i] != 0)
		{
			m_strDesc += _AL("\\r");
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_GOBLINEQUIP_GOLD+i), m_pDBEssence->magic[i]);
		}
	}

	//  Extend desc
	AddExtDescText();

	return m_strDesc;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrGoblinExpPill
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrGoblinExpPill::CECIvtrGoblinExpPill(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_GOBLIN_EXPPILL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (GOBLIN_EXPPILL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iLevel		= m_pDBEssence->level;
	m_iExp			= m_pDBEssence->exp;
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_bNeedUpdate	= true;
}

CECIvtrGoblinExpPill::CECIvtrGoblinExpPill(const CECIvtrGoblinExpPill& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;

	m_iLevel		= s.m_iLevel;
	m_iExp			= s.m_iExp;
}
CECIvtrGoblinExpPill::~CECIvtrGoblinExpPill()
{
}

//	Set item detail information
bool CECIvtrGoblinExpPill::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);
		
		// Set experiment info
		m_iExp = (unsigned int)dr.Read_int();
		m_iLevel = dr.Read_int();		
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrGoblinExpPill::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrGoblinExpPill::GetIconFile()
{
	return m_pDBEssence->file_icon;
}
//	Get item name
const wchar_t* CECIvtrGoblinExpPill::GetName()
{
	return m_pDBEssence->name;
}
//	Get drop model for shown
const char * CECIvtrGoblinExpPill::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
//	Get item description text
const wchar_t* CECIvtrGoblinExpPill::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name);

	AddIDDescText();

	//  Item level 
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_LEVEL), m_iLevel);
	//  Item exp
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_EXPPILL_EXP), m_iExp);
	//  Item price
	AddPriceDesc(white, false);
	
	AddExtDescText();
	
	return m_strDesc;
}

bool CECIvtrGoblinExpPill::IsRare() const
{ 
	return CECIvtrItem::IsRare() || m_iLevel >= 6;
}