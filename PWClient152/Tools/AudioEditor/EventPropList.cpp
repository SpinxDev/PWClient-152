#include "StdAfx.h"
#include "EventPropList.h"
#include "Global.h"
#include "PropListCtrl/BCGPPropertyObject.h"
#include "PropListCtrl/BCGPPropertyListWrapper.h"

using AudioEngine::EVENT_MODE;
using AudioEngine::EVENT_3D_ROLLOFF;
using AudioEngine::MAX_PLAYBACK_BEHAVIOR;

static prop_range_template<float> s_rangeVolume;
static prop_range_template<float> s_rangePitch;
static prop_range_template<int> s_rangeFadeTime;
static prop_set_template<int> s_setFadeInType;
static prop_set_template<int> s_setFadeOutType;
static prop_set_template<int> s_setMode;
static prop_set_template<int> s_setRolloff;
static prop_range_template<float> s_rangeDistance;
static prop_range_template<float> s_rangeAngle;
static prop_range_template<int> s_rangeMaxPlayBacks;
static prop_set_template<int> s_setMaxPlayBacksBehavior;
static prop_range_template<int> s_rangeIntervalTime;
static prop_range_template<int> s_rangePriority;

CEventPropList::CEventPropList(void)
{
	m_pProperties = 0;
	m_pBCGPPropListWrapper = 0;
}

CEventPropList::~CEventPropList(void)
{
}

bool CEventPropList::Init(CPropertyArray* pProperties, CBCGPPropertyListWrapper* pBCGPPropListWrapper)
{
	if(!pProperties || !pBCGPPropListWrapper)
		return false;
	m_pProperties = pProperties;
	m_pBCGPPropListWrapper = pBCGPPropListWrapper;
	return true;
}

bool CEventPropList::BuildProperty(const EVENT_PROPERTY& prop)
{
	m_Prop = prop;
	if(!m_pProperties)
		return false;
	s_rangeVolume.SetRange(-60.0f, 0.0f);
	s_rangeVolume.SetStep(0.1);
	s_rangeVolume.SetUnit("dB");
	s_rangeVolume.SetTipFunc(GF_VolumeTip);
	{
		PROP_ITEM pi;
		pi.Val = prop.fVolume;
		pi.strName = "����";
		pi.strDesc = "��λdB";
		pi.pRange = &s_rangeVolume;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.fVolumeRandomization;
		pi.strName = "�������";
		pi.strDesc = "��λdB";
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
		pi.strName = "����";
		pi.strDesc = "��λTone";
		pi.pRange = &s_rangePitch;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.fPitchRandomization;
		pi.strName = "�������";
		pi.strDesc = "��λTone";
		pi.pRange = &s_rangePitch;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeFadeTime.SetRange(0, 60000);
	s_rangeFadeTime.SetStep(1);
	s_rangeFadeTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = prop.iFadeInTime;
		pi.strName = "����ʱ��";
		pi.strDesc = "��λ����";
		pi.pRange = &s_rangeFadeTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setFadeInType.GetCount())
	{
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_LINEAR, "����");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_SIN, "S������");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_COS, "͹������");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_POW, "��������");
	}
	{
		PROP_ITEM pi;
		pi.Val = prop.iFadeInCurveType;
		pi.strName = "��������";
		pi.strDesc = "";
		pi.pOptions = &s_setFadeInType;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.iFadeOutTime;
		pi.strName = "����ʱ��";
		pi.strDesc = "��λ����";
		pi.pRange = &s_rangeFadeTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setFadeOutType.GetCount())
	{
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_LINEAR, "����");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_SIN, "S���½�");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_COS, "͹���½�");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_POW, "�����½�");
	}
	{
		PROP_ITEM pi;
		pi.Val = prop.iFadeOutCurveType;
		pi.strName = "��������";
		pi.strDesc = "";
		pi.pOptions = &s_setFadeOutType;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setMode.GetCount())
	{
		s_setMode.AddElement((int)AudioEngine::MODE_2D, "2D");
		s_setMode.AddElement((int)AudioEngine::MODE_3D, "3D");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)prop.mode;
		pi.strName = "ģʽ";
		pi.strDesc = "";
		pi.pOptions = &s_setMode;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	bool bReadOnly = false;
	if(prop.mode == AudioEngine::MODE_2D)
		bReadOnly = true;

	if(0 == s_setRolloff.GetCount())
	{
		s_setRolloff.AddElement((int)AudioEngine::ROLLOFF_INVERSE, "����˥��(Inverse)");
		s_setRolloff.AddElement((int)AudioEngine::ROLLOFF_LINEAR, "����˥��(Linear)");
		s_setRolloff.AddElement((int)AudioEngine::ROLLOFF_LINEAR_SQUARE, "ƽ������˥��(Linear-Square)");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)prop.rolloff;
		pi.strName = "˥����ʽ";
		pi.strDesc = "3Dģʽ����Ч�����ݾ���˥��";
		pi.bIsReadOnly = bReadOnly;
		pi.pOptions = &s_setRolloff;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	s_rangeDistance.SetRange(0.0f, 10000.0f);
	s_rangeDistance.SetStep(0.01);
	s_rangeDistance.SetUnit("m");
	{
		PROP_ITEM pi;
		pi.Val = prop.fMinDistance;
		pi.strName = "��С����";
		pi.strDesc = "3Dģʽ����Ч";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeDistance;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.fMaxDistance;
		pi.strName = "������";
		pi.strDesc = "3Dģʽ����Ч";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeDistance;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeAngle.SetRange(0.0f, 360.0f);
	s_rangeAngle.SetStep(0.01);
	s_rangeAngle.SetUnit("��");
	{
		PROP_ITEM pi;
		pi.Val = prop.f3DConeInsideAngle;
		pi.strName = "3D Cone Inside Angle";
		pi.strDesc = "3Dģʽ����Ч";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeAngle;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	{
		PROP_ITEM pi;
		pi.Val = prop.f3DConeOutsideAngle;
		pi.strName = "3D Cone Outside Angle";
		pi.strDesc = "3Dģʽ����Ч";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeAngle;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	{
		PROP_ITEM pi;
		pi.Val = GF_ConvertVolumeFromRatio2DB(prop.f3DConeOutsideVolume);
		pi.strName = "3D Cone Outside Volume";
		pi.strDesc = "3Dģʽ����Ч����λdB";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeVolume;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = prop.f3DSpeakerSpread;
		pi.strName = "3D Speaker Spread";
		pi.strDesc = "���������������ֱ�����Է�����ת��ļнǡ�ȡֵ180ʱ�ָ�������Ч����ȡֵ360ʱ����λ�÷��򣬱�����ǰ3D�������������3Dģʽ����Ч����λ��";
		pi.bIsReadOnly = bReadOnly;
		pi.pRange = &s_rangeAngle;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeMaxPlayBacks.SetRange(1, 500);
	s_rangeMaxPlayBacks.SetStep(1);
	{
		PROP_ITEM pi;
		pi.Val = (int)prop.uiMaxPlaybacks;
		pi.strName = "���ͬʱ��������";
		pi.strDesc = "";
		pi.pRange = &s_rangeMaxPlayBacks;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setMaxPlayBacksBehavior.GetCount())
	{
		s_setMaxPlayBacksBehavior.AddElement((int)AudioEngine::MPB_STEAL_OLDEST, "�滻���ϵ�");
		s_setMaxPlayBacksBehavior.AddElement((int)AudioEngine::MPB_STEAL_NEWEST, "�滻���µ�");
		s_setMaxPlayBacksBehavior.AddElement((int)AudioEngine::MPB_STEAL_QUIETEST, "�滻������С��");
		s_setMaxPlayBacksBehavior.AddElement((int)AudioEngine::MPB_JUST_FAIL, "��������");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)prop.maxPlaybacksBehavior;
		pi.strName = "����󲥷�����ʽ";
		pi.strDesc = "";
		pi.pOptions = &s_setMaxPlayBacksBehavior;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	s_rangeIntervalTime.SetRange(0, 60000);
	s_rangeIntervalTime.SetStep(1);
	s_rangeIntervalTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = prop.iMinIntervalTime;
		pi.strName = "������С���ʱ��";
		pi.strDesc = "��λ����";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangePriority.SetRange(0, 20000);
	s_rangePriority.SetStep(1);
	{
		PROP_ITEM pi;
		pi.Val = prop.iPriority;
		pi.strName = "���ȼ�";
		pi.strDesc = "��ֵԽ�����ȼ�Խ��";
		pi.pRange = &s_rangePriority;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	return true;
}

void CEventPropList::PropChanged(EVENT_PROPERTY& prop, bool bSave, bool bIsCommit, int iStartIdx)
{
	if(bSave)
	{
		int idx = iStartIdx;
		prop.fVolume = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fVolumeRandomization = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fPitch = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fPitchRandomization = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.iFadeInTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iFadeInCurveType = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iFadeOutTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iFadeOutCurveType = any_cast<int>(m_pProperties->GetPropVal(idx++));
		EVENT_MODE oldMode = prop.mode;
		prop.mode = (EVENT_MODE)any_cast<int>(m_pProperties->GetPropVal(idx++));		
		prop.rolloff = (EVENT_3D_ROLLOFF)any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.fMinDistance = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.fMaxDistance = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.f3DConeInsideAngle = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.f3DConeOutsideAngle = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.f3DConeOutsideVolume = GF_ConvertVolumeFromDB2Ratio(any_cast<float>(m_pProperties->GetPropVal(idx++)));
		prop.f3DSpeakerSpread = any_cast<float>(m_pProperties->GetPropVal(idx++));
		prop.uiMaxPlaybacks = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.maxPlaybacksBehavior = (MAX_PLAYBACK_BEHAVIOR)any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iMinIntervalTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		prop.iPriority = any_cast<int>(m_pProperties->GetPropVal(idx++));
		if(oldMode != prop.mode)
		{
			if(prop.mode == AudioEngine::MODE_2D)
			{
				CPropertyItem* pItem = m_pProperties->GetPropertyPtr(iStartIdx + 9);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 10);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 11);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 12);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 13);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 14);
				pItem->SetReadOnly(true);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 15);
				pItem->SetReadOnly(true);
			}
			else if(prop.mode == AudioEngine::MODE_3D)
			{
				CPropertyItem* pItem = m_pProperties->GetPropertyPtr(iStartIdx + 9);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 10);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 11);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 12);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 13);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 14);
				pItem->SetReadOnly(false);
				pItem = m_pProperties->GetPropertyPtr(iStartIdx + 15);
				pItem->SetReadOnly(false);
			}
			m_pBCGPPropListWrapper->AttachTo(m_pProperties);
		}
	}
	else
	{
		int idx = iStartIdx;
		m_pProperties->SetPropVal(idx++, prop.fVolume);
		m_pProperties->SetPropVal(idx++, prop.fVolumeRandomization);
		m_pProperties->SetPropVal(idx++, prop.fPitch);
		m_pProperties->SetPropVal(idx++, prop.fPitchRandomization);
		m_pProperties->SetPropVal(idx++, prop.iFadeInTime);
		m_pProperties->SetPropVal(idx++, prop.iFadeInCurveType);
		m_pProperties->SetPropVal(idx++, prop.iFadeOutTime);
		m_pProperties->SetPropVal(idx++, prop.iFadeOutCurveType);
		m_pProperties->SetPropVal(idx++, (int)prop.mode);
		m_pProperties->SetPropVal(idx++, (int)prop.rolloff);
		m_pProperties->SetPropVal(idx++, prop.fMinDistance);
		m_pProperties->SetPropVal(idx++, prop.fMaxDistance);
		m_pProperties->SetPropVal(idx++, prop.f3DConeInsideAngle);
		m_pProperties->SetPropVal(idx++, prop.f3DConeOutsideAngle);
		m_pProperties->SetPropVal(idx++, prop.f3DSpeakerSpread);
		m_pProperties->SetPropVal(idx++, GF_ConvertVolumeFromRatio2DB(prop.f3DConeOutsideVolume));
		m_pProperties->SetPropVal(idx++, (int)prop.uiMaxPlaybacks);
		m_pProperties->SetPropVal(idx++, (int)prop.maxPlaybacksBehavior);
		m_pProperties->SetPropVal(idx++, prop.iMinIntervalTime);
		m_pProperties->SetPropVal(idx++, prop.iPriority);
	}
}