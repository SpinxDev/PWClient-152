// File		: DlgPetNature.h
// Creator	: WYD
// Date		: 2013\03\08

#include "DlgPetNature.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"
#include "EC_HostPlayer.h"
#include "EC_Skill.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "AFI.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetNature, CDlgBase)

AUI_ON_COMMAND("Btn_PageUp",		OnCommandPre)
AUI_ON_COMMAND("Btn_PageDown",		OnCommandNext)
AUI_ON_COMMAND("Btn_Close",			OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgPetNature::CDlgPetNature():m_iCurPage(1),m_iPetIndex(-1)
{
	memset(m_pImgName,0,sizeof(m_pImgName));
	memset(m_pImgSkill,0,sizeof(m_pImgSkill));
	memset(m_pSkillName,0,sizeof(m_pSkillName));
	m_pLabelPage = NULL;

}

void CDlgPetNature::OnCommandNext(const char * szCommand)
{
	if (m_iCurPage < m_iMaxPage )
	{
		m_iCurPage++;
		UpateUI(m_iCurPage);
	}
}
void CDlgPetNature::OnCommandPre(const char * szCommand)
{
	if (m_iCurPage>1)
	{
		m_iCurPage--;
		UpateUI(m_iCurPage);
	}
}
void CDlgPetNature::OnCommandCancel(const char * szCommand)
{
	Show(false);
}
bool CDlgPetNature::OnInitDialog()
{
	AString str;
	for (int i=0;i<NATURE_NUM_PER_PAGE;i++)
	{
		str.Format("Lbl_Name%d",i+1);
		DDX_Control(str,m_pImgName[i]);

		int vice_index = i+NATURE_NUM_PER_PAGE; // 

		str.Format("Img_Icon%d",i+1);
		DDX_Control(str,m_pImgSkill[i]);
		str.Format("Img_Icon%d",vice_index + 1);
		DDX_Control(str,m_pImgSkill[vice_index]);

		str.Format("Lbl_SName%d",i+1);
		DDX_Control(str,m_pSkillName[i]);
		str.Format("Lbl_SName%d",vice_index+1);
		DDX_Control(str,m_pSkillName[vice_index]);
	}
	DDX_Control("Lbl_Page",m_pLabelPage);

	m_pNatureList = NULL;
	m_iMaxPage = 1;
	m_iSkillCount = 0;
	m_iCurPage = 1;

	return true;
}
void CDlgPetNature::OnShowDialog()
{
	if(m_iPetIndex<0) return ;
	
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();	
	CECPetData * pPet = pPetCorral->GetPetData(m_iPetIndex);
	// 初始化性格列表
	if( pPet && pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
	{
		PET_ESSENCE* pet_ess = pPet->GetPetEssence();
		if (pet_ess && pet_ess->id_evolved_skill_rand)
		{
			elementdataman* pDB = g_pGame->GetElementDataMan();
			
			DATA_TYPE DataType;
			const void* pDBData = pDB->get_data_ptr(pet_ess->id_evolved_skill_rand, ID_SPACE_CONFIG, DataType);
			if (pDBData && DataType == DT_PET_EVOLVED_SKILL_RAND_CONFIG)	
			{
				m_pNatureList = (PET_EVOLVED_SKILL_RAND_CONFIG*)pDBData;	
				m_iSkillCount = 0;
				for (int i=0;i< MAX_NATURE;i++)
				{
					if(m_pNatureList->rand_skill_group[0].list[i]) m_iSkillCount++;
				}

				m_iMaxPage = m_iSkillCount / NATURE_NUM_PER_PAGE + 1;
			}
		}
	}

	m_iCurPage = 1;
	// 更新性格技能信息
	UpateUI(m_iCurPage);
}
void CDlgPetNature::OnHideDialog()
{
	m_iPetIndex = -1;
}
void CDlgPetNature::UpateUI(int page)
{
	int i=0,j=0;

	// 清空界面信息
	for (i=0;i<NATURE_NUM_PER_PAGE;i++)
	{
		m_pImgName[i]->SetText(_AL(""));
		m_pImgSkill[i]->SetHint(_AL(""));
		m_pImgSkill[i]->ClearCover();
		m_pImgSkill[i]->Show(false);
		
		int vice_index = i+NATURE_NUM_PER_PAGE;

		m_pImgSkill[vice_index]->SetHint(_AL(""));
		m_pImgSkill[vice_index]->ClearCover();
		m_pImgSkill[vice_index]->Show(false);

		m_pSkillName[i]->SetText(_AL(""));
		m_pSkillName[vice_index]->SetText(_AL(""));

	}

	ACString strPage;
	strPage.Format(_AL("%d/%d"),page,m_iMaxPage);
	m_pLabelPage->SetText(strPage);

	if(page<1 || page>m_iMaxPage || !m_pNatureList) return;	
	
	// 取出技能列表，防止中间没技能的情况
	unsigned int skill_vec[MAX_NATURE] = {0};
	for (i=0;i<MAX_NATURE;i++)
	{
		int nature = m_pNatureList->rand_skill_group[0].list[i];
		if(nature)
		{
			skill_vec[j++] = nature;
		}
	}
	// 设置技能名字和悬浮, 性格名字
	int first_idx = NATURE_NUM_PER_PAGE * (page-1);
	for (i=0;i<NATURE_NUM_PER_PAGE;i++)
	{
		int skill_idx = first_idx + i;
		if(skill_idx >= m_iSkillCount) break;

		int nature = skill_vec[skill_idx];
		ASSERT(nature!=0);

		elementdataman* pDB = g_pGame->GetElementDataMan();
		
		DATA_TYPE DataType;
		const void* pDBData = pDB->get_data_ptr(nature, ID_SPACE_CONFIG, DataType);
		if (pDBData && DataType == DT_PET_EVOLVED_SKILL_CONFIG)
		{
			PET_EVOLVED_SKILL_CONFIG* pConfig = (PET_EVOLVED_SKILL_CONFIG*)pDBData;
			m_pImgName[i]->SetText(pConfig->name);

			for (j=0;j<2;j++)
			{
				ACHAR szText[1000];
				int img_idx = i*2+j;
				m_pImgSkill[img_idx]->Show(true);
				CECSkill::GetDesc(pConfig->skills[j].id, pConfig->skills[j].level, szText, 1000);
				m_pImgSkill[img_idx]->SetHint(szText);
				AString strIcon = GNET::ElementSkill::GetIcon(pConfig->skills[j].id);
				AString strFile;
				af_GetFileTitle(strIcon, strFile);
				strFile.MakeLower();
				m_pImgSkill[img_idx]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);	
				
				const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pConfig->skills[j].id * 10);
				m_pSkillName[img_idx]->SetText(szName ? szName:_AL(""));
			}		
		}
	}
}