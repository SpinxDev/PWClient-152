#include "stdafx.h"
#include "CheckSoundInsProp.h"
#include "FEventParameter.h"
#include "FSoundInstanceTemplate.h"
#include "FEventLayer.h"
#include "Fsounddef.h"
#include "Global.h"

using AudioEngine::EventLayer;
using AudioEngine::EventParameter;
using AudioEngine::EVENT_PARAM_PROPERTY;

CCheckSoundInsProp::CCheckSoundInsProp(void)
{
}

CCheckSoundInsProp::~CCheckSoundInsProp(void)
{
}

bool CCheckSoundInsProp::IsLegal(SOUND_INSTANCE_PROPERTY prop, SoundInstanceTemplate* pSoundInsTemplate, float& fStart, float& fLength)
{
	if(!pSoundInsTemplate)
		return false;	
	EventLayer* pEventLayer = pSoundInsTemplate->GetEventLayer();
	if(!pEventLayer)
		return false;

	EventParameter* pEventParameter = pEventLayer->GetParameter();
	EVENT_PARAM_PROPERTY paramProp;
	pEventParameter->GetProperty(paramProp);

	if(prop.fStartPosition + prop.fLength - 1.0f > 1e-5)
	{
		fStart = 1.0f;
		fLength = 0.0f;
		GF_Log(LOG_ERROR, "超过参数范围");
		return false;
	}
	int iSoundInstanceNum = pEventLayer->GetSoundInstanceTemplateNum();
	for (int i=0; i<iSoundInstanceNum; ++i)
	{
		SOUND_INSTANCE_PROPERTY insProp;
		SoundInstanceTemplate* pSoundInstanceTemplate = pEventLayer->GetSoundInstanceTemplateByIndex(i);
		if(pSoundInstanceTemplate == pSoundInsTemplate)
			continue;
		pSoundInstanceTemplate->GetProperty(insProp);
		float fMinStart = prop.fStartPosition;
		if(insProp.fStartPosition < fMinStart)
			fMinStart = insProp.fStartPosition;
		float fMaxEnd = prop.fStartPosition + prop.fLength;
		if(insProp.fStartPosition + insProp.fLength > fMaxEnd)
			fMaxEnd = insProp.fStartPosition + insProp.fLength;
		if(prop.fLength + insProp.fLength - (fMaxEnd - fMinStart) > 1e-5)
		{
			fStart = insProp.fStartPosition;
			fLength = insProp.fLength;
			GF_Log(LOG_ERROR, "与实例%s有重叠", pSoundInstanceTemplate->GetSoundDef()->GetName());
			return false;
		}
	}
	return true;
}