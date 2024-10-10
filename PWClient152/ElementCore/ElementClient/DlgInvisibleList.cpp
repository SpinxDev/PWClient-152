// Filename	: DlgInvisibleList.h
// Creator	: Xu Wenbin
// Date		: 2011/7/29

#include "AFI.h"
#include "DlgInvisibleList.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GPDataType.h"

#include <CSplit.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInvisibleList, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgInvisibleList, CDlgBase)
AUI_END_EVENT_MAP()

CDlgInvisibleList::CDlgInvisibleList()
{
	m_pLst_Player = NULL;

	m_dwOutOfDateMoment = 0;
}

bool CDlgInvisibleList::OnInitDialog()
{
	DDX_Control("Lst_Player", m_pLst_Player);

	if (m_pLst_Player == NULL)
		return false;

	return true;
}

void CDlgInvisibleList::OnTick()
{
	DWORD dwCurTime  = timeGetTime();
	if (dwCurTime > m_dwOutOfDateMoment)
	{
		if (!m_playerList.empty())
		{
			m_playerList.clear();
			UpdateUI();
		}
	}
	else
	{
		for (int i = 0; i < m_pLst_Player->GetCount(); ++ i)
			UpdateLine(m_pLst_Player, i);
	}

}

void CDlgInvisibleList::UpdateUI()
{
	m_pLst_Player->ResetContent();

	abase::vector<int> ids;
	for (PlayerList::iterator it = m_playerList.begin(); it != m_playerList.end(); ++ it)
	{
		const PlayerInfo &info = *it;

		if (AppendLine(m_pLst_Player, info.id))
			ids.push_back(info.id);
	}
	
	//	向服务器获取玩家名字
	if (!ids.empty())
		GetGameSession()->CacheGetPlayerBriefInfo((int)ids.size(), &ids[0], 2);
}

bool CDlgInvisibleList::AppendLine(PAUILISTBOX pLstBox, int id)
{
	bool bNeedGetPlayerName(false);

	ACString strText;
	
	ACString strName = GetGameRun()->GetPlayerName(id, false);
	if (strName.IsEmpty())
	{
		bNeedGetPlayerName = true;
		strName = _AL("-");
	}

	strText.Format(_AL("%s"), strName);
	int nItem = pLstBox->AddString(strText) -1;
	pLstBox->SetItemData(nItem, (DWORD)id, LST_INDEX_PLAYER);
	pLstBox->SetItemData(nItem, bNeedGetPlayerName ? 1 : 0, LST_INDEX_NEED_PLAYER);

	return bNeedGetPlayerName;
}

void CDlgInvisibleList::UpdateLine(PAUILISTBOX pLstBox, int line)
{
	//	更新玩家名称显示
	
	if (pLstBox->GetItemData(line, LST_INDEX_NEED_PLAYER))
	{
		int idPlayer = (int)pLstBox->GetItemData(line, LST_INDEX_PLAYER);
		ACString strName = GetGameRun()->GetPlayerName(idPlayer, false);
		if (!strName.IsEmpty())
		{
			pLstBox->SetItemData(line, 0, LST_INDEX_NEED_PLAYER);

			ACString strText = pLstBox->GetText(line);
			CSplit s(strText);
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));

			strText.Empty();
			for (size_t i = 0; i < vec.size(); ++ i)
			{
				strText += (i == LST_COLUMN_NAME) ? strName : vec[i];
				if (i +1 != vec.size())
					strText += _AL("\t");
			}

			pLstBox->SetText(line, strText);
		}
	}
}

void CDlgInvisibleList::UpdateList(const S2C::cmd_invisible_obj_list *pCmd)
{
	//	清除原有界面内容
	m_playerList.clear();
	PlayerInfo temp;
	for (size_t i = 0; i < pCmd->count; ++ i)
	{
		temp.id = pCmd->id_list[i];
		m_playerList.push_back(temp);
	}
	
	//	更新界面
	UpdateUI();

	//	设置数据失效时间，到时自动清除列表，以防止服务器不更新列表导致列表始终保持上次更新内容，误导玩家
	m_dwOutOfDateMoment = timeGetTime() + 4 * 1000;		//	服务器端为3秒钟，客户端适当延长

	if (!IsShow())
		Show(true);
}