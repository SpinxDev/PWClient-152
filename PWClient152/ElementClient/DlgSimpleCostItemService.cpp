// File		: DlgSimpleCostItemService.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/16

#include "DlgSimpleCostItemService.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_LogicHelper.h"
#include "EC_ProfConfigs.h"
#include "EC_UIHelper.h"
#include "EC_IvtrEquip.h"
#include "EC_LoginPlayer.h"
#include "EC_TaskInterface.h"
#include "TaskTempl.h"
#include "TaskTemplMan.h"
#include "auto_delete.h"
#include "EC_ProfConfigs.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_ManNPC.h"
#include "EC_CustomizeMgr.h"
#include "EC_ManPlayer.h"

#include "roleinfo"

#include "DlgMatchProfile.h"

#include <AUIImagePicture.h>

//	class CDlgSimpleCostItemServicePolicy
CDlgSimpleCostItemServicePolicy::CDlgSimpleCostItemServicePolicy()
: m_pProtocolParameterReadyPolicy(NULL)
{
}

void CDlgSimpleCostItemServicePolicy::AddCandiateID(int id){
	m_candidateIDs.push_back(id);
}

bool CDlgSimpleCostItemServicePolicy::GetCostItemID(int &id, int &iSlot)const{
	id = 0;
	iSlot = -1;
	if (m_candidateIDs.empty()){
		return false;
	}
	CECHostPlayer *pHost = CECLogicHelper::GetHostPlayer();
	CECInventory *pPack = pHost->GetPack();
	for (CandidateIDs::const_iterator cit = m_candidateIDs.begin(); cit != m_candidateIDs.end(); ++ cit){
		int idCurrent = *cit;
		int index = pPack->FindItem(idCurrent);
		if (index >= 0){
			id = idCurrent;
			iSlot = index;
			return true;
		}
	}
	id = *m_candidateIDs.begin();
	return false;
}

void CDlgSimpleCostItemServicePolicy::PrepareProtocolParameterImpl(){
	OnProtocolParameterReady();
}

void CDlgSimpleCostItemServicePolicy::OnProtocolParameterReady(){
	if (m_pProtocolParameterReadyPolicy){
		m_pProtocolParameterReadyPolicy->OnProtocolParameterReady();
	}
}

void CDlgSimpleCostItemServicePolicy::OnProtocolParameterPrepareFail(){
	if (m_pProtocolParameterReadyPolicy){
		m_pProtocolParameterReadyPolicy->OnProtocolParameterPrepareFail();
	}
}

void CDlgSimpleCostItemServicePolicy::OnProtocolParameterPrepareCancel(){
	if (m_pProtocolParameterReadyPolicy){
		m_pProtocolParameterReadyPolicy->OnProtocolParameterPrepareCancel();
	}
}

void CDlgSimpleCostItemServicePolicy::PrepareProtocolParameter(ProtocolParameterReadyPolicy *pReadyNotify){
	m_pProtocolParameterReadyPolicy = pReadyNotify;
	PrepareProtocolParameterImpl();
}

//	class CDlgSimpleCostItemService
AUI_BEGIN_COMMAND_MAP(CDlgSimpleCostItemService, CDlgBase)
AUI_ON_COMMAND("Btn_Confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgSimpleCostItemService::CDlgSimpleCostItemService()
: m_index(-1)
, m_id(0)
, m_pPolicy(NULL)
, m_pLbl_Title(NULL)
, m_pBtn_Confirm(NULL)
{
}

bool CDlgSimpleCostItemService::OnInitDialog()
{
	DDX_Control("Lbl_Title", m_pLbl_Title);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	return CDlgBase::OnInitDialog();
}

bool CDlgSimpleCostItemService::Release(){
	SetPolicy(NULL);
	return CDlgBase::Release();
}

void CDlgSimpleCostItemService::SetPolicy(CDlgSimpleCostItemServicePolicy *pNewPolicy){
	if (m_pPolicy == pNewPolicy){
		return;
	}
	delete m_pPolicy;
	m_pPolicy = pNewPolicy;
}

void CDlgSimpleCostItemService::OnShowDialog(){
	CDlgBase::OnShowDialog();

	m_pLbl_Title->SetText(GetStringFromTable(m_pPolicy->GetTitleStringID()));
	m_pPolicy->GetCostItemID(m_id, m_index);
		
	m_pBtn_Confirm->Enable(m_index >= 0);

	CECIvtrItem *pTempItem = CECIvtrItem::CreateItem(m_id, 0, 1);
	auto_delete<CECIvtrItem> _dummy(pTempItem);

	PAUIIMAGEPICTURE pImgPic = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Pic"));
	GetGameUIMan()->SetCover(pImgPic, pTempItem->GetIconFile());

	pImgPic->SetHint(pTempItem->GetName());

	if (CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_index)){
		pItem->Freeze(true);
		pImgPic->SetColor(A3DCOLORRGB(255, 255, 255));
	}else{
		pImgPic->SetColor(A3DCOLORRGB(128, 128, 128));
	}
}

void CDlgSimpleCostItemService::OnMessageBox(int value){
	if (IDYES == value && m_pPolicy){
		m_pPolicy->SendProtocol(m_id, m_index);	
	}
	OnCommandCancel("");
}

const char * CDlgSimpleCostItemService::GetMessageBoxName()const{
	return m_pPolicy ? m_pPolicy->GetMessageBoxName() : "";
}

void CDlgSimpleCostItemService::OnCommandConfirm(const char *szCommand)
{
	ACString strMessage;
	if (!m_pPolicy->CheckCondition(strMessage)){
		GetGameUIMan()->ShowErrorMsg(strMessage);
		OnCommandCancel("");
		return;
	}
	m_pPolicy->PrepareProtocolParameter(this);
}

void CDlgSimpleCostItemService::OnProtocolParameterReady(){
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	pGameUIMan->MessageBox(m_pPolicy->GetMessageBoxName(),
		GetStringFromTable(m_pPolicy->GetConfirmStringID()),
		MB_YESNO,
		pGameUIMan->GetMsgBoxColor());
}

void CDlgSimpleCostItemService::OnProtocolParameterPrepareFail(){
	OnCommandCancel("");
}

void CDlgSimpleCostItemService::OnProtocolParameterPrepareCancel(){
	OnCommandCancel("");
}

void CDlgSimpleCostItemService::OnCommandCancel(const char * szCommand)
{
	SetPolicy(NULL);
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetHostPlayer()->GetPack()->UnfreezeAllItems();
}

//	class CDlgChangeGenderPolicy
CDlgChangeGenderPolicy::CDlgChangeGenderPolicy()
: m_pPlayerAfterChangeGender(NULL)
{
	AddCandiateID(47090);
	AddCandiateID(47089);
}

CDlgChangeGenderPolicy::~CDlgChangeGenderPolicy(){
	if (m_pPlayerAfterChangeGender){
		ShowPlayer(false);
		A3DRELEASE(m_pPlayerAfterChangeGender);
	}
}

int CDlgChangeGenderPolicy::GetTitleStringID()const{
	return 11438;
}

bool CDlgChangeGenderPolicy::CheckCondition(ACString &strErrorMessage)const{
	return CanChangeGender(strErrorMessage);
}

int CDlgChangeGenderPolicy::GetConfirmStringID()const{
	return 11439;
}

const char *CDlgChangeGenderPolicy::GetMessageBoxName()const{
	return "PlayerChangeGender";
}

ACString CDlgChangeGenderPolicy::ColorString(const ACString &strMessage)const{
	ACString result;
	if (!strMessage.IsEmpty()){
		result.Format(CECUIHelper::GetGameUIMan()->GetStringFromTable(11442), strMessage);
	}
	return result;
}

bool CDlgChangeGenderPolicy::CanChangeGender(ACString &strMessage)const{
	bool result(true);

	ACString strTemp;
	ACString strNextLine = _AL("\r");

	ACString strReasonMessage;
	if (CheckHasSpouse(strTemp)){
		if (!strReasonMessage.IsEmpty()){
			strReasonMessage += strNextLine;
		}
		strReasonMessage += strTemp;
		result = false;
	}
	if (CheckIsEquippingAnyFashion(strTemp)){
		if (!strReasonMessage.IsEmpty()){
			strReasonMessage += strNextLine;
		}
		strReasonMessage += strTemp;
		result = false;
	}
	if (CheckSetPlayerProfileMatch(strTemp)){
		if (!strReasonMessage.IsEmpty()){
			strReasonMessage += strNextLine;
		}
		strReasonMessage += strTemp;
		result = false;
	}
	if (CheckHasGenderRelatedTask(strTemp)){
		if (!strReasonMessage.IsEmpty()){
			strReasonMessage += strNextLine;
		}
		strReasonMessage += strTemp;
		result = false;
	}

	if (!result){
		strMessage = CECUIHelper::GetGameUIMan()->GetStringFromTable(11440);
		strMessage += strNextLine;
		strMessage += strReasonMessage;
	}

	return result;
}

bool CDlgChangeGenderPolicy::CheckHasSpouse(ACString &strMessage)const{
	bool result(false);
	strMessage.Empty();
	if (CECLogicHelper::GetHostPlayer()->GetSpouse()){
		strMessage = CECUIHelper::GetGameUIMan()->GetStringFromTable(11443);
		result = true;
	}
	return result;
}

bool CDlgChangeGenderPolicy::CheckIsEquippingAnyFashion(ACString &strMessage)const{
	bool result(false);
	strMessage.Empty();

	ACString strTemp, strName;
	ACString strSeperator = CECUIHelper::GetGameUIMan()->GetStringFromTable(11441);
	CECInventory * pPack = CECLogicHelper::GetHostPlayer()->GetPack(IVTRTYPE_EQUIPPACK);
	for (int i(0); i < pPack->GetSize(); ++ i){
		CECIvtrItem *pItem = pPack->GetItem(i);
		if (!pItem || !pItem->IsEquipment()){
			continue;
		}
		CECIvtrEquip *pEquip = dynamic_cast<CECIvtrEquip *>(pItem);
		if (!pEquip->IsFashion()){
			continue;
		}
		strName = pEquip->GetName();
		if (strName.IsEmpty()){
			strName.Format(_AL("(%d)"), pEquip->GetTemplateID());
		}
		if (!strTemp.IsEmpty()){
			strTemp += strSeperator;
		}
		strTemp += ColorString(strName);
		result = true;
	}
	if (!strTemp.IsEmpty()){
		strMessage.Format(CECUIHelper::GetGameUIMan()->GetStringFromTable(11444), strTemp);
	}
	return result;
}

bool CDlgChangeGenderPolicy::CheckSetPlayerProfileMatch(ACString &strMessage)const{
	bool result(false);
	strMessage.Empty();
	CDlgMatchProfileSetting *pDlgMatchSetting = dynamic_cast<CDlgMatchProfileSetting*>(CECUIHelper::GetGameUIMan()->GetDialog("Win_InfoMatch"));
	if (pDlgMatchSetting->IsServerMaskInitialized() && pDlgMatchSetting->GetServerMask()){
		strMessage = CECUIHelper::GetGameUIMan()->GetStringFromTable(11445);
		result = true;
	}
	return result;
}

bool CDlgChangeGenderPolicy::CheckHasGenderRelatedTask(ACString &strMessage)const{
	bool result(false);
	strMessage.Empty();

	CECTaskInterface * pTaskInterface = CECLogicHelper::GetHostPlayer()->GetTaskInterface();
	typedef abase::vector<unsigned long> TaskIDArray;
	typedef abase::vector<TaskIDArray> TaskIDArrays;
	TaskIDArrays genderRelatedTaskIDArrays;
	TaskIDArray	tempTaskIDArray;
	if (pTaskInterface->HasTopTaskRelatingMarriage(&tempTaskIDArray)){
		genderRelatedTaskIDArrays.push_back(tempTaskIDArray);
		tempTaskIDArray.clear();
		result = true;
	}
	if (pTaskInterface->HasTopTaskRelatingWedding(&tempTaskIDArray)){
		genderRelatedTaskIDArrays.push_back(tempTaskIDArray);
		tempTaskIDArray.clear();
		result = true;
	}
	if (pTaskInterface->HasTopTaskRelatingSpouse(&tempTaskIDArray)){
		genderRelatedTaskIDArrays.push_back(tempTaskIDArray);
		tempTaskIDArray.clear();
		result = true;
	}
	if (pTaskInterface->HasTopTaskRelatingGender(&tempTaskIDArray)){
		genderRelatedTaskIDArrays.push_back(tempTaskIDArray);
		tempTaskIDArray.clear();
		result = true;
	}
	if (!genderRelatedTaskIDArrays.empty()){
		ACString strTemp, strName;
		ACString strSeperator = CECUIHelper::GetGameUIMan()->GetStringFromTable(11441);
		for (size_t i(0); i < genderRelatedTaskIDArrays.size(); ++ i){
			const TaskIDArray &taskIDArray = genderRelatedTaskIDArrays[i];
			for (size_t j(0); j < taskIDArray.size(); ++ j){
				ATaskTempl *pTaskTempl = GetTaskTemplMan()->GetTopTaskByID(taskIDArray[j]);
				if (!pTaskTempl){
					ASSERT(false);
					continue;
				}
				strName = CECUIHelper::TrimColorPrefix(pTaskTempl->GetName());
				if (strName.IsEmpty()){
					strName.Format(_AL("(%d)"), pTaskTempl->GetID());
				}
				if (!strTemp.IsEmpty()){
					strTemp += strSeperator;
				}
				strTemp += ColorString(strName);
			}
		}
		if (!strTemp.IsEmpty()){
			strMessage.Format(CECUIHelper::GetGameUIMan()->GetStringFromTable(11446), strTemp);
		}
	}
	return result;
}

void CDlgChangeGenderPolicy::PrepareProtocolParameterImpl(){
	if (!m_pPlayerAfterChangeGender){
		CECHostPlayer *pHost = CECLogicHelper::GetHostPlayer();
		
		GNET::RoleInfo roleInfo;
		roleInfo.race = pHost->GetRace();
		roleInfo.occupation = pHost->GetProfession();
		roleInfo.gender = CECProfConfig::Instance().GetCounterpartGender(pHost->GetGender());
		
		CECPlayer::PLAYER_CUSTOMIZEDATA custom_data;
		if (CECPlayer::LoadDefaultCustomizeData(roleInfo.occupation, roleInfo.gender, custom_data)){
			roleInfo.custom_data.swap(GNET::Octets((const void *)&custom_data, sizeof(custom_data)));
		}
		m_pPlayerAfterChangeGender = new CECLoginPlayer(NULL);
		if (!m_pPlayerAfterChangeGender->Load(roleInfo)){
			A3DRELEASE(m_pPlayerAfterChangeGender);
			OnProtocolParameterPrepareFail();
			return;
		}
	}

	CECNPC *pNPC = CECLogicHelper::GetWorld()->GetNPCMan()->GetNPC(CECLogicHelper::GetHostPlayer()->GetSelectedTarget());
	if (!pNPC){
		OnProtocolParameterPrepareFail();
		return;
	}
	
	CECCustomizeMgr::CustomizeInitParam_t cip;	
	cip.pAUImanager = CECUIHelper::GetGameUIMan();
	cip.pCurPlayer = m_pPlayerAfterChangeGender;
	cip.nFaceTicketID = FACE_TICKET_DEFAULT;
	cip.vPos = pNPC->GetPos();
	cip.vDir = pNPC->GetDir();
	
	CECUIHelper::GetGameUIMan()->GetCustomizeMgr()->Init(cip, this);
	ShowPlayer(true);
	CECLogicHelper::GetHostPlayer()->StartChangeFace();
}

bool CDlgChangeGenderPolicy::ShouldHideAllDialog()const{
	return true;
}

bool CDlgChangeGenderPolicy::ShouldShowOKCancel()const{
	return true;
}

void CDlgChangeGenderPolicy::FinishCustomize(CECCustomizeMgr &manager){
	CECUIHelper::GetBaseUIMan()->ShowAllDialogs(false);
	manager.ShowAllOldDlg();
	manager.RestoreCamera();
	manager.Finish();	
	ShowPlayer(false);
	CECLogicHelper::GetHostPlayer()->StopChangeFace();
}

void CDlgChangeGenderPolicy::SendProtocol(int id, int iSlot){
	unsigned char newGender = CECProfConfig::Instance().GetCounterpartGender(CECLogicHelper::GetHostPlayer()->GetGender());
	const CECPlayer::PLAYER_CUSTOMIZEDATA & customizeData = m_pPlayerAfterChangeGender->GetCustomizeData();
	CECLogicHelper::GetGameSession()->c2s_CmdNPCSevPlayerChangeGender(iSlot, id, newGender, (void *)&customizeData, sizeof(customizeData));
}

void CDlgChangeGenderPolicy::OnOK(CECCustomizeMgr &manager){
	manager.ConfirmPlayerCustomizeData();
	FinishCustomize(manager);
	OnProtocolParameterReady();
}

void CDlgChangeGenderPolicy::OnCancel(CECCustomizeMgr &manager){
	FinishCustomize(manager);
	OnProtocolParameterPrepareCancel();
}

void CDlgChangeGenderPolicy::ShowPlayer(bool bShow){
	if (!m_pPlayerAfterChangeGender){
		return;
	}
	CECPlayerMan::LoginPlayerTable & players = CECLogicHelper::GetWorld()->GetPlayerMan()->GetLoginPlayerTable();
	CECPlayerMan::LoginPlayerTable::iterator it = std::find(players.begin(), players.end(), m_pPlayerAfterChangeGender);
	if (!bShow){
		if (it != players.end()){
			players.erase(it);
		}
	}else{
		if (it == players.end()){
			players.push_back(m_pPlayerAfterChangeGender);
		}
	}
}