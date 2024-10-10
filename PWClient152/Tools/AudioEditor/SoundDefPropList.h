#pragma once

using AudioEngine::SOUND_DEF_PROPERTY;

enum
{
	idSoundDefName,
	idPlayMode,
	idSoundDefVolume,
	idSoundDefVolumeRandom,
	idSoundDefPitch,
	idSoundDefPitchRandom,
	idIntervalTime,
	idIntervalTimeRandomization,
	idSoundDefNote
};

class CPropertyArray;
class CBCGPPropertyListWrapper;

class CSoundDefPropList
{
public:
	CSoundDefPropList(void);
	~CSoundDefPropList(void);
public:
	bool			Init(CPropertyArray* pProperties, CBCGPPropertyListWrapper* pBCGPPropListWrapper);
	bool			BuildProperty(const SOUND_DEF_PROPERTY& prop);
	void			PropChanged(SOUND_DEF_PROPERTY& prop, bool bSave, bool bIsCommit, int iStartIdx);
protected:	
	SOUND_DEF_PROPERTY	m_Prop;
	CPropertyArray* m_pProperties;
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
};
