// File		: EC_QuickBuyPopMan.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#include "EC_QuickBuyPopMan.h"
#include "DlgQuickBuyPop.h"
#include "EC_UIConfigs.h"

#include "EC_UIHelper.h"
#include "EC_Game.h"
#include "EC_RTDebug.h"

#include <algorithm>

//CECQuickBuyPopManager
CECQuickBuyPopManager::CECQuickBuyPopManager()
{
}

CECQuickBuyPopManager & CECQuickBuyPopManager::Instance()
{
	static CECQuickBuyPopManager s_instance;
	return s_instance;
}

void CECQuickBuyPopManager::AddPop(CDlgQuickBuyPop *pDlg)
{
	if (pDlg && !pDlg->GetPolicy()
		&& std::find(m_freeList.begin(), m_freeList.end(), pDlg) == m_freeList.end()
		&& std::find(m_usedList.begin(), m_usedList.end(), pDlg) == m_usedList.end()){
		m_freeList.push_back(pDlg);
	}else{
		assert(false);
	}
}

void CECQuickBuyPopManager::RemovePop(CDlgQuickBuyPop *pDlg)
{
	while (pDlg){
		Pops::iterator it = std::find(m_freeList.begin(), m_freeList.end(), pDlg);
		if (it != m_freeList.end()){
			//	��δʹ�ã�ֱ��ɾ������
			m_freeList.erase(it);
			break;
		}

		it = std::find(m_usedList.begin(), m_usedList.end(), pDlg);
		if (it == m_usedList.end()){
			//	���ڹ���֮��
			assert(false);
			break;
		}
		//	��ʹ��

		//	ɾ������
		const CECQuickBuyPopPolicies *policy = pDlg->GetPolicy();
		if (!policy){
			assert(false);
			break;
		}
		if (!Unbind(policy->GetID())){
			assert(false);
			break;
		}
		//	�Ѿ��ͷŵ������б�
		it = std::find(m_freeList.begin(), m_freeList.end(), pDlg);
		if (it == m_freeList.end()){
			assert(false);
			break;
		}
		m_freeList.erase(it);
		break;
	}
}

void CECQuickBuyPopManager::RegisterPolicy(const CECQuickBuyPopPolicies &policy)
{
	if (!policy.IsValid() || m_configs.find(policy.GetID()) != m_configs.end()){
		assert(false);
		return;
	}
	PolicyPair & pp = (m_configs[policy.GetID()] = PolicyPair(policy, NULL));
	pp.first.Restart();	//	ע�ἴ����
}

void CECQuickBuyPopManager::UnregisterPolicy(QuickBuyPopConfigID id)
{
	Configs::iterator it = m_configs.find(id);
	if (it == m_configs.end()){
		assert(false);
		return;
	}
	PolicyPair &pp = it->second;
	if (pp.second != NULL){
		Unbind(id);
	}
	pp.first.Release();
	m_configs.erase(it);
}

bool CECQuickBuyPopManager::Bind(QuickBuyPopConfigID id)
{
	//	Ϊ��ǰ����������ʵĽ��桢���󶨵�һ��
	bool bSuccess(false);
	while (true)
	{
		Configs::iterator it = m_configs.find(id);
		if (it == m_configs.end()){
			assert(false);
			break;
		}
		PolicyPair &pp = it->second;
		if (pp.second != NULL){
			//	�Ƚ�������н���İ�
			assert(false);
			break;
		}
		CECQuickBuyPopPolicies &policy = pp.first;
		CDlgQuickBuyPop *pDlg = AllocatePop(policy.NeedCheckHide());
		if (!pDlg){
			//	û�����뵽���ʽ���
			policy.OnNoPopDialog();
			break;
		}
		if (pDlg->GetPolicy()){
			//	�����д����в��Էǿա��Ƚ�������в��Եİ�
			assert(false);
			FreePop(pDlg);
			break;
		}
		CECUIHelper::GetGame()->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Bind Policy(%d)"), id);
		pp.second = pDlg;
		pDlg->BindPolicy(pp.first);
		bSuccess = true;
		break;
	}
	return bSuccess;
}

bool CECQuickBuyPopManager::Unbind(QuickBuyPopConfigID id)
{
	bool bSuccess(false);
	while (true)
	{
		Configs::iterator it = m_configs.find(id);
		if (it == m_configs.end()){
			assert(false);
			break;
		}
		PolicyPair &pp = it->second;
		CDlgQuickBuyPop *pDlg = pp.second;
		CECQuickBuyPopPolicies *policy = pDlg->GetPolicy();
		if (!policy || policy->GetID() != id){
			assert(false);
			break;
		}
		CECUIHelper::GetGame()->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Unbind Policy(%d)"), id);
		//	ɾ������
		pp.second = NULL;
		pDlg->BindPolicy(CECQuickBuyPopPolicies());

		//	�ͷŽ��桢��������
		FreePop(pDlg);
		pp.first.Restart();
		bSuccess = true;
		break;
	}
	return bSuccess;
}

CDlgQuickBuyPop *CECQuickBuyPopManager::AllocatePop(bool needCheckHide)
{
	//	��ȡ�������ڰ�
	CDlgQuickBuyPop *pDlg = NULL;
	for (Pops::iterator it = m_freeList.begin(); it != m_freeList.end(); ++ it)
	{
		CDlgQuickBuyPop *pCur = *it;
		if (needCheckHide == pCur->HasCheckHide()){
			pDlg = pCur;
			m_usedList.push_back(pCur);
			m_freeList.erase(it);
			break;
		}
	}
	return pDlg;
}

void CECQuickBuyPopManager::FreePop(CDlgQuickBuyPop *pDlg)
{
	//	�ָ������ɽ��������
	Pops::iterator it = std::find(m_usedList.begin(), m_usedList.end(), pDlg);
	if (it != m_usedList.end()){
		m_usedList.erase(it);
	}
	it = std::find(m_freeList.begin(), m_freeList.end(), pDlg);
	if (it == m_freeList.end()){
		m_freeList.push_back(pDlg);
	}
}

void CECQuickBuyPopManager::Tick()
{
	if (!CECUIConfig::Instance().GetGameUI().bEnableRecommendQShopItem
		|| m_configs.empty()
		|| m_freeList.empty() && m_usedList.empty()){
		return;
	}
	Configs::iterator it;
	//	����ѹ������á��Ƿ���Ҫ���
	for (it = m_configs.begin(); it != m_configs.end(); ++ it)
	{
		PolicyPair &pp = it->second;
		CECQuickBuyPopPolicies &policy = pp.first;
		CDlgQuickBuyPop *pDlg = pp.second;
		if (!pDlg){
			continue;
		}
		const char *szHideReason = NULL;
		if (policy.CanHide(szHideReason)){
			policy.SetHideReason(szHideReason);
			Unbind(policy.GetID());
		}
	}
	//	���δ�������á��Ƿ���Ҫ����
	for (it = m_configs.begin(); it != m_configs.end(); ++ it)
	{
		PolicyPair &pp = it->second;
		if (pp.second){
			continue;
		}
		CECQuickBuyPopPolicies &policy = pp.first;
		if (policy.CanTrigger()){
			Bind(policy.GetID());
		}
	}
}

void CECQuickBuyPopManager::Reset()
{
	//	�����������в��ԣ��л�������ʱ��Ҫ��
	for (Configs::iterator it = m_configs.begin(); it != m_configs.end(); ++ it)
	{
		PolicyPair &pp = it->second;
		CECQuickBuyPopPolicies &policy = pp.first;

		//	����󶨡���������
		CDlgQuickBuyPop *pDlg = pp.second;
		if (pDlg){
			policy.SetHideReason("HR_Reset");
			Unbind(policy.GetID());
		}else{
			policy.Restart();
		}
	}
}

void CECQuickBuyPopManager::ClearPops()
{
	while (!m_freeList.empty())
	{
		RemovePop(m_freeList.front());
	}
	while (!m_usedList.empty())
	{
		RemovePop(m_usedList.front());
	}
}

void CECQuickBuyPopManager::ClearPolicies()
{
	while (!m_configs.empty())
	{
		Configs::iterator it = m_configs.begin();
		UnregisterPolicy(it->first);
	}
}