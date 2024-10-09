// Filename	: DlgEarthBagRank.h
// Creator	: Xu Wenbin
// Date		: 2011/5/12

#include "AFI.h"
#include "DlgEarthBagRank.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_GPDataType.h"

#include <CSplit.h>

#define EARTHBAG_RANK_PAGE_SIZE 12

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEarthBagRank, CDlgBase)
AUI_ON_COMMAND("Btn_DPS", OnCommand_Btn_DPS)
AUI_ON_COMMAND("Btn_DPH", OnCommand_Btn_DPH)
AUI_ON_COMMAND("Btn_Number", OnCommand_SortBy_Rank)
AUI_ON_COMMAND("Btn_Value", OnCommand_SortBy_Rank)
AUI_ON_COMMAND("Btn_LV", OnCommand_SortBy_Level)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_ON_COMMAND("Rdo_*", OnCommand_RankList)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEarthBagRank, CDlgBase)
AUI_ON_EVENT("List_Rank", WM_RBUTTONUP, OnEventRButtonUp_Lst_Rank)
AUI_END_EVENT_MAP()

CDlgEarthBagRank::CDlgEarthBagRank()
{
	m_pLst_Rank = NULL;
	m_pLst_Self = NULL;
	m_pBtn_DPS = NULL;
	m_pBtn_DPH = NULL;
	m_pTxt_Time = NULL;

	m_curList = RANKLIST_DPH;
	m_curSort = SORT_BY_RANK;
    m_iCurProfession = NUM_PROFESSION;

	m_dwMinRefreshMoment = 0;
    memset( m_dwNextRefreshMoment,0,sizeof(DWORD)*(NUM_PROFESSION+1)*(RANKLIST_NUM) );
}

bool CDlgEarthBagRank::OnInitDialog()
{
	DDX_Control("List_Rank", m_pLst_Rank);
	DDX_Control("List_Self", m_pLst_Self);

	m_pBtn_DPS = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_DPS"));
	m_pBtn_DPH = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_DPH"));

	m_pTxt_Time = GetDlgItem("Txt_Time");

	if (m_pLst_Self == NULL ||
		m_pLst_Rank == NULL )
		return false;

	return true;
}

void CDlgEarthBagRank::OnShowDialog()
{
	UpdateUI();
}

void CDlgEarthBagRank::OnTick()
{
	DWORD dwCurTime = timeGetTime();

	if (!GetHostPlayer()->GetCoolTime(GP_CT_DPS_DPH_RANK) && dwCurTime>=m_dwMinRefreshMoment &&
        dwCurTime>=m_dwNextRefreshMoment[m_iCurProfession][m_curList])
	{
		//	清除旧有表格
        m_rankList[m_iCurProfession][m_curList].clear();
		
		//	更新界面
		UpdateUI();
		
		//	发送请求协议
        int rank_mask = 0;
        if( m_curList == RANKLIST_DPS ) rank_mask |= 0x20;
        if( m_iCurProfession != NUM_PROFESSION )
        {
            rank_mask |= 0x10;
            rank_mask |= m_iCurProfession;
        }
		GetGameSession()->c2s_CmdNPCSevGetDPSDPHRank(rank_mask);

		//	限制下次更新最短时间
		m_dwMinRefreshMoment = dwCurTime + 10*1000;
	}
	else
	{
		for (int i = 0; i < m_pLst_Rank->GetCount(); ++ i)
			UpdateLine(m_pLst_Rank, i);

		UpdateRefreshTime();
	}
}

void CDlgEarthBagRank::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

void CDlgEarthBagRank::OnCommand_Btn_DPS(const char *szCommand)
{
	if (m_curList  == RANKLIST_DPS)
		return;

	m_curList = RANKLIST_DPS;
	m_curSort = SORT_BY_RANK;
	UpdateUI();
}

void CDlgEarthBagRank::OnCommand_Btn_DPH(const char *szCommand)
{
	if (m_curList  == RANKLIST_DPH)
		return;

	m_curList = RANKLIST_DPH;
	m_curSort = SORT_BY_RANK;
	UpdateUI();
}

void CDlgEarthBagRank::OnCommand_RankList(const char* szCommand)
{
    int iProfession = atoi(szCommand+strlen("Rdo_"));
    if( iProfession != m_iCurProfession )
    {
        m_iCurProfession = iProfession;
        UpdateUI();
    }
}

void CDlgEarthBagRank::OnCommand_SortBy_Rank(const char *szCommand)
{
	m_curSort = SORT_BY_RANK;
	m_pLst_Rank->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, LST_COLUMN_RANK);
}

void CDlgEarthBagRank::OnCommand_SortBy_Level(const char *szCommand)
{
	m_curSort = SORT_BY_LEVEL;
	m_pLst_Rank->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, LST_COLUMN_LEVEL);
}

void CDlgEarthBagRank::OnEventRButtonUp_Lst_Rank(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	int nCurSel = m_pLst_Rank->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pLst_Rank->GetCount() &&
		m_pLst_Rank->GetHitArea(x, y) == AUILISTBOX_RECT_TEXT )
	{
		int idPlayer = m_pLst_Rank->GetItemData(nCurSel, LST_INDEX_PLAYER);
		if (idPlayer != GetHostPlayer()->GetCharacterID())
		{
			x = GET_X_LPARAM(lParam) - p->X;
			y = GET_Y_LPARAM(lParam) - p->Y;
			GetGameUIMan()->PopupPlayerContextMenu(idPlayer, x, y);
		}
	}
}

void CDlgEarthBagRank::UpdateUI()
{
	//	排行榜选中信息
	if(m_pBtn_DPS) m_pBtn_DPS->SetPushed(m_curList == RANKLIST_DPS);
	if(m_pBtn_DPH) m_pBtn_DPH->SetPushed(m_curList == RANKLIST_DPH);

	RankList &curList = m_rankList[m_iCurProfession][m_curList];

    CheckRadioButton(1, m_iCurProfession);

	//	排名榜内容
	m_pLst_Rank->ResetContent();
	m_pLst_Self->ResetContent();

	abase::vector<int> ids;
	for (RankList::iterator it = curList.begin(); it != curList.end(); ++ it)
	{
		const RankInfo &info = *it;

		if (AppendLine(m_pLst_Rank, info.rank, info.id, info.level, info.value))
			ids.push_back(info.id);

		if (info.id == GetHostPlayer()->GetCharacterID())
			AppendLine(m_pLst_Self, info.rank, info.id, info.level, info.value);
	}
	
	//	向服务器获取玩家名字
	if (!ids.empty())
		GetGameSession()->CacheGetPlayerBriefInfo((int)ids.size(), &ids[0], 2);

	switch(m_curSort)
	{
	case SORT_BY_RANK:	OnCommand_SortBy_Rank("");	break;
	case SORT_BY_LEVEL:	OnCommand_SortBy_Level("");	break;
	}

	UpdateRefreshTime();
}

bool CDlgEarthBagRank::AppendLine(PAUILISTBOX pLstBox, int rank, int id, int level, int value)
{
	bool bNeedGetPlayerName(false);

	ACString strText;
	
	ACString strName = GetGameRun()->GetPlayerName(id, false);
	if (strName.IsEmpty())
	{
		bNeedGetPlayerName = true;
		strName = _AL("-");
	}

	strText.Format(_AL("%d\t%s\t%d\t%d"), rank, strName, level, value);
	int nItem = pLstBox->AddString(strText) -1;
	pLstBox->SetItemData(nItem, (DWORD)id, LST_INDEX_PLAYER);
	pLstBox->SetItemData(nItem, bNeedGetPlayerName ? 1 : 0, LST_INDEX_NEED_PLAYER);

	return bNeedGetPlayerName;
}

void CDlgEarthBagRank::UpdateLine(PAUILISTBOX pLstBox, int line)
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

void CDlgEarthBagRank::UpdateRefreshTime()
{
	//	更新下次刷新剩余时间
	if (!m_pTxt_Time)	return;	
	DWORD dwCurTime = timeGetTime();
	DWORD dwTimeLeft(0);
	if (dwCurTime < m_dwNextRefreshMoment[m_iCurProfession][m_curList])
		dwTimeLeft = m_dwNextRefreshMoment[m_iCurProfession][m_curList] - dwCurTime;
	ACString strTimeLeft = GetGameUIMan()->GetFormatTime((int)dwTimeLeft/1000);
	m_pTxt_Time->SetText(strTimeLeft);
}

void CDlgEarthBagRank::UpdateRank(const S2C::cmd_dps_dph_rank *pCmd)
{
	int i(0);
	RankInfo temp;

    int iDPH_DPS, iProfession;
    if( pCmd->rank_mask & 0x20 )
        iDPH_DPS = RANKLIST_DPS;
    else
        iDPH_DPS = RANKLIST_DPH;
    if( pCmd->rank_mask & 0x10 )
    {
        iProfession = pCmd->rank_mask & 0xF;
        ASSERT(iProfession>=0 && iProfession<NUM_PROFESSION);
    }
    else
        iProfession = NUM_PROFESSION;

	RankList &rankList = m_rankList[iProfession][iDPH_DPS];
    rankList.clear();
	for (i = 0; i < pCmd->rank_count; ++ i)
	{
		temp.rank = i+1;
		temp.id = pCmd->entry_list[i].roleid;
		temp.level = pCmd->entry_list[i].level;
		temp.value = pCmd->entry_list[i].value;
		rankList.push_back(temp);
	}

	m_dwNextRefreshMoment[iProfession][iDPH_DPS] = pCmd->next_refresh_sec * 1000;
	m_dwNextRefreshMoment[iProfession][iDPH_DPS] += timeGetTime();
    m_dwMinRefreshMoment = timeGetTime();
	
	//	更新界面
	UpdateUI();
}