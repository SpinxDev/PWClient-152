// File		: DlgSelect.cpp
// Creator	: Feng Ning
// Date		: 2010/10/25

#include "DlgSelect.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_LoginUIMan.h"
#include "EC_LoginPlayer.h"
#include "EC_BaseUIMan.h"
#include "DlgWebTradeInfo.h"
#include "EC_CrossServer.h"
#include "EC_Configs.h"
#include "EC_MCDownload.h"
#include "EC_UIConfigs.h"
#include "EC_Reconnect.h"
#include "EC_LoginQueue.h"

#include "gameclient.h"
#include "gnetdef.h"

#include <windowsx.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSelect, CDlgBase)
AUI_ON_COMMAND("char*", OnCommand_SelectChar)
AUI_ON_COMMAND("IDOK", OnCommand_Confirm)
AUI_ON_COMMAND("confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Game", OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Cancel)
AUI_ON_COMMAND("quit", OnCommand_Quit)
AUI_ON_COMMAND("Btn_Create", OnCommand_CreateAccount)
AUI_ON_COMMAND("passwd", OnCommand_Password)
AUI_ON_COMMAND("Btn_Modify", OnCommand_Modify)
AUI_ON_COMMAND("Btn_Restore", OnCommand_Restore)
AUI_ON_COMMAND("Btn_Delete", OnCommand_Delete)
AUI_ON_COMMAND("canceltrade", OnCommand_CancelTrade)
AUI_ON_COMMAND("Btn_PageUp", OnCommand_PagePrev)
AUI_ON_COMMAND("Btn_PageDown", OnCommand_PageNext)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSelect, CDlgBase)
AUI_ON_EVENT(NULL, WM_KEYDOWN, OnEventKeyDown)
AUI_ON_EVENT("*", WM_KEYDOWN, OnEventKeyDown)
AUI_ON_EVENT("Rdo_Char*", WM_LBUTTONDOWN, OnEventLButtonUp_RdoChar)
AUI_ON_EVENT("Rdo_Char*", WM_LBUTTONUP, OnEventLButtonUp_RdoChar)
AUI_END_EVENT_MAP()

CDlgSelect::CDlgSelect()
: m_nBaseCharIndex(0)
{
}

CDlgSelect::~CDlgSelect()
{
}

bool CDlgSelect::OnInitDialog()
{
	return CDlgBase::OnInitDialog();
}

void CDlgSelect::OnShowDialog()
{
	AUIManager *pUIMan = GetAUIManager();

	PAUIDIALOG pDlgManage = pUIMan->GetDialog("Win_Manage");
	pDlgManage->Show(true);
	EnableEnterGame(false);
	PAUIDIALOG pDlgLoginVersion = pUIMan->GetDialog("Win_LoginVersion");
	pDlgLoginVersion->Show(false);
	PAUIDIALOG pDlgManage2 = pUIMan->GetDialog("Win_Manage2");
	pDlgManage2->Show(true);
	pDlgManage2->AlignTo(pDlgManage, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_CENTER, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
	PAUIDIALOG pDlgLoginPage = pUIMan->GetDialog("Win_LoginPage");
	pDlgLoginPage->Show(true);

	RefreshCharacterList();

	// 检查完整客户端是否已下载成功
	if( g_pGame->GetConfigs()->IsMiniClient() )
		CECMCDownload::GetInstance().SendGetDownloadOK();
	CDlgBase::OnShowDialog();
}

void CDlgSelect::OnHideDialog()
{
	AUIManager *pUIMan = GetAUIManager();
	
	PAUIDIALOG pDlgManage = pUIMan->GetDialog("Win_Manage");
	pDlgManage->Show(false);
	EnableEnterGame(false);

	PAUIDIALOG pDlgManage2 = pUIMan->GetDialog("Win_Manage2");
	pDlgManage2->Show(false);
	
	PAUIDIALOG pDlgLoginPage = pUIMan->GetDialog("Win_LoginPage");
	pDlgLoginPage->Show(false);
	
	PAUIDIALOG pHintDlg = pUIMan->GetDialog("Win_PwdHint");
	if (pHintDlg && pHintDlg->IsShow()){
		pHintDlg->Show(false);
	}
}

void CDlgSelect::OnTick()
{
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();

	char szName[40];
	ACString strLevel;
	PAUIOBJECT pRadio;
	
	int i, nDay, nHour, nMin, nSec, nTime = time(NULL);

	for( i = 0; i < CECLOGINUIMAN_MAX_ROLES_SHOW; i++ )
	{
		sprintf(szName, "Rdo_Char%d", i + 1);
		pRadio = GetDlgItem(szName);
		if(!pRadio) break;

		int infoIndex = i + m_nBaseCharIndex;
		if( infoIndex >= (int)infos.size() ){
			continue;
		}

		PAUICHECKBOX pCheckBox = static_cast<PAUICHECKBOX>(pRadio);
		pCheckBox->Check(pUIMan->GetIDCurRole() == infoIndex);

		const GNET::RoleInfo& info = infos[infoIndex];
		if (CECCrossServer::Instance().IsWaitLogin())
		{
			pRadio->SetHint(GetStringFromTable(239));
		}
		else if ( info.status == _ROLE_STATUS_ON_CROSSSERVER)
		{
			pRadio->SetHint(GetStringFromTable(240));
		}
		else if( info.status == _ROLE_STATUS_NORMAL )
		{
			pRadio->SetHint(_AL(""));
		}
		else if ( info.status == _ROLE_STATUS_FROZEN)
		{
			pRadio->SetHint(GetStringFromTable(5555));
		}
		else
		{
			nSec = max(7 * 24 * 3600 - (nTime - info.delete_time), 0);
			//				nSec = max(60 - (nTime - info.delete_time), 0);
			if( nSec >= 60 * 60 * 24 )
			{
				nDay = nSec / (60 * 60 * 24);
				strLevel.Format(GetStringFromTable(213), nDay);
			}
			else
			{
				nMin = nSec / 60;
				nSec -= nMin * 60;
				nHour = nMin / 60;
				nMin -= nHour * 60;
				strLevel.Format(GetStringFromTable(214), nHour, nMin, nSec);
			}
			pRadio->SetHint(strLevel);
		}
	}

	if (IsCannotCreate())
		EnableCreate(false);

	//	Win_Manage2 界面处理
	PAUIDIALOG pDlgManage2 = pUIMan->GetDialog("Win_Manage2");
	PAUIOBJECT pDelete = pDlgManage2->GetDlgItem("Btn_Delete");
	PAUIOBJECT pModify = pDlgManage2->GetDlgItem("Btn_Modify");
	PAUIOBJECT pRestore = pDlgManage2->GetDlgItem("Btn_Restore");

	int idCurRole = pUIMan->GetIDCurRole();
	if (idCurRole < 0 || idCurRole >= (int)infos.size()){
		pDelete->Show(false);
		pRestore->Show(false);
		pModify->Enable(false);
	}else{
		const GNET::RoleInfo &info = infos[idCurRole];
		if (CECCrossServer::Instance().IsWaitLogin() ||
			info.status == _ROLE_STATUS_ON_CROSSSERVER){
			pDelete->Show(false);
			pRestore->Show(false);
			pModify->Enable(false);
		}else if( info.status == _ROLE_STATUS_NORMAL ){
			pDelete->Show(true);
			pDelete->Enable(!CECLoginQueue::Instance().InQueue());
			pRestore->Show(false);
			pModify->Show(true);
			pModify->Enable(pUIMan->CanModify(info.roleid) && !CECLoginQueue::Instance().InQueue());
		}else{
			pDelete->Show(false);
			pRestore->Show(true);
			pRestore->Enable(info.status != _ROLE_STATUS_FROZEN && !CECLoginQueue::Instance().InQueue());
			pModify->Enable(false);
		}
	}
	PAUIDIALOG pHintDlg = pUIMan->GetDialog("Win_PwdHint");
	if (pUIMan->GetRoleListReady()){
		if (pUIMan->GetUsbBind() == 0){
			pUIMan->SetUsbBind(-1);
			if (pHintDlg && !pHintDlg->IsShow())
				pHintDlg->Show(true, true);
		}
	}
	if (pHintDlg->IsShow()){
		m_pAUIManager->BringWindowToTop(pHintDlg);
	}

	CDlgBase::OnTick();
}

void CDlgSelect::RenderAccountTime()
{
	PAUIDIALOG pAccount = m_pAUIManager->GetDialog("Win_LoginAccountTime");
	
	bool bMonth = false;
	ACString szText;
	long stime = g_pGame->GetGameSession()->GetFreeGameEndTime();
	if( g_pGame->GetGameSession()->GetNewbieTime() == 0 )
	{
		pAccount->GetDlgItem("Txt_Month")->SetText(GetStringFromTable(219));
		bMonth = true;
	}
	else if( g_pGame->GetGameSession()->GetNewbieTime() < 
		g_pGame->GetGameSession()->GetAccCreatedTime() )
	{
		pAccount->GetDlgItem("Txt_Month")->SetText(GetStringFromTable(221));
		bMonth = true;
	}
	else if( g_pGame->GetGameSession()->GetFreeGameTime() > 0 )
	{
		stime -= g_pGame->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
		tm *gtime = gmtime(&stime);
		szText.Format(GetStringFromTable(218), gtime->tm_year + 1900, gtime->tm_mon + 1, 
			gtime->tm_mday, gtime->tm_hour, gtime->tm_min);
		if( g_pGame->GetGameSession()->GetFreeGameTime() > 10 * 3600 * 1000 )
			pAccount->GetDlgItem("Txt_Month")->SetText(szText);
		else
			pAccount->GetDlgItem("Txt_Month")->SetText(_AL("^FF0000") + szText);
		bMonth = true;
	}
	else
		pAccount->GetDlgItem("Txt_Month")->SetText(_AL(""));
	
	
	DWORD nSec = DWORD(g_pGame->GetGameSession()->GetAccountTime() / 1000);
	nSec = max(nSec, 0);
	DWORD nMin = nSec / 60;
	nSec -= nMin * 60;
	DWORD nHour = nMin / 60;
	nMin -= nHour * 60;
	//		DWORD nDay = nHour / 24;
	//		nHour -= nDay * 24;
	szText.Format(GetStringFromTable(215), nHour, nMin);
	if( !bMonth )
		pAccount->GetDlgItem("Txt_Point")->SetText(szText + GetStringFromTable(220));
	else if( nHour < 10 )
		pAccount->GetDlgItem("Txt_Point")->SetText(_AL("^FF0000") + szText);
	else
		pAccount->GetDlgItem("Txt_Point")->SetText(_AL("^007F00") + szText);
	
	//		GetDlgItem("Txt_Timeleft")->SetText(szText);
}

bool CDlgSelect::Render()
{
	RenderAccountTime();
	return CDlgBase::Render();
}

void CDlgSelect::DeleteCharacter(int roleIndex)
{
	if (!IsRoleShown(roleIndex)){
		return;
	}
	EnableEnterGame(false);
}

void CDlgSelect::RestoreCharacter(int roleIndex)
{
	if (!IsRoleShown(roleIndex)){
		return;
	}

	CECLoginUIMan* pUIMan = GetLoginUIMan();
	EnableEnterGame(true);
}

void CDlgSelect::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();
	if (0 == infos.size())
	{
		return;
	}
	if (AUI_PRESS('C')){
		return;
	}
	int iCurID = pUIMan->GetIDCurRole();
	if(wParam == VK_DOWN || wParam == VK_UP)
	{
		iCurID += (wParam == VK_DOWN) ? 1 : -1;

		if(infos.size() == (unsigned int)iCurID)
		{
			iCurID = 0;
		}
		else if (iCurID < 0)
		{
			iCurID = infos.size() - 1;
		}
		SelectChar(iCurID);
	}else if (wParam == VK_LEFT || wParam == VK_PRIOR){
		OnCommand_PagePrev(NULL);
	}else if (wParam == VK_RIGHT || wParam == VK_NEXT){
		OnCommand_PageNext(NULL);
	}else if (wParam == VK_HOME && HavePrevPage()){
		SelectChar(0);
	}else if (wParam == VK_END && HaveNextPage()){
		int baseCharIndexLastPage = ((int)infos.size()-1) / CECLOGINUIMAN_MAX_ROLES_SHOW * CECLOGINUIMAN_MAX_ROLES_SHOW;
		SelectChar(baseCharIndexLastPage);
	}
}

void CDlgSelect::OnEventLButtonUp_RdoChar(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	交互导致控件显示顺序改变，影响显示效果
	ChangeFocus(NULL);
}

bool CDlgSelect::OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	if( (uMsg == WM_LBUTTONDBLCLK || (uMsg == WM_KEYDOWN && wParam == VK_RETURN))
		&& pObj && strstr(pObj->GetName(), "Rdo_Char") )
	{
		OnCommand_Confirm(NULL);
		return true;
	}

	return CDlgBase::OnEventMap(uMsg, wParam, lParam, pObj);
}

void CDlgSelect::OnCommand_Confirm(const char* szCommand)
{
	LoginSelectedChar();
}

bool CDlgSelect::LoginSelectedChar(){
	//	交互导致控件显示顺序改变，影响显示效果
	ChangeFocus(NULL);
	
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	if (pUIMan->IsSelectRoleSent())
	{
		//	已发送 SelectRole，等待回复
		return false;
	}
	
	int iCurID = pUIMan->GetIDCurRole();
	if( iCurID < 0 || !CanEnterGame() ){
		return false;
	}
	if( !CECCrossServer::Instance().IsWaitLogin() &&		//	需要尽快完成跨服登录
		g_pGame->GetGameSession()->GetNetManager()->IsUseUsbKey() && 
		g_pGame->GetGameSession()->GetNetManager()->IsUsbKeyExisting() )
	{
		pUIMan->MessageBox("", GetStringFromTable(228), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return false;
	}
	if (CECCrossServer::Instance().IsWaitLogin()){
		CECCrossServer::Instance().SelectRole();
	}else{
		const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();
		g_pGame->GetGameRun()->SetSelectedRoleInfo(infos[iCurID]);
		g_pGame->GetGameRun()->SetRedirectLoginPosRoleInfo(RoleInfo());
		g_pGame->GetGameSession()->SelectRole(infos[iCurID].roleid);
		
		// calc how many role had been traded
		int tradeCount = 0;
		for(size_t idRole=0;idRole<infos.size();idRole++){
			if(infos[idRole].status == _ROLE_STATUS_FROZEN){
				tradeCount++;
			}
		}
		CDlgWebTradeInfo::SetRoleCount(tradeCount);		
	}
	pUIMan->SelectRoleSent(true);
	EnableEnterGame(false);
	return true;
}

void CDlgSelect::OnCommand_Cancel(const char* szCommand)
{
	if (CECLoginQueue::Instance().InQueue()){
		return;
	}
	if (CECUIConfig::Instance().GetLoginUI().bAvoidLoginUI){
		return;
	}
	GetLoginUIMan()->SwitchToLogin();
	
	if (CECCrossServer::Instance().IsWaitLogin())
		CECCrossServer::Instance().OnLoginFail();
}

void CDlgSelect::OnCommand_Quit(const char* szCommand)
{
	PostQuitMessage(0);
}

void CDlgSelect::Switch(bool bShow)
{
	CDlgWebTradeInfo* pDlgInfo = (CDlgWebTradeInfo*)m_pAUIManager->GetDialog("Win_TradeInfo");
	pDlgInfo->Show(false);

	Show(bShow);
	m_pAUIManager->GetDialog("Win_LoginAccountTime")->Show(false); // here force hide this
	((CECLoginUIMan*)m_pAUIManager)->ChangeScene(CECLoginUIMan::LOGIN_SCENE_SELCHAR);
}

void CDlgSelect::OnCommand_Password(const char* szCommand)
{
	ACString strText = GetStringFromTable(232);
	AString strURL = AC2AS(strText);
	GetBaseUIMan()->NavigateURL(strURL);
}

void CDlgSelect::OnCommand_CreateAccount(const char* szCommand)
{
	if (IsCannotCreate()){
		return;
	}
	Switch(false);
	
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	pUIMan->SwitchToCreate(NULL);
}

bool CDlgSelect::IsRoleShown(int roleIndex)
{
	int index = roleIndex - m_nBaseCharIndex;
	return index >= 0 && index < CECLOGINUIMAN_MAX_ROLES_SHOW;
}

void CDlgSelect::SelectChar(int roleIndex)
{
	ChangeFocus(NULL);	//	交互导致控件显示顺序改变，影响显示效果

	//	选中 roleIndex 并显示 roleIndex 所在页
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	if (roleIndex < 0 || roleIndex >= (int)pUIMan->GetVecRoleInfo().size() || roleIndex == pUIMan->GetIDCurRole() ||
		CECLoginQueue::Instance().InQueue()){
		return;
	}
	
	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();
	pUIMan->SetIDCurRole(roleIndex);
	pUIMan->RefreshPlayerList();
	
	m_nBaseCharIndex = (roleIndex/CECLOGINUIMAN_MAX_ROLES_SHOW)*CECLOGINUIMAN_MAX_ROLES_SHOW;
	RefreshCharacterList();
	
	CDlgWebTradeInfo* pDlgInfo = (CDlgWebTradeInfo*)m_pAUIManager->GetDialog("Win_TradeInfo");
	pDlgInfo->Show(false);
	
	const GNET::RoleInfo &info = infos[roleIndex];
	if (CECCrossServer::Instance().IsWaitLogin() ||
		info.status == _ROLE_STATUS_ON_CROSSSERVER)
	{
		EnableEnterGame(true);
	}
	else if( info.status == _ROLE_STATUS_NORMAL )
	{
		EnableEnterGame(true);
	}
	else
	{
		EnableEnterGame(false);
		if(info.status == _ROLE_STATUS_FROZEN)
		{
			pDlgInfo->FetchRoleTradeInfo(info.roleid);
		}
	}
}

void CDlgSelect::OnCommand_Modify(const char* szCommand)
{
	Switch(false);	
	GetLoginUIMan()->SwitchToCustomize(false);
}

void CDlgSelect::OnCommand_Restore(const char* szCommand)
{
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();

	const GNET::RoleInfo& info = infos[pUIMan->GetIDCurRole()];
	g_pGame->GetGameSession()->RestoreRole(info.roleid);
}

void CDlgSelect::OnCommand_Delete(const char* szCommand)
{
	m_pAUIManager->MessageBox("SelChar_DelChar", 
		GetStringFromTable(206), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgSelect::OnCommand_SelectChar(const char* szCommand)
{
	int index = atoi(szCommand + strlen("char"));
	SelectChar(m_nBaseCharIndex + index -1);
}

void CDlgSelect::RefreshCharacterList()
{
	CECLoginUIMan* pUIMan = GetLoginUIMan();
	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();
	int roleCount = (int)infos.size();

	if (infos.empty()){
		m_nBaseCharIndex = 0;
	}else{
		while (m_nBaseCharIndex >= roleCount){
			m_nBaseCharIndex -= CECLOGINUIMAN_MAX_ROLES_SHOW;
		}
	}
	
	PAUIOBJECT pObjPageUp = GetDlgItem("Btn_PageUp");
	PAUIOBJECT pObjPageDown = GetDlgItem("Btn_PageDown");
	if (!HavePrevPage() && !HaveNextPage()){
		pObjPageUp->Show(false);
		pObjPageDown->Show(false);
	}else{
		pObjPageUp->Show(true);
		pObjPageDown->Show(true);
		pObjPageUp->Enable(HavePrevPage());
		pObjPageDown->Enable(HaveNextPage());
	}
	
	char szName[40];
	PAUIOBJECT pRadio, pName, pLevel;
	PAUIOBJECT pImageOnCrossServer, pImageDelete, pImageFrozen;
	for(int i = 0; true ; i++ )
	{
		sprintf(szName, "Rdo_Char%d", i + 1);
		pRadio = GetDlgItem(szName);
		if(!pRadio) break;
		
		sprintf(szName, "Txt_Name%d", i + 1);
		pName = GetDlgItem(szName);
		if(!pName) break;
		
		sprintf(szName, "Txt_Level%d", i + 1);
		pLevel = GetDlgItem(szName);
		if(!pLevel) break;
		
		sprintf(szName, "Img_Prof%d", i + 1);
		PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szName));
		
		sprintf(szName, "Img_%d1", i + 1);
		pImageOnCrossServer = GetDlgItem(szName);
		sprintf(szName, "Img_%d2", i + 1);
		pImageDelete = GetDlgItem(szName);
		sprintf(szName, "Img_%d3", i + 1);
		pImageFrozen = GetDlgItem(szName);
				
		int infoIndex = i + m_nBaseCharIndex;
		if( infoIndex < roleCount && i < CECLOGINUIMAN_MAX_ROLES_SHOW)
		{
			const GNET::RoleInfo& info = infos[infoIndex];
			
			pRadio->Show(true);
			
			ACHAR szText[80];
			ACString strName((const ACHAR *)info.name.begin(),
				info.name.size() / sizeof(ACHAR));
			AUI_ConvertChatString(strName, szText);
			pName->SetText(szText);
			pName->Show(true);
			
			pImageOnCrossServer->Show(info.status == _ROLE_STATUS_ON_CROSSSERVER);
			pImageDelete->Show(info.status == _ROLE_STATUS_READYDEL);
			pImageFrozen->Show(info.status == _ROLE_STATUS_FROZEN);
			
			int realmLevel(0);
			if (info.realm_data.size() > 0){
				try{
					Marshal::OctetsStream(info.realm_data) >> realmLevel;
				}catch (Marshal::Exception){
				}
			}
			ACString strLevel;
			if (realmLevel > 0){
				ACString strRealm = GetBaseUIMan()->GetRealmName(realmLevel);
				strLevel.Format(_AL("%s %s%s %d"), g_pGame->GetGameRun()->GetProfName(info.occupation), strRealm, GetStringFromTable(323), info.level);
			}else{
				strLevel.Format(_AL("%s %s %d"),
					g_pGame->GetGameRun()->GetProfName(info.occupation),
					GetStringFromTable(323), info.level);
			}
			pLevel->SetText(strLevel);
			pLevel->Show(true);
			pImg->Show(true);
			pImg->FixFrame(info.occupation);
		}
		else
		{
			pRadio->Show(false);
			pName->Show(false);
			pLevel->Show(false);
			pImg->Show(false);
			pImageOnCrossServer->Show(false);
			pImageDelete->Show(false);
			pImageFrozen->Show(false);
		}
	}
}

void CDlgSelect::OnChangeLayoutEnd(bool bAllDone)
{
	RefreshCharacterList();
}

bool CDlgSelect::OnChangeLayout(PAUIOBJECT lhs, PAUIOBJECT rhs)
{
	if(lhs && rhs)
	{
		if( 0 == strncmp(lhs->GetName(), "Txt_Name", 8) ||
			0 == strncmp(lhs->GetName(), "Txt_Level", 9)  )
		{
			AUIOBJECT_SETPROPERTY lhsProp;
			AUIOBJECT_SETPROPERTY rhsProp;

			// copy the target color
			lhs->SetData(rhs->GetColor());

			AUI_SWAP_RES("Text Color");
			AUI_SWAP_RES("Outline");
			AUI_SWAP_RES("Outline Color");
		}
	}

	return CDlgBase::OnChangeLayout(lhs, rhs);
}

void CDlgSelect::OnCommand_CancelTrade(const char* szCommand)
{
	m_pAUIManager->MessageBox("SelChar_CancelPrepost", 
			GetStringFromTable(5556), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgSelect::EnableCreate(bool bEnable)
{
	PAUIOBJECT pObj = GetAUIManager()->GetDialog("Win_Manage")->GetDlgItem("Btn_Create");
	if (pObj){
		pObj->Enable(bEnable && !IsCannotCreate());
	}
}

bool CDlgSelect::IsCannotCreate()
{
	//	禁用角色创建的必要条件，满足即禁用
	return !g_pGame->GetGameSession()->IsConnected()
		|| (GetLoginUIMan()->GetVecRoleInfo().size() >= CECLOGINUIMAN_MAX_ROLES_CREATE)
		|| CECCrossServer::Instance().IsWaitLogin()		//	跨服过程中角色列表为仅有跨服角色、不允许创建角色
		|| CECReconnect::Instance().IsReconnecting()	//	重连中也不能创建角色
		|| CECLoginQueue::Instance().InQueue();			//	登录排队中也不能
}

bool CDlgSelect::CanEnterGame(){
	return GetAUIManager()->GetDialog("Win_Manage")->GetDlgItem("Btn_Game")->IsEnabled();
}

void CDlgSelect::EnableEnterGame(bool bEnable){
	GetAUIManager()->GetDialog("Win_Manage")->GetDlgItem("Btn_Game")->Enable(bEnable);
}

bool CDlgSelect::HavePrevPage()
{
	return m_nBaseCharIndex > 0;
}

void CDlgSelect::OnCommand_PagePrev(const char* szCommand)
{
	if (!HavePrevPage()){
		return;
	}
	SelectChar(m_nBaseCharIndex - CECLOGINUIMAN_MAX_ROLES_SHOW);
}

bool CDlgSelect::HaveNextPage()
{
	const abase::vector<GNET::RoleInfo>& infos = GetLoginUIMan()->GetVecRoleInfo();
	return m_nBaseCharIndex + CECLOGINUIMAN_MAX_ROLES_SHOW < (int)infos.size();
}

void CDlgSelect::OnCommand_PageNext(const char* szCommand)
{
	if (!HaveNextPage()){
		return;
	}
	SelectChar(m_nBaseCharIndex+CECLOGINUIMAN_MAX_ROLES_SHOW);
}