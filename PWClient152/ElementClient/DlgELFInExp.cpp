// Filename	: DlgELFInExp.cpp
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#include "WindowsX.h"
#include "AF.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFInExp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "elementdataman.h"
#include "AUICTranslate.h"

#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUIStillImageButton.h"
#include "AUIRadioButton.h"
#include "EC_Goblin.h"
#include "EC_IvtrGoblin.h"
#include "EC_ShortcutMgr.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	100
#define INTERAL_SHORT	40
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFInExp, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_OK)
AUI_ON_COMMAND("reset",			OnCommand_Reset)
AUI_ON_COMMAND("QuickSet",		OnCommand_QuickSet)
AUI_ON_COMMAND("HostInject",	OnCommand_HostInject)
AUI_ON_COMMAND("PillInject",	OnCommand_PillInject)
AUI_ON_COMMAND("SPInject",		OnCommand_SPInject)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFInExp, CDlgBase)

AUI_ON_EVENT("Btn_POWER",	WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Btn_POWER",	WM_LBUTTONDBLCLK,	OnEventLButtonDown)
AUI_ON_EVENT("Btn_POWER",	WM_LBUTTONUP,		OnEventLButtonUp)
AUI_ON_EVENT("Img_Pill",	WM_LBUTTONDOWN,		OnEventRemovePill)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFInExp
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFInExp> ELFInExpClickShortcut;
//------------------------------------------------------------------------

CDlgELFInExp::CDlgELFInExp()
{
	m_iBase = 0;
	m_iBasePill = 0;
	m_iGoblinBase = 0;
	m_iGoblinBasePill = 0;
	m_iBaseSP = 0;
	m_iGoblinBaseSP = 0;
	m_dwLastTime = 0;
	m_iInjectSrc = 1;
	m_pButton = NULL;
	m_dwFreezeTime = 0;
	m_fRange = 0.0f;
	m_iSlot = -1;
	m_bCanInject = false;
}

CDlgELFInExp::~CDlgELFInExp()
{
}

bool CDlgELFInExp::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFInExpClickShortcut(this));

	m_pPgs_Inject = (PAUIPROGRESS)GetDlgItem("Pgs_Inject");
	m_pTxt_CurrenExp = (PAUILABEL)GetDlgItem("Txt_CHAREXP");
	m_pTxt_Level = (PAUILABEL)GetDlgItem("Txt_LEVEL");
	m_pImg_ELF = (PAUIIMAGEPICTURE)GetDlgItem("Img_Pill");
	((PAUIRADIOBUTTON)GetDlgItem("Chk_PillInject"))->Check(true);
	OnCommand_PillInject(NULL);
	return true;
}

void CDlgELFInExp::OnTick(void)
{
	if (m_dwFreezeTime > 0)
	{
		if (CECTimeSafeChecker::ElapsedTimeFor(m_dwFreezeTime) > 500)
		{
			m_dwFreezeTime = 0;
		}		
	}
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		return;
	}
	GOBLIN_ESSENCE* pEssence = (GOBLIN_ESSENCE *)pHostGoblin->GetDBEssence();
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	if (-1 == pHostGoblin->GetSecurityState())
	{
		if (2 != m_iInjectSrc)
		{
			CheckRadioButton(1, 2);
			OnCommand_PillInject(NULL);
		}
		((PAUIRADIOBUTTON)GetDlgItem("Chk_HostInject"))->Enable(false);
		((PAUIRADIOBUTTON)GetDlgItem("Chk_SP"))->Enable(false);
	}
	else
	{
		((PAUIRADIOBUTTON)GetDlgItem("Chk_HostInject"))->Enable(true);
		((PAUIRADIOBUTTON)GetDlgItem("Chk_SP"))->Enable(true);
	}
	m_bCanInject = true;
	if (m_pButton && AUISTILLIMAGEBUTTON_STATE_NORMAL != m_pButton->GetState())		
	{
		if (CECTimeSafeChecker::ElapsedTimeFor(m_dwLastTime) >= INTERAL_LONG)
		{
			
			unsigned int iGoblinLevelUpExp = (unsigned int)(GetHostPlayer()->GetLevelUpExp(pHostGoblin->GetLevel()) * pEssence->exp_factor);
			float step;
			if (iGoblinLevelUpExp < 100)
			{
				step = (float)1.0/iGoblinLevelUpExp;
			}
			else
				step = 0.01f;
			m_fRange += step;
			if (m_iInjectSrc == 1)
			{
				if (pHostGoblin->GetLevel() >= GetHostPlayer()->GetMaxLevelSofar())
				{
					float BaseRange = (float)(pHostGoblin->GetExp())/iGoblinLevelUpExp;
					if (BaseRange + m_fRange >= 0.99)
					{
						m_fRange = 1.0f - step - BaseRange;
					}
					a_Clamp(m_fRange, 0.0f, 1.0f);
				}
				m_iBase = pHostGoblin->NeedPlayerExp((unsigned int)(iGoblinLevelUpExp*m_fRange));
				if (m_iBase > (unsigned int)rbp.iExp)
				{
					m_fRange -= step;
					m_iBase = rbp.iExp;
					m_pButton = NULL;
					GetGameUIMan()->MessageBox("", GetStringFromTable(7181), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					return;
				}
			}
			else if (m_iInjectSrc == 2)
			{
				if (m_iSlot >=0 && NULL == GetHostPlayer()->GetPack()->GetItem(m_iSlot))
				{

					m_iSlot = -1;
					m_iBasePill = 0;
					m_iGoblinBasePill = 0;
				}	
				else if (GetValidExpPill())
				{
					CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
					unsigned int iPillExp = pExpPill->GetExp();
					float BaseRange = (float)(pHostGoblin->GetExp())/iGoblinLevelUpExp;
// 					if (pExpPill->GetLevel() <= pHostGoblin->GetLevel())
// 					{
// 						if (BaseRange + m_fRange >= 0.99)
// 						{
// 							m_fRange = 1.0f - step - BaseRange;
// 						}
// 						a_Clamp(m_fRange, 0.0f, 1.0f);
// 					}
					m_iBasePill = pHostGoblin->NeedPillExp((unsigned int)(iGoblinLevelUpExp*m_fRange), pExpPill->GetLevel());
					if (m_iBasePill > iPillExp)
					{
						m_fRange -= step;
						m_iBasePill = iPillExp;
						m_pButton = NULL;
						GetGameUIMan()->MessageBox("", GetStringFromTable(7181), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
						return;
					}
				}
			}
			else if (m_iInjectSrc == 3)
			{
				if (pHostGoblin->GetLevel() >= GetHostPlayer()->GetMaxLevelSofar())
				{
					float BaseRange = (float)(pHostGoblin->GetExp())/iGoblinLevelUpExp;
					if (BaseRange + m_fRange >= 0.99)
					{
						m_fRange = 1.0f - step - BaseRange;
					}
					a_Clamp(m_fRange, 0.0f, 1.0f);
				}
				m_iBaseSP = pHostGoblin->NeedPlayerExp((unsigned int)(iGoblinLevelUpExp*m_fRange))/5;
				if (m_iBaseSP > (unsigned int)rbp.iSP)
				{
					m_fRange -= step;
					m_iBaseSP = rbp.iSP;
					m_pButton = NULL;
					GetGameUIMan()->MessageBox("", GetStringFromTable(7181), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					return;
				}
			}
			m_iGoblinBase = pHostGoblin->InjectExp(m_iBase);
			m_iGoblinBaseSP = pHostGoblin->InjectExp(m_iBaseSP*5);
			CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
			if (pExpPill)
			{
				m_iGoblinBasePill =  pHostGoblin->InjectExp(m_iBasePill, pExpPill->GetLevel());
			}
			else
				m_iGoblinBasePill = 0;
			if (m_iGoblinBase + m_iGoblinBasePill + m_iGoblinBaseSP + pHostGoblin->GetExp() >= iGoblinLevelUpExp)
			{
				if (m_iInjectSrc == 2)
				{
					CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
					m_iGoblinBasePill = iGoblinLevelUpExp >= pHostGoblin->GetExp()? iGoblinLevelUpExp - pHostGoblin->GetExp():0;
					int level = 100;
					if (pExpPill)
					{
						level = pExpPill->GetLevel();
					}
					m_iBasePill = pHostGoblin->NeedPillExp(m_iGoblinBasePill, level);
				}
				else if (m_iInjectSrc == 1)
				{
					m_iGoblinBase = iGoblinLevelUpExp >= pHostGoblin->GetExp()? iGoblinLevelUpExp - pHostGoblin->GetExp():0;
					m_iBase = pHostGoblin->NeedPlayerExp(m_iGoblinBase);
				}
				else if (m_iInjectSrc == 3)
				{
					m_iGoblinBaseSP = iGoblinLevelUpExp >= pHostGoblin->GetExp()? iGoblinLevelUpExp - pHostGoblin->GetExp():0;
					m_iBaseSP = pHostGoblin->NeedPlayerExp(m_iGoblinBaseSP)/5+1;
				}
				m_pButton = NULL;
				return;
			}
			m_dwLastTime += INTERAL_LONG;
		}
	}
}

bool CDlgELFInExp::Render(void)
{
	if (m_dwFreezeTime == 0)
	{
		RefreshHostDetails();
	}
	return CDlgBase::Render();
}

void CDlgELFInExp::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	ACHAR szText[200];
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		OnCommand_CANCEL("IDCANCEL");
		return;
	}
	GOBLIN_ESSENCE* pEssence = (GOBLIN_ESSENCE *)pHostGoblin->GetDBEssence();
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	unsigned int iGoblinLevelUpExp = (unsigned int)(GetHostPlayer()->GetLevelUpExp(pHostGoblin->GetLevel()) * pEssence->exp_factor);
	int iLevelForInExp = GetHostPlayer()->GetMaxLevelSofar();
	a_Clamp(iLevelForInExp, 0, 150);
	if (m_iInjectSrc == 1)
	{
		a_sprintf(szText, _AL("%u"), (unsigned int)rbp.iExp >= m_iBase? (unsigned int)rbp.iExp - m_iBase:0);
		m_pTxt_CurrenExp->SetText(szText);
		GetDlgItem("Lab_ItemLevel")->Show(false);
		GetDlgItem("Txt_ItemLevel")->Show(false);
		GetDlgItem("Lab_HostLevel")->Show(true);
		a_sprintf(szText, _AL("%d"), iLevelForInExp);
		GetDlgItem("Txt_HostLevel")->Show(true);
		GetDlgItem("Txt_HostLevel")->SetText(szText);
		a_sprintf(szText, _AL("%u"), m_iBase);
		GetDlgItem("Txt_ExpIn")->SetText(szText);
		m_iGoblinBase = pHostGoblin->InjectExp(m_iBase);
		a_sprintf(szText, _AL("%u"), m_iGoblinBase);
		GetDlgItem("Txt_ExpOut")->SetText(szText);
		
		float factor = (float)CECIvtrGoblin::elf_exp_loss_constant[pHostGoblin->GetLevel()]/(float)CECIvtrGoblin::elf_exp_loss_constant[iLevelForInExp];
		a_Clamp(factor, 0.1f, 1.0f);
		a_sprintf(szText, _AL("%.01f%%"),factor*100);
		GetDlgItem("Txt_Ratio")->SetText(szText);
	}
	else if (m_iInjectSrc == 2)
	{
		GetDlgItem("Lab_ItemLevel")->Show(true);
		GetDlgItem("Lab_HostLevel")->Show(false);
		GetDlgItem("Txt_HostLevel")->Show(false);
		a_sprintf(szText, _AL("%u"), m_iBasePill);
		GetDlgItem("Txt_ExpIn")->SetText(szText);
		CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
		if (pExpPill)
		{
			float ratio = 0.0f;
			
			unsigned int iPillExp = pExpPill->GetExp();
			a_sprintf(szText, _AL("%u"), iPillExp - m_iBasePill? iPillExp - m_iBasePill:0);
			m_pTxt_CurrenExp->SetText(szText);
			ratio = (float)CECIvtrGoblin::elf_exp_loss_constant[pHostGoblin->GetLevel()]/(float)CECIvtrGoblin::elf_exp_loss_constant[pExpPill->GetLevel()];
			m_iGoblinBasePill =  pHostGoblin->InjectExp(m_iBasePill, pExpPill->GetLevel());
			a_sprintf(szText, _AL("%u"), m_iGoblinBasePill);
			GetDlgItem("Txt_ExpOut")->SetText(szText);
			
			a_Clamp(ratio, 0.1f, 1.0f);
			a_sprintf(szText, _AL("%.01f%%"), ratio*100);
			GetDlgItem("Txt_Ratio")->SetText(szText);
			a_sprintf(szText, _AL("%d"), pExpPill->GetLevel());
			GetDlgItem("Txt_ItemLevel")->SetText(szText);
			GetDlgItem("Txt_ItemLevel")->Show(true);
		}
		else
		{
			m_pTxt_CurrenExp->SetText(_AL("0"));
			m_pImg_ELF->ClearCover();
			m_pImg_ELF->SetHint(_AL(""));
			m_iSlot = -1;
			GetDlgItem("Txt_Ratio")->SetText(_AL(""));
			GetDlgItem("Txt_ItemLevel")->Show(false);
			GetDlgItem("Txt_ExpOut")->SetText(_AL("0"));
		}
	}
	else if (m_iInjectSrc == 3)
	{
		a_sprintf(szText, _AL("%u"), (unsigned int)rbp.iSP >= m_iBaseSP? (unsigned int)rbp.iSP - m_iBaseSP:0);
		m_pTxt_CurrenExp->SetText(szText);
		GetDlgItem("Lab_ItemLevel")->Show(false);
		GetDlgItem("Txt_ItemLevel")->Show(false);
		GetDlgItem("Lab_HostLevel")->Show(true);
		a_sprintf(szText, _AL("%d"), iLevelForInExp);
		GetDlgItem("Txt_HostLevel")->Show(true);
		GetDlgItem("Txt_HostLevel")->SetText(szText);
		a_sprintf(szText, _AL("%u"), m_iBaseSP);
		GetDlgItem("Txt_ExpIn")->SetText(szText);
		m_iGoblinBaseSP = pHostGoblin->InjectExp(m_iBaseSP*5);
		a_sprintf(szText, _AL("%u"), m_iGoblinBaseSP);
		GetDlgItem("Txt_ExpOut")->SetText(szText);
		float factor = (float)CECIvtrGoblin::elf_exp_loss_constant[pHostGoblin->GetLevel()]/(float)CECIvtrGoblin::elf_exp_loss_constant[iLevelForInExp];
		a_Clamp(factor, 0.1f, 1.0f);
		a_sprintf(szText, _AL("%.01f%%"),factor*100*5);
		GetDlgItem("Txt_Ratio")->SetText(szText);
	}
	
	a_sprintf(szText, _AL("%d"), pHostGoblin->GetLevel());
	m_pTxt_Level->SetText(szText);
	/*float fPercent = (float)(pHostGoblin->GetExp() + m_iGoblinBase + m_iGoblinBasePill)/iGoblinLevelUpExp;*/
	float fPercent = (float)(pHostGoblin->GetExp())/iGoblinLevelUpExp;
	m_pPgs_Inject->SetProgress((int)((fPercent + m_fRange)*100));
}

void CDlgELFInExp::OnCommand_OK(const char * szCommand)
{
	if (!m_bCanInject)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7182), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_iBasePill > 0)
	{
		CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
		GetGameSession()->c2s_CmdUseItemWithArg(0, 1, m_iSlot, pExpPill->GetTemplateID(), (const void *)&m_iBasePill, 4);
	}
	else if (m_iBase > 0)
	{
		GetGameSession()->c2s_CmdGoblinPlayerInsertExp(m_iBase);
	}
	else if (m_iBaseSP > 0)
	{
		if (m_iBaseSP >= 840000000)
		{
			m_iBaseSP = 840000000;
		}
		GetGameSession()->c2s_CmdGoblinPlayerInsertExp(m_iBaseSP, 1);
	}
	
	OnCommand_Reset("");
	m_dwFreezeTime = GetTickCount();
}

void CDlgELFInExp::OnCommand_CANCEL(const char * szCommand)
{
	CECIvtrItem *pELF = GetValidExpPill();
	if( pELF )
		pELF->Freeze(false);
	OnEventRemovePill(0, 0, NULL);
	OnCommand_Reset("");
	m_dwFreezeTime = 0;
	Show(false);
}
void CDlgELFInExp::OnCommand_Reset(const char * szCommand)
{
	m_iBase = 0;
	m_iBasePill = 0;
	m_iGoblinBase = 0;
	m_iGoblinBasePill = 0;
	m_iBaseSP = 0;
	m_iGoblinBaseSP = 0;
	m_fRange = 0.0f;
}
void CDlgELFInExp::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!m_bCanInject)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7182), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_iInjectSrc ==2 && NULL == GetValidExpPill())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7180), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	m_pButton = pObj;
	m_dwLastTime = GetTickCount();
}

void CDlgELFInExp::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_pButton = NULL;
}

void CDlgELFInExp::OnCommand_HostInject(const char * szCommand)
{
	m_iInjectSrc = 1;
	m_iBasePill = 0;
	m_iGoblinBasePill = 0;
	m_iBaseSP = 0;
	m_iGoblinBaseSP = 0;
	m_fRange = 0.0f;
	if (GetValidExpPill())
	{
		OnEventRemovePill(0, 0, NULL);
	}
}

void CDlgELFInExp::OnCommand_PillInject(const char * szCommand)
{
	m_iInjectSrc = 2;
	m_iBase = 0;
	m_iGoblinBase = 0;
	m_iBaseSP = 0;
	m_iGoblinBaseSP = 0;
	m_fRange = 0.0f;
}

void CDlgELFInExp::OnCommand_SPInject(const char * szCommand)
{
	m_iInjectSrc = 3;
	m_iBase = 0;
	m_iGoblinBase = 0;
	m_iBasePill = 0;
	m_iGoblinBasePill = 0;
	m_fRange = 0.0f;
	if (GetValidExpPill())
	{
		OnEventRemovePill(0, 0, NULL);
	}
}

void CDlgELFInExp::OnEventRemovePill(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pELF = GetValidExpPill();
	if( pELF )
	{
		pELF->Freeze(false);
		m_pImg_ELF->ClearCover();
	}
	m_iSlot = -1;
	m_iBasePill = 0;
	m_iGoblinBasePill = 0;
	m_fRange = 0.0f;
}

void CDlgELFInExp::SetItem(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrItem* pItem = pItem1;
	OnEventRemovePill(0, 0, NULL);
	m_iSlot = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_ELF->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	m_iBasePill = 0;
	m_iGoblinBasePill = 0;
	m_fRange = 0.0f;
}

void CDlgELFInExp::OnCommand_QuickSet(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		OnCommand_CANCEL("IDCANCEL");
		return;
	}
	GOBLIN_ESSENCE* pEssence = (GOBLIN_ESSENCE *)pHostGoblin->GetDBEssence();
	const ROLEBASICPROP &rbp = GetHostPlayer()->GetBasicProps();
	unsigned int iGoblinLevelUpExp = (unsigned int)(GetHostPlayer()->GetLevelUpExp(pHostGoblin->GetLevel()) * pEssence->exp_factor);
	unsigned int neededGoblinExp = iGoblinLevelUpExp - pHostGoblin->GetExp();
	if (m_iInjectSrc == 1)
	{
		unsigned int neededHostExp = pHostGoblin->NeedPlayerExp(neededGoblinExp);
		unsigned int hostExp = rbp.iExp;
		m_iBase = min(hostExp, neededHostExp);
		m_iBasePill = 0;
		m_iBaseSP = 0;
	}
	else if (m_iInjectSrc == 2)
	{
		if (GetValidExpPill())
		{
			CECIvtrGoblinExpPill* pExpPill = GetValidExpPill();
			unsigned int neededPillExp = pHostGoblin->NeedPillExp(neededGoblinExp, pExpPill->GetLevel());
			unsigned int PillExp = pExpPill->GetExp();
			m_iBasePill = min(PillExp, neededPillExp);
			m_iBase = 0;
			m_iBaseSP = 0;
		}
		else
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(7180), MB_OK, A3DCOLORRGB(255, 255, 255), NULL);
		}
	}
	else if (m_iInjectSrc == 3)
	{
		unsigned int neededHostExp = pHostGoblin->NeedPlayerExp(neededGoblinExp);
		unsigned int neededHostSP = neededHostExp/5 + (neededHostExp%5 ? 1 : 0);
		unsigned int hostSP = rbp.iSP;
		m_iBaseSP = min(hostSP, neededHostSP);
		m_iBase = 0;
		m_iBasePill = 0;
	}
	OnCommand_OK(NULL);
}

bool CDlgELFInExp::OnChangeLayout(PAUIOBJECT lhs, PAUIOBJECT rhs)
{
	if(lhs && rhs && lhs->GetType() == AUIOBJECT_TYPE_LABEL)
	{
		// force swap this property in this dialog
		AUIOBJECT_SETPROPERTY lhsProp;
		AUIOBJECT_SETPROPERTY rhsProp;
		
		AUI_SWAP_RES("Text Color");
	}

	return true;
}

CECIvtrGoblinExpPill * CDlgELFInExp::GetValidExpPill()
{
	//	经验丸使用完后，服务器会删除相应物品，因此，拖拽物品时、不能直接保存物品指针
	//
	CECIvtrGoblinExpPill *pRet = NULL;
	if (m_iSlot >=0)
	{
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_iSlot);
		if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_GOBLIN_EXPPILL)
			pRet = (CECIvtrGoblinExpPill *)pItem;
	}
	return pRet;
}

void CDlgELFInExp::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Pill", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN_EXPPILL )
		this->SetItem(pIvtrSrc, iSrc);
}