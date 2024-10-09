// File		: EC_Reconnect.h
// Creator	: Xu Wenbin
// Date		: 2014/10/8

#ifndef _ELEMENTCLIENT_EC_RECONNECT_H_
#define _ELEMENTCLIENT_EC_RECONNECT_H_

#include "EC_Global.h"

#include <AString.h>

class CECReconnectPolicy{
protected:
	virtual ACString GetUser()const=0;
	ACString GetCurrentUser()const;
	bool MatchCurrentUser(const ACString &user)const;
	bool MatchCurrentUser()const;
public:
	virtual ~CECReconnectPolicy(){}
	virtual bool CanRequestReconnect()const=0;
	virtual bool RequestReconnect()=0;
	virtual bool CanReconnectNow()const=0;
	virtual void UseLoginParameter()=0;
};

//	�򵥵�"�û��� + ����"��������
class CECReconnectSimpleUserPasswordPolicy : public CECReconnectPolicy{
	ACString	m_user;
	ACString	m_password;
protected:
	virtual ACString GetUser()const;
public:
	CECReconnectSimpleUserPasswordPolicy(const ACString &user, const ACString &password);
	virtual bool CanRequestReconnect()const;
	virtual bool RequestReconnect();
	virtual bool CanReconnectNow()const;
	virtual void UseLoginParameter();
};

//	"�û��� + token" ��������
class CECReconnectUserTokenPolicy : public CECReconnectPolicy{
	ACString	m_user;
	ACString	m_token;
protected:
	virtual ACString GetUser()const;
public:
	CECReconnectUserTokenPolicy(const ACString &user);
	virtual bool SetUserToken(const ACString &user, const ACString &token);
	virtual bool CanReconnectNow()const;
	virtual void UseLoginParameter();
};

//	���� Arc ���û��� + token�� ��������
class CECReconnectForeignArcUserTokenPolicy : public CECReconnectUserTokenPolicy{
public:
	CECReconnectForeignArcUserTokenPolicy(const ACString &user);
	virtual bool CanRequestReconnect()const;
	virtual bool RequestReconnect();
};

enum enumReconnectState{
	RS_NONE,
	RS_CAN_REQUEST,
	RS_REQUESTED,
	RS_CAN_RECONNECT,
	RS_RECONNECTING,
	RS_RECONNECTED,
};

//	����״̬����
class CECReconnect;
class CECReconnectState{
	enumReconnectState	m_id;
	CECReconnect*		m_parent;
public:
	virtual ~CECReconnectState(){}
	CECReconnectState(enumReconnectState id)
		: m_parent(0), m_id(id){
	}
	void Init(CECReconnect *parent){
		m_parent = parent;
	}
	CECReconnect * GetParent()const{
		return m_parent;
	}
	enumReconnectState GetID()const{ return m_id; }
	virtual const char *GetName()const{ return "RSState"; }
	
	virtual void Enter(CECReconnectState *prev){}
	virtual void SetServerIndex(int serverIndex);
	virtual void SetRoleID(int roleID);
	virtual void SetReconnectPolicyCandidate(CECReconnectPolicy *policy);
	virtual void AcceptReconnectPolicyCandidate();
	
	virtual bool CanRequestReconnect()const{ return false; }
	virtual bool RequestReconnect();
	virtual void Tick(){}
	virtual bool ApplyServerSetting();
	virtual void UseLoginParameter();
	
	virtual bool IsReconnecting()const{ return false; }
	virtual bool IsReconnectFromGame()const{ return false; }
	
	virtual void OnReconnectSuccess();
	virtual void OnReconnectFail();
};

class CECReconnectStateNone : public CECReconnectState{
	//	����δ��ʼ��״̬
	bool CanRequest()const;
	void TestAndGotoStateCanRequest();
public:
	CECReconnectStateNone() : CECReconnectState(RS_NONE) {}
	virtual const char *GetName()const{ return "ReconnectStateNone"; }		
	
	virtual void Enter(CECReconnectState *prev);
	virtual void SetServerIndex(int serverIndex);
	virtual void SetRoleID(int roleID);
	virtual void SetReconnectPolicyCandidate(CECReconnectPolicy *policy);
	virtual void AcceptReconnectPolicyCandidate();
};

class CECReconnectStateCanRequest : public CECReconnectState{
	//	�������������߱�������������
public:
	CECReconnectStateCanRequest() : CECReconnectState(RS_CAN_REQUEST) {}
	virtual const char *GetName()const{ return "ReconnectStateCanRequest"; }
	
	virtual void SetRoleID(int roleID);
	virtual bool CanRequestReconnect()const;
	virtual bool RequestReconnect();
};

class CECReconnectStateRequested : public CECReconnectState{
	//	�������������ȴ���ȡ������¼����
public:
	CECReconnectStateRequested() : CECReconnectState(RS_REQUESTED) {}
	virtual const char *GetName()const{ return "ReconnectStateRequested"; }	
	virtual void Tick();
};

class CECReconnectStateReconnecting : public CECReconnectState{
	//	����������¼
public:
	CECReconnectStateReconnecting() : CECReconnectState(RS_RECONNECTING) {}
	virtual const char *GetName()const{ return "ReconnectStateReconnecting"; }	
	virtual bool CanRequestReconnect()const;
	virtual bool RequestReconnect();
	virtual bool IsReconnecting()const{ return true; }
	virtual bool IsReconnectFromGame()const;
	virtual bool ApplyServerSetting();
	virtual void UseLoginParameter();
	virtual void OnReconnectSuccess();
	virtual void OnReconnectFail();
};

//	��������
class CECReconnect{
	int								m_serverIndex;			//	�����ķ�������ַ
	int								m_roleID;				//	�����¼�����Ľ�ɫID

	CECReconnectPolicy*				m_reconnectPolicyCandidate;
	CECReconnectPolicy*				m_reconnectPolicy;

	bool							m_reconnectFromGame;	//	������������ʱ������Ϸ������

	CECReconnectState*				m_curState;

	CECReconnectStateNone			m_stateNone;
	CECReconnectStateCanRequest		m_stateCanRequest;
	CECReconnectStateRequested		m_stateRequested;
	CECReconnectStateReconnecting	m_stateReconnecting;
	
	//	����״̬����ȫȨ����
	friend class CECReconnectStateNone;
	friend class CECReconnectStateCanRequest;
	friend class CECReconnectStateRequested;
	friend class CECReconnectStateReconnecting;
	friend class CECReconnectStateReconnected;
	
	void SetServerIndexImpl(int serverIndex);
	void SetRoleIDImpl(int roleID);
	void SetReconnectPolicyCandidateImpl(CECReconnectPolicy *policy);
	void AcceptReconnectPolicyCandidateImpl();

	bool CanRequestReconnectImpl()const;
	bool RequestReconnectImpl();
	bool CanReconnectNowImpl()const;
	void ReconnectImpl();
	bool ApplyServerSettingImpl();
	void UseLoginParameterImpl();
	bool IsReconnectFromGameImpl()const;

	void ResetStateData();

	ELEMENTCLIENT_DECLARE_SINGLETON(CECReconnect);
public:
	~CECReconnect();

	//	�ַ���״̬��ķ���
	void SetServerIndex(int serverIndex);
	void SetRoleID(int roleID);
	void SetReconnectPolicyCandidate(CECReconnectPolicy *policy);
	void AcceptReconnectPolicyCandidate();
	void SetUserToken(const ACString &user, const ACString &token);

	bool CanRequestReconnect()const;
	bool RequestReconnect();
	void Tick();
	bool ApplyServerSetting();
	void UseLoginParameter();

	bool IsReconnecting()const;
	bool IsReconnectFromGame()const;

	void OnReconnectSuccess();
	void OnReconnectFail();

	//	���� Get ����
	int	 GetRoleIDToSelect()const{ return m_roleID; }
	int	 GetServerIndex()const{ return m_serverIndex; }
	bool HasReconnectPolicyCandidate()const{ return m_reconnectPolicyCandidate != NULL; }

	//	���� Set ����
	void ResetState();

private:	
	//	���� State ����ʹ�õķ���
	void ChangeState(CECReconnectState *state);

	CECReconnectState * GetStateNone(){ return &m_stateNone; }
	CECReconnectState * GetStateCanRequest(){ return &m_stateCanRequest; }
	CECReconnectState * GetStateRequested(){ return &m_stateRequested; }
	CECReconnectState * GetStateReconnecting(){ return &m_stateReconnecting; }
	
	const CECReconnectState * GetStateNone()const{ return &m_stateNone; }
	const CECReconnectState * GetStateCanRequest()const{ return &m_stateCanRequest; }
	const CECReconnectState * GetStateRequested()const{ return &m_stateRequested; }
	const CECReconnectState * GetStateReconnecting()const{ return &m_stateReconnecting; }
};

#endif