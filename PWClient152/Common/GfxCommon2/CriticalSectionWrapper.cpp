#include "StdAfx.h"
#include "CriticalSectionWrapper.h"

CSWrapper::CSWrapper(void)
{
	InitializeCriticalSection(&m_CriticalSection);
}

CSWrapper::~CSWrapper(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CSWrapper::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void CSWrapper::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}

CSController::CSController(CSWrapper* pCriticalSectionWrapper)
{
	m_pCriticalSectionWrapper = pCriticalSectionWrapper;
	if(m_pCriticalSectionWrapper)
		m_pCriticalSectionWrapper->Lock();
}

CSController::~CSController(void)
{
	if(m_pCriticalSectionWrapper)
		m_pCriticalSectionWrapper->Unlock();
}