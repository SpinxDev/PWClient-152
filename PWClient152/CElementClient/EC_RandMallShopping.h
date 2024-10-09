// File		: EC_RandMallShopping.h
// Creator	: Xu Wenbin
// Date		: 2014/2/25

#pragma once

#include <vector.h>
#include <ABaseDef.h>

//	前置声明
namespace S2C
{
	struct cmd_rand_mall_shopping_res;
}
struct RAND_SHOP_CONFIG;

//	各状态的枚举名称
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

//	倒计时状态切换类
class RMSStateSwitchTrigger
{
	DWORD	m_triggerTimeEnd;	//	倒计时结束时刻
public:
	RMSStateSwitchTrigger();
	void Start(DWORD dwTimeLength);
	bool Tick();
};

//	状态基类
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
	//	默认状态: RAND_SHOP_CONFIG 配置未初始化
public:
	RMSStateNoConfig() : RandMallShoppingState(RMSS_NO_CONFIG) {}
	virtual const char *GetName()const{ return "RMSStateNoConfig"; }
	virtual bool SetConfig(const RAND_SHOP_CONFIG *pConfig);
};
class RMSStateToQuery : public RandMallShoppingState
{
	//	等待查询当前 RAND_SHOP_CONFIG 配置对应状态
public:
	RMSStateToQuery() : RandMallShoppingState(RMSS_TO_QUERY) {}
	virtual const char *GetName()const{ return "RMSStateToQuery"; }
	virtual bool CanQuery()const{ return true; }
	virtual bool Query();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInQuery : public RandMallShoppingState
{
	//	等待当前 RAND_SHOP_CONFIG 配置对应状态返回
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
	//	等待玩家随机下一个商品
public:	
	RMSStateToRoll() : RandMallShoppingState(RMSS_TO_ROLL) {}
	virtual const char *GetName()const{ return "RMSStateToRoll"; }
	virtual bool CanRoll()const{ return true; }
	virtual bool Roll();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInRoll : public RandMallShoppingState
{
	//	等待随机结果返回
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
	//	等待玩家付款
public:
	RMSStateToPay() : RandMallShoppingState(RMSS_TO_PAY) {}
	virtual const char *GetName()const{ return "RMSStateToPay"; }
	virtual bool CanPay()const{ return true; }
	virtual bool Pay();
	virtual bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
};
class RMSStateInPay : public RandMallShoppingState
{
	//	已付款、等待付款结果
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
	//	成功付款
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

//	对单个 RAND_SHOP_CONFIG 配置的购买控制
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
	
	//	各状态共用的对象
	const RAND_SHOP_CONFIG	*m_pConfig;		//	当前处理的配置
	int						m_itemToPay;	//	待付款商品ID（无待付款时值为0）
	int						m_itemPrice;	//	待付款商品价格（元宝）
	bool					m_bFirstTime;	//	是否为首次购买

	//	观察者
	typedef RandMallShoppingObserver *	Observer;
	typedef abase::vector<Observer>	Observers;
	Observers	m_observers;

	//	允许状态对象全权访问
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

	//	转发给各 State 的方法
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

	//	外界查询方法
	const RAND_SHOP_CONFIG * GetConfig()const{ return m_pConfig; }
	int GetItemToPay()const{ return m_itemToPay; }
	int GetItemPrice()const{ return m_itemPrice; }
	bool IsFirstTime()const{ return m_bFirstTime; }

	//	观察者
	bool IsObserverRegistered(const RandMallShoppingObserver *pObserver)const;
	bool RegisterObserver(RandMallShoppingObserver *pObserver);
	bool UnregisterObserver(RandMallShoppingObserver *pObserver);

private:
	//	被各 State 对象使用的方法
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

	//	私有方法
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

	//	战灵随机商城
	RandMallShopping * GetPokerShopping(){ return m_pPokerShopping; }
	const RandMallShopping * GetPokerShopping()const { return m_pPokerShopping; }

	//	其它随机商城
	int	GetCount()const{ return (int)m_items.size(); }
	RandMallShopping * GetShopping(int index){ return m_items[index]; }
	const RandMallShopping * GetShopping(int index) const{ return m_items[index]; }

	bool Process(const S2C::cmd_rand_mall_shopping_res *pCmd);
	void Tick();
};