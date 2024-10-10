#pragma once

#include "windows.h"

class CSWrapper
{
public:
	CSWrapper(void);
	~CSWrapper(void);
public:
	void				Lock();
	void				Unlock();
protected:
	CRITICAL_SECTION	m_CriticalSection;
};

class CSController
{
public:
	CSController(CSWrapper* pCriticalSectionWrapper);
	~CSController(void);
protected:
	CSWrapper*		m_pCriticalSectionWrapper;
};
