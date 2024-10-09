// File		: EC_ComputerAid.cpp
// Creator	: Xu Wenbin
// Date		: 2012/8/8

#include "EC_ComputerAid.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "auto_delete.h"
#include "DlgInfo.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"

CEComputerAid::CEComputerAid()
{
	for (int i(0); i < COUNT; ++ i)
	{
		m_counters[i].SetPeriod(2000);
		m_counters[i].Reset(true);
	}
}

CEComputerAid & CEComputerAid::Instance()
{
	static CEComputerAid s_dummy;
	return s_dummy;
}

bool CEComputerAid::Set(const EC_COMPUTER_AIDED_SETTING &cas)
{
	for (int i(0); i < COUNT; ++ i)
	{
		if (Validate(i, cas.group[i])) return false;
	}
	g_pGame->GetConfigs()->SetComputerAidedSetting(cas);
	return true;
}

const EC_COMPUTER_AIDED_SETTING & CEComputerAid::Get()const
{
	return g_pGame->GetConfigs()->GetComputerAidedSetting();
}

bool CEComputerAid::ValidateItem(int i, int item)const
{
	bool bOK(false);
	
	while (item > 0)
	{
		DATA_TYPE dt;
		const void * pDBEssence = g_pGame->GetElementDataMan()->get_data_ptr(item, ID_SPACE_ESSENCE, dt);
		if (!pDBEssence) break;
		
		if (i == 0)
		{
			if (dt != DT_MEDICINE_ESSENCE) break;			
			const MEDICINE_ESSENCE * pMedicine = (MEDICINE_ESSENCE *) pDBEssence;
			if (pMedicine->id_major_type != 1794 && pMedicine->id_major_type != 1810)
				break;
		}
		else if (i == 1)
		{
			if (dt != DT_MEDICINE_ESSENCE) break;			
			const MEDICINE_ESSENCE * pMedicine = (MEDICINE_ESSENCE *) pDBEssence;
			if (pMedicine->id_major_type != 1802 && pMedicine->id_major_type != 1810)
				break;
		}
		else if (i == 2)
		{
			if (dt != DT_ARMORRUNE_ESSENCE) break;
		}
		else if (i == 3)
		{
			if (dt != DT_MEDICINE_ESSENCE &&
				dt != DT_SKILLMATTER_ESSENCE)
				break;
		}
		else if (i == 4)
		{
			if (dt != DT_ARMORRUNE_ESSENCE) break;
		}
		else
		{
			ASSERT(false);
			break;
		}
		
		bOK = true;
		break;
	}
	
	return bOK;
}

bool CEComputerAid::ValidateItem(int i, const CECIvtrItem *pItem)const
{
	return pItem && ValidateItem(i, pItem->GetTemplateID());
}

bool CEComputerAid::ValidateSlot(int i, short slot)const
{
	//	判断普通包裹中 slot 处物品是否适合用于 group

	bool bOK(false);

	if (slot >= 0)
	{
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECIvtrItem *pItem = pHost->GetPack()->GetItem(slot);
		bOK = ValidateItem(i, pItem);
	}

	return bOK;
}

bool CEComputerAid::ValidatePercent(int i, char percent, bool bForEnable)const
{
	bool bOK(false);
	while (true)
	{
		if (i < 0 || i >= COUNT){
			ASSERT(false);
			break;
		}
		if (bForEnable){
			if (percent < 1){
				break;
			}
		}else{
			if (percent < 0){
				//	禁用时，允许 percent 为0
				break;
			}
		}
		switch (i)
		{
		case 2:
		case 4:
			bOK = (percent <= 100);
			break;
			
		default:
			bOK = (percent < 100);
			break;
		}
		break;
	}
	return bOK;
}

int CEComputerAid::Validate(int i, const ITEM_GROUP &group)const
{
	//	返回：0 合法 1 物品非法 2 百分比非法

	int ret = -1;

	while (i >= 0 && i < COUNT)
	{
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECIvtrItem *pItem = pHost->GetPack()->GetItem(group.slot);

		if (!group.enable)
		{
			if (pItem)
			{
				//	有物品时验证物品合法性
				if (pItem->GetTemplateID() != group.item ||
					!ValidateSlot(i, group.slot))
				{
					ret = 1;
					break;
				}
			}

			if (!ValidatePercent(i, group.percent, group.enable))
			{
				ret = 2;
				break;
			}

			ret = 0;
			break;
		}
		
		//	未禁用时，物品必须存在且合法
		if (group.item <= 0)
		{
			ret = 1;
			break;
		}
		
		if (!pItem || pItem->GetTemplateID() != group.item ||
			!ValidateSlot(i, group.slot))
		{
			ret = 1;
			break;
		}
		
		//	比例
		if (!ValidatePercent(i, group.percent, group.enable))
		{
			ret = 2;
			break;
		}

		ret = 0;
		break;
	}

	return ret;
}

bool CEComputerAid::MakeValid(int i, ITEM_GROUP &group)const
{
	bool bModified(false);

	ASSERT(i >= 0 && i < COUNT);
	while (i >= 0 && i < COUNT)
	{
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECInventory *pPack = pHost->GetPack();
		CECIvtrItem *pItem = pPack->GetItem(group.slot);

		//	验证 item slot enable
		if (group.item > 0)
		{
			if (!ValidateItem(i, group.item))
			{
				//	此物品(已经)不适合摆放在此位置，或 elements.data 错误导致物品消失
				group.Reset();
				bModified = true;
			}
			if (group.item > 0 && (!pItem || pItem->GetTemplateID() != group.item))
			{
				//	slot 处物品用完
				int newslot = pPack->FindItem(group.item);
				if (newslot >= 0)
				{
					//	在其它位置找到同类物品
					group.slot = newslot;
				}
				else
				{
					//	物品用完
					group.Reset();
				}
				bModified = true;
			}
		}
		else
		{
			if (group.enable)
			{
				group.enable = false;
				bModified = true;
			}
			if (group.item != 0)
			{
				group.item = 0;
				bModified = true;
			}
			if (group.slot >= 0)
			{
				group.slot = -1;
				bModified = true;
			}
		}

		//	验证 percent
		if (group.enable)
		{
			if (!ValidatePercent(i, group.percent, group.enable))
			{
				group.percent = 0;
				group.enable = false;
				bModified = true;
			}
		}
		else
		{
			if (!ValidatePercent(i, group.percent, group.enable))
			{
				group.percent = 0;
				bModified = true;
			}
		}

		break;
	}

	return bModified;
}

bool CEComputerAid::CanUse()const
{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost->HaveHealthStones();
}

void CEComputerAid::Tick()
{
	int i = 0;

	//	定时器应及时更新
	DWORD dwTickTime = g_pGame->GetRealTickTime();
	for (i = 0; i < COUNT; ++ i)
	{
		if (!m_counters[i].IsFull())
			m_counters[i].IncCounter(dwTickTime);
	}
	
	//	无网络连接时，不做任何修改或处理
	if (!g_pGame->GetGameSession()->IsConnected())
	{
		return;
	}
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost->HostIsReady() || 
		pHost->IsDead() ||
		!CanUse())
	{
		return;
	}
	//	刚复活，如果装备里有符、则会直接回满血、蓝，此时忽略当前功能，避免浪费
	if (pHost->GetReviveTime() && CECTimeSafeChecker::ElapsedTimeFor(pHost->GetReviveTime()) < 2000){
		if (pHost->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(EQUIPIVTR_AUTOHP) ||
			pHost->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(EQUIPIVTR_AUTOMP)){
			return;
		}
	}

	//	验证物品
	abase::vector<int> disappeared;
	EC_COMPUTER_AIDED_SETTING temp = Get();
	bool bModified(false);
	for (i = 0; i < COUNT; ++ i)
	{
		ITEM_GROUP &group = temp.group[i];
		bool enable = group.enable;
		int item = group.item;
		if (MakeValid(i, group))
		{
			bModified = true;
			if (enable && item > 0 && group.item <= 0)
			{
				//	只记录当前启用物品消失
				disappeared.push_back(item);
			}
		}
	}

	if (bModified && !Set(temp))
	{
		ASSERT(false);
		return;
	}
	
#ifdef _DEBUG
	if (bModified)
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, _AL("CLIENT - autohpmp modified"));
#endif

	//	提示消失物品
	if (!disappeared.empty())
	{
		CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		ACString strFormat = pGameUIMan->GetStringFromTable(9930);
		ACString strText;
		for (size_t i(0); i < disappeared.size(); ++ i)
		{
			CECIvtrItem *pItem = CECIvtrItem::CreateItem(disappeared[i], 0, 1);
			auto_delete<CECIvtrItem> tmp(pItem);
			strText.Format(strFormat, pItem->GetName());
			pGameUIMan->AddInformation(CDlgInfo::INFO_SYSTEM, "", strText, -1, 0, 0, 0);
		}
	}

	//	执行喝药逻辑
	Condition c;
	const ROLEEXTPROP& ep = pHost->GetExtendProps();
	const ROLEBASICPROP& bp = pHost->GetBasicProps();
	c.cur_hp_percent = bp.iCurHP/(float)max(ep.bs.max_hp, 1) * 100;
	c.cur_mp_percent = bp.iCurMP/(float)max(ep.bs.max_mp, 1) * 100;

	const EC_COMPUTER_AIDED_SETTING &cas = Get();
	for (i = 0; i < COUNT; ++ i)
	{
		const ITEM_GROUP &group = cas.group[i];
		if (!group.enable || !m_counters[i].IsFull() || !Meet(i, c))
			continue;
		if (pHost->UseItemInPack(IVTRTYPE_PACK, group.slot, false))
		{
			m_counters[i].Reset();
#ifdef _DEBUG
			g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, _AL("CLIENT - autohpmp %d used"), i);
#endif
		}
	}
}

bool CEComputerAid::Meet(int i, const Condition &c)const
{
	bool bMeet(false);

	const EC_COMPUTER_AIDED_SETTING &cas = Get();
	const ITEM_GROUP &group = cas.group[i];
	switch (i)
	{
	case 1:
		bMeet = group.percent >= c.cur_mp_percent;
		break;

	default:
		bMeet = group.percent >= c.cur_hp_percent;
		break;
	}

	return bMeet;
}
