// Filename	: DlgCharacter.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "WindowsX.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "AUIProgress.h"
#include "DlgCharacter.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "EC_UIConfigs.h"
#include "elementdataman.h"
#include "EC_ForceMgr.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_GameUIMan.h"
#include "EC_TimeSafeChecker.h"

#include "AUIStillImageButton.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	618
#define INTERAL_SHORT	100

AUI_BEGIN_COMMAND_MAP(CDlgCharacter, CDlgBase)

AUI_ON_COMMAND("addauto",		OnCommand_addauto)
AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("reset",			OnCommand_reset)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Force",		OnCommand_force)
AUI_ON_COMMAND("Btn_TitleList",	OnCommand_Title)
AUI_ON_COMMAND("Btn_Book",		OnCommand_ReincarnationBook)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCharacter, CDlgBase)

AUI_ON_EVENT("Img_Char",	WM_MOUSEMOVE,		OnEventMouseMove)
AUI_ON_EVENT("Img_Char",	WM_LBUTTONDOWN,		OnEventLButtonDown_Size)
AUI_ON_EVENT("Img_Char",	WM_LBUTTONUP,		OnEventLButtonUp_Size)
AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONDOWN,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONDBLCLK,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONDOWN,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONDBLCLK,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONUP,		OnEventLBUttonUp_stop)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONUP,		OnEventLBUttonUp_stop)

AUI_END_EVENT_MAP()

CDlgCharacter::CDlgCharacter()
{
	m_nMouseOffset = 0;
	m_nMouseOffsetThis = 0;
	m_iIntervalTime = INTERAL_LONG;
	m_dwLastTime = 0;
	m_pButtonPress = NULL;
}

CDlgCharacter::~CDlgCharacter()
{
}

bool CDlgCharacter::OnInitDialog()
{
	m_pTxt_CharName = (PAUILABEL)GetDlgItem("Txt_CharName");
	m_pTxt_CharLevel = (PAUILABEL)GetDlgItem("Txt_CharLevel");
	m_pTxt_Profession = (PAUILABEL)GetDlgItem("Txt_Profession");
	m_pTxt_ExpCurrent = (PAUILABEL)GetDlgItem("Txt_ExpCurrent");
	m_pTxt_ExpRequire = (PAUILABEL)GetDlgItem("Txt_ExpRequire");
	m_pTxt_Faction = (PAUILABEL)GetDlgItem("Txt_Faction");
	m_pTxt_Partner = (PAUILABEL)GetDlgItem("Txt_Partner");
	m_pTxt_Distinction = (PAUILABEL)GetDlgItem("Txt_Distinction");
	m_pTxt_xz = (PAUILABEL)GetDlgItem("Txt_xz");
	m_pTxt_HP = (PAUILABEL)GetDlgItem("Txt_HP");
	m_pTxt_MP = (PAUILABEL)GetDlgItem("Txt_MP");
	m_pTxt_SP = (PAUILABEL)GetDlgItem("Txt_SP");
	m_pTxt_Point = (PAUILABEL)GetDlgItem("Txt_Point");
	m_pTxt_Str = (PAUILABEL)GetDlgItem("Txt_Str");
	m_pTxt_Agi = (PAUILABEL)GetDlgItem("Txt_Agi");
	m_pTxt_Vit = (PAUILABEL)GetDlgItem("Txt_Vit");
	m_pTxt_Int = (PAUILABEL)GetDlgItem("Txt_Int");
	m_pTxt_Attack = (PAUILABEL)GetDlgItem("Txt_Attack");
	m_pTxt_AtkSpeed = (PAUILABEL)GetDlgItem("Txt_AtkSpeed");
	m_pTxt_Definition = (PAUILABEL)GetDlgItem("Txt_Definition");
	m_pTxt_Evade = (PAUILABEL)GetDlgItem("Txt_Evade");
	m_pTxt_MoveSpeed = (PAUILABEL)GetDlgItem("Txt_MoveSpeed");
	m_pTxt_MgcAttack = (PAUILABEL)GetDlgItem("Txt_MgcAttack");
	m_pTxt_Critical = (PAUILABEL)GetDlgItem("Txt_Critical");
	m_pTxt_PhyDefense = (PAUILABEL)GetDlgItem("Txt_PhyDefense");
	m_pTxt_MgcDefense = (PAUILABEL)GetDlgItem("Txt_MgcDefense");
	m_pTxt_AttackLevel = (PAUILABEL)GetDlgItem("Txt_AttackLevel");
	m_pTxt_DefenseLevel = (PAUILABEL)GetDlgItem("Txt_DefenseLevel");
	m_pNew_PhyDefense = (PAUILABEL)GetDlgItem("New_PhyDefense");
	m_pNew_MgcDefense = (PAUILABEL)GetDlgItem("New_MgcDefense");;
	m_pNew_AttackLevel = (PAUILABEL)GetDlgItem("New_AttackLevel");
	m_pNew_DefenseLevel = (PAUILABEL)GetDlgItem("New_DefenseLevel");
	m_pImg_Char = (PAUIIMAGEPICTURE)GetDlgItem("Img_Char");
	m_pImg_Char->SetAcceptMouseMessage(true);
	m_pTxt_CrtPower = (PAUILABEL)GetDlgItem("Txt_CrtPower");
	m_pTxt_SoulPower = (PAUILABEL)GetDlgItem("Txt_SoulPower");
	m_pBtn_Force = GetDlgItem("Btn_Force");
	m_pTxt_Force = (PAUILABEL)GetDlgItem("Txt_Force");
	PAUIOBJECT pBtn = GetDlgItem("Btn_TitleList");
	if (pBtn) pBtn->Show(CECUIConfig::Instance().GetGameUI().bEnableTitle);
	m_pBtn_Book = GetDlgItem("Btn_Book");

	return true;
}

void CDlgCharacter::OnCommand_add(const char * szCommand)
{
	ROLEBASICPROP rbp = GetHostPlayer()->GetBasicProps();

	if( m_nStatusPtUsed < rbp.iStatusPt )
	{
		if( 0 == stricmp(szCommand, "addstr") )
			m_repBase.strength++;
		else if( 0 == stricmp(szCommand, "addagi") )
			m_repBase.agility++;
		else if( 0 == stricmp(szCommand, "addvit") )
			m_repBase.vitality++;
		else
			m_repBase.energy++;
		m_nStatusPtUsed++;
	}
}

void CDlgCharacter::OnCommand_minusstr(const char * szCommand)
{
	if( m_repBase.strength > 0 )
	{
		m_repBase.strength--;
		m_nStatusPtUsed--;
	}
}

void CDlgCharacter::OnCommand_minusagi(const char * szCommand)
{
	if( m_repBase.agility > 0 )
	{
		m_repBase.agility--;
		m_nStatusPtUsed--;
	}
}

void CDlgCharacter::OnCommand_minusvit(const char * szCommand)
{
	if( m_repBase.vitality > 0 )
	{
		m_repBase.vitality--;
		m_nStatusPtUsed--;
	}
}

void CDlgCharacter::OnCommand_minusint(const char * szCommand)
{
	if( m_repBase.energy > 0 )
	{
		m_repBase.energy--;
		m_nStatusPtUsed--;
	}
}

void CDlgCharacter::OnCommand_addauto(const char * szCommand)
{
}

void CDlgCharacter::OnCommand_confirm(const char * szCommand)
{
	GetGameSession()->c2s_CmdSetStatusPts(
		m_repBase.vitality, m_repBase.energy,
		m_repBase.strength, m_repBase.agility);
	ResetPoints();
}

void CDlgCharacter::OnCommand_reset(const char * szCommand)
{
	ResetPoints();
}

void CDlgCharacter::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgCharacter::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( (wParam & MK_LBUTTON) )
	{
		m_nMouseOffsetThis = GET_X_LPARAM(lParam) - m_nMouseLastX;
		if( m_nMouseOffset + m_nMouseOffsetThis > 70 )
			m_nMouseOffsetThis = 70 - m_nMouseOffset;
		else if( m_nMouseOffset + m_nMouseOffsetThis < -70 )
			m_nMouseOffsetThis = -70 - m_nMouseOffset;
	}
}

void CDlgCharacter::OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(pObj);
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseOffset += m_nMouseOffsetThis;
	m_nMouseOffsetThis = 0;
}

void CDlgCharacter::OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(NULL);
}

void CDlgCharacter::OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyAdd(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = true;

}

void CDlgCharacter::PropertyAdd(AUIObject *pObj)
{
	ROLEBASICPROP rbp = GetHostPlayer()->GetBasicProps();
	const char *szCommand = pObj->GetName();
	if( m_nStatusPtUsed < rbp.iStatusPt )
	{
		if( 0 == stricmp(szCommand, "Btn_AddStr") )
		{
			m_repBase.strength++;
		}
		else if( 0 == stricmp(szCommand, "Btn_AddAgi") )
		{
			m_repBase.agility++;
		}
		else if( 0 == stricmp(szCommand, "Btn_AddVit") )
		{
			m_repBase.vitality++;
		}
		else
		{
			m_repBase.energy++;
		}
		m_nStatusPtUsed++;
	}
}

void CDlgCharacter::OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyMinus(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = false;
}

void CDlgCharacter::PropertyMinus(AUIObject *pObj)
{
	const char *szCommand = pObj->GetName();
	if( 0 == stricmp(szCommand, "Btn_MinusStr") )
	{
		OnCommand_minusstr(NULL);
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusAgi") )
	{
		OnCommand_minusagi(NULL);
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusVit") )
	{
		OnCommand_minusvit(NULL);
	}
	else
	{
		OnCommand_minusint(NULL);
	}
}

void CDlgCharacter::OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_pButtonPress = NULL;
}

void CDlgCharacter::OnTick(void)
{
	if (m_pButtonPress != NULL )
	{
		if (AUISTILLIMAGEBUTTON_STATE_NORMAL != m_pButtonPress->GetState())
		{
			if (CECTimeSafeChecker::ElapsedTimeFor(m_dwLastTime) >= (DWORD)m_iIntervalTime)
			{
				if (m_bAdd)
				{
					PropertyAdd(m_pButtonPress);
				}
				else
					PropertyMinus(m_pButtonPress);
				m_dwLastTime += m_iIntervalTime;

				if (m_iIntervalTime == INTERAL_LONG)
					m_iIntervalTime = INTERAL_SHORT;
			}
		}
		else
		{
			m_pButtonPress = NULL;
		}
	}
}

bool CDlgCharacter::Render(void)
{

	RefreshHostDetails();

	return CDlgBase::Render();
}

void CDlgCharacter::RefreshHostDetails()
{
	if( !IsShow() ) return;

	ACHAR szText[512];
	ACString strText;
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	const ROLEEXTPROP &rep = GetHostPlayer()->GetExtendProps();

	AUI_ConvertChatString(GetHostPlayer()->GetName(), szText);
	m_pTxt_CharName->SetText(szText);

	a_sprintf(szText, _AL("%d"), rbp.iLevel);
	m_pTxt_CharLevel->SetText(szText);

	m_pTxt_Profession->SetText(GetGameRun()->GetProfName(GetHostPlayer()->GetProfession()));

	Faction_Info *pInfo = NULL;
	int idFaction = GetHostPlayer()->GetFactionID();
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	if( idFaction ) pInfo = pFMan->GetFaction(idFaction);
	if( idFaction <= 0 )
		m_pTxt_Faction->SetText(GetStringFromTable(251));
	else if( pInfo )
		m_pTxt_Faction->SetText(pInfo->GetName());
	else
		m_pTxt_Faction->SetText(_AL(""));

	if( GetHostPlayer()->GetSpouse() )
	{
		const ACHAR *szSpouseName = GetGameRun()->GetPlayerName(GetHostPlayer()->GetSpouse(), false);
		if( szSpouseName )
			m_pTxt_Partner->SetText(szSpouseName);
		else
			m_pTxt_Partner->SetText(_AL(""));
	}
	else
		m_pTxt_Partner->SetText(GetStringFromTable(786));

	CECIvtrItem *pItem = NULL;
	CECIvtrEquip *pEquip = NULL;
	CECInventory *pInventory = GetHostPlayer()->GetEquipment();
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair;

	int iFourPro[4] = {0};	//[0]Str, [1]Agi, [2]Int, [3]Vit

	for (int i = 0; i <SIZE_ALL_EQUIPIVTR; i++)
	{
		pItem = pInventory->GetItem(i);
		if( pItem )
		{
			pEquip = (CECIvtrEquip *)pItem;
			int iProperNum = pEquip->GetPropertyNum();
			for (int j = 0; j < iProperNum; j++)
			{
				const CECIvtrEquip::PROPERTY pro = pEquip->GetProperty(j);
				Pair = PropTab.get(pro.iType);
				BYTE byPropType = Pair.second ? *Pair.first : 0xff;
				if (41 <= byPropType && byPropType <= 44)
					iFourPro[byPropType-41] += pro.aParams[0];
				else if (106 <= byPropType && byPropType <= 108)
					iFourPro[byPropType-106] += pro.aParams[0];
				else if (95 <= byPropType && byPropType <= 98)
					iFourPro[byPropType-95] += pro.aParams[0];
				else if (128 <= byPropType && byPropType <= 131){
					int attributeOrder[4] = {3, 0, 1, 2};	//	将体力、力量、敏捷、灵力按力量、敏捷、灵力、体力的顺序映射到结果数组中
					iFourPro[attributeOrder[byPropType-128]] += pro.aParams[0];
				}
			}
		}
	}


	a_sprintf(szText, _AL("%d"), rbp.iExp);
	m_pTxt_ExpCurrent->SetText(szText);

	a_sprintf(szText, _AL("%d"), GetHostPlayer()->GetLevelUpExp(rbp.iLevel));
	m_pTxt_ExpRequire->SetText(szText);

	a_sprintf(szText, _AL("%d"), GetHostPlayer()->GetReputation());
	m_pTxt_Distinction->SetText(szText);

	m_pTxt_xz->SetText(m_pAUIManager->GetStringFromTable(1001 + rbp.iLevel2));

	a_sprintf(szText, _AL("%d/%d"), rbp.iCurHP, rep.bs.max_hp);
	m_pTxt_HP->SetText(szText);

	a_sprintf(szText, _AL("%d/%d"), rbp.iCurMP, rep.bs.max_mp);
	m_pTxt_MP->SetText(szText);

	a_sprintf(szText, _AL("%d"), rbp.iSP);
	m_pTxt_SP->SetText(szText);

	a_sprintf(szText, _AL("%d"), rbp.iStatusPt - m_nStatusPtUsed);
	m_pTxt_Point->SetText(szText);

	a_sprintf(szText, _AL("%d"), rep.bs.strength + m_repBase.strength);
	m_pTxt_Str->SetText(szText);
	if (0 != iFourPro[0])
	{
		m_pTxt_Str->SetFontAndColor(NULL, 0, A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Str->SetFontAndColor(NULL, 0, A3DCOLORRGB(255, 255, 255));

	a_sprintf(szText, _AL("%d"), rep.bs.agility + m_repBase.agility);
	m_pTxt_Agi->SetText(szText);
	if (0 != iFourPro[1])
	{
		m_pTxt_Agi->SetFontAndColor(NULL, 0, A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Agi->SetFontAndColor(NULL, 0, A3DCOLORRGB(255, 255, 255));

	a_sprintf(szText, _AL("%d"), rep.bs.vitality + m_repBase.vitality);
	m_pTxt_Vit->SetText(szText);
	if (0 != iFourPro[3])
	{
		m_pTxt_Vit->SetFontAndColor(NULL, 0, A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Vit->SetFontAndColor(NULL, 0, A3DCOLORRGB(255, 255, 255));

	a_sprintf(szText, _AL("%d"), rep.bs.energy + m_repBase.energy);
	m_pTxt_Int->SetText(szText);
	if (0 != iFourPro[2])
	{
		m_pTxt_Int->SetFontAndColor(NULL, 0, A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Int->SetFontAndColor(NULL, 0, A3DCOLORRGB(255, 255, 255));

	a_sprintf(szText, _AL("%d-%d"), rep.ak.damage_low, rep.ak.damage_high);
	m_pTxt_Attack->SetText(szText);

	strText.Format(_AL("%4.2f %s"), rep.ak.attack_speed ? 1.0f
		/ (rep.ak.attack_speed * 0.05f) : 0.0f, m_pAUIManager->GetStringFromTable(279));
	m_pTxt_AtkSpeed->SetText(strText);

	a_sprintf(szText, _AL("%d"), rep.ak.attack);
	m_pTxt_Definition->SetText(szText);

	a_sprintf(szText, _AL("%d"), rep.df.armor);
	m_pTxt_Evade->SetText(szText);

	strText.Format(_AL("%3.1f %s"), rep.mv.run_speed, m_pAUIManager->GetStringFromTable(280));
	m_pTxt_MoveSpeed->SetText(strText);

	a_sprintf(szText, _AL("%d-%d"), rep.ak.damage_magic_low, rep.ak.damage_magic_high);
	m_pTxt_MgcAttack->SetText(szText);
	
	PAUIOBJECT pObj = NULL;
	int vigour_realm(0);
	int realm_level = GetHostPlayer()->GetRealmLevel();
	int layer= GetHostPlayer()->GetRealmLayer();
	int sub_level = GetHostPlayer()->GetRealmSubLevel();
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Realm"));
	if (pImg && layer>=0) {
		pImg->Show(true);
		pImg->FixFrame(layer);
	}
	if (realm_level > 0){
		pObj = GetDlgItem("Txt_Realm");
		pObj->Show(true);
		pObj->SetText(GetGameUIMan()->GetRealmName(realm_level));

		int require_exp(0);
		const PLAYER_REALM_CONFIG* pConfig = GetHostPlayer()->GetRealmConfig();
		if (pConfig) 
			require_exp = layer == 0 ? 0 : pConfig->list[layer - 1].level[sub_level - 1].require_exp; 

		PAUIPROGRESS pProg = (PAUIPROGRESS)GetDlgItem("Pro_RealmExp");
		if (pProg)
			pProg->Show(true);

		if (require_exp) {
			pProg->SetProgress(int((double)max(GetHostPlayer()->GetRealmExp(), 0) * AUIPROGRESS_MAX / require_exp));
			strText.Format(GetGameUIMan()->GetStringFromTable(11164), GetHostPlayer()->GetRealmExp(), require_exp);
			pProg->SetHint(strText);

		}
		else{
			pProg->SetProgress(AUIPROGRESS_MAX);
			pProg->SetHint(_AL(""));
		}
		vigour_realm = pConfig->list[layer - 1].level[sub_level - 1].vigour;
	}else{
		pObj = GetDlgItem("Txt_Realm");
		pObj->Show(false);
		pObj = GetDlgItem("Pro_RealmExp");		
		pObj->Show(false);
	}
	pObj = GetDlgItem("Txt_Vigour");
	strText.Format(_AL("%d"), rbp.iVigour);
	pObj->SetText(strText);
	pObj = GetDlgItem("New_Vigour");
	strText.Format(GetStringFromTable(11146), vigour_realm);
	pObj->SetHint(strText);
	

/*	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, DataType);
	while( id )
	{
		if( DataType == DT_CHARRACTER_CLASS_CONFIG )
		{
			const CHARRACTER_CLASS_CONFIG *pData = (const CHARRACTER_CLASS_CONFIG *)
				pDataMan->get_data_ptr(id, ID_SPACE_CONFIG, DataType);

			if( (int)pData->character_class_id == GetHostPlayer()->GetProfession() )
			{
				a_sprintf(szText, _AL("%d%c"), pData->crit_rate, '%');
				m_pTxt_Critical->SetText(szText);
				break;
			}
		}

		id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
*/
	a_sprintf(szText, _AL("%d%%"), rbp.iCritRate);
	m_pTxt_Critical->SetText(szText);
	
	#define DefenceToPercent(def)\
	fReduce = 100.f * def / (def + 40.f * nLevel - 25);\
	fReduce = fReduce > 95.f ? 95.f : fReduce

	int nLevel = rbp.iLevel;
	float fReduce;

	a_sprintf(szText, _AL("%d"), rep.df.defense);
	m_pTxt_PhyDefense->SetText(szText);
	DefenceToPercent(rep.df.defense);
	strText.Format(m_pAUIManager->GetStringFromTable(490), 
		nLevel, fReduce);
	m_pNew_PhyDefense->SetHint(strText);
	
	if (m_pNew_MgcDefense || m_pTxt_MgcDefense){
		int nSumDefense(0);
		ACString strMagicText;
		for (int i(0); i < NUM_MAGICCLASS; ++ i)
		{
			nSumDefense += rep.df.resistance[i];
			DefenceToPercent(rep.df.resistance[i]);
			strText.Format(m_pAUIManager->GetStringFromTable(491+i), rep.df.resistance[i], nLevel, fReduce);
			if (!strMagicText.IsEmpty()){
				strMagicText += _AL("\r");
			}
			strMagicText += strText;
		}
		if (m_pTxt_MgcDefense){
			a_sprintf(szText, _AL("%d"), nSumDefense/NUM_MAGICCLASS);
			m_pTxt_MgcDefense->SetText(szText);
		}
		if (m_pNew_MgcDefense){
			m_pNew_MgcDefense->SetHint(strMagicText);
		}
	}
	
	a_sprintf(szText, _AL("%d"), rbp.iAtkDegree);
	m_pTxt_AttackLevel->SetText(szText);
	
	a_sprintf(szText, _AL("%d"), rbp.iDefDegree);
	m_pTxt_DefenseLevel->SetText(szText);

	a_sprintf(szText, _AL("%d%%"), rbp.iCritDamageBonus+200);
	m_pTxt_CrtPower->SetText(szText);
	
	int soulPower = GetHostPlayer()->GetSoulPower();
	if (soulPower >= 0)
		a_sprintf(szText, _AL("%d"), soulPower);
	else
		a_sprintf(szText, _AL("-"));
	m_pTxt_SoulPower->SetText(szText);

	if (soulPower > 0 && GetHostPlayer()->GetProfession() == PROF_MONK)
	{
		ACString strTemp;
		strText = GetStringFromTable(8136);
		strText += _AL("\r");

		strTemp.Format(GetStringFromTable(8130), soulPower);
		strText += strTemp + _AL("\r");

		strTemp.Format(GetStringFromTable(8131), (int)floor(soulPower*0.08f*(1+0.01f*rbp.iAtkDegree)));
		strText += strTemp + _AL("\r");

		strTemp.Format(GetStringFromTable(8132), (int)floor(soulPower*0.0002f+1));
		strText += strTemp + _AL("\r");

		strTemp.Format(GetStringFromTable(8133), soulPower*0.0006f);
		strText += strTemp + _AL("\r");

		strTemp.Format(GetStringFromTable(8134), (int)floor(soulPower*0.5f));
		strText += strTemp;
	}
	else
	{
		strText = GetStringFromTable(8135);
	}
	GetDlgItem("New_SoulPower")->SetHint(strText);
	
	a_sprintf(szText, _AL("%d"), rbp.iInvisibleDegree);
	GetDlgItem("Txt_Stealth")->SetText(szText);
	a_sprintf(szText, _AL("%d"), rbp.iAntiInvisibleDegree);
	GetDlgItem("Txt_Antistealth")->SetText(szText);

	a_sprintf(szText, _AL("%d"), rbp.iPenetration);
	GetDlgItem("Txt_Penetration")->SetText(szText);
	strText.Format(GetStringFromTable(9380), (int)(100*rbp.iPenetration*3/(float)(rbp.iPenetration + 300)));
	GetDlgItem("New_Penetration")->SetHint(strText);

	a_sprintf(szText, _AL("%d"), rbp.iResilience);
	GetDlgItem("Txt_Resilience")->SetText(szText);
	strText.Format(GetStringFromTable(9381), nLevel, (int)(100*rbp.iResilience/(float)(rbp.iResilience+nLevel)));
	GetDlgItem("New_Resilience")->SetHint(strText);
	
	if( GetHostPlayer()->IsAllResReady() )
	{
		m_pImg_Char->SetRenderCallback(PlayerRenderPortrait,
			(DWORD)GetHostPlayer(), m_nMouseOffset + m_nMouseOffsetThis);
	}
	else
		m_pImg_Char->SetRenderCallback(NULL, 0);

	// refresh the force info
	int forceid = GetHostPlayer()->GetForce();
	if(m_pBtn_Force) m_pBtn_Force->Enable(forceid > 0);
	if(m_pTxt_Force)
	{
		const FORCE_CONFIG* pConfig = GetHostPlayer()->GetForceMgr()->GetForceData(forceid);
		m_pTxt_Force->SetText(pConfig ? pConfig->name : _AL(""));
	}
	PAUIOBJECT pTitle = GetDlgItem("Txt_Title");
	if (pTitle) {
		const TITLE_CONFIG* pConfig = CECPlayer::GetTitleConfig(GetHostPlayer()->GetCurrentTitle());
		ACString strTitle;
		if (pConfig) {
			if (pConfig->after_name) strTitle.Format(GetStringFromTable(10608), pConfig->name);
			else strTitle.Format(GetStringFromTable(10607), pConfig->name); 
			pTitle->SetText(strTitle);
		} else pTitle->SetText(_AL(""));
	}
	if (m_pBtn_Book){
		m_pBtn_Book->Enable(GetHostPlayer()->GetReincarnationCount()>0);
	}
}

void CDlgCharacter::ResetPoints()
{
	m_nStatusPtUsed = 0;
	memset(&m_repBase, 0, sizeof(ROLEEXTPROP_BASE));
}

void CDlgCharacter::OnCommand_force(const char * szCommand)
{
	PAUIDIALOG pForce = m_pAUIManager->GetDialog("Win_Force");
	if(pForce) pForce->Show(!pForce->IsShow());
}

void CDlgCharacter::OnCommand_Title(const char * szCommand)
{	
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_TitleList");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}
void CDlgCharacter::OnCommand_ReincarnationBook(const char * szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_ReincarnationBook");
	if (pDlg && GetHostPlayer()->GetReincarnationCount()>0) pDlg->Show(!pDlg->IsShow());
}