// File		: EC_LoginQueue.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/22

#include <windows.h>

#include "EC_LoginQueue.h"
#include "EC_LogicHelper.h"
#include "EC_GameSession.h"

#include "EC_UIHelper.h"
#include "EC_LoginUIMan.h"

#include "waitqueuestatenotify.hpp"
#include "cancelwaitqueue.hpp"
#include "cancelwaitqueue_re.hpp"


//	class CECLoginQueueChange
CECLoginQueueChange::CECLoginQueueChange(unsigned int changeMask)
: m_changeMask(changeMask)
{
}

unsigned int CECLoginQueueChange::ChangeMask()const{
	return m_changeMask;
}

bool CECLoginQueueChange::EnterQueue()const{
	return (ChangeMask() & ENTER_QUEUE) != 0;
}

bool CECLoginQueueChange::ChangeQueue()const{
	return (ChangeMask() & CHANGE_QUEUE) != 0;
}

bool CECLoginQueueChange::QuitQueue()const{
	return (ChangeMask() & QUIT_QUEUE) != 0;
}

bool CECLoginQueueChange::VIPQueueSizeChange()const{
	return (ChangeMask() & VIP_QUEUE_SIZE_CHANGE) != 0;
}

bool CECLoginQueueChange::CurrentQueuePositionChange()const{
	return (ChangeMask() & CURRENT_QUEUE_POSITION_CHANGE) != 0;
}

bool CECLoginQueueChange::EnterTimeChange()const{
	return (ChangeMask() & ENTER_TIME_CHANGE) != 0;
}

bool CECLoginQueueChange::QuitQueueFailed()const{
	return (ChangeMask() & QUIT_QUEUE_FAILED) != 0;
}

//	class CECLoginQueue
CECLoginQueue & CECLoginQueue::Instance(){
	static CECLoginQueue s_instance;
	return s_instance;
}

CECLoginQueue::CECLoginQueue(){
	Reset();
}

void CECLoginQueue::Reset(){
	m_inQueue = false;
	m_inVIPQueue = false;
	m_VIPQueueSize = 0;
	m_currentQueuePosition = 0;
	m_enterGameTimer.Reset(0);
}

void CECLoginQueue::NotifyChange(unsigned int changeMask){
	CECLoginQueueChange change(changeMask);
	NotifyObservers(&change);
}

void CECLoginQueue::ClearState(){
	if (!InQueue()){
		return;
	}
	a_LogOutput(1, "CECLoginQueue::ClearState");
	Reset();
	NotifyChange(CECLoginQueueChange::QUIT_QUEUE);
}

bool CECLoginQueue::InQueue()const{
	return m_inQueue;
}

bool CECLoginQueue::InVIPQueue()const{
	return InQueue() && m_inVIPQueue;
}

bool CECLoginQueue::InNormalQueue()const{
	return InQueue() && !m_inVIPQueue;
}

int  CECLoginQueue::VIPQueueSize()const{
	return m_VIPQueueSize;
}

int  CECLoginQueue::CurrentQueuePosition()const{
	return m_currentQueuePosition;
}

int  CECLoginQueue::LeftEnterTime()const{
	return m_enterGameTimer.LeftTime();
}

bool CECLoginQueue::QuitQueue(){
	if (!InQueue()){
		return false;
	}
	a_LogOutput(1, "CECLoginQueue::QuitQueue");
	CECLogicHelper::GetGameSession()->QuitLoginQueue();
	return true;
}

void CECLoginQueue::OnPrtcWaitQueueStateNotify(const GNET::WaitQueueStateNotify *p){
	a_LogOutput(1, "CECLoginQueue::OnPrtcWaitQueueStateNotify,vip=%d,vq_size=%d,cur_queue_pos=%d,waittime=%d",p->vip,p->vq_size,p->cur_queue_pos,p->waittime);

	//	检查网络连接状态，断开链接后仍有缓冲协议导致排队状态不正确的可能性，确保同步
	if (!CECLogicHelper::GetGameSession()->IsConnected()){
		ClearState();
		return;
	}

	if (!InQueue()){
		m_inQueue = true;
		m_inVIPQueue = (p->vip != 0);
		m_VIPQueueSize = p->vq_size;
		m_currentQueuePosition = p->cur_queue_pos;
		m_enterGameTimer.Reset(p->waittime*1000);
		NotifyChange(CECLoginQueueChange::ENTER_QUEUE);
	}else{
		unsigned int changeMask(0);
		if (m_inVIPQueue != (p->vip != 0)){
			m_inVIPQueue = (p->vip != 0);
			changeMask |= CECLoginQueueChange::CHANGE_QUEUE;
		}
		if (m_VIPQueueSize != p->vq_size){
			m_VIPQueueSize = p->vq_size;
			changeMask |= CECLoginQueueChange::VIP_QUEUE_SIZE_CHANGE;
		}
		if (m_currentQueuePosition != p->cur_queue_pos){
			m_currentQueuePosition = p->cur_queue_pos;
			changeMask |= CECLoginQueueChange::CURRENT_QUEUE_POSITION_CHANGE;
		}
		if (m_enterGameTimer.LeftTime() != p->waittime*1000){
			m_enterGameTimer.Reset(p->waittime*1000);
			changeMask |= CECLoginQueueChange::ENTER_TIME_CHANGE;
		}
		NotifyChange(changeMask);
	}
}

void CECLoginQueue::OnPrtcCancelWaitQueue_Re(const GNET::CancelWaitQueue_Re *p){
	a_LogOutput(1, "CECLoginQueue::OnPrtcCancelWaitQueue_Re,result=%d", p->result);
	if (p->result == ERR_SUCCESS){
		ClearState();
		if (CECLoginUIMan *pLoginUIMan = CECUIHelper::GetLoginUIMan()){
			pLoginUIMan->ReselectRole();
		}
	}else{
		NotifyChange(CECLoginQueueChange::QUIT_QUEUE_FAILED);
	}
}
