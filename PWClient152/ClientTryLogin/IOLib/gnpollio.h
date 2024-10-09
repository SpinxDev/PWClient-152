#ifndef __POLLIO_H
#define __POLLIO_H

#include <map>
#include <vector>
#include <algorithm>
#include <functional>

#include "gnthread.h"
#include "gntimer.h"

#define IO_POLLIN  0x0001
#define IO_POLLOUT 0x0004
#define IO_POLLERR 0x0008

namespace GNET
{

class PollIO
{
	typedef std::map<SOCKET, PollIO*> IOMap;
	typedef std::vector<PollIO *> FDSet;
	static fd_set rfds, wfds, efds;

	static IOMap iomap;
	static FDSet fdset;
	static IOMap ionew;
	static Thread::Mutex locker_ionew;
	static Thread::Mutex locker_poll;
	static bool wakeup_scope;
	static bool wakeup_flag;


	virtual int UpdateEvent() = 0;
	virtual void PollIn()  { }
	virtual void PollOut() { }
	virtual void PollErr() { }

	static void UpdateEvent(const IOMap::value_type iopair)
	{
		PollIO *io = iopair.second;

		if (io == NULL)
			return;

		int event = io->UpdateEvent();

		if (event == -1)
		{
			iomap[io->fd] = NULL;
			delete io;
			return;
		}

		if (event)
		{
			if (event & IO_POLLIN ) FD_SET(io->fd, &rfds);
			if (event & IO_POLLOUT) FD_SET(io->fd, &wfds);
			if (event & IO_POLLERR) FD_SET(io->fd, &efds);
			fdset.push_back(io);
		}
	}

	static void TriggerEvent(PollIO *io)
	{
		if (FD_ISSET(io->fd, &rfds))
			io->PollIn();
		if (FD_ISSET(io->fd, &wfds))
			io->PollOut();
		if (FD_ISSET(io->fd, &efds))
			io->PollErr();

	}

protected:
	SOCKET fd;

	virtual ~PollIO() { }

	PollIO(SOCKET x) : fd(x)
	{
		unsigned long n = 1;
		ioctlsocket(fd, FIONBIO , &n);
		ionew[fd] = this;
	}

public:	
	static void Init();
	static void Close();
	static void ShutDown();

	static void Poll(int timeout)
	{
		Thread::Mutex::Scoped l(locker_poll);
		{
			Thread::Mutex::Scoped l(locker_ionew);
			
			for(IOMap::const_iterator i = ionew.begin(); i != ionew.end(); ++i)
				iomap[(*i).first] = (*i).second;
			ionew.clear();

		}
		fdset.clear();
		wakeup_flag  = false;
		for(std::map<SOCKET, PollIO*>::iterator it=iomap.begin(); it!=iomap.end(); ++it)
			UpdateEvent(*it);			
		wakeup_scope = true;
		if (wakeup_flag)
		{
			wakeup_scope = false;
			wakeup_flag  = false;
			timeout = 0;
		}
		int nevents;		
		if (timeout < 0){
			nevents = select(0, &rfds, &wfds, &efds, NULL);
		}
		else
		{
			struct timeval tv;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout - (tv.tv_sec * 1000)) * 1000;
			nevents = select(0, &rfds, &wfds, &efds, &tv);
		}
		wakeup_scope = false;
		if (nevents > 0)
			std::for_each(fdset.begin(), fdset.end(), std::ptr_fun(&TriggerEvent));
		FD_ZERO(&rfds); 
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
	}

	class Task : public Thread::Runnable
	{
		Task() { }
		void Run()
		{
			PollIO::Poll(1000);
			Timer::Update();
			if(!Thread::Pool::AddTask(this))
			{
				PollIO::ShutDown();
			}
		}
	public:
		static Task *GetInstance();
	};
	
	static void WakeUp();
	static Thread::Mutex& LockerNew() { return locker_ionew; }
};

class PollControl : public PollIO
{
	friend class PollIO;
	static SOCKET writer;

	int UpdateEvent() { return IO_POLLIN; }
	void PollIn()  
	{ 
		for(char buff[256]; recv(fd, buff, 256, 0)==256; );
	}
	PollControl(int r ) : PollIO(r) { writer = r;}

	~PollControl()
	{
		closesocket(fd);
	}

	static void WakeUp() { 
		send(writer, "", 1, 0); 
	}

	static void Init()
	{
		unsigned long n = 1;
		SOCKET s = socket(AF_INET,SOCK_DGRAM,0);

		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr("127.0.0.1");
		sin.sin_port = htons(0);

		bind(s,(LPSOCKADDR)&sin,sizeof(sin));
		struct sockaddr addr;
		int len = sizeof(struct sockaddr);
		getsockname(s,&addr, &len);

		connect(s, (LPSOCKADDR)&addr, len);
		ioctlsocket(s, FIONBIO , &n);

		new PollControl(s);

	}
};

inline void PollIO::WakeUp() 
{ 
	if (wakeup_scope)
		PollControl::WakeUp(); 
	else
		wakeup_flag = true;
}

};

#endif
