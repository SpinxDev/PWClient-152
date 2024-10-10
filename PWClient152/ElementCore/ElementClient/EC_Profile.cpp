// Filename	: EC_Profile.h
// Creator	: Xu Wenbin
// Date		: 2013/11/21

#include "EC_Profile.h"
#include "EC_Game.h"
#include "EC_RTDebug.h"

extern CECGame * g_pGame;

//	ElapsedTime
ElapsedTime::ElapsedTime()
: m_dwStart(GetTickCount())
{
}

DWORD ElapsedTime::GetElapsed()
{
	DWORD dwTick = GetTickCount();
	DWORD dwRet = dwTick > m_dwStart ? (dwTick - m_dwStart) : 0;
	m_dwStart = dwTick;
	return dwRet;
}

//	ScopedElapsedTime
ScopedElapsedTime::ScopedElapsedTime(const ACHAR *szType)
: m_szType(szType)
{
}

ScopedElapsedTime::~ScopedElapsedTime()
{
	g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("%s %.3f"), m_szType, 0.001f * (int)m_counter.GetElapsed());
}

//	ScatteredElapsedTime
ScatteredElapsedTime::ScatteredElapsedTime(const ACHAR *szType)
: m_szType(szType)
, m_dwSum(0)
, m_dwEnterTime(0)
{
}

ScatteredElapsedTime::~ScatteredElapsedTime()
{
	g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("%s %.3f"), m_szType, 0.001f * (int)m_dwSum);
}

void ScatteredElapsedTime::Enter()
{
	m_dwEnterTime = GetTickCount();
}

void ScatteredElapsedTime::Leave()
{
	DWORD dwTick = GetTickCount();
	if (dwTick > m_dwEnterTime){
		m_dwSum += (dwTick - m_dwEnterTime);
	}
}