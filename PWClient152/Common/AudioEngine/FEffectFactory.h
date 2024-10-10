#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"

namespace AudioEngine
{
	class EventSystem;
	class Effect;

	class _EXPORT_DLL_ EffectFactory
	{
	public:
		EffectFactory(void);
		~EffectFactory(void);
	public:
		bool					Init(EventSystem* pEventSystem);
		Effect*					CreateEffect(EFFECT_TYPE type);
		bool					DestroyEffect(Effect* pEffect);
	protected:
		void					release();
	protected:
		EventSystem*			m_pEventSystem;
		EffectList				m_listEffect;
	};
}
