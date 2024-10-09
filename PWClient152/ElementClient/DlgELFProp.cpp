// Filename	: DlgELFProp.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#include "WindowsX.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFProp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "elementdataman.h"
#include "AFI.h"

#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUIStillImageButton.h"
#include "AUICTranslate.h"
#include "DlgELFGenius.h"
#include "DlgELFInExp.h"
#include "EC_HostGoblin.h"
#include "EC_IvtrGoblin.h"
#include "DlgELFTransaction.h"
#include "EC_ShortcutMgr.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	500
#define INTERAL_SHORT	200
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFProp, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("reset",			OnCommand_reset)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("skill",			OnCommand_Genius)
AUI_ON_COMMAND("equip",			OnCommand_Equip)
AUI_ON_COMMAND("safe",			OnCommand_Safe)
AUI_ON_COMMAND("inexp",			OnCommand_InExp)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFProp, CDlgBase)

AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONDOWN,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONDBLCLK,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONDOWN,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONDBLCLK,		OnEventLButtonDown_minus)
AUI_ON_EVENT("Btn_Add*",	WM_LBUTTONUP,		OnEventLBUttonUp_stop)
AUI_ON_EVENT("Btn_Minus*",	WM_LBUTTONUP,		OnEventLBUttonUp_stop)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFProp
//------------------------------------------------------------------------
class ELFPropClickShortcut : public CECShortcutMgr::SimpleClickShortcut<CDlgELFProp>
{
	typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFProp> Base;
public:
	ELFPropClickShortcut(CDlgELFProp* pDlg):Base(pDlg){};
	virtual bool CanTrigger(PAUIOBJECT pSrcObj)
	{
		return Base::CanTrigger(pSrcObj) && 
			   // skip this trigger if this window shown
			   !m_pDlg->GetAUIManager()->GetDialog("Win_ELFInExp")->IsShow();
	}
};
//------------------------------------------------------------------------

CDlgELFProp::CDlgELFProp()
{
	m_nMouseOffset = 0;
	m_nMouseOffsetThis = 0;
	m_iIntervalTime = INTERAL_LONG;
	m_dwLastTime = 0;
	m_pButtonPress = NULL;
	m_dwFreezeTime = 0;
}

CDlgELFProp::~CDlgELFProp()
{
}

bool CDlgELFProp::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFPropClickShortcut(this));

	m_pTxt_ELFName = (PAUILABEL)GetDlgItem("Txt_Name");	//
	m_pTxt_ELFLevel = (PAUILABEL)GetDlgItem("Txt_Level");	//
	m_pTxt_Describe = (PAUILABEL)GetDlgItem("Txt_DESC");	//
	m_pTxt_PointRemain = (PAUILABEL)GetDlgItem("Txt_Shengyudian");
	m_pTxt_Str = (PAUILABEL)GetDlgItem("Txt_STR");	//	
	m_pTxt_Agi = (PAUILABEL)GetDlgItem("Txt_AGI");	//
	m_pTxt_Vit = (PAUILABEL)GetDlgItem("Txt_VIT");	//
	m_pTxt_Nig = (PAUILABEL)GetDlgItem("Txt_NIG");	//
	m_pTxt_Power = (PAUILABEL)GetDlgItem("Txt_Power");	//
	m_pTxt_ReplyPower = (PAUILABEL)GetDlgItem("Txt_ReplyPower");	//
	m_pTxt_Endurance = (PAUILABEL)GetDlgItem("Txt_Endurance");	//
	m_pTxt_SuccinctLevel = (PAUILABEL)GetDlgItem("Txt_SuccinctLevel");	//
	m_pTxt_SuccinctFAQ = (PAUILABEL)GetDlgItem("Txt_SuccinctFAQ");	//
	m_pTxt_PointEx = (PAUILABEL)GetDlgItem("Txt_PointEx");
	m_pTxt_EXP = (PAUILABEL)GetDlgItem("Txt_EXP");	//
	m_pTxt_Chengzhang = (PAUILABEL)GetDlgItem("Txt_Chengzhang"); //
	m_pImg_Char = (PAUIIMAGEPICTURE)GetDlgItem("Img_Elf");	//
	m_pPGS_EXP = (PAUIPROGRESS)GetDlgItem("Progress_EXP"); //
	char szItem[40];
	for(int i = 0; i < 4; i++ )
	{
		sprintf(szItem, "Item_%d", i+1);
		m_pImgEquip[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}
	m_pEquip = NULL;

	return true;
}

void CDlgELFProp::OnShowDialog()
{
	UpdateEquip();
}

void CDlgELFProp::OnCommand_minusstr(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if( m_repBase.strength > 0 )
	{
		m_repBase.strength--;
		m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetStrength() + m_repBase.strength, -1);
	}
}

void CDlgELFProp::OnCommand_minusagi(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if( m_repBase.agility > 0 )
	{
		m_repBase.agility--;
		m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetAgility() + m_repBase.agility, -1);
	}
}

void CDlgELFProp::OnCommand_minusvit(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if( m_repBase.vitality > 0 )
	{
		m_repBase.vitality--;
		m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetVitality() + m_repBase.vitality, -1);
	}
}

void CDlgELFProp::OnCommand_minusint(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if( m_repBase.energy > 0 )
	{
		m_repBase.energy--;
		m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetEnergy() + m_repBase.energy, -1);
	}
}

void CDlgELFProp::OnCommand_addauto(const char * szCommand)
{
}

void CDlgELFProp::OnCommand_confirm(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (m_repBase.strength + m_repBase.agility + m_repBase.vitality + m_repBase.energy != 0)
	{
		ACString strProb = _AL("");
		if (m_repBase.strength > 0 && pHostGoblin->NeedStatusPt(pHostGoblin->GetStrength()+m_repBase.strength , 1) > 1)
		{
			strProb += GetStringFromTable(7115);
		}
		if (m_repBase.agility && pHostGoblin->NeedStatusPt(pHostGoblin->GetAgility()+m_repBase.agility , 1) > 1)
		{
			strProb += GetStringFromTable(7116);
		}
		if (m_repBase.vitality && pHostGoblin->NeedStatusPt(pHostGoblin->GetVitality()+m_repBase.vitality , 1) > 1)
		{
			strProb += GetStringFromTable(7117);
		}
		if (m_repBase.energy && pHostGoblin->NeedStatusPt(pHostGoblin->GetEnergy()+m_repBase.energy , 1) > 1)
		{
			strProb += GetStringFromTable(7118);
		}

		if (strProb != _AL(""))
		{
			PAUIDIALOG pMsgBox;
			ACString str;
			str.Format(GetStringFromTable(7166), strProb);
			GetGameUIMan()->MessageBox("Game_Over40Point", str, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 166), &pMsgBox);
		}
		else
			OnCommand_YES();
	}
}

void CDlgELFProp::OnCommand_YES()
{
	GetGameSession()->c2s_CmdGoblinAddAttribute(
		m_repBase.strength, m_repBase.agility,
		m_repBase.vitality, m_repBase.energy);
	ResetPoints();
	m_dwFreezeTime = GetTickCount();
}

void CDlgELFProp::OnCommand_reset(const char * szCommand)
{
	ResetPoints();
}

void CDlgELFProp::OnCommand_CANCEL(const char * szCommand)
{
	ResetPoints();
	m_dwFreezeTime = 0;
	Show(false);
}

void CDlgELFProp::OnCommand_Genius(const char * szCommand)
{
	CDlgELFGenius *pDlgGenius = GetGameUIMan()->m_pDlgELFGenius;
	pDlgGenius->Show(!pDlgGenius->IsShow());
}

void CDlgELFProp::OnCommand_Equip(const char * szCommand)
{

}

void CDlgELFProp::OnCommand_Safe(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	int status = pHostGoblin->GetSecurityState();
	if (status == -1)
	{
		GetGameUIMan()->m_pDlgELFTransaction1->Show(true);
		//GetGameSession()->c2s_CmdGoblinChangeSecureStatus(1);
	}
	else
		GetGameUIMan()->m_pDlgELFTransaction2->Show(true);
		//GetGameSession()->c2s_CmdGoblinChangeSecureStatus(0);
	
}

void CDlgELFProp::OnCommand_InExp(const char * szCommand)
{
	CDlgELFInExp *pDlgInExp = GetGameUIMan()->m_pDlgELFInExp;
	pDlgInExp->Show(!pDlgInExp->IsShow());
}

void CDlgELFProp::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
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

void CDlgELFProp::OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(pObj);
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseOffset += m_nMouseOffsetThis;
	m_nMouseOffsetThis = 0;
}

void CDlgELFProp::OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(NULL);
}

void CDlgELFProp::OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = true;
	PropertyAdd(pObj);
}

void CDlgELFProp::PropertyAdd(AUIObject *pObj)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	int iPointRemain = pHostGoblin->UnusedStatusPt();
	const char *szCommand = pObj->GetName();
	if( m_nStatusPtUsed < iPointRemain )
	{
		if( 0 == stricmp(szCommand, "Btn_AddSTR"))
		{
			if (m_nStatusPtUsed + pHostGoblin->NeedStatusPt(pHostGoblin->GetStrength()+m_repBase.strength , 1) <= iPointRemain)
			{
				m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetStrength()+m_repBase.strength , 1);
				m_repBase.strength++;
			}
			else if (iPointRemain - m_nStatusPtUsed == 1)
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(7167), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				m_pButtonPress = NULL;
			}
		}
		else if( 0 == stricmp(szCommand, "Btn_AddAGI") )
		{
			if (m_nStatusPtUsed + pHostGoblin->NeedStatusPt(pHostGoblin->GetAgility()+m_repBase.agility , 1) <= iPointRemain)
			{
				m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetAgility()+m_repBase.agility , 1);
				m_repBase.agility++;
			}
			else if (iPointRemain - m_nStatusPtUsed == 1)
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(7167), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				m_pButtonPress = NULL;
			}
		}
		else if( 0 == stricmp(szCommand, "Btn_AddVIT") )
		{
			if (m_nStatusPtUsed + pHostGoblin->NeedStatusPt(pHostGoblin->GetVitality()+m_repBase.vitality , 1) <= iPointRemain)
			{
				m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetVitality()+m_repBase.vitality , 1);
				m_repBase.vitality++;
			}
			else if (iPointRemain - m_nStatusPtUsed == 1)
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(7167), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				m_pButtonPress = NULL;
			}
		}
		else
		{
			if(m_nStatusPtUsed + pHostGoblin->NeedStatusPt(pHostGoblin->GetEnergy()+m_repBase.energy , 1) <= iPointRemain)
			{
				m_nStatusPtUsed += pHostGoblin->NeedStatusPt(pHostGoblin->GetEnergy()+m_repBase.energy, 1);
				m_repBase.energy++;
			}
			else if (iPointRemain - m_nStatusPtUsed == 1)
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(7167), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				m_pButtonPress = NULL;
			}
		}
	}
}

void CDlgELFProp::OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PropertyMinus(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = false;
}

void CDlgELFProp::PropertyMinus(AUIObject *pObj)
{
	const char *szCommand = pObj->GetName();
	if( 0 == stricmp(szCommand, "Btn_MinusSTR") )
	{
		OnCommand_minusstr(NULL);
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusAGI") )
	{
		OnCommand_minusagi(NULL);
	}
	else if( 0 == stricmp(szCommand, "Btn_MinusVIT") )
	{
		OnCommand_minusvit(NULL);
	}
	else
	{
		OnCommand_minusint(NULL);
	}
}

void CDlgELFProp::OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_pButtonPress = NULL;
}

void CDlgELFProp::OnTick(void)
{
	if (m_dwFreezeTime > 0)
	{
		if (CECTimeSafeChecker::ElapsedTimeFor(m_dwFreezeTime) > 500)
		{
			m_dwFreezeTime = 0;
		}
	}
	if (NULL == m_pButtonPress)
	{
		return ;
	}
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
		}
		if (CECTimeSafeChecker::ElapsedTimeFor(m_dwStartTime) >= TEMPERED_TIME)
		{
			m_iIntervalTime = INTERAL_SHORT;
		}
	}
	else
	{
		m_pButtonPress = NULL;
	}
}

bool CDlgELFProp::Render(void)
{
	if (m_dwFreezeTime == 0)
	{
		RefreshHostDetails();
	}

	return CDlgBase::Render();
}

void CDlgELFProp::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	ACHAR szText[512];
	//ACString strText;

	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (pHostGoblin == NULL)
	{
		OnCommand_CANCEL("IDCANCEL");
		return;
	}

	//遍历装备
	int iFourPro[4] = {0};	//[0]Str, [1]Agi, [2]Int, [3]Vit
	CECIvtrItem *pItem = NULL;
	CECIvtrGoblinEquip *pEquip = NULL;
	CECInventory *pInventory = pHostGoblin->GetEquipment();
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair;
	
	for (int i = 0; i <4; i++)
	{
		pItem = pInventory->GetItem(i);
		if( pItem )
		{
			pEquip = (CECIvtrGoblinEquip *)pItem;
			iFourPro[0] += pEquip->GetDBEssence()->strength;
			iFourPro[1] += pEquip->GetDBEssence()->agility;
			iFourPro[2] += pEquip->GetDBEssence()->energy;
			iFourPro[3] += pEquip->GetDBEssence()->tili;
		}
	}

	GOBLIN_ESSENCE* pEssence = (GOBLIN_ESSENCE *)pHostGoblin->GetDBEssence();
	
	//Level
	a_sprintf(szText, _AL("%s%d"), m_pAUIManager->GetStringFromTable(514), pHostGoblin->GetLevel());
 	m_pTxt_ELFLevel->SetText(szText);

	//Name
	a_sprintf(szText, _AL("%s"), pHostGoblin->GetName());
	m_pTxt_ELFName->SetText(szText);

	//Describe
	if (pHostGoblin->GetBasicDescText())
	{
		m_pTxt_Describe->SetText(pHostGoblin->GetBasicDescText());
	}
	else
		m_pTxt_Describe->SetText(_AL(""));

	//Image
	AString strFile;
	char *pFileIcon;
	int index = pHostGoblin->GetModelID();
	if (1 == index)
	{
		pFileIcon = pEssence->file_icon1;
	}
	else if (2 == index)
	{
		pFileIcon = pEssence->file_icon2;
	}
	else if (3 == index)
	{
		pFileIcon = pEssence->file_icon3;
	}
	else
		pFileIcon = pEssence->file_icon4;
	af_GetFileTitle(pFileIcon, strFile);
	strFile.MakeLower();
	m_pImg_Char->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ELF],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ELF][strFile]);

	//Remain Point
	int iRemainPoint = pHostGoblin->UnusedStatusPt() - m_nStatusPtUsed;
	a_sprintf(szText, _AL("%d"), iRemainPoint);
 	m_pTxt_PointRemain->SetText(szText);

	//PointEx

	int iPointEx = pHostGoblin->GetRandomStatusPt();
	int iPointExMax = pHostGoblin->GetMaxRandomStatusPt();
	a_sprintf(szText, _AL("%d/%d"), iPointEx, iPointExMax);
	m_pTxt_PointEx->SetText(szText);

	int iShowPoint;
	//Strength
	iShowPoint = pHostGoblin->GetStrength() + m_repBase.strength + iFourPro[0] + pHostGoblin->GetStrengthEnhance() + pEssence->init_strength;
	a_sprintf(szText, _AL("%d"), iShowPoint);
 	m_pTxt_Str->SetText(szText);
	if (iFourPro[0] > 0)
	{
		m_pTxt_Str->SetColor(A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Str->SetColor(A3DCOLORRGB(255, 255, 255));

	//Vit
	iShowPoint = pHostGoblin->GetVitality() + m_repBase.vitality + iFourPro[3] + pHostGoblin->GetVitalityEnhance() + pEssence->init_tili;
	a_sprintf(szText, _AL("%d"), iShowPoint);
 	m_pTxt_Vit->SetText(szText);
	if (iFourPro[3] > 0)
	{
		m_pTxt_Vit->SetColor(A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Vit->SetColor(A3DCOLORRGB(255, 255, 255));

	//Agi
	iShowPoint = pHostGoblin->GetAgility() + m_repBase.agility + iFourPro[1] + pHostGoblin->GetAgilityEnhance() + pEssence->init_agility;
	a_sprintf(szText, _AL("%d"), iShowPoint);
 	m_pTxt_Agi->SetText(szText);
	if (iFourPro[1] > 0)
	{
		m_pTxt_Agi->SetColor(A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Agi->SetColor(A3DCOLORRGB(255, 255, 255));

	//NIG
	iShowPoint = pHostGoblin->GetEnergy() + m_repBase.energy + iFourPro[2] + pHostGoblin->GetEnergyEnhance() + pEssence->init_energy;
	a_sprintf(szText, _AL("%d"), iShowPoint);
 	m_pTxt_Nig->SetText(szText);
	if (iFourPro[2] > 0)
	{
		m_pTxt_Nig->SetColor(A3DCOLORRGB(0, 255, 0));
	}
	else
		m_pTxt_Nig->SetColor(A3DCOLORRGB(255, 255, 255));

	//Power
	a_sprintf(szText, _AL("%d/%d"), pHostGoblin->GetAP(), pHostGoblin->GetMaxAP());
 	m_pTxt_Power->SetText(szText);

	//ReplyPower
	a_sprintf(szText, _AL("%.2f%s"), pHostGoblin->GetAPGen(), GetStringFromTable(7190));
 	m_pTxt_ReplyPower->SetText(szText);

	//Stamina
	a_sprintf(szText, _AL("%d/999999"), pHostGoblin->GetStamina());
 	m_pTxt_Endurance->SetText(szText);

	//SuccinctLevel
	a_sprintf(szText, _AL("%d/36"), pHostGoblin->GetRefineLevel());
	m_pTxt_SuccinctLevel->SetText(szText);

	//SuccinctFAQ
	m_pTxt_SuccinctFAQ->SetText(GetAUIManager()->GetStringFromTable(7000 + pHostGoblin->GetRefineLevel()));
	//Txt EXP
	unsigned int iLevelUp = (unsigned int)(GetHostPlayer()->GetLevelUpExp(pHostGoblin->GetLevel()) * pEssence->exp_factor);
	a_sprintf(szText, _AL("%u/%u"), pHostGoblin->GetExp(), iLevelUp);
	m_pTxt_EXP->SetText(szText);

	//Growup
	a_sprintf(szText, _AL("%d"), pHostGoblin->GetGrowDegree());
	m_pTxt_Chengzhang->SetText(szText);

	//Progress EXP
	float fPercent = (float)pHostGoblin->GetExp()/iLevelUp;
	m_pPGS_EXP->SetProgress((int)(fPercent*100));
	UpdateEquip();
}

void CDlgELFProp::ResetPoints()
{
	m_nStatusPtUsed = 0;
	memset(&m_repBase, 0, sizeof(ROLEEXTELF_BASE));
}

void CDlgELFProp::UpdateEquip()
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		return;
	}
	m_pEquip = pHostGoblin->GetEquipment();
	AString strFile;
	if (m_pEquip == NULL)
	{
		return;
	}
	for (int i =0; i < 4; i++)
	{
		CECIvtrItem *pItem = m_pEquip->GetItem(i);
		PAUIIMAGEPICTURE pObj = m_pImgEquip[i];
		if (pItem != NULL)
		{
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");
			pObj->SetData(CECGameUIMan::ICONS_INVENTORY);
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			const wchar_t *szDesc = pItem->GetDesc();
			AUICTranslate trans;
			if( szDesc )
				pObj->SetHint(trans.Translate(szDesc));
			else
				pObj->SetHint(_AL(""));
		}
		else
		{
			pObj->SetDataPtr(NULL, "ptr_CECIvtrItem");
			pObj->ClearCover();
			pObj->SetHint(GetStringFromTable(7309+i));
		}
	}
}

void CDlgELFProp::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	CECIvtrGoblinEquip* pGoblinEqu = dynamic_cast<CECIvtrGoblinEquip*>(pIvtrSrc);
	if(!pGoblinEqu || pGoblinEqu->GetClassID() != CECIvtrItem::ICID_GOBLIN_EQUIP)
	{
		return;
	}

	// for right-click scene
	if(!pObjOver)
	{
		int iSlotStart = 0;	
		while(!pObjOver)
		{
			AString strName;
			PAUIOBJECT pObj =  GetDlgItem(strName.Format("Item_%d", ++iSlotStart));
			if(!pObj) break;
			
			if(pObj->IsShow() && pGoblinEqu->CanEquippedTo(iSlotStart - 1))
			{
				// find a valid one
				pObjOver = pObj;
			}
		}
	}
	
	if( pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		int iDst = atoi(pObjOver->GetName() + strlen("Item_")) - 1;
		if( pGoblinEqu->CanEquippedTo(iDst))			
		{
			//物品无法取下，每次都要提醒
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_GoblinEquip", GetStringFromTable(7154), 
				MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(iSrc);
			pMsgBox->SetDataPtr((void*)iDst);
		}
	}
}