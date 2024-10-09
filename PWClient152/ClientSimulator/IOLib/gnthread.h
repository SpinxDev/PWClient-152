#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <process.h>
#include <deque>

namespace Thread{

class Runnable
{
	int m_priority;
public:

	Runnable( int priority = 0 ) : m_priority(priority) { }
	virtual ~Runnable() { }

	virtual void Run() = 0;

	int GetPriority() const { return m_priority; }
	void SetPriority( int priority ) { m_priority = priority; }
};

class Mutex
{
	friend class Condition;
	CRITICAL_SECTION cs;

public:
	~Mutex ()
	{
		DeleteCriticalSection(&cs);
	}
	explicit Mutex(bool recursive=false)
	{
		InitializeCriticalSection(&cs);
	}

	void Lock()   { EnterCriticalSection(&cs); }
	void UNLock() { LeaveCriticalSection(&cs); }
	class Scoped
	{
		Mutex *mx;
	public:
		~Scoped () { mx->UNLock(); }
		explicit Scoped(Mutex& m) : mx(&m) { 
			mx->Lock(); 
		}
	};
};

		
class RWLock
{
	CRITICAL_SECTION cs;
public:
	~RWLock()
	{
		DeleteCriticalSection(&cs);
	}   
	RWLock()
	{
		InitializeCriticalSection(&cs);
	}
	void WRLock() {  EnterCriticalSection(&cs);}
	void RDLock() {  EnterCriticalSection(&cs);}
	void UNLock() {  LeaveCriticalSection(&cs);}
	class WRScoped
	{
		RWLock *rw;
	public:
		~WRScoped () { rw->UNLock(); }
		explicit WRScoped(RWLock &l) : rw(&l) { rw->WRLock(); }
	};  
	class RDScoped
	{   
		RWLock *rw;
	public:
		~RDScoped () { rw->UNLock(); }
		explicit RDScoped(RWLock &l) : rw(&l) { rw->RDLock(); }
	};
};

class Condition
{
	HANDLE event;

public:
	~Condition ()
	{   
		CloseHandle(event);
	}   

	Condition( ) 
	{
		event = CreateEvent( NULL, TRUE, TRUE, NULL); 
	}

	void Wait( Mutex & mutex ) 
	{
		mutex.UNLock();
		WaitForSingleObject(event, INFINITE);
		mutex.Lock();
		ResetEvent(event);
	}

	void NotifyOne( ) 
	{
		SetEvent(event);
	}

};

class Semaphore
{
	HANDLE sem;
public:
	~Semaphore ()
	{   
		CloseHandle(sem);
	}   

	Semaphore( ) 
	{
		sem = CreateSemaphore(NULL, 0, 0x7fff, NULL); 
	}

	void Wait( Mutex & mutex ) 
	{
		WaitForSingleObject(sem, INFINITE);
	}

	void NotifyOne( ) 
	{
		ReleaseSemaphore( sem, 1, NULL);
	}

	void NotifyAll(int n) 
	{
		ReleaseSemaphore( sem, n, NULL);
	}
}; 


class Pool
{
public:
	enum PoolState
	{
		stateNormal = 1,
		stateQuit = 2,
		stateQuitAtOnce = 3
	};

	typedef std::deque<Runnable*> TaskQueue;
private:
	static TaskQueue s_tasks;
	static Mutex s_mutex_tasks;
	static Semaphore s_cond_tasks;
	static int pool_state;

public:
	static void Run(size_t count)
	{
		s_thread_count = count;
		{
			Mutex::Scoped lock(s_mutex_tasks);
			for( size_t i=0; i<count; i++ ){
				 _beginthread(&Pool::RunThread, 0, NULL);
			}
		}
	}

	static void Shutdown()
	{
		pool_state = stateQuit;
		s_cond_tasks.NotifyAll(Size());
	}

	static bool AddTask( Runnable * task )
	{
		if(pool_state == stateQuit)
			return false;
		Mutex::Scoped lock(s_mutex_tasks);
		s_tasks.push_back( task );
		s_cond_tasks.NotifyOne();
		return true;
	}

	static size_t QueueSize()
	{
		Mutex::Scoped lock(s_mutex_tasks);
		return s_tasks.size();
	}

	static size_t Size() { return s_thread_count; }

private:
	static size_t s_thread_count;
	virtual ~Pool() { }

	static void __cdecl RunThread( void * id);
};

};

#endif
