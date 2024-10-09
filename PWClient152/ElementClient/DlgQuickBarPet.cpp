// File		: DlgQuickBarPet.cpp
// Creator	: Xiao Zhou
// Date		: 2006/3/22

#include "AFI.h"
#include "DlgQuickBarPet.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_Utility.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_Skill.h"
#include "EC_Global.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQuickBarPet, CDlgBase)

AUI_ON_COMMAND("switch",		OnCommandSwitch)
AUI_ON_COMMAND("attack",		OnCommandAttack)
AUI_ON_COMMAND("follow",		OnCommandFollow)
AUI_ON_COMMAND("stop",			OnCommandStop)
AUI_ON_COMMAND("offensive",		OnCommandOffensive)
AUI_ON_COMMAND("defensive",		OnCommandDefensive)
AUI_ON_COMMAND("combat",		OnCommandCombat)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQuickBarPet, CDlgBase)

AUI_ON_EVENT("Skill_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Skill)
AUI_ON_EVENT("Skill_*",		WM_RBUTTONDOWN,		OnEventRButtonDown_Skill)

AUI_END_EVENT_MAP()

int CDlgQuickBarPet::m_nAutoCastID = 0;


CDlgQuickBarPet::CDlgQuickBarPet()
{
	m_idLastSkill = 0;
	m_idLastTarget = 0;
}

CDlgQuickBarPet::~CDlgQuickBarPet()
{
}

bool CDlgQuickBarPet::OnInitDialog()
{
	m_pBtn_Combat = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Combat");
	m_pBtn_Offensive = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Offensive");
	m_pBtn_Defensive = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Defensive");
	m_pBtn_Stop = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Stop");
	m_pBtn_Follow = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Follow");
	int i=0;
	for(i = 0; i < MAX_PET_ALL_SKILL_SLOT; i++ )
	{
		char szText[20];
		sprintf(szText, "Skill_%d", i + 1);
		m_pImg_Skill[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
	}

	// record the default size
	OnChangeLayoutEnd(true);

	return true;
}
void CDlgQuickBarPet::ResetAutoCastSkill()
{
	CECPetCorral *pPetCorral = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral();
	CECPetData *pPet = pPetCorral->GetActivePet();
	if( pPet && pPet->GetSkillNum(CECPetData::EM_SKILL_NORMAL) > 0 )
	{
		int param = pPet->GetSkill(CECPetData::EM_SKILL_NORMAL,0)->idSkill;
		g_pGame->GetGameSession()->c2s_CmdPetCtrl(0, 5, &param, sizeof(param));
		m_nAutoCastID = param;
	}
}
void CDlgQuickBarPet::OnShowDialog()
{
	ResetAutoCastSkill();
}

void CDlgQuickBarPet::OnCommandCancel(const char * szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

void CDlgQuickBarPet::OnCommandSwitch(const char * szCommand)
{
	if( 0 == stricmp(GetName(), "Win_QuickbarPetH") )
	{
		GetGameUIMan()->GetDialog("Win_QuickbarPetH")->Show(false);
		GetGameUIMan()->m_pDlgQuickBarPet = 
			(CDlgQuickBarPet*)GetGameUIMan()->GetDialog("Win_QuickbarPetV");
		GetGameUIMan()->m_pDlgQuickBarPet->Show(true);
	}
	else if( 0 == stricmp(GetName(), "Win_QuickbarPetV") )
	{
		m_pAUIManager->GetDialog("Win_QuickbarPetV")->Show(false);
		GetGameUIMan()->m_pDlgQuickBarPet = 
			(CDlgQuickBarPet*)GetGameUIMan()->GetDialog("Win_QuickbarPetH");
		GetGameUIMan()->m_pDlgQuickBarPet->Show(true);
	}
}

void CDlgQuickBarPet::OnCommandAttack(const char * szCommand)
{
	CECHostPlayer* pHost = GetHostPlayer();
	int idSelected = pHost->GetSelectedTarget();
	bool bForctAttack = glb_GetForceAttackFlag(NULL);

	if( pHost->AttackableJudge(idSelected, bForctAttack) == 1 )
	{
		BYTE param = glb_BuildPVPMask(bForctAttack);
		GetGameSession()->c2s_CmdPetCtrl(GetHostPlayer()->GetSelectedTarget(), 1, &param, sizeof(char));
	}
}

void CDlgQuickBarPet::OnCommandFollow(const char * szCommand)
{
	int param = 0;
	GetGameSession()->c2s_CmdPetCtrl(0, 2, &param, sizeof(int));
}

void CDlgQuickBarPet::OnCommandStop(const char * szCommand)
{
	int param = 1;
	GetGameSession()->c2s_CmdPetCtrl(0, 2, &param, sizeof(int));
}

void CDlgQuickBarPet::OnCommandDefensive(const char * szCommand)
{
	int param = 0;
	GetGameSession()->c2s_CmdPetCtrl(0, 3, &param, sizeof(int));
}

void CDlgQuickBarPet::OnCommandOffensive(const char * szCommand)
{
	int param = 1;
	GetGameSession()->c2s_CmdPetCtrl(0, 3, &param, sizeof(int));
}

void CDlgQuickBarPet::OnCommandCombat(const char * szCommand)
{
	int param = 2;
	GetGameSession()->c2s_CmdPetCtrl(0, 3, &param, sizeof(int));
}

void CDlgQuickBarPet::OnEventLButtonDown_Skill(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
#pragma pack(1)

	struct CMDPARAM
	{
		int idSkill;
		BYTE byPVPMask;
	};

#pragma pack()

	int nSkill = atoi(pObj->GetName() + strlen("Skill_"));
	if( pObj->GetData() == 0 )
		return;

	CECHostPlayer* pHost = GetHostPlayer();
	int idPet = pHost->GetPetCorral()->GetActivePetNPCID();

	CMDPARAM param;
	param.idSkill = pObj->GetData();
	param.byPVPMask = glb_BuildPVPMask(glb_GetForceAttackFlag(NULL));

	int iSkillType = GNET::ElementSkill::GetType(param.idSkill);
	int idTarget = 0;

	if (iSkillType == GNET::TYPE_BLESSPET)
	{
		idTarget = idPet;
	}
	else
	{
		idTarget = pHost->GetSelectedTarget();

		if (iSkillType == GNET::TYPE_ATTACK || iSkillType == GNET::TYPE_CURSE)
		{
			bool bForctAttack = glb_GetForceAttackFlag(NULL);
			if (pHost->AttackableJudge(idTarget, bForctAttack) != 1)
				return;
		}
	}

	if( idTarget == m_idLastTarget && param.idSkill == m_idLastSkill && 
		CECTimeSafeChecker::ElapsedTimeFor(m_dwLastSkillTime) < 2000 )
		return;

	GetGameSession()->c2s_CmdPetCtrl(idTarget, 4, &param, sizeof(param));
	SetCapture(false);

	m_idLastTarget = idTarget;
	m_idLastSkill = param.idSkill;
	m_dwLastSkillTime = GetTickCount();
}

void CDlgQuickBarPet::OnEventRButtonDown_Skill(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nSkill = atoi(pObj->GetName() + strlen("Skill_"));
	if( pObj->GetData() != 0 )
	{
		int idSkill = pObj->GetData();
		if( m_nAutoCastID == idSkill )
		{
			int param = 0;
			GetGameSession()->c2s_CmdPetCtrl(0, 5, &param, sizeof(param));
			m_nAutoCastID = 0;
		}
		else
		{
			int param = idSkill;
			GetGameSession()->c2s_CmdPetCtrl(0, 5, &param, sizeof(param));
			m_nAutoCastID = idSkill;
		}
		SetCapture(false);
	}
}

void CDlgQuickBarPet::OnTick()
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	switch(pPetCorral->GetAttackMode())
	{
	case CECPetCorral::ATK_POSITIVE:
		m_pBtn_Combat->SetPushed(false);
		m_pBtn_Defensive->SetPushed(false);
		m_pBtn_Offensive->SetPushed(true);
		break;
	case CECPetCorral::ATK_DEFENSE:
		m_pBtn_Combat->SetPushed(false);
		m_pBtn_Defensive->SetPushed(true);
		m_pBtn_Offensive->SetPushed(false);
		break;
	case CECPetCorral::ATK_PASSIVE:
		m_pBtn_Combat->SetPushed(true);
		m_pBtn_Defensive->SetPushed(false);
		m_pBtn_Offensive->SetPushed(false);
		break;
	}

	switch(pPetCorral->GetMoveMode())
	{
	case CECPetCorral::MOVE_FOLLOW:
		m_pBtn_Stop->SetPushed(false);
		m_pBtn_Follow->SetPushed(true);
		break;
	case CECPetCorral::MOVE_STAND:
		m_pBtn_Stop->SetPushed(true);
		m_pBtn_Follow->SetPushed(false);
		break;
	}
	CECPetData * pPet = pPetCorral->GetActivePet();
	if( !pPet )
		return;
	
	ACHAR szText[1000];
	CECStringTab* pDescTab = GetGame()->GetItemDesc();
	int nSkillSum = -1;
	bool bFind = false;
	int imgIdx = 0;
	int i=0;
	for(i = 0; i < MAX_PET_ALL_SKILL_SLOT; i++ )
	{
		int idx = 0;
		CECPetData::SKILLTYPE pet_st = CECPetData::EM_SKILL_NORMAL;

		if (i<MAX_PET_SKILL_SLOT)
		{
			idx = i;
			pet_st = CECPetData::EM_SKILL_NORMAL;
		}
		else if (i<MAX_PET_SKILL_SLOT + MAX_PET_NATURE_SKILL_SLOT)
		{
			idx = i - MAX_PET_SKILL_SLOT;
			pet_st = CECPetData::EM_SKILL_NATURE;
		}
		else // special skill , idx is not used
		{
			idx = i; // not used
			pet_st = CECPetData::EM_SKILL_SPECIAL;
		}

		const CECPetData::PETSKILL *pSkill = pPet->GetSkill(pet_st,idx);
		if( pSkill && pSkill->idSkill != 0 )
		{
			m_pImg_Skill[imgIdx]->Show(true);
			nSkillSum = imgIdx;
			m_pImg_Skill[imgIdx]->SetData(pSkill->idSkill);
			const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pSkill->idSkill * 10);
			CECSkill::GetDesc(pSkill->idSkill, pSkill->iLevel, szText, 1000);
			m_pImg_Skill[imgIdx]->SetHint(szText);
			const char* szIcon = GNET::ElementSkill::GetIcon(pSkill->idSkill);
			AString strFile;
			af_GetFileTitle(szIcon, strFile);
			strFile.MakeLower();
			m_pImg_Skill[imgIdx]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
			AUIClockIcon *pClock;
			pClock = m_pImg_Skill[imgIdx]->GetClockIcon();
			int nMax;
			int nCool = pPet->GetSkillCoolTime(pet_st,idx, &nMax);
			if( nMax > 0 )
			{
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
				pClock->SetProgressRange(0, nMax);
				pClock->SetProgressPos(nMax - nCool);
			}
			else
			{
				pClock->SetProgressRange(0, 1);
				pClock->SetProgressPos(1);
			}
			if( m_nAutoCastID == (int)m_pImg_Skill[imgIdx]->GetData() )
			{
				m_pImg_Skill[imgIdx]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
				bFind = true;
			}
			else
				m_pImg_Skill[imgIdx]->SetCover(NULL, 0, 1);

			imgIdx++;
		}
	}
	for (i = imgIdx;i<MAX_PET_ALL_SKILL_SLOT;i++)
	{
		m_pImg_Skill[i]->ClearCover();
		m_pImg_Skill[i]->SetHint(_AL(""));
		m_pImg_Skill[i]->SetData(0);
		m_pImg_Skill[i]->Show(false);
	}

	if( !bFind )
		m_nAutoCastID = 0;

	// 调整对话框大小，使只显示必要的技能
	if (nSkillSum < MAX_PET_ALL_SKILL_SLOT)
	{
		int missed = MAX_PET_ALL_SKILL_SLOT - nSkillSum - 1;
		int nWidth = m_Original.cx - m_IconMargin.cx * missed;
		int nHeight = m_Original.cy - m_IconMargin.cy * missed;

		SetDefaultSize(nWidth, nHeight);
	}
}

void CDlgQuickBarPet::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		m_Original = GetDefaultSize();
		if(MAX_PET_ALL_SKILL_SLOT > 1)
		{
			POINT p0 = m_pImg_Skill[0]->GetPos(true);
			POINT p1 = m_pImg_Skill[1]->GetPos(true);
			m_IconMargin.cx = p1.x - p0.x;
			m_IconMargin.cy = p1.y - p0.y;
		}
		else
		{
			m_IconMargin.cx = 0;
			m_IconMargin.cy = 0;
		}
	}
}