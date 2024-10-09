#ifndef __GNET__QQCLIENT
#define __GNET__QQCLIENT

#include "gnet.h"
#include "netclient.h"

namespace GNET
{
	class QQClient: public NetClient
	{
		static QQClient instance;

	public:
		typedef int(* callback_t)(Protocol*, Session::ID, int);
		static QQClient& GetInstance() { return instance;}
		void Attach(callback_t funptr) 
		{
			m_callback = funptr;
		}

		QQClient() : m_callback(NULL){}	

		void OnAddSession(Session::ID sid) 
		{
			if(m_callback)
				m_callback(NULL, sid, EVENT_ADDSESSION);
		}

		void OnDelSession(Session::ID sid) 
		{
			if(m_callback)
				m_callback(NULL, sid, EVENT_DELSESSION);
		}

		void OnAbortSession(Session::ID sid) 
		{ 
			if(m_callback)
				m_callback(NULL, sid, EVENT_ABORTSESSION);
		}

		void OnRecvProtocol(Session::ID sid, Protocol* pdata) 
		{
			if(m_callback)
				m_callback(pdata, sid, WM_IOPROTOCOL);
		} 

		bool InputPolicy(Protocol::Type type, size_t len) const { return true; }
		const Session::State *GetInitState() const { return &state_LoginClient;}
		std::string Identification() const { return "QQClient"; }

	protected:
		callback_t m_callback;
	};

};
#endif
