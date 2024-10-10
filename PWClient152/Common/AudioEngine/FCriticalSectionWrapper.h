#pragma once

#include "windows.h"

class CriticalSectionWrapper
{
public:
	CriticalSectionWrapper(void);
	~CriticalSectionWrapper(void);
public:
	void				Lock();
	void				Unlock();
protected:
	CRITICAL_SECTION	m_CriticalSection;
};

class CriticalSectionController
{
public:
	CriticalSectionController(CriticalSectionWrapper* pCriticalSectionWrapper);
	~CriticalSectionController(void);
protected:
	CriticalSectionWrapper*		m_pCriticalSectionWrapper;
};
