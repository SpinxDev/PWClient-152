// Filename	: EC_ShopSearch.h
// Creator	: Xu Wenbin
// Date		: 2013/12/11

#pragma once

#include <vector.h>

struct _GSHOP_ITEM;
typedef struct _GSHOP_ITEM GSHOP_ITEM;

class CECShopBase;
class CECShopSearchPolicyBase;
class CECShopSearchResult;

//	使用给定算法搜索给定 CECShopBase
class CECShopSearch
{
	CECShopBase	* m_pShop;
	CECShopSearchPolicyBase	* m_pPolicy;

	CECShopSearch(const CECShopSearch &);				//	禁用
	CECShopSearch & operator=(const CECShopSearch &);	//	禁用
public:

	CECShopSearch(CECShopBase *p);
	~CECShopSearch();
	void SetShop(CECShopBase *p);
	void SetPolicy(CECShopSearchPolicyBase *policy);
	bool Search(CECShopSearchResult *result);
};

//	CECShopSearch搜索结果：存储和再过滤
class CECShopSearchResult
{
	typedef abase::vector<int> Candidates;

	CECShopBase	* m_pShop;
	Candidates	  m_candidates;

public:
	CECShopSearchResult(CECShopBase *p);

	bool Add(int index);
	void Filter(CECShopSearchPolicyBase &policy);	//	按给定策略再过滤
	void Swap(CECShopSearchResult &rhs);

	bool IsEmpty()const;
	int  Count()const;
	int  operator[](int index)const;
};

//	搜索算法基类
class CECShopSearchPolicyBase
{
public:
	virtual ~CECShopSearchPolicyBase(){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const=0;
	virtual bool CanStop(int count)const{ return false; }
};

//	检查数据有效性
class CECShopSearchValidPolicy : public CECShopSearchPolicyBase
{
public:
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查ID
class CECShopSearchIDPolicy : public CECShopSearchPolicyBase
{
	int		m_id;
public:
	CECShopSearchIDPolicy(int id):m_id(id){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查玩家等级配置
class CECShopSearchLevelPolicy : public CECShopSearchPolicyBase
{
	int		m_level;
public:
	CECShopSearchLevelPolicy(int playerLevel) : m_level(playerLevel) {}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查任务相关限制
class CECInventory;
class ATaskTemplMan;
class CECTaskInterface;
class elementdataman;
class CECShopSearchTaskPolicy : public CECShopSearchPolicyBase
{
	CECInventory		*m_pPack;
	ATaskTemplMan		*m_pTaskMan;
	CECTaskInterface	*m_pTaskInterface;
	elementdataman		*m_pDataMan;
public:
	CECShopSearchTaskPolicy();
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查价格
class CECShopSearchPricePolicy : public CECShopSearchPolicyBase
{
	int			m_money;
public:
	CECShopSearchPricePolicy(int money)	: m_money(money){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查包裹剩余数量
class CECShopSearchPackSpacePolicy : public CECShopSearchPolicyBase
{
	int			m_emptySlot;
public:
	CECShopSearchPackSpacePolicy(int emptySlot)	: m_emptySlot(emptySlot){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	检查搜索数目
class CECShopSearchCountPolicy : public CECShopSearchPolicyBase
{
	int			m_maxCount;	//	允许值 <=0，此时，表示不限制
public:
	CECShopSearchCountPolicy(int count)	: m_maxCount(count){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
	virtual bool CanStop(int count)const;
};

//	复合检查策略
class CECShopSearchCompositePolicy : public CECShopSearchPolicyBase
{
	CECShopSearchPolicyBase	*m_pPolicy1;
	CECShopSearchPolicyBase	*m_pPolicy2;

	CECShopSearchCompositePolicy(const CECShopSearchCompositePolicy &);				//	禁用
	CECShopSearchCompositePolicy & operator=(const CECShopSearchCompositePolicy &);	//	禁用
public:
	virtual ~CECShopSearchCompositePolicy();
	CECShopSearchCompositePolicy(CECShopSearchPolicyBase *p1, CECShopSearchPolicyBase *p2)
		: m_pPolicy1(p1), m_pPolicy2(p2) {}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};
