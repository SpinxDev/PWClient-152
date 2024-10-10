#include "stdafx.h"
#include "AudioAssistDll.h"
#include "EventSelector.h"
#include "DlgSelectEvent.h"

extern CAudioAssistDllApp theApp;

EventSelector::EventSelector(EventSystem* pEventSystem, const VECTOR& vListenerPos)
{
	m_pDlgSelectEvent = new CDlgSelectEvent(pEventSystem, vListenerPos);
}

EventSelector::~EventSelector(void)
{
	delete m_pDlgSelectEvent;
	m_pDlgSelectEvent = 0;
}

INT_PTR EventSelector::DoModal()
{
	HINSTANCE hHostResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);
	INT_PTR r = m_pDlgSelectEvent->DoModal();
	AfxSetResourceHandle(hHostResourceHandle);
	return r;
}

bool EventSelector::SetInitSelectEvent(const char* szGuid)
{
	return m_pDlgSelectEvent->SetInitSelectEvent(szGuid);
}

const char* EventSelector::GetSelectEventGuid() const
{
	return m_pDlgSelectEvent->GetSelectEventGuid();
}