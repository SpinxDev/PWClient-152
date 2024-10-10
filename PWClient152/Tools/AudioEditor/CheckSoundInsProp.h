#pragma once

using AudioEngine::SOUND_INSTANCE_PROPERTY;
using AudioEngine::SoundInstanceTemplate;

class CCheckSoundInsProp
{
public:
	CCheckSoundInsProp(void);
	~CCheckSoundInsProp(void);
public:
	bool		IsLegal(SOUND_INSTANCE_PROPERTY prop, SoundInstanceTemplate* pSoundInsTemplate, float& fStart, float& fLength);
};
