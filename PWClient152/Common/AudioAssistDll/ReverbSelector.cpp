#include "stdafx.h"
#include "AudioAssistDll.h"
#include "ReverbSelector.h"
#include "DlgSelectReverb.h"

extern CAudioAssistDllApp theApp;

ReverbSelector::ReverbSelector(EventSystem* pEventSystem)
{
	m_pDlgSelectReverb = new CDlgSelectReverb(pEventSystem);
}

ReverbSelector::~ReverbSelector(void)
{
	delete m_pDlgSelectReverb;
	m_pDlgSelectReverb = 0;
}

REVERB* ReverbSelector::GetSelectedReverb() const
{
	return m_pDlgSelectReverb->GetSelectedReverb();
}

INT_PTR ReverbSelector::DoModal()
{
	HINSTANCE hHostResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(theApp.m_hInstance);
	INT_PTR r = m_pDlgSelectReverb->DoModal();
	AfxSetResourceHandle(hHostResourceHandle);
	return r;
}
