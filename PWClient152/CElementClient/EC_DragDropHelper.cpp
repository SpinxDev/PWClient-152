// Filename	: EC_DragDropHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/8

#include "EC_DragDropHelper.h"
#include "EC_LogicHelper.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrFashion.h"
#include "EC_GameSession.h"

#include "EC_UIHelper.h"
#include "EC_GameUIMan.h"

#include <AUIDialog.h>

//	local functions
static CECIvtrItem * GetPackItem(int iPack, int iSlot){
	CECHostPlayer *pHost = CECLogicHelper::GetHostPlayer();
	if (!pHost){
		return NULL;
	}
	CECInventory *pStorage = pHost->GetPack(iPack);
	if (!pStorage){
		return NULL;
	}
	return pStorage->GetItem(iSlot);
}
static int GetFashionStoragePackIndex(){
	return IVTRTYPE_TRASHBOX3;
}
static int GetEquipPackIndex(){
	return IVTRTYPE_EQUIPPACK;
}
static int GetNormalPackIndex(){
	return IVTRTYPE_PACK;
}
static CECIvtrFashion *GetFashionStorageFashion(int iSlot){
	return dynamic_cast<CECIvtrFashion *>(GetPackItem(GetFashionStoragePackIndex(), iSlot));
}
static CECIvtrFashion *GetNormalPackFashion(int iSlot){
	return dynamic_cast<CECIvtrFashion *>(GetPackItem(GetNormalPackIndex(), iSlot));
}
static CECIvtrFashion *  GetEquipPackFashion(int iSlot){
	return dynamic_cast<CECIvtrFashion *>(GetPackItem(GetEquipPackIndex(), iSlot));
}

//	struct CECHostFashionComponent
CECHostFashionComponent::CECHostFashionComponent()
: m_iPack(-1)
, m_iSlot(-1)
, m_nItemID(0)
, m_wItemColor(0)
, m_nItemExpireTime(0)
{
}

CECHostFashionComponent::CECHostFashionComponent(int iPack, int iSlot)
{
	CECHostFashionComponent();
	Init(iPack, iSlot);
}

bool CECHostFashionComponent::HasItem()const{
	return m_nItemID != 0;
}

void CECHostFashionComponent::Init(int iPack, int iSlot){
	CECInventory *pPack = CECLogicHelper::GetHostPlayer()->GetPack(iPack);
	if (!pPack || iSlot < 0 || iSlot >= pPack->GetSize()){
		ASSERT(false);
		return;
	}
	m_iPack = iPack;
	m_iSlot = iSlot;
	if (CECIvtrFashion *pIvtrFashion = dynamic_cast<CECIvtrFashion *>(pPack->GetItem(iSlot))){
		m_nItemID = pIvtrFashion->GetTemplateID();
		m_wItemColor = pIvtrFashion->GetWordColor();
		m_nItemExpireTime = pIvtrFashion->GetExpireDate();
	}else{
		m_nItemID = 0;
		m_wItemColor = 0;
		m_nItemExpireTime = 0;
	}
}

bool CECHostFashionComponent::SameWith(CECIvtrFashion *pIvtrFashion)const{
	return pIvtrFashion != NULL
		&& pIvtrFashion->GetTemplateID() == m_nItemID
		&& pIvtrFashion->GetWordColor() == m_wItemColor
		&& pIvtrFashion->GetExpireDate() == m_nItemExpireTime;
}

bool CECHostFashionComponent::Validate()const{
	return SameWith(dynamic_cast<CECIvtrFashion *>(GetPackItem(m_iPack, m_iSlot)));
}

int CECHostFashionComponent::FindInNormalPack()const{
	if (!HasItem()){
		return -1;
	}
	CECHostPlayer *pHost = CECLogicHelper::GetHostPlayer();
	if (!pHost){
		return -1;
	}
	CECInventory *pNormalPack = pHost->GetPack();
	if (!pNormalPack){
		return -1;
	}
	for (int i(0); i < pNormalPack->GetSize(); ++ i){
		CECIvtrItem *pItem = pNormalPack->GetItem(i);
		if (!pItem || !pItem->IsEquipment()){
			continue;
		}
		CECIvtrEquip *pEquip = dynamic_cast<CECIvtrEquip*>(pItem);
		if (!pEquip->IsFashion()){
			continue;
		}
		CECIvtrFashion *pFashion = dynamic_cast<CECIvtrFashion *>(pEquip);
		if (SameWith(pFashion)){
			return i;
		}
	}
	return -1;
}

//	class CECHostFashionEquipFromStorageComponent
CECHostFashionEquipFromStorageComponent::CECHostFashionEquipFromStorageComponent()
: m_state(STATE_INVALID)
{
}

bool CECHostFashionEquipFromStorageComponent::SetComponentData(int iSrcStorageSlot, int iDstEquipSlot){
	if (m_state != STATE_INVALID){
		return false;
	}
	CECIvtrFashion *pSrcFashion = GetFashionStorageFashion(iSrcStorageSlot);
	if (!pSrcFashion){
		return false;
	}
	if (!pSrcFashion->CanEquippedTo(iDstEquipSlot)){
		return false;
	}
	if (!CECLogicHelper::GetHostPlayer()->CanUseEquipment(pSrcFashion)){
		return false;
	}
	if (CECIvtrFashion *pDstFashion = GetEquipPackFashion(iDstEquipSlot)){
		if (CECHostFashionComponent(GetFashionStoragePackIndex(), iSrcStorageSlot).SameWith(pDstFashion)){
			return false;	//	ʱװ��ͬ������װ����Ҳ�����������߼����������鷳
		}
	}
	m_fashionPackComponent.Init(GetFashionStoragePackIndex(), iSrcStorageSlot);
	m_equipPackComponent.Init(GetEquipPackIndex(), iDstEquipSlot);
	m_state = STATE_IN_STORAGE;
	return true;
}

bool CECHostFashionEquipFromStorageComponent::IsSame(int iSrcStorageSlot, int iDstEquipSlot)const{
	return iSrcStorageSlot == m_fashionPackComponent.m_iSlot
		&& iDstEquipSlot == m_equipPackComponent.m_iSlot;
}

//	class CECHostFashionEquipFromStorageSystem
ELEMENTCLIENT_DEFINE_SINGLETON(CECHostFashionEquipFromStorageSystem);

CECHostFashionEquipFromStorageSystem::iterator CECHostFashionEquipFromStorageSystem::ComponentBegin(){
	return m_components.begin();
}

CECHostFashionEquipFromStorageSystem::iterator CECHostFashionEquipFromStorageSystem::ComponentEnd(){
	return m_components.end();
}

CECHostFashionEquipFromStorageSystem::iterator CECHostFashionEquipFromStorageSystem::FindComponent(int iSrcStorageSlot, int iDstEquipSlot){
	iterator result = ComponentBegin();
	while (result != ComponentEnd()){
		CECHostFashionEquipFromStorageComponent &c = *result;
		if (c.IsSame(iSrcStorageSlot, iDstEquipSlot)){
			break;
		}else{
			++ result;
		}
	}
	return result;
}

bool CECHostFashionEquipFromStorageSystem::AddComponent(int iSrcStorageSlot, int iDstEquipSlot){
	if (FindComponent(iSrcStorageSlot, iDstEquipSlot) != ComponentEnd()){
		return false;
	}
	CECHostFashionEquipFromStorageComponent c;
	if (!c.SetComponentData(iSrcStorageSlot, iDstEquipSlot)){
		return false;
	}
	m_components.push_back(c);
	return true;
}

void CECHostFashionEquipFromStorageSystem::Tick(){
	iterator it = ComponentBegin();
	while (it != ComponentEnd()){
		CECHostFashionEquipFromStorageComponent &c = *it;
		switch (c.m_state){
		case CECHostFashionEquipFromStorageComponent::STATE_IN_STORAGE:
			//	����ǰ����ͨ����
			if (!c.m_fashionPackComponent.Validate()){
				c.m_state = CECHostFashionEquipFromStorageComponent::STATE_INVALID;	//	ʱװ��ʧЧ
			}else{
				int iEmptySlot = CECLogicHelper::GetHostPlayer()->GetPack()->SearchEmpty();
				if (iEmptySlot < 0){
					c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_PACK;
				}else{
					CECLogicHelper::GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(GetFashionStoragePackIndex(), c.m_fashionPackComponent.m_iSlot, iEmptySlot);
					c.m_state = CECHostFashionEquipFromStorageComponent::STATE_WAIT_TO_PACK;
					c.m_stateTimer.Reset(1000);
				}
			}
			break;
		case CECHostFashionEquipFromStorageComponent::STATE_WAIT_TO_PACK:
			//	����Ƿ��ѵ��˰���
			{
				int iTempNormalPackSlot = c.m_fashionPackComponent.FindInNormalPack();
				if (iTempNormalPackSlot >= 0){
					//	�Ѿ�������ͨ����������ǰ��װ������
					CECIvtrFashion *pFashion = GetNormalPackFashion(iTempNormalPackSlot);
					CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
					if(pFashion->GetProcType() & CECIvtrItem::PROC_BIND){
						PAUIDIALOG pMsgBox;
						pGameUIMan->MessageBox("Game_EquipBind1", pGameUIMan->GetStringFromTable(872), MB_YESNO, pGameUIMan->GetMsgBoxColor(), &pMsgBox);
						pMsgBox->SetData(iTempNormalPackSlot);
						pMsgBox->SetDataPtr((void*)c.m_equipPackComponent.m_iSlot);
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_SEND_TO_EQUIP_NEEDCONFIRM;
					}else{
						CECLogicHelper::GetGameSession()->c2s_CmdEquipItem(iTempNormalPackSlot, c.m_equipPackComponent.m_iSlot);
						pGameUIMan->PlayItemSound(pFashion, true);
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_SENT_TO_EQUIP;
						c.m_stateTimer.Reset(1000);
					}
				}else{
					if (c.m_stateTimer.IsTimeArrived()){
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_PACK;
					}
				}
			}
			break;
		case CECHostFashionEquipFromStorageComponent::STATE_SEND_TO_EQUIP_NEEDCONFIRM:
			//	�ȴ�����ȷ��װ����Ϣ
			if (!CECUIHelper::GetGameUIMan()->GetDialog("Game_EquipBind1")){
				c.m_state = CECHostFashionEquipFromStorageComponent::STATE_SENT_TO_EQUIP;
				c.m_stateTimer.Reset(1000);
			}
			break;
		case CECHostFashionEquipFromStorageComponent::STATE_SENT_TO_EQUIP:
			//	�ȴ��ɹ�װ����Ϣ
			{
				CECIvtrFashion *pFashion = GetEquipPackFashion(c.m_equipPackComponent.m_iSlot);
				if (c.m_fashionPackComponent.SameWith(pFashion)){
					//	�Ѿ��ɹ�װ��
					if (c.m_equipPackComponent.HasItem()){
						//	����Ҫ��ж����������Ʒ���ŵ�ʱװ����
						int iTempNormalPackSlot = c.m_equipPackComponent.FindInNormalPack();
						if (iTempNormalPackSlot < 0){
							c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_STORAGE;		//	��ͨ������û�ҵ������Ķ�ȥ�ˣ�
						}else{
							if (GetFashionStorageFashion(c.m_fashionPackComponent.m_iSlot)){
								c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_STORAGE;	//	ʱװ����ԭλ���ѱ�ռ�ã�Ϊʲô��
							}else{
								CECLogicHelper::GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(GetFashionStoragePackIndex(), c.m_fashionPackComponent.m_iSlot, iTempNormalPackSlot);
								c.m_state = CECHostFashionEquipFromStorageComponent::STATE_WAIT_EXCHANGE_TO_STORAGE;
								c.m_stateTimer.Reset(1000);
							}
						}
					}else{
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_EXCHANGE_FINISHED;
					}
				}else{
					if (c.m_stateTimer.IsTimeArrived()){
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_EQUIP;
					}
				}
			}
			break;
		case CECHostFashionEquipFromStorageComponent::STATE_WAIT_EXCHANGE_TO_STORAGE:
			//	����滻�µ�ʱװ�Ƿ��ѵ���ʱװ����
			{
				if (CECIvtrFashion *pFashion = GetFashionStorageFashion(c.m_fashionPackComponent.m_iSlot)){
					if (c.m_equipPackComponent.SameWith(pFashion)){
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_EXCHANGE_FINISHED;
					}else{
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_STORAGE;
					}
				}else{
					if (c.m_stateTimer.IsTimeArrived()){
						c.m_state = CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_STORAGE;
					}
				}
			}
			break;
		}

		switch (c.m_state){
			//	�ɹ�/ʧ���������ʱ��ɾ��
		case CECHostFashionEquipFromStorageComponent::STATE_INVALID:
		case CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_PACK:
		case CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_EQUIP:
		case CECHostFashionEquipFromStorageComponent::STATE_FAIL_TO_STORAGE:
		case CECHostFashionEquipFromStorageComponent::STATE_EXCHANGE_FINISHED:
			it = m_components.erase(it);
			break;
		default:
			//	���������������ע
			++ it;
		}
	}
}

void CECHostFashionEquipFromStorageSystem::Clear(){
	m_components.clear();
}