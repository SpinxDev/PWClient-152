
#include "gtransformserver.hpp"
#include "state.hxx"
namespace GNET
{

GTransformServer GTransformServer::instance;

const Protocol::Manager::Session::State* GTransformServer::GetInitState() const
{
	return &state_GTransformServer;
}

void GTransformServer::OnAddSession(Session::ID sid)
{
	Thread::Mutex::Scoped l(locker_state);
	if (conn_state)
	{
		Close(sid);
		return;
	}
	conn_state = true;
	this->sid = sid;
	printf("connected...\n");
	//TODO
}

void GTransformServer::OnDelSession(Session::ID sid)
{
	Thread::Mutex::Scoped l(locker_state);
	conn_state = false;
	printf("session deleted.\n");
	//TODO
}

void GTransformServer::OnAbortSession(const SockAddr &sa)
{
	Thread::Mutex::Scoped l(locker_state);
	conn_state = false;
	printf("session aborted.\n");
	//TODO
}

void GTransformServer::OnCheckAddress(SockAddr &sa) const
{
	//TODO
}

void GTransformServer::OnRecvProtocol(Session::ID sid, Protocol* pdata)
{
	pdata->Process(this, sid);
}

};
