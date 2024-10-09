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

	//	从包裹中搜索并设置默认物品
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
		//	指定玩家时，检查 ID 是否已获得并发送协议
		//
		ACString strName = m_pTxt_Name->GetText();
		if (!strName.IsEmpty())
		{
			//	测试查看玩家的 ID 
			int id = GetGameRun()->GetPlayerID(strName);
			bool bRewrite(false), bClearName(false);
			if (id > 0)
			{
				//	玩家的 ID 已经查询到，发送协议到服务器端
				WriteCard(true, false);
				
				//	已经发送协议，重启界面，写给新玩家
				bRewrite = true;
				bClearName = true;
			}
			else
			{
				//	检查超时
				time_t newTime = time(NULL);
				if (newTime - m_sendTime >= 10)
				{
					//	查询ID超时，弹出消息框提示
					GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8750));
					
					//	已经超时，重启界面
					bRewrite = true;
					bClearName = false;
				}
			}
			
			if (bRewrite)
				Rewrite(bClearName);
		}

		return;
	}

	//	名字输入可能影响确认状态
	m_pBtn_Confirm->Enable(CanConfirm());
}

void CDlgWeddingInvite::WriteCard(bool bOK, bool bForAnyOne /* = true */)
{
	//	发送写请柬协议，或取消写请柬
	//
	if (m_bConfirmed)
	{
		//	还在等待确认中，接受请求
		//
		bool bClearName(false);
		if (bOK)
		{
			if (IsImageItemValid(m_iSlot) &&
				IsInviteItemValid())
			{
				//	物品都还在
				int id(0);
				if (!bForAnyOne)
				{
					//	指定了玩家
					ACString strName = m_pTxt_Name->GetText();
					id = GetGameRun()->GetPlayerID(strName);
					if (id <= 0)
					{
						//	无法查到玩家 ID 时，置 -1 不发送协议
						id = -1;
					}
					else
					{
						//	检查 ID 是否是新郎或新娘
						CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_iSlot);
						CECIvtrWeddingInviteCard *pInviteCard = dynamic_cast<CECIvtrWeddingInviteCard*>(pItem);
						const IVTR_ESSENCE_WEDDING_INVITECARD &essence = pInviteCard->GetEssence();
						if (id == essence.groom || id == essence.bride)
						{
							//	请柬不能写给新人
							GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8772));
							id = -1;
						}
					}
				}
				if (id >= 0)
				{
					//	可以发送协议
					GetGameSession()->c2s_CmdNPCSevWeddingInvite(m_iSlot, id);
					bClearName = true;
				}
			}
		}

		//	无论成功失败，重启界面填写
		Rewrite(bClearName);
	}
}

void CDlgWeddingInvite::Rewrite(bool bClearName)
{
	if (m_bConfirmed)
	{
		//	开启重新填写界面
		m_bConfirmed = false;
		m_pTxt_Name->Enable(true);
		m_pBtn_Confirm->Enable(CanConfirm());
		if (bClearName)
			m_pTxt_Name->SetText(_AL(""));
	}
}

void CDlgWeddingInvite::OnEventLButtonDown_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//	清除特殊物品
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

	//	暂时继续点击确定，等待返回结果
	m_pBtn_Confirm->Enable(false);

	//	暂时禁止修改名称，等待返回结果
	m_pTxt_Name->Enable(false);

	ACString strName = m_pTxt_Name->GetText();
	if (!strName.IsEmpty())
	{
		//	指定了玩家，需要查询 ID 后才能发协议
		int id = GetGameRun()->GetPlayerID(strName);
		if (id <= 0)
			GetGameSession()->GetPlayerIDByName(strName, 0);
	}
	else
	{
		//	弹出提示框，确认不填玩家名称
		PAUIDIALOG pMsgBox;		
		GetGameUIMan()->MessageBox("Game_WriteCard", GetStringFromTable(8771),
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	}

	//	在 OnTick 中查询玩家 ID 并发送确认协议
}

bool CDlgWeddingInvite::CanConfirm()
{
	//	判断当前能否点击确定以处理写请柬要求
	//
	bool bCan(false);

	while (true)
	{
		if (m_bConfirmed)
		{
			//	刚点击了确定
			break;
		}
		if (!IsImageItemValid(m_iSlot))
		{
			//	没有物品
			break;
		}
		if (!IsInviteItemValid())
		{
			//	没有请柬原型
			break;
		}

		bCan = true;
		break;
	}
	return bCan;
}

void CDlgWeddingInvite::SetItem(int iSlot /* = -1 */)
{
	//	点击确认后，暂时不允许修改
	if (m_bConfirmed)
		return;

	while (true)
	{
		CECIvtrItem *pItem = NULL;
		CECHostPlayer *pHost = GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		if (IsImageItemValid(iSlot))
			pItem = pPack->GetItem(iSlot);

		//	参数合法
		CECIvtrItem *pItemOld = NULL;
		if (IsImageItemValid(m_iSlot))
			pItemOld = pPack->GetItem(m_iSlot);

		//	清除图标
		m_pImg_01->SetCover(NULL, -1);

		//	清除请柬内容
		m_pTxt_01->SetText(_AL(""));
		if (pItemOld)
			pItemOld->Freeze(false);

		//	显示新结果
		m_iSlot = iSlot;

		if (!pItem)
			break;
		
		pItem->Freeze(true);
		
		//	更新图标
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_01->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		//	更新请柬内容
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
	
	//	更新状态
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
	//	判断新郎（新娘）的参考物品是否合法
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
		
		//	检查配置表是否正确
		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(CECIvtrWeddingInviteCard::WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);
		if (!pWeddingConfig || DataType!=DT_WEDDING_CONFIG)
			break;
		
		if (essence.scene<0 ||
			essence.scene>=sizeof(pWeddingConfig->wedding_scene)/sizeof(pWeddingConfig->wedding_scene[0]) ||
			!pWeddingConfig->wedding_scene[essence.scene].name[0])
			break;
		
		//	检查 ID 是否正确
		int id = pHost->GetCharacterID();
		
		if (essence.invitee != id)
		{
			//	被邀请者不是本人
			break;
		}
		
		if (essence.groom != id &&
			essence.bride != id)
		{
			//	不是新郎或者新娘不能填写
			break;
		}
		
		bValid = true;
		break;
	}
	return bValid;
}

bool CDlgWeddingInvite::IsInviteItemValid()
{
	//	判断请柬原型是否合法
	return GetHostPlayer()->GetPack()->GetItemTotalNum(INVITE_TICKET_ID) > 0;
}

void CDlgWeddingInvite::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_01", only one available drag-drop target
	this->SetItem(iSrc);
}