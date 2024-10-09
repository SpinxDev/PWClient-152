// File		: EC_ShoppingController.h
// Creator	: Xu Wenbin
// Date		: 2014/5/15

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCONTROLLER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCONTROLLER_H_

#include "EC_Observer.h"
#include "EC_TimeSafeChecker.h"
#include <vector.h>

//	前置声明和定义
class CECShoppingCart;
typedef CECObserver<CECShoppingCart> CECShoppingCartObserver;
class CECShoppingCartItem;
typedef int	ItemShoppingCartID;

struct ItemShoppingInfo{
	ItemShoppingCartID	m_itemCartID;
	int					m_itemIndex;
	int					m_buyIndex;
	int					m_buyedCount;	//	已经购买的商城物品份数（因一件商城物品中可以包含多份物品，因此不等价于商品个数）
	int					m_inbuyCount;	//	正在购买的商城物品份数（同上）
	int					m_tobuyCount;	//	还需要再购买的商城物品份数（同上）
	ItemShoppingInfo();
	ItemShoppingInfo(const CECShoppingCartItem *pItem);
	int	CountNeedToBuy()const;
	void OnBuy(int count);
};
typedef abase::vector<ItemShoppingInfo>		ItemShoppingInfos;
extern int globalGetItemShoppingBuyedCount(const ItemShoppingInfos &infos);
extern int globalGetItemShoppingInBuyCount(const ItemShoppingInfos &infos);
extern int globalGetItemShoppingToBuyCount(const ItemShoppingInfos &infos);

//	购买停止原因
enum enumShoppingControllerStopBuyingReason
{
	SCSBR_NONE,
	SCSBR_FINISHED,				//	完成所有购买
	SCSBR_BY_USER,				//	用户点击停止
	SCSBR_TIME_ARRIVED,			//	发出的协议一定时间内未收到服务器反馈
	SCSBR_CLEAR,				//	程序清除（切换到登录状态等）
	SCSBR_MALL_ITEM_CHANGED,	//	商城数据有更新，不适合再购买
};

//	购物过程控制通知类型
class CECShoppingControllerChange : public CECObservableChange
{
public:
	enum ChangeMask{
		START_SHOPPING,			//	开始购买
		SHOPPING_CMD_SENT,		//	发送某物品的购买协议
		ITEM_APPEAR_IN_PACK,	//	包裹中新增已发送购买协议待返回的物品
		STOP_SHOPPING,			//	购买结束
	};
private:
	ChangeMask				m_changeMask;
	const CECShoppingCart*	m_pShoppingCart;
	ItemShoppingInfos		m_buyedItems;			//	已购买物品
	ItemShoppingInfos		m_buyingItems;			//	待购买协议返回的物品
	ItemShoppingInfos		m_waitingItems;			//	还未购买的物品
	enumShoppingControllerStopBuyingReason m_stopReason;	//	停止购买原因

	//	禁用
	CECShoppingControllerChange(const CECShoppingControllerChange &);
	CECShoppingControllerChange & operator == (const CECShoppingControllerChange &);
public:
	CECShoppingControllerChange(const CECShoppingCart *pShoppingCart, ChangeMask changeMask
		, ItemShoppingInfos *pBuyedItems
		, ItemShoppingInfos *pBuyingItems
		, ItemShoppingInfos *pWaitingItems
		, enumShoppingControllerStopBuyingReason stopReason=SCSBR_NONE);

	ChangeMask GetChangeMask()const;
	const CECShoppingCart* ShoppingCart()const;
	const ItemShoppingInfos & BuyedItems()const;
	const ItemShoppingInfos & BuyingItems()const;
	const ItemShoppingInfos & WaitingItems()const;
	enumShoppingControllerStopBuyingReason StopReason()const;
	int BuyedItemsCount()const;
	int InBuyItemsCount()const;
	int ToBuyItemsCount()const;
};

//	定义 CECShoppingController 的观察者
class CECShoppingController;
typedef CECObserver<CECShoppingController>	CECShoppingControllerObserver;

//	购物车数据模型状态
enum enumShoppingControllerState{
	SCS_NO_SHOPPING_CART,		//	尚未指定购物车
	SCS_IDLE,					//	闲置
	SCS_TO_SEND_BUY_COMMAND,	//	待发送购买命令（针对某商品）
	SCS_WAITING_FEEDBACK,		//	等待购买反馈
};

class CECShoppingController;
class CECShoppingControllerState
{
	enumShoppingControllerState	m_id;
	CECShoppingController	*	m_parent;
public:
	virtual ~CECShoppingControllerState(){}
	CECShoppingControllerState(enumShoppingControllerState id)
		: m_parent(0), m_id(id){
	}
	void Init(CECShoppingController *parent){
		m_parent = parent;
	}
	CECShoppingController * GetParent(){
		return m_parent;
	}
	enumShoppingControllerState GetID()const{ return m_id; }
	virtual const char *GetName()const{ return "SCState"; }
	
	virtual void Enter(CECShoppingControllerState *prev){}

	virtual bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	virtual bool Buy();
	virtual bool StopBuying(enumShoppingControllerStopBuyingReason reason);
	virtual void Tick(){}
	virtual bool IsBuying(){ return false; }
	virtual bool OnObtainItem(int iPack, int idItem, int iAmount){ return false; }
};
class SCStateNoShoppingCart : public CECShoppingControllerState
{
public:
	SCStateNoShoppingCart();
	virtual const char *GetName()const{ return "SCStateNoShoppingCart"; }
	virtual bool SetShoppingCart(CECShoppingCart *pShoppingCart);
};
class SCStateIdle : public CECShoppingControllerState
{
public:
	SCStateIdle();
	virtual const char *GetName()const{ return "SCStateIdle"; }
	virtual bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	virtual bool Buy();
};
class SCStateToSendBuyCommand : public CECShoppingControllerState
{
public:
	SCStateToSendBuyCommand();
	virtual const char *GetName()const{ return "SCStateToSendBuyCommand"; }
	virtual bool StopBuying(enumShoppingControllerStopBuyingReason reason);
	virtual void Tick();
	virtual bool IsBuying(){ return true; }
private:
};
class SCStateWaitingFeedback : public CECShoppingControllerState
{
public:
	SCStateWaitingFeedback();
	virtual const char *GetName()const{ return "SCStateWaitingFeedback"; }
	virtual void Enter(CECShoppingControllerState *prev);
	virtual bool StopBuying(enumShoppingControllerStopBuyingReason reason);
	virtual void Tick();
	virtual bool IsBuying(){ return true; }
	virtual bool OnObtainItem(int iPack, int idItem, int iAmount);
};

//	购物车购买数据模型
class CECShoppingController : public CECObservable<CECShoppingController>, public CECShoppingCartObserver
{
	CECShoppingControllerState*	m_curState;
	
	SCStateNoShoppingCart		m_stateNoShoppingCart;
	SCStateIdle					m_stateIdle;
	SCStateToSendBuyCommand		m_stateToSendBuyCommand;
	SCStateWaitingFeedback		m_stateWaitingFeedback;

	//	各 State 共用对象
	CECShoppingCart			*	m_pShoppingCart;
	ItemShoppingInfos			m_buyedItems;			//	已购买商城商品
	ItemShoppingInfos			m_buyingItems;			//	待购买协议返回的商城商品
	ItemShoppingInfos			m_waitingItems;			//	还未购买的商城商品
	CECTimeSafeChecker			m_switchTrigger;		//	部分状态使用到的倒计时
	
	//	允许状态对象全权访问
	friend class SCStateNoShoppingCart;
	friend class SCStateIdle;
	friend class SCStateToSendBuyCommand;
	friend class SCStateWaitingFeedback;

public:
	CECShoppingController();
	~CECShoppingController();

	//	派生自 CECShoppingCartObserver
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECShoppingCart *p);

	//	转发给各 State 方法
	bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	bool Buy();
	bool StopBuying(enumShoppingControllerStopBuyingReason reason);
	void Tick();

	bool IsBuying();

	bool OnObtainItem(int iPack, int idItem, int iAmount);

	//	外界查询方法	
	bool IsStateNoShoppingCart()const{ return m_curState == GetStateNoShoppingCart(); }
	bool IsStateIdle()const{ return m_curState == GetStateIdle(); }
	bool IsStateToSendBuyCommand()const{ return m_curState == GetStateToSendBuyCommand(); }
	bool IsStateWaitingFeedback()const{ return m_curState == GetStateWaitingFeedback(); }

private:
	//	被各 State 使用的方法
	void ChangeState(CECShoppingControllerState *state);

	CECShoppingControllerState * GetStateNoShoppingCart(){ return &m_stateNoShoppingCart; }
	CECShoppingControllerState * GetStateIdle(){ return &m_stateIdle; }
	CECShoppingControllerState * GetStateToSendBuyCommand(){ return &m_stateToSendBuyCommand; }
	CECShoppingControllerState * GetStateWaitingFeedback(){ return &m_stateWaitingFeedback; }

	const CECShoppingControllerState * GetStateNoShoppingCart()const { return &m_stateNoShoppingCart; }
	const CECShoppingControllerState * GetStateIdle()const { return &m_stateIdle; }
	const CECShoppingControllerState * GetStateToSendBuyCommand()const { return &m_stateToSendBuyCommand; }
	const CECShoppingControllerState * GetStateWaitingFeedback()const { return &m_stateWaitingFeedback; }

	void SetShoppingCartImpl(CECShoppingCart *pShoppingCart);
	void StopBuyingImpl(enumShoppingControllerStopBuyingReason reason);
	void UpdateShoppingCartOnObtainItem(ItemShoppingCartID itemCartID);
};

#endif	//	_ELEMENTCLIENT_EC_SHOPPINGCONTROLLER_H_