#include "FCriticalSectionWrapper.h"

CriticalSectionWrapper::CriticalSectionWrapper(void)
{
	InitializeCriticalSection(&m_CriticalSection);
}

CriticalSectionWrapper::~CriticalSectionWrapper(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CriticalSectionWrapper::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void CriticalSectionWrapper::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}

CriticalSectionController::CriticalSectionController(CriticalSectionWrapper* pCriticalSectionWrapper)
{
	m_pCriticalSectionWrapper = pCriticalSectionWrapper;
	if(m_pCriticalSectionWrapper)
		m_pCriticalSectionWrapper->Lock();
}

CriticalSectionController::~CriticalSectionController(void)
{
	if(m_pCriticalSectionWrapper)
		m_pCriticalSectionWrapper->Unlock();
}