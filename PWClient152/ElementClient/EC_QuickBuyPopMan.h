// File		: EC_QuickBuyPopMan.h
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#pragma once

#include "EC_QuickBuyPopPolicy.h"

#include <hashmap.h>

//	所有弹出式快捷购买的管理
class CDlgQuickBuyPop;
class CECQuickBuyPopManager
{
	//	所有的弹出式界面由空闲、使用中两个列表管理
	typedef abase::vector<CDlgQuickBuyPop *> Pops;
	Pops		m_freeList;
	Pops		m_usedList;

	//	建立弹出配置和对话框的关联
	typedef abase::pair<CECQuickBuyPopPolicies, CDlgQuickBuyPop *> PolicyPair;

	//	所有配置统一管理
	typedef abase::hash_map<QuickBuyPopConfigID, PolicyPair> Configs;
	Configs		m_configs;
	
	CECQuickBuyPopManager();

	bool Bind(QuickBuyPopConfigID id);
	bool Unbind(QuickBuyPopConfigID id);

	CDlgQuickBuyPop *AllocatePop(bool needCheckHide);
	void FreePop(CDlgQuickBuyPop *pDlg);
	
	CECQuickBuyPopManager(const CECQuickBuyPopManager &);				//	禁用
	CECQuickBuyPopManager & operator=(const CECQuickBuyPopManager &);	//	禁用
	
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
