// Filename	: EC_DragDropHelper.h
// Creator	: Xu Wenbin
// Date		: 2015/1/8

#pragma once

#include <ABaseDef.h>
#include <vector.h>

#include "EC_Global.h"
#include "EC_TimeSafeChecker.h"

class CECIvtrFashion;

struct CECHostFashionComponent{
	int		m_iPack;
	int		m_iSlot;
	int		m_nItemID;
	WORD	m_wItemColor;
	int		m_nItemExpireTime;

	CECHostFashionComponent();
	CECHostFashionComponent(int iPack, int iSlot);

	void Init(int iPack, int iSlot);
	bool HasItem()const;
	bool SameWith(CECIvtrFashion *pFashionItem)const;
	bool Validate()const;
	int  FindInNormalPack()const;
};

struct CECHostFashionEquipFromStorageComponent
{
	CECHostFashionComponent	m_fashionPackComponent;	//	ʱװ����ʱװ
	CECHostFashionComponent	m_equipPackComponent;	//	װ������ʱװ

	CECTimeSafeChecker	m_stateTimer;	//	�ȴ���ʱ��״̬�л�

	enum State{STATE_INVALID,			//	���ݷǷ�
		STATE_IN_STORAGE,				//	Ҫװ����ʱװ����ʱװ����
		STATE_WAIT_TO_PACK,				//	�Ѿ������л�����ͨ������Ϣ���ȴ����
		STATE_FAIL_TO_PACK,				//	ǰ����ͨ����ʧ��
		STATE_SEND_TO_EQUIP_NEEDCONFIRM,//	�ɹ���������ͨ�������ѵ���װ�������ʾ�ȴ�ȷ��
		STATE_SENT_TO_EQUIP,			//	�ɹ�������ͨ��������װ�����ʱװ�Ѿ�����װ����Ϣ
		STATE_FAIL_TO_EQUIP,			//	ǰ��װ������ʧ��
		STATE_WAIT_EXCHANGE_TO_STORAGE,	//	�ѷ��ͽ��滻������ʱװ����ʱװ��������Ϣ
		STATE_EXCHANGE_FINISHED,		//	�滻������ʱװ�ɹ���ʱװ��������ԭװ��λ��Ϊ��
		STATE_FAIL_TO_STORAGE,			//	�滻������ʱװû�ܵ�ʱװ����
	};
	State	m_state;

	CECHostFashionEquipFromStorageComponent();
	bool SetComponentData(int iSrcStorageSlot, int iDstEquipSlot);
	bool IsSame(int iSrcStorageSlot, int iDstEquipSlot)const;
};

class CECHostFashionEquipFromStorageSystem{
	typedef abase::vector<CECHostFashionEquipFromStorageComponent>	Components;
	typedef Components::iterator iterator;
	Components	m_components;

	iterator ComponentBegin();
	iterator ComponentEnd();
	iterator FindComponent(int iSrcStorageSlot, int iDstEquipSlot);

	ELEMENTCLIENT_DECLARE_SINGLETON(CECHostFashionEquipFromStorageSystem);

public:
	bool AddComponent(int iSrcStorageSlot, int iDstEquipSlot);
	void Tick();
	void Clear();
};
