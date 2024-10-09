// Filename	: WikiEntity.cpp
// Creator	: Feng Ning
// Date		: 2010/04/27

#include "AMemory.h"
#include "WikiEntity.h"

#define new A_DEBUG_NEW

WikiEntityPtr::WikiEntityPtr(WikiEntity* p)
:m_Ptr(p)
,m_PtrRef(new Ref())
{}

WikiEntityPtr::~WikiEntityPtr()
{
	if(m_PtrRef)
	{
		if(m_PtrRef->GetCount() == 1)
		{
			if(m_Ptr) delete m_Ptr;
		}
		m_PtrRef->Release();
	}

	m_PtrRef = 0;
	m_Ptr = 0;
}

WikiEntityPtr::WikiEntityPtr(const WikiEntityPtr& ptr)
:m_Ptr(ptr.m_Ptr)
,m_PtrRef(ptr.m_PtrRef)
{
	if(m_PtrRef)
	{
		m_PtrRef->AddRef();
	}
}

WikiEntityPtr& WikiEntityPtr::operator=(const WikiEntityPtr& ptr)
{
	if(m_PtrRef)
	{
		if(m_PtrRef->GetCount() == 1)
		{
			if(m_Ptr) delete m_Ptr;
		}
		m_PtrRef->Release();
	}
	
	m_Ptr = ptr.m_Ptr;
	m_PtrRef = ptr.m_PtrRef;

	if(m_PtrRef)
	{
		m_PtrRef->AddRef();
	}

	return *this;
}


WikiEntityPtr::Ref::Ref()
:m_Ref(1)
{
}

void WikiEntityPtr::Ref::Release()
{
	//ASSERT(m_Ref > 0);
	m_Ref -= 1;
	if(m_Ref == 0)
	{
		delete this;
	}
}

void WikiEntityPtr::Ref::AddRef()
{
	//ASSERT(m_Ref > 0);
	m_Ref += 1;
}