// File		: EC_RandMallShopping.cpp
// Creator	: Xu Wenbin
// Date		: 2014/2/25

#include "EC_RandMallShopping.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"

//	
extern CECGame * g_pGame;

//	class RMSStateSwitchTrigger
RMSStateSwitchTrigger::RMSStateSwitchTrigger()
: m_triggerTimeEnd(0)
{
}

void RMSStateSwitchTrigger::Start(DWORD dwTimeLength)
{
	m_triggerTimeEnd = timeGetTime() + dwTimeLength;
}

bool RMSStateSwitchTrigger::Tick()
{
	//	����ʱʱ�䵽ʱ���� true 
	return m_triggerTimeEnd <= timeGetTime();
}

//	class RandMallShopping
RandMallShopping::RandMallShopping()
: m_curState(NULL)
, m_pConfig(NULL)
, m_itemToPay(0)
, m_itemPrice(-1)
, m_bFirstTime(false)
{
	GetStateNoConfig()->Init(this);
	GetStateToQuery()->Init(this);
	GetStateInQuery()->Init(this);
	GetStateToRoll()->Init(this);
	GetStateInRoll()->Init(this);
	GetStateToPay()->Init(this);
	GetStateInPay()->Init(this);
	GetStatePayed()->Init(this);

	ChangeState(GetStateNoConfig());
}

void RandMallShopping::ChangeState(RandMallShoppingState *state)
{
	if (state == m_curState){
		return;
	}
	RandMallShoppingState *prev = m_curState;
	m_curState = state;
	m_curState->Enter(prev);
	NotifyObservers(GetStateID(prev));
}

bool RandMallShopping::SetConfig(const RAND_SHOP_CONFIG *pConfig)
{
	return m_curState->SetConfig(pConfig);
}

bool RandMallShopping::Query()
{
	//	ִ��״̬ͬ����ѯ����
	return m_curState->Query();
}

bool RandMallShopping::Roll()
{
	//	ִ���������
	return m_curState->Roll();
}

bool RandMallShopping::Pay()
{
	//	ִ�и�������
	return m_curState->Pay();
}

bool RandMallShopping::GotoNextRoll()
{
	//	�л��������״̬��׼���´����
	return m_curState->GotoNextRoll();
}

bool RandMallShopping::CanQuery()const
{
	return m_curState->CanQuery();
}

bool RandMallShopping::CanRoll()const
{
	return m_curState->CanRoll();
}

bool RandMallShopping::CanPay()const
{
	return m_curState->CanPay();
}

bool RandMallShopping::CanGotoNextRoll()const
{
	return m_curState->CanGotoNextRoll();
}

bool RandMallShopping::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	if (!m_pConfig || pCmd->config_id != (int)m_pConfig->id){
		return false;
	}
	a_LogOutput(1, "%s::Process(RANDOM_MALL_SHOPPING_RES)",	m_curState->GetName());
	return m_curState->Process(pCmd);
}

void RandMallShopping::Tick()
{
	//	ʹ�� Tick �����������£����̯�������������ʱ����ӦЭ�����⣺
	//	�����м�״̬ʹ�� Tick ��ɲ���ӦЭ������µ�״̬�л����Ա�����л����ٴγ���
	//	���̯ʱ����ѯ������Ӧ����һ��ʱ���״̬�л������ٴβ�ѯ״̬
	//	�л����ٴοɲ���״̬�£���� RMSStateInQuery �л��� RMSStateToQuery����Ҫͬʱ������Ӧ���ز�ѯ������������ RMSStateToQuery �����в�ѯ���������� ����Ӧ���������
	m_curState->Tick();
}

void RandMallShopping::SendCommand(int op)
{
	g_pGame->GetGameSession()->c2s_CmdRandMallShopping(m_pConfig->id, op);
}

void RandMallShopping::Update(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	m_itemToPay = pCmd->item_to_pay;
	m_itemPrice = pCmd->price;
	m_bFirstTime = pCmd->firstflag;
}

void RandMallShopping::PrintError(int result)
{
	int iFixedMsg(-1);
	switch (result)
	{
	case RAND_MALL_SHOPPING_RES_OK:
		ASSERT(false);
		break;
	case RAND_MALL_SHOPPING_RES_INVALID_CONFIG:
		iFixedMsg = FIXMSG_RAND_SHOP_INVALID_CONFIG;
		break;
	case RAND_MALL_SHOPPING_RES_INVALID_ROLE_STATE:
		iFixedMsg = FIXMSG_RAND_SHOP_INVALID_ROLE_STATE;
		break;
	case RAND_MALL_SHOPPING_RES_INVALID_OP:
		iFixedMsg = FIXMSG_RAND_SHOP_INVALID_OP;
		break;
	case RAND_MALL_SHOPPING_RES_CASH_NOT_ENOUGH:
		iFixedMsg = FIXMSG_NEEDCASH;
		break;
	case RAND_MALL_SHOPPING_RES_PACK_FULL:
		iFixedMsg = FIXMSG_PACKFULL1;
		break;
	default:
		ASSERT(false);
		break;
	}
	if (iFixedMsg >= 0){
		g_pGame->GetGameRun()->AddFixedMessage(iFixedMsg);
	}else if (result != RAND_MALL_SHOPPING_RES_OK){
		g_pGame->GetGameRun()->AddChatMessage(ACString().Format(_AL("Unknown error(%d)"), result), GP_CHAT_SYSTEM);
	}
}

enumRandMallShoppingState RandMallShopping::GetStateID(RandMallShoppingState *p)const
{
	return p ? p->GetID() : RMSS_NONE;
}

void RandMallShopping::NotifyObservers(enumRandMallShoppingState lastState)
{
	Observers observers;
	for (Observers::iterator it = m_observers.begin(); it != m_observers.end(); ++ it)
	{
		Observer & o = *it;
		o->OnStateChange(this, lastState);
	}
}

bool RandMallShopping::IsObserverRegistered(const RandMallShoppingObserver *pObserver)const
{
	return std::find(m_observers.begin(), m_observers.end(), pObserver) != m_observers.end();
}

bool RandMallShopping::RegisterObserver(RandMallShoppingObserver *pObserver)
{
	bool bRegistered(false);
	if (!IsObserverRegistered(pObserver)){
		m_observers.push_back(pObserver);
		pObserver->OnStateChange(this, RMSS_NONE);
		bRegistered = true;
	}
	return bRegistered;
}

bool RandMallShopping::UnregisterObserver(RandMallShoppingObserver *pObserver)
{
	bool bUnRegistered(false);
	Observers::iterator it = std::find(m_observers.begin(), m_observers.end(), pObserver);
	if (it != m_observers.end()){
		m_observers.erase(it);
		bUnRegistered = true;
	}
	return bUnRegistered;
}

//	class RandMallShoppingState
bool RandMallShoppingState::SetConfig(const RAND_SHOP_CONFIG *pConfig)
{
	ASSERT(false);
	return false;
}
bool RandMallShoppingState::Query()
{
	ASSERT(false);
	return false;
}
bool RandMallShoppingState::Roll()
{
	ASSERT(false);
	return false;
}
bool RandMallShoppingState::Pay()
{
	ASSERT(false);
	return false;
}
bool RandMallShoppingState::GotoNextRoll()
{
	ASSERT(false);
	return false;
}
bool RandMallShoppingState::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	ASSERT(false);
	return false;
}

//	class RMSStateNoConfig
bool RMSStateNoConfig::SetConfig(const RAND_SHOP_CONFIG *pConfig)
{
	if (!pConfig){
		return false;
	}
	GetParent()->m_pConfig = pConfig;
	GetParent()->m_itemToPay = 0;
	GetParent()->m_itemPrice = -1;
	GetParent()->ChangeState(GetParent()->GetStateToQuery());
	return true;
}

//	class RMSStateToQuery
bool RMSStateToQuery::Query()
{
	GetParent()->SendCommand(RAND_MALL_OP_QUERY);
	GetParent()->ChangeState(GetParent()->GetStateInQuery());
	return true;
}

bool RMSStateToQuery::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	�� void RandMallShopping::Tick() ��˵��
	if (pCmd->op != RAND_MALL_OP_QUERY){
		ASSERT(false);
		return false;
	}
	GetParent()->ChangeState(GetParent()->GetStateInQuery());
	return GetParent()->Process(pCmd);
}

//	class RMSStateInQuery
bool RMSStateInQuery::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	�����ѯ���

	if (pCmd->op != RAND_MALL_OP_QUERY){
		ASSERT(false);
		return false;
	}
	switch (pCmd->result){
	case RAND_MALL_SHOPPING_RES_OK:
		//	��ѯ�ɹ�����¼��ѯ���
		GetParent()->Update(pCmd);

		//	�л�����״̬
		{
			RandMallShoppingState *pNewState = NULL;
			if (pCmd->item_to_pay > 0){
				//	�д�������Ʒ�����������״̬
				pNewState = GetParent()->GetStateToPay();
			}else{
				//	�޴�������Ʒ����������״̬
				pNewState = GetParent()->GetStateToRoll();
			}
			GetParent()->ChangeState(pNewState);
		}
		break;
	default:
		//	ֻ������ RAND_MALL_SHOPPING_RES_OK��RAND_MALL_SHOPPING_RES_INVALID_CONFIG ���ַ��ؽ��
		ASSERT(false);

		//	��ӡ���󡢲�ά����״
		GetParent()->PrintError(pCmd->result);
		break;
	}
	return true;
}

void RMSStateInQuery::Enter(RandMallShoppingState *pref)
{
	m_reQueryTrigger.Start(2000);		//	��������ʱ��ʱ�䵽���л��ؿ��ٴβ�ѯ״̬����Ӧ�԰�̯��ʱ����������Ӧ
}

void RMSStateInQuery::Tick()
{
	if (m_reQueryTrigger.Tick()){
		GetParent()->ChangeState(GetParent()->GetStateToQuery());
	}
}

//	RMSStateToRoll
bool RMSStateToRoll::Roll()
{
	GetParent()->SendCommand(RAND_MALL_OP_ROLL);
	GetParent()->ChangeState(GetParent()->GetStateInRoll());
	return true;
}

bool RMSStateToRoll::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	�� void RandMallShopping::Tick() ��˵��
	if (pCmd->op != RAND_MALL_OP_ROLL){
		ASSERT(false);
		return false;
	}
	GetParent()->ChangeState(GetParent()->GetStateInRoll());
	return GetParent()->Process(pCmd);
}

//	RMSStateInRoll
bool RMSStateInRoll::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	����������

	if (pCmd->op != RAND_MALL_OP_ROLL){
		ASSERT(false);
		return false;
	}
	switch (pCmd->result){
	case RAND_MALL_SHOPPING_RES_OK:
		//	����ɹ�����¼������
		GetParent()->Update(pCmd);
		//	���������״̬
		GetParent()->ChangeState(GetParent()->GetStateToPay());
		break;
	case RAND_MALL_SHOPPING_RES_INVALID_ROLE_STATE:
		//	��ɫ״̬�޷������������ӡʧ����Ϣ
		GetParent()->PrintError(pCmd->result);
		//	��������״̬���ȴ�����ʱ��
		GetParent()->ChangeState(GetParent()->GetStateToRoll());
		break;
	default:
		ASSERT(false);
		GetParent()->PrintError(pCmd->result);
		break;
	}
	return true;
}

void RMSStateInRoll::Enter(RandMallShoppingState *pref)
{
	m_reRollTrigger.Start(2000);		//	��������ʱ��ʱ�䵽���л��ؿ��ٴ����״̬����Ӧ�԰�̯��ʱ����������Ӧ
}

void RMSStateInRoll::Tick()
{
	if (m_reRollTrigger.Tick()){
		GetParent()->ChangeState(GetParent()->GetStateToRoll());
	}
}

//	RMSStateToPay
bool RMSStateToPay::Pay()
{
	GetParent()->SendCommand(RAND_MALL_OP_PAY);
	GetParent()->ChangeState(GetParent()->GetStateInPay());
	return true;
}

bool RMSStateToPay::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	�� void RandMallShopping::Tick() ��˵��
	if (pCmd->op != RAND_MALL_OP_PAY){
		ASSERT(false);
		return false;
	}
	GetParent()->ChangeState(GetParent()->GetStateInPay());
	return GetParent()->Process(pCmd);
}

//	RMSStateInPay
bool RMSStateInPay::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	//	��������

	if (pCmd->op != RAND_MALL_OP_PAY){
		ASSERT(false);
		return false;
	}
	switch (pCmd->result){
	case RAND_MALL_SHOPPING_RES_OK:
		//	����ɹ�����¼֧�����
		GetParent()->Update(pCmd);
		//	�л����Ѹ���״̬
		GetParent()->ChangeState(GetParent()->GetStatePayed());
		break;
	case RAND_MALL_SHOPPING_RES_INVALID_ROLE_STATE:	//	��ɫ״̬�޷����и���
	case RAND_MALL_SHOPPING_RES_CASH_NOT_ENOUGH:	//	Ԫ������
	case RAND_MALL_SHOPPING_RES_PACK_FULL:			//	�����ռ䲻��
		//	��ӡʧ����Ϣ
		GetParent()->PrintError(pCmd->result);
		//	����ɸ���״̬���ȴ�����ʱ��
		GetParent()->ChangeState(GetParent()->GetStateToPay());
		break;
	default:
		ASSERT(false);
		GetParent()->PrintError(pCmd->result);
		break;
	}
	return true;
}

void RMSStateInPay::Enter(RandMallShoppingState *pref)
{
	m_rePayTrigger.Start(2000);		//	��������ʱ��ʱ�䵽���л��ؿ��ٴ����״̬����Ӧ�԰�̯��ʱ����������Ӧ
}

void RMSStateInPay::Tick()
{
	if (m_rePayTrigger.Tick()){
		GetParent()->ChangeState(GetParent()->GetStateToPay());
	}
}

//	RMSStatePayed
bool RMSStatePayed::GotoNextRoll()
{
	GetParent()->ChangeState(GetParent()->GetStateToRoll());
	return true;
}

//	RandMallShoppingManager
RandMallShoppingManager::RandMallShoppingManager()
: m_pPokerShopping(NULL)
, m_bInited(false)
{
}

RandMallShoppingManager::~RandMallShoppingManager()
{
	Release();
}

void RandMallShoppingManager::Release()
{
	for (RandMallShoppingArray::iterator it = m_items.begin(); it != m_items.end(); ++ it)
	{
		delete *it;
	}
	m_items.clear();

	delete m_pPokerShopping;
	m_pPokerShopping = NULL;

	m_bInited = false;
}

void RandMallShoppingManager::Init(int pokerConfigID)
{
	if (m_bInited){
		return;
	}
	
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);	
	while(tid)
	{
		if (DataType == DT_RAND_SHOP_CONFIG){
			if (RAND_SHOP_CONFIG* pConfig = (RAND_SHOP_CONFIG *)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType)){
				RandMallShopping * p = new RandMallShopping;
				p->SetConfig(pConfig);
				if ((int)pConfig->id == pokerConfigID){
					m_pPokerShopping = p;
				}else{
					m_items.push_back(p);
				}
			}
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
	if (!m_pPokerShopping){
		a_LogOutput(1, "RandMallShoppingManager::Init(%d), Failed to init poker shopping",	pokerConfigID);
	}
	m_bInited = true;
}

RandMallShoppingManager & RandMallShoppingManager::Instance()
{
	static RandMallShoppingManager s_instance;
	return s_instance;
}

bool RandMallShoppingManager::Process(const S2C::cmd_rand_mall_shopping_res *pCmd)
{
	if (m_pPokerShopping && m_pPokerShopping->Process(pCmd)){
		return true;
	}
	for (RandMallShoppingArray::iterator it = m_items.begin(); it != m_items.end(); ++ it)
	{
		RandMallShopping *p = *it;
		if (p->Process(pCmd)){
			return true;
		}
	}
	return false;
}

void RandMallShoppingManager::Tick()
{
	if (m_pPokerShopping){
		m_pPokerShopping->Tick();
	}
	for (RandMallShoppingArray::iterator it = m_items.begin(); it != m_items.end(); ++ it)
	{
		RandMallShopping *p = *it;
		p->Tick();
	}
}