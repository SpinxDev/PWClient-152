#include "CriticalSession.h"
#include <windows.h>


CriticalSession::CriticalSession() :
	pCriticalSection_(NULL)
{
	pCriticalSection_ = new CRITICAL_SECTION;
	CRITICAL_SECTION * pCS = static_cast<CRITICAL_SECTION *>(pCriticalSection_);
	::InitializeCriticalSection(pCS);
}

CriticalSession::~CriticalSession()
{
	CRITICAL_SECTION * pCS = static_cast<CRITICAL_SECTION *>(pCriticalSection_);
	::DeleteCriticalSection(pCS);
	delete pCS;
}

#if (WINVER >= 0x0400) && (_WIN32_WINNT >= 0x0400)

bool CriticalSession::TryEnter()
{
	CRITICAL_SECTION * pCS = static_cast<CRITICAL_SECTION *>(pCriticalSection_);
	return (TRUE == TryEnterCriticalSection(pCS));
}

#endif

void CriticalSession::Enter()
{
	CRITICAL_SECTION * pCS = static_cast<CRITICAL_SECTION *>(pCriticalSection_);
	::EnterCriticalSection(pCS);
}

void CriticalSession::Leave()
{
	CRITICAL_SECTION * pCS = static_cast<CRITICAL_SECTION *>(pCriticalSection_);
	::LeaveCriticalSection(pCS);
}
