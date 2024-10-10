// Filename	: DlgWebList.h
// Creator	: Xu Wenbin
// Date		: 2012/05/25

#include "DlgWebList.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_UIHelper.h"
#include "Network/IOLib/gnoctets.h"
#include "Network/ssogetticket_re.hpp"

#include <AUIManager.h>
#include <AIniFile.h>

extern CECGame * g_pGame;

//	class CDlgWebListSSOURLNavigator
CDlgWebListSSOURLNavigator::CDlgWebListSSOURLNavigator(AUIObject *pClickButton)
: m_pClickButton(pClickButton)
{
}

void CDlgWebListSSOURLNavigator::EnableClickButton(bool bEnable){
	if (m_pClickButton && m_pClickButton->IsEnabled() != bEnable)
		m_pClickButton->Enable(bEnable);
}

bool CDlgWebListSSOURLNavigator::IsClickButtonEnabled()const{
	return m_pClickButton != NULL && m_pClickButton->IsEnabled();
}

void CDlgWebListSSOURLNavigator::ClickNavigateButton(bool bOSNavigate){	
	if (!IsClickButtonEnabled()){
		return;
	}
	//	申请 ticket
	g_pGame->GetGameSession()->sso_GetTicket(GetSSOTicketContext(bOSNavigate), GetSSOTicketInfo());
	//	到 ticket 返回前，都不再恢复
	EnableClickButton(false);
}

bool CDlgWebListSSOURLNavigator::ShouldNavigateInOSForTicketContext(const GNET::Octets &context)const{
	return context == GetSSOTicketContext(true);
}

bool CDlgWebListSSOURLNavigator::IsMySSOTicketContext(const GNET::Octets &context)const{
	return context == GetSSOTicketContext(false)
		|| context == GetSSOTicketContext(true);
}

GNET::Octets CDlgWebListSSOURLNavigator::GetSSOTicketContext(bool bForOSNavigate)const{
	GNET::Octets result = GetSSOTicketBaseContext();
	if (bForOSNavigate){
		result = CECURLOSNavigator::AppendNavigateInOSSuffix(result);
	}
	return result;
}

AString CDlgWebListSSOURLNavigator::GetNavigateURL(const GNET::Octets &ticket)const{
	AString result;
	while (true){
		AString strFormat = GetNavigateURLFormat();
		if (strFormat.IsEmpty()){
			a_LogOutput(1, "CDlgWebListSSOURLNavigator::GetNavigateURL, format is empty.");
			break;
		}		
		AString strTicket((const char *)ticket.begin(), ticket.size());
		if (strTicket.IsEmpty()){
			a_LogOutput(1, "CDlgWebListSSOURLNavigator::GetNavigateURL, ticket is empty.");
			break;
		}
		AString strAccount = g_pGame->GetGameSession()->GetTicketAccount();
		result.Format(strFormat, strTicket, strAccount);
		break;
	}
	return result;
}

//	class CDlgWebListSSOPayNavigator
CDlgWebListSSOPayNavigator::CDlgWebListSSOPayNavigator(AUIObject *pClickButton)
: CDlgWebListSSOURLNavigator(pClickButton)
{
}

GNET::Octets CDlgWebListSSOPayNavigator::GetSSOTicketBaseContext()const{	
	const char *szContext = "ticket_pay";
	return GNET::Octets(szContext, strlen(szContext));
}

GNET::Octets CDlgWebListSSOPayNavigator::GetSSOTicketInfo()const{
	return GNET::Octets();
}

AString CDlgWebListSSOPayNavigator::GetNavigateURLFormat()const{
	return CECUIHelper::GetBaseUIMan()->GetURL("WEB", "PAY");
}

//	class CDlgWebListSSOActivityNavigator
CDlgWebListSSOActivityNavigator::CDlgWebListSSOActivityNavigator(AUIObject *pClickButton)
: CDlgWebListSSOURLNavigator(pClickButton)
{
}

GNET::Octets CDlgWebListSSOActivityNavigator::GetSSOTicketBaseContext()const{	
	const char *szContext = "ticket_activity";
	return GNET::Octets(szContext, strlen(szContext));
}

GNET::Octets CDlgWebListSSOActivityNavigator::GetSSOTicketInfo()const{	
	GNET::Marshal::OctetsStream result;
	result << CECGameSession::SSO_TI_ROLEINFO_1;
	return result;
}

AString CDlgWebListSSOActivityNavigator::GetNavigateURLFormat()const{
	return CECUIHelper::GetBaseUIMan()->GetURL("WEB", "ACTIVITY");
}

//	class CDlgWebListSSOActivity2Navigator
CDlgWebListSSOActivity2Navigator::CDlgWebListSSOActivity2Navigator(AUIObject *pClickButton)
: CDlgWebListSSOURLNavigator(pClickButton)
{
}

GNET::Octets CDlgWebListSSOActivity2Navigator::GetSSOTicketBaseContext()const{	
	const char *szContext = "ticket_activity2";
	return GNET::Octets(szContext, strlen(szContext));
}

GNET::Octets CDlgWebListSSOActivity2Navigator::GetSSOTicketInfo()const{	
	GNET::Marshal::OctetsStream result;
	result << CECGameSession::SSO_TI_ROLEINFO_1;
	return result;
}

AString CDlgWebListSSOActivity2Navigator::GetNavigateURLFormat()const{
	return CECUIHelper::GetBaseUIMan()->GetURL("WEB", "ACTIVITY2");
}

//	class CDlgWebList

AUI_BEGIN_COMMAND_MAP(CDlgWebList, CDlgBase)
AUI_ON_COMMAND("Btn_Radio", OnCommand_Btn_Radio)
AUI_ON_COMMAND("Btn_Pay", OnCommand_Btn_Pay)
AUI_ON_COMMAND("Btn_Activity", OnCommand_Btn_Activity)
AUI_ON_COMMAND("Btn_Activity2", OnCommand_Btn_Activity2)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWebList, CDlgBase)
AUI_END_EVENT_MAP()

CDlgWebList::CDlgWebList()
{
	for (int i(0); i < SSO_NAVIGATE_NUM; ++ i){
		m_pSSOURLNavigator[i] = NULL;
	}
	m_pLastSSOURLNavigator = NULL;
}

bool CDlgWebList::OnInitDialog()
{
	m_pSSOURLNavigator[SSO_NAVIGATE_PAY]		= new CDlgWebListSSOPayNavigator(GetDlgItem("Btn_Pay"));
	m_pSSOURLNavigator[SSO_NAVIGATE_ACTIVITY]	= new CDlgWebListSSOActivityNavigator(GetDlgItem("Btn_Activity"));
	m_pSSOURLNavigator[SSO_NAVIGATE_ACTIVITY2]	= new CDlgWebListSSOActivity2Navigator(GetDlgItem("Btn_Activity2"));

	DisableForCrossServer("Btn_Pay");
	DisableForCrossServer("Btn_Activity");
	DisableForCrossServer("Btn_Activity2");
	return CDlgBase::OnInitDialog();
}

bool CDlgWebList::Release(){
	m_pLastSSOURLNavigator = NULL;
	for (int i(0); i < SSO_NAVIGATE_NUM; ++ i){
		delete m_pSSOURLNavigator[i];
		m_pSSOURLNavigator[i] = NULL;
	}
	return CDlgBase::Release();
}

void CDlgWebList::OnCommand_Btn_Radio(const char *szCommand)
{	
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_Broadcast");
	if( pDlg)
		pDlg->OnCommand("confirm");
	Show(false);
}

void CDlgWebList::OnCommand_Btn_Pay(const char *szCommand)
{
	m_pSSOURLNavigator[SSO_NAVIGATE_PAY]->ClickNavigateButton(false);
	Show(false);
}

void CDlgWebList::OnCommand_Btn_Activity(const char *szCommand)
{
	m_pSSOURLNavigator[SSO_NAVIGATE_ACTIVITY]->ClickNavigateButton(false);
	Show(false);
}

void CDlgWebList::OnCommand_Btn_Activity2(const char *szCommand)
{
	m_pSSOURLNavigator[SSO_NAVIGATE_ACTIVITY2]->ClickNavigateButton(false);
	Show(false);
}

void CDlgWebList::HandleRequest(const CECSSOTicketHandler::Request *p)
{
	bool bProcessed(false);
	while (p){
		int i(0);
		for (i = 0; i < SSO_NAVIGATE_NUM; ++ i){
			if (m_pSSOURLNavigator[i]->IsMySSOTicketContext(p->local_context)){
				m_pSSOURLNavigator[i]->EnableClickButton(true);
				break;
			}
		}
		if (p->retcode != 0){
			break;	//	retcode 其它情况传递给 CECGameUIMan 统一处理
		}
		if (i >= SSO_NAVIGATE_NUM){
			break;
		}
		bProcessed = true;
		AString strURL = m_pSSOURLNavigator[i]->GetNavigateURL(p->ticket);
		if (strURL.IsEmpty()){
			break;
		}
		if (!m_pSSOURLNavigator[i]->CanNavigateInGame() ||
			m_pSSOURLNavigator[i]->ShouldNavigateInOSForTicketContext(p->local_context)){
			GetBaseUIMan()->NavigateURL(strURL, NULL);
		}else{
			m_pLastSSOURLNavigator = m_pSSOURLNavigator[i];
			GetBaseUIMan()->NavigateURL(strURL, "Win_Explorer", false, this);
		}
		break;
	}
	if (!bProcessed){
		CECSSOTicketHandler::HandleRequest(p);
	}
}

bool CDlgWebList::CanNavigateNow()const{
	ASSERT(m_pLastSSOURLNavigator != NULL);
	return m_pLastSSOURLNavigator ? m_pLastSSOURLNavigator->IsClickButtonEnabled() : false;
}

void CDlgWebList::OnClickNavigate(){
	if (!CanNavigateNow()){
		return;
	}
	m_pLastSSOURLNavigator->ClickNavigateButton(true);
}

bool CDlgWebList::AutoRelease()const{
	return false;
}