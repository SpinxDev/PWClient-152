// Filename	: DlgSignIn.cpp
// Creator	: Han Guanghui
// Date		: 2013/6/13

#include "DlgSignIn.h"
#include "DlgActivity.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_CrossServer.h"
#include "EC_IvtrItem.h"
#include "elementdataman.h"
#include "ExpTypes.h"

#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUICTranslate.h"


AUI_BEGIN_COMMAND_MAP(CDlgSignIn, CDlgBase)

AUI_ON_COMMAND("Btn_SignIn",		OnCommandSignIn)
AUI_ON_COMMAND("Btn_Award",			OnCommandGetYearReward)
AUI_ON_COMMAND("Btn_DaysBefore",	OnCommandMoveLeft)
AUI_ON_COMMAND("Btn_DaysLater",		OnCommandMoveRight)
AUI_ON_COMMAND("Btn_Award0*",		OnCommandGetDailyReward)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSignIn, CDlgBase)
AUI_ON_EVENT("Img_Date*", WM_LBUTTONDOWN, OnEventMouseDownDateImg)
AUI_END_EVENT_MAP()

extern CECGame* g_pGame;

namespace {
	const int DaysInMonth[MONTH_COUNT_IN_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	const int AwardItemConfigIDs[MONTH_COUNT_IN_YEAR] = {1800, 1801, 1802, 1803, 1804, 1805, 1806, 1807, 1808, 1809, 1810, 1811};
}

CDlgSignIn::CDlgSignIn():
m_iCurrentSelection(0),
m_iServerUpdateTime(0),
m_iAwardListFirstDay(1),
m_iAwardListLastDay(7),
m_iEmptySlotsCount(0)
{

}

CDlgSignIn::~CDlgSignIn()
{

}

void CDlgSignIn::OnShowDialog()
{
	// 每次打开界面都请求更新数据
	GetGameSession()->c2s_CmdRefreshSignIn();
}

bool CDlgSignIn::OnInitDialog() {
	if (!CDlgBase::OnInitDialog()) {
		return false;
	}
	DATA_TYPE dt;
	elementdataman* pDB = g_pGame->GetElementDataMan();
	int i;
	for (i = 0; i < MONTH_COUNT_IN_YEAR; i++) {
		SIGN_AWARD_CONFIG* config = (SIGN_AWARD_CONFIG*)pDB->get_data_ptr(AwardItemConfigIDs[i], ID_SPACE_CONFIG, dt);
		if (dt != DT_SIGN_AWARD_CONFIG || !config){
			ASSERT(false);
			a_LogOutput(1, "Invalid sign in avoid item config id %d", AwardItemConfigIDs[i]);
		}
		m_signAwardConfigs[i] = config;
	}
	return true;
}

void CDlgSignIn::OnCommandSignIn(const char* szCommand)
{
	char szTemp[50];
	sprintf(szTemp, "Img_Date%d", m_iCurrentSelection);
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
	if (pImg){
		int iFrame = pImg->GetFixFrame();
		if (iFrame == IMG_DATE_FRAME_ABSENT) {
			// 检查包裹中的补签道具
			int item_pos =  GetHostPlayer()->GetPack()->FindItem(39960);
			if (item_pos == -1) {
				GetGameUIMan()->MessageBox("", GetStringFromTable(10627), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
			tm time_signin = g_pGame->GetServerLocalTime(m_iServerUpdateTime);
			time_signin.tm_mday = m_iCurrentSelection;
			int dest_time = mktime(&time_signin);
			GetGameSession()->c2s_CmdLateSignin(0, item_pos, dest_time);
		}
	}
}

void CDlgSignIn::OnCommandGetYearReward(const char* szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (!pObj) return;
	//三位，分别表示0x1月度0x2年度0x4全勤超级奖（月度奖已取消，只有年度奖和超级奖只能领更好的那个）
	int type = pObj->GetData() & 0x7;
	//int mon = pObj->GetData() >> 4;
	//int prize_count = (type & 0x1) + (type >> 1 & 0x1) + (type >> 2 & 0x1);
	int prize_count = 1;
	if (GetHostPlayer()->GetPack()->GetEmptySlotNum() < prize_count) {
		ACString strText;
		strText.Format(GetStringFromTable(10626), prize_count);
		GetGameUIMan()->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else GetGameSession()->c2s_CmdSignInAward(type, 0);
}

void CDlgSignIn::OnEventMouseDownDateImg(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (!pImg || !pImg->IsShow()) return;
	int iFrame = pImg->GetFixFrame();
	if (iFrame == IMG_DATE_FRAME_ABSENT) {
		if (m_iCurrentSelection == pImg->GetData()) {
			return;
		}
		int iLastSelect = m_iCurrentSelection;
		m_iCurrentSelection = pImg->GetData();
		pImg->SetFlash(true);
		if (iLastSelect) {
			char szTemp[50];
			sprintf(szTemp, "Img_Date%d", iLastSelect);
			PAUIOBJECT pLast = GetDlgItem(szTemp);
			if (pLast) pLast->SetFlash(false);
		}
	}
}

bool CDlgSignIn::SumUpMonthState(int year_state, int state)
{
	int month_state(0);
	for (int i = 0; i < 12; ++i) { 
		month_state = (year_state >> (i * 2)) & 0x3;
		if (month_state < state) return false;
	}
	return true;
}
void CDlgSignIn::Update(int iServerTime)
{
	if (CECCrossServer::Instance().IsOnSpecialServer())
		return;
	m_iServerUpdateTime = iServerTime;
	CECHostPlayer* pHost = GetHostPlayer();
	int iMonthCalendar = pHost->GetSignInMonthCalendar();
	int iYearStat = pHost->GetSignInYearStat();
	int iLastYearStat = pHost->GetSignInLastYearStat();
	tm time_server = g_pGame->GetServerLocalTime(iServerTime);
	GetGameUIMan()->m_pDlgActivity->SetSignInUpdateDay(time_server.tm_mday);
	char szTemp[50];
	PAUIIMAGEPICTURE pImg;
	int state(0), frame(0), reward_state(0),month_award(0),i(0);
	ACString strHint,strTemp;
	PAUIOBJECT pBtn = GetDlgItem("Btn_SignIn");
	if (!pBtn) return;
	pBtn->Enable(false);
	// 服务器日期
	strTemp.Format(m_pAUIManager->GetStringFromTable(10629),
		time_server.tm_year+1900,
		time_server.tm_mon+1,
		time_server.tm_mday);
	PAUIOBJECT pObj = GetDlgItem("Txt_title");
	if (pObj) pObj->SetText(strTemp);
	// 只有1月份能领去年的年度奖和全勤奖
	if (time_server.tm_mon == 0) {
		if (((iLastYearStat & 0x40000000) == 0) && ((iLastYearStat & 0x80000000) == 0)) {
			// 没有领过年度奖或超级奖
			if (SumUpMonthState(iLastYearStat, 2)) {
				// 可以领取超级奖
				reward_state = 0x4;
				strHint = GetStringFromTable(10623);
			} else if (SumUpMonthState(iLastYearStat, 1)) {
				// 可以领取年度奖
				reward_state = 0x2;
				strHint = GetStringFromTable(10622);
			}
		}

		/*
		if ((iLastYearStat & 0x40000000) == 0 
			&& SumUpMonthState(iLastYearStat, 1)) {
			reward_state |= 0x2;
			strHint += GetStringFromTable(10622);
		}
		*/
		/*
		if ((iLastYearStat & 0x80000000) == 0
			&& SumUpMonthState(iLastYearStat, 2)) {
			reward_state |= 0x4;
			strHint += _AL(", ");
			strHint += GetStringFromTable(10623);
		}
		*/
	}
	int hint(0);
	// 更新月份签到状态
	while (true) {
		sprintf(szTemp, "Img_Month%d", i + 1);
		pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
		if (!pImg) break;
		if (i < time_server.tm_mon) {
			//	前24bit按 每月2bit排列状态值
			//		0大于3次漏签
			//		1小等于3次漏签
			//		2 当月全勤
			//		3 已领取过全勤奖励
			state = (iYearStat >> (i * 2)) & 0x3;
			frame = IMG_MONTH_FRAME_COMPLETE;
			switch(state) {
			case 0:
				frame = IMG_MONTH_FRAME_ABSENT_MORE_THAN_3;
				hint = 10631;
				break;
			case 1:
				frame = IMG_MONTH_FRAME_ABSENT_LESS_OR_EQUAL_THAN_3;
				hint = 10632;
				break;
			case 2:
				frame = IMG_MONTH_FRAME_COMPLETE;
				hint = 10630;
				break;
			case 3:
				frame = IMG_MONTH_FRAME_COMPLETE;
				hint = 10630;
				break;
			default:
				break;
			}
			pImg->Show(true);
			pImg->SetHint(GetStringFromTable(hint));
			pImg->FixFrame(frame);
		} else if (i == time_server.tm_mon && IsSignInEnoughThisMonth()) {
			if (IsSignInCompeleteThisMonth()) {
				// 本月签到次数已满
				pImg->Show(true);
				pImg->SetHint(GetStringFromTable(10630));
				pImg->FixFrame(IMG_MONTH_FRAME_COMPLETE);
			} else if (IsSignInEnoughThisMonth()) {
				// 本月未签次数不超过3
				pImg->Show(true);
				pImg->SetHint(GetStringFromTable(10632));
				pImg->FixFrame(IMG_MONTH_FRAME_ABSENT_LESS_OR_EQUAL_THAN_3);
			} else {
				// 本月未签次数超过3
				pImg->Show(false);
			}
		} else {
			pImg->Show(false);
		}
		// 上个月的月度奖
		/*
		int last_month = time_server.tm_mon - 1;
		if (last_month < 0) last_month = 11;
		if (time_server.tm_mon != last_month){
			if ((i ==  last_month && state == 2) ||
				(i == 0  && (((iLastYearStat >> 22) & 0x3) == 2))){
				reward_state |= 0x1;
				if (!strHint.IsEmpty()) strHint += _AL(", ");
				strHint += GetStringFromTable(10621);
				month_award = last_month;
			}
		}
		*/
		++i;
	}
	// 更新当月签到状态
	i = 1;
	bool bSelect(false);
	while (true) {
		sprintf(szTemp, "Img_Date%d", i);
		pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
		if (!pImg) break;
		sprintf(szTemp, "Txt_Date%d", i);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		pImg->Show(true);
		pObj->Show(false);
		pImg->SetData(i);
		pImg->SetFlash(false);
		state = (iMonthCalendar >> (i - 1)) & 0x1;
		if (i < time_server.tm_mday){
			frame = state ? IMG_DATE_FRAME_SIGNED_IN : IMG_DATE_FRAME_ABSENT;
			pImg->FixFrame(frame);
			if (state == 0 && !bSelect) {
				bSelect = true;
				m_iCurrentSelection = i;
				pImg->SetFlash(true);
				if (pHost->GetSignInLateCountThisMonth() < MAX_LATE_SIGN_IN_COUNT) {
					pBtn->Enable(true);
				}
			}
		}else if (i == time_server.tm_mday) {
			pImg->FixFrame(state ? IMG_DATE_FRAME_SIGNED_IN : IMG_DATE_FRAME_TODAY);
			GetGameUIMan()->m_pDlgActivity->EnableSignIn(state == 0);
		} else {
			tm time_future = time_server;
			time_future.tm_mday = i;
			time_future = g_pGame->GetServerLocalTime(mktime(&time_future));
			if (time_future.tm_mon == time_server.tm_mon) {
				pImg->FixFrame(IMG_DATE_FRAME_FUTURE);
				pObj->Show(true);
			}
			else {
				pImg->Show(false);
			}
		}
		++i;
	}
	pBtn = GetDlgItem("Btn_Award");
	if (pBtn) {
		pBtn->SetHint(strHint);
		pBtn->SetData(reward_state);
		pBtn->Enable(reward_state != 0);
			
		// 可以领超级年度奖时，显示GFX
		AUIObject* pObj = GetDlgItem("Img_BtnGfx");
		if (pObj) {
			pObj->Show(reward_state == 0x4);
		}	
	}

	// added
	time_server = g_pGame->GetServerLocalTime(iServerTime);
	ShowMonth(time_server.tm_mon + 1);
	int signedInCount = GetSignedInCount();
	ShowSignedInDaysCount(signedInCount);
	int canLateSignInCount = GetCanLateSignInCount();
	ShowCanLateSignDaysCount(canLateSignInCount);
	ResetAwardListDayRange();
	SetListMoveBtnEnable();
	ShowAwardListDayRange();
	ShowAwardListItemsView();
}

int CDlgSignIn::GetCurMonth() {
	tm time_server = g_pGame->GetServerLocalTime(m_iServerUpdateTime);
	return time_server.tm_mon + 1;
}

int CDlgSignIn::GetCurDay() {
	tm time_server = g_pGame->GetServerLocalTime(m_iServerUpdateTime);
	return time_server.tm_mday;
}

int CDlgSignIn::GetSignedInCount() {
	CECHostPlayer* pHost = GetHostPlayer();
	int iMonthCalendar = pHost->GetSignInMonthCalendar();
	int signInDays = 0;
	int bitMask = 1;
	for (int i = 0; i < sizeof(bitMask) * 8; i++) {
		if (bitMask & iMonthCalendar) {
			signInDays++;
		}
		bitMask = bitMask << 1;
	}
	return signInDays;
}

int CDlgSignIn::GetRecvAwardCount() {
	return g_pGame->GetGameRun()->GetHostPlayer()->GetSignInAwardedCountThisMonth();
}

int CDlgSignIn::GetCanLateSignInCount() {
	int iDay = GetCurDay();
	int iSignedInCount = GetSignedInCount();
	if (HasSignedInToday()) {
		return min(MAX_LATE_SIGN_IN_COUNT - GetHostPlayer()->GetSignInLateCountThisMonth(), iDay - iSignedInCount);
	} else {
		return min(MAX_LATE_SIGN_IN_COUNT - GetHostPlayer()->GetSignInLateCountThisMonth(), iDay - iSignedInCount - 1);
	}
}

bool CDlgSignIn::HasSignedInToday() {
	CECHostPlayer* pHost = GetHostPlayer();
	int iMonthCalendar = pHost->GetSignInMonthCalendar();
	int today = GetCurDay();
	return (1 << (today - 1)) & iMonthCalendar;
}

int CDlgSignIn::GetDaysInMonth() {
	tm time_server = g_pGame->GetServerLocalTime(m_iServerUpdateTime);
	int year = time_server.tm_year + 1900;
	int month = time_server.tm_mon + 1;
	int daysInMonth = DaysInMonth[month - 1];
	if (month == 2) {
		if (year % 400 == 0) {
			daysInMonth = 29;
		} else if (year % 100 != 0 && year % 4 == 0) {
			daysInMonth = 29;
		}
	}
	return daysInMonth;
}

void CDlgSignIn::ShowMonth(int iMonth) {
	ACString monthStr;
	monthStr.Format(_AL("%d"), iMonth);
	AUILabel* plblMonth = NULL;
	DDX_Control("Txt_Month", plblMonth);
	plblMonth->SetText(monthStr);
}

void CDlgSignIn::ShowSignedInDaysCount(int iSignedInDaysCount) {
	ACString signedInDaysStr;
	signedInDaysStr.Format(_AL("%d"), iSignedInDaysCount);
	AUILabel* plblDays = NULL;
	DDX_Control("Txt_Days", plblDays);
	plblDays->SetText(signedInDaysStr);
}

void CDlgSignIn::ShowCanLateSignDaysCount(int iCanLateSignDaysCount) {
	ACString canSignInDaysStr;
	canSignInDaysStr.Format(_AL("%d"), iCanLateSignDaysCount);
	AUILabel* plblCanSignInDays = NULL;
	DDX_Control("Txt_Signdays", plblCanSignInDays);
	plblCanSignInDays->SetText(canSignInDaysStr);
}

void CDlgSignIn::ResetAwardListDayRange() {
	int recvAwardCount = GetRecvAwardCount();
	int signedInCount = GetSignedInCount();
	int curDay = GetCurDay();

	int dayToShow;
	if (recvAwardCount == curDay) {
		dayToShow = curDay;
	} else {
		dayToShow = recvAwardCount + 1; 
	}

	int daysInMonth = GetDaysInMonth();
	int i;
	for (i = 1; i <= daysInMonth; i += AWARD_LIST_SIZE) {
		if (i <= dayToShow && dayToShow < i + AWARD_LIST_SIZE) {
			break;
		}
	}
	m_iAwardListFirstDay = i;
	m_iAwardListLastDay = m_iAwardListFirstDay + AWARD_LIST_SIZE - 1;
	if (m_iAwardListLastDay > daysInMonth) {
		m_iAwardListLastDay = daysInMonth;
	}
}

void CDlgSignIn::ShowAwardListDayRange() {
	AUILabel* plblFirstDay = NULL;
	DDX_Control("Txt_Days1", plblFirstDay);
	plblFirstDay->SetText(ACString().Format(_AL("%d"), m_iAwardListFirstDay));

	AUILabel* plblLastDay = NULL;
	DDX_Control("Txt_Days2", plblLastDay);
	plblLastDay->SetText(ACString().Format(_AL("%d"), m_iAwardListLastDay));
}

void CDlgSignIn::SetListMoveBtnEnable() {
	AUIStillImageButton *pbtnMoveLeft = NULL, *pbtnMoveRight = NULL;
	DDX_Control("Btn_DaysBefore", pbtnMoveLeft);
	DDX_Control("Btn_DaysLater", pbtnMoveRight);
	pbtnMoveLeft->Enable(true);
	pbtnMoveRight->Enable(true);
	if (m_iAwardListFirstDay == 1) {
		pbtnMoveLeft->Enable(false);
	}
	if (m_iAwardListFirstDay + AWARD_LIST_SIZE > GetDaysInMonth()) {
		pbtnMoveRight->Enable(false);
	}
}

void CDlgSignIn::OnCommandMoveLeft(const char* szCommand) {
	if (m_iAwardListFirstDay < AWARD_LIST_SIZE) {
		return;
	}
	m_iAwardListFirstDay -= AWARD_LIST_SIZE;
	m_iAwardListLastDay = m_iAwardListFirstDay + AWARD_LIST_SIZE - 1;
	ShowAwardListDayRange();
	SetListMoveBtnEnable();
	ShowAwardListItemsView();
}

void CDlgSignIn::OnCommandMoveRight(const char* szCommand) {
	int iDaysInMonth = GetDaysInMonth();
	if (m_iAwardListFirstDay + AWARD_LIST_SIZE > iDaysInMonth) {
		return;
	}
	m_iAwardListFirstDay += AWARD_LIST_SIZE;
	m_iAwardListLastDay += AWARD_LIST_SIZE;
	if (m_iAwardListLastDay > iDaysInMonth) {
		m_iAwardListLastDay = iDaysInMonth;
	}
	ShowAwardListDayRange();
	SetListMoveBtnEnable();
	ShowAwardListItemsView();
}

void CDlgSignIn::GetAwardItemInfo(int month, int day, int &itemID, int &itemCount) {
	itemID = m_signAwardConfigs[month - 1]->list[day - 1].id;
	itemCount = m_signAwardConfigs[month - 1]->list[day - 1].num;
}

void CDlgSignIn::ShowAwardListItemsView() {
	tm time_server = g_pGame->GetServerLocalTime(m_iServerUpdateTime);
	int month = time_server.tm_mon + 1;
	int day = time_server.tm_mday;
	int signedInDays = GetSignedInCount();
	int recvAwardDays = GetRecvAwardCount();
	int daysInMonth = GetDaysInMonth();
	
	int awardDay;
	for (awardDay = m_iAwardListFirstDay; awardDay < m_iAwardListFirstDay + AWARD_LIST_SIZE; awardDay++) {
		PAUIIMAGEPICTURE awardImg = NULL;
		PAUISTILLIMAGEBUTTON recvAwardBtn = NULL;
		DDX_Control(AString().Format("Btn_Award%02d", awardDay - m_iAwardListFirstDay + 1), recvAwardBtn);
		DDX_Control(AString().Format("Img_Item%02d", awardDay - m_iAwardListFirstDay + 1), awardImg);
		
		if (awardDay <= m_iAwardListLastDay) {
			int itemID, itemCount;
			GetAwardItemInfo(month, awardDay, itemID, itemCount);
			SetAwardBtn(awardDay, recvAwardBtn, signedInDays, recvAwardDays, day);
			SetAwardImg(awardImg, itemID, itemCount);
		} else {
			awardImg->SetCover(NULL, 0);
			awardImg->SetHint(_AL(""));
			awardImg->SetText(_AL(""));
			recvAwardBtn->Show(false);
		}
		
	}
}

void CDlgSignIn::SetAwardBtn(int awardDay, AUIStillImageButton* pBtn, int signedInDays, int recvAwardDays, int curDay) {
	// 已经领取的不可见
	if (awardDay <= recvAwardDays) {
		pBtn->Show(false);
		return;
	}
	// 第一个未领取的可点击
	if (awardDay == recvAwardDays + 1 && awardDay <= signedInDays) {
		pBtn->Show(true);
		pBtn->Enable(true);
		return;
	}
	// 剩下的不可点击
	pBtn->Show(true);
	pBtn->Enable(false);
}

void CDlgSignIn::SetAwardImg(AUIImagePicture* pImg, int itemID, int itemCount) {
	AUICTranslate trans;
	CECIvtrItem* item = CECIvtrItem::CreateItem(itemID, 0, 1);
	item->GetDetailDataFromLocal();
	GetGameUIMan()->SetCover(pImg, item->GetIconFile());
	pImg->SetHint(trans.Translate(item->GetDesc(CECIvtrItem::DESC_REWARD)));
	if (0 == itemCount) {
		pImg->SetText(_AL(""));
	} else {
		pImg->SetText(ACString().Format(_AL("%d"), itemCount));
	}
	delete item;

	return;
}

void CDlgSignIn::OnCommandGetDailyReward(const char* szCommand) {
	int emptySlotsCount = g_pGame->GetGameRun()->GetHostPlayer()->GetPack()->GetEmptySlotNum();
	if (emptySlotsCount == 0) {
		ACString strText;
		strText.Format(GetStringFromTable(10626), 1);
		GetGameUIMan()->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	} else {
		GetGameSession()->c2s_CmdSignInAward(0x8, 0);
	}
}

bool CDlgSignIn::IsSignInEnoughThisMonth() {
	int month = GetCurMonth();
	if (GetSignedInCount() >= DaysInMonth[month - 1] - 3) {
		return true;
	} else {
		return false;
	}
}

bool CDlgSignIn::IsSignInCompeleteThisMonth() {
	int month = GetCurMonth();
	if (GetSignedInCount() == DaysInMonth[month - 1]) {
		return true;
	} else {
		return false;
	}
}