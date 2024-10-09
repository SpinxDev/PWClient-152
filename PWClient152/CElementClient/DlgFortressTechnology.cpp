#include "DlgFortressTechnology.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "../../CElementData/ExpTypes.h"
#include "Network\\ids.hxx"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressTechnology, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_*",	OnCommand_Upgrade)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressTechnology, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFortressTechnology::CDlgFortressTechnology()
{
}

bool CDlgFortressTechnology::OnInitDialog()
{
	m_pLab_Technology = GetDlgItem("Lab_Technology");

	int i(0);
	AString strName;
	for (i = 0; i < TECHNOLOGY_COUNT; ++i)
	{
		strName.Format("Lab_%02d", i+1);
		m_pLab_Tech[i] = GetDlgItem(strName);
		
		strName.Format("Img_%02d", i+1);
		m_pImg_Tech[i] = (PAUIIMAGEPICTURE)GetDlgItem(strName);
	}
	return true;
}

void CDlgFortressTechnology::OnShowDialog()
{
	UpdateInfo();
}

void CDlgFortressTechnology::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);	
}

void CDlgFortressTechnology::OnCommand_Upgrade(const char *szCommand)
{
	if (!GetGameUIMan()->m_pCurNPCEssence ||
		!(GetGameUIMan()->m_pCurNPCEssence->combined_services & 0x10000000) ||
		!GetHostPlayer()->GetFactionID() ||
		GetHostPlayer()->GetFRoleID() != GNET::_R_MASTER)
	{
		//	只有帮主在基地的第2个NPC基地服务处可加科技点，
		return;
	}

	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_FORTRESS_CONFIG * pConfig = GetHostPlayer()->GetFactionFortressConfig();
	if (!pInfo || !pConfig)
		return;

	int iTech = atoi(szCommand + strlen("Btn_")) -1;
	int iTechLevel = pInfo->technology[iTech];
	const int MAX_TECH_LEVEL = sizeof(pConfig->tech_point_cost[0])/sizeof(pConfig->tech_point_cost[0][0]);
	if (iTechLevel >= MAX_TECH_LEVEL)
	{
		//	无法继续升级
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9128));
		return;
	}
	int nTechPointNeeded = pConfig->tech_point_cost[iTech][iTechLevel];
	if (pInfo->tech_point < nTechPointNeeded)
	{
		//	点数不够
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9121));
		return;
	}

	//	发送协议
	ACString strMsg;
	strMsg.Format(GetStringFromTable(9125), nTechPointNeeded);
	PAUIDIALOG pMsgBox = NULL;
	GetGameUIMan()->MessageBox("Fortress_TechLevelup", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(iTech);
}

void CDlgFortressTechnology::UpdateInfo()
{
	m_pLab_Technology->SetText(_AL(""));
	int i(0);
	for (i = 0; i < TECHNOLOGY_COUNT; ++ i)
	{	
		m_pLab_Tech[i]->SetText(_AL(""));
		m_pImg_Tech[i]->FixFrame(0);
	}

	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	if (pInfo)
	{
		ACString strText;
		strText.Format(_AL("%d"), pInfo->tech_point);
		m_pLab_Technology->SetText(strText);

		for (i = 0; i < TECHNOLOGY_COUNT; ++ i)
		{
			strText.Format(_AL("%d"), pInfo->technology[i]);
			m_pLab_Tech[i]->SetText(strText);
			m_pImg_Tech[i]->FixFrame(pInfo->technology[i]);
		}
	}
}