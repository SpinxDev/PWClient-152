#include "DlgFortressStatus.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressStatus, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressStatus, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFortressStatus::CDlgFortressStatus()
{
	m_pLab_GuildName = NULL;
	m_pTxt_Level = NULL;
	m_pPro_Exp = NULL;
	m_pLab_Exp = NULL;
	m_pLab_Health = NULL;
	m_pPro_Health = NULL;
	m_pTxt_FortressLevel = NULL;
	m_pLab_CumulateContrib = NULL;
	m_pLab_ConsumeContrib = NULL;
	m_pLab_ExpContrib = NULL;
}

bool CDlgFortressStatus::OnInitDialog()
{
	DDX_Control("Lab_GuildName", m_pLab_GuildName);
	DDX_Control("Txt_Level", m_pTxt_Level);
	DDX_Control("Pro_Exp", m_pPro_Exp);
	DDX_Control("Lab_Exp", m_pLab_Exp);
	DDX_Control("Lab_Health", m_pLab_Health);
	DDX_Control("Pro_Health", m_pPro_Health);
	DDX_Control("Txt_FortressLevel", m_pTxt_FortressLevel);
	DDX_Control("Lab_CumulateContrib", m_pLab_CumulateContrib);
	DDX_Control("Lab_ConsumeContrib", m_pLab_ConsumeContrib);	
	DDX_Control("Lab_ExpContrib", m_pLab_ExpContrib);
	return true;
}

void CDlgFortressStatus::OnShowDialog()
{
	//	显示最新结果
	UpdateInfo();
	
	//	发送协议更新帮派基地信息
	GetGame()->GetGameSession()->c2s_CmdGetFactionFortressInfo();
}

void CDlgFortressStatus::UpdateInfo()
{
	m_pLab_GuildName->SetText(_AL(""));
	m_pTxt_Level->SetText(_AL(""));

	m_pLab_Exp->SetText(_AL(""));
	m_pPro_Exp->SetProgress(0);
	m_pLab_Health->SetText(_AL(""));
	m_pPro_Health->SetProgress(0);
	m_pTxt_FortressLevel->SetText(_AL(""));

	m_pLab_CumulateContrib->SetText(_AL(""));
	m_pLab_ConsumeContrib->SetText(_AL(""));
	m_pLab_ExpContrib->SetText(_AL(""));

	CECHostPlayer *pHost = GetHostPlayer();
	int idMyFaction = pHost->GetFactionID();
	if (idMyFaction > 0)
	{
		ACString strText;

		//	帮派信息
		const Faction_Info *pFInfo = GetGame()->GetFactionMan()->GetFaction(idMyFaction);
		if (pFInfo)
		{
			//	帮派名称
			m_pLab_GuildName->SetText(pFInfo->GetName());
			
			//	帮派等级
			strText.Format(_AL("%d"), pFInfo->GetLevel() + 1);
			m_pTxt_Level->SetText(strText);
		}

		//	贡献度信息
		const CECHostPlayer::CONTRIB_INFO *pCInfo = pHost->GetContribInfo();
		if (pCInfo)
		{
			strText.Format(_AL("%d"), pCInfo->cumulate_contrib);
			m_pLab_CumulateContrib->SetText(strText);
			
			strText.Format(_AL("%d"), pCInfo->consume_contrib);
			m_pLab_ConsumeContrib->SetText(strText);
			
			strText.Format(_AL("%d"), pCInfo->exp_contrib);
			m_pLab_ExpContrib->SetText(strText);
		}

		//	基地信息
		const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = pHost->GetFactionFortressInfo();
		if (pInfo)
		{
			//	等级
			strText.Format(GetStringFromTable(9120), pInfo->level);
			m_pTxt_FortressLevel->SetText(strText);

			//	经验值
			const FACTION_FORTRESS_CONFIG *pConfig = pHost->GetFactionFortressConfig();
			if (pConfig)
			{
				//	经验值
				int expLevelup = pConfig->level[pInfo->level-1].exp;
				strText.Format(_AL("%d/%d"), pInfo->exp, expLevelup);
				m_pLab_Exp->SetText(strText);
				
				int nProgress(0);
				if (expLevelup > 0)
					nProgress = (int)(pInfo->exp/(float)expLevelup*100);
				else
					nProgress = 100;
				m_pPro_Exp->SetProgress(nProgress);
			}

			//	健康度
			int nMaxHealth = 150;
			strText.Format(_AL("%d/%d"), pInfo->health, nMaxHealth);
			m_pLab_Health->SetText(strText);

			int nProgress = (int)(pInfo->health/(float)nMaxHealth*100);
			m_pPro_Health->SetProgress(nProgress);
		}
	}
}