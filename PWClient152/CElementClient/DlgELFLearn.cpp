// File		: DlgELFLearn.cpp
// Creator	: Chen Zhixin
// Date		: 2008/12/01

#include "AFI.h"
#include "AUIImagePicture.h"
#include "A2DSprite.h"
#include "DlgELFLearn.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_NPCServer.h"
#include "EC_Skill.h"
#include "EC_World.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_IvtrGoblin.h"
#include "AUICTranslate.h"
#include "EC_GameSession.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgELFLearn, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("pageup",		OnCommandMovel)
AUI_ON_COMMAND("pagedown",		OnCommandMover)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgELFLearn, CDlgBase)

AUI_ON_EVENT("Skill_*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_ELF", WM_LBUTTONDOWN, OnEventLButtonDown_ELF)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFLearn
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFLearn> ELFLearnClickShortcut;
//------------------------------------------------------------------------

CDlgELFLearn::CDlgELFLearn()
		:m_nCurTeachPage(0)
{
	m_pTxt_ELFName = NULL;
	m_pImg_ELF = NULL;
	for(int i = 0; i < CDLGELFLEARN_PP; i ++)
	{
		m_pImg_SkillIcon[i] = NULL;
		m_pTxt_SkillName[i] = NULL;
		m_pTxt_SkillCostSP[i] = NULL;
		m_pTxt_Xuqiu[i] = NULL;
	}
}

CDlgELFLearn::~CDlgELFLearn()
{
}

bool CDlgELFLearn::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFLearnClickShortcut(this));

	return CDlgBase::OnInitDialog();
}

void CDlgELFLearn::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_ELFName", m_pTxt_ELFName);
	DDX_Control("Img_ELF", m_pImg_ELF);
	char szName[40];
	for(int i = 0; i < CDLGELFLEARN_PP; i ++)
	{
		sprintf(szName, "Skill_%d", i + 1);
		DDX_Control(szName, m_pImg_SkillIcon[i]);
		sprintf(szName, "Skill_name_%d", i + 1);
		DDX_Control(szName, m_pTxt_SkillName[i]);
		sprintf(szName, "Skill_SP_%d", i + 1);
		DDX_Control(szName, m_pTxt_SkillCostSP[i]);
		sprintf(szName, "Txt_Xuqiu%d", i + 1);
		DDX_Control(szName, m_pTxt_Xuqiu[i]);
	}
}

void CDlgELFLearn::OnShowDialog()
{
	Reset();
}

void CDlgELFLearn::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
	OnEventLButtonDown_ELF(0, 0, NULL);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
}

void CDlgELFLearn::OnCommandMovel(const char *szCommand)
{
	if( m_nCurTeachPage > 0 )
		UpdateTeach( m_nCurTeachPage - 1);
}

void CDlgELFLearn::OnCommandMover(const char *szCommand)
{
	int nNumSkills = (int)GetDataPtr("int");
	int nNumPages = (nNumSkills + CDLGELFLEARN_PP - 1) / CDLGELFLEARN_PP;

	if( m_nCurTeachPage < nNumPages - 1 )
		UpdateTeach( m_nCurTeachPage + 1);
}

void CDlgELFLearn::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetDataPtr("ptr_CECSkill") ) return;
	CECIvtrGoblin *pGoblin = (CECIvtrGoblin *)m_pImg_ELF->GetDataPtr();
	if (!pGoblin)
	{
		return;
	}

	PAUIDIALOG pMsgBox;

	CECSkill *pSkill = (CECSkill *)pObj->GetDataPtr("ptr_CECSkill");

	int nCondition;
	nCondition = pGoblin->CheckSkillLearnCondition(pSkill->GetSkillID(), true);
	if( nCondition == 0 || 10 == nCondition || 11 == nCondition || 12 == nCondition)
	{
		ACString str;
		if (0 == nCondition)
		{
			str = GetGameUIMan()->GetStringFromTable(231);
		}
		else if (10 == nCondition)
		{
			str = GetGameUIMan()->GetStringFromTable(7400);
		}
		else if (11 == nCondition)
		{
			str = GetGameUIMan()->GetStringFromTable(7401);
		}
		else if (12 == nCondition)
		{
			str = GetGameUIMan()->GetStringFromTable(7402);
		}

		GetGameUIMan()->MessageBox("Game_TeachGoblinSkill", str,
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		void *ptr;
		AString szType;
		pObj->ForceGetDataPtr(ptr,szType);
		pMsgBox->SetDataPtr(ptr,szType);
		pMsgBox->SetData(m_nSlot);
	}
	else if( 1 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(270), GP_CHAT_MISC);
	else if( 2 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(7172), GP_CHAT_MISC);
	else if( 4 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(7173), GP_CHAT_MISC);
	else if( 6 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(527), GP_CHAT_MISC);
	else if( 7 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(541), GP_CHAT_MISC);
	else if( 8 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(271), GP_CHAT_MISC);
	else if( 9 == nCondition )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(556), GP_CHAT_MISC);
}

bool CDlgELFLearn::UpdateTeach(int nPage)
{
	CECWorld *pWorld = GetWorld();
	CECHostPlayer *pHost = GetHostPlayer();

	int idServiceNPC = pHost->GetCurServiceNPC();
	if( 0 == idServiceNPC ) return true;

	CECNPC *pNPC = pWorld->GetNPCMan()->GetNPC(idServiceNPC);
	if( !pNPC || !pNPC->IsServerNPC() ) return true;

	CECNPCServer *pServer = (CECNPCServer *)pNPC;
	int nNumSkills;
	nNumSkills = pServer->GetSkillNum();

	CECSkill *pSkill;
	int i, idSkill, nCondition;
	abase::vector<CECSkill *> vecSkillAvailable;
	int aConditions[128];
	CECIvtrGoblin *pGoblin = (CECIvtrGoblin *)m_pImg_ELF->GetDataPtr();
	if (!pGoblin)
	{
		return true;
	}

	for( i = 0; i < nNumSkills; i++ )
	{
		pSkill = pServer->GetSkill(i);
		idSkill = pSkill->GetSkillID();
		nCondition = pGoblin->CheckSkillLearnCondition(idSkill, true);
		if( 0 == nCondition || 1 == nCondition || 2 == nCondition ||
			4 == nCondition || 10 == nCondition || 11 == nCondition || 12== nCondition)
		{
			aConditions[vecSkillAvailable.size()] = nCondition;
			vecSkillAvailable.push_back(pSkill);
		}
	}
	nNumSkills = vecSkillAvailable.size();

	AString strFile;
	PAUIIMAGEPICTURE pImage;
	int idxSkill, nSkillLevel, nSP, nCost;

	if( nPage >= 0 )
	{
		m_nCurTeachPage = nPage;
	}
	else
	{
		m_nCurTeachPage = max(0, min(m_nCurTeachPage, (nNumSkills
			+ CDLGELFLEARN_PP - 1) / CDLGELFLEARN_PP - 1));
	}
	SetDataPtr((void *)nNumSkills,"int");

	ACHAR szSkillDesc[1024];

	for( i = 0; i < CDLGELFLEARN_PP; i++ )
	{
		pImage = m_pImg_SkillIcon[i];

		idxSkill = m_nCurTeachPage * CDLGELFLEARN_PP + i;
		if( idxSkill < nNumSkills )
		{
			pSkill = vecSkillAvailable[idxSkill];
			idSkill = pSkill->GetSkillID();
			nSkillLevel = 1;
			int j;
			for (j = 0; j < pGoblin->GetSkillNum(); j++)
			{
				if (idSkill == pGoblin->GetSkill(j).skill)
				{
					nSkillLevel = pGoblin->GetSkill(j).level+1;
					break;
				}
			}
			CECSkill::GetDesc(idSkill, nSkillLevel, szSkillDesc, 1024);

			af_GetFileTitle(pSkill->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
			pImage->SetHint(szSkillDesc);
			pImage->SetDataPtr(pSkill,"ptr_CECSkill");
			
			ACString strName;
			strName.Format(_AL("%s %d"), g_pGame->GetSkillDesc()->GetWideString(pSkill->GetSkillID() * 10), nSkillLevel);
			m_pTxt_SkillName[i]->SetText(strName);

			nSP = GNET::ElementSkill::GetRequiredSp(idSkill, nSkillLevel);
			nCost = GNET::ElementSkill::GetRequiredMoney(idSkill, nSkillLevel);

			if (pGoblin->CheckSkillLearnCondition(idSkill, true))				
				pImage->SetColor(A3DCOLORRGB(128, 128, 128));
			else
				pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			int *require = pSkill->GetRequiredGenius();
			ACString strReq = _AL("");
			ACString strTemp;
			for (j = 0; j < 5; j++)
			{
				if (require[j] > 0)
				{
					if (require[j] > pGoblin->GetGenius(j))
					{
						strTemp.Format(_AL("%s:^ff0000%d^ffffff"), GetStringFromTable(7110 + j), require[j]);
					}
					else
						strTemp.Format(_AL("%s:%d"), GetStringFromTable(7110 + j), require[j]);
					strReq += strTemp;
				}
			}
			m_pTxt_Xuqiu[i]->SetText(strReq);

			ACString strText;
			strText.Format(_AL("%s%d"), GetStringFromTable(7119), nSP);
			m_pTxt_SkillCostSP[i]->SetText(strText);

		}
		else
		{
			pImage->SetHint(_AL(""));
			pImage->SetDataPtr(NULL);
			pImage->SetCover(NULL, -1);
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			m_pTxt_SkillName[i]->SetText(_AL(""));
			m_pTxt_SkillCostSP[i]->SetText(_AL(""));
			m_pTxt_Xuqiu[i]->SetText(_AL(""));
		}
	}

	return true;
}

void CDlgELFLearn::SetELF(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pImg_ELF->SetDataPtr(pELF);
	m_nSlot = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_ELF->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_ELF->SetHint(trans.Translate(szDesc));
	else
		m_pImg_ELF->SetHint(_AL(""));
	m_pTxt_ELFName->SetText(pELF->GetName());
	UpdateTeach(0);
}

void CDlgELFLearn::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
	if( pELF )
		pELF->Freeze(false);
	m_pImg_ELF->ClearCover();
	m_pImg_ELF->SetHint(GetStringFromTable(7301));
	m_pImg_ELF->SetDataPtr(NULL);
	m_pTxt_ELFName->SetText(_AL(""));
	m_nSlot = -1;
	Reset();
}

void CDlgELFLearn::Reset()
{
	int i;
	for (i = 0; i < CDLGELFLEARN_PP; i++)
	{
		m_pTxt_SkillName[i]->SetText(_AL(""));
		m_pTxt_SkillCostSP[i]->SetText(_AL(""));
		m_pTxt_Xuqiu[i]->SetText(_AL(""));
		m_pImg_SkillIcon[i]->ClearCover();
		m_pImg_SkillIcon[i]->SetHint(_AL(""));
		m_pImg_SkillIcon[i]->SetDataPtr(NULL);
	}
	m_nSlot = -1;
}

void CDlgELFLearn::OnTick(void)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin *)m_pImg_ELF->GetDataPtr();
	if (pELF)
	{
		AUICTranslate trans;
		const ACHAR * szDesc = pELF->GetDesc();
		if( szDesc )
			m_pImg_ELF->SetHint(trans.Translate(szDesc));
		else
			m_pImg_ELF->SetHint(_AL(""));
	}
	else
	{
		m_pImg_ELF->SetHint(GetStringFromTable(7301));
	}
}

void CDlgELFLearn::SetNPCName(ACString name)
{
	if(GetDlgItem("Txt_npc"))
		GetDlgItem("Txt_npc")->SetText(name);
}

void CDlgELFLearn::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_ELF", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
			this->SetELF(pIvtrSrc, iSrc);
}