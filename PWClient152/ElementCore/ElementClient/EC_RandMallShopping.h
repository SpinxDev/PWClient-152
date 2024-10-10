// File		: EC_RandMallShopping.h
// Creator	: Xu Wenbin
// Date		: 2014/2/25

#pragma once

#include <vector.h>
#include <ABaseDef.h>

//	ǰ������
namespace S2C
{
	struct cmd_rand_mall_shopping_res;
}
struct RAND_SHOP_CONFIG;

//	��״̬��ö������
enum enumRandMallShoppingState
{
	RMSS_NONE,
	RMSS_NO_CONFIG,
	RMSS_TO_QUERY,
	RMSS_IN_QUERY,
	RMSS_TO_ROLL,
	RMSS_IN_ROLL,
	RMSS_TO_PAY,
	RMSS_IN_PAY,
	RMSS_PAYED,
};

//	����ʱ״̬�л���
class RMSStateSwitchTrigger
{
	DWORD	m_triggerTimeEnd;	//	����ʱ����ʱ��
public:
	RMSStateSwitchTrigger();
	void Start(DWORD dwTimeLength);
	bool Tick();
};

//	״̬����
class RandMallShopping;
class RandMallShoppingState
{
	enumRandMallShoppingState	m_id;
	RandMallShopping*	m_parent;
public:
	virtual ~RandMallShoppingState(){}
	RandMallShoppingState(enumRandMallShoppingState id)
		: m_parent(0), m_id(id){
	}
	void Init(RandMallShopping *parent){
		m_parent = parent;
	}
	RandMallShopping * GetParent(){
		return m_parent;
	}
	enumRandMallShoppingState GetID()const{ return m_id; }
	virtual const char *GetName()const{ return "RMSState"; }
	virtual bool SetConfig(const RAND_SHOP_CONFIG *pConfig);
	virtual bool Query();
	virtual void Enter(RandMallShoppingState *pref){}
	virtual bool Roll();
	virtual bool Pay();
	virtual bool GotoNextRoll();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	virtual void Tick(){}
	
	virtual bool CanQuery()const{ return false; }
	virtual bool CanRoll()const{ return false; }
	virtual bool CanPay()const{ return false; }
	virtual bool CanGotoNextRoll()const{ return false; }
};
class RMSStateNoConfig : public RandMallShoppingState
{
	//	Ĭ��״̬: RAND_SHOP_CONFIG ����δ��ʼ��
public:
	RMSStateNoConfig() : RandMallShoppingState(RMSS_NO_CONFIG) {}
	virtual const char *GetName()const{ return "RMSStateNoConfig"; }
	virtual bool SetConfig(const RAND_SHOP_CONFIG *pConfig);
};
class RMSStateToQuery : public RandMallShoppingState
{
	//	�ȴ���ѯ��ǰ RAND_SHOP_CONFIG ���ö�Ӧ״̬
public:
	RMSStateToQuery() : RandMallShoppingState(RMSS_TO_QUERY) {}
	virtual const char *GetName()const{ return "RMSStateToQuery"; }
	virtual bool CanQuery()const{ return true; }
	virtual bool Query();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInQuery : public RandMallShoppingState
{
	//	�ȴ���ǰ RAND_SHOP_CONFIG ���ö�Ӧ״̬����
	RMSStateSwitchTrigger	m_reQueryTrigger;
public:
	RMSStateInQuery() : RandMallShoppingState(RMSS_IN_QUERY) {}
	virtual const char *GetName()const{ return "RMSStateInQuery"; }
	virtual void Enter(RandMallShoppingState *pref);
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	virtual void Tick();
};
class RMSStateToRoll : public RandMallShoppingState
{
	//	�ȴ���������һ����Ʒ
public:	
	RMSStateToRoll() : RandMallShoppingState(RMSS_TO_ROLL) {}
	virtual const char *GetName()const{ return "RMSStateToRoll"; }
	virtual bool CanRoll()const{ return true; }
	virtual bool Roll();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInRoll : public RandMallShoppingState
{
	//	�ȴ�����������
	RMSStateSwitchTrigger	m_reRollTrigger;
public:
	RMSStateInRoll() : RandMallShoppingState(RMSS_IN_ROLL) {}
	virtual const char *GetName()const{ return "RMSStateInRoll"; }
	virtual void Enter(RandMallShoppingState *pref);
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	virtual void Tick();
};
class RMSStateToPay : public RandMallShoppingState
{
	//	�ȴ���Ҹ���
public:
	RMSStateToPay() : RandMallShoppingState(RMSS_TO_PAY) {}
	virtual const char *GetName()const{ return "RMSStateToPay"; }
	virtual bool CanPay()const{ return true; }
	virtual bool Pay();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInPay : public RandMallShoppingState
{
	//	�Ѹ���ȴ�������
	RMSStateSwitchTrigger	m_rePayTrigger;
public:
	RMSStateInPay() : RandMallShoppingState(RMSS_IN_PAY) {}
	virtual const char *GetName()const{ return "RMSStateInPay"; }
	virtual void Enter(RandMallShoppingState *pref);
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	virtual void Tick();
};
class RMSStatePayed : public RandMallShoppingState
{
	//	�ɹ�����
public:
	RMSStatePayed() : RandMallShoppingState(RMSS_PAYED) {}
	virtual const char *GetName()const{ return "RMSStatePayed"; }
	virtual bool CanGotoNextRoll()const{ return true; }
	virtual bool GotoNextRoll();
};

class RandMallShopping;
class RandMallShoppingObserver
{
public:
	virtual ~RandMallShoppingObserver(){}
	virtual void OnStateChange(const RandMallShopping *p, enumRandMallShoppingState lastState)=0;
};

//	�Ե��� RAND_SHOP_CONFIG ���õĹ������
class RandMallShopping
{	
	RandMallShoppingState*	m_curState;

	RMSStateNoConfig		m_stateNoConfig;
	RMSStateToQuery			m_stateToQuery;
	RMSStateInQuery			m_stateInQuery;
	RMSStateToRoll			m_stateToRoll;
	RMSStateInRoll			m_stateInRoll;
	RMSStateToPay			m_stateToPay;
	RMSStateInPay			m_stateInPay;
	RMSStatePayed			m_statePayed;
	
	//	��״̬���õĶ���
	const RAND_SHOP_CONFIG	*m_pConfig;		//	��ǰ���������
	int						m_itemToPay;	//	��������ƷID���޴�����ʱֵΪ0��
	int						m_itemPrice;	//	��������Ʒ�۸�Ԫ����
	bool					m_bFirstTime;	//	�Ƿ�Ϊ�״ι���

	//	�۲���
	typedef RandMallShoppingObserver *	Observer;
	typedef abase::vector<Observer>	Observers;
	Observers	m_observers;

	//	����״̬����ȫȨ����
	friend class RMSStateNoConfig;
	friend class RMSStateToQuery;
	friend class RMSStateInQuery;
	friend class RMSStateToRoll;
	friend class RMSStateInRoll;
	friend class RMSStateToPay;
	friend class RMSStateInPay;
	friend class RMSStatePayed;

public:
	RandMallShopping();

	//	ת������ State �ķ���
	bool SetConfig(const RAND_SHOP_CONFIG *pConfig);
	bool Query();
	bool Roll();
	bool Pay();
	bool GotoNextRoll();
	bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	void Tick();

	bool CanQuery()const;
	bool CanRoll()const;
	bool CanPay()const;
	bool CanGotoNextRoll()const;

	bool IsStateNoConfig()const{ return m_curState == GetStateNoConfig(); }
	bool IsStateToQuery()const{ return m_curState == GetStateToQuery(); }
	bool IsStateInQuery()const{ return m_curState == GetStateInQuery(); }
	bool IsStateToRoll()const{ return m_curState == GetStateToRoll(); }
	bool IsStateInRoll()const{ return m_curState == GetStateInRoll(); }
	bool IsStateToPay()const{ return m_curState == GetStateToPay(); }
	bool IsStateInPay()const{ return m_curState == GetStateInPay(); }
	bool IsStatePayed()const{ return m_curState == GetStatePayed(); }

	//	����ѯ����
	const RAND_SHOP_CONFIG * GetConfig()const{ return m_pConfig; }
	int GetItemToPay()const{ return m_itemToPay; }
	int GetItemPrice()const{ return m_itemPrice; }
	bool IsFirstTime()const{ return m_bFirstTime; }

	//	�۲���
	bool IsObserverRegistered(const RandMallShoppingObserver *pObserver)const;
	bool RegisterObserver(RandMallShoppingObserver *pObserver);
	bool UnregisterObserver(RandMallShoppingObserver *pObserver);

private:
	//	���� State ����ʹ�õķ���
	void ChangeState(RandMallShoppingState *state);
	RandMallShoppingState * GetStateNoConfig(){ return &m_stateNoConfig; }
	RandMallShoppingState * GetStateToQuery(){ return &m_stateToQuery; }
	RandMallShoppingState * GetStateInQuery(){ return &m_stateInQuery; }
	RandMallShoppingState * GetStateToRoll(){ return &m_stateToRoll; }
	RandMallShoppingState * GetStateInRoll(){ return &m_stateInRoll; }
	RandMallShoppingState * GetStateToPay(){ return &m_stateToPay; }
	RandMallShoppingState * GetStateInPay(){ return &m_stateInPay; }
	RandMallShoppingState * GetStatePayed(){ return &m_statePayed; }

	const RandMallShoppingState * GetStateNoConfig()const { return &m_stateNoConfig; }
	const RandMallShoppingState * GetStateToQuery()const { return &m_stateToQuery; }
	const RandMallShoppingState * GetStateInQuery()const { return &m_stateInQuery; }
	const RandMallShoppingState * GetStateToRoll()const { return &m_stateToRoll; }
	const RandMallShoppingState * GetStateInRoll()const { return &m_stateInRoll; }
	const RandMallShoppingState * GetStateToPay()const { return &m_stateToPay; }
	const RandMallShoppingState * GetStateInPay()const { return &m_stateInPay; }
	const RandMallShoppingState * GetStatePayed()const { return &m_statePayed; }

	void SendCommand(int op);
	void PrintError(int result);
	void Update(const  S2C::cmd_rand_mall_shopping_res *pCmd);

	//	˽�з���
	enumRandMallShoppingState GetStateID(RandMallShoppingState *p)const;
	void NotifyObservers(enumRandMallShoppingState lastState);
};

//	RandMallShoppingManager
class RandMallShoppingManager
{
	RandMallShoppingManager();
	RandMallShoppingManager(const RandMallShoppingManager &);
	RandMallShoppingManager & operator = (const RandMallShoppingManager &);

	typedef abase::vector<RandMallShopping *>	RandMallShoppingArray;
	RandMallShoppingArray	m_items;
	RandMallShopping	*	m_pPokerShopping;
	bool					m_bInited;

public:
	static RandMallShoppingManager & Instance();

	~RandMallShoppingManager();
	void Init(int pokerConfigID);
	void Release();

	//	ս������̳�
	RandMallShopping * GetPokerShopping(){ return m_pPokerShopping; }
	const RandMallShopping * GetPokerShopping()const { return m_pPokerShopping; }

	//	��������̳�
	int	GetCount()const{ return (int)m_items.size(); }
	RandMallShopping * GetShopping(int index){ return m_items[index]; }
	const RandMallShopping * GetShopping(int index) const{ return m_items[index]; }

	bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	void Tick();
};