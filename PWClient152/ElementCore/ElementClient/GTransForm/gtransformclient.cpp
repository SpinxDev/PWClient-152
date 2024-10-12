
#include "gtransformclient.hpp"
#include "state.hxx"
#include "keepalive.hpp"
#include "toolannounceaid.hpp"
#include "../EC_Global.h"

namespace GNET
{	
	GTransformClient GTransformClient::instance;
	
	void GTransformClient::Update()
	{
		Thread::Mutex::Scoped l(locker_state);
		if(tick <= 0) return;
		if(--tick <=0 )
		{
			Log_Info("reconnecting(current sid = %d, conn_state = %d)...", this->sid, this->conn_state);
			Protocol::Client(GTransformClient::GetInstance());
		}
	}
	
	void GTransformClient::Reconnect()
	{
		Thread::Mutex::Scoped l(locker_state);
		backoff *= 2;
		if (backoff > BACKOFF_DEADLINE) backoff = BACKOFF_DEADLINE;
		tick = backoff;
	}
	
	
	const Protocol::Manager::Session::State* GTransformClient::GetInitState() const
	{
		return &state_GTransformClient;
	}
	
	void GTransformClient::OnAddSession(Session::ID sid)
	{
		Thread::Mutex::Scoped l(locker_state);
		if (conn_state)
		{
			Log_Info("new session(sid = %d) refused(current sid = %d, conn_state = %d).", sid, this->sid, this->conn_state);
			Close(sid);
			return;
		}
		this->conn_state = true;
		this->sid = sid;
		backoff = BACKOFF_INIT;
		Log_Info("connected(sid = %d).", this->sid);
		//TODO
		
		//	连接后即发送小工具对应游戏的aid，表明小工具身份
		const int WMGJ_AID = 10;
		ToolAnnounceAid aid(WMGJ_AID);
		GTransformClient::GetInstance()->SendProtocol(aid);
		Log_Info("ToolAnnounceAid(%d) sent...", WMGJ_AID);
	}
	
	void GTransformClient::OnDelSession(Session::ID sid)
	{
		Thread::Mutex::Scoped l(locker_state);
		Log_Info("session deleted(current sid = %d, conn_state = %d).", this->sid, this->conn_state);
		this->conn_state = false;
		this->sid = 0;
		//TODO
		Reconnect();
	}
	
	void GTransformClient::OnAbortSession(Session::ID sid)
	{
		Thread::Mutex::Scoped l(locker_state);
		Log_Info("session aborted(current sid = %d, conn_state = %d).", this->sid, this->conn_state);
		this->conn_state = false;
		this->sid = 0;
		//TODO
		Reconnect();
	}
	
	void GTransformClient::OnCheckAddress(SockAddr &sa) const
	{
		//TODO
	}
	
	void GTransformClient::OnRecvProtocol(Session::ID sid, Protocol* pdata)
	{
		pdata->Process(this, sid);
	}

	GKeepAlive GKeepAlive::instance;

	void GKeepAlive::Update()		
	{
		// 每隔一定时间发送 KEEP ALIVE 协议
		//
		if (++m_tick >= KEEP_ALIVE_COUNTER)
		{
			m_tick = 0;
			
			// 发送 KEEPALIVE 协议
			if (GTransformClient::GetInstance()->GetConnectState())
			{
				// 连接状态时发送 KEEPALIVE 协议
				KeepAlive ka;
				GTransformClient::GetInstance()->SendProtocol(ka);
				
				// 输出显示到界面
				Log_Info("Keep alive sent...");
			}
		}
	}
};
