// Filename	: EC_InstanceReenter.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/18

#include "EC_InstanceReenter.h"
#include "EC_GPDataType.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Instance.h"

#include "EC_UIHelper.h"
#include "DlgMessageBox.h"
#include "EC_GameUIMan.h"

ELEMENTCLIENT_DEFINE_SINGLETON_NO_CTOR(CECInstanceReenter)

CECInstanceReenter::CECInstanceReenter()
: m_worldTag(0)
, m_deadline(0)
{
}

void CECInstanceReenter::OnNotify(const S2C::cmd_instance_reenter_notify &cmd){
	a_LogOutput(1, "INSTANCE_REENTER_NOTIFY(%d,%d)", cmd.world_tag, cmd.world_tag);
	if (IsNotifyShown()){
		ASSERT(false);
		a_LogOutput(1, "CECInstanceReenter::OnNotify, too many INSTANCE_REENTER_NOTIFY");
		return;
	}
	m_worldTag = cmd.world_tag;
	m_deadline = cmd.time_out;
}

void CECInstanceReenter::Tick(){
	if (!HasReenterNotify()){
		return;
	}
	if (IsNotifyShown()){
		if (HasTimeLeft()){
			UpdateShownNotify();
		}else{
			HideNotify();
			Clear();
		}
		return;
	}
	if (!IsGoodTimeToShowNotify()){
		return;
	}
	if (HasTimeLeft()){
		ShowNotify();
	}else{
		Clear();
		a_LogOutput(1, "CECInstanceReenter::Tick, notify cleared before shown because time arrived");
	}
}

void CECInstanceReenter::Reenter(bool agree){
	if (!HasReenterNotify()){
		ASSERT(false);
		return;
	}
	if (g_pGame->GetGameSession()->IsConnected()){
		g_pGame->GetGameSession()->c2s_CmdSendReenterInstance(agree);
	}
	Clear();
}

void CECInstanceReenter::Clear(){
	m_worldTag = 0;
	m_deadline = 0;
}

bool CECInstanceReenter::HasReenterNotify()const{
	return m_worldTag > 0 && m_deadline != 0;
}

int CECInstanceReenter::GetTimeLeft()const{
	int result(0);
	int currentTime = g_pGame->GetServerGMTTime();
	if (currentTime < m_deadline){
		result = m_deadline - currentTime;
	}
	return result;
}

bool CECInstanceReenter::HasTimeLeft()const{
	return GetTimeLeft() > 0;
}

bool CECInstanceReenter::IsGoodTimeToShowNotify()const{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost){
		return pHost->HostIsReady() && !pHost->IsDead() && !CECUIHelper::GetGameUIMan()->GetNewMessageBox()->IsShow();
	}
	return false;
}

ACString CECInstanceReenter::GetNotifyMessage()const{
	ACString result;

	ACString strInstance;
	CECInstance *pInstance = CECUIHelper::GetGameRun()->GetInstance(m_worldTag);
	if (pInstance){
		strInstance = pInstance->GetName();
	}else{
		strInstance.Format(_AL("Instance::%d"), m_worldTag);
	}

	CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
	result.Format(pGameUIMan->GetStringFromTable(11351), strInstance, pGameUIMan->GetFormatTime(GetTimeLeft()));

	return result;
}

const char * CECInstanceReenter::GetNotifyContext(){
	return "Instance_Reenter";
}

void CECInstanceReenter::ShowNotify(){
	if (!CECUIHelper::GetGameUIMan()->NewMessageBox(GetNotifyContext(), GetNotifyMessage(), MB_YESNO)){
		ASSERT(false);
		a_LogOutput(1, "CECInstanceReenter::ShowNotify, failed to show");
		Clear();
		return;
	}
}

void CECInstanceReenter::HideNotify(){
	CDlgMessageBox *pMsgBox = CECUIHelper::GetGameUIMan()->GetNewMessageBox();
	pMsgBox->SetContext("");
	pMsgBox->SetMessage(_AL(""));
	pMsgBox->Show(false);
}

void CECInstanceReenter::UpdateShownNotify(){
	CECUIHelper::GetGameUIMan()->GetNewMessageBox()->SetMessage(GetNotifyMessage());
}

bool CECInstanceReenter::IsNotifyShown()const{	
	CDlgMessageBox *pMsgBox = CECUIHelper::GetGameUIMan()->GetNewMessageBox();
	return pMsgBox->IsShow() && !strcmp(pMsgBox->GetContext(), GetNotifyContext());
}