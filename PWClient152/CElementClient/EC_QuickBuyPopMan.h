// File		: EC_QuickBuyPopMan.h
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#pragma once

#include "EC_QuickBuyPopPolicy.h"

#include <hashmap.h>

//	���е���ʽ��ݹ���Ĺ���
class CDlgQuickBuyPop;
class CECQuickBuyPopManager
{
	//	���еĵ���ʽ�����ɿ��С�ʹ���������б����
	typedef abase::vector<CDlgQuickBuyPop *> Pops;
	Pops		m_freeList;
	Pops		m_usedList;

	//	�����������úͶԻ���Ĺ���
	typedef abase::pair<CECQuickBuyPopPolicies, CDlgQuickBuyPop *> PolicyPair;

	//	��������ͳһ����
	typedef abase::hash_map<QuickBuyPopConfigID, PolicyPair> Configs;
	Configs		m_configs;
	
	CECQuickBuyPopManager();

	bool Bind(QuickBuyPopConfigID id);
	bool Unbind(QuickBuyPopConfigID id);

	CDlgQuickBuyPop *AllocatePop(bool needCheckHide);
	void FreePop(CDlgQuickBuyPop *pDlg);
	
	CECQuickBuyPopManager(const CECQuickBuyPopManager &);				//	����
	CECQuickBuyPopManager & operator=(const CECQuickBuyPopManager &);	//	����
	
public:
	void AddPop(CDlgQuickBuyPop *pDlg);
	void RemovePop(CDlgQuickBuyPop *pDlg);
	
	void RegisterPolicy(const CECQuickBuyPopPolicies &policy);
	void UnregisterPolicy(QuickBuyPopConfigID id);

	void Reset();
	void ClearPops();
	void ClearPolicies();

	void Tick();
	
	static CECQuickBuyPopManager & Instance();
};
