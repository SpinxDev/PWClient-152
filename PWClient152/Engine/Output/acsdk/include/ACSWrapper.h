/*
 * FILE: ACSWrapper.h
 *
 * DESCRIPTION: Critical section wrapper
 *
 * CREATED BY: Hedi, 2005/5/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ACSWRAPPER_H_
#define _ACSWRAPPER_H_

#include "ABaseDef.h"
#include "ACPlatform.h"

class ACSWrapper
{
public:

	ACSWrapper(CRITICAL_SECTION* pcs)
	{
		ASSERT(pcs);
		m_pcs = pcs;
		::EnterCriticalSection(m_pcs);
	}

	~ACSWrapper()
	{
		if (m_pcs)
			::LeaveCriticalSection(m_pcs);
	}

	void EnterCriticalSection() { ASSERT(m_pcs); ::EnterCriticalSection(m_pcs); }
	void LeaveCriticalSection() { ASSERT(m_pcs); ::LeaveCriticalSection(m_pcs); }

	void Attach(CRITICAL_SECTION* pcs)
	{
		//	Ensure not attach two cs at same time
		ASSERT(!m_pcs && pcs);
		m_pcs = pcs;
		::EnterCriticalSection(m_pcs);
	}

	void Detach(bool bLeave)
	{
		if (bLeave && m_pcs)
			::LeaveCriticalSection(m_pcs);

		m_pcs = NULL;
	}

protected:

	CRITICAL_SECTION*	m_pcs;
};

#endif	//	_ACSWRAPPER_H_


