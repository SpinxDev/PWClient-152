// File		: EC_TimeSafeChecker.h
// Creator	: Xu Wenbin
// Date		: 2014/4/18

#ifndef _ELEMENTCLIENT_EC_TIMESAFECHECKER_H
#define _ELEMENTCLIENT_EC_TIMESAFECHECKER_H

#include <ABaseDef.h>

//	GetTickCount() 两次相邻调用，最新调用的有可能返回结果比之前的值还小，
//	用 DWORD 直接相减不会返回负值，而返回更大的正值
//	因此增加此类处理此误差
class CECTimeSafeChecker
{
	DWORD	m_dwBeginTick;
	DWORD	m_dwTimeLength;
public:
	CECTimeSafeChecker();
	void Reset(DWORD dwCurrentTickCount, DWORD dwTimeLength);
	void Reset(DWORD dwTimeLength);
	void Reset();
	bool IsTimeArrived(DWORD dwCurrentTickCount)const;
	bool IsTimeArrived()const;
	DWORD ElapsedTime(DWORD dwCurrentTickCount)const;
	DWORD ElapsedTime()const;
	DWORD BeginTick()const;
	DWORD TimeLength()const;
	DWORD LeftTime(DWORD dwCurrentTickCount)const;
	DWORD LeftTime()const;

	static DWORD ElapsedTime(DWORD dwEndTickCount, DWORD dwBeginTickCount);
	static DWORD ElapsedTimeFor(DWORD dwBeginTickCount);
};

#endif		//	_ELEMENTCLIENT_EC_TIMESAFECHECKER_H