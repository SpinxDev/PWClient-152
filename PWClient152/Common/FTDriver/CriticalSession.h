#pragma once

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define  _WIN32_WINNT 0x0501
#endif

class CriticalSession
{
private:
	void * pCriticalSection_;
public:
	CriticalSession();
	~CriticalSession();
	void Enter();
	void Leave();
#if (WINVER >= 0x0400) && (_WIN32_WINNT >= 0x0400)
	bool TryEnter();
#endif

public:
	class Scope
	{
		CriticalSession & cs_;
	public:
		Scope(CriticalSession & cs) : cs_(cs) { cs_.Enter(); }
		~Scope() { cs_.Leave(); }
	};
};