// File		: EC_TimeSafeChecker.h
// Creator	: Xu Wenbin
// Date		: 2014/4/18

#ifndef _ELEMENTCLIENT_EC_TIMESAFECHECKER_H
#define _ELEMENTCLIENT_EC_TIMESAFECHECKER_H

#include <ABaseDef.h>

//	GetTickCount() �������ڵ��ã����µ��õ��п��ܷ��ؽ����֮ǰ��ֵ��С��
//	�� DWORD ֱ��������᷵�ظ�ֵ�������ظ������ֵ
//	������Ӵ��ദ������
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