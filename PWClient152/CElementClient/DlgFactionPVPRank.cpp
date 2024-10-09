// Filename	: DlgFactionPVPRank.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/2

#include "DlgFactionPVPRank.h"

#include "EC_UIHelper.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_Split.h"

#include <AUIManager.h>

static const int PAGE_SIZE = 10;

//	class CDlgFactionPVPRank

AUI_BEGIN_COMMAND_MAP(CDlgFactionPVPRank, CDlgBase)
AUI_ON_COMMAND("Btn_Pagehome",		OnCommandHomePage)
AUI_ON_COMMAND("Btn_Pageup",		OnCommandPrevPage)
AUI_ON_COMMAND("Btn_Pagedown",		OnCommandNextPage)
AUI_ON_COMMAND("Btn_Pageend",		OnCommandLastPage)
AUI_ON_COMMAND("Btn_Name",			OnCommandSortByName)
AUI_ON_COMMAND("Btn_killNumber",	OnCommandSortByKillNumber)
AUI_ON_COMMAND("Btn_Mortailty",		OnCommandSortByDeathNumber)
AUI_ON_COMMAND("Btn_ItemUsed",		OnCommandSortByUseItemCount)
AUI_ON_COMMAND("Btn_Score",			OnCommandSortByScore)
AUI_END_COMMAND_MAP()

CDlgFactionPVPRank::CDlgFactionPVPRank()
: m_pTxt_GuildName(NULL)
, m_pTxt_MyScore(NULL)
, m_pTxt_RewardListSize(NULL)
, m_pLst_Result(NULL)
, m_pBtn_HomePage(NULL)
, m_pBtn_PrevPage(NULL)
, m_pBtn_NextPage(NULL)
, m_pBtn_LastPage(NULL)
, m_reportCount(0)
, m_pageCount(0)
, m_currentPage(-1)
{
}

bool CDlgFactionPVPRank::OnInitDialog()
{
	if (CDlgBase::OnInitDialog()){
		DDX_Control("Txt_GuildName",	m_pTxt_GuildName);
		DDX_Control("Txt_GuildScore",	m_pTxt_MyScore);
		DDX_Control("Txt_MemberNum",	m_pTxt_RewardListSize);
		DDX_Control("Txt_Result",		m_pLst_Result);
		DDX_Control("Btn_Pagehome",		m_pBtn_HomePage);
		DDX_Control("Btn_Pageup",		m_pBtn_PrevPage);
		DDX_Control("Btn_Pagedown",		m_pBtn_NextPage);
		DDX_Control("Btn_Pageend",		m_pBtn_LastPage);
		ResetUI();
		return true;
	}
	return false;
}

void CDlgFactionPVPRank::ResetUI()
{
	m_reportCount = 0;
	m_pageCount = 0;
	m_currentPage = -1;
	UpdatePageButtons();
	UpdateCurrentPage();
	UpdateOtherInfo();
}


bool CDlgFactionPVPRank::CanGoHomePage()const
{
	return m_pageCount > 1 && m_currentPage > 0;
}

bool CDlgFactionPVPRank::CanGoPrevPage()const
{
	return m_pageCount > 1 && m_currentPage > 0;
}

bool CDlgFactionPVPRank::CanGoNextPage()const
{
	return m_pageCount > 1 && m_currentPage+1 < m_pageCount;
}

bool CDlgFactionPVPRank::CanGoLastPage()const
{
	return m_pageCount > 1 && m_currentPage+1 != m_pageCount;
}

void CDlgFactionPVPRank::UpdatePageButtons()
{
	m_pBtn_HomePage->Enable(CanGoHomePage());
	m_pBtn_PrevPage->Enable(CanGoPrevPage());
	m_pBtn_NextPage->Enable(CanGoNextPage());
	m_pBtn_LastPage->Enable(CanGoLastPage());
}

enum{
	LST_ITEMDATA_NAME_UNKNOWN = 1,	//	未获取玩家名称时设置的标识
};

void CDlgFactionPVPRank::UpdateCurrentPage()
{
	m_pLst_Result->ResetContent();
	if (m_currentPage >= 0){
		CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
		int beginIndex = m_currentPage * PAGE_SIZE;
		int endIndex = min(beginIndex+PAGE_SIZE, factionPVPModel.ReportListCount());
		abase::vector<int> unknownIDs;
		CECGameRun *pGameRun = GetGameRun();
		for (int i = beginIndex; i < endIndex; ++ i)
		{
			const CECFactionPVPModel::ReportItem *p = factionPVPModel.ReportListItem(i);
			const ACHAR *szName = NULL;
			bool unknown = !PlayerName(p->roleid, szName);
			ACString strText;
			strText.Format(_AL("%s\t%u\t%u\t%u\t%d"), szName, p->killCount, p->deathCount, p->useToolCount, p->score);
			m_pLst_Result->AddString(strText);
			if (unknown){
				int itemIndex = m_pLst_Result->GetCount()-1;
				m_pLst_Result->SetItemData(itemIndex, LST_ITEMDATA_NAME_UNKNOWN, LST_ITEMINDEX_NAME);
				m_pLst_Result->SetItemDataPtr(itemIndex, (void *)p->roleid, LST_ITEMINDEX_NAME);
				unknownIDs.push_back(p->roleid);
			}
		}
		if (!unknownIDs.empty()){
			GetGameSession()->CacheGetPlayerBriefInfo(unknownIDs.size(), &unknownIDs[0], 2);
		}
	}
}

bool CDlgFactionPVPRank::PlayerName(int roleid, const ACHAR * &szName)
{
	bool bFound(false);
	if (szName = GetGameRun()->GetPlayerName(roleid, false)){
		bFound = true;
	}else{
		szName = _AL("-");
	}
	return bFound;
}

void CDlgFactionPVPRank::OnTick()
{
	CDlgBase::OnTick();
	for (int i(0); i < m_pLst_Result->GetCount(); ++ i)
	{
		if (m_pLst_Result->GetItemData(i, LST_ITEMINDEX_NAME) == LST_ITEMDATA_NAME_UNKNOWN){
			int roleid = (int)m_pLst_Result->GetItemDataPtr(i, LST_ITEMINDEX_NAME);
			const ACHAR *szName = NULL;
			if (PlayerName(roleid, szName)){
				m_pLst_Result->SetItemData(i, 0, LST_ITEMINDEX_NAME);
				m_pLst_Result->SetItemDataPtr(i, NULL, LST_ITEMINDEX_NAME);
				ACString currentText = m_pLst_Result->GetText(i);
				CECSplitHelperW splitter(currentText, _AL("\t"));
				ACString strText;
				strText.Format(_AL("%s\t%s\t%s\t%s\t%d"), szName, splitter.ToString(1), splitter.ToString(2), splitter.ToString(3), splitter.ToString(4));
				m_pLst_Result->SetText(i, strText);
			}
		}
	}
}

void CDlgFactionPVPRank::UpdateOtherInfo()
{
	CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	if (factionPVPModel.ReportListReady() && factionPVPModel.ReportListCount() > 0){
		//	帮派名称
		CECHostPlayer *pHost = GetHostPlayer();
		CECUIHelper::SetFactionName(pHost->GetFactionID(), m_pTxt_GuildName);

		//	获得奖励人数
		int listCount = factionPVPModel.ReportListCount();
		ACString str;
		str.Format(_AL("%d"), listCount);
		m_pTxt_RewardListSize->SetText(str);
		
		//	我的积分
		m_pTxt_MyScore->SetText(_AL(""));
		int idHost = pHost->GetCharacterID();
		for (int i(0); i < listCount; ++ i)
		{
			const CECFactionPVPModel::ReportItem *p = factionPVPModel.ReportListItem(i);
			if (p->roleid == idHost){
				str.Format(_AL("%d"), p->score);
				m_pTxt_MyScore->SetText(str);
				break;
			}
		}
	}else{
		m_pTxt_GuildName->SetText(_AL(""));
		m_pTxt_MyScore->SetText(_AL(""));
		m_pTxt_RewardListSize->SetText(_AL(""));
	}
}

void CDlgFactionPVPRank::OnRegistered(const CECFactionPVPModel *p)
{
	UpdateRankList(p);
}

void CDlgFactionPVPRank::OnModelChange(const CECFactionPVPModel *p, const CECObservableChange *q)
{
	const CECFactionPVPModelChange *pChange = dynamic_cast<const CECFactionPVPModelChange *>(q);
	if (!pChange){
		ASSERT(false);
	}else if (!pChange->IsRankListChanged()){
		return;
	}
	UpdateRankList(p);
}

void CDlgFactionPVPRank::UpdateRankList(const CECFactionPVPModel *p)
{
	if (p->ReportListReady()){
		m_reportCount = p->ReportListCount();
		m_pageCount = m_reportCount / PAGE_SIZE;
		if (m_reportCount % PAGE_SIZE){
			++ m_pageCount;
		}
	}else{
		m_reportCount = 0;
		m_pageCount = 0;
	}
	if (m_pageCount > 0){
		m_currentPage = 0;
	}else{
		m_currentPage = -1;
	}
	UpdatePageButtons();
	UpdateCurrentPage();
	UpdateOtherInfo();
}

bool CDlgFactionPVPRank::CanShow()
{	
	bool bCan(false);
	if (CECFactionPVPModel::Instance().CanShowReportList()){
		if (PAUIDIALOG pDlg = GetAUIManager()->GetDialog("Win_GuildMap")){
			bCan = pDlg->IsShow();
		}
	}
	return bCan;
}

void CDlgFactionPVPRank::OnCommandHomePage(const char * szCommand)
{
	if (!CanGoHomePage()){
		return;
	}
	m_currentPage = 0;
	UpdateCurrentPage();
	UpdatePageButtons();
}

void CDlgFactionPVPRank::OnCommandPrevPage(const char * szCommand)
{
	if (!CanGoPrevPage()){
		return;
	}
	-- m_currentPage;
	UpdateCurrentPage();
	UpdatePageButtons();
}

void CDlgFactionPVPRank::OnCommandNextPage(const char * szCommand)
{
	if (!CanGoNextPage()){
		return;
	}
	++ m_currentPage;
	UpdateCurrentPage();
	UpdatePageButtons();
}

void CDlgFactionPVPRank::OnCommandLastPage(const char * szCommand)
{
	if (!CanGoLastPage()){
		return;
	}
	m_currentPage = m_pageCount-1;
	UpdateCurrentPage();
	UpdatePageButtons();
}

static CECFactionPVPModel::ReportItemSortMethod GetSortMethodOnClickOn(CECFactionPVPModel::ReportItemSortIndex itemIndex)
{
	//	返回用户点击 itemIndex 栏后，将采用的新排序模式
	const CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	CECFactionPVPModel::ReportItemSortMethod newSortMethod = factionPVPModel.DefaultReportSortMethod();
	if (factionPVPModel.ReportSortIndex() == itemIndex){
		switch (factionPVPModel.ReportSortMethod())
		{
		case CECFactionPVPModel::RISM_ASCENT:
			newSortMethod = CECFactionPVPModel::RISM_DESENT;
			break;
		case CECFactionPVPModel::RISM_DESENT:
			newSortMethod = CECFactionPVPModel::RISM_ASCENT;
			break;
		}
	}
	return newSortMethod;
}

void CDlgFactionPVPRank::OnCommandSortByName(const char * szCommand)
{
	//	因为获取玩家名称是异步操作、对名称排序只适合实现为局部操作，而这与其它排序的全局性不一致，故不实现此种排序
}

void CDlgFactionPVPRank::OnCommandSortByKillNumber(const char * szCommand)
{
	if (m_pLst_Result->GetCount() <= 0){
		return;
	}
	CECFactionPVPModel::ReportItemSortIndex itemIndex = CECFactionPVPModel::RISI_KILL_COUNT;
	CECFactionPVPModel::Instance().SetSortMethod(itemIndex, GetSortMethodOnClickOn(itemIndex));
	UpdateCurrentPage();
}

void CDlgFactionPVPRank::OnCommandSortByDeathNumber(const char * szCommand)
{
	if (m_pLst_Result->GetCount() <= 0){
		return;
	}
	CECFactionPVPModel::ReportItemSortIndex itemIndex = CECFactionPVPModel::RISI_DEATH_COUNT;
	CECFactionPVPModel::Instance().SetSortMethod(itemIndex, GetSortMethodOnClickOn(itemIndex));
	UpdateCurrentPage();
}

void CDlgFactionPVPRank::OnCommandSortByUseItemCount(const char * szCommand)
{
	if (m_pLst_Result->GetCount() <= 0){
		return;
	}
	CECFactionPVPModel::ReportItemSortIndex itemIndex = CECFactionPVPModel::RISI_USE_TOOL_COUNT;
	CECFactionPVPModel::Instance().SetSortMethod(itemIndex, GetSortMethodOnClickOn(itemIndex));
	UpdateCurrentPage();
}

void CDlgFactionPVPRank::OnCommandSortByScore(const char * szCommand)
{
	if (m_pLst_Result->GetCount() <= 0){
		return;
	}
	CECFactionPVPModel::ReportItemSortIndex itemIndex = CECFactionPVPModel::RISI_SCORE;
	CECFactionPVPModel::Instance().SetSortMethod(itemIndex, GetSortMethodOnClickOn(itemIndex));
	UpdateCurrentPage();
}
