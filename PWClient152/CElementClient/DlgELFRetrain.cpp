// File		: DlgELFRetrain.cpp
// Creator	: Chen Zhixin
// Date		: 2008/11/18

#include "AFI.h"
#include "DlgELFRetrain.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "EC_Skill.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_IvtrGoblin.h"
#include "AUICTranslate.h"
#include "ElementSkill.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgELFRetrain, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFRetrain, CDlgBase)

AUI_ON_EVENT("Img_Skill*",	WM_LBUTTONDOWN,	OnEventLButtonDownSkill)
AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN, OnEventLButtonDown_ELF)
AUI_ON_EVENT("Txt_Number",	WM_CHAR,		OnEventInput)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFRetrain
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFRetrain> ELFRetrainClickShortcut;
//------------------------------------------------------------------------

CDlgELFRetrain::CDlgELFRetrain()
{
	m_nRetrainLevel = 0;
	for (int i =0; i < 8; i++)
	{
		m_pSkill[i] = NULL;
	}
}

CDlgELFRetrain::~CDlgELFRetrain()
{
}

bool CDlgELFRetrain::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFRetrainClickShortcut(this));
	m_pTxt_ELFName = (PAUILABEL)GetDlgItem("Txt_ELFName");
	m_pTxt_ELFLevel = (PAUILABEL)GetDlgItem("Txt_ELFLevel");
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_ELF");
	m_pTxt_Desc = (PAUILABEL)GetDlgItem("Txt_SkillDesc");
	m_pTxt_SkillName = (PAUILABEL)GetDlgItem("Txt_SkillName");
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");
	int i;
	for(i = 0; i < 8; i++ )
	{
		char szText[20];
		sprintf(szText, "Img_Skill%d", i + 1);
		m_pImg_Skill[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Txt_Skill%d", i + 1);
		m_pTxt_Skill[i] = (PAUILABEL)GetDlgItem(szText);
	}
	for (i = 0; i < 5; i++)
	{
		char szName[20];
		sprintf(szName, "Img_%d", i);
		m_pImg_Genius[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	}
	Reset();
	return true;
}

void CDlgELFRetrain::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	DDX_EditBox(bSave, "Txt_Number", m_nRetrainLevel);
}

void CDlgELFRetrain::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);
	if (NULL == m_pImg_Item->GetDataPtr())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7121), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (0 == m_nSkillID || m_nRetrainLevel <= 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7122), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	ACString str;
	PAUIDIALOG pMsgBox;
	str.Format(GetStringFromTable(7123),g_pGame->GetSkillDesc()->GetWideString(m_nSkillID * 10), m_nRetrainLevel);
	GetGameUIMan()->MessageBox("Game_ELFRetrain", str, MB_YESNO, 
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	DWORD data = m_nSlot;
	data <<= 16;
	data |= m_nRetrainLevel;
	pMsgBox->SetData(data);
	pMsgBox->SetDataPtr((void *)m_nSkillID);
//	GetGameSession()->c2s_CmdNPCSevGoblinForgetSkill(m_nSlot, m_nSkillID, m_nRetrainLevel);
	m_nRetrainLevel = 0;
	m_nSelect = -1;
	m_nSkillID = 0;
	UpdateData(false);
	RefreshHostDetails();
}

void CDlgELFRetrain::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	m_nSlot = -1;
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
}

void CDlgELFRetrain::OnShowDialog()
{
	m_nSkillID = 0;
	m_nRetrainLevel = 0;
	m_nSelect = -1;
	Reset();
	UpdateData(false);
}

void CDlgELFRetrain::OnEventLButtonDownSkill(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
// 	GNET::SkillStr skillstr;
// 	ACHAR szDesc[200];
	CECIvtrGoblin* pGoblin = (CECIvtrGoblin *)m_pImg_Item->GetDataPtr();
	if (pGoblin)
	{
		int index = atoi(pObj->GetName() + strlen("Img_Skill")) - 1;
		GOBLINSKILL GoblinSkill = pGoblin->GetSkill(index);
		m_pTxt_SkillName->SetText(m_pTxt_Skill[index]->GetText());
//		GNET::ElementSkill::GetIntroduction(GoblinSkill.skill, GoblinSkill.level, szDesc, 200, skillstr);
		m_pTxt_Desc->SetText(_AL(""));
		m_nMaxLevel = GoblinSkill.level;
		m_nSkillID = GoblinSkill.skill;
		m_nSelect = index;
		m_nRetrainLevel = m_nMaxLevel;
		UpdateData(false);
	}
	RefreshHostDetails();
}

void CDlgELFRetrain::OnEventInput(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	OnTick();
// 	GNET::SkillStr skillstr;
// 	ACHAR szDesc[200];
	CECIvtrGoblin* pGoblin = (CECIvtrGoblin *)m_pImg_Item->GetDataPtr();
	if (pGoblin)
	{
		int index = m_nSelect;
		GOBLINSKILL GoblinSkill = pGoblin->GetSkill(index);
		//GNET::ElementSkill::GetIntroduction(GoblinSkill.skill, GoblinSkill.level - m_nRetrainLevel, szDesc, 200, skillstr);
		m_pSkill[index]->SetLevel(GoblinSkill.level - m_nRetrainLevel);
		m_pTxt_Desc->SetText(m_pSkill[index]->GetDesc());
		m_pSkill[index]->SetLevel(GoblinSkill.level);
	}
}

void CDlgELFRetrain::OnTick()
{
	//¶ÁÈëÊý¾Ý
	UpdateData(true);

	CECIvtrGoblin* pELF = (CECIvtrGoblin *)m_pImg_Item->GetDataPtr();
	if (pELF)
	{
		if (m_nRetrainLevel > m_nMaxLevel)
		{
			m_nRetrainLevel = m_nMaxLevel;
		}
		bool bLast = true;
		int skillnum = pELF->GetSkillNum();
		bool bFind = false;
		int i(0);
		for (i = 0; i < skillnum; i++)
		{
			GOBLINSKILL GolinSkill = pELF->GetSkill(i);
			if (GolinSkill.skill != m_pSkill[i]->GetSkillID())
			{
				if (!bFind)
				{
					delete m_pSkill[i];
					bFind = true;
				}
				m_pSkill[i] = m_pSkill[i+1];
				bLast = false;
			}
			m_pSkill[i]->SetLevel(GolinSkill.level);
		}
		if (bLast == false)
		{
			m_pSkill[i] = NULL;
		}
		else if (bLast && skillnum < 8 && m_pSkill[skillnum] != NULL)
		{
			delete m_pSkill[skillnum];
			m_pSkill[skillnum] = NULL;
		}
		if (bFind)
		{
			RefreshHostDetails();
		}
	}
	UpdateData(false);
	if (m_nSelect < 0)
	{
		GetDlgItem("Txt_Number")->Enable(false);
	}
	else
	{
		GetDlgItem("Txt_Number")->Enable(true);
	}
}

bool CDlgELFRetrain::Render(void)
{
	
//	RefreshHostDetails();
	
	return CDlgBase::Render();
}

void CDlgELFRetrain::RefreshHostDetails()
{
	if( !IsShow()) return;
	
/*	GNET::SkillStr skillstr;*/
	CECIvtrGoblin* pGoblin = (CECIvtrGoblin *)m_pImg_Item->GetDataPtr();
	if( pGoblin )
	{
		AUICTranslate trans;
		const ACHAR * szDesc = pGoblin->GetDesc();
		if( szDesc )
			m_pImg_Item->SetHint(trans.Translate(szDesc));
		else
		m_pImg_Item->SetHint(_AL(""));
		int i(0);
		for(i = 0; i < pGoblin->GetSkillNum(); i++ )
		{
			GOBLINSKILL GoblinSkill = pGoblin->GetSkill(i);
			m_pImg_Skill[i]->Show(true);
			m_pTxt_Skill[i]->Show(true);
			m_pImg_Skill[i]->ClearCover();
			SetImage(m_pImg_Skill[i], GoblinSkill.skill);
			if (m_nSelect == i)
			{
				m_pImg_Skill[i]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
			}
			else
			{
				m_pImg_Skill[i]->SetCover(NULL, -1, 1);
			}

			ACString strName;
			strName.Format(_AL("%s%d"), g_pGame->GetSkillDesc()->GetWideString(m_pSkill[i]->GetSkillID() * 10), GoblinSkill.level);
			m_pTxt_Skill[i]->SetText(strName);
			//a_sprintf(szName, _AL("%s"), m_pSkill[i]->GetDesc());
			m_pImg_Skill[i]->SetHint(m_pSkill[i]->GetDesc());
		}
		if (m_nSelect == -1)
		{
			m_pTxt_Desc->SetText(_AL(""));
			m_pTxt_SkillName->SetText(_AL(""));
		}
		for (; i < 8; i++)
		{
			SetImage(m_pImg_Skill[i], NULL);
			m_pImg_Skill[i]->Show(false);
			m_pTxt_Skill[i]->Show(false);
		}
		if (m_nSelect >= 0)
		{
			m_pTxt_SkillName->SetText(m_pTxt_Skill[m_nSelect]->GetText());
		}
	}
}
void CDlgELFRetrain::SetELF(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pImg_Item->SetDataPtr(pELF);
	m_nSlot = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_Item->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Item->SetHint(_AL(""));
	m_pTxt_ELFName->SetText(pELF->GetName());
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), pELF->GetEssence().data.level);
	m_pTxt_ELFLevel->SetText(szText);
	int i;
	for (i = 0; i < 5; i++)
	{
		m_pImg_Genius[i]->FixFrame(pELF->GetGenius(i));
	}
	for(i = 0; i < pELF->GetSkillNum(); i++ )
	{
		CECSkill *pSkill = new CECSkill(pELF->GetSkill(i).skill, pELF->GetSkill(i).level);
		m_pSkill[i] = pSkill;
	}
	RefreshHostDetails();
}

void CDlgELFRetrain::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrGoblin *pELF = (CECIvtrGoblin *)m_pImg_Item->GetDataPtr();
	if( pELF )
		pELF->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(GetStringFromTable(7305));
	m_pImg_Item->SetDataPtr(NULL);
	Reset();
	m_nSlot = -1;
	RefreshHostDetails();
}

void CDlgELFRetrain::SetImage(AUIImagePicture *pImage, int id)
{
	ASSERT(pImage);
	
	if (id)
	{
		AString strFile = GNET::ElementSkill::GetIcon(id);
		strFile.MakeLower();
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
		pImage->SetData(0);
	}
	else
	{
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);
		pImage->SetData(0);
		pImage->SetHint(_AL("")); 
	}
}

void CDlgELFRetrain::Reset()
{
	int i;
	for (i = 0; i < 8; i++)
	{
		SetImage(m_pImg_Skill[i], 0);
		m_pTxt_Skill[i]->SetText(_AL(""));
		m_pImg_Skill[i]->Show(false);
		m_pImg_Skill[i]->SetHint(_AL(""));
		if (m_pSkill[i])
		{
			delete m_pSkill[i];
			m_pSkill[i] = NULL;
		}
	}
	m_pTxt_Desc->SetText(_AL(""));
	m_pTxt_SkillName->SetText(_AL(""));
//	m_pBtn_Confirm->Enable(false);
	m_pTxt_ELFName->SetText(_AL(""));
	m_pTxt_ELFLevel->SetText(_AL(""));
	m_nRetrainLevel = 0;
	m_nSkillID = 0;
	m_nMaxLevel = 0;
	m_nSelect = -1;
	for (i = 0; i < 5; i++)
	{
		m_pImg_Genius[i]->FixFrame(0);
	}
	UpdateData(false);
}

void CDlgELFRetrain::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_ELF", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
		this->SetELF(pIvtrSrc, iSrc);
}