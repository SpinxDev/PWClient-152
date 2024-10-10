
#include "DlgGuildDiplomacyMan.h"
#include "DlgInfo.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GuildDiplomacyReminder.h"
#include "Network/factionlistrelation_re.hpp"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"

CECGuildDiplomacyReminder::CECGuildDiplomacyReminder()
{
	m_counter.SetPeriod(3600*1000);		//	ÿСʱ���һ��
	m_counter.Reset(true);							//	�״α���м��
}

void CECGuildDiplomacyReminder::OnPrtcFactionListRelation_Re(GNET::FactionListRelation_Re *pProtocol)
{
	//	���԰����⽻��������󷵻ء����ߴ����е����󷵻�
	//
	if (!pProtocol)
		return;

	size_t i(0);

	//	������ע������
	VApplyToMe apply;
	for (i = 0; i < pProtocol->apply.size(); ++ i)
	{
		const GNET::GFactionRelationApply &r = pProtocol->apply[i];
		switch(r.type)
		{
		case ALLIANCE_FROM_OTHER:
		case HOSTILE_FROM_OTHER:
		case REMOVE_RELATION_FROM_OTHER:
			apply.push_back(r);
			break;
		}
	}

	//	��ԭ������Ƚ�
	if (!apply.empty())
	{
		bool bNewApply(false);

		for (i = 0; i < apply.size(); ++ i)
		{
			const GNET::GFactionRelationApply &cur = apply[i];

			bool bFound(false);
			for (size_t j = 0; j < m_apply.size(); ++ j)
			{
				const GNET::GFactionRelationApply &old = m_apply[j];
				if (old.type == cur.type && 
					old.fid == cur.fid &&
					old.end_time == cur.end_time)
				{
					//	�����µ�����
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				//	���µ�����
				bNewApply = true;
				break;
			}
		}

		if (bNewApply)
		{
			//	�·���Э�������µ�����
			CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUIMan->GetDialog("Win_GuildDiplomacyMan");
				if (!pDlg->IsShow())
				{
					//	��ʾ��Ӧ������ʾ����ʾ������µİ����⽻���󣬴򿪰����⽻������в鿴
					ACString strMsg = pGameUIMan->GetStringFromTable(9204);
					pGameUIMan->AddInformation(CDlgInfo::INFO_SYSTEM, "GDiplomacy_NewApplyNotify", strMsg, 0xFFFFFFF, 0, 0, 0);
				}
			}
		}
	}

	//	�洢���´αȽ�ʹ��
	m_apply = apply;
}

void CECGuildDiplomacyReminder::Tick(DWORD dwDeltaTime)
{
	//	��������Ϣ�Ƿ�������
	if (!g_pGame->GetGameRun()->GetHostPlayer()->HostIsReady())
		return;

	//	��ʱ����������Ͳ�ѯЭ�飨�����⽻�����ʱ��Ҳ������������ʹ�Э�飩
	if (m_counter.IncCounter(dwDeltaTime))
	{
		m_counter.Reset();
		g_pGame->GetGameSession()->faction_get_relationlist();
	}
}