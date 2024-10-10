#pragma once

#include "FSoundInstance.h"
#include "FCriticalSectionWrapper.h"

namespace AudioEngine
{
	class SoundInstance;

	class IntervalManager : public SoundInstance::SoundInstanceListener
	{
	protected:
		IntervalManager(void);
		virtual ~IntervalManager(void);
	public:
		static IntervalManager&	GetInstance();
		void					AddSoundInstance(SoundInstance* pSoundInstance);
		void					RemoveSoundInstance(SoundInstance* pSoundInstance);
		void					Update(DWORD dwEllapseTime);
	protected:
		virtual bool			OnStop(SoundInstance* pSoundInstance);
		void					process(SoundInstance* pSoundInstance);
		void					release();
	protected:
		SoundInstanceSet		m_setSoundInstance;
		CriticalSectionWrapper	m_csForThreadSafe;
	};
}