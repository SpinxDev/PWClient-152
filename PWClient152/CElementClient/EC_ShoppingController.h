// File		: EC_ShoppingController.h
// Creator	: Xu Wenbin
// Date		: 2014/5/15

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCONTROLLER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCONTROLLER_H_

#include "EC_Observer.h"
#include "EC_TimeSafeChecker.h"
#include <vector.h>

//	ǰ�������Ͷ���
class CECShoppingCart;
typedef CECObserver<CECShoppingCart> CECShoppingCartObserver;
class CECShoppingCartItem;
typedef int	ItemShoppingCartID;

struct ItemShoppingInfo{
	ItemShoppingCartID	m_itemCartID;
	int					m_itemIndex;
	int					m_buyIndex;
	int					m_buyedCount;	//	�Ѿ�������̳���Ʒ��������һ���̳���Ʒ�п��԰��������Ʒ����˲��ȼ�����Ʒ������
	int					m_inbuyCount;	//	���ڹ�����̳���Ʒ������ͬ�ϣ�
	int					m_tobuyCount;	//	����Ҫ�ٹ�����̳���Ʒ������ͬ�ϣ�
	ItemShoppingInfo();
	ItemShoppingInfo(const CECShoppingCartItem *pItem);
	int	CountNeedToBuy()const;
	void OnBuy(int count);
};
typedef abase::vector<ItemShoppingInfo>		ItemShoppingInfos;
extern int globalGetItemShoppingBuyedCount(const ItemShoppingInfos &infos);
extern int globalGetItemShoppingInBuyCount(const ItemShoppingInfos &infos);
extern int globalGetItemShoppingToBuyCount(const ItemShoppingInfos &infos);

//	����ֹͣԭ��
enum enumShoppingControllerStopBuyingReason
{
	SCSBR_NONE,
	SCSBR_FINISHED,				//	������й���
	SCSBR_BY_USER,				//	�û����ֹͣ
	SCSBR_TIME_ARRIVED,			//	������Э��һ��ʱ����δ�յ�����������
	SCSBR_CLEAR,				//	����������л�����¼״̬�ȣ�
	SCSBR_MALL_ITEM_CHANGED,	//	�̳������и��£����ʺ��ٹ���
};

//	������̿���֪ͨ����
class CECShoppingControllerChange : public CECObservableChange
{
public:
	enum ChangeMask{
		START_SHOPPING,			//	��ʼ����
		SHOPPING_CMD_SENT,		//	����ĳ��Ʒ�Ĺ���Э��
		ITEM_APPEAR_IN_PACK,	//	�����������ѷ��͹���Э������ص���Ʒ
		STOP_SHOPPING,			//	�������
	};
private:
	ChangeMask				m_changeMask;
	const CECShoppingCart*	m_pShoppingCart;
	ItemShoppingInfos		m_buyedItems;			//	�ѹ�����Ʒ
	ItemShoppingInfos		m_buyingItems;			//	������Э�鷵�ص���Ʒ
	ItemShoppingInfos		m_waitingItems;			//	��δ�������Ʒ
	enumShoppingControllerStopBuyingReason m_stopReason;	//	ֹͣ����ԭ��

	//	����
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

//	���� CECShoppingController �Ĺ۲���
class CECShoppingController;
typedef CECObserver<CECShoppingController>	CECShoppingControllerObserver;

//	���ﳵ����ģ��״̬
enum enumShoppingControllerState{
	SCS_NO_SHOPPING_CART,		//	��δָ�����ﳵ
	SCS_IDLE,					//	����
	SCS_TO_SEND_BUY_COMMAND,	//	�����͹���������ĳ��Ʒ��
	SCS_WAITING_FEEDBACK,		//	�ȴ�������
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

//	���ﳵ��������ģ��
class CECShoppingController : public CECObservable<CECShoppingController>, public CECShoppingCartObserver
{
	CECShoppingControllerState*	m_curState;
	
	SCStateNoShoppingCart		m_stateNoShoppingCart;
	SCStateIdle					m_stateIdle;
	SCStateToSendBuyCommand		m_stateToSendBuyCommand;
	SCStateWaitingFeedback		m_stateWaitingFeedback;

	//	�� State ���ö���
	CECShoppingCart			*	m_pShoppingCart;
	ItemShoppingInfos			m_buyedItems;			//	�ѹ����̳���Ʒ
	ItemShoppingInfos			m_buyingItems;			//	������Э�鷵�ص��̳���Ʒ
	ItemShoppingInfos			m_waitingItems;			//	��δ������̳���Ʒ
	CECTimeSafeChecker			m_switchTrigger;		//	����״̬ʹ�õ��ĵ���ʱ
	
	//	����״̬����ȫȨ����
	friend class SCStateNoShoppingCart;
	friend class SCStateIdle;
	friend class SCStateToSendBuyCommand;
	friend class SCStateWaitingFeedback;

public:
	CECShoppingController();
	~CECShoppingController();

	//	������ CECShoppingCartObserver
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECShoppingCart *p);

	//	ת������ State ����
	bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	bool Buy();
	bool StopBuying(enumShoppingControllerStopBuyingReason reason);
	void Tick();

	bool IsBuying();

	bool OnObtainItem(int iPack, int idItem, int iAmount);

	//	����ѯ����	
	bool IsStateNoShoppingCart()const{ return m_curState == GetStateNoShoppingCart(); }
	bool IsStateIdle()const{ return m_curState == GetStateIdle(); }
	bool IsStateToSendBuyCommand()const{ return m_curState == GetStateToSendBuyCommand(); }
	bool IsStateWaitingFeedback()const{ return m_curState == GetStateWaitingFeedback(); }

private:
	//	���� State ʹ�õķ���
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