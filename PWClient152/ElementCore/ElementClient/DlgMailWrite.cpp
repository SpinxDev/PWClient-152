// Filename	: DlgMailWrite.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#include "AFI.h"
#include "DlgMailWrite.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "playersendmail_re.hpp"
#include "DlgInputNO.h"
#include "EC_ShortcutMgr.h"
#include "EC_Utility.h"
#include "EC_FixedMsg.h"


AUI_BEGIN_COMMAND_MAP(CDlgMailWrite, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_ON_COMMAND("send",		OnCommandSend)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMailWrite, CDlgBase)

AUI_ON_EVENT("Img_AttachItem",	WM_LBUTTONDOWN,	OnEvent_LButtonDown_AttachItem)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgMailWrite
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgMailWrite> MailWriteClickShortcut;
//------------------------------------------------------------------------

CDlgMailWrite::CDlgMailWrite()
{
	m_pTxt_MailTo = NULL;
	m_pTxt_Subject = NULL;
	m_pTxt_Cost = NULL;
	m_pTxt_Content = NULL;
	m_pTxt_AttachGold = NULL;
	m_pImg_AttachItem = NULL;
	m_pTxt_PackGold = NULL;
	m_pBtn_Send = NULL;
	m_idItem = 0;
	m_nItemNumber = 0;
	m_nItemPos = 0;
}

CDlgMailWrite::~CDlgMailWrite()
{
}

bool CDlgMailWrite::OnInitDialog()
{
	DDX_Control("Txt_MailTo", m_pTxt_MailTo);
	DDX_Control("Txt_Subject", m_pTxt_Subject);
	DDX_Control("Txt_Cost", m_pTxt_Cost);
	DDX_Control("Txt_Content", m_pTxt_Content);
	DDX_Control("Txt_AttachGold", m_pTxt_AttachGold);
	DDX_Control("Img_AttachItem", m_pImg_AttachItem);
	DDX_Control("Txt_PackGold", m_pTxt_PackGold);
	DDX_Control("Btn_Send", m_pBtn_Send);
	m_pTxt_AttachGold->SetIsNumberOnly(true);
	m_pTxt_Content->SetIsAutoReturn(true);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new MailWriteClickShortcut(this));
	return true;
}

void CDlgMailWrite::OnCommandCancel(const char* szCommand)
{
	OnEvent_LButtonDown_AttachItem(0, 0, NULL);
	
	Show(false);
}

void CDlgMailWrite::OnCommandSend(const char* szCommand)
{
	int idPlayer = IsSendToMass() ? 0 : GetGameRun()->GetPlayerID(m_pTxt_MailTo->GetText());
	SendMail(idPlayer);
}

void CDlgMailWrite::OnEvent_LButtonDown_AttachItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( m_idItem == 0 )
		return;

	CECInventory *pPack = GetHostPlayer()->GetPack();
	CECIvtrItem *pItem = pPack->GetItem(m_nItemPos);
	if( pItem )
		pItem->Freeze(false);
	m_idItem = 0;
	m_nItemNumber = 0;
	m_nItemPos = 0;
	m_pImg_AttachItem->SetCover(NULL, -1);
	m_pImg_AttachItem->SetText(_AL(""));
}

void CDlgMailWrite::OnTick()
{
	if( m_idItem )
	{
		CECInventory *pPack = GetHostPlayer()->GetPack();
		CECIvtrItem *pItem = pPack->GetItem(m_nItemPos);
		if( !pItem )
		{
			m_idItem = 0;
			m_nItemNumber = 0;
			m_nItemPos = 0;
			m_pImg_AttachItem->SetCover(NULL, -1);
			m_pImg_AttachItem->SetText(_AL(""));
		}
	}
	m_pBtn_Send->Enable(glb_IsTextNotEmpty(m_pTxt_MailTo) && 
						glb_IsTextNotEmpty(m_pTxt_Subject) &&
						glb_IsTextNotEmpty(m_pTxt_Content) );

	m_pTxt_PackGold->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
	m_pTxt_PackGold->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));
}

void CDlgMailWrite::AttachGold(int money)
{
	if (m_pTxt_AttachGold->IsEnabled()){
		m_pTxt_AttachGold->SetText(ACString().Format(_AL("%d"), money));
	}
}

void CDlgMailWrite::CreateNewMail(const ACHAR *strMailto, ACString szSubject, int nCost, const MassReceivers &mass)
{
	m_pTxt_Content->SetText(_AL(""));
	m_pTxt_AttachGold->SetText(_AL("0"));
	m_pImg_AttachItem->SetCover(NULL, -1);
	m_pImg_AttachItem->SetText(_AL(""));
	m_nCost = nCost;
	m_massReceivers = mass;
	OnEvent_LButtonDown_AttachItem(0, 0, NULL);

	if( strMailto == NULL )
	{
		m_pTxt_MailTo->SetText(_AL(""));
		m_pTxt_Subject->SetText(_AL(""));
	}
	else
	{
		m_pTxt_MailTo->SetText(strMailto);
		m_pTxt_Subject->SetText(szSubject);
	}

	m_pTxt_Cost->SetText(ACString().Format(GetStringFromTable(11232), GetBaseUIMan()->GetFormatNumber(nCost)));
	
	m_pTxt_MailTo->Enable(!IsSendToMass());
	m_pTxt_AttachGold->Enable(!IsSendToMass());
	m_pImg_AttachItem->Enable(!IsSendToMass());

	//	°´Ë³ÐòÑ¡Ôñ½¹µã
	PAUIOBJECT pObjFocus = NULL;
	if (strMailto == NULL){
		pObjFocus = m_pTxt_MailTo;
	}else if (szSubject.IsEmpty()){
		pObjFocus = m_pTxt_Subject;
	}else{
		pObjFocus = m_pTxt_Content;
	}
	ChangeFocus(pObjFocus);

	Show(true);
}

void CDlgMailWrite::SetAttachItem(CECIvtrItem *pItem, int nItemNumber, int nItemPos)
{
	if (IsSendToMass()){
		return;
	}
	OnEvent_LButtonDown_AttachItem(0, 0, NULL);
	pItem->Freeze(true);
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), nItemNumber);
	m_pImg_AttachItem->SetText(szText);
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_AttachItem->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	m_idItem = pItem->GetTemplateID();
	m_nItemNumber = nItemNumber;
	m_nItemPos = nItemPos;
}

void CDlgMailWrite::SendMail(int idPlayer)
{
	if (idPlayer == -2)
	{		
		CECStringTab* pStrTab = GetGame()->GetFixedMsgTab();
		const wchar_t* szFixMsg = pStrTab->GetWideString(FIXMSG_OBSOLETE_ROLENAME);
		GetGameUIMan()->ShowErrorMsg(szFixMsg);
		return;
	}
	if( idPlayer == -1 )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(643), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if( !IsShow() )
		return;

	if( glb_IsTextNotEmpty(m_pTxt_MailTo) && 
		glb_IsTextNotEmpty(m_pTxt_Subject) &&
		glb_IsTextNotEmpty(m_pTxt_Content) )
	{
		if( idPlayer == GetHostPlayer()->GetCharacterID() )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(641), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		int nAttachGold = a_atoi(m_pTxt_AttachGold->GetText());
		int nPackGold = GetHostPlayer()->GetMoneyAmount();
		if( nAttachGold >= 0 && nPackGold < m_nCost )
		{
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("", GetStringFromTable(635), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}

		if( nAttachGold < 0 || nAttachGold + m_nCost > nPackGold )
		{
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("", GetStringFromTable(636), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}

		if (!IsSendToMass()){
			if( idPlayer != 0 )
				GetGameSession()->mail_Send(
				idPlayer ,
				m_pTxt_Subject->GetText(), m_pTxt_Content->GetText(), 
				m_idItem, m_nItemNumber, m_nItemPos, nAttachGold);
			else
				GetGameSession()->GetPlayerIDByName(m_pTxt_MailTo->GetText(), 3);
		}else{
			GetGameSession()->c2s_CmdSendMassMail(0, m_pTxt_Subject->GetText(), m_pTxt_Content->GetText(), m_massReceivers);
		}
	}
}

void CDlgMailWrite::SendMailRe(void *pData)
{
	PlayerSendMail_Re *pPreserveMail = (PlayerSendMail_Re *)pData;
	if( pPreserveMail->retcode == ERR_SUCCESS)
	{
		Show(false);
		int nAttachGold = a_atoi(m_pTxt_AttachGold->GetText());
		if( m_idItem == 0 )
			GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(637), GP_CHAT_MISC);
		else
			GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(649), GP_CHAT_MISC);
		m_idItem = 0;
		m_nItemNumber = 0;
		m_nItemPos = 0;
		m_pImg_AttachItem->SetCover(NULL, -1);
		m_pImg_AttachItem->SetText(_AL(""));
	}
	else if( pPreserveMail->retcode == ERR_MS_BOXFULL)
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(650), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	else if( pPreserveMail->retcode == ERR_MS_ACCOUNTFROZEN)
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(651), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	else if( pPreserveMail->retcode == ERR_MS_ATTACH_INV)
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(640), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	else if( pPreserveMail->retcode == ERR_MS_SEND_SELF)
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(641), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	else
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(638), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgMailWrite::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (IsSendToMass()){
		return;
	}
	// default drag-drop to this object
	pObjOver = GetDlgItem("Img_AttachItem");
	
	if(!pObjOver || !pIvtrSrc->IsTradeable() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(652), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( pIvtrSrc->GetCount() == 1 )
	{
		this->SetAttachItem(pIvtrSrc, 1, iSrc );
	}
	else
	{
		GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
			CDlgInputNO::INPUTNO_MOVE_MAILATTACH, pIvtrSrc->GetCount());
	}
}

bool CDlgMailWrite::IsSendToMass()
{
	return !m_massReceivers.empty();
}