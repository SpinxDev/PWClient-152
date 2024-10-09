// Filename	: DlgWeddingInvite.cpp
// Creator	: Xu Wenbin
// Date		: 2010/07/08

#include "AFI.h"
#include "DlgWeddingInvite.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrConsume.h"
#include "EC_Inventory.h"
#include "EC_GameSession.h"
#include "elementdataman.h"
#include "EC_ShortcutMgr.h"
#include <time.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWeddingInvite, CDlgBase)
AUI_ON_COMMAND("Btn_Confirm",	OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Cancel",	OnCommand_Cancel)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Cancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWeddingInvite, CDlgBase)
AUI_ON_EVENT("Img_*",	WM_LBUTTONDOWN,	OnEventLButtonDown_Img)
AUI_ON_EVENT("Txt_Name",WM_KEYDOWN,	OnEventKeyDown_Name)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgWeddingInvite
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgWeddingInvite> WeddingInviteClickShortcut;
//------------------------------------------------------------------------

CDlgWeddingInvite::CDlgWeddingInvite()
{
	m_pImg_01 = NULL;
	m_pTxt_01 = NULL;
	m_pTxt_Name = NULL;
	m_pBtn_Confirm = NULL;
	m_pBtn_Cancel = NULL;
	m_bConfirmed = false;
	m_iSlot = -1;
}

bool CDlgWeddingInvite::OnInitDialog()
{
	DDX_Control("Img_01", m_pImg_01);
	DDX_Control("Txt_01", m_pTxt_01);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Btn_Cancel", m_pBtn_Cancel);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new WeddingInviteClickShortcut(this));
	return true;
}

void CDlgWeddingInvite::OnShowDialog()
{
	m_bConfirmed = false;
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Name->Enable(true);

	//	�Ӱ���������������Ĭ����Ʒ
	int iSlotDefault(-1);
	CECInventory *pPack = GetHostPlayer()->GetPack();
	for (int i = 0; i < pPack->GetSize(); ++ i)
	{
		if (IsImageItemValid(i))
		{
			iSlotDefault = i;
			break;
		}
	}
	SetItem(iSlotDefault);
}

void CDlgWeddingInvite::OnCommand_Cancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	m_bConfirmed = false;
	SetItem(-1);
	Show(false);
}

void CDlgWeddingInvite::OnTick()
{
	if (m_bConfirmed)
	{
		//	ָ�����ʱ����� ID �Ƿ��ѻ�ò�����Э��
		//
		ACString strName = m_pTxt_Name->GetText();
		if (!strName.IsEmpty())
		{
			//	���Բ鿴��ҵ� ID 
			int id = GetGameRun()->GetPlayerID(strName);
			bool bRewrite(false), bClearName(false);
			if (id > 0)
			{
				//	��ҵ� ID �Ѿ���ѯ��������Э�鵽��������
				WriteCard(true, false);
				
				//	�Ѿ�����Э�飬�������棬д�������
				bRewrite = true;
				bClearName = true;
			}
			else
			{
				//	��鳬ʱ
				time_t newTime = time(NULL);
				if (newTime - m_sendTime >= 10)
				{
					//	��ѯID��ʱ��������Ϣ����ʾ
					GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8750));
					
					//	�Ѿ���ʱ����������
					bRewrite = true;
					bClearName = false;
				}
			}
			
			if (bRewrite)
				Rewrite(bClearName);
		}

		return;
	}

	//	�����������Ӱ��ȷ��״̬
	m_pBtn_Confirm->Enable(CanConfirm());
}

void CDlgWeddingInvite::WriteCard(bool bOK, bool bForAnyOne /* = true */)
{
	//	����д���Э�飬��ȡ��д���
	//
	if (m_bConfirmed)
	{
		//	���ڵȴ�ȷ���У���������
		//
		bool bClearName(false);
		if (bOK)
		{
			if (IsImageItemValid(m_iSlot) &&
				IsInviteItemValid())
			{
				//	��Ʒ������
				int id(0);
				if (!bForAnyOne)
				{
					//	ָ�������
					ACString strName = m_pTxt_Name->GetText();
					id = GetGameRun()->GetPlayerID(strName);
					if (id <= 0)
					{
						//	�޷��鵽��� ID ʱ���� -1 ������Э��
						id = -1;
					}
					else
					{
						//	��� ID �Ƿ������ɻ�����
						CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_iSlot);
						CECIvtrWeddingInviteCard *pInviteCard = dynamic_cast<CECIvtrWeddingInviteCard*>(pItem);
						const IVTR_ESSENCE_WEDDING_INVITECARD &essence = pInviteCard->GetEssence();
						if (id == essence.groom || id == essence.bride)
						{
							//	�����д������
							GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8772));
							id = -1;
						}
					}
				}
				if (id >= 0)
				{
					//	���Է���Э��
					GetGameSession()->c2s_CmdNPCSevWeddingInvite(m_iSlot, id);
					bClearName = true;
				}
			}
		}

		//	���۳ɹ�ʧ�ܣ�����������д
		Rewrite(bClearName);
	}
}

void CDlgWeddingInvite::Rewrite(bool bClearName)
{
	if (m_bConfirmed)
	{
		//	����������д����
		m_bConfirmed = false;
		m_pTxt_Name->Enable(true);
		m_pBtn_Confirm->Enable(CanConfirm());
		if (bClearName)
			m_pTxt_Name->SetText(_AL(""));
	}
}

void CDlgWeddingInvite::OnEventLButtonDown_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	���������Ʒ
	SetItem(-1);
}

void CDlgWeddingInvite::OnEventKeyDown_Name(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);
}

void CDlgWeddingInvite::OnCommand_Confirm(const char *szCommand)
{
	if (!CanConfirm())
		return;

	m_bConfirmed = true;
	m_sendTime = time(NULL);

	//	��ʱ�������ȷ�����ȴ����ؽ��
	m_pBtn_Confirm->Enable(false);

	//	��ʱ��ֹ�޸����ƣ��ȴ����ؽ��
	m_pTxt_Name->Enable(false);

	ACString strName = m_pTxt_Name->GetText();
	if (!strName.IsEmpty())
	{
		//	ָ������ң���Ҫ��ѯ ID ����ܷ�Э��
		int id = GetGameRun()->GetPlayerID(strName);
		if (id <= 0)
			GetGameSession()->GetPlayerIDByName(strName, 0);
	}
	else
	{
		//	������ʾ��ȷ�ϲ����������
		PAUIDIALOG pMsgBox;		
		GetGameUIMan()->MessageBox("Game_WriteCard", GetStringFromTable(8771),
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	}

	//	�� OnTick �в�ѯ��� ID ������ȷ��Э��
}

bool CDlgWeddingInvite::CanConfirm()
{
	//	�жϵ�ǰ�ܷ���ȷ���Դ���д���Ҫ��
	//
	bool bCan(false);

	while (true)
	{
		if (m_bConfirmed)
		{
			//	�յ����ȷ��
			break;
		}
		if (!IsImageItemValid(m_iSlot))
		{
			//	û����Ʒ
			break;
		}
		if (!IsInviteItemValid())
		{
			//	û�����ԭ��
			break;
		}

		bCan = true;
		break;
	}
	return bCan;
}

void CDlgWeddingInvite::SetItem(int iSlot /* = -1 */)
{
	//	���ȷ�Ϻ���ʱ�������޸�
	if (m_bConfirmed)
		return;

	while (true)
	{
		CECIvtrItem *pItem = NULL;
		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		if (IsImageItemValid(iSlot))
			pItem = pPack->GetItem(iSlot);

		//	�����Ϸ�
		CECIvtrItem *pItemOld = NULL;
		if (IsImageItemValid(m_iSlot))
			pItemOld = pPack->GetItem(m_iSlot);

		//	���ͼ��
		m_pImg_01->SetCover(NULL, -1);

		//	����������
		m_pTxt_01->SetText(_AL(""));
		if (pItemOld)
			pItemOld->Freeze(false);

		//	��ʾ�½��
		m_iSlot = iSlot;

		if (!pItem)
			break;
		
		pItem->Freeze(true);
		
		//	����ͼ��
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_01->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		//	�����������
		CECIvtrWeddingInviteCard *pInviteCard = dynamic_cast<CECIvtrWeddingInviteCard*>(pItem);
		const IVTR_ESSENCE_WEDDING_INVITECARD &essence = pInviteCard->GetEssence();

		ACString strTime = GetTimeString(essence.start_time, essence.end_time);

		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(CECIvtrWeddingInviteCard::WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);

		ACString strText;
		strText.Format(GetStringFromTable(8751), strTime, pWeddingConfig->wedding_scene[essence.scene].name);
		m_pTxt_01->SetText(strText);

		break;
	}
	
	//	����״̬
	m_pBtn_Confirm->Enable(CanConfirm());

}

ACString CDlgWeddingInvite::GetTimeString(int start_time, int end_time)
{
	int timeBias = GetGame()->GetTimeZoneBias() * 60;
	int localTime1 = start_time - timeBias;
	tm t1 = *gmtime((time_t*)&localTime1);
	int localTime2 = end_time - timeBias;
	tm t2 = *gmtime((time_t *)&localTime2);
	
	ACString strText;
	int idString =
		(t1.tm_year == t2.tm_year) &&
		(t1.tm_mon == t2.tm_mon) &&
		(t1.tm_mday == t2.tm_mday)
		? 8773 : 8774;
	strText.Format(GetStringFromTable(idString),
		t1.tm_year+1900,
		t1.tm_mon+1,
		t1.tm_mday,
		t1.tm_hour,
		t1.tm_min,
		t2.tm_hour,
		t2.tm_min);

	return strText;
}

bool CDlgWeddingInvite::IsImageItemValid(int iSlot)
{
	//	�ж����ɣ�����Ĳο���Ʒ�Ƿ�Ϸ�
	bool bValid(false);
	while (true)
	{
		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		if (iSlot < 0 && iSlot >= pPack->GetSize())
			break;
		
		CECIvtrItem *pItem = pPack->GetItem(iSlot);
		if (!pItem)
			break;
				
		if (pItem->GetClassID() != CECIvtrItem::ICID_WEDDINGINVITECARD)
			break;
		
		CECIvtrWeddingInviteCard *pInviteCard = dynamic_cast<CECIvtrWeddingInviteCard*>(pItem);
		if (!pInviteCard)
			break;
		
		const IVTR_ESSENCE_WEDDING_INVITECARD &essence = pInviteCard->GetEssence();
		
		//	������ñ��Ƿ���ȷ
		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(CECIvtrWeddingInviteCard::WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);
		if (!pWeddingConfig || DataType!=DT_WEDDING_CONFIG)
			break;
		
		if (essence.scene<0 ||
			essence.scene>=sizeof(pWeddingConfig->wedding_scene)/sizeof(pWeddingConfig->wedding_scene[0]) ||
			!pWeddingConfig->wedding_scene[essence.scene].name[0])
			break;
		
		//	��� ID �Ƿ���ȷ
		int id = pHost->GetCharacterID();
		
		if (essence.invitee != id)
		{
			//	�������߲��Ǳ���
			break;
		}
		
		if (essence.groom != id &&
			essence.bride != id)
		{
			//	�������ɻ������ﲻ����д
			break;
		}
		
		bValid = true;
		break;
	}
	return bValid;
}

bool CDlgWeddingInvite::IsInviteItemValid()
{
	//	�ж����ԭ���Ƿ�Ϸ�
	return GetHostPlayer()->GetPack()->GetItemTotalNum(INVITE_TICKET_ID) > 0;
}

void CDlgWeddingInvite::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_01", only one available drag-drop target
	this->SetItem(iSrc);
}