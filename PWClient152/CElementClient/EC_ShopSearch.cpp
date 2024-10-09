// Filename	: EC_ShopSearch.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/11

#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "globaldataman.h"
#include "EC_UIConfigs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "TaskTempl.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"

//	CECShopSearch
CECShopSearch::CECShopSearch(CECShopBase *p)
: m_pShop(p)
, m_pPolicy(NULL)
{
}

CECShopSearch::~CECShopSearch()
{
	delete m_pPolicy;
}

void CECShopSearch::SetShop(CECShopBase *p)
{
	m_pShop = p;
}

void CECShopSearch::SetPolicy(CECShopSearchPolicyBase *policy)
{
	delete m_pPolicy;
	m_pPolicy = policy;
}

bool CECShopSearch::Search(CECShopSearchResult *result)
{
	bool bEmpty(true);
	if (m_pShop && m_pPolicy){
		CECShopSearchResult tmp(m_pShop);
		int count = m_pShop->GetCount();
		for (int i(0); i < count; ++ i)
		{
			if (m_pPolicy->CanAccept(m_pShop, i)){
				tmp.Add(i);
			}
			if (m_pPolicy->CanStop(tmp.Count())){
				break;
			}
		}
		bEmpty = tmp.IsEmpty();
		if (result){
			result->Swap(tmp);
		}
	}
	return !bEmpty;
}

//	CECShopSearchResult
CECShopSearchResult::CECShopSearchResult(CECShopBase *p)
: m_pShop(p)
{
}

bool CECShopSearchResult::Add(int index)
{
	bool bAdded(false);
	if (m_pShop->GetItem(index)){
		m_candidates.push_back(index);
		bAdded = true;
	}
	return bAdded;
}

void CECShopSearchResult::Filter(CECShopSearchPolicyBase &policy)
{
	if (IsEmpty()){
		return;
	}
	Candidates tmp;
	for (Candidates::const_iterator cit = m_candidates.begin(); cit != m_candidates.end(); ++ cit)
	{
		int index = *cit;
		if (policy.CanAccept(m_pShop, index)){
			tmp.push_back(index);
		}
		if (policy.CanStop((int)tmp.size())){
			break;
		}
	}
	if (tmp.size() != m_candidates.size()){
		m_candidates.swap(tmp);
	}
}

void CECShopSearchResult::Swap(CECShopSearchResult &rhs)
{
	std::swap(m_pShop, rhs.m_pShop);
	m_candidates.swap(rhs.m_candidates);
}

bool CECShopSearchResult::IsEmpty()const
{
	return m_candidates.empty();
}

int CECShopSearchResult::Count()const
{
	return (int)m_candidates.size();
}

int CECShopSearchResult::operator [](int index)const
{
	return m_candidates[index];
}

//	CECShopSearchIDPolicy
bool CECShopSearchIDPolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	return (int)pShop->GetItem(index)->id == m_id;
}

//	CECShopSearchValidPolicy
bool CECShopSearchValidPolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	//	无任何 price 合法时认为商品失效
	bool bValid(false);
	const GSHOP_ITEM &item = *pShop->GetItem(index);
	for(int i(0); i < sizeof(item.buy)/sizeof(item.buy[0]); ++i)
	{
		if (item.buy[i].price > 0){
			bValid = true;
			break;
		}
	}
	return bValid;
}

//	CECShopSearchLevelPolicy
bool CECShopSearchLevelPolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	return !CECQShopConfig::Instance().IsFilteredByLevel((int)pShop->GetItem(index)->id, m_level);
}

//	CECShopSearchTaskPolicy
extern CECGame * g_pGame;
CECShopSearchTaskPolicy::CECShopSearchTaskPolicy()
{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	m_pPack = pHost->GetPack();
	m_pTaskMan = g_pGame->GetTaskTemplateMan();
	m_pTaskInterface = pHost->GetTaskInterface();
	m_pDataMan = g_pGame->GetElementDataMan();
}

bool CECShopSearchTaskPolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	bool bAccept(false);
	while (true)
	{
		int id = pShop->GetItem(index)->id;
		if (!CECQShopConfig::Instance().CanFilterID(id)){
			//	不在受限之列
			bAccept = true;
			break;
		}
		if (CECQShopConfig::Instance().IsFilteredByID(id)){
			//	已经买过
			break;
		}
		if (m_pPack->FindItem(id) >= 0){
			//	包裹中已有
			break;
		}
		
		DATA_TYPE dt(DT_INVALID);
		const void *pData = m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
		if (dt != DT_TASKDICE_ESSENCE){
			//	只检查任务随机发生器相关
			bAccept = true;
			break;
		}

		bAccept = true;

		const TASKDICE_ESSENCE *pDice = static_cast<const TASKDICE_ESSENCE *>(pData);
		for (int i = 0;i < sizeof(pDice->task_lists)/sizeof(pDice->task_lists[0]); ++i)
		{
			unsigned long task = pDice->task_lists[i].id;
			ATaskTempl* pTempl = m_pTaskMan->GetTopTaskByID(task);
			if (pTempl && m_pTaskInterface->CanDeliverTask(task)){
				//	任务不能接取
				bAccept = false;
				break;
			}
		}
		break;
	}
	return bAccept;
}

//	CECShopSearchPricePolicy
bool CECShopSearchPricePolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	return pShop->CalcBuyIndex(index, NULL, m_money);
}

//	CECShopSearchPricePolicy
bool CECShopSearchPackSpacePolicy::CanAccept(const CECShopBase *pShop, int index)const
{	
	const GSHOP_ITEM &item = *pShop->GetItem(index);
	int needSpace(0);
	if (item.id > 0){
		++ needSpace;
	}
	if (item.idGift > 0){
		++ needSpace;
	}
	return needSpace <= m_emptySlot;
}

//	CECShopSearchCountPolicy
bool CECShopSearchCountPolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	//	至少会接受1个，因此，要求 m_maxCount >= 1 为正常值
	return true;
}

bool CECShopSearchCountPolicy::CanStop(int count)const
{
	//	m_maxCount 为非正值时，表示允许尽可能多的策略
	return m_maxCount > 0 && count >= m_maxCount;
}

//	CECShopSearchCompositePolicy
CECShopSearchCompositePolicy::~CECShopSearchCompositePolicy()
{
	delete m_pPolicy1;
	delete m_pPolicy2;
}

bool CECShopSearchCompositePolicy::CanAccept(const CECShopBase *pShop, int index)const
{
	return (!m_pPolicy1 || m_pPolicy1->CanAccept(pShop, index))
		&& (!m_pPolicy2 || m_pPolicy2->CanAccept(pShop, index));
}