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
//	类 CECLoginQueue 数据改变通知类型
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

//	定义 CECLoginQueue 的观察者
class CECLoginQueue;
typedef CECObserver<CECLoginQueue>	CECLoginQueueObserver;

//	登录等待队列数据模型
class CECLoginQueue : public CECObservable<CECLoginQueue>
{
private:
	bool				m_inQueue;				//	处于队列中
	bool				m_inVIPQueue;			//	当前处于VIP队列
	int					m_VIPQueueSize;			//	VIP队列当前人数
	int					m_currentQueuePosition;	//	当前队列排名（从1开始）
	CECTimeSafeChecker	m_enterGameTimer;		//	进入游戏计时器

	CECLoginQueue();

	//	单例禁用
	CECLoginQueue(const CECLoginQueue &);
	CECLoginQueue & operator = (const CECLoginQueue &);

	void Reset();
	void NotifyChange(unsigned int changeMask);

public:

	static CECLoginQueue &Instance();

	void ClearState();

	//	状态查询
	bool InQueue()const;
	bool InVIPQueue()const;
	bool InNormalQueue()const;
	int  VIPQueueSize()const;
	int  CurrentQueuePosition()const;
	int  LeftEnterTime()const;

	//	发往服务器：取消排队
	bool QuitQueue();

	//	数据改变
	void OnPrtcWaitQueueStateNotify(const GNET::WaitQueueStateNotify *p);
	void OnPrtcCancelWaitQueue_Re(const GNET::CancelWaitQueue_Re *p);
};
