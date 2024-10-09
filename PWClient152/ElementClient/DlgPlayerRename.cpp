/********************************************************************
	created:	2012/1/8
	created:	12:11:2012   16:31
	file base:	DlgPlayerRename
	file ext:	cpp
	author:		zhougaomin01305
	
	purpose:	½ÇÉ«¸ÄÃû
*********************************************************************/

#include "DlgPlayerRename.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

#include "auto_delete.h"

//	class CDlgModifyNamePolicy
void CDlgModifyNamePolicy::AddCandiateID(int id){
	m_candidateIDs.push_back(id);
}

bool CDlgModifyNamePolicy::GetCostItemID(int &id, int &iSlot)const{
	id = 0;
	iSlot = -1;
	if (m_candidateIDs.empty()){
		return false;
	}
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
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

//	class CDlgModifyName
AUI_BEGIN_COMMAND_MAP(CDlgModifyName, CDlgBase)
AUI_ON_COMMAND("Btn_Confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgModifyName, CDlgBase)
AUI_ON_EVENT("DEFAULT_Edt_Name",	WM_KEYUP,	OnEventKeyUp_Edt_Name)
AUI_END_EVENT_MAP()

CDlgModifyName::CDlgModifyName()
: m_pLbl_Title(NULL)
, m_pLbl_Msg(NULL)
, m_pEdt_Name(NULL)
, m_pBtn_Confirm(NULL)
, m_pPolicy(NULL)
{
}

void CDlgModifyName::OnEventKeyUp_Edt_Name(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{	
	if (wParam == VK_ESCAPE)
	{
		ChangeFocus(NULL);
		return;
	}

	ACString strName = m_pEdt_Name->GetText();
	m_pBtn_Confirm->Enable(!strName.IsEmpty() && m_index >= 0);
}

bool CDlgModifyName::OnInitDialog()
{
	DDX_Control("Lbl_Title", m_pLbl_Title);
	DDX_Control("Lbl_Msg", m_pLbl_Msg);
	DDX_Control("DEFAULT_Edt_Name", m_pEdt_Name);	
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	return CDlgBase::OnInitDialog();
}

bool CDlgModifyName::Release(){
	SetPolicy(NULL);
	return CDlgBase::Release();
}

void CDlgModifyName::SetPolicy(CDlgModifyNamePolicy *pNewPolicy){
	if (m_pPolicy == pNewPolicy){
		return;
	}
	delete m_pPolicy;
	m_pPolicy = pNewPolicy;
}

void CDlgModifyName::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_pLbl_Title->SetText(GetStringFromTable(m_pPolicy->GetTitleStringID()));
	m_pLbl_Msg->SetText(GetStringFromTable(m_pPolicy->GetInputHintStringID()));

	m_pEdt_Name->SetMaxLength(m_pPolicy->GetMaxNameLength());
	m_pEdt_Name->SetText(_AL(""));
		
	m_pBtn_Confirm->Enable(false);

	m_pPolicy->GetCostItemID(m_id, m_index);

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

void CDlgModifyName::SendRenameProtocol()
{	
	if (m_pPolicy){
		m_pPolicy->SendProtocol(m_pEdt_Name->GetText(), m_id, m_index);	
	}
	OnCommandCancel("");
}

const char * CDlgModifyName::GetMessageBoxName()const{
	return m_pPolicy ? m_pPolicy->GetMessageBoxName() : "";
}

void CDlgModifyName::OnCommandConfirm(const char *szCommand)
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	pGameUIMan->MessageBox(m_pPolicy->GetMessageBoxName(),
		GetStringFromTable(m_pPolicy->GetConfirmStringID()),
		MB_YESNO,
		pGameUIMan->GetMsgBoxColor());
}

void CDlgModifyName::OnCommandCancel(const char * szCommand)
{
	SetPolicy(NULL);
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetHostPlayer()->GetPack()->UnfreezeAllItems();
}

//	class CDlgPlayerRenamePolicy
CDlgPlayerRenamePolicy::CDlgPlayerRenamePolicy()
{
	AddCandiateID(46901);
	AddCandiateID(37302);
}

int CDlgPlayerRenamePolicy::GetTitleStringID()const{
	return 11430;
}

int CDlgPlayerRenamePolicy::GetInputHintStringID()const{
	return 11431;
}

int CDlgPlayerRenamePolicy::GetConfirmStringID()const{
	return 10153;
}

const char *CDlgPlayerRenamePolicy::GetMessageBoxName()const{
	return "PlayerRename";
}

void CDlgPlayerRenamePolicy::SendProtocol(const ACString &strNewName, int id, int iSlot){
	g_pGame->GetGameSession()->c2s_CmdNPCSevPlayerRename(iSlot, id, (unsigned short)(strNewName.GetLength() * sizeof(ACHAR)),(const char*)(const ACHAR *)strNewName);
}

int CDlgPlayerRenamePolicy::GetMaxNameLength()const{
	return g_pGame->GetConfigs()->GetMaxNameLen();
}

//	class CDlgFactionRenamePolicy
CDlgFactionRenamePolicy::CDlgFactionRenamePolicy()
{
	AddCandiateID(46903);
	AddCandiateID(46902);
}

int CDlgFactionRenamePolicy::GetTitleStringID()const{
	return 11432;
}

int CDlgFactionRenamePolicy::GetInputHintStringID()const{
	return 11433;
}

int CDlgFactionRenamePolicy::GetConfirmStringID()const{
	return 11434;
}

const char *CDlgFactionRenamePolicy::GetMessageBoxName()const{
	return "FactionRename";
}

void CDlgFactionRenamePolicy::SendProtocol(const ACString &strNewName, int id, int iSlot){
	g_pGame->GetGameSession()->faction_renamefaction(strNewName);
}

int CDlgFactionRenamePolicy::GetMaxNameLength()const{
	return 9;
}