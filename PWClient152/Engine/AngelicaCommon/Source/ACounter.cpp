
#include "ACounter.h"
#include "ATime.h"

static bool g_bBadPerformanceCounter = false;
static bool g_bSpeedGeared = false;

double ACounter::m_fCPUFrequency = 1;

ACounter::ACounter()
{
	m_dwFPSLastTick  = 0;
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;
	//__int64 nMS1 = GetMicroSecondNow();
	//__int64 nCycle1 = GetCPUCycle();
	//while( GetMicroSecondNow() - nMS1 < 1000 )
	//{
	//}
	//__int64 nMS2 = GetMicroSecondNow();
	//__int64 nCycle2 = GetCPUCycle();
	//m_nCPUFrequency = (nCycle2 - nCycle1) * 1000000 / (nMS2 - nMS1);
    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency(&liFreq);
    m_fCPUFrequency = (double)liFreq.QuadPart;

	// now test if the performance counter is bad or
	// a speed gear is activated.
	DWORD dwTimeStart, dwTimeThis;
	int pmStart, pmThis;
	SYSTEMTIME smStart, smThis;
	FILETIME ftStart, ftThis;
	ULARGE_INTEGER liStart, liThis;

	dwTimeStart = timeGetTime();
	pmStart = int(GetMicroSecondNow() / 1000);
	GetLocalTime(&smStart);
	SystemTimeToFileTime(&smStart, &ftStart);
	memcpy(&liStart, &ftStart, sizeof(FILETIME));

	while(true)
	{
		dwTimeThis = timeGetTime();
		pmThis = int(GetMicroSecondNow() / 1000);
		GetLocalTime(&smThis);
		SystemTimeToFileTime(&smThis, &ftThis);
		memcpy(&liThis, &ftThis, sizeof(FILETIME));

		if( liThis.QuadPart - liStart.QuadPart > 5000000 )
			break;
	}

	int nDelta = (int)((liThis.QuadPart - liStart.QuadPart) / 10000);
	if( abs(pmThis - pmStart - nDelta) > 200 )
		g_bBadPerformanceCounter = true;
	
	if( abs(int(dwTimeThis - dwTimeStart) - nDelta) > 200 )
		g_bSpeedGeared = true;
}

ACounter::~ACounter()
{
}

bool ACounter::Init()
{
	m_dwFPSLastTick = a_GetTime();
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;
	return true;
}

bool ACounter::Release()
{
	return true;
}

bool ACounter::ResetFPSCounter()
{
	m_dwFPSLastTick = a_GetTime();
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;

	return true;
}

bool ACounter::UpdateFPSCounter(bool * pFPSChanged)
{
	DWORD dwTickNow = a_GetTime();

	m_dwFPSCounter ++;

	if( dwTickNow > m_dwFPSLastTick + 1000 )
	{
		m_vFPS				= m_dwFPSCounter * 1000.0f / (dwTickNow - m_dwFPSLastTick);
		m_dwFPSLastTick		= dwTickNow;
		m_dwFPSCounter		= 0;

		if( pFPSChanged )
			*pFPSChanged = true;
	}
	else
	{
		if( pFPSChanged )
			*pFPSChanged = false;
	}
	
	return true;
}

__int64 ACounter::GetMicroSecondNow()
{
	if( g_bSpeedGeared )
	{
		SYSTEMTIME sm;
		FILETIME ft;
		ULARGE_INTEGER li;
		GetLocalTime(&sm);
		SystemTimeToFileTime(&sm, &ft);
		memcpy(&li, &ft, sizeof(FILETIME));
		return li.QuadPart * 1000;
	}
	else if( g_bBadPerformanceCounter )
		return __int64(timeGetTime()) * __int64(1000);
	

	static LARGE_INTEGER frequency;
	static bool bFirstTime = true;

	if( bFirstTime )
	{
		if( !QueryPerformanceFrequency(&frequency) )
			return 0;
		bFirstTime = false;
	}
	
	LARGE_INTEGER large_int;
	if( !QueryPerformanceCounter(&large_int) )
		return 0;

	__int64 iSecond = large_int.QuadPart / frequency.QuadPart;
	__int64 iRemainder = large_int.QuadPart % frequency.QuadPart;

	return iSecond * 1000000 + iRemainder * 1000000 / frequency.QuadPart;
}

//不准确 现在用windows api实现
//__int64 ACounter::GetCPUCycle()
//{
//	//	RDTSC store time stamp in EDX:EAX, here we only use low 32-bit, because
//	//	we only care the delta between two stamps, which won't be very large.
//	DWORD dwTime1;
//	DWORD dwTime2;
//
//    __asm
//	{
//		RDTSC
//		mov		dwTime1, edx
//		mov		dwTime2, eax
//	}
//
//	return (__int64(dwTime1) << 32) + dwTime2;
//}
//
//__int64 ACounter::GetCPUFrequency()
//{
//	return m_nCPUFrequency;
//}

__int64 ACounter::GetCPUCycle()
{
    LARGE_INTEGER iCounter;
    DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
    QueryPerformanceCounter(&iCounter);
    SetThreadAffinityMask(GetCurrentThread(), oldmask);
    return iCounter.QuadPart;
}

double ACounter::GetCPUFrequency()
{
    return m_fCPUFrequency;
}

DWORD ACounter::GetMilliSecondNow()
{
	return (DWORD)GetMicroSecondNow() / 1000;
}