
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
	m_counter.SetPeriod(3600*1000);		//	每小时检查一次
	m_counter.Reset(true);							//	首次便进行检查
}

void CECGuildDiplomacyReminder::OnPrtcFactionListRelation_Re(GNET::FactionListRelation_Re *pProtocol)
{
	//	来自帮派外交界面的请求返回、或者此类中的请求返回
	//
	if (!pProtocol)
		return;

	size_t i(0);

	//	拷贝关注的请求
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

	//	与原有需求比较
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
					//	不是新的请求
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				//	是新的请求
				bNewApply = true;
				break;
			}
		}

		if (bNewApply)
		{
			//	新发的协议中有新的请求
			CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUIMan)
			{
				CDlgGuildDiplomacyMan *pDlg = (CDlgGuildDiplomacyMan *)pGameUIMan->GetDialog("Win_GuildDiplomacyMan");
				if (!pDlg->IsShow())
				{
					//	显示相应悬浮提示，提示玩家有新的帮派外交请求，打开帮派外交界面进行查看
					ACString strMsg = pGameUIMan->GetStringFromTable(9204);
					pGameUIMan->AddInformation(CDlgInfo::INFO_SYSTEM, "GDiplomacy_NewApplyNotify", strMsg, 0xFFFFFFF, 0, 0, 0);
				}
			}
		}
	}

	//	存储供下次比较使用
	m_apply = apply;
}

void CECGuildDiplomacyReminder::Tick(DWORD dwDeltaTime)
{
	//	检查玩家信息是否已完整
	if (!g_pGame->GetGameRun()->GetHostPlayer()->HostIsReady())
		return;

	//	定时向服务器发送查询协议（帮派外交界面打开时，也会向服务器发送此协议）
	if (m_counter.IncCounter(dwDeltaTime))
	{
		m_counter.Reset();
		g_pGame->GetGameSession()->faction_get_relationlist();
	}
}