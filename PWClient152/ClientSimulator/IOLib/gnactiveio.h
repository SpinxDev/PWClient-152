#ifndef __ACTIVEIO_H
#define __ACTIVEIO_H

#include "gnconf.h"
#include "gnpollio.h"
#include "gnnetio.h"

namespace GNET
{
class ActiveIO : PollIO
{
	enum Type { STREAM, DGRAM, DGRAM_BROADCAST };
	enum Status { CONNECTING, CONNECTED, ABORTING };
	Type type;
	int closing;
	NetSession *assoc_session;
	SockAddr sa;

	int UpdateEvent()
	{
		return (closing!=CONNECTING) ? -1 : (IO_POLLOUT | IO_POLLERR);
	}

	void PollOut()
	{
		Close(CONNECTED);
	}

	void PollErr()
	{
		Close(ABORTING);
	}

	ActiveIO(SOCKET x, const SockAddr &saddr, NetSession *s, Type t) :
		PollIO(x), type(t), closing(t == STREAM ? CONNECTING:CONNECTED), assoc_session(s), sa(saddr)
	{
		assoc_session->LoadConfig();
		if (type != DGRAM_BROADCAST) 
			connect(fd, sa, sa.GetLen());
		PollIO::WakeUp();
	}
public:
	~ActiveIO()
	{
		if (type == STREAM)
		{
			if (closing==CONNECTED)
			{
				{
					Thread::Mutex::Scoped l(PollIO::LockerNew());
					new StreamIO(fd, assoc_session);
				}
				PollIO::WakeUp();
			}
			else
			{
				assoc_session->OnAbort();
				assoc_session->Destroy();
				closesocket(fd);
			}
		}
		else
		{
			Thread::Mutex::Scoped l(PollIO::LockerNew());
			new DgramClientIO(fd, assoc_session, sa);
        }
	}

	const NetSession * GetSession() { return assoc_session; }

	void Close(int code) { closing = code; }
	SOCKET GetSocket() { return fd; }

	static ActiveIO *Open(NetSession *assoc_session) 
	{
		Conf *conf = Conf::GetInstance();
		Conf::section_type section = assoc_session->Identification();
		Conf::value_type type = conf->find(section, "type");
		SockAddr sa;

		SOCKET s;
		int optval = 1;
		Type t;
		if (!stricmp(type.c_str(), "tcp"))
		{
			t = STREAM;
			struct sockaddr_in *addr = sa;
			memset(addr, 0, sizeof(*addr));
			s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			addr->sin_family = AF_INET;
			addr->sin_addr.s_addr = inet_addr(conf->find(section, "address").c_str());
			addr->sin_port = htons(atoi(conf->find(section, "port").c_str()));
			assoc_session->OnCheckAddress(sa);

			setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(optval));

			optval = atoi(conf->find(section, "so_sndbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));
			optval = atoi(conf->find(section, "so_rcvbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
			optval = atoi(conf->find(section, "tcp_nodelay").c_str());
			if (optval) setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
		}
		else if (!stricmp(type.c_str(), "udp"))
		{
			struct sockaddr_in *addr = sa;
			memset(addr, 0, sizeof(*addr));
			s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			addr->sin_family = AF_INET;
			addr->sin_addr.s_addr = inet_addr(conf->find(section, "address").c_str());
			addr->sin_port = htons(atoi(conf->find(section, "port").c_str()));
			assoc_session->OnCheckAddress(sa);

			optval = atoi(conf->find(section, "so_broadcast").c_str());
			if (optval)
			{
				setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval));
				t = DGRAM_BROADCAST;
			}
			else
				t = DGRAM;
			optval = atoi(conf->find(section, "so_sndbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));
			optval = atoi(conf->find(section, "so_rcvbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
		}
		else
			s = INVALID_SOCKET;
		Thread::Mutex::Scoped l(PollIO::LockerNew());
		return s != INVALID_SOCKET ? new ActiveIO(s, sa, assoc_session, t) : NULL;
	}

	static ActiveIO *Open(NetSession *assoc_session, const char* host, unsigned short port) 
	{
		Conf *conf = Conf::GetInstance();
		Conf::section_type section = assoc_session->Identification();
		SockAddr sa;

		SOCKET s;
		int optval = 1;
		Type t;
		t = STREAM;
		struct sockaddr_in *addr = sa;
		memset(addr, 0, sizeof(*addr));
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr(host);
		addr->sin_port = htons(port);
		assoc_session->OnCheckAddress(sa);

		setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(optval));

		//optval = atoi(conf->find(section, "so_sndbuf").c_str());
		//if (optval) setsockopt(s, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval));
		//if (optval) setsockopt(s, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));
		//optval = atoi(conf->find(section, "tcp_nodelay").c_str());
		optval = 8192;
		setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));
		optval = 8192;
		setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
		if (optval) setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
		Thread::Mutex::Scoped l(PollIO::LockerNew());
		return s != INVALID_SOCKET ? new ActiveIO(s, sa, assoc_session, t) : NULL;
	}
};

};

#endif
