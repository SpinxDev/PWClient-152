#ifndef __TIMER_H
#define __TIMER_H

#include <vector>
#include <time.h>

namespace GNET
{

class Timer
{
public:
	class Observer
	{
	public:
		virtual ~Observer() { }
		virtual void Update() = 0;
	};
private:
	static time_t now;
	typedef std::vector<Observer*> Observers;
	static Observers& observers();
	static CRITICAL_SECTION cs_observers;
	static bool cs_init;
	static int timer_ref;
	time_t t;
	
public:
	Timer() : t(now) { timer_ref++; }
	~Timer()
	{
		timer_ref--;
		if( timer_ref <= 0 )
		{
			DeleteCriticalSection(&cs_observers);
			cs_init = false;
		}
	}

	static void Lock()
	{
		if( !cs_init )
		{
			InitializeCriticalSection(&cs_observers);
			cs_init = true;
		}

		EnterCriticalSection(&cs_observers);
	}

	static void Unlock()
	{
		if( cs_init )
			LeaveCriticalSection(&cs_observers);
	}

	static void Attach(Observer *o) 
	{
		Lock();

		observers().push_back(o);

		Unlock();
	}

	static void Detach(Observer *o)
	{
		Lock();

		Observers::iterator it = observers().begin();
		for (;it != observers().end();++it)
		{
			if (*it == o)
			{
				observers().erase(it);
				break;
			}
		}

		Unlock();
	}

	static void Update() 
	{
		Lock();

		time_t tmp = time(NULL);
		if (tmp > now)
		{
			std::for_each(observers().begin(), observers().end(), std::mem_fun(&Observer::Update));	
			now = tmp;
		}

		Unlock();
	}

	static time_t GetTime() { return now; }
	int Elapse() const { return now - t; }
	void Reset() { t = now; }
};

};

#endif
