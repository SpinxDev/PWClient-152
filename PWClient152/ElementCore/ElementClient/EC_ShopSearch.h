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

//	ʹ�ø����㷨�������� CECShopBase
class CECShopSearch
{
	CECShopBase	* m_pShop;
	CECShopSearchPolicyBase	* m_pPolicy;

	CECShopSearch(const CECShopSearch &);				//	����
	CECShopSearch & operator=(const CECShopSearch &);	//	����
public:

	CECShopSearch(CECShopBase *p);
	~CECShopSearch();
	void SetShop(CECShopBase *p);
	void SetPolicy(CECShopSearchPolicyBase *policy);
	bool Search(CECShopSearchResult *result);
};

//	CECShopSearch����������洢���ٹ���
class CECShopSearchResult
{
	typedef abase::vector<int> Candidates;

	CECShopBase	* m_pShop;
	Candidates	  m_candidates;

public:
	CECShopSearchResult(CECShopBase *p);

	bool Add(int index);
	void Filter(CECShopSearchPolicyBase &policy);	//	�����������ٹ���
	void Swap(CECShopSearchResult &rhs);

	bool IsEmpty()const;
	int  Count()const;
	int  operator[](int index)const;
};

//	�����㷨����
class CECShopSearchPolicyBase
{
public:
	virtual ~CECShopSearchPolicyBase(){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const=0;
	virtual bool CanStop(int count)const{ return false; }
};

//	���������Ч��
class CECShopSearchValidPolicy : public CECShopSearchPolicyBase
{
public:
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	���ID
class CECShopSearchIDPolicy : public CECShopSearchPolicyBase
{
	int		m_id;
public:
	CECShopSearchIDPolicy(int id):m_id(id){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	�����ҵȼ�����
class CECShopSearchLevelPolicy : public CECShopSearchPolicyBase
{
	int		m_level;
public:
	CECShopSearchLevelPolicy(int playerLevel) : m_level(playerLevel) {}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	��������������
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

//	���۸�
class CECShopSearchPricePolicy : public CECShopSearchPolicyBase
{
	int			m_money;
public:
	CECShopSearchPricePolicy(int money)	: m_money(money){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	������ʣ������
class CECShopSearchPackSpacePolicy : public CECShopSearchPolicyBase
{
	int			m_emptySlot;
public:
	CECShopSearchPackSpacePolicy(int emptySlot)	: m_emptySlot(emptySlot){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};

//	���������Ŀ
class CECShopSearchCountPolicy : public CECShopSearchPolicyBase
{
	int			m_maxCount;	//	����ֵ <=0����ʱ����ʾ������
public:
	CECShopSearchCountPolicy(int count)	: m_maxCount(count){}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
	virtual bool CanStop(int count)const;
};

//	���ϼ�����
class CECShopSearchCompositePolicy : public CECShopSearchPolicyBase
{
	CECShopSearchPolicyBase	*m_pPolicy1;
	CECShopSearchPolicyBase	*m_pPolicy2;

	CECShopSearchCompositePolicy(const CECShopSearchCompositePolicy &);				//	����
	CECShopSearchCompositePolicy & operator=(const CECShopSearchCompositePolicy &);	//	����
public:
	virtual ~CECShopSearchCompositePolicy();
	CECShopSearchCompositePolicy(CECShopSearchPolicyBase *p1, CECShopSearchPolicyBase *p2)
		: m_pPolicy1(p1), m_pPolicy2(p2) {}
	virtual bool CanAccept(const CECShopBase *pShop, int index)const;
};
