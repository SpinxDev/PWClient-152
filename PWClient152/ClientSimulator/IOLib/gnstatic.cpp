#include <winsock2.h>

#include "gnconf.h"
#include "gnrpc.h"
#include "gnsecure.h"
#include "gnthread.h"
#include "gncompress.h"
#include "streamcompress.h"
#include "../Common.h"

extern unsigned long HandleException(LPEXCEPTION_POINTERS pExceptionPointers);

namespace Thread{
	// Thread
	Mutex           Pool::s_mutex_tasks;
	Semaphore       Pool::s_cond_tasks;
	Pool::TaskQueue Pool::s_tasks;   

	size_t  Pool::s_thread_count = 0;
	int     Pool::pool_state = stateNormal;

	void __cdecl Pool::RunThread( void * id)
	{
		__try{
			while (true)
			{
				Runnable * pTask = NULL;
				{
					s_cond_tasks.Wait(s_mutex_tasks);
					s_mutex_tasks.Lock();
					if(pool_state == stateQuit)
					{
						s_mutex_tasks.UNLock();
						break;
					}

					if(s_tasks.empty()){
						s_mutex_tasks.UNLock();
						continue;
					}
					pTask = s_tasks.front();
					s_tasks.pop_front();
					s_mutex_tasks.UNLock();
				}
				if(pTask)
					pTask->Run();
				else
					break;
			}
			s_thread_count--;
		}
		__except (glb_HandleException(GetExceptionInformation()))
		{
			::OutputDebugString(_AL("Exception occurred in gnet...\n"));
			a_LogOutput(1, "Exception occurred gnet... mini dumped!");
			::ExitProcess(-1);
		}
	}

};

namespace GNET
{
	extern void	SHA256_Init  ( SHA256_ALG_INFO	*AlgInfo);
	extern void SHA256_Update( SHA256_ALG_INFO	*AlgInfo, unsigned char	*Message,unsigned int MessageLen);
	extern void SHA256_Final ( SHA256_ALG_INFO	*AlgInfo, unsigned char *Digest);	

	// Conf
	Thread::RWLock Conf::locker;
	Conf  Conf::instance;

	// Protocol
	ProtocolBinder    __ProtocolBinder_stub(PROTOCOL_BINDER);
	CompressBinder    __CompressBinder_stub(PROTOCOL_COMPRESSBINDER);
	Protocol::Manager::Session::ID   Protocol::Manager::Session::session_id = 1;

	// Rpc
	Rpc::Map          Rpc::map;
	Thread::RWLock    Rpc::locker_map;
	unsigned int      Rpc::XID::xid_count = 0;
	Thread::Mutex     Rpc::XID::locker_xid;
	Rpc::HouseKeeper  Rpc::housekeeper;

	// Security
	int Random::fd  = Random::Init();
	Random          __Random(RANDOM);
	NullSecurity    __NullSecurity_stub(NULLSECURITY);
	ARCFourSecurity __ARCFourSecurity_stub(ARCFOURSECURITY);
	MD5Hash         __MD5Hash_stub(MD5HASH);
	SHA256Hash      __SHA256Hash_stub(SHA256HASH);
	HMAC_MD5Hash    __HMAC_MD5Hash_stub(HMAC_MD5HASH);
	CompressARCFourSecurity  __CompressARCFourSecurity_stub(COMPRESSARCFOURSECURITY);
    DecompressARCFourSecurity  __DecompressARCFourSecurity_stub(DECOMPRESSARCFOURSECURITY);

	// Timer
	time_t Timer::now = time(NULL);
	CRITICAL_SECTION Timer::cs_observers;
	bool Timer::cs_init = false;
	int Timer::timer_ref = 0;

	// PollIO
	PollIO::IOMap PollIO::iomap;
	PollIO::FDSet PollIO::fdset;
	PollIO::IOMap PollIO::ionew;
	Thread::Mutex PollIO::locker_ionew;
	Thread::Mutex PollIO::locker_poll;
	fd_set PollIO::rfds;
	fd_set PollIO::wfds;
	fd_set PollIO::efds;
	SOCKET PollControl::writer;
	bool PollIO::wakeup_scope = false;
	bool PollIO::wakeup_flag  = false;

	Protocol::Map& Protocol::GetMap() { static Map map; return map; }
	Security::Map& Security::GetMap() { static Map map; return map; }
	PollIO::Task *PollIO::Task::GetInstance() { static PollIO::Task instance; return &instance; }
	Timer::Observers& Timer::observers() { static Observers tmp; return tmp; }

	char errormsg[128] = "";

	void PollIO::ShutDown()
	{
		Thread::Mutex::Scoped l(locker_poll);
		for(IOMap::const_iterator i = iomap.begin(); i != iomap.end(); ++i)
		{
			if(i->second)
				delete i->second;
		}
		iomap.clear();
	}

	void PollIO::Init()
	{
		FD_ZERO(&rfds); 
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		PollControl::Init();
		Thread::Pool::AddTask(PollIO::Task::GetInstance());
		Thread::Pool::Run(1);
	}

	void PollIO::Close()
	{
		Thread::Pool::Shutdown();
		PollIO::WakeUp();
	}

	void SHA256Hash::init()
	{
		SHA256_Init(&alginfo);
	}
	Octets& SHA256Hash::Update(Octets &o)
	{
		SHA256_Update(&alginfo, (unsigned char*)o.begin(), o.size());
		return o;
	}
	Octets& SHA256Hash::Final(Octets &digest)
	{
		digest.resize(SHA256_DIGEST_VALUELEN);
		SHA256_Final(&alginfo, (unsigned char*)digest.begin());
		return digest;
	}

};
