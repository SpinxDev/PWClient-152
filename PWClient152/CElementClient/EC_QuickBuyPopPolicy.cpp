// File		: EC_QuickBuyPopPolicy.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/19

#include "EC_QuickBuyPopPolicy.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "EC_UIHelper.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_RTDebug.h"

#include <AIniFile.h>
#include <AFI.h>

//	CECQuickBuyPopBuyItemPolicy
CECQuickBuyPopBuyItemPolicy::CECQuickBuyPopBuyItemPolicy(int recommendMessage, int recommendItem, bool needConfirm, bool needFlash)
: m_recommendMessage(recommendMessage)
, m_recommendItem(recommendItem)
, m_needConfirm(needConfirm)
, m_needFlash(needFlash)
{
}

CECQuickBuyPopBuyItemPolicy::~CECQuickBuyPopBuyItemPolicy()
{
}

CECShopBase * CECQuickBuyPopBuyItemPolicy::GetShopData()
{
	//	Ĭ��ʹ����ͨ�̳�
	return &CECQShop::Instance();
}

CECShopSearchPolicyBase * CECQuickBuyPopBuyItemPolicy::GetShopSearchPolicy(int itemID)
{
	//	Ĭ�ϲ���ӱ����ռ䡢�۸�ȼ�飬���ں���������ʾ
	CECShopSearchPolicyBase *p1 = new CECShopSearchIDPolicy(itemID);
	CECShopSearchPolicyBase *p2 = new CECShopSearchValidPolicy();
	CECShopSearchPolicyBase *p3 = new CECShopSearchLevelPolicy(CECUIHelper::GetHostPlayer()->GetMaxLevelSofar());
	CECShopSearchPolicyBase *p4 = new CECShopSearchTaskPolicy();
	return
		new CECShopSearchCompositePolicy(p1,
		new CECShopSearchCompositePolicy(p2,
		new CECShopSearchCompositePolicy(p3, p4)));
}

int CECQuickBuyPopBuyItemPolicy::GetItemID()const
{
	return m_recommendItem;
}

void CECQuickBuyPopBuyItemPolicy::GetItem(int &recommendMessage, int &recommendItem, bool &needConfirm, bool &needFlash)const
{
	recommendMessage = m_recommendMessage;
	recommendItem = m_recommendItem;
	needConfirm = m_needConfirm;
	needFlash = m_needFlash;
}

//	CECQuickBuyPopTriggerPolicy
CECQuickBuyPopTriggerPolicy::CECQuickBuyPopTriggerPolicy(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy)
: m_pBuyItemPolicy(pBuyItemPolicy)
, m_bTriggered(false)
, m_bItemGained(false)
, m_chkHideTime(0)
, m_dwNextTriggerTime(0)
, m_szHideReason(NULL)
{
}

void CECQuickBuyPopTriggerPolicy::Restart()
{
	m_bTriggered = false;
	m_bItemGained = false;
	RestartMore();
}

void CECQuickBuyPopTriggerPolicy::OnShow()
{
	m_bTriggered = true;
	m_bItemGained = false;
	m_dwNextTriggerTime = 0;
	OnShowMore();
}

void CECQuickBuyPopTriggerPolicy::OnHide()
{
	if (!GetHideReason()){
		SetHideReason("HR_Unregistered");
	}
	CECUIHelper::GetGame()->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Policy Hide Reason:%s"), AS2AC(GetHideReason()));
	m_bTriggered = false;
	m_bItemGained = false;
	OnHideMore();
	SetHideReason(NULL);
}

void CECQuickBuyPopTriggerPolicy::OnNoItem()
{
	m_bTriggered = false;
	m_bItemGained = false;
	m_dwNextTriggerTime = timeGetTime() + 60*1000;	//	60 ����л���������
	OnNoItemMore();
	SetHideReason("HR_NoItem");
}

void CECQuickBuyPopTriggerPolicy::OnNoPopDialog()
{
	m_bTriggered = false;
	m_bItemGained = false;
	m_dwNextTriggerTime = timeGetTime() + 10*1000;	//	10 ����л���������
	OnNoPopDialogMore();
	SetHideReason("HR_NoPopDialog");
}

bool CECQuickBuyPopTriggerPolicy::CanTrigger()const
{
	bool bCan(false);
	while (true)
	{
		if (m_bTriggered){
			//	��ǰ�Ѿ�����
			break;
		}
		if (m_chkHideTime){
			int now = CECUIHelper::GetGame()->GetServerGMTTime();
			if (CECUIHelper::IsOnSameDay(m_chkHideTime, now)){
				//	����ͬһ��
				break;
			}
		}
		if (m_dwNextTriggerTime > 0 && timeGetTime() < m_dwNextTriggerTime){
			//	��ȴʱ��δ��
			break;
		}
		if (CECUIHelper::GetHostPlayer()->GetPack()->FindItem(m_pBuyItemPolicy->GetItemID()) >= 0){
			//	���������д���Ʒ
			break;
		}
		bCan = CanTriggerCheckMore();
		break;
	}
	return bCan;
}

bool CECQuickBuyPopTriggerPolicy::CanHide(const char *&szHideReason)const
{
	bool bHide(false);
	while (true)
	{
		if (!m_bTriggered){
			//	����״̬���رպ����������
			bHide = true;
			szHideReason = "HR_Ignore";
			break;
		}
		if (m_bItemGained){
			//	��Ʒ�ѻ��
			bHide = true;
			szHideReason = "HR_ItemGained";
			break;
		}
		if (m_chkHideTime){
			int now = CECUIHelper::GetGame()->GetServerGMTTime();
			if (m_chkHideTime >= now){
				//	ʱ��������ò��ٵ�����ʱ�̡��ȵ�ǰʱ�̻�Ҫ��
				break;
			}
			if (CECUIHelper::IsOnSameDay(m_chkHideTime, now)){
				//	�����첻�ٵ�����
				bHide = true;
				szHideReason = "HR_OnSameDay";
				break;
			}
		}//else û�С����첻�ٵ���������
		bHide = CanHideMore(szHideReason);
		break;
	}
	return bHide;
}

void CECQuickBuyPopTriggerPolicy::OnCheckHide()
{
	if (NeedCheckHide()){
		m_chkHideTime = CECUIHelper::GetGame()->GetServerGMTTime();
	}
}

void CECQuickBuyPopTriggerPolicy::OnItemAppearInPack(int itemID, int iSlot)
{
	if (m_bTriggered){		
		CECIvtrItem *pItem = CECUIHelper::GetHostPlayer()->GetPack()->GetItem(iSlot);
		if (pItem && pItem->GetTemplateID() == m_pBuyItemPolicy->GetItemID()){
			m_bItemGained = true;
			OnItemAppearInPackMore();
		}
	}
}

void CECQuickBuyPopTriggerPolicy::SetHideReason(const char *szReason)
{
	if (!szReason || stricmp(szReason, "HR_Ignore")){	//	"HR_Ignore" ʱ�������ط���������ֵ
		m_szHideReason = const_cast<char *>(szReason);
	}
}

void CECQuickBuyPopTriggerPolicy::SaveConfig(AIniFile &file)
{
	if (NeedCheckHide()){
		file.WriteIntValue(GetConfigName(), "last_chk_hide_time", m_chkHideTime);
	}
	SaveConfigMore(file);
}

void CECQuickBuyPopTriggerPolicy::LoadConfig(AIniFile &file)
{
	if (NeedCheckHide()){
		m_chkHideTime = file.GetValueAsInt(GetConfigName(), "last_chk_hide_time", m_chkHideTime);
	}
	LoadConfigMore(file);
}

//	CECQuickBuyPopPolicies
const char * CECQuickBuyPopPolicies::GetConfigFileDir()
{
	return "userdata\\quickbuy";
}

AString CECQuickBuyPopPolicies::GetConfigFilePath(int playerID)
{
	AString strFile;
	strFile.Format("%s\\%d.ini", GetConfigFileDir(), playerID);
	return strFile;
}

void CECQuickBuyPopPolicies::SaveConfigs()
{
	if (IsValid()){
		CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();
		if (pHost){
			AIniFile file;
			AString strFile = GetConfigFilePath(pHost->GetCharacterID());
			if (af_IsFileExist(strFile)){
				//	�������ļ����Լ̳���������ѡ��
				file.Open(strFile);
			}
			pTriggerPolicy->SaveConfig(file);
			CreateDirectoryA(GetConfigFileDir(), NULL);
			file.Save(strFile);
			file.Close();
		}
	}
}

void CECQuickBuyPopPolicies::LoadConfigs()
{
	if (IsValid()){
		CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();
		if (pHost){
			AIniFile file;
			if (file.Open(GetConfigFilePath(pHost->GetCharacterID()))){
				pTriggerPolicy->LoadConfig(file);
				file.Close();
			}
		}
	}
}
