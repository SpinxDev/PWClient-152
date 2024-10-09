#ifndef __GNET__GAMECLIENT
#define __GNET__GAMECLIENT

#include "gnet.h"
#include "netclient.h"
#include "gnconf.h"
#include <AString.h>

namespace GNET
{
	union Attr{
		unsigned int _attr;
		struct {
			unsigned char load;
			unsigned char lambda;
			unsigned char anything;
			unsigned char doubleExp:1;
            unsigned char doubleMoney:1;
            unsigned char doubleObject:1;
	        unsigned char doubleSP:1;
			unsigned char freeZone:1;
			unsigned char sellPoint:1;
			unsigned char battle:1;
		};
	};

	typedef struct _ServerStatus
	{
		DWORD ping;
		union Attr attr;
		unsigned int createtime;
		unsigned char exp_rate;
	}ServerStatus;

	class GameClient: public NetClient, public Timer::Observer
	{
		static GameClient instance;

		void* m_pGameSession;	// ËùÊôµÄGameSession
		char m_zoneid;
		Session::ID m_sid;
		Thread::Mutex locker_ping;
		std::map<Session::ID, DWORD> pingmap;

		int m_state;
		unsigned int m_ping;
		DWORD m_lastsend;
		DWORD m_lastrecv;

		bool m_bIsUseUsbKey;

		mutable SockAddr m_lastAddr;

		enum STATE{
			STATE_CLOSED,
			STATE_CONNECTING,
			STATE_PINGING,
			STATE_KEEPING,
			STATE_CLOSING,
		};

	public:
		typedef int(* callback_t)(void*, void*, Session::ID, int);

		GameClient(void* pGameSession=NULL) : m_pGameSession(pGameSession), m_callback(NULL), m_state(STATE_CLOSED), m_ping(100), m_counter(0)	
		{
			Timer::Attach(this);
		}

		~GameClient()
		{
			Timer::Detach(this);
		}

		static GameClient& GetInstance() 
		{ 
			instance.blCompress = (atoi(Conf::GetInstance()->find("GameClient","compress").c_str())==1);
			return instance;
		}

		bool InputPolicy(Protocol::Type type, size_t len) const { return true; }
		const Session::State *GetInitState() const { return &state_GLoginClient;}
		std::string Identification() const { return "SClient"; }

		void Attach(callback_t funptr) { m_callback = funptr; }
		void Detach() { m_callback = NULL; }
		void SetZoneID(char zoneid) { m_zoneid = zoneid; }
		char GetZoneID() const { return m_zoneid; }

		void* GetGameSession() const { return m_pGameSession; }

		void OnAddSession(Session::ID sid) 
		{
			if(m_state==STATE_PINGING){
				Thread::Mutex::Scoped l(locker_ping);
				pingmap[sid] = GetTickCount();
			}
			m_sid = sid;
			if(m_callback&&m_state==STATE_CONNECTING)
				m_callback(m_pGameSession, NULL, sid, EVENT_ADDSESSION);
		}

		void OnDelSession(Session::ID sid) 
		{
			if(m_callback)
			{
				if(sid==m_sid && m_state==STATE_KEEPING)
				{
					m_callback(m_pGameSession, NULL, sid, EVENT_DELSESSION);
					m_state = STATE_CLOSED;
				}
				if(m_state!=STATE_PINGING)
					m_callback(m_pGameSession, NULL, sid, EVENT_DISCONNECT);
			}
		}

		void OnAbortSession(Session::ID sid) 
		{ 
			if(m_callback && m_state==STATE_CONNECTING)
				m_callback(m_pGameSession, NULL, sid, EVENT_ABORTSESSION);
		}

		int Connect(const char* host, unsigned short port)
		{
			if(m_state!=STATE_CLOSED)
				Disconnect();
			m_state = STATE_CONNECTING;
			ConnectTo(host, port);
			return m_sid;
		}
		int Ping(struct in_addr *host, unsigned short port)
		{
			m_state = STATE_PINGING;
			ConnectTo(host, port);
			return m_sid;
		}
		void Disconnect()
		{
			if(Close(m_sid))
				m_state = STATE_CLOSED;
			else
				m_state = STATE_CLOSING;
		}
		void OnRecvProtocol(Session::ID sid, Protocol* pdata);
		void Update();

		bool ChangeState(Session::ID id, Session::State *state)
		{
			if(state==&state_GSelectRoleClient)
			{
				m_state = STATE_KEEPING;
				Thread::Mutex::Scoped l(locker_ping);
				pingmap.clear();
			}
			return Manager::ChangeState(id,state);
		}

		unsigned int GetPing()
		{
			if(m_ping > 9999)
				return 9999;
			return m_ping>5?m_ping:5;
		}

		bool IsUseUsbKey() { return m_bIsUseUsbKey; }
		bool IsUsbKeyExisting();
		const char* GetLastError();
		
		struct LoginParameter
		{
			AString	name;
			AString passwd;
			Octets	nonce;
			char		algo;
			char		use_token;
			AString	token;
		};
		void SetupResponse(Response&, LoginParameter &);
		
		virtual void OnCheckAddress(SockAddr &addr) const{
			m_lastAddr = addr;
		}
		const SockAddr & GetLastAddr()const{
			return m_lastAddr;
		}
	protected:
		int ConnectTo(const char* host, unsigned short port);
		int ConnectTo(struct in_addr *, unsigned short port);
		callback_t m_callback;
		unsigned long m_counter;
	};
};
#endif
