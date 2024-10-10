// File		: EC_TimeSafeChecker.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/18

#include "EC_TimeSafeChecker.h"

#include <windows.h>

CECTimeSafeChecker::CECTimeSafeChecker()
: m_dwBeginTick(0)
, m_dwTimeLength(0)
{
}

void CECTimeSafeChecker::Reset(DWORD dwCurrentTickCount, DWORD dwTimeLength)
{
	m_dwBeginTick = dwCurrentTickCount;
	m_dwTimeLength = dwTimeLength;
}

void CECTimeSafeChecker::Reset(DWORD dwTimeLength)
{
	Reset(::GetTickCount(), dwTimeLength);
}

void CECTimeSafeChecker::Reset()
{
	Reset(::GetTickCount(), m_dwTimeLength);
}

bool CECTimeSafeChecker::IsTimeArrived(DWORD dwCurrentTickCount)const
{
	return ElapsedTime(dwCurrentTickCount) >= TimeLength();
}

bool CECTimeSafeChecker::IsTimeArrived()const
{
	return IsTimeArrived(::GetTickCount());
}

DWORD CECTimeSafeChecker::ElapsedTime(DWORD dwEndTickCount, DWORD dwBeginTickCount)
{
	return (dwEndTickCount>=dwBeginTickCount) ? (dwEndTickCount-dwBeginTickCount) : 0;
}

DWORD CECTimeSafeChecker::ElapsedTime(DWORD dwCurrentTickCount)const
{
	return ElapsedTime(dwCurrentTickCount, m_dwBeginTick);
}

DWORD CECTimeSafeChecker::ElapsedTimeFor(DWORD dwBeginTickCount)
{
	return ElapsedTime(::GetTickCount(), dwBeginTickCount);
}

DWORD CECTimeSafeChecker::ElapsedTime()const
{
	return ElapsedTime(::GetTickCount());
}

DWORD CECTimeSafeChecker::BeginTick()const
{
	return m_dwBeginTick;
}

DWORD CECTimeSafeChecker::TimeLength()const
{
	return m_dwTimeLength;
}

DWORD CECTimeSafeChecker::LeftTime(DWORD dwCurrentTickCount)const{
	DWORD result = 0;
	DWORD dwElapsedTime = ElapsedTime(dwCurrentTickCount);
	if (dwElapsedTime < TimeLength()){
		result = TimeLength() - dwElapsedTime;
	}
	return result;
}

DWORD CECTimeSafeChecker::LeftTime()const{
	return LeftTime(::GetTickCount());
}