// Filename	: EC_Profile.h
// Creator	: Xu Wenbin
// Date		: 2013/11/21

#pragma once

#include <ABaseDef.h>

//	ʱ��ͳ��
class ElapsedTime
{
	DWORD m_dwStart;
public:
	ElapsedTime();
	DWORD GetElapsed();
};

//	����ʱ��ͳ�Ʋ���ʾ
class ScopedElapsedTime
{
	const ACHAR*	m_szType;
	ElapsedTime		m_counter;
public:
	ScopedElapsedTime(const ACHAR *szType);
	~ScopedElapsedTime();
};

//	���ͳ��ʱ����Ӳ���ʾ
class ScatteredElapsedTime
{
	const ACHAR*	m_szType;
	DWORD			m_dwSum;
	DWORD			m_dwEnterTime;
public:
	ScatteredElapsedTime(const ACHAR *szType);
	~ScatteredElapsedTime();
	void Enter();
	void Leave();
};

class ScatteredElapsedTimeWrapper
{
	ScatteredElapsedTime *m_counter;
public:
	ScatteredElapsedTimeWrapper(ScatteredElapsedTime &counter)
		: m_counter(&counter){
		m_counter->Enter();
	}
	~ScatteredElapsedTimeWrapper(){
		m_counter->Leave();
	}
};