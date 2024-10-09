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
	CECHostFashionComponent	m_fashionPackComponent;	//	时装包裹时装
	CECHostFashionComponent	m_equipPackComponent;	//	装备包裹时装

	CECTimeSafeChecker	m_stateTimer;	//	等待计时和状态切换

	enum State{STATE_INVALID,			//	数据非法
		STATE_IN_STORAGE,				//	要装备的时装还在时装包裹
		STATE_WAIT_TO_PACK,				//	已经发送切换到普通包裹消息，等待结果
		STATE_FAIL_TO_PACK,				//	前往普通包裹失败
		STATE_SEND_TO_EQUIP_NEEDCONFIRM,//	成功到达在普通包裹，已弹出装备后绑定提示等待确认
		STATE_SENT_TO_EQUIP,			//	成功到达普通包裹，非装备后绑定时装已经发送装备消息
		STATE_FAIL_TO_EQUIP,			//	前往装备包裹失败
		STATE_WAIT_EXCHANGE_TO_STORAGE,	//	已发送将替换下来的时装换到时装包裹的消息
		STATE_EXCHANGE_FINISHED,		//	替换下来的时装成功到时装包裹、或原装备位置为空
		STATE_FAIL_TO_STORAGE,			//	替换下来的时装没能到时装包裹
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
