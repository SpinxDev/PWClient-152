// File		: EC_LoginQueue.h
// Creator	: Xu Wenbin
// Date		: 2015/1/22

#pragma once

#include "EC_Observer.h"
#include "EC_TimeSafeChecker.h"

namespace GNET
{
	class WaitQueueStateNotify;
	class CancelWaitQueue_Re;
}

//	class CECLoginQueueChange
//	�� CECLoginQueue ���ݸı�֪ͨ����
class CECLoginQueueChange : public CECObservableChange
{
public:
	enum ChangeMask{
		ENTER_QUEUE					=	0x01,
		CHANGE_QUEUE				=	0x02,
		QUIT_QUEUE					=	0x04,
		VIP_QUEUE_SIZE_CHANGE		=	0x08,
		CURRENT_QUEUE_POSITION_CHANGE	=	0x10,
		ENTER_TIME_CHANGE			=	0x20,
		QUIT_QUEUE_FAILED			=	0x40,
	};
private:
	unsigned int	m_changeMask;
public:
	CECLoginQueueChange(unsigned int changeMask);
	unsigned int ChangeMask()const;
	bool EnterQueue()const;
	bool ChangeQueue()const;
	bool QuitQueue()const;
	bool VIPQueueSizeChange()const;
	bool CurrentQueuePositionChange()const;
	bool EnterTimeChange()const;
	bool QuitQueueFailed()const;
};

//	���� CECLoginQueue �Ĺ۲���
class CECLoginQueue;
typedef CECObserver<CECLoginQueue>	CECLoginQueueObserver;

//	��¼�ȴ���������ģ��
class CECLoginQueue : public CECObservable<CECLoginQueue>
{
private:
	bool				m_inQueue;				//	���ڶ�����
	bool				m_inVIPQueue;			//	��ǰ����VIP����
	int					m_VIPQueueSize;			//	VIP���е�ǰ����
	int					m_currentQueuePosition;	//	��ǰ������������1��ʼ��
	CECTimeSafeChecker	m_enterGameTimer;		//	������Ϸ��ʱ��

	CECLoginQueue();

	//	��������
	CECLoginQueue(const CECLoginQueue &);
	CECLoginQueue & operator = (const CECLoginQueue &);

	void Reset();
	void NotifyChange(unsigned int changeMask);

public:

	static CECLoginQueue &Instance();

	void ClearState();

	//	״̬��ѯ
	bool InQueue()const;
	bool InVIPQueue()const;
	bool InNormalQueue()const;
	int  VIPQueueSize()const;
	int  CurrentQueuePosition()const;
	int  LeftEnterTime()const;

	//	������������ȡ���Ŷ�
	bool QuitQueue();

	//	���ݸı�
	void OnPrtcWaitQueueStateNotify(const GNET::WaitQueueStateNotify *p);
	void OnPrtcCancelWaitQueue_Re(const GNET::CancelWaitQueue_Re *p);
};
