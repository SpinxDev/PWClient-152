// Filename	: DlgCountryMap.cpp
// Creator	: Han Guanghui
// Date		: 2012/7/18

#include "DlgCountryMap.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EL_Precinct.h"
#include "A2DSprite.h"
#include "A3DDevice.h"
#include "A3DGdi.h"
#include "globaldataman.h"
#include "Home\\ClosedArea.h"
#include "A3DFlatCollector.h"
#include "EC_GameSession.h"
#include "countrybattlemove.hpp"
#include "countrybattlemove_re.hpp"
#include "countrybattlesyncplayerlocation.hpp"
#include "countrybattlegetmap_re.hpp"
#include "countrybattlegetscore_re.hpp"
#include "countrybattlegetconfig_re.hpp"
#include "countrybattlepreenternotify.hpp"
#include "countrybattlegetbattlelimit_re.hpp"
#include "Network\\ids.hxx"
#include "EC_DomainCountry.h"
#include "EC_Global.h"
#include "EC_CountryConfig.h"
#include "EC_UIConfigs.h"
#include "EC_Configs.h"
#include "EC_CrossServer.h"
#include "DlgCountryScore.h"
#include "DlgCountryWarMap.h"
#include "DlgMessageBox.h"
#include "DlgKingCommand.h"
#include "DlgCountryMapSub.h"
#include <A3DFTFont.h>
#include <AUICheckBox.h>

static CDlgCountryMap::TimeType s_GetConfigPeriod = CDlgCountryMap::ConvertFromSeconds(10);
static CDlgCountryMap::TimeType s_GetMapPeriod = CDlgCountryMap::ConvertFromSeconds(10);
static CDlgCountryMap::TimeType s_StopMovePeriod = CDlgCountryMap::ConvertFromSeconds(1);
static CDlgCountryMap::TimeType s_GetScorePeriod = CDlgCountryMap::ConvertFromSeconds(20);

AUI_BEGIN_COMMAND_MAP(CDlgCountryMap, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Chat",		OnCommandChat)
AUI_ON_COMMAND("Btn_Refresh",	OnCommandRefresh)
AUI_ON_COMMAND("Btn_Back",		OnCommandBack)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgCountryMap, CDlgBase)

AUI_ON_EVENT("CountryMap",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_ON_EVENT("CountryMap",	WM_LBUTTONDBLCLK,	OnEventLButtonDBClick)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONDOWN,	OnEventRButtonDown)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONUP,	OnEventRButtonUp)
AUI_ON_EVENT("CountryMap",	WM_RBUTTONDBLCLK,	OnEventRButtonDBClick)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONDOWN,	OnEventMButtonDown)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONUP,	OnEventMButtonUp)
AUI_ON_EVENT("CountryMap",	WM_MBUTTONDBLCLK,	OnEventMButtonDBClick)

AUI_END_EVENT_MAP()


CDlgCountryMap::CDlgCountryMap()
: m_pBtn_Back(NULL)
, m_pBtn_Refresh(NULL)
, m_domainLoc(0)
, m_bGetMap(false)
, m_moveState(MS_STOPPED)
, m_cntGetMap(0)
, m_cntStopMove(0)
, m_cntMoveStart(0)
, m_cntMoveEnd(0)
, m_bGetConfig(false)
, m_cntGetConfig(0)
, m_bVersionOK(false)
, m_bOpen(false)
, m_countryStartTime(0)
, m_countryEndTime(0)
, m_nTotalBonus(0)
, m_nTotalScore(0)
, m_cntGetScore(0)
, m_bLoadCenter(false)
, m_vLoadCenter(0, 0, 0)
, m_iLimitDomainCnt(0)
, m_pSubDlgControl(NULL)
, m_pCountryMapSub(NULL)
{
	m_route.clear();
	memset(m_nCountryScore, 0, sizeof(m_nCountryScore));
	memset(m_iKingID, 0, sizeof(m_iKingID));
	memset(m_pLbl_KingName, 0, sizeof(m_pLbl_KingName));
}

CDlgCountryMap::~CDlgCountryMap()
{
}

bool CDlgCountryMap::OnInitDialog()
{
	DDX_Control("Btn_Back",m_pBtn_Back);
	DDX_Control("Btn_Refresh",m_pBtn_Refresh);
	DDX_Control("Sub_View", m_pSubDlgControl);
	m_pCountryMapSub = dynamic_cast<CDlgCountryMapSub*>(m_pSubDlgControl->GetSubDialog());
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Reward"));
	if (pImg && pImg->GetImage())
		pImg->GetImage()->SetLinearFilter(true);
	pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_King"));
	if (pImg && pImg->GetImage())
		pImg->GetImage()->SetLinearFilter(true);
	char szName[40];
	for( int i = 0;i < DOMAIN2_INFO::COUNTRY_COUNT;i++ )
	{
		_snprintf(szName, 40, "Img_0%d", i+1);
		pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szName));
		if( pImg && pImg->GetImage() ) 
			pImg->GetImage()->SetLinearFilter(true);
	}
	for( i=0;i<DOMAIN2_INFO::COUNTRY_COUNT;i++ )
	{
		_snprintf(szName, 40, "Lbl_Name%d", i+1);
		DDX_Control(szName, m_pLbl_KingName[i]);
		if( !m_pLbl_KingName[i] ) break;
		m_pLbl_KingName[i]->SetText(_AL(""));
		m_pLbl_KingName[i]->SetData(0);
	}

	return true;
}

void CDlgCountryMap::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

static void LogRoute(const char *title, const CECDomainCountryInfo::Route &r)
{
	AString str;

	const int BUF_LEN = 32;
	char szBuf[BUF_LEN] = {0};

	_snprintf(szBuf, BUF_LEN, "size=%d,cost=%d", r.size(), r.cost);
	str += szBuf;
	for (size_t i(0); i<r.size(); ++ i)
	{
		const CECDomainCountryInfo::RouteNode &node = r.nodes[i];
		_snprintf(szBuf, BUF_LEN, ",%d(%d)", node.idDomain, node.cost);
		str += szBuf;
	}

	a_LogOutput(1, "%s=%s", title, str);
}

void CDlgCountryMap::LogState()const
{
	a_LogOutput(1, "m_domainLoc=%d, m_bGetMap=%d, m_bGetConfig=%d, m_bVersionOK=%d, m_bOpen=%d"
		, m_domainLoc, m_bGetMap, m_bGetConfig, m_bVersionOK, m_bOpen);	
	a_LogOutput(1, "m_countryStartTime=%d, m_countryEndTime=%d, m_nTotalBonus=%d, m_nTotalScore=%d, m_iLimitDomainCnt=%d"
		, m_countryStartTime, m_countryEndTime, m_nTotalBonus, m_nTotalScore, m_iLimitDomainCnt);
	a_LogOutput(1, "m_moveState=%d, m_domainNext=%d, m_cntMoveStart=%u, m_cntMoveEnd=%u"
		, m_moveState, m_domainNext, m_cntMoveStart, m_cntMoveEnd);

	TimeType currentTime = GetCurrentTime();
	a_LogOutput(1, "current time=%u, m_cntGetConfig=%u(%u), m_cntGetMap=%u(%u), m_cntStopMove=%u(%u), m_cntGetScore=%u(%u)"
		, currentTime
		, m_cntGetConfig, a_Max(currentTime, m_cntGetConfig)-m_cntGetConfig
		, m_cntGetMap, a_Max(currentTime, m_cntGetMap)-m_cntGetMap
		, m_cntStopMove, a_Max(currentTime, m_cntStopMove)-m_cntStopMove
		, m_cntGetScore, a_Max(currentTime, m_cntGetScore)-m_cntGetScore);

	AString str;
	char szBuf[32] = {0};
	for (int i(0); i < ARRAY_SIZE(m_iKingID); ++ i)
	{
		_snprintf(szBuf, ARRAY_SIZE(szBuf), "king%d=%d,", i, m_iKingID[i]);
		str += szBuf;
	}
	a_LogOutput(1, str);

	LogRoute("m_route", m_route);
	LogRoute("m_routeCand", m_routeCand);
}

void CDlgCountryMap::OnClickDomain(int nHoverIndex, LPARAM lParam)
{
	if (!GetGameSession()->IsConnected()){
		return;
	}

	if (IsOpen() && !IsVersionOK())
	{
		AddMessage(9998);
		return;
	}

	if (!IsCanMove()){
		return;
	}
	
	A3DPOINT2 pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	
	if (nHoverIndex == CECDomain::OUT_OF_DOMAIN){
		return;
	}
	
	if (!IsStopped() && !IsWaitingArrive()){
		return;
	}
	
	CECDomainCountryInfo *pInfo = CECDomainCountryInfo::GetInstance();
	const DOMAIN2_INFO &domain = pInfo->Get(nHoverIndex);
	
	// 处理国王的情形
	if( GetHostPlayer()->IsKing() )
	{
		A3DVIEWPORTPARAM* p = m_pA3DEngine->GetActiveViewport()->GetParam();
		PopupKingOrderMenu(pt.x - p->X, pt.y - p->Y, domain.id);
		return;
	}
	
	if (domain.id == m_domainLoc)
	{
		if (IsWaitingArrive())
			StopMove();
		return;
	}
	if (domain.id == m_route.getDst())
	{
		return;
	}
	
	CECDomainCountryInfo::Route route;
	CECDomainCountryInfo::FindRouteResult result;
	result = pInfo->FindRoute(m_domainLoc, domain.id, route);	
		
	switch (result)
	{
	case CECDomainCountryInfo::FRR_INVALID:
	case CECDomainCountryInfo::FRR_INPLACE:
		break;
		
	case CECDomainCountryInfo::FRR_LOCKED:		AddMessage(9875); break;
	case CECDomainCountryInfo::FRR_BLOCKED:		AddMessage(9876); break;
	case CECDomainCountryInfo::FRR_FULL:		AddMessage(9877); break;
	case CECDomainCountryInfo::FRR_OTHERS_WAR:	AddMessage(9878); break;
	case CECDomainCountryInfo::FRR_COOLDOWN:	AddMessage(9879); break;
		
	case CECDomainCountryInfo::FRR_CAN_MOVETO:
	case CECDomainCountryInfo::FRR_CAN_ATTACK:
		{
			m_routeCand.swap(route);
			m_pCountryMapSub->DropCapture();
			ACString strText;
			strText.Format(GetGameUIMan()->GetStringFromTable(result==CECDomainCountryInfo::FRR_CAN_MOVETO ? 9880:9881), domain.name);
			GetGameUIMan()->MessageBox("Country_Move", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
			
		}
		break;
	case CECDomainCountryInfo::FRR_CAN_ASSIST:
		{
			m_routeCand.swap(route);
			m_pCountryMapSub->DropCapture();
			ACString strText = GetGameUIMan()->GetStringFromTable(9882);
			GetGameUIMan()->MessageBox("Country_Move", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
		}
		break;
	}
}

void CDlgCountryMap::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventLButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
	ShowFalse();
}

void CDlgCountryMap::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventRButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventMButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnEventMButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ChangeFocus(GetDlgItem("Btn_Back"));
}

void CDlgCountryMap::OnCommandCancel(const char * szCommand)
{
	ShowFalse();
}

void CDlgCountryMap::OnCommandChat(const char * szCommand)
{
	ToggleChatDlg();
}

void CDlgCountryMap::OnCommandRefresh(const char * szCommand)
{
	GetScore();
}

void CDlgCountryMap::OnCommandBack(const char * szCommand)
{
	if (CanBackToCapital())
		GetGameSession()->country_BackToCapital();
}

void CDlgCountryMap::ToggleChatDlg()
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();

	PAUIDIALOG pDlgChat = pGameUIMan->GetDialog("Win_Chat");
	PAUIDIALOG pDlgChatSmall = pGameUIMan->GetDialog("Win_ChatSmall");
	PAUIDIALOG pDlgChatBig = pGameUIMan->GetDialog("Win_ChatBig");
	
	pGameUIMan->BringWindowToTop(pDlgChat);
	pGameUIMan->BringWindowToTop(pDlgChatSmall);
	pGameUIMan->BringWindowToTop(pDlgChatBig);
}

void CDlgCountryMap::GetMap()
{
	//	定时向服务器获取地图信息
	TimeType currentTime = GetCurrentTime();
	if (m_cntGetMap <= currentTime && GetGameSession()->IsConnected())
	{
		m_cntGetMap = currentTime + s_GetMapPeriod;
		GetGameSession()->country_GetMap();
	}
}

void CDlgCountryMap::StopMove()
{
	//	一定间隔发送停止移动
	if (!IsWaitingArrive()) return;
	TimeType currentTime = GetCurrentTime();
	if (m_cntStopMove <= currentTime && GetGameSession()->IsConnected())
	{
		m_cntStopMove = currentTime + s_StopMovePeriod;
		GetGameSession()->country_Move(-1);
		m_moveState = MS_WAITING_STOP;
		ClearMoreRoute();
	}
}

void CDlgCountryMap::GetScore()
{
	TimeType currentTime = GetCurrentTime();
	if (CanGetScore() && GetGameSession()->IsConnected())
	{
		m_cntGetScore = currentTime + s_GetScorePeriod;
		GetGameSession()->country_GetScore();
	}
}

void CDlgCountryMap::GetConfig()
{
	TimeType currentTime = GetCurrentTime();
	if (m_cntGetConfig <= currentTime && GetGameSession()->IsConnected())
	{
		m_cntGetConfig = currentTime + s_GetConfigPeriod;
		GetGameSession()->country_GetConfig();
	}
}

void CDlgCountryMap::StartMove()
{
	if (!IsCanMove() ||
		!IsStopped() ||
		m_route.size() < 2)
	{
		ClearMove();
		ASSERT(false);
		return;
	}

	GetGameSession()->country_Move(m_route.getNext());

	m_moveState = MS_WAITING_MOVE;	//	设置等待移动状态
	m_domainNext = 0;				//	清空移动目标、移动计时
	m_cntMoveStart = m_cntMoveEnd = 0;
}

void CDlgCountryMap::ContinueMove()
{
	//	已到达目标、检查路径并往下一节点移动

	if (!IsCanMove())
	{
		ASSERT(false);
		ClearMove();
		return;
	}

	switch (m_moveState)
	{
	case MS_STOPPED:
	case MS_WAITING_MOVE:
		ASSERT(false);
		break;

	case MS_WAITING_STOP:
	case MS_WAITING_REDIRECT:
		ClearMove();
		break;
		
	case MS_WAITING_ARRIVE:
		if (m_route.size() <= 2)
		{
			//	到达最终目标
			ClearMove();
		}
		else
		{
			m_route.removeHead(1);
			m_moveState = MS_STOPPED;
			StartMove();
		}
		break;
	}
}

void CDlgCountryMap::ClearMove()
{
	//	停止移动、清空路径
	m_moveState = MS_STOPPED;
	m_domainNext = 0;
	m_cntMoveStart = m_cntMoveEnd = 0;
	ClearRoute();
}

void CDlgCountryMap::ClearRoute()
{
	//	清除后续移动路径
	m_route.clear();
}

void CDlgCountryMap::ClearMoreRoute()
{
	//	清除后续移动路径
	if (m_route.size() < 2)
	{
		ASSERT(false);
		return;
	}
	m_route.removeTail(m_route.size()-2);
}

CDlgCountryMap::TimeType CDlgCountryMap::GetCurrentTime()
{
	return timeGetTime();
};

int	CDlgCountryMap::ConverToSeconds(TimeType t)
{
	return t/1000;
}

CDlgCountryMap::TimeType CDlgCountryMap::ConvertFromSeconds(int sec)
{
	return static_cast<TimeType>(sec * 1000);
}

CDlgCountryMap::TimeType CDlgCountryMap::GetTimeLeft(TimeType endTime)
{
	TimeType t = GetCurrentTime();
	return (t >= endTime) ? 0 : (endTime-t);
}

CDlgCountryMap::TimeType CDlgCountryMap::GetNodeMoveTimeLeft()const
{
	//	获取到达邻居节点剩余时间
	return GetTimeLeft(m_cntMoveEnd);
}

CDlgCountryMap::TimeType CDlgCountryMap::GetNodeMoveTime()const
{
	//	获取到达邻居节点将花费时间
	return m_cntMoveEnd - m_cntMoveStart;
}

CDlgCountryMap::TimeType CDlgCountryMap::GetRouteMoveTime()const
{
	//	获取当前所处节点到达最终目标花费时间
	//	MS_WAITING_REDIRECT 状态下，m_route 存放重定向的新路径而非之前路径
	return IsWaitingRedirect() ? GetNodeMoveTime() : ConvertFromSeconds(m_route.cost);
}

CDlgCountryMap::TimeType CDlgCountryMap::GetRouteMoveTimeLeft()const
{
	//	获取到达最终目标剩余时间
	TimeType pass = GetNodeMoveTime() - GetNodeMoveTimeLeft();
	return GetRouteMoveTime() - pass;
}

void CDlgCountryMap::AddMessage(int idString)
{
	if (IsShow()) {
		m_pCountryMapSub->DropCapture();
		GetGameUIMan()->MessageBox("", GetStringFromTable(idString), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgCountryMap::OnTick()
{	
	GetMap();
	UpdateRestTime();
	if (m_pBtn_Back) m_pBtn_Back->Enable(CanBackToCapital());
	if (m_pBtn_Refresh) m_pBtn_Refresh->Enable(CanGetScore());

	// 更新国王名字
	for( int i=0;i<DOMAIN2_INFO::COUNTRY_COUNT;i++ )
	{
		if( m_pLbl_KingName[i] && m_pLbl_KingName[i]->GetData() )
		{
			const ACHAR* szName = GetGameRun()->GetPlayerName(m_iKingID[i], false);
			if( szName )
			{
				m_pLbl_KingName[i]->SetText(szName);
				m_pLbl_KingName[i]->SetHint(szName);
				m_pLbl_KingName[i]->SetData(0);
			}
		}
	}
}

CDlgCountryMove * CDlgCountryMap::GetDlgCountryMove()
{
	return dynamic_cast<CDlgCountryMove*>(GetGameUIMan()->GetDialog("Win_CountryMove"));
}

void CDlgCountryMap::UpdateRestTime()
{
	CDlgCountryMove *pDlgCountryMove = GetDlgCountryMove();
	if (IsWaitingArrive() || IsWaitingStop() || IsWaitingRedirect())
	{
		if (!pDlgCountryMove->IsShow())
			pDlgCountryMove->Show(true);
		
		BringDialogToBefore(pDlgCountryMove);
	}
	else if (pDlgCountryMove->IsShow())
	{
		pDlgCountryMove->Show(false);
	}
}

void CDlgCountryMap::BringDialogToBefore(PAUIDIALOG pDlg)
{
	if (!IsShow() || !pDlg || !pDlg->IsShow())
		return;
	GetGameUIMan()->InsertDialogBefore(pDlg, m_pCountryMapSub);
}

static int GetHourFromSeconds(int sec)
{
	return sec/3600;
}

static int GetMinFromSeconds(int sec)
{
	return (sec%3600)/60;
}

void CDlgCountryMap::UpdateAdditionalInfo()
{
	//	统计各国家总人数
	int players[DOMAIN2_INFO::COUNTRY_COUNT] = {0};
	if (IsMapReady())
	{
		const CECDomainCountryInfo *pInfo = CECDomainCountryInfo::GetInstance();
		for (size_t i(0); i < pInfo->GetSize(); ++ i)
		{
			const DOMAIN2_INFO &domain = pInfo->Get((int)i);
			for (int j(0); j < DOMAIN2_INFO::COUNTRY_COUNT; ++ j)
				players[j] += domain.player_count[j];
		}
	}

	//	显示各国家名称、人数、比分
	AString strName;
	ACString strText;
	PAUIOBJECT pObj = NULL;
	for (int i(0); i < DOMAIN2_INFO::COUNTRY_COUNT; ++ i)
	{
		strName.Format("Img_%d", i+1);
		if (pObj = GetDlgItem(strName))
			pObj->SetHint(*CECCountryConfig::Instance().GetName(i+1));

		strName.Format("Lbl_People%d", i+1);
		if (pObj = GetDlgItem(strName))
			pObj->SetText(strText.Format(_AL("%d"), players[i]));
		
		strName.Format("Lbl_Point%d", i+1);
		if (pObj = GetDlgItem(strName))
			pObj->SetText(strText.Format(_AL("%d"), m_nCountryScore[i]));
	}

	//	个人总积分
	if (pObj = GetDlgItem("Lbl_Point"))
		pObj->SetText(strText.Format(GetStringFromTable(9913), m_nTotalScore));

	//	总奖励
	PAUIIMAGEPICTURE pImg_Reward = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Reward"));
	if (pImg_Reward)
	{
		int nLevel = CECUIConfig::Instance().GetGameUI().GetCountryWarBonusLevel(m_nTotalBonus);

		int nMaxFrame = 0;
		AUIOBJECT_SETPROPERTY property;
		if (pImg_Reward->GetProperty("Frames Number", &property) && property.i >= 0)
			nMaxFrame = property.i;

		if (nLevel < 0 || nLevel >= nMaxFrame)
			pImg_Reward->Show(false);
		else
		{
			pImg_Reward->Show(true);
			pImg_Reward->FixFrame(nLevel);
			pImg_Reward->SetHint(strText.Format(GetStringFromTable(9933), m_nTotalBonus));
		}
	}

	//	国战时间
	PAUIOBJECT pObjTime = GetDlgItem("Lbl_Time");
	if (pObjTime)
	{
		ACString strText;
		if (m_bGetConfig)
		{
			if (IsOpen())
				strText.Format(GetStringFromTable(9988),
				GetHourFromSeconds(m_countryStartTime), GetMinFromSeconds(m_countryStartTime),
				GetHourFromSeconds(m_countryEndTime), GetMinFromSeconds(m_countryEndTime));
			else
				strText = GetStringFromTable(9989);
		}
		pObjTime->SetText(strText);
	}
}

void CDlgCountryMap::OnShowDialog()
{
	if (!IsStopped())
	{
		ASSERT(false);
	}
	
	GetConfig();

	UpdateAdditionalInfo();
	GetGameUIMan()->GetDialog("Win_Chat")->SetBackgroundDialog(false);

	char szName[40];
	PAUIOBJECT pObj;
	GetDlgItem("Img_King")->Show(CECCrossServer::Instance().IsOnSpecialServer());
	for( int i=0;i<DOMAIN2_INFO::COUNTRY_COUNT;i++ )
	{
		_snprintf(szName, 40, "Img_0%d", i+1);
		pObj = GetDlgItem(szName);
		if( pObj ) pObj->Show(CECCrossServer::Instance().IsOnSpecialServer());
		_snprintf(szName, 40, "Lbl_Name%d", i+1);
		pObj = GetDlgItem(szName);
		if( pObj ) pObj->Show(CECCrossServer::Instance().IsOnSpecialServer());
	}
}

void CDlgCountryMap::OnHideDialog()
{
	GetGameUIMan()->GetDialog("Win_Chat")->SetBackgroundDialog(true);
	GetDlgCountryMove()->Show(false);
	GetGameUIMan()->GetDialog("Win_KingOrder")->Show(false);
	GetGameUIMan()->GetDialog("Win_KingSet")->Show(false);

	PAUIDIALOG pDlgMoveConfirm = GetGameUIMan()->GetDialog("Country_Move");
	if (pDlgMoveConfirm)
		pDlgMoveConfirm->OnCommand("IDCANCEL");

	CommitLoadCenter();
}

void CDlgCountryMap::ShowFalse()
{	
	if (GetGameSession()->IsConnected()) //	避免断开链接后无法关闭客户端
	{
		if (!IsStopped())
		{
			if (GetWorld()->IsCountryMap()){
				return;	//	防止意外传送等情况
			}
			ClearMove();
		}
	}

	Show(false);
}


void CDlgCountryMap::OnPrtcCountry(GNET::Protocol* pProtocol)
{
	switch (pProtocol->GetType())
	{
	case PROTOCOL_COUNTRYBATTLEMOVE_RE:
		OnPrtcCountryMove_Re(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLESYNCPLAYERLOCATION:
		OnPrtcCountrySynLocation(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLEGETMAP_RE:
		OnPrtcCountryGetMap_Re(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLEGETSCORE_RE:
		OnPrtcCountryGetScore_Re(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLEGETCONFIG_RE:
		OnPrtcCountryGetConfig_Re(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLEPREENTERNOTIFY:
		OnPrtcCountryEnterNotify(pProtocol);
		break;

	case PROTOCOL_COUNTRYBATTLEGETBATTLELIMIT_RE:
		OnPrtcCountryBattleLimit_Re(pProtocol);
		break;
	}
}

void CDlgCountryMap::OnPrtcCountryMove_Re(GNET::Protocol* pProtocol)
{
	//	收到此协议后开始/继续移动

	using namespace GNET;
	CountryBattleMove_Re *p = dynamic_cast<CountryBattleMove_Re*>(pProtocol);
	if (p->retcode != 0)
	{
		//	操作被服务器拒绝
		switch (m_moveState)
		{
		case MS_WAITING_STOP:
			ASSERT(p->dest == -1);
			AddMessage(9782);
			m_moveState = MS_WAITING_ARRIVE;
			ClearMoreRoute();
			break;

		case MS_WAITING_MOVE:
			ASSERT(m_route.size() >= 2 && m_route.getNext() == p->dest);
			AddMessage(9783);
			ClearMove();
			break;

		case MS_WAITING_REDIRECT:
			ASSERT(m_route.size() >= 2 && m_route.getNext() == p->dest);
			AddMessage(9784);
			m_moveState = MS_WAITING_ARRIVE;

			//	恢复当前移动路径，但删除后续路径
			ClearMoreRoute();
			if (m_route.size() == 2)
			{
				CECDomainCountryInfo::RouteNode &node = m_route.nodes[1];
				node.idDomain = m_domainNext;
				node.cost = ConverToSeconds(GetNodeMoveTime());
				m_route.setCost(node.cost);
			}
			break;

		default:
			ASSERT(false);
			ClearMove();
		}
		return;
	}

	switch (m_moveState)
	{
	case MS_WAITING_STOP:
		ASSERT(p->dest == -1);
		ClearMove();
		break;

	case MS_WAITING_MOVE:
	case MS_WAITING_REDIRECT:
		if (m_route.size() < 2 || m_route.getNext() != p->dest)
		{
			ASSERT(false);
			ClearMove();
		}
		else
		{
			m_moveState = MS_WAITING_ARRIVE;
			m_cntMoveStart = GetCurrentTime();
			m_cntMoveEnd = m_cntMoveStart + ConvertFromSeconds(m_route.nodes[1].cost);
			m_domainNext = m_route.getNext();
		}
		break;

	default:
		ASSERT(false);
		ClearMove();
	}
}

void CDlgCountryMap::OnPrtcCountrySynLocation(GNET::Protocol* pProtocol)
{	
	using namespace GNET;

	enum{
		REASON_SYNC_MOVE = 0,		//	上一次移动请求成功完成
		REASON_SYNC_PREV_STEP,		//	异常：返回上一块领土
		REASON_SYNC_CAPITAL,		//	异常：返回首都
		REASON_SYNC_CLI_REQUEST,	//	客户端请求返回
		REASON_SYNC_FINISH,			//	战场结束，清除客户端移动状态
	};

	CountryBattleSyncPlayerLocation *p = dynamic_cast<CountryBattleSyncPlayerLocation*>(pProtocol);	

	m_domainLoc = p->domain_id;
	
	switch (p->reason)
	{
	case REASON_SYNC_MOVE:
		ContinueMove();
		break;
		
	case REASON_SYNC_PREV_STEP:
		AddMessage(9784);
		ClearMove();
		StopEnterWarCountDown(true);
		break;
		
	case REASON_SYNC_CAPITAL:
		AddMessage(9785);
		ClearMove();
		StopEnterWarCountDown(true);
		break;
		
	case REASON_SYNC_CLI_REQUEST:
		break;

	case REASON_SYNC_FINISH:
		m_bOpen = false;
		ClearMove();
		StopEnterWarCountDown(true);
		break;
		
	default:
		a_LogOutput(1, "Unknown country syn location reason %d", p->reason);
		ASSERT(false);
	}
}

void CDlgCountryMap::OnPrtcCountryGetMap_Re(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CountryBattleGetMap_Re *p = dynamic_cast<CountryBattleGetMap_Re*>(pProtocol);
	if (p->retcode != 0)
	{
		//	错误处理
		a_LogOutput(1, "CountryBattleGetMap return %d", p->retcode);
		return;
	}

	CECDomainCountryInfo* pInfo = CECDomainCountryInfo::GetInstance();
	pInfo->ClearState();
	if (p->domains.size() != pInfo->GetSize())
	{
		//	出错处理
		ASSERT(false);
		a_LogOutput(1, "Invalid country domain size refused(client=%d, server=%d)",	pInfo->GetSize(), p->domains.size());
		ClearMove();
		return;
	}

	m_iLimitDomainCnt = 0;
	
	//	更新状态
	TimeType currentTime = GetCurrentTime();
	for (size_t i(0); i < p->domains.size(); ++ i)
	{
		const GCountryBattleDomain &state = p->domains[i];
		DOMAIN2_INFO *pDomain = pInfo->Find(state.id);
		if (!pDomain)
		{
			ASSERT(false);
			a_LogOutput(1, "Invalid country domain id=%d, state cleared", state.id);
			pInfo->ClearState();
			ClearMove();
			return;
		}

		pDomain->owner = state.owner;
		pDomain->challenger = state.challenger;
		pDomain->status = static_cast<Domain2Status>(state.status);
		
		if (pDomain->HasEndTime())
			pDomain->end_time = ConvertFromSeconds(state.time) + currentTime;
		else pDomain->end_time = 0;
		
		if (state.country_playercnt.size() != DOMAIN2_INFO::COUNTRY_COUNT)
		{
			ASSERT(false);
			a_LogOutput(1, "Invalid country size (at index=%d, id=%d), state cleared", i, state.id);
			pInfo->ClearState();
			ClearMove();
			return;
		}

		for (int j(0); j < DOMAIN2_INFO::COUNTRY_COUNT; ++ j)
			pDomain->player_count[j] = state.country_playercnt[j];

		pDomain->byStateMask = state.battle_config_mask;
		if( pDomain->byStateMask & DOMAIN2_INFO::STATE_RESTRICT )
		{
			GetGameSession()->country_GetBattleLimit(pDomain->id);
			m_iLimitDomainCnt++;
		}
		else
		{
			pDomain->iPlayer.clear();
			pDomain->iGhost.clear();
		}
	}
	
	//	检查客户端对领土状态要求
	int breakIndex(-1);
	if (!pInfo->Validate(&breakIndex))
	{
		ASSERT(false);
		a_LogOutput(1, "Invalid country domain state (break at index=%d, id=%d), state cleared", breakIndex, pInfo->Get(breakIndex).id);
		pInfo->ClearState();
		ClearMove();
		return;
	}

	// 取四个国王的名字
	if( p->kings.size() == DOMAIN2_INFO::COUNTRY_COUNT )
	{
		for( size_t i=0;i<DOMAIN2_INFO::COUNTRY_COUNT;i++ )
		{
			if( m_pLbl_KingName[i] && m_iKingID[i] != p->kings[i] )
			{
				m_iKingID[i] = p->kings[i];

				const ACHAR* szName = GetGameRun()->GetPlayerName(m_iKingID[i], false);
				if( szName )
				{
					m_pLbl_KingName[i]->SetText(szName);
					m_pLbl_KingName[i]->SetHint(szName);
				}
				else
				{
					m_pLbl_KingName[i]->SetData(1);		// 需要更新国王的名字
					GetGameSession()->CacheGetPlayerBriefInfo(1, &m_iKingID[i], 2);
				}
			}
		}
	}
	else
	{
		ASSERT(0);
		a_LogOutput(1, "CDlgCountryMap::OnPrtcCountryGetMap_Re, Invalid country king count(%d)!", p->kings.size());
		return;
	}

	//	数据合法
	m_bGetMap = true;
	m_cntGetMap = GetCurrentTime() + s_GetMapPeriod;
	if (IsShow())
		UpdateAdditionalInfo();
	if (GetGameUIMan()->m_pDlgCountryWarMap->IsShow())
	{
		GetGameUIMan()->m_pDlgCountryWarMap->OnGetMapInfo();
	}
}

void CDlgCountryMap::OnPrtcCountryGetScore_Re(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CountryBattleGetScore_Re *p = dynamic_cast<CountryBattleGetScore_Re*>(pProtocol);
	m_nTotalScore = p->player_score;
	if (p->country_score.size() != DOMAIN2_INFO::COUNTRY_COUNT)
	{
		ASSERT(false);
		memset(m_nCountryScore, 0, sizeof(m_nCountryScore));
	}
	else
	{
		for (int i(0); i < DOMAIN2_INFO::COUNTRY_COUNT; ++ i)
			m_nCountryScore[i] = p->country_score[i];
	}
	UpdateAdditionalInfo();
}

void CDlgCountryMap::OnPrtcCountryGetConfig_Re(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CountryBattleGetConfig_Re *p = dynamic_cast<CountryBattleGetConfig_Re*>(pProtocol);
	m_bOpen = (p->is_battle_open != 0);
	m_countryStartTime = p->start_timestamp;
	m_countryEndTime = p->end_timetamp;
	m_nTotalBonus = p->bonus;

	m_bGetConfig = true;
	m_cntGetConfig = GetCurrentTime() + s_GetConfigPeriod;
	DWORD localTimeStamp = globaldata_getdomain2_timestamp(p->domain2_datatype);
	m_bVersionOK = (localTimeStamp == p->domain2_datat_timestamp);
	if (!m_bVersionOK)
 		a_LogOutput(1, "CDlgCountryMap::OnPrtcCountryGetConfig_Re, Invalid version(local=%u, server=%u)!", localTimeStamp, p->domain2_datat_timestamp);
	
	int lastType = CECDomainCountryInfo::GetInstance()->GetDataType();
	if (!CECDomainCountryInfo::GetInstance()->Initialize(p->domain2_datatype)){
		a_LogOutput(1, "CDlgCountryMap::OnPrtcCountryGetConfig_Re, Invalid domain_type=%d!", p->domain2_datatype);
	}
	m_pCountryMapSub->UpdateImage(lastType != CECDomainCountryInfo::GetInstance()->GetDataType());

	UpdateAdditionalInfo();

	if (GetHostPlayer()->IsGM())
		GetGameUIMan()->AddChatMessage(ACString().Format(_AL("Hello GM, country war total bonus is %d"), m_nTotalBonus), GP_CHAT_LOCAL);

	if (GetHostPlayer()->IsKing())
		GetGameSession()->king_GetKingPoint();
}

void CDlgCountryMap::OnPrtcCountryEnterNotify(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CountryBattlePreEnterNotify *p = dynamic_cast<CountryBattlePreEnterNotify *>(pProtocol);

	if (p->roleid != GetHostPlayer()->GetCharacterID())
	{
		ASSERT(false);
		return;
	}

	m_pCountryMapSub->DropCapture();
	ACString strMsg;
	strMsg.Format(GetStringFromTable(9928), p->timeout);
	if (!GetGameUIMan()->NewMessageBox("Country_EnterWar", strMsg, MB_OK))
		return;
	
	CDlgMessageBox *pMsgBox = GetGameUIMan()->GetNewMessageBox();
	pMsgBox->SetText(CDlgMessageBox::BTN_OK, GetStringFromTable(9929));

	pMsgBox->SetData(p->battle_id);
	pMsgBox->SetDataPtr((void *)(GetCurrentTime() + ConvertFromSeconds(p->timeout)));
}

void CDlgCountryMap::OnPrtcCountryBattleLimit_Re(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	CountryBattleGetBattleLimit_Re* p = dynamic_cast<CountryBattleGetBattleLimit_Re*>(pProtocol);

	CECDomainCountryInfo* pInfo = CECDomainCountryInfo::GetInstance();
	DOMAIN2_INFO *pDomain = pInfo->Find(p->domain_id);
	if (!pDomain)
	{
		ASSERT(false);
		a_LogOutput(1, "CDlgCountryMap::OnPrtcCountryBattleLimit_Re, Invalid country domain id=%d!", p->domain_id);
		return;
	}

	if( !(pDomain->byStateMask & DOMAIN2_INFO::STATE_RESTRICT) ||
		(GetHostPlayer()->IsKing() && p->limit.size() != NUM_PROFESSION) ||
		(!GetHostPlayer()->IsKing() && p->limit.size() != 1) )
	{
		ASSERT(0);
		a_LogOutput(1, "CDlgCountryMap::OnPrtcCountryBattleLimit_Re, Invalid domain battle limit data!");
		return;
	}

	pDomain->iPlayer.clear();
	pDomain->iGhost.clear();

	for( size_t i=0;i<p->limit.size();i++ )
	{
		pDomain->iPlayer.push_back(p->limit[i].occupation_cnt_ceil);
		pDomain->iGhost.push_back(p->limit[i].minor_str_floor);
	}
}

bool CDlgCountryMap::IsEnterWarCountDown()
{
	CDlgMessageBox *pMsgBox = GetGameUIMan()->GetNewMessageBox();
	return pMsgBox->IsShow() && !stricmp(pMsgBox->GetContext(), "Country_EnterWar");
}

void CDlgCountryMap::UpdateEnterWarCountDown()
{
	if (!IsEnterWarCountDown())	return;
	
	CDlgMessageBox *pMsgBox = GetGameUIMan()->GetNewMessageBox();
	TimeType end_time = (TimeType)pMsgBox->GetDataPtr();
	TimeType leftTime = GetTimeLeft(end_time);
	int nsec = ConverToSeconds(leftTime);
	
	ACString strMsg;
	strMsg.Format(GetStringFromTable(9928), nsec);
	pMsgBox->SetMessage(strMsg);

	//	到点未点击确定，则强制发送
	if (nsec <= 0) pMsgBox->OnCommand("IDOK");
}

void CDlgCountryMap::StopEnterWarCountDown(bool bLogLeftTime/* = false*/)
{	
	if (IsEnterWarCountDown())
	{
		CDlgMessageBox *pMsgBox = GetGameUIMan()->GetNewMessageBox();
		if (bLogLeftTime)
		{
			TimeType end_time = (TimeType)pMsgBox->GetDataPtr();
			TimeType leftTime = GetTimeLeft(end_time);
			int nsec = ConverToSeconds(leftTime);
			a_LogOutput(1, "Enter Country War still left = %d seconds", nsec);
		}
		pMsgBox->OnCommand("IDCANCEL");
	}
}

bool CDlgCountryMap::SaveLoadCenter(int idInst, const A3DVECTOR3 &vPos, int iParallelWorldID)
{
	bool bSaved(false);

	AString strMsg;
	while (true)
	{
		CECWorld *pWorld = GetWorld();
		if (!pWorld)
		{
			//	非法调用
			m_bLoadCenter = false;
			break;
		}

		if (!pWorld->IsCountryMap() || idInst != pWorld->GetInstanceID() || iParallelWorldID != pWorld->GetCurParallelWorld())
		{
			//	排除不相关场景、以及切换到不相关场景时处理
			if (m_bLoadCenter) strMsg = "CDlgCountryMap::SaveLoadCenter, cleared cause not in countrymap";
			m_bLoadCenter = false;
			break;
		}

		if (!IsShow())
		{
			//	Win_CountryMap 不显示时不做处理
			if (m_bLoadCenter) strMsg = "CDlgCountryMap::SaveLoadCenter, cleared cause ui not shown";
			m_bLoadCenter = false;
			break;
		}

		strMsg = "CDlgCountryMap::SaveLoadCenter, saved";
		m_bLoadCenter = true;
		m_vLoadCenter = vPos;
		bSaved = true;
		break;
	}
	if (!strMsg.IsEmpty()) a_LogOutput(1, strMsg);

	return bSaved;
}

bool CDlgCountryMap::CommitLoadCenter()
{
	bool bCommit(false);

	AString strMsg;
	while (m_bLoadCenter)
	{
		m_bLoadCenter = false;
		strMsg = "CDlgCountryMap::CommitLoadCenter, ";

		CECGameRun *pGameRun = GetGameRun();
		CECWorld *pWorld = GetWorld();
		if (!pWorld->IsCountryMap())
		{
			strMsg += "skipped cause not in countrymap";
			break;
		}
		if (!GetGameSession()->IsConnected())
		{
			strMsg += "skipped cause disconnected";
			break;
		}

		GetHostPlayer()->Goto(pWorld->GetInstanceID(), m_vLoadCenter, pWorld->GetCurParallelWorld());
		strMsg += "ok";
		bCommit = true;
		break;
	}

	if (!strMsg.IsEmpty()) a_LogOutput(1, strMsg);

	return bCommit;
}

bool CDlgCountryMap::CanBackToCapital()
{
	bool bOK(false);

	while (true)
	{
		if (GetHostPlayer()->GetCountry() <= 0 || !GetWorld()->IsCountryMap() || !IsStopped())
			break;

		if (IsMapReady() && IsLocationReady())
		{
			//	此功能比较重要，仅在信息可用情况下做检查
			const CECDomainCountryInfo* pInfo = CECDomainCountryInfo::GetInstance();
			const DOMAIN2_INFO *pNode = pInfo->Find(GetDomainLoc());
			if (!pNode || pNode->IsMyCapital(GetHostPlayer()->GetCountry()))
				break;
		}

		bOK = true;
		break;
	}
	return bOK;
}

bool CDlgCountryMap::CanGetScore()
{
	return (m_cntGetScore <= GetCurrentTime());
}

void CDlgCountryMap::OnCountryChange()
{	
	if (!IsStopped())
	{
		ASSERT(false);
	}

	//	国家改变时，清空与某国家相关的计时设置，以免国家改变带来的计时耦合
	m_cntGetMap = 0;
	m_cntStopMove = 0;
	m_cntGetScore = 0;
	
	m_domainLoc = 0;
	CECDomainCountryInfo::GetInstance()->ClearState();
	m_bGetMap = false;
	ClearMove();

	//	国战配置数据此处不清空，只根据初始化和协议改变
		
	m_nTotalScore = 0;
	memset(m_nCountryScore, 0, sizeof(m_nCountryScore));

	if (GetHostPlayer()->GetCountry() > 0)
	{
		GetConfig();
		GetGameSession()->country_GetPlayerLocation();
		GetMap();
		GetScore();
	}
}

void CDlgCountryMap::OnCountryEnterWar()
{
	if (IsStopped() || !GetHostPlayer()->IsInCountryWar())
		return;

	ClearMove();
	ShowFalse();
}

void CDlgCountryMap::MoveConfirm(bool bYes)
{
	if (!bYes ||
		!IsCanMove() ||
		!GetGameSession()->IsConnected())
	{
		m_routeCand.clear();
		return;
	}

	if (m_routeCand.empty())
	{
		ASSERT(false);
		return;
	}

	//	无法确定玩家点击时机，需处理任何状态

	if (!IsStopped() && !IsWaitingArrive())
	{
		m_routeCand.clear();
		return;
	}

	if (IsStopped())
	{
		m_route.swap(m_routeCand);
		m_routeCand.clear();
		StartMove();
		return;
	}

	if (IsWaitingArrive())
	{
		int pos = m_routeCand.find(m_domainLoc);
		if (pos < 0)
		{
			//	错过时机
			m_routeCand.clear();
			StopMove();
			return;
		}
		if (m_routeCand.getDst() == m_domainLoc)
		{
			//	正从新终点往别处移，取消移动
			m_routeCand.clear();
			StopMove();
			return;
		}

		//	以当前位置同步新路径
		m_routeCand.removeHead(pos);

		//	正移往位置与新路径要移往位置相同
		if (m_routeCand.getNext() == m_domainNext)
		{
			m_route.swap(m_routeCand);
			m_routeCand.clear();
			return;
		}

		//	取消当前移动、移往新路径下一节点
		m_route.swap(m_routeCand);
		m_routeCand.clear();
		GetGameSession()->country_Move(m_route.getNext());
		m_moveState = MS_WAITING_REDIRECT;
	}
}

void CDlgCountryMap::PopupKingOrderMenu(int x, int y, int iDomainID)
{
	CDlgKingCommand* pDlg = GetGameUIMan()->m_pDlgKingCommand;
	pDlg->SetDomainID(iDomainID);
	pDlg->Show(true);
	GetGameUIMan()->BringWindowToTop(pDlg);
	pDlg->SetPosEx(x, y);
}

///////////////////////////////////////////////////////////////////////////

//	CDlgCountryMove

AUI_BEGIN_COMMAND_MAP(CDlgCountryMove, CDlgBase)
AUI_ON_COMMAND("Btn_Stop", OnCommandStop)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgCountryMove::CDlgCountryMove()
: m_pTxt_RestTime(NULL)
, m_pBtn_Stop(NULL)
{
}

CDlgCountryMove::~CDlgCountryMove()
{
}

bool CDlgCountryMove::OnInitDialog()
{
	DDX_Control("Txt_RestTime", m_pTxt_RestTime);
	DDX_Control("Btn_Stop", m_pBtn_Stop);
	SetCanEscClose(false);
	return true;
}

CDlgCountryMap * CDlgCountryMove::GetDlgCountryMap()
{
	return dynamic_cast<CDlgCountryMap*>(GetGameUIMan()->GetDialog("Win_CountryMap"));
}

void CDlgCountryMove::OnTick()
{
	CDlgCountryMap *pDlgCountryMap = GetDlgCountryMap();

	m_pBtn_Stop->Enable(pDlgCountryMap->IsWaitingArrive());
	
	ACString strText;
	
	//	显示到达下一节点剩余时间
	const CECDomainCountryInfo* pInfo = CECDomainCountryInfo::GetInstance();
	const DOMAIN2_INFO *pNode = pInfo->Find(pDlgCountryMap->m_domainNext);
	if (!pNode)
	{
		ASSERT(false);
		m_pTxt_RestTime->SetText(_AL(""));
		return;
	}
	int nodeLeft = CDlgCountryMap::ConverToSeconds(pDlgCountryMap->GetNodeMoveTimeLeft());
	strText.Format(GetStringFromTable(9883), pNode->name, GetGameUIMan()->GetFormatTime(nodeLeft));

	//	显示到达最终目标剩余时间
	if (pDlgCountryMap->m_route.getDst() != pDlgCountryMap->m_domainNext)
	{
		const DOMAIN2_INFO *pTarget = pInfo->Find(pDlgCountryMap->m_route.getDst());
		if (!pTarget)
		{
			ASSERT(false);
			m_pTxt_RestTime->SetText(_AL(""));
			return;
		}
		int allLeft = CDlgCountryMap::ConverToSeconds(pDlgCountryMap->GetRouteMoveTimeLeft());
		ACString strTemp;
		strTemp.Format(GetStringFromTable(9883), pTarget->name, GetGameUIMan()->GetFormatTime(allLeft));
		strText += _AL("\r");
		strText += strTemp;
	}

	m_pTxt_RestTime->SetText(strText);
}

void CDlgCountryMove::OnCommandStop(const char *szCommand)
{
	CDlgCountryMap *pDlgCountryMap = GetDlgCountryMap();
	if (!pDlgCountryMap->IsWaitingArrive()) return;
	pDlgCountryMap->StopMove();
}

void CDlgCountryMove::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->RespawnMessage();
}
