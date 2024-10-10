#pragma once

#include "FAudioEngine.h"

using AudioEngine::EVENT_PROPERTY;

enum
{
	idEventName,
	//idOneShot,
	idEventVolume,
	idEventVolumeRandom,
	idEventPitch,
	idEventPitchRandom,
	idFadeInTime,
	idFadeInCurveType,
	idFadeOutTime,
	idFadeOutCurveType,
	idMode,
	idRolloff,
	idMinDistance,
	idMaxDistance,
	id3DConeInsideAngle,
	id3DConeOutsideAngle,
	id3DConeOutsideVolume,
	id3DSpread,
	idMaxPlaybacks,
	idMaxPlaybacksBehavior,
	idMinIntervalTime,
	idPriority,
	idEventNote,
	idGuid
};

class CPropertyArray;
class CBCGPPropertyListWrapper;

class CEventPropList
{
public:
	CEventPropList(void);
	~CEventPropList(void);
public:
	bool			Init(CPropertyArray* pProperties, CBCGPPropertyListWrapper* pBCGPPropListWrapper);
	bool			BuildProperty(const EVENT_PROPERTY& prop);
	void			PropChanged(EVENT_PROPERTY& prop, bool bSave, bool bIsCommit, int iStartIdx);
protected:
	CPropertyArray* m_pProperties;
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	EVENT_PROPERTY	m_Prop;
};
