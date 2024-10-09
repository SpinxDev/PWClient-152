#ifndef __GNET_GTRANSFORMSERVER_HPP
#define __GNET_GTRANSFORMSERVER_HPP

#include <windows.h>
#include <gnproto.h>

namespace GNET
{

class GTransformServer : public Protocol::Manager
{
	static GTransformServer instance;
	size_t		accumulate_limit;
	Session::ID	sid;
	bool		conn_state;
	Thread::Mutex	locker_state;
	const Session::State *GetInitState() const;
	bool OnCheckAccumulate(size_t size) const { return accumulate_limit == 0 || size < accumulate_limit; }
	void OnAddSession(Session::ID sid);
	void OnDelSession(Session::ID sid);
	void OnAbortSession(const SockAddr &sa);
	void OnCheckAddress(SockAddr &) const;
	void OnRecvProtocol(Session::ID sid, Protocol* pdata);
public:
	static GTransformServer *GetInstance() { return &instance; }
	std::string Identification() const { return "GTransformServer"; }
	void SetAccumulate(size_t size) { accumulate_limit = size; }
	GTransformServer() : accumulate_limit(0), conn_state(false), locker_state(false) { }

	bool SendProtocol(const Protocol &protocol) { return conn_state && Send(sid, protocol); }
	bool SendProtocol(const Protocol *protocol) { return conn_state && Send(sid, protocol); }
};

};
#endif
