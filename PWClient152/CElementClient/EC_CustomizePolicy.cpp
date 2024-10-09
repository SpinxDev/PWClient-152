// Filename	: EC_CustomizePolicy.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/21

#include "EC_CustomizePolicy.h"
#include "EC_UIHelper.h"
#include "EC_LogicHelper.h"
#include "EC_CustomizeMgr.h"
#include "EC_LoginUIMan.h"
#include "EC_Player.h"

//	class CECCustomizeCreateCharacterPolicy
CECCustomizeCreateCharacterPolicy &CECCustomizeCreateCharacterPolicy::Instance(){
	static CECCustomizeCreateCharacterPolicy s_dummy;
	return s_dummy;
}

bool CECCustomizeCreateCharacterPolicy::ShouldHideAllDialog()const{
	return false;
}

bool CECCustomizeCreateCharacterPolicy::ShouldShowOKCancel()const{
	return false;
}

void CECCustomizeCreateCharacterPolicy::OnOK(CECCustomizeMgr &manager){
	manager.ConfirmPlayerCustomizeData();		
	// 界面继续显示直到创建返回
}

void CECCustomizeCreateCharacterPolicy::OnCancel(CECCustomizeMgr &manager){		
	manager.GetPlayer()->RestoreCustomizeData();
	manager.HideRelatedDialogs();
	manager.Finish();
}

//	class CECCustomizeLoginCharacterPolicy
CECCustomizeLoginCharacterPolicy &CECCustomizeLoginCharacterPolicy::Instance(){
	static CECCustomizeLoginCharacterPolicy s_dummy;
	return s_dummy;
}
bool CECCustomizeLoginCharacterPolicy::ShouldHideAllDialog()const{
	return false;
}
bool CECCustomizeLoginCharacterPolicy::ShouldShowOKCancel()const{
	return true;
}
void CECCustomizeLoginCharacterPolicy::OnOK(CECCustomizeMgr &manager){		
	manager.HideRelatedDialogs();
	manager.ConfirmPlayerCustomizeData();
	
	// now we send this data to the server to store it on the net
	CECLoginUIMan *pLoginUIMan = CECUIHelper::GetLoginUIMan();
	int roleID = pLoginUIMan->GetVecRoleInfo()[pLoginUIMan->GetIDCurRole()].roleid;
	const CECPlayer::PLAYER_CUSTOMIZEDATA & finalData = manager.GetPlayer()->GetCustomizeData();
	CECLogicHelper::GetGameSession()->SetRoleCustomizeData(roleID, (void *)&finalData, sizeof(finalData));
	
	manager.Finish();
}
void CECCustomizeLoginCharacterPolicy::OnCancel(CECCustomizeMgr &manager){		
	manager.GetPlayer()->RestoreCustomizeData();
	manager.HideRelatedDialogs();
	manager.Finish();
	CECUIHelper::GetLoginUIMan()->SwitchToSelectChar();
}

//	class CECCustomizeHostPolicy
CECCustomizeHostPolicy &CECCustomizeHostPolicy::Instance(){
	static CECCustomizeHostPolicy s_dummy;
	return s_dummy;
}
bool CECCustomizeHostPolicy::ShouldHideAllDialog()const{
	return true;
}
bool CECCustomizeHostPolicy::ShouldShowOKCancel()const{
	return true;
}
void CECCustomizeHostPolicy::OnOK(CECCustomizeMgr &manager){		
	CECUIHelper::GetBaseUIMan()->ShowAllDialogs(false);
	manager.ConfirmPlayerCustomizeData();
	
	int roleID = CECLogicHelper::GetGameRun()->GetSelectedRoleInfo().roleid;
	const CECPlayer::PLAYER_CUSTOMIZEDATA & finalData = manager.GetPlayer()->GetCustomizeData();
	CECLogicHelper::GetGameSession()->SetRoleCustomizeData(roleID, (void *)&finalData, sizeof(finalData));
	
	manager.ShowAllOldDlg();
	manager.RestoreCamera();		
	manager.Finish();
}
void CECCustomizeHostPolicy::OnCancel(CECCustomizeMgr &manager){
	CECUIHelper::GetBaseUIMan()->ShowAllDialogs(false);
	
	manager.GetPlayer()->RestoreCustomizeData();
	CECLogicHelper::GetGameSession()->c2s_CmdCancelAction();		
	manager.ShowAllOldDlg();
	manager.RestoreCamera();
	manager.Finish();
}
