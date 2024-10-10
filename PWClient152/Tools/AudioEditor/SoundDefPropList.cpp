#include "StdAfx.h"
#include "SoundDefPropList.h"
#include "Global.h"
#include "PropListCtrl/BCGPPropertyObject.h"
#include "PropListCtrl/BCGPPropertyListWrapper.h"

using AudioEngine::PLAY_MODE;

static prop_range_template<float> s_rangeVolume;
static prop_range_template<float> s_rangePitch;
static prop_range_template<int> s_rangeIntervalTime;
static prop_range_template<int> s_rangePriority;

static prop_set_template<int> s_setPlayMode;

CSoundDefPropList::CSoundDefPropList(void)
{
	m_pProperties = 0;
	m_pBCGPPropListWrapper = 0;
}

CSoundDefPropList::~CSoundDefPropList(void)
{
}

bool CSoundDefPropList::Init(CPropertyArray* pProperties, CBCGPPropertyListWrapper* pBCGPPropListWrapper)
{
	if(!pProperties || !pBCGPPropListWrapper)
		return false;
	m_pProperties = pProperties;
	m_pBCGPPropListWrapper = pBCGPPropListWrapper;
	return true;
}

bool CSoundDefPropList::BuildProperty(const SOUND_DEF_PROPERTY& prop)
{
	m_Prop = prop;
	if(!m_pProperties)
		return false;

	if(0 == s_setPlayMode.GetCount())
	{
		s_setPlayMode.AddElement((int)AudioEngine::PLAY_MODE_SEQUENTIAL_EVENT_RESTART, "从头顺序播放");
		s_setPlayMode.AddElement((int)AudioEngine::PLAY_MODE_SEQUENTIAL, "从当前顺序播放");
		s_setPlayMode.AddElement((int)AudioEngine::PLAY_MODE_RANDOM, "随机播放（可重复）");
		s_setPlayMode.AddElement((int)AudioEngine::PLAY_MODE_RANDOM_NOREPEAT, "随机播放（不可重复）");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)prop.playMode;
		pi.strName = "播放模式";
		pi.strDesc = "";
		pi.pOptions = &s_setPlayMode;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	s_rangeVolume.SetRange(-60.0f, 0.0f);
	s_rangeVolume.SetStep(0.1);
	s_rangeVolume.SetUnit("dB");
	s_rangeVolume.SetTipFunc(GF_VolumeTip);
	{
		PROP_ITEM pi;
		pi.Val = prop.fVolume;
		pi.strName = "音量";
		pi.strDesc = "单位dB";
		pi.pRange = &s_rangeVolume;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.fVolumeRandomization;
		pi.strName = "音量随机";
		pi.strDesc = "单位dB";
		pi.pRange = &s_rangeVolume;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangePitch.SetRange(0.0f, 24.0f);
	s_rangePitch.SetStep(0.1);
	s_rangePitch.SetUnit("Tone");
	s_rangePitch.SetTipFunc(GF_PitchTip);
	{
		PROP_ITEM pi;
		pi.Val = prop.fPitch;
		pi.strName = "音高";
		pi.strDesc = "单位Tone";
		pi.pRange = &s_rangePitch;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.fPitchRandomization;
		pi.strName = "音高随机";
		pi.strDesc = "单位Tone";
		pi.pRange = &s_rangePitch;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeIntervalTime.SetRange(0, 1200000);
	s_rangeIntervalTime.SetStep(1);
	s_rangeIntervalTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = prop.iIntervalTime;
		pi.strName = "时间间隔";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.iIntervalTimeRandomization;
		pi.strName = "时间间隔随机";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	return true;
}

void CSoundDefPropList::PropChanged(SOUND_DEF_PROPERTY& prop, bool bSave, bool bIsCommit, int iStartIdx)
{
	if(bSave)
	{
		int idx = iStartIdx;
		prop.playMode = (PLAY_MODE)any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.fVolume = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fVolumeRandomization = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fPitch = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fPitchRandomization = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.iIntervalTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iIntervalTimeRandomization = any_cast<int>(m_pProperties->GetPropVal(idx++));
	}
	else
	{
		int idx = iStartIdx;
		m_pProperties->SetPropVal(idx++, (int)prop.playMode);
		m_pProperties->SetPropVal(idx++, prop.fVolume);
		m_pProperties->SetPropVal(idx++, prop.fVolumeRandomization);
		m_pProperties->SetPropVal(idx++, prop.fPitch);
		m_pProperties->SetPropVal(idx++, prop.fPitchRandomization);
		m_pProperties->SetPropVal(idx++, prop.iIntervalTime);
		m_pProperties->SetPropVal(idx++, prop.iIntervalTimeRandomization);
	}
}