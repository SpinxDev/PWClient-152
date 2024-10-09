#ifndef __GNET_GTRANSFORMCLIENT_HPP
#define __GNET_GTRANSFORMCLIENT_HPP

#include <windows.h>
#include <gnproto.h>
#include <gntimer.h>

namespace GNET
{
	class GTransformClient : public Protocol::Manager, public Timer::Observer
	{
		size_t tick;
		void Update();

		static GTransformClient instance;
		size_t		accumulate_limit;
		Session::ID	sid;
		bool		conn_state;
		Thread::Mutex	locker_state;
		enum { BACKOFF_INIT = 2, BACKOFF_DEADLINE = 64 };
		size_t		backoff;
		void Reconnect();
		const Session::State *GetInitState() const;
		bool OnCheckAccumulate(size_t size) const { return accumulate_limit == 0 || size < accumulate_limit; }
		void OnAddSession(Session::ID sid);
		void OnDelSession(Session::ID sid);
		void OnAbortSession(Session::ID sid);
		void OnCheckAddress(SockAddr &) const;
		void OnRecvProtocol(Session::ID sid, Protocol* pdata);
	public:
		static GTransformClient *GetInstance() { return &instance; }
		std::string Identification() const { return "GTransformClient"; }
		void SetAccumulate(size_t size) { accumulate_limit = size; }
		GTransformClient() : accumulate_limit(0), sid(0), conn_state(false), locker_state(false) ,backoff(BACKOFF_INIT),tick(0){
			Timer::Attach(this);
		}
		
		bool SendProtocol(const Protocol &protocol) { return conn_state && Send(sid, protocol); }
		bool SendProtocol(const Protocol *protocol) { return conn_state && Send(sid, protocol); }

		bool GetConnectState()const{ return conn_state;}
	};

	class GKeepAlive : public Timer::Observer
	{
	public:

		GKeepAlive() : m_tick(0)
		{
			Timer::Attach(this);
		}

		void Update();

	private:

		enum {KEEP_ALIVE_COUNTER = 60};
		int m_tick;

		static GKeepAlive instance;
	};
	

};
#endif
