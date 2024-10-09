// File		: DlgTeach.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#include "AFI.h"
#include "AUIImagePicture.h"
#include "A2DSprite.h"
#include "DlgTeach.h"
#include "DlgSkillTree.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_NPCServer.h"
#include "EC_Skill.h"
#include "EC_World.h"
#include "EC_PetCorral.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "ExpTypes.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTeach, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("movel",		OnCommandMovel)
AUI_ON_COMMAND("mover",		OnCommandMover)
AUI_ON_COMMAND("skilltree",	OnCommandSkilltree)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgTeach, CDlgBase)

AUI_ON_EVENT("Skill_*",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()


CDlgTeach::CDlgTeach()
		:m_nCurTeachPage(0)
{
	m_pTxtRestSP = NULL;
	for(int i = 0; i < CDLGTEACH_TEACHS_PP; i ++)
	{
		m_pSkillIcon[i] = NULL;
		m_pSkillName[i] = NULL;
		m_pSkillLevel[i] = NULL;
		m_pSkillCostSP[i] = NULL;
		m_pSkillCostMoney[i] = NULL;
	}
}

CDlgTeach::~CDlgTeach()
{
}

void CDlgTeach::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_RestSP", m_pTxtRestSP);
	char szName[40];
	for(int i = 0; i < CDLGTEACH_TEACHS_PP; i ++)
	{
		sprintf(szName, "Skill_%d", i + 1);
		DDX_Control(szName, m_pSkillIcon[i]);
		sprintf(szName, "Txt_SkillName%d", i + 1);
		DDX_Control(szName, m_pSkillName[i]);
		sprintf(szName, "Txt_SkillLevel%d", i + 1);
		DDX_Control(szName, m_pSkillLevel[i]);
		sprintf(szName, "Txt_CostSP%d", i + 1);
		DDX_Control(szName, m_pSkillCostSP[i]);
		sprintf(szName, "Txt_CostMoney%d", i + 1);
		DDX_Control(szName, m_pSkillCostMoney[i]);
	}
}

void CDlgTeach::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
}

void CDlgTeach::OnCommandMovel(const char *szCommand)
{
	if( m_nCurTeachPage > 0 )
		UpdateTeach( m_nCurTeachPage - 1);
}

void CDlgTeach::OnCommandMover(const char *szCommand)
{
	int nNumSkills = (int)GetDataPtr("int");
	int nNumPages = (nNumSkills + CDLGTEACH_TEACHS_PP - 1) / CDLGTEACH_TEACHS_PP;

	if( m_nCurTeachPage < nNumPages - 1 )
		UpdateTeach( m_nCurTeachPage + 1);
}

void CDlgTeach::OnCommandSkilltree(const char *szCommand)
{
	if( GetData() == DT_NPC_PETLEARNSKILL_SERVICE ){
		CDlgSkillTree* pShow = (CDlgSkillTree*)GetGameUIMan()->GetDialog("Win_SkillPet");
		pShow->Show(!pShow->IsShow());
	}
}

void CDlgTeach::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj->GetDataPtr("ptr_CECSkill") ) return;
	PAUIDIALOG pMsgBox;

	CECSkill *pSkill = (CECSkill *)pObj->GetDataPtr("ptr_CECSkill");
	if(!pSkill) return;

	if(CDlgAutoHelp::IsAutoHelp())
	{
		CDlgWikiShortcut::PopSkillWiki(GetGameUIMan(),pSkill->GetSkillID());
		return;
	}

	if( GetData() == DT_NPC_PETLEARNSKILL_SERVICE )
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetActivePet();
		const CECPetData::PETSKILL* pPetSkill = pPet->GetSkillByID(pSkill->GetSkillID());
		if ( !pPetSkill && pPet->GetSkillNum(CECPetData::EM_SKILL_NORMAL) == 4 )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(815),	MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
	}

	int nCondition;
	if( GetData() == DT_NPC_SKILL_SERVICE )
		nCondition = GetHostPlayer()->CheckSkillLearnCondition(pSkill->GetSkillID(), true);
	else
		nCondition = GetHostPlayer()->CheckPetSkillLearnCondition(pSkill->GetSkillID(), true);
	int nCheckCode = 0;
	
	if( 1 == nCondition )
		nCheckCode = 270;
	else if( 6 == nCondition )
		nCheckCode = 527;
	else if( 7 == nCondition )
		nCheckCode = 541;
	else if( 8 == nCondition )
		nCheckCode = 271;
	else if( 9 == nCondition )
		nCheckCode = 556;
	else if( 10 == nCondition )
		nCheckCode = 557;
	else if (12 == nCondition)
		nCheckCode = 11168;
	if( nCheckCode == 0 )
	{
		if( GetData() == DT_NPC_SKILL_SERVICE )
			GetGameUIMan()->MessageBox("Game_TeachSkill", GetGameUIMan()->GetStringFromTable(231),
				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		else
			GetGameUIMan()->MessageBox("Game_TeachPetSkill", GetGameUIMan()->GetStringFromTable(231),
				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		void *ptr;
		AString szType;
		pObj->ForceGetDataPtr(ptr,szType);
		pMsgBox->SetDataPtr(ptr,szType);
	}
	else
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(nCheckCode),	MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
	}
}

bool CDlgTeach::UpdateTeach(int nPage)
{
	CECWorld *pWorld = GetWorld();
	CECHostPlayer *pHost = GetHostPlayer();

	int idServiceNPC = pHost->GetCurServiceNPC();
	if( 0 == idServiceNPC ) return true;

	CECNPC *pNPC = pWorld->GetNPCMan()->GetNPC(idServiceNPC);
	if( !pNPC || !pNPC->IsServerNPC() ) return true;

	CECNPCServer *pServer = (CECNPCServer *)pNPC;
	int nNumSkills;
	if( GetData() == DT_NPC_SKILL_SERVICE )
		nNumSkills = pServer->GetSkillNum();
	else
		nNumSkills = pServer->GetPetSkillNum();

	CECSkill *pSkill;
	int i, idSkill, nCondition;
	abase::vector<CECSkill *> vecSkillAvailable;
	int aConditions[128];

	for( i = 0; i < nNumSkills; i++ )
	{
		if( GetData() == DT_NPC_SKILL_SERVICE )
			pSkill = pServer->GetSkill(i);
		else
			pSkill = pServer->GetPetSkill(i);
		idSkill = pSkill->GetSkillID();
		if( GetData() == DT_NPC_SKILL_SERVICE )
			nCondition = pHost->CheckSkillLearnCondition(idSkill, true);
		else
			nCondition = pHost->CheckPetSkillLearnCondition(idSkill, true);
		if( 0 == nCondition || 1 == nCondition || 6 == nCondition ||
			7 == nCondition || /*8 == nCondition ||*/ 9 == nCondition ||
			10 == nCondition || 12 == nCondition)
		{
			aConditions[vecSkillAvailable.size()] = nCondition;
			vecSkillAvailable.push_back(pSkill);
		}
	}
	nNumSkills = vecSkillAvailable.size();

	char szName[40];
	AString strFile;
	ACHAR szText[40];
	CECSkill *pSkillHost;
	PAUIIMAGEPICTURE pImage;
	int idxSkill, nSkillLevel, nSP, nCost;

	if( nPage >= 0 )
	{
		m_nCurTeachPage = nPage;
	}
	else
	{
		m_nCurTeachPage = max(0, min(m_nCurTeachPage, (nNumSkills
			+ CDLGTEACH_TEACHS_PP - 1) / CDLGTEACH_TEACHS_PP - 1));
	}
	SetDataPtr((void *)nNumSkills,"int");

	const ROLEBASICPROP &rbp = pHost->GetBasicProps();

	a_sprintf(szText, _AL("%d"), rbp.iSP);
	m_pTxtRestSP->SetText(szText);
	
	ACHAR szSkillDesc[1024];

	for( i = 0; i < CDLGTEACH_TEACHS_PP; i++ )
	{
		pImage = m_pSkillIcon[i];

		idxSkill = m_nCurTeachPage * CDLGTEACH_TEACHS_PP + i;
		if( idxSkill < nNumSkills )
		{
			pSkill = vecSkillAvailable[idxSkill];
			idSkill = pSkill->GetSkillID();
			if( GetData() == DT_NPC_SKILL_SERVICE )
			{
				pSkillHost = pHost->GetNormalSkill(idSkill);
				if( pSkillHost )
					nSkillLevel = pSkillHost->GetSkillLevel() + 1;
				else
					nSkillLevel = 1;
			}
			else
			{
				CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
				CECPetData * pPet = pPetCorral->GetActivePet();
				const CECPetData::PETSKILL* pPetSkill = pPet->GetSkillByID(idSkill);
				if( pPetSkill )
					nSkillLevel = pPetSkill->iLevel + 1;
				else
					nSkillLevel = 1;
			}
			CECSkill::GetDesc(idSkill, nSkillLevel, szSkillDesc, 1024);

			af_GetFileTitle(pSkill->GetIconFile(), strFile);
			strFile.MakeLower();
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
			pImage->SetHint(szSkillDesc);
			pImage->SetDataPtr(pSkill,"ptr_CECSkill");

			m_pSkillName[i]->SetText(g_pGame->GetSkillDesc()->GetWideString(pSkill->GetSkillID() * 10));

			nSP = GNET::ElementSkill::GetRequiredSp(idSkill, nSkillLevel);
			nCost = GNET::ElementSkill::GetRequiredMoney(idSkill, nSkillLevel);

			if (aConditions[idxSkill])
				pImage->SetColor(A3DCOLORRGB(128, 128, 128));
			else
				pImage->SetColor(A3DCOLORRGB(255, 255, 255));

			a_sprintf(szText, _AL("%d"), nSkillLevel);
			m_pSkillLevel[i]->SetText(szText);

			a_sprintf(szText, _AL("%d"), nSP);
			m_pSkillCostSP[i]->SetText(szText);

			a_sprintf(szText, _AL("%d"), nCost);
			m_pSkillCostMoney[i]->SetText(szText);
		}
		else
		{
			pImage->SetHint(_AL(""));
			pImage->SetDataPtr(NULL);
			pImage->SetCover(NULL, -1);
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));

			sprintf(szName, "Txt_SkillName%d", i + 1);
			m_pSkillName[i]->SetText(_AL(""));

			sprintf(szName, "Txt_SkillLevel%d", i + 1);
			m_pSkillLevel[i]->SetText(_AL(""));

			sprintf(szName, "Txt_CostSP%d", i + 1);
			m_pSkillCostSP[i]->SetText(_AL(""));

			sprintf(szName, "Txt_CostMoney%d", i + 1);
			m_pSkillCostMoney[i]->SetText(_AL(""));
		}
	}

	return true;
}
