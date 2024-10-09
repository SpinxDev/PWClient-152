#ifndef __PASSIVEIO_H
#define __PASSIVEIO_H

#include <sys/types.h>

#include "gnconf.h"
#include "gnpollio.h"
#include "gnnetio.h"

namespace GNET
{

class PassiveIO : PollIO
{
	enum Type { STREAM, DGRAM };
	NetSession *assoc_session;
	Type type;
	bool closing;

	int UpdateEvent()
	{
		return closing ? -1 : IO_POLLIN;
	}

	void PollIn()
	{
		if (type == STREAM)
		{
			SOCKET s = accept(fd, 0, 0);
			if (s != INVALID_SOCKET)
			{
				Thread::Mutex::Scoped l(PollIO::LockerNew());
				new StreamIO(s, assoc_session->Clone());
			}
		}
		else Close();
	}

	PassiveIO (SOCKET x, NetSession *y, Type t) : 
		PollIO(x), assoc_session(y), type(t), closing(false)
	{
		assoc_session->LoadConfig();
	}
public:
	virtual ~PassiveIO ()
	{
		if (type == STREAM)
		{
			assoc_session->Destroy();
			closesocket(fd);
		}
		else 
		{
			{
				Thread::Mutex::Scoped l(PollIO::LockerNew());
				new DgramServerIO(fd, assoc_session);
			}
			PollIO::WakeUp();
		}
	}

	void Close() { closing = true; }

	static PassiveIO *Open(NetSession *assoc_session)
	{
		Conf *conf = Conf::GetInstance();
		Conf::section_type section = assoc_session->Identification(); 
		Conf::value_type type = conf->find(section, "type");
		SockAddr sa;

		SOCKET s;
		char optval = 1;
		Type t;
		if (!stricmp(type.c_str(), "tcp"))
		{
			t = STREAM;
			s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			struct sockaddr_in *addr = sa;
			memset(addr, 0, sizeof(*addr));
			addr->sin_family = AF_INET;
			if ( (addr->sin_addr.s_addr = inet_addr(conf->find(section, "address").c_str())) == INADDR_NONE)
				addr->sin_addr.s_addr = INADDR_ANY;
			addr->sin_port = htons(atoi(conf->find(section, "port").c_str()));
			assoc_session->OnCheckAddress(sa);

			setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
			setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

			optval = atoi(conf->find(section, "so_sndbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval));
			optval = atoi(conf->find(section, "so_rcvbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));
			optval = atoi(conf->find(section, "tcp_nodelay").c_str());
			if (optval) setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));

			bind (s, sa, sa.GetLen());
			optval = atoi(conf->find(section, "listen_backlog").c_str());
			listen (s, optval ? optval : SOMAXCONN);
		}
		else if (!stricmp(type.c_str(), "udp"))
		{
			t = DGRAM;
			s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			struct sockaddr_in *addr = sa;
			memset(addr, 0, sizeof(*addr));
			addr->sin_family = AF_INET;
			if ( (addr->sin_addr.s_addr = inet_addr(conf->find(section, "address").c_str())) == INADDR_NONE)
				addr->sin_addr.s_addr = INADDR_ANY;
			addr->sin_port = htons(atoi(conf->find(section, "port").c_str()));
			assoc_session->OnCheckAddress(sa);

			setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

			optval = atoi(conf->find(section, "so_broadcast").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
			optval = atoi(conf->find(section, "so_sndbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval));
			optval = atoi(conf->find(section, "so_rcvbuf").c_str());
			if (optval) setsockopt(s, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));

			bind (s, sa, sa.GetLen());
		}
		else
			s = INVALID_SOCKET;

		Thread::Mutex::Scoped l(PollIO::LockerNew());
		return s == INVALID_SOCKET ? NULL : new PassiveIO(s, assoc_session, t);
	}
};

};

#endif
