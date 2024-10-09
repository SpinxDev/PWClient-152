// File		: DlgProfView.cpp
// Creator	: Chen Zhixin
// Date		: 2009/5/5

#include "DlgProfView.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgProfView, CDlgBase)

AUI_END_COMMAND_MAP()

static A3DCOLOR s_color[] = {	0xFFFFFF,							//白
								0x7081FF,							//蓝
								0x9000FF,							//紫
								0xFFD200,							//黄
								0xFF8400							//橙
							};

CDlgProfView::CDlgProfView()
{
	m_type = 0;
}

CDlgProfView::~CDlgProfView()
{

}

void CDlgProfView::OnShowDialog()
{
	ACString strText;

	((PAUILABEL)GetDlgItem("Txt_Name"))->SetText(m_prof.name);
	strText.Format(_AL("%d"), m_prof.level);
	GetDlgItem("Txt_Level")->SetText(strText);
	GetDlgItem("Txt_Xz")->SetText(GetStringFromTable(1001+m_prof.level2));
	GetDlgItem("Txt_Profession")->SetText(GetGameRun()->GetProfName(m_prof.profession));
	strText.Format(_AL("%d"), m_prof.hp);
	GetDlgItem("Txt_HP")->SetText(strText);
	strText.Format(_AL("%d"), m_prof.mp);
	GetDlgItem("Txt_MP")->SetText(strText);

	PAUIOBJECT pObj = GetDlgItem("Txt_Realm");
	if (m_prof.realm_level > 0){
		pObj->SetText(GetGameUIMan()->GetRealmName(m_prof.realm_level));
	}else{
		pObj->SetText(GetStringFromTable(786));
	}
	pObj = GetDlgItem("Txt_Reincarnation");
	if (pObj) {
		strText.Format(_AL("%d"), m_prof.reincarnation_count);
		pObj->SetText(strText);
	}

	strText.Format(_AL("%d-%d"), m_prof.prof[PHYSICAL_DAMAGE_LOW], m_prof.attackmax[0]);						//物理攻击
	m_pTxt_Target[PHYSICAL_DAMAGE_LOW]->SetText(strText);
	
	strText.Format(_AL("%d-%d"), m_prof.prof[MAGIC_DAMAGE_LOW], m_prof.attackmax[1]);							//法术攻击
	m_pTxt_Target[MAGIC_DAMAGE_LOW]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[ACCURATE]);															//准确度							
	m_pTxt_Target[ACCURATE]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[ATTACK_LEVEL]);														//攻击等级							
	m_pTxt_Target[ATTACK_LEVEL]->SetText(strText);
	
	strText.Format(_AL("%d%%"), m_prof.prof[CRIT_DAMANGE_BONUS]+200);											//暴伤
	m_pTxt_Target[CRIT_DAMANGE_BONUS]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[PHYSICAL_DEFENCE]);													//物理防御
	m_pTxt_Target[PHYSICAL_DEFENCE]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[MAGIC_DEFENCE_GOLD]);													//金系防御
	m_pTxt_Target[MAGIC_DEFENCE_GOLD]->SetText(strText);
	strText.Format(_AL("%d"), m_prof.prof[MAGIC_DEFENCE_WOOD]);													//木系防御
	m_pTxt_Target[MAGIC_DEFENCE_WOOD]->SetText(strText);
	strText.Format(_AL("%d"), m_prof.prof[MAGIC_DEFENCE_WATER]);												//水系防御
	m_pTxt_Target[MAGIC_DEFENCE_WATER]->SetText(strText);
	strText.Format(_AL("%d"), m_prof.prof[MAGIC_DEFENCE_FAIR]);													//火系防御
	m_pTxt_Target[MAGIC_DEFENCE_FAIR]->SetText(strText);
	strText.Format(_AL("%d"), m_prof.prof[MAGIC_DEFENCE_EARTH]);												//土系防御
	m_pTxt_Target[MAGIC_DEFENCE_EARTH]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[ARMOR]);																//躲闪度
	m_pTxt_Target[ARMOR]->SetText(strText);
	
	strText.Format(_AL("%d"), m_prof.prof[DEFENCE_LEVEL]);														//防御等级
	m_pTxt_Target[DEFENCE_LEVEL]->SetText(strText);
	
	strText.Format(_AL("%4.2f %s"), m_prof.prof[ATTACK_SPEED] ? 
		1.0f / (m_prof.prof[ATTACK_SPEED] * 0.05f) : 0.0f, m_pAUIManager->GetStringFromTable(279));				//普攻频率
	m_pTxt_Target[ATTACK_SPEED]->SetText(strText);
	
	strText.Format(_AL("%3.1f %s"), (float)m_prof.prof[RUN_SPEED]/100, m_pAUIManager->GetStringFromTable(280));		//移动速度
	m_pTxt_Target[RUN_SPEED]->SetText(strText);
	
	strText.Format(_AL("%d%%"), m_prof.prof[CRIT_RATE]);														//致命一击率
	m_pTxt_Target[CRIT_RATE]->SetText(strText);

	strText.Format(_AL("%d%%"), m_prof.prof[DAMAGE_REDUCE]);													//物理减免
	m_pTxt_Target[DAMAGE_REDUCE]->SetText(strText);

	strText.Format(_AL("%d%%"), m_prof.prof[PRAYSPEED]);														//吟唱加速
	m_pTxt_Target[PRAYSPEED]->SetText(strText);

	strText.Format(_AL("%d"),   m_prof.prof[ANTI_INVISIBLE_DEGREE]);											//反遁隐等级
	m_pTxt_Target[ANTI_INVISIBLE_DEGREE]->SetText(strText);
	
	strText.Format(_AL("%d"),   m_prof.prof[VIGOUR]);														//气魄
	m_pTxt_Target[VIGOUR]->SetText(strText);

	// 躲闪率=1-30*角色等级/(30*角色等级+躲闪度/2)
	// 物理减免= （物理防御/（物理防御+40*角色等级-25））*100%
	// 法术减免= （全系法防平均值/（全系法防平均值+40*角色等级-25））*100%
	// 命中率=准确度/(准确度+14*角色等级)

	float fArmor = 1 - 30.0f * m_prof.level/(30 * m_prof.level + m_prof.prof[ARMOR] * 0.5f);
	a_Clamp(fArmor, 0.0f, 0.9f);
	float fPhyReduce = CalcDegrade(m_prof.prof[PHYSICAL_DEFENCE], m_prof.level)/100.0f;
	a_Clamp(fPhyReduce, 0.0f, 0.9f);
	int magic = (m_prof.prof[MAGIC_DEFENCE_GOLD] + m_prof.prof[MAGIC_DEFENCE_WOOD] + m_prof.prof[MAGIC_DEFENCE_WATER]
		+ m_prof.prof[MAGIC_DEFENCE_FAIR] + m_prof.prof[MAGIC_DEFENCE_EARTH])/5;
	float fMagReduce = CalcDegrade(magic, m_prof.level) / 100.0f;
	a_Clamp(fMagReduce, 0.0f, 0.9f);
	float fAccurate = m_prof.prof[ACCURATE] / (m_prof.prof[ACCURATE] + 14.0f * m_prof.level);
	a_Clamp(fAccurate, 0.0f, 0.95f);

	//	有效血量1=血量/(1-物理减免)/(1-躲闪率)/(1-装备带的物理减免/100)/(1-防御等级/100)
	//	有效血量2=血量/(1-法术减免) /(1-防御等级/100)
	//	防御值=(有效血量1+有效血量2)/2
		
		
	//	物理DPS=(物理攻击力上限+物理攻击力下限)/2*武器攻击速度*(1+暴击率/100)*命中率*(1+攻击等级/100)
	//	法术DPS=(法术攻击力上限+法术攻击力下限)/2*(1+暴击率/100)* (1+攻击等级/100)*(1+ 10 * (装备带的法术咏唱减少/100)^2 - 25 * (装备带的法术咏唱减少/100)^3 + 25 * (装备带的法术咏唱减少/100)^4)
	//	攻击值=物理DPS+法术DPS
	float hp1 = m_prof.hp/(1 - fPhyReduce)/(1 - fArmor)/(1 - m_prof.prof[DAMAGE_REDUCE] / 100.0f) / (1 - m_prof.prof[DEFENCE_LEVEL] / 100.0f);
	float hp2 = m_prof.hp/(1 - fMagReduce)/(1 - m_prof.prof[DEFENCE_LEVEL] / 100.0f);
	int temp = (int)((hp1 + hp2)/2);
	a_Clamp(temp, 0, temp);
	strText.Format(_AL("%d"), temp);
	GetDlgItem("Txt_Target_b")->SetText(strText);
	if (temp == 0)
		GetDlgItem("Txt_Target_b")->SetColor(A3DCOLORRGB(255, 0, 0));
	else
		SetDefenceColor((PAUILABEL)GetDlgItem("Txt_Target_b"), temp);
	

	float attckrate = m_prof.prof[ATTACK_SPEED] ? 1.0f / (m_prof.prof[ATTACK_SPEED] * 0.05f) : 0.0f;
	float PhyDPS = (m_prof.attackmax[0] + m_prof.prof[PHYSICAL_DAMAGE_LOW]) * 0.5f * attckrate * (1 + m_prof.prof[CRIT_RATE] / 100.0f) * fAccurate * (1 + m_prof.prof[ATTACK_LEVEL] / 100.0f);
	float MagDPS = (m_prof.attackmax[1] + m_prof.prof[MAGIC_DAMAGE_LOW]) * 0.5f * (1 + m_prof.prof[CRIT_RATE] / 100.0f) * (1 + m_prof.prof[ATTACK_LEVEL] / 100.0f);
	float praySpeed = m_prof.prof[PRAYSPEED] / 100.0f;
	MagDPS *= (float)(1 + 10 * pow(praySpeed, 2) - 25 * pow(praySpeed, 3) + 25 * pow(praySpeed, 4));
	temp = (int)(PhyDPS + MagDPS);
	a_Clamp(temp, 0, temp);
	strText.Format(_AL("%d"), temp);
	GetDlgItem("Txt_Target_a")->SetText(strText);
	if (temp == 0)
		GetDlgItem("Txt_Target_a")->SetColor(A3DCOLORRGB(255, 0, 0));
	else
		SetAttckColor((PAUILABEL)GetDlgItem("Txt_Target_a"), temp);

	RefreshDetails();
}

bool CDlgProfView::OnInitDialog()
{
	int i;
	m_pObj = (PAUICOMBOBOX)GetDlgItem("Combo_Change");
	for (i = 0; i < 3; i++)
	{
		m_pObj->AddString(GetStringFromTable(7701 + i));	
	}
	m_pObj->SetCurSel(0);

	char szName[30];
	for (i = 0; i< PROPERTYNUM; i++)
	{
		sprintf(szName, "Txt_Target%02d", i+1);
		m_pTxt_Target[i] = (PAUILABEL)GetDlgItem(szName);
		sprintf(szName, "Txt_Tome%02d", i+1);
		m_pTxt_Tome[i] = (PAUILABEL)GetDlgItem(szName);
	}
	return true;
}

void CDlgProfView::OnTick()
{
	int sel = m_pObj->GetCurSel();
	if (m_type != sel)
	{
		m_type = sel;
		RefreshDetails();
	}
}

bool CDlgProfView::Render()
{
	if( !CDlgBase::Render() )
		return false;
	return true;
}

void CDlgProfView::SetProperty(ProfView& prof)
{
	m_prof = prof;
}

void CDlgProfView::RefreshDetails()
{
	int i;
	ACString strText;
	const ROLEBASICPROP& BasicProps =  GetHostPlayer()->GetBasicProps();
	const ROLEEXTPROP& ExtProps =  GetHostPlayer()->GetExtendProps();
	int level = BasicProps.iLevel;
	int temp;

	if (m_type == 0)															//我的属性
	{
		for (i = 0; i< PROPERTYNUM; i++)
		{
			if (m_pTxt_Tome[i])
			{
				m_pTxt_Tome[i]->Show(true);
				m_pTxt_Tome[i]->SetColor(s_color[0]);
			}
		}
		strText.Format(_AL("%d-%d"), ExtProps.ak.damage_low, ExtProps.ak.damage_high);						//物理攻击
		m_pTxt_Tome[PHYSICAL_DAMAGE_LOW]->SetText(strText);

		strText.Format(_AL("%d-%d"), ExtProps.ak.damage_magic_low , ExtProps.ak.damage_magic_high);			//法术攻击
		m_pTxt_Tome[MAGIC_DAMAGE_LOW]->SetText(strText);
		GetDlgItem("Img_HighLight1")->Show(false);
		GetDlgItem("Img_HighLight2")->Show(true);
		GetDlgItem("Img_HighLight3")->Show(true);

		strText.Format(_AL("%d"), ExtProps.ak.attack );															//准确度							
		m_pTxt_Tome[ACCURATE]->SetText(strText);

		strText.Format(_AL("%d"), BasicProps.iAtkDegree);														//攻击等级							
		m_pTxt_Tome[ATTACK_LEVEL]->SetText(strText);

		strText.Format(_AL("%d%%"), BasicProps.iCritDamageBonus+200);											//暴伤							
		m_pTxt_Tome[CRIT_DAMANGE_BONUS]->SetText(strText);

		strText.Format(_AL("%d"), ExtProps.df.defense);															//物理防御
		m_pTxt_Tome[PHYSICAL_DEFENCE]->SetText(strText);

		strText.Format(_AL("%d"), ExtProps.df.resistance[0]);													//金系防御
		m_pTxt_Tome[MAGIC_DEFENCE_GOLD]->SetText(strText);
		strText.Format(_AL("%d"), ExtProps.df.resistance[1]);													//木系防御
		m_pTxt_Tome[MAGIC_DEFENCE_WOOD]->SetText(strText);
		strText.Format(_AL("%d"), ExtProps.df.resistance[2]);													//水系防御
		m_pTxt_Tome[MAGIC_DEFENCE_WATER]->SetText(strText);
		strText.Format(_AL("%d"), ExtProps.df.resistance[3]);													//火系防御
		m_pTxt_Tome[MAGIC_DEFENCE_FAIR]->SetText(strText);
		strText.Format(_AL("%d"), ExtProps.df.resistance[4]);													//土系防御
		m_pTxt_Tome[MAGIC_DEFENCE_EARTH]->SetText(strText);
		
		strText.Format(_AL("%d"), ExtProps.df.armor);															//躲闪度
		m_pTxt_Tome[ARMOR]->SetText(strText);

		strText.Format(_AL("%d"), BasicProps.iDefDegree);														//防御等级
		m_pTxt_Tome[DEFENCE_LEVEL]->SetText(strText);

		strText.Format(_AL("%4.2f %s"), ExtProps.ak.attack_speed ? 
			1.0f / (ExtProps.ak.attack_speed * 0.05f) : 0.0f, m_pAUIManager->GetStringFromTable(279));			//普攻频率
		m_pTxt_Tome[ATTACK_SPEED]->SetText(strText);

		strText.Format(_AL("%3.1f %s"), ExtProps.mv.run_speed, m_pAUIManager->GetStringFromTable(280));			//移动速度
		m_pTxt_Tome[RUN_SPEED]->SetText(strText);

		strText.Format(_AL("%d%%"), BasicProps.iCritRate);														//致命一击率
		m_pTxt_Tome[CRIT_RATE]->SetText(strText);

		strText.Format(_AL("%d%%"), m_prof.self_damagereduce);													//物理减免
		m_pTxt_Tome[DAMAGE_REDUCE]->SetText(strText);
		
		strText.Format(_AL("%d%%"), m_prof.self_prayspeed);														//吟唱加速
		m_pTxt_Tome[PRAYSPEED]->SetText(strText);

		strText.Format(_AL("%d"), BasicProps.iAntiInvisibleDegree);												//反遁隐等级
		m_pTxt_Tome[ANTI_INVISIBLE_DEGREE]->SetText(strText);

		strText.Format(_AL("%d"), BasicProps.iVigour);														//气魄
		m_pTxt_Tome[VIGOUR]->SetText(strText);

		// 躲闪率=1-30*角色等级/(30*角色等级+躲闪度/2)
		// 物理减免= （物理防御/（物理防御+40*角色等级-25））*100%
		// 法术减免= （全系法防平均值/（全系法防平均值+40*角色等级-25））*100%
		// 命中率=准确度/(准确度+14*角色等级)

		float fArmor = 1 - 30.0f * level/(30 * level + ExtProps.df.armor * 0.5f);
		a_Clamp(fArmor, 0.0f, 0.9f);
		float fPhyReduce = CalcDegrade(ExtProps.df.defense, level)/100.0f;
		a_Clamp(fPhyReduce, 0.0f, 0.9f);
		int magic = (ExtProps.df.resistance[0] + ExtProps.df.resistance[1] + ExtProps.df.resistance[2]
			+ ExtProps.df.resistance[3] + ExtProps.df.resistance[4])/5;
		float fMagReduce = CalcDegrade(magic, level) / 100.0f;
		a_Clamp(fMagReduce, 0.0f, 0.9f);
		float fAccurate = ExtProps.ak.attack / (ExtProps.ak.attack + 14.0f * level);
		a_Clamp(fAccurate, 0.0f, 0.95f);
		
		//	有效血量1=血量/(1-物理减免)/(1-躲闪率)/(1-装备带的物理减免/100)/(1-防御等级/100)
		//	有效血量2=血量/(1-法术减免) /(1-防御等级/100)
		//	防御值=(有效血量1+有效血量2)/2
		
		
		//	物理DPS=(物理攻击力上限+物理攻击力下限)/2*武器攻击速度*(1+暴击率/100)*命中率*(1+攻击等级/100)
		//	法术DPS=(法术攻击力上限+法术攻击力下限)/2*(1+暴击率/100)* (1+攻击等级/100)*(1+ 10 * (装备带的法术咏唱减少/100)^2 - 25 * (装备带的法术咏唱减少/100)^3 + 25 * (装备带的法术咏唱减少/100)^4)
		//	攻击值=物理DPS+法术DPS
		float hp1 = ExtProps.bs.max_hp /(1 - fPhyReduce)/(1 - fArmor)/(1 - m_prof.self_damagereduce / 100.0f) / (1 - BasicProps.iDefDegree / 100.0f);
		float hp2 = ExtProps.bs.max_hp /(1 - fMagReduce)/(1 - BasicProps.iDefDegree / 100.0f);
		int temp = (int)((hp1 + hp2)/2);
		a_Clamp(temp, 0, temp);
		strText.Format(_AL("%d"), temp);
		GetDlgItem("Txt_Tome_b")->SetText(strText);
		GetDlgItem("Txt_Tome_b")->Show(true);
		if (temp == 0)
			GetDlgItem("Txt_Tome_b")->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			SetDefenceColor((PAUILABEL)GetDlgItem("Txt_Tome_b"), temp);
		
		
		float attckrate = ExtProps.ak.attack_speed ? 1.0f / (ExtProps.ak.attack_speed * 0.05f) : 0.0f;
		float PhyDPS = (ExtProps.ak.damage_high + ExtProps.ak.damage_low) * 0.5f * attckrate * (1 + BasicProps.iCritRate / 100.0f) 
			* fAccurate * (1 + BasicProps.iAtkDegree / 100.0f);
		float MagDPS = (ExtProps.ak.damage_magic_high + ExtProps.ak.damage_magic_low) * 0.5f * (1 + BasicProps.iCritRate / 100.0f) 
			* (1 + BasicProps.iAtkDegree / 100.0f);
		float praySpeed = m_prof.self_prayspeed / 100.0f;
		MagDPS *= (float)(1 + 10 * pow(praySpeed, 2) - 25 * pow(praySpeed, 3) + 25 * pow(praySpeed, 4));
		temp = (int)(PhyDPS + MagDPS);
		a_Clamp(temp, 0, temp);
		strText.Format(_AL("%d"), temp);
		GetDlgItem("Txt_Tome_a")->SetText(strText);
		GetDlgItem("Txt_Tome_a")->Show(true);
		if (temp == 0)
			GetDlgItem("Txt_Tome_a")->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			SetAttckColor((PAUILABEL)GetDlgItem("Txt_Tome_a"), temp);
	}
	else if (m_type == 1)														//对我的攻击
	{
		for (i = 0; i< PROPERTYNUM; i++)
		{
			if (m_pTxt_Tome[i])
				m_pTxt_Tome[i]->Show(i<=ATTACK_LEVEL);
		}
		temp = CalcDegrade(ExtProps.df.defense, level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//物理攻击减免	
		m_pTxt_Tome[PHYSICAL_DAMAGE_LOW]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[PHYSICAL_DAMAGE_LOW], temp);
		
		strText.Format(_AL("%s"), GetStringFromTable(7704));																				//法术攻击减免
		m_pTxt_Tome[MAGIC_DAMAGE_LOW]->SetText(strText);
		strText = _AL("");
		ACString strTemp;
		for (int j = 0; j < 5; j++)
		{
			temp = CalcDegrade(ExtProps.df.resistance[j], level);
			a_Clamp(temp, 10, 90);
			strTemp.Format(_AL("%s:%d%%%s\r"), GetStringFromTable(7110+j), temp, GetStringFromTable(7761));
			strText += strTemp;
		}
		GetDlgItem("Img_HighLight1")->SetHint(strText);
		GetDlgItem("Img_HighLight1")->Show(true);
		GetDlgItem("Img_HighLight2")->Show(false);
		GetDlgItem("Img_HighLight3")->Show(false);
		
		temp = (int)(m_prof.prof[ACCURATE] / (m_prof.prof[ACCURATE] + ExtProps.df.armor * 0.5f) * 100);
		a_Clamp(temp, 5, 95);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7762));																		//准确度减免					
		m_pTxt_Tome[ACCURATE]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[ACCURATE], temp);
		
		temp = m_prof.prof[ATTACK_LEVEL] > BasicProps.iDefDegree?m_prof.prof[ATTACK_LEVEL] - BasicProps.iDefDegree:0;
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7764));																		//攻击等级减免
		m_pTxt_Tome[ATTACK_LEVEL]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[ATTACK_LEVEL], temp);

		GetDlgItem("Txt_Tome_a")->Show(false);
		GetDlgItem("Txt_Tome_b")->Show(false);
	}
	else if (m_type == 2)														//对我的防御
	{
		for (i = 0; i< PROPERTYNUM; i++)
		{
			if (m_pTxt_Tome[i])
				m_pTxt_Tome[i]->Show(i>ATTACK_LEVEL && i<=DEFENCE_LEVEL);
		}

		GetDlgItem("Img_HighLight1")->Show(false);
		GetDlgItem("Img_HighLight2")->Show(false);
		GetDlgItem("Img_HighLight3")->Show(false);

		temp = CalcDegrade(m_prof.prof[PHYSICAL_DEFENCE], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//物理防御减免
		m_pTxt_Tome[PHYSICAL_DEFENCE]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[PHYSICAL_DEFENCE], temp);
		
		temp = CalcDegrade(m_prof.prof[MAGIC_DEFENCE_GOLD], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//金系防御减免
		m_pTxt_Tome[MAGIC_DEFENCE_GOLD]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[MAGIC_DEFENCE_GOLD], temp);

		temp = CalcDegrade(m_prof.prof[MAGIC_DEFENCE_WOOD], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//木系防御减免
		m_pTxt_Tome[MAGIC_DEFENCE_WOOD]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[MAGIC_DEFENCE_WOOD], temp);

		temp = CalcDegrade(m_prof.prof[MAGIC_DEFENCE_WATER], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//水系防御减免
		m_pTxt_Tome[MAGIC_DEFENCE_WATER]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[MAGIC_DEFENCE_WATER], temp);

		temp = CalcDegrade(m_prof.prof[MAGIC_DEFENCE_FAIR], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//火系防御减免
		m_pTxt_Tome[MAGIC_DEFENCE_FAIR]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[MAGIC_DEFENCE_FAIR], temp);

		temp = CalcDegrade(m_prof.prof[MAGIC_DEFENCE_EARTH], m_prof.level);
		a_Clamp(temp, 10, 90);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7761));																		//土系防御减免
		m_pTxt_Tome[MAGIC_DEFENCE_EARTH]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[MAGIC_DEFENCE_EARTH], temp);
		
		temp = (int)(ExtProps.ak.attack / (ExtProps.ak.attack + m_prof.prof[ARMOR] * 0.5f) * 100);
		a_Clamp(temp, 5, 95);
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7763));																		//躲闪度减免
		m_pTxt_Tome[ARMOR]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[ARMOR], temp);

		temp = BasicProps.iAtkDegree > m_prof.prof[DEFENCE_LEVEL]? BasicProps.iAtkDegree - m_prof.prof[DEFENCE_LEVEL] : 0;
		strText.Format(_AL("%d%%%s"), temp, GetStringFromTable(7764));																		//防御等级减免
		m_pTxt_Tome[DEFENCE_LEVEL]->SetText(strText);
		SetReduceColor(m_pTxt_Tome[DEFENCE_LEVEL], temp);

		GetDlgItem("Txt_Tome_a")->Show(false);
		GetDlgItem("Txt_Tome_b")->Show(false);
	}
}

void CDlgProfView::SetAttckColor(PAUILABEL pObj, int attack)
{
	if (attack <= 2500)
	{
		pObj->SetColor(s_color[0]);
	}
	else if (attack <= 10000)
	{
		pObj->SetColor(s_color[1]);
	}
	else if (attack <= 18750)
	{
		pObj->SetColor(s_color[2]);
	}
	else if (attack <= 21250)
	{
		pObj->SetColor(s_color[3]);
	}
	else
	{
		pObj->SetColor(s_color[4]);
	}
}

void CDlgProfView::SetDefenceColor(PAUILABEL pObj, int defence)
{
	if (defence <= 17500)
	{
		pObj->SetColor(s_color[0]);
	}
	else if (defence <= 35000)
	{
		pObj->SetColor(s_color[1]);
	}
	else if (defence <= 70000)
	{
		pObj->SetColor(s_color[2]);
	}
	else if (defence <= 280000)
	{
		pObj->SetColor(s_color[3]);
	}
	else
	{
		pObj->SetColor(s_color[4]);
	}
}

void CDlgProfView::SetReduceColor(PAUILABEL pObj, int percent)
{
	if (percent <= 20)
	{
		pObj->SetColor(s_color[0]);
	}
	else if (percent <= 50)
	{
		pObj->SetColor(s_color[1]);
	}
	else if (percent <= 75)
	{
		pObj->SetColor(s_color[2]);
	}
	else if (percent <= 85)
	{
		pObj->SetColor(s_color[3]);
	}
	else
	{
		pObj->SetColor(s_color[4]);
	}
}
