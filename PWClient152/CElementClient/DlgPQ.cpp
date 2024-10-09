// Filename	: DlgPQ.h
// Creator	: Xu Wenbin
// Date		: 2009/10/12

#include "AFI.h"
#include "DlgPQ.h"
#include "DlgTask.h"
#include "TaskTemplMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "EC_TaskInterface.h"
#include "ECScriptMan.h"
#include "EC_UIManager.h"
#include "ECScriptContext.h"
#include "ECScriptCheckState.h"
#include "EC_IvtrItem.h"
#include "EC_FixedMsg.h"
#include "AUIDef.h"
#include "AUICTranslate.h"

#define new A_DEBUG_NEW

CDlgPQ::RankList CDlgPQ::s_rankList[NUM_PROFESSION+1];
CDlgPQ::PQInfo   CDlgPQ::s_PQInfo;
int              CDlgPQ::s_playerNumber;
time_t           CDlgPQ::m_lastTimeUpdated;

AUI_BEGIN_COMMAND_MAP(CDlgPQ, CDlgTaskTrace)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("rank",     OnCommand_Rank)
AUI_ON_COMMAND("Rdo_*",    OnCommand_RankList)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPQ, CDlgTaskTrace)
AUI_ON_EVENT("*",  WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_ON_EVENT(NULL, WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_END_EVENT_MAP()

CDlgPQ::CDlgPQ()
{
	m_pTxt_Name0 = NULL;
	m_pBtn_Rank = NULL;
	m_pBtn_Award = NULL;
	m_pTxt_Name1 = NULL;
	
	::memset(m_pLab_Radio, 0, sizeof(m_pLab_Radio));
	::memset(m_rankListUI, 0, sizeof(m_rankListUI));
	m_pTxt_Time = NULL;
	m_lastProfSelected = GetAllProfID();
	m_pScl_List = NULL;
	m_pTxt_PlayerNum = NULL;
	m_pTxt_myRank = NULL;
	m_pImg_myAwards = NULL;
	m_pTxt_myName = NULL;
	m_pTxt_myContribution = NULL;
}

bool CDlgPQ::OnInitDialog()
{
	if(!CDlgTaskTrace::OnInitDialog())
		return false;

	if (IsPQtitle())
	{
		DDX_Control("Txt_Name0", m_pTxt_Name0);
		DDX_Control("Btn_Rank", m_pBtn_Rank);
		DDX_Control("Btn_Award", m_pBtn_Award);
		DDX_Control("Txt_Name1", m_pTxt_Name1);

		ClearPQ();
	}
	else
	{
		const char * labRadio[NUM_PROFESSION+1] = {"Lab_WX", "Lab_FS", "Lab_WS", "Lab_YJ", "Lab_SS", "Lab_CK", "Lab_MG", "Lab_MM", "Lab_JL", "Lab_ML", "Lab_YY", "Lab_YX", "Lab_All"};
		for (int k = 0; k < sizeof(labRadio)/sizeof(labRadio[0]); ++ k)
			DDX_Control(labRadio[k], m_pLab_Radio[k]);

		DDX_Control("Txt_Time", m_pTxt_Time);
		AString strText;
		for (int i = 0; i < PAGE_SIZE; ++ i)
		{
			RankListItemUI &item = m_rankListUI[i];

			strText.Format("Txt_Rank%d", i+1);
			DDX_Control(strText, item.Txt_Rank);

			strText.Format("Img_Awards%d", i+1);
			DDX_Control(strText, item.Img_Awards);

			strText.Format("Txt_Name%d", i+1);
			DDX_Control(strText, item.Txt_Name);

			for (int j = 0; j < RANDOM_COUNT; ++ j)
			{
				strText.Format("Img_%02d%02d", i+1, j+1);
				DDX_Control(strText, item.Img_Random[j]);
			}

			strText.Format("Txt_Contribution%d", i+1);
			DDX_Control(strText, item.Txt_Contribution);
		}

		DDX_Control("Scl_List", m_pScl_List);
		DDX_Control("Txt_PlayerNum", m_pTxt_PlayerNum);
		DDX_Control("Txt_Rank8", m_pTxt_myRank);
		DDX_Control("Img_Awards8", m_pImg_myAwards);
		DDX_Control("Txt_Name8", m_pTxt_myName);
		DDX_Control("Txt_Contribution8", m_pTxt_myContribution);

		ClearRank();
	}
	return true;
}

void CDlgPQ::OnShowDialog()
{
	if (IsPQtitle())
		m_lastTimeUpdated = time(NULL);
}

bool CDlgPQ::Tick()
{
	if (IsPQtitle())
		RefreshPQUI();
	else
		RefreshRankUI();
	return CDlgTaskTrace::Tick();
}

void CDlgPQ::RefreshPQUI()
{
	bool bShow = IsHasPQ();
	if (!bShow)
		ClearPQ();
	if (IsShow() != bShow)
		Show(bShow);

	if (bShow)
	{
		bool valid(false);
		while (true)
		{
			ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
			const PQInfo &pq = GetPQInfo();
			ATaskTempl *pTempMain = pMan->GetTaskTemplByID(pq.task_id);
			ATaskTempl *pTempSub = pMan->GetTaskTemplByID(pq.sub_task_id);
			if (!pTempMain || !pTempSub)
				break;

			ACString strHint;
			
			m_pTxt_Name0->SetText(pTempMain->GetName());
			strHint = CDlgTask::FormatTaskText(pTempMain->GetDescription(), m_pTxt_Name0->GetColor());
			SetConstrainedHint(m_pTxt_Name0, strHint);

			m_pTxt_Name1->SetText(pTempSub->GetName());
			strHint = CDlgTask::FormatTaskText(pTempSub->GetDescription(), m_pTxt_Name1->GetColor());
			SetConstrainedHint(m_pTxt_Name1, strHint);

			RefreshTaskTrace(&pq.sub_task_id, 1, false);
			
			m_pBtn_Rank->Enable(true);
			m_pBtn_Award->Enable(true);
			
			valid = true;
			break;
		}
		
		if (!valid)
		{
			m_pTxt_Name0->SetText(_AL(""));
			m_pTxt_Name0->SetHint(_AL(""));
			m_pTxt_Name1->SetText(_AL(""));
			m_pTxt_Name1->SetHint(_AL(""));
			m_pTxt_Desc->SetText(_AL(""));
			m_pBtn_Rank->Enable(false);
			m_pBtn_Award->Enable(false);
		}

		time_t now = time(NULL);
		if (now-m_lastTimeUpdated >= GetUpdateTimeLength())
			m_lastTimeUpdated = now;
	}
	else
	{
		PAUIDIALOG pDlg_Rank = GetGameUIMan()->GetDialog("Win_PQrank");
		if (pDlg_Rank->IsShow())
			pDlg_Rank->Show(false);
	}
}

void CDlgPQ::RefreshRankUI()
{
	// Highlight color for group select and user select
	A3DCOLOR clrNormal = A3DCOLORRGB(0xff, 0xcc, 0x4a);
	A3DCOLOR clrHighlight = A3DCOLORRGB(0xff, 0, 0);

	// Update radio button
	CheckRadioButton(1, m_lastProfSelected);
	for (int k = 0; k < sizeof(m_pLab_Radio)/sizeof(m_pLab_Radio[0]); ++ k)
		m_pLab_Radio[k]->SetColor((k==m_lastProfSelected) ? clrHighlight : clrNormal);

	// Update time display
	time_t now = time(NULL);
	time_t elapsed = now-m_lastTimeUpdated;
	if (elapsed >= GetUpdateTimeLength())
	{
		m_lastTimeUpdated = now;
		elapsed = 0;
	}
	else
	{
		elapsed = GetUpdateTimeLength()-elapsed;
	}
	ACString strTime;
	strTime.Format(_AL("%d"), elapsed);
	m_pTxt_Time->SetText(strTime);

	// Update scroll info
	bool bFirstShow = m_pScl_List->GetScrollBegin() == m_pScl_List->GetScrollEnd();
	const RankList &rList = GetRankList(m_lastProfSelected);
	int listSize = (int)rList.size();
	if (listSize <= PAGE_SIZE)
	{
		m_pScl_List->SetBarLevel(0);
		m_pScl_List->Show(false);
	}
	else
	{
		m_pScl_List->SetScrollRange(0, listSize-PAGE_SIZE);
		m_pScl_List->SetScrollStep(1);
		m_pScl_List->SetBarLength(-1);
		m_pScl_List->Show(true);
	}
	
	// Get list display start
	int start = m_pScl_List->GetBarLevel();
	if (bFirstShow)
	{
		// Get my index in the rank list
		for (int i(0); i<listSize; ++i)
		{
			const RankInfo & item = GetRankListItem(m_lastProfSelected, i);
			if (item.roleid == GetHostPlayer()->GetCharacterID())
			{
				// Change start to my index
				start = (i/PAGE_SIZE)*PAGE_SIZE;
				break;
			}
		}
	}

	ACString strText;
	int digit(0);

	// Update current rank list part (except Award info)
	for (int i = 0; i < PAGE_SIZE; ++ i)
	{
		int iRankItem = start+i;

		RankListItemUI & itemUI = m_rankListUI[i];
		if (iRankItem >= listSize)
		{
			itemUI.Show(false);
			continue;
		}

		itemUI.Show(true);

		RankInfo & item = GetRankListItem(m_lastProfSelected, iRankItem);
		bool isMySelf = (GetHostPlayer()->GetCharacterID()==item.roleid);
		A3DCOLOR clr = isMySelf?clrHighlight:clrNormal;

		// Rank
		strText.Format(_AL("%d"), item.place);
		itemUI.Txt_Rank->SetText(strText);
		itemUI.Txt_Rank->SetColor(clr);

		// Award default to empty (updated later)
		itemUI.Img_Awards->ClearCover();
		itemUI.Img_Awards->SetHint(_AL(""));
		itemUI.Img_Awards->Show(false);

		// Player
		if (item.rolename.IsEmpty())
			item.rolename = GetGameRun()->GetPlayerName(item.roleid, false);
		itemUI.Txt_Name->SetText(item.rolename.IsEmpty() ? _AL("-") : item.rolename);
		itemUI.Txt_Name->SetColor(clr);

		// Random Digits
		if (item.rand_score < 0)
		{
			for (int j(0); j<RANDOM_COUNT; ++j)
			{
				digit = rand();
				itemUI.Img_Random[j]->FixFrame((digit+9)%10);
			}
		}
		else
		{		
			for (int j(0); j<RANDOM_COUNT; ++j)
			{
				digit = item.rand_score/(int)pow(10.0, RANDOM_COUNT-j-1) - (item.rand_score/(int)pow(10.0, RANDOM_COUNT-j))*10;
				itemUI.Img_Random[j]->FixFrame((digit+9)%10);
			}
		}

		// Contribution
		strText.Format(_AL("%d"), item.score);
		itemUI.Txt_Contribution->SetText(strText);
		itemUI.Txt_Contribution->SetColor(clr);
	}

	// Update list (for Award info)
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTempAward = pMan->GetTaskTemplByID(GetPQInfo().awardinfo_task_id);
	const AWARD_PQ_RANKING * pAwardInfo = NULL;
	while (pTempAward)
	{
		if (!pTempAward->m_Award_S)
			break;

		pAwardInfo = pTempAward->m_Award_S->m_PQRankingAward;
		if (!pAwardInfo)
			break;

		if (!pAwardInfo->m_ulRankingAwardNum)
		{
			pAwardInfo = NULL;
			break;
		}

		if (pAwardInfo->m_bAwardByProf && m_lastProfSelected==GetAllProfID() ||
			!pAwardInfo->m_bAwardByProf && m_lastProfSelected!=GetAllProfID())
		{
			break;
		}

		AString strFile;
		ACString strHint;
		int end = min(start+PAGE_SIZE, listSize);  // [start, end) corresponds to current display
		for (unsigned long i(0); i<pAwardInfo->m_ulRankingAwardNum; ++i)
		{
			const RANKING_AWARD &award = pAwardInfo->m_RankingAward[i];
			strFile.Format("%d.tga", i+1);
			if (award.m_ulAwardItemId>0 &&
				award.m_ulAwardItemNum>0)
			{
				CECIvtrItem *pAwardItem = CECIvtrItem::CreateItem(award.m_ulAwardItemId, 0, 1);
				if (pAwardItem)
				{
					strHint.Format(_AL("%s(%d)"), WC2AC(pAwardItem->GetName()), award.m_ulAwardItemNum);
					delete pAwardItem;
				}
			}

			for (int j=0; j <PAGE_SIZE; ++j)
			{
				int iRankItem = start+j;				
				RankListItemUI & itemUI = m_rankListUI[j];
				if (iRankItem >= listSize)
					break;
				const RankInfo & item = GetRankListItem(m_lastProfSelected, iRankItem);
				if (item.place>=(int)award.m_iRankingStart &&
					item.place<=(int)award.m_iRankingEnd &&
					item.score>=(int)pTempAward->m_Award_S->m_ulLowestcontrib)
				{
					itemUI.Img_Awards->SetCover(
						GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_PQ],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_PQ][strFile]);
					itemUI.Img_Awards->SetHint(strHint);
					itemUI.Img_Awards->Show(true);
				}
			}
		}

		break;
	}

	strText.Format(_AL("%d"), s_playerNumber);
	m_pTxt_PlayerNum->SetText(strText);
	
	// Show information of myself
	m_pTxt_myName->SetText(GetHostPlayer()->GetName());

	bool allProfession = (m_lastProfSelected == GetAllProfID());
	bool myProfession = (GetHostPlayer()->GetProfession() == m_lastProfSelected);
	bool imageShown(false);
	while (true)
	{
		m_pTxt_myRank->SetText(_AL("-"));
		m_pTxt_myContribution->SetText(_AL("-"));

		if (!allProfession && !myProfession)
			break;
		
		// Show contribution
		const PQInfo &pqInfo = GetPQInfo();
		strText.Format(_AL("%d"), pqInfo.score);
		m_pTxt_myContribution->SetText(strText);
		
		// Show rank
		int rank = allProfession ? pqInfo.all_place : pqInfo.prof_place;
		if (rank < 0)
			break;
		strText.Format(_AL("%d"), rank);
		m_pTxt_myRank->SetText(strText);

		// Show award regardless of profession or all-profession
		if (!pAwardInfo)
			break;
		rank = pAwardInfo->m_bAwardByProf ? pqInfo.prof_place : pqInfo.all_place;
		for (unsigned long i(0); i<pAwardInfo->m_ulRankingAwardNum; ++i)
		{
			const RANKING_AWARD &award = pAwardInfo->m_RankingAward[i];
			if (rank>=(int)award.m_iRankingStart &&
				rank<=(int)award.m_iRankingEnd)
			{
				if (pqInfo.score>=(int)pTempAward->m_Award_S->m_ulLowestcontrib)
				{
					AString strFile;
					strFile.Format("%d.tga", i+1);
					
					ACString strHint;
					if (award.m_ulAwardItemId>0 &&
						award.m_ulAwardItemNum>0)
					{
						CECIvtrItem *pAwardItem = CECIvtrItem::CreateItem(award.m_ulAwardItemId, 0, 1);
						if (pAwardItem)
						{
							strHint.Format(_AL("%s(%d)"), WC2AC(pAwardItem->GetName()), award.m_ulAwardItemNum);
							delete pAwardItem;
						}
					}
					
					m_pImg_myAwards->SetCover(
						GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_PQ],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_PQ][strFile]);
					m_pImg_myAwards->SetHint(strHint);
					m_pImg_myAwards->Show(true);
					
					imageShown = true;
				}
				break;
			}
		}
		break;
	}
	if (!imageShown)
	{
		m_pImg_myAwards->ClearCover();
		m_pImg_myAwards->SetHint(_AL(""));
		m_pImg_myAwards->Show(false);
	}
}

void CDlgPQ::UpdateRank(int profession, const RankList &rList)
{
	// Profession from 0~8, with 8 corresponding to all profession rank
	//
	if (!IsPQrank())
		return;
	s_rankList[profession] = rList;
	m_lastTimeUpdated = time(NULL);
	if (!rList.empty())
	{
		// Check player name that's unknown
		abase::vector<int> ids;
		for (size_t i(0); i<rList.size(); ++i)
		{
			const RankInfo &info = rList[i];
			if (!GetGameRun()->GetPlayerName(info.roleid, false))
				ids.push_back(info.roleid);
		}

		// Get player names that's still unknown
		if (!ids.empty())
			GetGameSession()->CacheGetPlayerBriefInfo((int)ids.size(), &ids[0], 2);
	}
	if (IsShow())
		RefreshRankUI();
}

void CDlgPQ::UpdatePQ(const PQInfo &pqInfo)
{
	if (!IsPQtitle())
		return;

	// Save last PQInfo for award
	PQInfo old = s_PQInfo;

	// Update PQInfo except award
	s_PQInfo = pqInfo;

	// Update PQInfo award
	if (!pqInfo.IsValid() || !old.IsValid())
	{
		s_PQInfo.awardinfo_task_id = 0;
	}
	else
	{
		if (old.task_id == pqInfo.task_id)
		{
			if (old.sub_task_id == pqInfo.sub_task_id)
			{
				// Other values update: keep award info unchanged
				s_PQInfo.awardinfo_task_id = old.awardinfo_task_id;
			}
			else
			{
				// Not the same sub task id
				// Award info comes from last sub task
				s_PQInfo.awardinfo_task_id = old.sub_task_id;
			}
		}
		else
		{
			// Else hide award info
			s_PQInfo.awardinfo_task_id = 0;
		}
	}

	// Show at demand
	if (!IsShow() && s_PQInfo.IsValid())
		Show(true);
	
	// Set script state for show help information for first PQ
	// The script will be executed once if we successfully set true here
	// Don't worry about the FIRST
	if (s_PQInfo.IsValid())
	{
		CECScriptMan * pScriptMan = GetGameRun()->GetUIManager()->GetScriptMan();
		if (pScriptMan)
			pScriptMan->GetContext()->GetCheckState()->SetPQIsTriggered(true);
	}
	
	// Show score got
	if (pqInfo.IsValid() && old.IsValid())
	{
		int scoreGot = pqInfo.score-old.score;
		if (scoreGot>0)
			GetGameRun()->AddFixedMessage(FIXMSG_GOTSCORE, scoreGot);
	}
}

void CDlgPQ::UpdatePQPlayerNumber(int num)
{
	if (!IsPQrank())
		return;
	s_playerNumber = num;
}

void CDlgPQ::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsPQrank())
		return;
	if (m_pScl_List->IsShow())
	{
		int zDelta = (short)HIWORD(wParam);
		zDelta /= 120;
		m_pScl_List->SetBarLevel(m_pScl_List->GetBarLevel() - zDelta);
	}
}


void CDlgPQ::OnCommand_CANCEL(const char * szCommand)
{
	if (IsPQtitle())
		m_pAUIManager->RespawnMessage();
	else
		Show(false);
}

void CDlgPQ::OnCommand_Rank(const char *szCommand)
{
	PAUIDIALOG pDlg_Rank = GetGameUIMan()->GetDialog("Win_PQrank");
	pDlg_Rank->Show(!pDlg_Rank->IsShow());
}

void CDlgPQ::OnCommand_RankList(const char *szCommand)
{
	int iProfession = atoi(szCommand + strlen("Rdo_"));
	if (iProfession != m_lastProfSelected)
	{
		m_lastProfSelected = iProfession;
		RefreshRankUI();
	}
}

void CDlgPQ::ClearPQ()
{
	s_PQInfo.Clear();
}

void CDlgPQ::ClearRank()
{
	for (int i = 0; i < sizeof(s_rankList)/sizeof(s_rankList[0]); ++ i)
		s_rankList[i].clear();
	s_playerNumber = 0;
}

bool CDlgPQ::IsHasPQ()
{
	// Check whether current pq is still active
	bool valid(false);
	while (s_PQInfo.IsValid())
	{
		CECTaskInterface *pTask = g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface();
		if (!pTask->HasTask(s_PQInfo.task_id))
			break;

//		if (!pTask->HasTask(s_PQInfo.sub_task_id))
// 			break;
		// We can NOT validate sub_task_id here because it's different from original task
		// The original task finished and disappears in CECTaskInterface::HasTask from client check and protocols to server
		// The PQ sub task finish comes directly from server protocol
		// So when new PQ sub task comes from server, client CECTaskInterface still have old task, and new sub task is still invalid
		
		valid = true;
		break;
	}
	return valid;
}

bool CDlgPQ::IsPQtitle()
{
	return m_szName == "Win_PQtitle";
}

bool CDlgPQ::IsPQrank()
{
	return m_szName == "Win_PQrank";
}

void CDlgPQ::SetConstrainedHint(PAUIOBJECT pObj, const ACString &strHint)
{
	// Test code
	//ACString strHint = _AL("Help the Tideborn to release the pressure of soldiers on Blunted Lance Stand.\rYou are doing a Regional Quest. Do not leave Shattered Cloud Island, otherwise the quest will fail.");

	AUICTranslate trans;
	ACString strText = trans.Translate(strHint);

	int len = strHint.GetLength();
	int maxWidth = min(25, max(10, (int)sqrt(len*3.0)));
	maxWidth *= pObj->GetFontHeight();
	pObj->SetHintMaxWidth(maxWidth);

	pObj->SetHint(strHint);
}
