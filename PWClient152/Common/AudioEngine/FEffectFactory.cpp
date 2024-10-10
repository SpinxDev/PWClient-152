#include "FEffectFactory.h"
#include "FEffect.h"

using namespace AudioEngine;

EffectFactory::EffectFactory(void)
: m_pEventSystem(0)
{
}

EffectFactory::~EffectFactory(void)
{
}

Effect* EffectFactory::CreateEffect(EFFECT_TYPE type)
{
	Effect* pEffect = 0;
	switch (type)
	{
	case EFFECT_TYPE_VOLUME:
		pEffect = new EffectVolume;
		break;
	case EFFECT_TYPE_PITCH:
		pEffect = new EffectPitch;
		break;
	case EFFECT_TYPE_PAN:
		pEffect = new EffectPan;
		break;
	case EFFECT_TYPE_SURROUND_PAN:
		pEffect = new EffectSurroundPan;
		break;	
	case EFFECT_TYPE_PARAMEQ:
		pEffect = new EffectParamEQ;
		break;
	case EFFECT_TYPE_COMPRESSOR:
		pEffect = new EffectCompressor;
		break;
	case EFFECT_TYPE_ECHO:
		pEffect = new EffectEcho;
		break;
	case EFFECT_TYPE_OCCLUSION:
		pEffect = new EffectOcclusion;
		break;
	}
	if(!pEffect->Init(m_pEventSystem))
	{
		delete pEffect;
		return 0;
	}
	m_listEffect.push_back(pEffect);
	return pEffect;
}

bool EffectFactory::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_pEventSystem = pEventSystem;
	return true;
}

bool EffectFactory::DestroyEffect(Effect* pEffect)
{
	if(!pEffect)
		return true;
	EffectList::iterator it = m_listEffect.begin();
	for (; it != m_listEffect.end(); ++it)
	{
		if(*it == pEffect)
		{
			delete pEffect;
			m_listEffect.erase(it);
			return true;
		}
	}
	return false;
}

void EffectFactory::release()
{
	EffectList::const_iterator it = m_listEffect.begin();
	for (; it != m_listEffect.end(); ++it)
	{
		delete *it;
	}
	m_listEffect.clear();
}