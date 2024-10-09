// File		: DlgPetEvolution.cpp
// Creator	: Wang Yongdong
// Date		: 2012/11/26

#include "AFI.h"
#include "DlgPetEvolution.h"
#include "DlgInventory.h"
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
#include "EC_NPC.h "
#include "DlgPreviewPetRebuild.h"
#include "DlgPetEvolutionAnim.h"
#include "DlgPetDetail.h"
#include "DlgPetNature.h"
#include "EC_TimeSafeChecker.h"

#include <A3DSkin.h>

#define new A_DEBUG_NEW

#define INTERAL_LONG	618
#define INTERAL_SHORT	100

AUI_BEGIN_COMMAND_MAP(CDlgPetEvolution, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Feed",			OnCommand_AddExp)
AUI_ON_COMMAND("Btn_Nature",		OnCommand_RebuildNature)
AUI_ON_COMMAND("Btn_Wash",			OnCommand_RebuildInherit)
AUI_ON_COMMAND("Btn_PetSkillsTree", OnCommand_NatrueList)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetEvolution, CDlgBase)
AUI_ON_EVENT("Btn_ExpUp",	WM_LBUTTONDOWN,		OnEventLButtonDown_add)
AUI_ON_EVENT("Btn_ExpDown",	WM_LBUTTONDOWN,		OnEventLButtonDown_minus)
AUI_END_EVENT_MAP()

CDlgPetEvolution::CDlgPetEvolution()
: m_pModel(NULL)
, m_nAngle(30)
, m_idPet(0)
, m_iPetIndex(-1)
, m_pButtonPress(NULL)
, m_iAddExpItemNum(1)
, m_aabb(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0))
, m_dwStartTime(0)
, m_dwLastTime(0)
, m_iIntervalTime(0)
, m_bAdd(FALSE)
{
	m_pAtkProg = NULL;
	m_pDefProg = NULL;
	m_pAtkLvlProg = NULL;
	m_pDefLvlProg = NULL;
	m_pHpProg = NULL;
	m_pImg_Skill[0] = NULL;
	m_pImg_Skill[1] = NULL;
	m_pComboWash = NULL;
	m_pComboNature = NULL;

	m_pBtnAddExp = NULL;
	m_pBtnEvo = NULL;
	m_pBtnNature = NULL;

	m_pBtn_LeftTurn = NULL;
	m_pBtn_RightTurn = NULL;
}

CDlgPetEvolution::~CDlgPetEvolution()
{
	A3DRELEASE(m_pModel);
}

void CDlgPetEvolution::Reset()
{
	A3DRELEASE(m_pModel);
	m_nAngle = 30;
	m_idPet = 0;
	m_aabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));
	m_iPetIndex = -1;

//	UpdateRenderCallback();

	m_pComboNature->ResetContent();
	m_pComboNature->SetCurSel(-1);
	m_pComboWash->ResetContent();
	m_pComboWash->SetCurSel(-1);

	UpdateNature(true, true); // 更新性格
}

void CDlgPetEvolution::UpdateRenderCallback()
{	
	if (m_pImg_Pet)
		m_pImg_Pet->SetRenderCallback(NPCRenderDemonstration, (DWORD)m_pModel, m_nAngle, (DWORD)&m_aabb);
}

bool CDlgPetEvolution::OnInitDialog()
{
	m_pImg_Pet = (PAUIIMAGEPICTURE)GetDlgItem("Img_Model");
	m_pExpProg = (PAUIPROGRESS)GetDlgItem("Pro_Exp");

	m_pAtkProg = (PAUIPROGRESS)GetDlgItem("Pro_Damage");
	m_pDefProg = (PAUIPROGRESS)GetDlgItem("Pro_Def");
	m_pAtkLvlProg = (PAUIPROGRESS)GetDlgItem("Pro_DamLv");
	m_pDefLvlProg = (PAUIPROGRESS)GetDlgItem("Pro_DefLv");
	m_pHpProg = (PAUIPROGRESS)GetDlgItem("Pro_Hp");

	m_pImg_Skill[0] = (PAUIIMAGEPICTURE)GetDlgItem("Img_Nature1");
	m_pImg_Skill[1] = (PAUIIMAGEPICTURE)GetDlgItem("Img_Nature2");
	m_pComboWash = (PAUICOMBOBOX)GetDlgItem("Cob_Wash");
	m_pComboNature = (PAUICOMBOBOX)GetDlgItem("Cob_Nature");

	DDX_Control("Btn_Feed",m_pBtnAddExp);
	DDX_Control("Btn_Wash",m_pBtnEvo);
	DDX_Control("Btn_Nature",m_pBtnNature);

	DDX_Control("Btn_Left",m_pBtn_LeftTurn);
	DDX_Control("Btn_Right",m_pBtn_RightTurn);

	return true;
}
void CDlgPetEvolution::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	
	DDX_EditBox(bSave,"Edt_Inputno", m_iAddExpItemNum);
}

CECModel * CDlgPetEvolution::LoadPetModel(int idPet)
{
	CECModel *pModel = NULL;
	if (idPet > 0)
	{
		DATA_TYPE DataType;
		const PET_ESSENCE* pEssence = (const PET_ESSENCE*)GetGame()->GetElementDataMan()->get_data_ptr(idPet, ID_SPACE_ESSENCE, DataType);
		if (DataType == DT_PET_ESSENCE && pEssence != NULL && pEssence->file_model[0])
		{
			if (CECPlayer::LoadPetModel(pEssence->file_model, &pModel))
			{				
				pModel->SetPos(A3DVECTOR3(0));
				pModel->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
				pModel->SetAutoUpdateFlag(false);
			}
		}
	}

	return pModel;
}

void CDlgPetEvolution::OnShowDialog()
{
	m_nAngle = 0;
}
void CDlgPetEvolution::OnHideDialog()
{
	if (GetGameUIMan()->m_pDlgPetPreviewRebuild->IsShow()) // 正在培训或者洗髓
	{
		if (GetHostPlayer()->IsRebuildingPet())
		{
			ASSERT(FALSE);
			a_LogOutput(1,"CDlgPetEvolution::OnHideDialog(), error %d", m_iPetIndex);
			GetGameSession()->c2s_CmdCancelAction();
		}
		GetGameUIMan()->m_pDlgPetPreviewRebuild->Show(false);
	}
	if (GetGameUIMan()->m_pDlgPetNature->IsShow())
	{
		GetGameUIMan()->m_pDlgPetNature->Show(false);
	}
}

void CDlgPetEvolution::OnTick()
{
	// rotate model
	if (m_pBtn_RightTurn && m_pBtn_RightTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nAngle = (m_nAngle + 360 - GetGame()->GetTickTime() / 6 ) % 360;
	}
	else if (m_pBtn_LeftTurn && m_pBtn_LeftTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nAngle = (m_nAngle + GetGame()->GetTickTime() / 6) % 360;
	}

	if (m_pModel)
	{
		const char *szName = CECNPC::GetBaseActionName(CECNPC::ACT_STAND);
		m_pModel->PlayActionByName(szName, 1.0f, false);
		m_pModel->Tick(GetGame()->GetTickTime());
		UpdateRenderCallback();
	}

	// 喂养数量调整
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

	UpdateUI();
}

void CDlgPetEvolution::OnCommand_CANCEL(const char * szCommand)
{
	if (GetHostPlayer()->IsRebuildingPet()) // 正在培训或者洗髓
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10119), MB_OK, 
			A3DCOLORRGBA(255, 255, 255, 160), NULL);
		return;
	}
	Show(false);
}
void CDlgPetEvolution::ComputeAABB(CECModel* pModel, A3DAABB& aabb)
{
	if(!pModel) return;	

	aabb = pModel->GetModelAABB(); // 可能是bone的aabb，所以有时候aabb会小于mesh的aabb，导致处于视锥外面
	
	// mesh的aabb
	for (int i=0;i<pModel->GetSkinNum();i++)
	{
		A3DAABB a = pModel->GetA3DSkin(i)->GetInitMeshAABB();
		aabb.Merge(a);
	}

	aabb.CompleteMinsMaxs();
}
void CDlgPetEvolution::SetPet(int tid,int index)
{
	if ( index == m_iPetIndex && tid == m_idPet)	
		return;
	
	//	释放现有模型
	Reset();

	m_idPet = tid;
	m_iPetIndex = index;

	CECModel *pModel = LoadPetModel(tid);
	if (pModel)	
	{
		m_pModel = pModel;
		const char *szName = CECNPC::GetBaseActionName(CECNPC::ACT_STAND); // 循环动作
		m_pModel->PlayActionByName(szName,1.0f,false);
		if (tid == 33908 || tid == 11952 || tid == 37902) // 帝骨、冰石行者刺棘蜂、刺棘蜂
		{
			ComputeAABB(m_pModel,m_aabb);
		}
		else
		{
			m_pModel->Tick(0);
			m_aabb = m_pModel->GetModelAABB();
		}		
	}

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	if(pPet)
		GetDlgItem("Txt_Name")->SetText(pPet->GetName());

	PrepareUI();

	m_iAddExpItemNum = 1; // 喂养物品数量

	UpdateData(false);

	UpdateRenderCallback();
}
// 进化相关界面
void CDlgPetEvolution::UpdateEvolution(bool bShow)
{
	GetDlgItem("Lbl_Msg")->Show(bShow);

	GetDlgItem("Cob_Wash")->Show(bShow);
	GetDlgItem("Btn_Wash")->Show(bShow);

	if (bShow)
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);

		if (pPet && pPet->GetPetEssence() && pPetCorral->GetPetEvoConfig())
		{
			m_pComboWash->ResetContent();

			const int (*p)[2] = pPetCorral->GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_evolve;
			for (int i=0;i<3;i++)
			{
				int item1 = p[i][0];
				int item2 = p[i][1];
				
				ACString str;
				str.Format(GetStringFromTable(10100),item1,item2);
				m_pComboWash->AddString(str);
			}			
			m_pComboWash->SetCurSel(0);
		}	

		GetDlgItem("Btn_Wash")->SetText(GetStringFromTable(10102));
	}
}
// 性格培训相关
void CDlgPetEvolution::UpdateNature(bool bShow,bool bResetCombo)
{
	GetDlgItem("Lbl_Nature")->Show(bShow);
	GetDlgItem("Txt_Nature")->Show(bShow);
	GetDlgItem("Cob_Nature")->Show(bShow);
	GetDlgItem("Img_Nature1")->Show(bShow);
	GetDlgItem("Img_Nature2")->Show(bShow);
	GetDlgItem("Btn_Nature")->Show(bShow);

	GetDlgItem("Btn_PetSkillsTree")->Show(bShow);

	m_pImg_Skill[0]->ClearCover();
	m_pImg_Skill[1]->ClearCover();
	m_pImg_Skill[0]->SetHint(_AL(""));
	m_pImg_Skill[1]->SetHint(_AL(""));

	if (bShow &&m_iPetIndex>=0)
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
		int i=0;
		if (pPet)
		{
			ACHAR szText[1000];
			for (i=0;i<GP_PET_NATURE_SKILL_NUM;i++) // 性格技能
			{
				const CECPetData::PETSKILL *pSkill = pPet->GetSkill(CECPetData::EM_SKILL_NATURE,i);
				
				if( pSkill && pSkill->idSkill != 0 )
				{
					CECSkill::GetDesc(pSkill->idSkill, pSkill->iLevel, szText, 1000);
					m_pImg_Skill[i]->SetHint(szText);
					const char* szIcon = GNET::ElementSkill::GetIcon(pSkill->idSkill);
					AString strFile;
					af_GetFileTitle(szIcon, strFile);
					strFile.MakeLower();
					m_pImg_Skill[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
				}
			}		
			
			if (bResetCombo)
			{
				m_pComboNature->ResetContent();
				
				const int (*p)[2] = pPetCorral->GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_rand_skill;
				for (i=0;i<3;i++)
				{
					int item1 = p[i][0];
					int item2 = p[i][1];
					
					ACString str;
					str.Format(GetStringFromTable(10100),item1,item2);
					m_pComboNature->AddString(str);
				}	
				m_pComboNature->SetCurSel(0);
			}
		}
	}
}
// 洗髓相关
void CDlgPetEvolution::UpdateInherit(bool bShow)
{
	GetDlgItem("Lbl_Score")->Show(bShow);

	GetDlgItem("Lbl_Damage")->Show(bShow);
	GetDlgItem("Pro_Damage")->Show(bShow);
	GetDlgItem("Img_DamageBg")->Show(bShow);
	
	GetDlgItem("Lbl_Def")->Show(bShow);
	GetDlgItem("Pro_Def")->Show(bShow);
	GetDlgItem("Img_DefBg")->Show(bShow);
	
	GetDlgItem("Lbl_Hp")->Show(bShow);
	GetDlgItem("Pro_Hp")->Show(bShow);
	GetDlgItem("Img_HpBg")->Show(bShow);
	
	GetDlgItem("Lbl_DamLv")->Show(bShow);
	GetDlgItem("Lbl_DefLv")->Show(bShow);
	GetDlgItem("Img_DamLvBg")->Show(bShow);
	
	GetDlgItem("Pro_DamLv")->Show(bShow);
	GetDlgItem("Pro_DefLv")->Show(bShow);
	GetDlgItem("Img_DefLvBg")->Show(bShow);
	
	GetDlgItem("Txt_Damage")->Show(bShow);
	GetDlgItem("Txt_Def")->Show(bShow);
	GetDlgItem("Txt_Hp")->Show(bShow);
	GetDlgItem("Txt_DamLv")->Show(bShow);
	GetDlgItem("Txt_DefLv")->Show(bShow);

	if (bShow)
	{
		GetDlgItem("Cob_Wash")->Show(bShow);
		GetDlgItem("Btn_Wash")->Show(bShow);

		m_pComboWash->ResetContent();

		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
		ACString str;
		if (pPet && pPet->GetPetEssence() && pPetCorral->GetPetEvoConfig())
		{
			const int (*p)[2] = pPetCorral->GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_inherit;
			for (int i=0;i<3;i++)
			{
				int item1 = p[i][0];
				int item2 = p[i][1];
				
				str.Format(GetStringFromTable(10100),item1,item2);
				m_pComboWash->AddString(str);
			}
			
			m_pComboWash->SetCurSel(0);
		}		
		
		GetDlgItem("Btn_Wash")->SetText(GetStringFromTable(10101));	
	}
}

void CDlgPetEvolution::UpdateUI()
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	if( pPet && pPet->GetClass() >= 0)
	{	
		ACString str;
		str.Format(_AL("%d"),pPet->GetLevel());
		GetDlgItem("Txt_Level")->SetText(str);
		
		str.Format(_AL("%d/%d"),pPet->GetExp(),pPet->GetMaxExp());
		GetDlgItem("Txt_Exp")->SetText(str);
		
		m_pExpProg->SetProgress((int)(100.0f *pPet->GetExp()/(float)pPet->GetMaxExp())); // 

		bool bActive = pPetCorral->GetActivePetIndex()== m_iPetIndex;

		if (pPet->GetClass() == GP_PET_CLASS_EVOLUTION) // 已进化
		{
			PET_ESSENCE* pE = pPet->GetPetEssence();
			str.Format(_AL("%d/%d"),pPet->GetAtkRation(),pE->attack_inherit_max_rate);
			GetDlgItem("Txt_Damage")->SetText(str);
			m_pAtkProg->SetRangeMax(pE->attack_inherit_max_rate);
			m_pAtkProg->SetProgress(pPet->GetAtkRation());
			
			str.Format(_AL("%d/%d"),pPet->GetDefRation(),pE->defence_inherit_max_rate);
			GetDlgItem("Txt_Def")->SetText(str);
			m_pDefProg->SetRangeMax(pE->defence_inherit_max_rate);
			m_pDefProg->SetProgress(pPet->GetDefRation());
			
			str.Format(_AL("%d/%d"),pPet->GetHpRation(),pE->hp_inherit_max_rate);
			GetDlgItem("Txt_Hp")->SetText(str);
			m_pHpProg->SetRangeMax(pE->hp_inherit_max_rate);
			m_pHpProg->SetProgress(pPet->GetHpRation());
			
			str.Format(_AL("%d/%d"),pPet->GetAtkLvlRation(),pE->attack_level_inherit_max_rate);
			GetDlgItem("Txt_DamLv")->SetText(str);
			m_pAtkLvlProg->SetRangeMax(pE->attack_level_inherit_max_rate);
			m_pAtkLvlProg->SetProgress(pPet->GetAtkLvlRation());
			
			str.Format(_AL("%d/%d"),pPet->GetDefLvlRation(),pE->defence_level_inherit_max_rate);
			GetDlgItem("Txt_DefLv")->SetText(str);
			m_pDefLvlProg->SetRangeMax(pE->defence_level_inherit_max_rate);
			m_pDefLvlProg->SetProgress(pPet->GetDefLvlRation());

			// 性格部分
			GetDlgItem("Txt_Nature")->SetText(pPet->GetNature());

			GetDlgItem("Btn_Nature")->Enable(!bActive && !GetHostPlayer()->IsRebuildingPet());			
			GetDlgItem("Btn_Wash")->Enable(!bActive && !GetHostPlayer()->IsRebuildingPet());

			if (bActive && !GetHostPlayer()->IsRebuildingPet())
			{
				m_pBtnEvo->SetHint(GetGameUIMan()->GetStringFromTable(10116));
				m_pBtnNature->SetHint(GetGameUIMan()->GetStringFromTable(10117));
			}
			else
			{
				m_pBtnEvo->SetHint(GetGameUIMan()->GetStringFromTable(10123));
				m_pBtnNature->SetHint(GetGameUIMan()->GetStringFromTable(10124));
			}

		}	
		else if (pPet->CanEvolution()) // 可进化
		{
			GetDlgItem("Btn_Wash")->Enable(pPet->GetLevel()>=100 && !bActive); // 进化按钮
			if (bActive)
			{
				m_pBtnEvo->SetHint(GetGameUIMan()->GetStringFromTable(10118));
			}
			else
			{
				m_pBtnEvo->SetHint(_AL(""));
			}
		}
		
		EnableAddExp(bActive); // 喂养UI
	}
}
void CDlgPetEvolution::PrepareUI()
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();

	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	if( pPet && pPet->GetClass() >= 0)
	{
		if (pPet->GetClass() == GP_PET_CLASS_EVOLUTION) // 已进化
		{
			UpdateEvolution(false);
			UpdateNature(true);
			UpdateInherit(true);
		}
		else if (pPet->CanEvolution()) // 可进化
		{
			UpdateEvolution(true);
			UpdateNature(false);
			UpdateInherit(false);
		}
		else // 不能进化
		{
			UpdateEvolution(false);
			UpdateNature(false);
			UpdateInherit(false);
		}
		// 喂养
		EnableAddExp(pPetCorral->GetActivePetIndex()== m_iPetIndex);
	}
}
void CDlgPetEvolution::EnableAddExp(bool bCan)
{
	m_pBtnAddExp->Enable(bCan);
	m_pBtnAddExp->SetHint(GetGameUIMan()->GetStringFromTable(!bCan ? 10115:10122));

	GetDlgItem("Edt_Inputno")->Enable(bCan);
	GetDlgItem("Btn_ExpDown")->Enable(bCan);
	GetDlgItem("Btn_ExpUp")->Enable(bCan);

}
// 喂养
void CDlgPetEvolution::OnCommand_AddExp(const char * szCommand)
{
	ACString str = GetDlgItem("Edt_Inputno")->GetText();
	
	int num = str.ToInt();
	if(num<=0) return;

	if(GetHostPlayer()->GetPack()->GetItemTotalNum(37401)>=num)
		GetGameSession()->c2s_CmdPetAddExp(m_iPetIndex,num);
	else
		GetGameUIMan()->MessageBox("",GetStringFromTable(10110),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
}
// 培训
void CDlgPetEvolution::OnCommand_RebuildNature(const char * szCommand)
{
	int iSel = m_pComboNature->GetCurSel();

	GetHostPlayer()->RebuildPet(m_iPetIndex,iSel,true);
}

void CDlgPetEvolution::OnCommand_RebuildInherit(const char * szCommand)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	if( pPet && pPet->GetClass() >= 0)
	{		
		if (pPet->GetClass() == GP_PET_CLASS_EVOLUTION) // 洗髓
		{
			int iSel = m_pComboWash->GetCurSel();

			GetHostPlayer()->RebuildPet(m_iPetIndex,iSel,false);
		}
		else if (pPet->CanEvolution()) //  进化
		{
			int iSel = m_pComboWash->GetCurSel();

			if(!pPetCorral->CheckRebuildPetItemCond(m_iPetIndex,iSel,0))
				return;			
			
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_PetEvolution", GetGameUIMan()->GetStringFromTable(10114), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);

			pMsgBox->SetData(iSel);		
		}	
	}
}
void CDlgPetEvolution::OnSendEvolutonCmd(int iSel)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	// 关闭进化前战宠属性界面
	if( pPet && pPet->GetClass() >= 0)
	{
		char szText[20];
		sprintf(szText, "Win_PetDetail%d", pPet->GetClass());
		CDlgPetDetail *pDlgDetail = (CDlgPetDetail *)GetGameUIMan()->GetDialog(szText);
		if(pDlgDetail)
			pDlgDetail->Show(false);
	}
	
	GetGameSession()->c2s_CmdPetEvolution(m_iPetIndex,iSel);
	
	// 关闭本界面
	Show(false);
}

// 喂养物品
void CDlgPetEvolution::OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	if (pPetCorral->GetActivePetIndex()!= m_iPetIndex)
		return;

	PropertyAdd(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = true;
	
}
void CDlgPetEvolution::OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	if (pPetCorral->GetActivePetIndex()!= m_iPetIndex)
		return;

	PropertyMinus(pObj);
	m_dwStartTime = GetTickCount();
	m_dwLastTime = m_dwStartTime;
	m_iIntervalTime = INTERAL_LONG;
	m_pButtonPress = pObj;
	m_bAdd = false;
}

void CDlgPetEvolution::PropertyAdd(AUIObject *pObj)
{
	UpdateData();
	m_iAddExpItemNum++;

	int num = GetHostPlayer()->GetPack()->GetItemTotalNum(37401);
	if(m_iAddExpItemNum>num) m_iAddExpItemNum = num;

	UpdateData(false);
}
void CDlgPetEvolution::PropertyMinus(AUIObject *pObj)
{
	UpdateData();

	m_iAddExpItemNum--;

	if(m_iAddExpItemNum<1) m_iAddExpItemNum = 1;

	UpdateData(false);
}
void CDlgPetEvolution::OnCommand_NatrueList(const char* szCommand)
{
	GetGameUIMan()->m_pDlgPetNature->SetPetIndex(m_iPetIndex);
	if(!GetGameUIMan()->m_pDlgPetNature->IsShow())
		GetGameUIMan()->m_pDlgPetNature->Show(true);
}