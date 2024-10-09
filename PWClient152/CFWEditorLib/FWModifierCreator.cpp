// ModifierCreator.cpp: implementation of the FWModifierCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "FWModifierCreator.h"
#include "FWModifierParabola.h"
#include "FWModifierRing.h"

#define new A_DEBUG_NEW



IMPLEMENT_SINGLETON(FWModifierCreator)

// used for Init()
static const char * _Names[] = 
{
	"抛物下落",
	"环形炸开"
};

// used for Init()
static const FWRuntimeClass * _Classes[] = 
{
	FW_RUNTIME_CLASS(FWModifierParabola),
	FW_RUNTIME_CLASS(FWModifierRing),
};




FWModifierCreator::FWModifierCreator()
{
	m_bInited = false;
}

FWModifierCreator::~FWModifierCreator()
{
	m_bInited = false;
}

FWModifier * FWModifierCreator::CreateModifier(int nID)
{
	FWModifierInfo *p = FindByID(nID);
	if (!p)	return NULL;

	FWModifier *pModifier = static_cast<FWModifier *>(p->pClass->CreateObject());
	pModifier->InitParam();
	return pModifier;
}

bool FWModifierCreator::Init()
{
	if (m_bInited) return true;

	for (int i = 0; i < FWModifierCreator::ID_MODIFIER_MAX; i++)
	{
		FWModifierInfo *p = new FWModifierInfo;
		p->ID = i;
		p->Name = AString(_Names[i]);
		p->pClass = _Classes[i];
		AddTail(p);
	}

	m_bInited = true;
	return true;
}
