// Filename	: DlgPreviewPetRebuild.cpp
// Creator	: Wang Yongdong
// Date		: 2012/12/04

#include "DlgPreviewPetRebuild.h"
#include "AFI.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GPDataType.h"
#include "AUICTranslate.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "EC_HostPlayer.h"
#include "DlgPetEvolution.h"
#include "EC_PetCorral.h"
#include "EC_Global.h"
#include "ExpTypes.h"
#include "elementdataman.h"
#include "EC_Skill.h"
#include "DlgPetEvolutionAnim.h"
#include "EC_TimeSafeChecker.h"

AUI_BEGIN_COMMAND_MAP(CDlgPreviewPetRebuild, CDlgBase)
AUI_ON_COMMAND("SelectOld",OnCommandSelectOldOne)
AUI_ON_COMMAND("SelectNew",OnCommandSelectNewOne)
AUI_ON_COMMAND("Reproduce",OnCommandReproduce)
AUI_ON_COMMAND("Close", OnCommandCANCEL)
AUI_ON_COMMAND("IDCANCEL", OnCommandCANCEL)
AUI_END_COMMAND_MAP()

static const char* ENERGY_BALL_SMALL = "界面\\生产能量球小.gfx";
static const char* ENERGY_BALL_MIDDLE = "界面\\生产能量球中.gfx";
static const int ANIMATION_LAST_COUNT_MINISECONDS = 1000;

CDlgPreviewPetRebuild::CDlgPreviewPetRebuild():
m_iStopThinkingTime(0),
m_pButtonSelectOld(NULL),
m_pButtonSelectNew(NULL),
m_pButtonReproduce(NULL),
m_pTextAttributeAfter(NULL),
m_pTextAttributeBefore(NULL),
m_pLabelTime(NULL),
m_bThinking(false),
m_pImgOld(NULL),
m_pImgNew(NULL),
m_pGfxLightLeft(NULL),
m_pGfxLightRight(NULL),
m_pGfxCore(NULL),
m_iAnimationTickStart(0),
m_bAnimationPlaying(false),
m_iType(-1),
m_iPetIdx(-1),
m_iUseTime(0)
{
	memset(m_pImgSkill,0,sizeof(m_pImgSkill));
	memset(m_pSkillName,0,sizeof(m_pSkillName));
	memset(m_bGfxVisible,0,sizeof(m_bGfxVisible));

	memset(m_szGfxFileOld,0,sizeof(m_szGfxFileOld));
	memset(m_szGfxFileNew,0,sizeof(m_szGfxFileNew));
}

bool CDlgPreviewPetRebuild::OnInitDialog()
{
	DDX_Control("SelectOld",m_pButtonSelectOld);
	DDX_Control("SelectNew",m_pButtonSelectNew);
	DDX_Control("Reproduce",m_pButtonReproduce);
	DDX_Control("AttributeBefore",m_pTextAttributeBefore);
	DDX_Control("AttributeAfter",m_pTextAttributeAfter);
	DDX_Control("ThinkingTime",m_pLabelTime);
	m_pImgOld = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Old"));
	m_pImgNew = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_New"));

	m_pLabelTime->SetText(_AL(""));

	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_OldLight"));
	if (pImg)
		m_pGfxLightLeft = pImg->GetGfx();
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_NewLight"));
	if (pImg)
		m_pGfxLightRight = pImg->GetGfx();
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Light"));
	if (pImg)
		m_pGfxCore = pImg->GetGfx();

	for (int i=1;i<5;i++)
	{
		AString str;
		str.Format("Img_Skills%d",i);
		DDX_Control(str,m_pImgSkill[i-1]);
		str.Format("Lbl_SName%d",i);
		DDX_Control(str,m_pSkillName[i-1]);
	}

	m_bMannualSel = false;
	ShowGfxWhenProducing();

	return true;
}
// 开始时只显示中间的大球gfx
void CDlgPreviewPetRebuild::ShowGfxWhenProducing()
{
	A3DGFXEx* pGfx;
	if (m_pImgOld)
	{
		pGfx = m_pImgOld->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pImgNew)
	{
		pGfx = m_pImgNew->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(false);
		}
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(false);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
}
// 显示两边的小球
void CDlgPreviewPetRebuild::ShowGfxWhenProduced()
{
	if (m_pImgOld)
	{
		m_pImgOld->SetGfx(ENERGY_BALL_SMALL);
		m_pImgOld->Resize();
	}
	if (m_pImgNew)
	{
		m_pImgNew->SetGfx(ENERGY_BALL_SMALL);
		m_pImgNew->Resize();
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(false);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
}
// 选择一边
void CDlgPreviewPetRebuild::ShowGfxWhenSelected(bool bNew)
{
	if (m_pImgOld)
	{
		m_pImgOld->SetGfx(bNew ? "" : ENERGY_BALL_MIDDLE);
		m_pImgOld->Resize();
	}
	if (m_pImgNew)
	{
		m_pImgNew->SetGfx(bNew ? ENERGY_BALL_MIDDLE : "");
		m_pImgNew->Resize();
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(bNew ? false : true);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(bNew ? true : false);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(true);
	}
	AnimationStart(); // 开始播闪电gfx
}
void CDlgPreviewPetRebuild::AnimationStart()
{
	m_iAnimationTickStart = GetTickCount();
	m_bAnimationPlaying = true;
}
void CDlgPreviewPetRebuild::AnimationTick()
{
	if (m_bAnimationPlaying)
	{
		if (CECTimeSafeChecker::ElapsedTimeFor(m_iAnimationTickStart) > ANIMATION_LAST_COUNT_MINISECONDS)
		{
			// 闪电gfx 停止
			m_bAnimationPlaying = false;
			if (m_pGfxLightLeft)
			{
				m_pGfxLightLeft->SetVisible(false);
			}
			if (m_pGfxLightRight)
			{
				m_pGfxLightRight->SetVisible(false);
			}
			if (m_pGfxCore)
			{
				m_pGfxCore->SetVisible(false);
			}			
		}
	}
}
void CDlgPreviewPetRebuild::OnShowDialog()
{
	m_pLabelTime->SetText(_AL(""));
}

void CDlgPreviewPetRebuild::OnTick()
{
	if (m_bThinking) // 思考计时
	{
		int leftTime = m_iStopThinkingTime - GetGame()->GetServerAbsTime();
		int leftTimeBackup = leftTime;
		a_ClampFloor(leftTime, 0);
		
		ACString strText;
		strText.Format(GetStringFromTable(9721), GetGameUIMan()->GetFormatTime(leftTime));
		m_pLabelTime->SetText(strText);
	}
	AnimationTick();

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIdx);
	bool bActive = pPetCorral->GetActivePetIndex()== m_iPetIdx;
	bool bInState = GetHostPlayer()->IsRebuildingPet();		
	m_pButtonReproduce->Enable(!bActive && !bInState);
}

void CDlgPreviewPetRebuild::OnCommandReproduce(const char *szCommand)
{
	return ; // 废弃

	if(IsNature())
		GetGameUIMan()->m_pDlgPetEvolution->OnCommand_RebuildNature(NULL); // 培训
	else if(IsInherit())
		GetGameUIMan()->m_pDlgPetEvolution->OnCommand_RebuildInherit(NULL); // 洗髓
	else
		ASSERT(0);

	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	m_pButtonReproduce->Enable(false);
	GetDlgItem("Lbl_Nature2")->SetText(_AL(""));
}

void CDlgPreviewPetRebuild::OnCommandSelectOldOne(const char *szCommand)
{
	if(IsNature())
		GetGameSession()->c2s_CmdPetRebuildNatureChoose(false);
	else if (IsInherit())
		GetGameSession()->c2s_CmdPetRebuildInheritChoose(false);

	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	m_pTextAttributeAfter->SetText(_AL(""));
	GetDlgItem("Close")->Enable(true);
	m_pImgSkill[2]->ClearCover();
	m_pImgSkill[2]->SetHint(_AL(""));
	m_pImgSkill[3]->ClearCover();
	m_pImgSkill[3]->SetHint(_AL(""));
	m_pSkillName[2]->SetText(_AL(""));
	m_pSkillName[3]->SetText(_AL(""));
	GetDlgItem("Lbl_Nature2")->SetText(_AL(""));

	m_bMannualSel = true;
	ShowGfxWhenSelected(false);
}

void CDlgPreviewPetRebuild::OnCommandSelectNewOne(const char *szCommand)
{
	if(IsNature())
		GetGameSession()->c2s_CmdPetRebuildNatureChoose(true);
	else if (IsInherit())
		GetGameSession()->c2s_CmdPetRebuildInheritChoose(true);

	m_bThinking = false;
	m_pLabelTime->SetText(_AL(""));
	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	m_pButtonReproduce->Enable(false);
	m_pTextAttributeBefore->SetText(_AL(""));
	
	GetDlgItem("Close")->Enable(true);
	m_pImgSkill[0]->ClearCover();
	m_pImgSkill[0]->SetHint(_AL(""));
	m_pImgSkill[1]->ClearCover();
	m_pImgSkill[1]->SetHint(_AL(""));
	m_pSkillName[0]->SetText(_AL(""));
	m_pSkillName[1]->SetText(_AL(""));
	GetDlgItem("Lbl_Nature1")->SetText(_AL(""));
	
	m_bMannualSel = true;
	ShowGfxWhenSelected(true);
}
void CDlgPreviewPetRebuild::OnCommandCANCEL(const char * szCommand)
{
	if(GetHostPlayer()->IsRebuildingPet()) 
		return;

	Show(false);
}
// 设置老属性
void CDlgPreviewPetRebuild::SetOldInfo(int idx, bool bNature)
{
	if(idx<0) 
	{
		ASSERT(FALSE);
		a_LogOutput(1,"CDlgPreviewPetRebuild::SetOldInfo, idx is %d", idx);
		return;
	}

	m_iType = bNature ? 0:1;
	m_iPetIdx = idx;

	m_pButtonSelectOld->Enable(false);
	m_pButtonSelectNew->Enable(false);
	m_pButtonReproduce->Enable(false);
	
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIdx);

	if(!pPet) 
	{
		ASSERT(FALSE);
		a_LogOutput(1,"CDlgPreviewPetRebuild::SetOldInfo, pet is not in corral!");
		return;
	}

	GetDlgItem("Close")->Enable(false);
	// 旧属性
	if (IsNature())
	{
		int i=0;

		ACString str;
		str.Format(GetStringFromTable(10103),_AL(""));
		GetDlgItem("Lbl_Nature2")->SetText(str);

		str.Format(GetStringFromTable(10103),pPet->GetNature());
		GetDlgItem("Lbl_Nature1")->SetText(str);		

		for (i=0;i<GP_PET_NATURE_SKILL_NUM + GP_PET_NATURE_SKILL_NUM;i++)
		{
			m_pImgSkill[i]->ClearCover();
			m_pImgSkill[i]->SetHint(_AL(""));
			m_pSkillName[i]->SetText(_AL(""));
		}

		for (i=0;i<GP_PET_NATURE_SKILL_NUM;i++)
		{			
			ACHAR szText[1024];
			int id = pPet->GetSkill(CECPetData::EM_SKILL_NATURE,i) ? pPet->GetSkill(CECPetData::EM_SKILL_NATURE,i)->idSkill : 0;
			int lvl = pPet->GetSkill(CECPetData::EM_SKILL_NATURE,i) ? pPet->GetSkill(CECPetData::EM_SKILL_NATURE,i)->iLevel : 0;

			if (id>0)
			{
				CECSkill::GetDesc(id, lvl, szText, 1024);
				m_pImgSkill[i]->SetHint(szText);
				
				AString strIcon = GNET::ElementSkill::GetIcon(id);
				AString strFile;
				af_GetFileTitle(strIcon, strFile);
				strFile.MakeLower();
				m_pImgSkill[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
				
				const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(id * 10);
				m_pSkillName[i]->SetText(szName ? szName:_AL(""));

				m_pImgSkill[2+i]->ClearCover();
				m_pImgSkill[2+i]->SetHint(_AL(""));
				m_pSkillName[2+i]->SetText(_AL(""));
			}
		}
		
		SwitchType(true);
	}
	else if (IsInherit())
	{	
		m_pTextAttributeBefore->SetText(_AL(""));

		ACString str;
		str.Format(GetStringFromTable(10104),pPet->GetAtkRation());
		m_pTextAttributeBefore->AppendText(str);
		str.Format(GetStringFromTable(10105),pPet->GetDefRation());
		m_pTextAttributeBefore->AppendText(str);
		str.Format(GetStringFromTable(10106),pPet->GetHpRation());
		m_pTextAttributeBefore->AppendText(str);
		str.Format(GetStringFromTable(10107),pPet->GetAtkLvlRation());
		m_pTextAttributeBefore->AppendText(str);
		str.Format(GetStringFromTable(10108),pPet->GetDefLvlRation());
		m_pTextAttributeBefore->AppendText(str);

		SwitchType(false);
	}

	m_pTextAttributeAfter->SetText(_AL(""));

	m_bMannualSel = false;

	//m_bInSession = true;
	GetHostPlayer()->SetRebuildPetFlag(true);
	// gfx
	ShowGfxWhenProducing();
}
// 设置新属性
void CDlgPreviewPetRebuild::SetPreviewInfo(void* pData)
{
	using namespace S2C;

	m_pButtonSelectNew->Enable(true);
	m_pButtonSelectOld->Enable(true);

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIdx);
	
	if(!pPet) return;

	// 新属性
	if (IsNature())
	{
		cmd_pet_rebuild_nature_info* pInfo = (cmd_pet_rebuild_nature_info*) pData;
		elementdataman* pDB = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		const void* pDBData = pDB->get_data_ptr(pInfo->nature, ID_SPACE_CONFIG, DataType);
		if (pDBData)
		{
			if (DataType == DT_PET_EVOLVED_SKILL_CONFIG)
			{
				const PET_EVOLVED_SKILL_CONFIG* pSkillConfig = (const PET_EVOLVED_SKILL_CONFIG*)pDBData;			
				if (pSkillConfig)
				{
					int i=0;
					ACString str;
					str.Format(GetStringFromTable(10103),pSkillConfig->name);
					GetDlgItem("Lbl_Nature2")->SetText(str);
					
					for (i=0;i<GP_PET_NATURE_SKILL_NUM;i++)
					{			
						ACHAR szText[1024] = {0};			

						CECSkill::GetDesc(pSkillConfig->skills[i].id, pSkillConfig->skills[i].level, szText, 1024);
						m_pImgSkill[2+i]->SetHint(szText);
						
						AString strIcon = GNET::ElementSkill::GetIcon(pSkillConfig->skills[i].id);
						AString strFile;
						af_GetFileTitle(strIcon, strFile);
						strFile.MakeLower();
						m_pImgSkill[2+i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
							GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);

						const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pSkillConfig->skills[i].id * 10);
						m_pSkillName[2+i]->SetText(szName ? szName:_AL(""));
					}
				}
			}		
		}	
	}
	else if (IsInherit())
	{
		cmd_pet_rebuild_inherit_info* pInfo = (cmd_pet_rebuild_inherit_info*) pData;
		PAUITEXTAREA text = dynamic_cast<PAUITEXTAREA>(GetDlgItem("AttributeAfter"));
		ACString str, strPostfix;

		// atk ration
		str.Format(GetStringFromTable(10104),pInfo->r_attack);
		int val = pInfo->r_attack - pPet->GetAtkRation();
		if(val>=0)
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10120), val);
		else
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10121), -val);
		str += strPostfix;
		text->AppendText(str);

		// defense ration
		str.Format(GetStringFromTable(10105),pInfo->r_defense);
		val = pInfo->r_defense - pPet->GetDefRation();
		if(val>=0)
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10120), val);
		else
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10121), -val);
		str += strPostfix;
		text->AppendText(str);

		// hp ration
		str.Format(GetStringFromTable(10106),pInfo->r_hp);
		val = pInfo->r_hp - pPet->GetHpRation();
		if(val>=0)
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10120), val);
		else
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10121), -val);
		str += strPostfix;
		text->AppendText(str);

		// attack level ration
		str.Format(GetStringFromTable(10107),pInfo->r_atk_lvl);
		val = pInfo->r_atk_lvl - pPet->GetAtkLvlRation();
		if(val>=0)
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10120), val);
		else
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10121), -val);
		str += strPostfix;
		text->AppendText(str);

		//defense level ration
		str.Format(GetStringFromTable(10108),pInfo->r_def_lvl);
		val = pInfo->r_def_lvl - pPet->GetDefLvlRation();
		if(val>=0)
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10120), val);
		else
			strPostfix.Format(GetGameUIMan()->GetStringFromTable(10121), -val);
		str += strPostfix;
		text->AppendText(str);
	}

	// gfx
	ShowGfxWhenProduced();
	
	// 开始思考
	m_bThinking = true; 
	// 截止思考时间
	m_iStopThinkingTime = GetGame()->GetServerAbsTime() + 30;
}

void CDlgPreviewPetRebuild::SwitchType(bool bNature)
{	
	GetDlgItem("AttributeBefore")->Show(!bNature);
	GetDlgItem("AttributeAfter")->Show(!bNature);
	
	GetDlgItem("Lbl_Nature1")->Show(bNature);
	GetDlgItem("Lbl_Nature2")->Show(bNature);

	for (int i=0;i<4;i++)
	{
		m_pImgSkill[i]->Show(bNature);
		m_pSkillName[i]->Show(bNature);
	}
	
	GetDlgItem("Lbl_Title")->SetText(GetStringFromTable(bNature ? 10109:10101));
	m_pButtonReproduce->SetText(GetStringFromTable(bNature ? 10113:10112));
}

void CDlgPreviewPetRebuild::Show(bool bShow, bool bModal /* = false */, bool bActive /* = true */)
{	
	AUIDialog::Show(bShow,bModal,bActive);	
}

void CDlgPreviewPetRebuild::OnProduceEnd()
{
	if(!m_bMannualSel)
		ThinkingTimeEnd(); // 结束思考

	GetGameUIMan()->m_pDlgPetEvolution->UpdateNature(true, false); // 更新性格

//	m_bInSession = false;
	GetHostPlayer()->SetRebuildPetFlag(false);
}

void CDlgPreviewPetRebuild::ThinkingTimeEnd()
{
	m_bThinking = false;
	m_pTextAttributeAfter->SetText(_AL(""));
	m_pLabelTime->SetText(GetStringFromTable(9720));
	m_pButtonSelectNew->Enable(false);
	m_pButtonSelectOld->Enable(false);
	m_pImgSkill[2]->ClearCover();
	m_pImgSkill[2]->SetHint(_AL(""));
	m_pImgSkill[3]->ClearCover();
	m_pImgSkill[3]->SetHint(_AL(""));
	m_pSkillName[2]->SetText(_AL(""));
	m_pSkillName[3]->SetText(_AL(""));
	GetDlgItem("Lbl_Nature2")->SetText(_AL(""));

	GetDlgItem("Close")->Enable(true);

	ShowGfxWhenSelected(false);
}
void CDlgPreviewPetRebuild::OnServerNotify(int cmd,void* pData)
{
	using namespace S2C;
	switch (cmd)
	{
	case PET_REBUILD_INHERIT_START:		
		{			
			const cmd_pet_rebuild_inherit_start * pCmd = (const cmd_pet_rebuild_inherit_start *)pData;	
			m_iUseTime = pCmd->use_time;
			m_iPetIdx = (int)pCmd->index;

			// 打开属性选择界面		
			Show(true);
			SetOldInfo(m_iPetIdx,false);	

			ASSERT(m_iType==1);

			//test
	//		GetHostPlayer()->GetMoveControl().SendStopMoveCmd();
			
			break;
		}
	case PET_REBUILD_INHERIT_INFO:
		{
			ASSERT(m_iType==1);	
			SetPreviewInfo(pData);

			break;
		}
	case PET_REBUILD_INHERIT_END:
		OnProduceEnd();
		break;
	case PET_EVOLUTION_DONE:
		{
			const cmd_pet_evolution_done* pCmd = (const cmd_pet_evolution_done*)pData;
			m_iPetIdx = (int)pCmd->index;

			CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
			CECPetData * pPet = pPetCorral->GetPetData(m_iPetIdx);

			// 打开动画界面
			if (pPet)
			{
				CDlgPetEvoAnim* pAnim = GetGameUIMan()->m_pDlgPetEvoAnim;
				CDlgPetEvolution* pEvo = GetGameUIMan()->m_pDlgPetEvolution;
				int id = pEvo->GetCurPetId();
				pAnim->SetPet(id,CECPetData::GetEvolutionID(id));
				pAnim->Show(true);
			}
			break;
		}
	case PET_REBUILD_NATURE_START:
		{			
			const cmd_pet_rebuild_nature_start* pCmd = (const cmd_pet_rebuild_nature_start*)pData;
			m_iUseTime = pCmd->use_time;
			m_iPetIdx = (int)pCmd->index;
			// 打开属性选择界面	
			Show(true);
			SetOldInfo(m_iPetIdx,true);	
			
			ASSERT(m_iType==0);
			break;
		}
	case PET_REBUILD_NATURE_INFO:
		{
			SetPreviewInfo(pData);
			break;
		}
	case PET_REBUILD_NATURE_END:
		OnProduceEnd();
		break;
	}
}
bool CDlgPreviewPetRebuild::OnChangeLayoutBegin()
{
	memset(m_bGfxVisible,0,sizeof(m_bGfxVisible));

	A3DGFXEx* pGfx;
	if (m_pImgOld)
	{
		pGfx = m_pImgOld->GetGfx();
		m_bGfxVisible[0] = false; // 可能被置空了
		memset(m_szGfxFileOld,0,sizeof(m_szGfxFileOld));
		if (pGfx)
		{
			strncpy(m_szGfxFileOld,pGfx->GetPath(),MAX_PATH);			
			m_bGfxVisible[0] = pGfx->IsVisible();
		}
	}
	if (m_pImgNew)
	{
		pGfx = m_pImgNew->GetGfx();
		m_bGfxVisible[1] = false; // 可能被置空了
		memset(m_szGfxFileNew,0,sizeof(m_szGfxFileNew));
		if (pGfx)
		{
			strncpy(m_szGfxFileNew,pGfx->GetPath(),MAX_PATH);				
			m_bGfxVisible[1] = pGfx->IsVisible();
		}
	}
	if (m_pGfxLightLeft)
	{
		m_bGfxVisible[2] = m_pGfxLightLeft->IsVisible();
	}
	if (m_pGfxLightRight)
	{
		m_bGfxVisible[3] = m_pGfxLightRight->IsVisible();
	}
	if (m_pGfxCore)
	{
		m_bGfxVisible[4] = m_pGfxCore->IsVisible();
	}

	return true;
}
// for notify user the layout changing is over
void CDlgPreviewPetRebuild::OnChangeLayoutEnd(bool bAllDone)
{
	A3DGFXEx* pGfx;
	if (m_pImgOld)
	{	
		m_pImgOld->SetGfx(m_szGfxFileOld+4);
		pGfx = m_pImgOld->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(m_bGfxVisible[0]);
		}
	}
	if (m_pImgNew)
	{
		m_pImgNew->SetGfx(m_szGfxFileNew+4);
		pGfx = m_pImgNew->GetGfx();
		if (pGfx)
		{
			pGfx->SetVisible(m_bGfxVisible[1]);
		}
	}
	if (m_pGfxLightLeft)
	{
		m_pGfxLightLeft->SetVisible(m_bGfxVisible[2]);
	}
	if (m_pGfxLightRight)
	{
		m_pGfxLightRight->SetVisible(m_bGfxVisible[3]);
	}
	if (m_pGfxCore)
	{
		m_pGfxCore->SetVisible(m_bGfxVisible[4]);
	}
}