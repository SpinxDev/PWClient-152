// File		: DlgHostELF.cpp
// Creator	: Chen Zhixin
// Date		: 2008/11/07

#include "AFI.h"
#include "DlgHostELF.h"
#include "DlgPetDetail.h"
#include "DlgPetList.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_World.h"
#include "DlgELFProp.h"
#include "DlgTrade.h"
#include "DlgShop.h"
#include "EC_HostGoblin.h"
#include "DlgELFRefine.h"
#include "DlgELFToPill.h"
#include "DlgELFRetrain.h"
#include "DlgELFXidian.h"
#include "EC_Skill.h"
#include "EC_Utility.h"
#include "AUICheckBox.h"
//#include "EC_Shortcut.h"

#define new A_DEBUG_NEW
#define MAX_ELF_SKILL	8

AUI_BEGIN_COMMAND_MAP(CDlgHostELF, CDlgBase)

AUI_ON_COMMAND("activation",		OnCommand_Activation)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_ON_COMMAND("max",				OnCommand_Max)
AUI_ON_COMMAND("min",				OnCommand_Min)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgHostELF, CDlgBase)

AUI_ON_EVENT("Skill_*",			WM_LBUTTONUP,		OnEventSkill)
AUI_ON_EVENT("Skill_*",			WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Img_Elf",			WM_RBUTTONDOWN,		OnEventRButtonUp)
AUI_ON_EVENT("Img_Active",		WM_RBUTTONDOWN,		OnEventRButtonUp)

AUI_END_EVENT_MAP()

CDlgHostELF::CDlgHostELF()
{
	m_nSlot = -1;
}

CDlgHostELF::~CDlgHostELF()
{
}

void CDlgHostELF::OnCommand_Activation(const char * szCommand)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		return;
	}
	GetGameSession()->c2s_CmdGoblinRefineActive(pHostGoblin->GetTemplateID());
	
}


void CDlgHostELF::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgHostELF::OnCommand_Max(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ELF");
	POINT pt = GetPos();
	pDlg->SetPosEx(pt.x, pt.y);
	pDlg->Show(true);
	Show(false);
}

void CDlgHostELF::OnCommand_Min(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ELFZoom");
	POINT pt = GetPos();
	pDlg->SetPosEx(pt.x, pt.y);
	pDlg->Show(true);
	Show(false);
}

bool CDlgHostELF::OnInitDialog()
{
	m_pImg_Icon = (PAUIIMAGEPICTURE)GetDlgItem("Img_Elf");
	m_pPro_energy = (PAUIPROGRESS)GetDlgItem("Progress_Energy");
	m_pPro_power = (PAUIPROGRESS)GetDlgItem("Progress_Power");
	m_pTxt_name = (PAUILABEL)GetDlgItem("ELF_Name");
	m_pImg_Activation = (PAUIIMAGEPICTURE)GetDlgItem("Img_Active");	
	//m_pELFProp = (CDlgELFProp *)GetGameUIMan()->GetDialog("Win_CharacterELF");
	return true;
}

bool CDlgHostELF::Render()
{
	bool bRet = CDlgBase::Render();
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		Show(false);
		return true;
	}
	int i = 0;
	char szName[20];
	AUIClockIcon *pClock;
	ACString strELFName = pHostGoblin->GetName();
	static int frame = 0;
	ACString strText;
	if (0 != pHostGoblin->GetMaxAP())
	{
		float fPercent = ((float)pHostGoblin->GetAP()/pHostGoblin->GetMaxAP());
		m_pPro_energy->SetProgress((int)(fPercent*100));
		strText.Format(_AL("%d/%d"), pHostGoblin->GetAP(), pHostGoblin->GetMaxAP());
		GetDlgItem("Lab_Energy")->SetText(strText);
	}
	else
	{
		m_pPro_energy->SetProgress(0);
	}
	if (0 == stricmp(m_szName, "Win_ELFZoom"))
	{
		return true;
	}
	for (; i < MAX_ELF_SKILL; i++)
	{
		sprintf(szName, "Skill_%d", i+1);
		PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		if (i < pHostGoblin->GetSkillNum())
		{
			CECSkill *pSkill = (CECSkill *)pHostGoblin->GetSkill(i);
			SetImage(pImg, pSkill);
			pImg->Show(true);
			pClock = pImg->GetClockIcon();
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
			if( pSkill && pSkill->ReadyToCast())
			{
				if( !pHostGoblin->CheckSkillCastCondition(pSkill) )
					pImg->SetColor(A3DCOLORRGB(255, 255, 255));
				else
					pImg->SetColor(A3DCOLORRGB(128, 128, 128));
			}
			else
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			if( pSkill && pSkill->GetCoolingTime() > 0)
			{
				pClock->SetProgressRange(0, pSkill->GetCoolingTime());
				pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
			}
		}
		else
			pImg->Show(false);
	}
	//是否激活 使用fixframe确定显示哪一幁
	AUIObject* pBtn = GetDlgItem("Btn_Power");
	if (pHostGoblin->IsRefineActive())
	{
		frame = (frame + 1)%16;
		m_pImg_Activation->FixFrame((int)(frame/4));
		pBtn->SetHint(GetStringFromTable(7314));
	}
	else
	{
		ACString str;
		str.Format(GetStringFromTable(7313), pHostGoblin->GetMPCost());
		pBtn->SetHint(str);
		m_pImg_Activation->FixFrame(0);
	}
	if (pHostGoblin->GetRefineLevel() == 0)
	{
		pBtn->Enable(false);
		pBtn->SetHint(_AL(""));
	}
	else
		pBtn->Enable(true);
	

	GOBLIN_ESSENCE* pEssence = (GOBLIN_ESSENCE *)pHostGoblin->GetDBEssence();
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
	m_pImg_Icon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_ELF],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_ELF][strFile]);
	m_pTxt_name->SetText(strELFName);
	strText.Format(_AL("%d/999999"), pHostGoblin->GetStamina());
	GetDlgItem("Lab_Power")->SetText(strText);
	m_pPro_power->SetProgress(pHostGoblin->GetStamina()*100/999999);
 
 	return bRet;
}

void CDlgHostELF::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
 	CDlgELFProp* pDlg = GetGameUIMan()->m_pDlgELFProp;
	if( pDlg != NULL && !pDlg->IsShow() )
	{
		GetGameSession()->c2s_CmdGetExtProps();
		pDlg->ResetPoints();
	}
	if(pDlg)
		pDlg->Show(!pDlg->IsShow());
}

void CDlgHostELF::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!pObj->GetDataPtr("ptr_CECGoblinSkill"))
	{
		return;
	}

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgHostELF::OnEventSkill(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	int i = atoi(pObj->GetName() + strlen("Skill_"));
	CECHostPlayer* pHost = GetHostPlayer();
	int idSelected = pHost->GetSelectedTarget();
	bool bForctAttack = glb_GetForceAttackFlag(NULL);
	
	pHostGoblin->CastSkill(i-1, idSelected, bForctAttack);
}

void CDlgHostELF::SetImage(AUIImagePicture *pImage, CECSkill *pSkill)
{
	ASSERT(pImage);
	
	if (pSkill)
	{
		AString strFile;
		af_GetFileTitle(pSkill->GetIconFile(), strFile);
		strFile.MakeLower();
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
		pImage->SetDataPtr(pSkill,"ptr_CECGoblinSkill");
		pImage->SetData(0);
		pImage->SetHint(pSkill->GetDesc());
	}
	else
	{
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);
		pImage->SetData(0);
		pImage->SetHint(_AL(""));
	}
}