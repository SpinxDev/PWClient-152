// File		: DlgLogin.cpp
// Creator	: Feng Ning
// Date		: 2010/10/20

#include "DlgLogin.h"
#include "DlgLoginOther.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_Configs.h"
#include "EC_LoginUIMan.h"
#include "EC_BaseUIMan.h"
#include "EC_CommandLine.h"
#include "DlgLoginServerList.h"
#include "EC_UIConfigs.h"
#include "EC_LoginSwitch.h"
#include "EC_CrossServer.h"
#include "EC_ServerList.h"
#include "EC_MCDownload.h"
#include "EC_RecordAccount.h"
#include "DlgCurrentServer.h"
#include "EC_UIAnimation.h"
#include "Arc/Asia/EC_ArcAsia.h"
#include "EC_Reconnect.h"

#include "AFI.h"
#include "AIniFile.h"

#include <AUIListBox.h>
#include <AUICheckBox.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLogin, CDlgBase)
AUI_ON_COMMAND("Char_*", OnCommand_SoftKey)
AUI_ON_COMMAND("switch", OnCommand_Switch)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Cancel)
AUI_ON_COMMAND("confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Sina", OnCommand_Sina)
AUI_ON_COMMAND("Btn_QQ", OnCommand_QQ)
AUI_ON_COMMAND("Btn_360", OnCommand_360)
AUI_ON_COMMAND("Btn_Taobao", OnCommand_Taobao)
AUI_ON_COMMAND("Btn_Renren", OnCommand_Renren)
AUI_ON_COMMAND("Lst_Account", OnCommand_LstAccount)
AUI_ON_COMMAND("Chk_Record", OnCommand_ChkRecord)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLogin, CDlgBase)
AUI_ON_EVENT("DEFAULT_Txt_Account",	WM_KEYDOWN,	OnEventKeyDown_TxtAccount)
AUI_ON_EVENT("Lst_Account",			WM_KEYDOWN,	OnEventKeyDown_LstAccount)
AUI_ON_EVENT("Lst_Account",		WM_LBUTTONDOWN,	OnEventLButtonUp_LstAccount)
AUI_ON_EVENT("Lbl_Server",		WM_LBUTTONDOWN,	OnEventLButtonDown_ChangeServer)
AUI_ON_EVENT("Lbl_ServerName",	WM_LBUTTONDOWN,	OnEventLButtonDown_ChangeServer)
AUI_ON_EVENT("Lbl_Ping",		WM_LBUTTONDOWN,	OnEventLButtonDown_ChangeServer)
AUI_ON_EVENT("Img_Server",		WM_LBUTTONDOWN,	OnEventLButtonDown_ChangeServer)
AUI_END_EVENT_MAP()

CDlgLogin::CDlgLogin()
: m_pTxt_Account(NULL)
, m_pTxt_Password(NULL)
, m_pLst_Account(NULL)
, m_pChk_Record(NULL)
, m_pDlgCurrentServer(NULL)
, m_bAlwaysTryAutoLogin(false)
{
}

CDlgLogin::~CDlgLogin()
{
}

bool CDlgLogin::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
	{
		return false;
	}
	m_pDlgCurrentServer = new CDlgCurrentServer(this);
	DDX_Control("DEFAULT_Txt_Account", m_pTxt_Account);
	DDX_Control("Txt_PassWord", m_pTxt_Password);
	DDX_Control("Lst_Account", m_pLst_Account);
	DDX_Control("Chk_Record", m_pChk_Record);
	m_pChk_Record->Check(CECRecordAccount::Instance().GetCanRecord());
	ResetAccountList();

	// check the password protect button
	AIniFile theIni;
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\configs\\loginpage.ini", af_GetBaseDir());
	if( theIni.Open(szFile) )
	{
		if( theIni.GetValueAsInt("PWDPROTECT", "Open", 0) == 0 )
		{
			GetDlgItem("Btn_Link")->Show(false);
		}

		m_strSinaAddress = theIni.GetValueAsString("LoginOther", "URL_Sina", "");
		m_strQQAddress = theIni.GetValueAsString("LoginOther", "URL_QQ", "");
		m_str360Address = theIni.GetValueAsString("LoginOther", "URL_360", "");
		m_strTaobaoAddress = theIni.GetValueAsString("LoginOther", "URL_Taobao", "");
		m_strRenrenAddress = theIni.GetValueAsString("LoginOther", "URL_Renren", "");

		theIni.Close();
	}
	
	if (!CECUIConfig::Instance().GetLoginUI().bEnableOtherLogin)
	{
		PAUIOBJECT pObj = GetDlgItem("Btn_Sina");
		if (pObj) pObj->Show(false);
		
		pObj = GetDlgItem("Btn_QQ");
		if (pObj) pObj->Show(false);
		
		pObj = GetDlgItem("Btn_360");
		if (pObj) pObj->Show(false);

		pObj = GetDlgItem("Btn_Taobao");
		if (pObj) pObj->Show(false);

		pObj = GetDlgItem("Btn_Renren");
		if (pObj) pObj->Show(false);
	}
	
	if (CECUIConfig::Instance().GetLoginUI().bEnableForceLogin)
		SetKickoutUser(true);

	return true;
}

void CDlgLogin::OnShowDialog()
{
	CECLoginUIMan *pUIMan = GetLoginUIMan();

	PAUIDIALOG pDlgLoginButton = pUIMan->GetDialog("Win_LoginButton");
	if (!pDlgLoginButton->IsShow()){
		pDlgLoginButton->Show(true);
	}
	PAUIDIALOG pDlgLoginVersion = pUIMan->GetDialog("Win_LoginVersion");
	if (!pDlgLoginVersion->IsShow()){
		pDlgLoginVersion->Show(true);
	}
	pUIMan->BringWindowToTop(this);

	pUIMan->ChangeScene(CECLoginUIMan::LOGIN_SCENE_LOGIN);
	ResetUserPassword(false);	
	if (GetUser().IsEmpty()){
		FocusUser();
	}else{
		FocusPassword();
	}
	
	ResetAccountList();
	UpdateAccountList();

	// ��������ͻ����Ƿ������سɹ�
	if( g_pGame->GetConfigs()->IsMiniClient() )
		CECMCDownload::GetInstance().SendGetDownloadOK();

	CDlgBase::OnShowDialog();
}

void CDlgLogin::OnHideDialog()
{
	m_pAUIManager->GetDialog("Win_LoginButton")->Show(false);
	m_pAUIManager->GetDialog("Win_Softkb")->Show(false);
	CDlgBase::OnHideDialog();
}

bool CDlgLogin::Release()
{
	if (m_pDlgCurrentServer){
		delete m_pDlgCurrentServer;
		m_pDlgCurrentServer = NULL;
	}
	return CDlgBase::Release();
}

void CDlgLogin::ResetUserPassword(bool bClearUserPwd)
{
	//	�����˺ż���������

	bool bCanAutoLogin = CECLoginSwitch::Instance().CanAutoLogin();

	if (bCanAutoLogin || m_bAlwaysTryAutoLogin)
	{
		//	�Զ���½ʱ���˺ź��������þ�Ϊ******����ʾ������������˺ź�����
		SetUser(_AL("******"));
		SetPassword(_AL("******"));
		m_pTxt_Account->SetIsReadOnly(true);
		m_pTxt_Password->SetIsReadOnly(true);
	}
	else
	{
		//	���Զ���½ʱ�����������������˺�
		glb_RepairExeInMemory();
		if (bClearUserPwd){
			SetUser(_AL(""));
			SetPassword(_AL(""));
		}
		m_pTxt_Account->SetIsReadOnly(false);
		m_pTxt_Password->SetIsReadOnly(false);
		glb_RepairExeInMemory();
	}
}

void CDlgLogin::OnCommand_Confirm(const char* szCommand)
{
	//	ע�⣺CDlgLogin δ��ʾʱҲ�п��ܵ��ã������¼ʱ��

	//	�����¼���
	if (CECCrossServer::Instance().IsWaitLogin()){
		CECLoginSwitch::Instance().CrossServerLogin(CECCrossServer::Instance().GetUser());
	}else if (CECReconnect::Instance().IsReconnecting()){
		CECReconnect::Instance().UseLoginParameter();
	}
	
	//	����ʼ���Զ���¼ʱ���ص���¼���治��Ҫ�����˺�����ʼ����ֱ�ӵ�¼��Ϸ��Դ�ں�������
	if (!CECLoginSwitch::Instance().CanAutoLogin() &&
		m_bAlwaysTryAutoLogin){
		CECLoginSwitch::Instance().CheckAutoLogin();
	}
	
	//	Asia Arc ÿ�ε�¼ʱ��ѯ��¼��������Ϊ��¼������ʱ��
	if (CECLoginSwitch::Instance().IsUseArc()){
		CECLoginSwitch::Instance().ArcAsiaGetToken();
	}

	//	Ĭ�ϵ�¼��ʽ�¡�����û��������룬���Ϊ�գ����޸Ľ�����ʾ�û����벢����
	if (!CECLoginSwitch::Instance().CanAutoLogin()){
		if (CECLoginSwitch::Instance().GetUser().IsEmpty()){
			FocusUser();
			return;
		}
		if (CECLoginSwitch::Instance().GetPassword().IsEmpty()){
			FocusPassword();
			return;
		}
	}

	bool bServerOK(false);
	while (true)
	{
		if (CECLoginSwitch::Instance().IsUseCrossServer()){
			//	ת����¼�����⴦��
			if (!CECCrossServer::Instance().ApplyServerSetting()){
				break;
			}
		}else if (CECReconnect::Instance().IsReconnecting()){
			if (!CECReconnect::Instance().ApplyServerSetting()){
				break;
			}
		}else{
			//	��ͨ��¼���ӷ������б���ѡ��
			if (!CECServerList::Instance().ApplyServerSetting()){
				break;
			}
		}

		bServerOK = true;
		break;
	}
	if (!bServerOK)
	{
		//	��¼ʧ��ʱ��յ�¼��Ϣ���ָ�Ĭ�ϵ�¼��ʽ
		if (CECCrossServer::Instance().IsWaitLogin())
			CECCrossServer::Instance().OnLoginFail();
		CECLoginSwitch::Instance().DefaultLogin();
		GetLoginUIMan()->SwitchToLogin();
		GetLoginUIMan()->ShowErrorMsg(GetStringFromTable(202), "MsgBox_LoginFail");
		return;
	}
	
	//	ʹ�õ�ǰ���õ�½
	if (CECLoginSwitch::Instance().Login())
	{
		//	�����ͻ��˺Ϸ����ȴ�����
		if (IsShow())	Show(false);

		m_pAUIManager->GetDialog("Win_LoginWait")->Show(true);
	}
	else
	{
		//	�������Ϸ�����ʾ��½����
		if (CECCrossServer::Instance().IsWaitLogin())
			CECCrossServer::Instance().OnLoginFail();
		GetLoginUIMan()->SwitchToLogin();
		GetLoginUIMan()->ShowErrorMsg(GetStringFromTable(203), "MsgBox_LoginFail");
	}

	//	���ý���
	ResetUserPassword(true);
}

void CDlgLogin::OnCommand_Cancel(const char* szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_LoginWait");
	
	// quite from message windows
	if(pDlg->IsShow())
	{
		pDlg->Show(false);
		a_LogOutput(1, "CDlgLogin::OnCommand_Cancel");
		if (CECCrossServer::Instance().IsWaitLogin()){
			CECCrossServer::Instance().OnLoginFail();
		}
		if (CECReconnect::Instance().IsReconnecting()){
			CECReconnect::Instance().ResetState();
		}
		if (GetLoginUIMan()->GetCurScene() == CECLoginUIMan::LOGIN_SCENE_LOGIN ||	//	ѡ�˽��棨���������������
			GetLoginUIMan()->GetCurScene() == CECLoginUIMan::LOGIN_SCENE_NULL){		//	������������Ϸ����
			Show(true);
		}
		g_pGame->GetGameSession()->Close();
		GetLoginUIMan()->CancelAutoLogin();
	}
	else
	{
		PostQuitMessage(0);
	}
}

void CDlgLogin::OnCommand_SoftKey(const char* szCommand)
{
	ACString strText = m_pTxt_Password->GetText();

	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_Softkb");
	PAUIOBJECT pBtn = pDlg->GetDlgItem(szCommand);	
	strText += pBtn->GetText();
	m_pTxt_Password->SetText(strText);

	m_pAUIManager->BringWindowToTop(this);
	FocusPassword();
}

void CDlgLogin::OnCommand_Switch(const char* szCommand)
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_Softkb");
	PAUIOBJECT pBtn = pDlg->GetDlgItem("Switch");
	ACHAR szText[40];
	DWORD dwUpper = 1 - pBtn->GetData();
	char c, szName[40], cBase = dwUpper ? 'A' : 'a';
	
	for( c = 'a'; c <= 'z'; c++ )
	{
		sprintf(szName, "Char_%c", c);
		a_sprintf(szText, _AL("%c"), cBase + c - 'a');
		pDlg->GetDlgItem(szName)->SetText(szText);
	}
	pBtn->SetData(dwUpper);

	m_pAUIManager->BringWindowToTop(this);
	FocusPassword();
}

void CDlgLogin::OnCommand_Sina(const char* szCommand)
{
	WebLogin(m_strSinaAddress);
}

void CDlgLogin::OnCommand_QQ(const char* szCommand)
{
	WebLogin(m_strQQAddress);
}

void CDlgLogin::OnCommand_360(const char* szCommand)
{
	WebLogin(m_str360Address);
}

void CDlgLogin::OnCommand_Taobao(const char* szCommand)
{
	WebLogin(m_strTaobaoAddress);
}

void CDlgLogin::OnCommand_Renren(const char* szCommand)
{
	WebLogin(m_strRenrenAddress);
}

void CDlgLogin::WebLogin(const AString &strAddress)
{
	if (!CECUIConfig::Instance().GetLoginUI().bEnableOtherLogin ||
		strAddress.IsEmpty())
		return;
	
	CDlgLoginOther *pDlgLoginOther = dynamic_cast<CDlgLoginOther*>(GetLoginUIMan()->GetDialog("Win_LoginOther"));
	if (pDlgLoginOther)
	{		
		Show(false);
		pDlgLoginOther->Show(true);
		pDlgLoginOther->NavigateUrl(strAddress);
	}
}

bool CDlgLogin::GetKickoutUser()
{
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_Forcelog");
	return pCheck->IsChecked();
}

void CDlgLogin::SetKickoutUser(bool bKick)
{
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_Forcelog");
	pCheck->Check(bKick);
}

ACString CDlgLogin::GetUser()
{
	return m_pTxt_Account->GetText();
}

void CDlgLogin::SetUser(const ACString &user)
{
	m_pTxt_Account->SetText(user);
}

void CDlgLogin::FocusUser()
{
	ChangeFocus(m_pTxt_Account);
}

ACString CDlgLogin::GetPassword()
{
	return m_pTxt_Password->GetText();
}

void CDlgLogin::SetPassword(const ACString &pwd)
{
	m_pTxt_Password->SetText(pwd);
}

void CDlgLogin::FocusPassword()
{
	ChangeFocus(m_pTxt_Password);
}

void CDlgLogin::OnTick()
{
	// show the soft keyboard if necessary
	PAUIDIALOG pDlgS = m_pAUIManager->GetDialog("Win_Softkb");
	if( IsActive() && m_pTxt_Password->IsFocus() && !m_bAlwaysTryAutoLogin )
	{
		if( !pDlgS->IsShow() )
		{
			pDlgS->Show(true);
			m_pAUIManager->BringWindowToTop(this);
		}
	}
	else if( pDlgS->IsShow() && !pDlgS->IsActive() )
	{
		pDlgS->Show(false);
	}

	// bind the soft keyboard to this window
	if(pDlgS->IsShow())
	{
		PAUIOBJECT pBtnGame = GetDlgItem("Btn_Game");
		int x = pBtnGame->GetRect().CenterPoint().x - pDlgS->GetRect().Width()/2;
		int y = pBtnGame->GetRect().bottom;
		pDlgS->SetPosEx(x, y);
	}

	UpdateAccountList();
	m_pDlgCurrentServer->OnTick();
}

void CDlgLogin::OnCommand_LstAccount(const char *szCommand)
{
	int nSel = m_pLst_Account->GetCurSel();
	if (nSel >= 0 && nSel < m_pLst_Account->GetCount()){
		SetUser(m_pLst_Account->GetText(nSel));
		int iServer = (int)m_pLst_Account->GetItemData(nSel);
		if (iServer >= 0){
			CECServerList::Instance().SelectServer(iServer);
		}
		SetPassword(_AL(""));
		FocusPassword();	//	ת����������
	}
}

void CDlgLogin::OnCommand_ChkRecord(const char *szCommand)
{
	CECRecordAccount::Instance().SetCanRecord(m_pChk_Record->IsChecked());
	ResetAccountList();
}

void CDlgLogin::OnEventLButtonDown_ChangeServer(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	GetAUIManager()->GetDialog("Win_LoginButton")->OnCommand("changeserver");
}

void CDlgLogin::ResetAccountList()
{
	m_strAccountLastInput = _AL("");
	m_pLst_Account->ResetContent();
	m_pLst_Account->Show(false);
}

void CDlgLogin::UpdateAccountList()
{
	if (!IsActive() || !m_pTxt_Account->IsFocus() && !m_pLst_Account->IsFocus() || m_pTxt_Account->IsReadOnly()){
		//	��ǰ��������˺Ż��ѡ�б����˺Ų��ɸı�ʱ�����غ�ѡ�б�
		if (m_pLst_Account->IsShow()){
			//	ʧȥ���������£�����б��Ա��ȡ����ʱ���¼���
			ResetAccountList();
		}
		return;
	}
	ACString strCurInput = GetUser();
	if (strCurInput == m_strAccountLastInput){
		//	���������ޱ仯ʱ�����ı䵱ǰ״̬
		return;
	}
	m_strAccountLastInput = strCurInput;
	//	������ѡ�б�
	CECRecordAccount::Records candidates;
	if (!CECRecordAccount::Instance().Complete(strCurInput, candidates) ||	//	û�к�ѡ������
		candidates.size() == 1 && candidates[0] == strCurInput){			//	��ѡ�߸�Ŀ����ȫ�Ǻ�ʱ����ʱ����Ҫ����ʾ��
		if (m_pLst_Account->IsShow()){
			ResetAccountList();
		}
		return;
	}
	if (!m_pLst_Account->IsShow()){
		m_pLst_Account->Show(true);
	}
	//	���º�ѡ�б�
	m_pLst_Account->ResetContent();
	for (size_t u(0); u < candidates.size(); ++ u)
	{
		const CECRecordAccount::Record &r = candidates[u];
		int index = m_pLst_Account->AddString(r.account) -1;
		m_pLst_Account->SetItemData(index, (DWORD)r.server);
	}
	m_pLst_Account->SetCurSel(-1);
	//	�ָ�����򽹵�
	FocusUser();
}

void CDlgLogin::OnEventKeyDown_LstAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (wParam == VK_RETURN){
		//	�б����Ӵ��� VK_RETURN
		OnCommand_LstAccount("");
		return;
	}
	if (wParam == VK_TAB || wParam == VK_ESCAPE){
		//	VK_TAB ��Ϣ����Ϊ�ı佹��
		FocusUser();
		return;
	}
	//	�б���������AUI�⴦��
}

void CDlgLogin::OnEventKeyDown_TxtAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//	��������� VK_UP �� VK_DOWN ������
	if (wParam != VK_UP && wParam != VK_DOWN){
		return;
	}
	if (m_pTxt_Account->IsReadOnly() ||
		!m_pLst_Account->IsShow()){
		return;
	}
	int nCount = m_pLst_Account->GetCount();
	if (nCount <= 0){
		return;
	}
	int nSel = m_pLst_Account->GetCurSel();
	if (nSel < 0 || nSel >= nCount){
		nSel = 0;
	}else if (wParam == VK_DOWN){
		nSel = (nSel + 1)%nCount;
	}else{ // if (wParam == VK_UP)
		nSel = (nSel + nCount-1)%nCount;
	}
	m_pLst_Account->SetCurSel(nSel);
	ChangeFocus(m_pLst_Account);		//	�����޸�Ϊ��ѡ�б��Ա�ʹ�� VK_RETURN ѡ�ж���
}

void CDlgLogin::OnEventLButtonUp_LstAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//	������ѡ��
	OnCommand_LstAccount("");
}