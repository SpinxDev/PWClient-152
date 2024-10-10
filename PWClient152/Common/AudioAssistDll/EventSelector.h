#pragma once

#include "FAudioEngine.h"

class CDlgSelectEvent;

namespace AudioEngine
{
	class EventSystem;
	struct VECTOR;
}

using AudioEngine::EventSystem;
using AudioEngine::VECTOR;

class __declspec(dllexport) EventSelector
{
public:
	EventSelector(EventSystem* pEventSystem, const VECTOR& vListenerPos);
	~EventSelector(void);
public:
	INT_PTR								DoModal();
	
	//	This function may return false if project not exist, or event not found
	//	True if event exist
	bool								SetInitSelectEvent(const char* szGuid);
	const char*							GetSelectEventGuid() const;
protected:
	CDlgSelectEvent*					m_pDlgSelectEvent;
private:
	EventSelector(const EventSelector&);
	EventSelector& operator = (const EventSelector&);
};
