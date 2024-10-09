// Filename	: DlgQuickBar.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "AUICheckBox.h"
#include "AUIClockIcon.h"
#include "AUIImagePicture.h"
#include "DlgQuickBar.h"
#include "DlgOptimizeFunc.h"
#include "EC_GameUIMan.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_Skill.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_PetCorral.h"
#include "EC_HostGoblin.h"
#include "ElementSkill.h"
#include "EC_Global.h"
#include "EC_GameRun.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQuickBar, CDlgBase)

AUI_ON_COMMAND("add",			OnCommand_add)
AUI_ON_COMMAND("minus",			OnCommand_minus)
AUI_ON_COMMAND("openextra",		OnCommand_openextra)
AUI_ON_COMMAND("new",			OnCommand_new)
AUI_ON_COMMAND("switch",		OnCommand_switch)
AUI_ON_COMMAND("enableext",		OnCommand_enableext)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Chk_Lock",		OnCommand_lock)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQuickBar, CDlgBase)

AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)

AUI_END_EVENT_MAP()

int CDlgQuickBar::m_nCurPanel1 = 1;
int CDlgQuickBar::m_nCurPanel2 = 1;
bool CDlgQuickBar::m_bShowAll1 = false;
bool CDlgQuickBar::m_bShowAll2 = false;
int CDlgQuickBar::m_nDisplayPanels1 = 0;
int CDlgQuickBar::m_nDisplayPanels2 = 0;

CDlgQuickBar::CDlgQuickBar()
{
	m_bDelGoblinSkillSC = false;
	m_pChkLock = NULL;
}

CDlgQuickBar::~CDlgQuickBar()
{
	m_bShowAll1 = false;
	m_bShowAll2 = false;
	m_nDisplayPanels1 = 0;
	m_nDisplayPanels2 = 0;
}

bool CDlgQuickBar::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog()) {
		return false;
	}
	SetHintDelay(1000);

	// store all icons
	m_vecImgCell.clear();
	AString strName;
	for(int i=1; ;i++)
	{
		strName.Format("Item_%d", i);
		PAUIIMAGEPICTURE pCell = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if(!pCell) break;
		m_vecImgCell.push_back(pCell);
	}

	m_pChkLock = dynamic_cast<AUICheckBox*>(GetDlgItem("Chk_Lock"));
	return true;
}

void CDlgQuickBar::OnCommand_add(const char * szCommand)
{
	SwitchHotkeyPanel(1);
}

void CDlgQuickBar::OnCommand_minus(const char * szCommand)
{
	SwitchHotkeyPanel(-1);
}

void CDlgQuickBar::OnCommand_openextra(const char * szCommand)
{
	// Open all panels in current quick bar set
	//

	int size(0), num(0), index(-1);
	bool horizontal(true);
	bool show(false);
	ParseName(size, num, index, horizontal);

	if (size == SIZE_HOSTSCSET1)
	{
		m_nDisplayPanels1 = 0;
		show = m_bShowAll1 = !m_bShowAll1;
	}
	else
	{
		m_nDisplayPanels2 = 0;
		show = m_bShowAll2 = !m_bShowAll2;
	}
	
	PAUIDIALOG pDlgLast = this;
	for (int i = 0; i < num; ++ i)
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%d%cb_%d", size, horizontal?'H':'V', i+1);
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog(dlgName);
		if (horizontal)
		{
			pDlg->AlignTo(pDlgLast
				, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT
				, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP
				, 0, 1);
		}
		else
		{
			pDlg->AlignTo(pDlgLast
				, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT
				, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP
				, 1);
		}
		pDlg->Show(show);
		pDlg->SetCanMove(false);
		
		pDlgLast = pDlg;
	}
}

void CDlgQuickBar::OnCommand_new(const char * szCommand)
{
	// Open one more panel in current quick bar set
	//

	int size(0), num(0), index(-1);
	bool horizontal(true);
	int panel(0);
	ParseName(size, num, index, horizontal);

	if (size == SIZE_HOSTSCSET1)
	{
		m_bShowAll1 = false;
		m_nDisplayPanels1 = (m_nDisplayPanels1+1) % NUM_HOSTSCSETS1;
		panel = m_nDisplayPanels1;
	}
	else
	{
		m_bShowAll2 = false;
		m_nDisplayPanels2 = (m_nDisplayPanels2+1) % NUM_HOSTSCSETS2;
		panel = m_nDisplayPanels2;
	}
	
	PAUIDIALOG pDlgLast = this;
	for (int i = 0; i < num; ++ i)
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%d%cb_%d", size, horizontal?'H':'V', i+1);
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog(dlgName);

		pDlg->Show(false);

		if (i > 0 && i <= panel)
		{
			if (horizontal)
			{
				pDlg->AlignTo(pDlgLast
					, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT
					, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP
					, 0, 1);
			}
			else
			{
				pDlg->AlignTo(pDlgLast
					, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT
					, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP
					, 1);
			}
			pDlg->Show(true);
			pDlg->SetCanMove(false);

			pDlgLast = pDlg;
		}
	}
}

void CDlgQuickBar::OnCommand_switch(const char * szCommand)
{
	// Switch between horizontal and vertical
	//
	int size(0), num(0), index(-1);
	bool horizontal(true);
	ParseName(size, num, index, horizontal);

	if (size == SIZE_HOSTSCSET1)
	{
		m_nDisplayPanels1 = 0;
		m_bShowAll1 = false;
	}
	else
	{
		m_nDisplayPanels2 = 0;
		m_bShowAll2 = false;
	}

	// Hide current panels
	this->Show(false);
	for (int i = 0; i < num; ++ i)
	{
		AString dlgName;
		dlgName.Format("Win_Quickbar%d%cb_%d", size, horizontal?'H':'V', i+1);
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog(dlgName);
		pDlg->Show(false);
	}

	// Show in the other direction (horizontal or vertical)
	AString dlgOther;
	dlgOther.Format("Win_Quickbar%d%ca", size, horizontal?'V':'H');
	PAUIDIALOG pDlgOther = m_pAUIManager->GetDialog(dlgOther);
	pDlgOther->Show(true);
}

void CDlgQuickBar::OnCommand_enableext(const char * szCommand)
{
	// Switch Tab stop
	//

	int size(0), num(0), index(-1);
	bool horizontal(true);
	ParseName(size, num, index, horizontal);

	AString CheckName = "Chk_Normal";
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem(CheckName);
	AString dlgOther;
	dlgOther.Format("Win_Quickbar%d%cb_%d", size, horizontal?'V':'H', index);
	PAUICHECKBOX pCheckOther = (PAUICHECKBOX)m_pAUIManager->GetDialog(dlgOther)->GetDlgItem(CheckName);
	pCheckOther->Check(pCheck->IsChecked());
}

void CDlgQuickBar::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgQuickBar::OnCommand_lock(const char * szCommand) {
	bool locked = m_pChkLock->IsChecked();
	
	EC_GAME_SETTING setting = GetGame()->GetConfigs()->GetGameSettings();
	setting.bLockQuickBar = locked;
	GetGame()->GetConfigs()->SetGameSettings(setting);

	CDlgOptimizeFunc* pDlgOptimizeFunc = dynamic_cast<CDlgOptimizeFunc*>(GetGameUIMan()->GetDialog("Win_OptimizeFunc"));
	if (pDlgOptimizeFunc->IsShow()) {
		pDlgOptimizeFunc->SetLockQBar(locked);
	}
}

void CDlgQuickBar::OnTick() {
	if (m_pChkLock != NULL && m_pChkLock->IsChecked() != GetGame()->GetConfigs()->GetGameSettings().bLockQuickBar) {
		m_pChkLock->Check(GetGame()->GetConfigs()->GetGameSettings().bLockQuickBar);
	}
}

void CDlgQuickBar::SwitchHotkeyPanel(int nDelta)
{
	int size(0), num(0), index(-1);
	bool horizontal(true);
	ParseName(size, num, index, horizontal);

	int &curPanel = (size == SIZE_HOSTSCSET1 ? m_nCurPanel1 : m_nCurPanel2);
	int nCurrent = curPanel;
	for (int i = 0; i < num; ++ i)
	{
		nCurrent += nDelta;
		if (nCurrent > num)
			nCurrent = 1;
		else if (nCurrent < 1)
			nCurrent = num;

		AString dlgName;
		dlgName.Format("Win_Quickbar%d%cb_%d", size, horizontal?'H':'V', nCurrent);
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog(dlgName);
		PAUICHECKBOX pCheck = (PAUICHECKBOX)pDlg->GetDlgItem("Chk_Normal");
		if (pCheck->IsChecked())
		{
			curPanel = nCurrent;
			break;
		}
	}
}

int CDlgQuickBar::GetCurPanel1()
{
	return m_nCurPanel1;
}

int CDlgQuickBar::GetCurPanel2()
{
	return m_nCurPanel2;
}

void CDlgQuickBar::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	void *ptr;
	AString szType;
	pObj->ForceGetDataPtr(ptr,szType);
	if( !ptr ) return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}
void CDlgQuickBar::GetQuickBarNameAndSC(CECHostPlayer* pHost,abase::vector<AString>& pszPanel,abase::vector<CECShortcutSet *>* pSCS, int panel9,int panel8)
{
	AString dlgName;
	int i=0;
	dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET1);
	if(pSCS)
		pSCS->push_back(pHost->GetShortcutSet1(panel9-1));
	pszPanel.push_back(dlgName);
	for (i = 0; i < NUM_HOSTSCSETS1; ++ i)
	{
		dlgName.Format("Win_Quickbar%dHb_%d", SIZE_HOSTSCSET1, i+1);
		if(pSCS)
			pSCS->push_back(pHost->GetShortcutSet1(i));
		pszPanel.push_back(dlgName);
	}
	
	dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET1);
	if(pSCS)
		pSCS->push_back(pHost->GetShortcutSet1(panel9-1));
	pszPanel.push_back(dlgName);
	for (i = 0; i < NUM_HOSTSCSETS1; ++ i)
	{
		dlgName.Format("Win_Quickbar%dVb_%d", SIZE_HOSTSCSET1, i+1);
		if(pSCS)
			pSCS->push_back(pHost->GetShortcutSet1(i));
		pszPanel.push_back(dlgName);
	}
	
	dlgName.Format("Win_Quickbar%dHa", SIZE_HOSTSCSET2);
	if(pSCS)
		pSCS->push_back(pHost->GetShortcutSet2(panel8-1));
	pszPanel.push_back(dlgName);
	for (i = 0; i < NUM_HOSTSCSETS2; ++ i)
	{
		dlgName.Format("Win_Quickbar%dHb_%d", SIZE_HOSTSCSET2, i+1);
		if(pSCS)
			pSCS->push_back(pHost->GetShortcutSet2(i));
		pszPanel.push_back(dlgName);
	}
	
	dlgName.Format("Win_Quickbar%dVa", SIZE_HOSTSCSET2);
	if (pSCS)
		pSCS->push_back(pHost->GetShortcutSet2(panel8-1));
	pszPanel.push_back(dlgName);
	for (i = 0; i < NUM_HOSTSCSETS2; ++ i)
	{
		dlgName.Format("Win_Quickbar%dVb_%d", SIZE_HOSTSCSET2, i+1);
		if(pSCS)
			pSCS->push_back(pHost->GetShortcutSet2(i));
		pszPanel.push_back(dlgName);
	}
}
bool CDlgQuickBar::UpdateShortcuts()
{
	CECPetCorral* pPetCorral = GetHostPlayer()->GetPetCorral();
	AString strFile;
	CECSkill *pSkill;
	CECShortcut *pSC;
	CECIvtrItem *pItem;
	CECInventory *pIvtr;
	CECSCItem *pSCItem;
	CECSCSkill *pSCSkill;
	CECSCPet *pSCPet;
	AUIClockIcon *pClock;
	PAUIIMAGEPICTURE pCell;
	int i, j, iIconFile, nMax;
	PAUIDIALOG pDlg, pDlgLast;
	int nCurPanel9 = GetCurPanel1();
	int nCurPanel8 = GetCurPanel2();
	CECHostPlayer *pHost = GetHostPlayer();
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)pHost->GetGoblinModel();
	
	int fashionCoolTimeMax(0);
	int fashionCoolTime(0);

	// Construct dialog name and short cut in order
	abase::vector<CECShortcutSet *> a_pSCS;
	abase::vector<AString> a_pszPanel;

	CDlgQuickBar::GetQuickBarNameAndSC(pHost,a_pszPanel,&a_pSCS,nCurPanel9,nCurPanel8);

	int a_panelSetOffset[4] = {0};
	int a_panelSetSize[4] = {NUM_HOSTSCSETS1+1, NUM_HOSTSCSETS1+1, NUM_HOSTSCSETS2+1, NUM_HOSTSCSETS2+1};
	bool a_panelSetHorizontal[4] = {true, false, true, false};
	for (i = 1; i < 4; ++ i)
	{
		a_panelSetOffset[i] = a_panelSetOffset[i-1] + a_panelSetSize[i-1];
	}

	for( i = 0; i < (int)a_pSCS.size(); i++ )
	{
		if( !a_pSCS[i] ) continue;

		CDlgQuickBar* pQuickBar =dynamic_cast<CDlgQuickBar*>(GetGameUIMan()->GetDialog(a_pszPanel[i]));
		if( !pQuickBar || !pQuickBar->IsShow() ) continue;

		int num = pQuickBar->m_vecImgCell.size();
		for( j = 0; j < num; j++ )
		{
			pCell = pQuickBar->m_vecImgCell[j];

			pSC = a_pSCS[i]->GetShortcut(j);
			pClock = pCell->GetClockIcon();
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
			if( pSC )
			{
				if( pSC->GetType() == CECShortcut::SCT_SKILL )
				{
					iIconFile = CECGameUIMan::ICONS_SKILL;
					pSCSkill = (CECSCSkill *)pSC;
					pSkill = pSCSkill->GetSkill();
					if (m_bDelGoblinSkillSC && GNET::ElementSkill::IsGoblinSkill(pSkill->GetSkillID()))
					{
							a_pSCS[i]->SetShortcut(j, NULL);
							pSC = NULL;
					}
					else
					{
						if( pSkill && pSkill->ReadyToCast() && pHost->GetPrepSkill() != pSkill )
						{
							if (GNET::ElementSkill::IsGoblinSkill(pSkill->GetSkillID()))
							{
								if (pHostGoblin && !pHostGoblin->CheckSkillCastCondition(pSkill))
								{
									pCell->SetColor(A3DCOLORRGB(255, 255, 255));
								}
								else
								{
									pCell->SetColor(A3DCOLORRGB(128, 128, 128));
								}
							}
							else
							{
								if( !pHost->CheckSkillCastCondition(pSkill) )
									pCell->SetColor(A3DCOLORRGB(255, 255, 255));
								else
									pCell->SetColor(A3DCOLORRGB(128, 128, 128));
							}
						}
						else
							pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
						if( pSkill && (pSkill->GetCoolingTime() > 0 ||
							pHost->GetPrepSkill() == pSkill ))
						{
							pClock->SetProgressRange(0, pSkill->GetCoolingTime());
							if( pHost->GetPrepSkill() == pSkill )
								pClock->SetProgressPos(0);
							else
								pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
						}
					}
				}
				else if( pSC->GetType() == CECShortcut::SCT_ITEM )
				{
					iIconFile = CECGameUIMan::ICONS_INVENTORY;
					pCell->SetColor(A3DCOLORRGB(255, 255, 255));
					pSCItem = (CECSCItem *)pSC;
					pIvtr = GetHostPlayer()->GetPack(pSCItem->GetInventory());
					pItem = pIvtr->GetItem(pSCItem->GetIvtrSlot());
					if( pItem && pItem->GetCoolTime(&nMax) > 0 )
					{
						pClock->SetProgressRange(0, nMax);
						pClock->SetProgressPos(nMax - pItem->GetCoolTime());
						pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
					}
					if( pSCItem->GetInventory() == IVTRTYPE_EQUIPPACK )
						pCell->SetColor(A3DCOLORRGBA(128, 128, 255, 128));
				}
				else if( pSC->GetType() == CECShortcut::SCT_PET )
				{
					pSCPet = (CECSCPet*)pSC;
					CECPetData *pPet = pPetCorral->GetPetData(pSCPet->GetPetIndex());
					iIconFile = CECGameUIMan::ICONS_INVENTORY;
					pCell->SetColor(A3DCOLORRGB(255, 255, 255));
					if( pPet )
					{
						// dead combat pet
						if( (pPet->GetClass() == GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION) && pPet->GetHPFactor() == 0.0f )
						{
							pCell->SetColor(A3DCOLORRGB(128, 128, 128));
						}
						// current active pet
						else if(pSCPet->IsActivePet())
						{
							pCell->SetColor(A3DCOLORRGB(255, 255, 0));
						}
					}
				}
				else if (pSC->GetType() == CECShortcut::SCT_AUTOFASHION)
				{
					iIconFile = CECGameUIMan::ICONS_SUITE;
					fashionCoolTime = pHost->GetCoolTime(GP_CT_EQUIP_FASHION_ITEM, &fashionCoolTimeMax);
					pCell->SetColor(A3DCOLORRGB(255, 255, 255));
					if (fashionCoolTimeMax > 0)
					{
						pClock->SetProgressRange(0, fashionCoolTimeMax);
						pClock->SetProgressPos(fashionCoolTimeMax - fashionCoolTime);
						pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 175));
					}
				}
				else
				{
					iIconFile = CECGameUIMan::ICONS_ACTION;
					if (pSC->GetType() == CECShortcut::SCT_COMMAND)
					{
						CECSCCommand *pCommandSC = (CECSCCommand *)pSC;
						if (GetHostPlayer()->IsInvisible())
						{
							if (pCommandSC->GetCommandID() == CECSCCommand::CMD_STARTTRADE ||
								pCommandSC->GetCommandID() == CECSCCommand::CMD_SELLBOOTH ||
								pCommandSC->GetCommandID() == CECSCCommand::CMD_BINDBUDDY)
							{
								pCell->SetColor(A3DCOLORRGB(128, 128, 128));
							}
							else
							{
								pCell->SetColor(A3DCOLORRGB(255, 255, 255));
							}
						}
						else
						{
							pCell->SetColor(A3DCOLORRGB(255, 255, 255));
						}
					}

					if( pSC->GetCoolTime(&nMax) > 0 )
					{
						pClock->SetProgressRange(0, nMax);
						pClock->SetProgressPos(nMax - pSC->GetCoolTime());
						pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
					}
				}
				if (pSC)
				{
					pCell->SetDataPtr(pSC,"ptr_CECShortcut");
					if( pSC->GetType() == CECShortcut::SCT_SKILLGRP )
					{
						EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
						pCell->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 
							setting.comboSkill[((CECSCSkillGrp*)pSC)->GetGroupIndex()].nIcon + 1);
					}
					else
					{
						af_GetFileTitle(pSC->GetIconFile(), strFile);
						strFile.MakeLower();
						pCell->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[iIconFile],
							GetGameUIMan()->m_IconMap[iIconFile][strFile]);
					}
				}
			}
			else
			{
				pCell->SetCover(NULL, -1);
				pCell->SetText(_AL(""));
				pCell->SetDataPtr(NULL);
				pCell->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}
	}

	SIZE s;
	int nAlign;
	POINT ptPos;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	for (i = 0; i < sizeof(a_panelSetOffset) / sizeof(a_panelSetOffset[0]); ++ i)
	{
		int offset = a_panelSetOffset[i];
		pDlg = GetGameUIMan()->GetDialog(a_pszPanel[offset]);
		if( !pDlg->IsShow() ) continue;
		pDlgLast = pDlg;

		s = pDlg->GetSize();
		ptPos = pDlg->GetPos();
		if (a_panelSetHorizontal[i]) // Horizontal.
		{
			if (ptPos.y > s.cy * (a_panelSetSize[i]-1))
				nAlign = AUIDIALOG_ALIGN_TOP;
			else
				nAlign = AUIDIALOG_ALIGN_BOTTOM;
		}
		else						// Vertical.
		{
			if (ptPos.x > s.cx * (a_panelSetSize[i]-1))
				nAlign = AUIDIALOG_ALIGN_LEFT;
			else
				nAlign = AUIDIALOG_ALIGN_RIGHT;
		}

		int nStart;
		if (i < 2 && m_bShowAll1 || i >= 2 && m_bShowAll2 )
			nStart = 0;
		else
			nStart = 1;
		for (j = nStart; j < a_panelSetSize[i]-1; j++)
		{
			int offset = a_panelSetOffset[i]+j+1;
			pDlg = GetGameUIMan()->GetDialog(a_pszPanel[offset]);
			if( !pDlg->IsShow() ) continue;
			pDlg->SetCanMove(false);

			if( nAlign == AUIDIALOG_ALIGN_TOP )
			{
				pDlg->AlignTo(pDlgLast, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
					AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP, 0, 1);
			}
			else if( nAlign == AUIDIALOG_ALIGN_BOTTOM )
			{
				pDlg->AlignTo(pDlgLast, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
					AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM, 0, -1);
			}
			else if( nAlign == AUIDIALOG_ALIGN_LEFT )
			{
				pDlg->AlignTo(pDlgLast, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
					AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP, 1);
			}
			else if( nAlign == AUIDIALOG_ALIGN_RIGHT )
			{
				pDlg->AlignTo(pDlgLast, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT,
					AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM, -1);
			}
			pDlgLast = pDlg;
		}
	}

	return true;
}

void CDlgQuickBar::DelGoblinSkillSC()
{
	m_bDelGoblinSkillSC = true;
	UpdateShortcuts();
	m_bDelGoblinSkillSC = false;
}

void CDlgQuickBar::ParseName(int &size, int &num, int &index, bool &horizontal)
{
	// Get information with name from current quick bar instance
	// i.e. From Win_Quickbar9Hb_1, 9, H and 1 are drawn
	// Return: size should be SIZE_HOSTSCSET1 or SIZE_HOSTSCSET2
	//         num should be NUM_HOSTSCSETS1 or NUM_HOSTSCSETS2, depending on length
	//         index should be -1 or less than num

	const char * name = GetName();

	// Get short cut size and num
	int offset = strlen("Win_Quickbar");
	size = atoi(name + offset);
	num = (size == SIZE_HOSTSCSET1 ? NUM_HOSTSCSETS1 : NUM_HOSTSCSETS2);

	// Get quick bar direction : horizontal or vertical
	offset += 1;
	char dirChar = name[offset];
	horizontal = (dirChar == 'H');

	// Get quick bar index in current quick bar set
	offset += 1;
	char typeChar = name[offset];
	if (typeChar == 'a')
	{
		// the hot key panel
		index = -1;
	}
	else
	{
		// other panel
		offset += 2;
		index = atoi(name + offset);
	}
}
// quick bar ItemDataExtractor
class ItemDataExtractorQuickbar : public CECGameUIMan::ItemDataExtractor
{
public:
	virtual CECIvtrItem* GetItemPtr(PAUIOBJECT pObj)
	{
		CECShortcut *pSC = (CECShortcut *)pObj->GetDataPtr("ptr_CECShortcut");
		if( pSC && pSC->GetType() == CECShortcut::SCT_ITEM )
		{
			CECSCItem *pSCItem = (CECSCItem *)pSC;
			CECInventory* pIvtr = g_pGame->GetGameRun()->GetHostPlayer()->GetPack(pSCItem->GetInventory());
			return pIvtr->GetItem(pSCItem->GetIvtrSlot());
		}
		
		return NULL;
	}
};

bool CDlgQuickBar::Render()
{
	if(!CDlgBase::Render())
		return false;

	// render the cover for quickbar
	GetGameUIMan()->RenderItemCover((PAUIOBJECT*)m_vecImgCell.begin(), 
									m_vecImgCell.size(), 
									&ItemDataExtractorQuickbar());
	return true;
}