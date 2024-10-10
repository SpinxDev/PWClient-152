// DlgEffectProp.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgEffectProp.h"
#include "FEffect.h"
#include "Global.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

static prop_range_template<float> s_rangeUnit;
static prop_range_template<float> s_rangePan;
static prop_range_template<float> s_rangeLFE;
static prop_range_template<float> s_rangeCenterFreq;
static prop_range_template<float> s_rangeOctaveRange;
static prop_range_template<float> s_rangeFreqGain;

static prop_range_template<float> s_rangeAttack;
static prop_range_template<float> s_rangeRelease;
static prop_range_template<float> s_rangeMakeUpGain;
static prop_range_template<float> s_rangeThreshold;

static prop_range_template<float> s_rangeDelayTime;
static prop_range_template<float> s_rangeMaxChannels;


// CDlgEffectProp 对话框

IMPLEMENT_DYNAMIC(CDlgEffectProp, CBCGPDialog)

CDlgEffectProp::CDlgEffectProp(Effect* pEffect, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgEffectProp::IDD, pParent)
	, m_pEffect(pEffect)
	, m_strEffectType(_T(""))
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgEffectProp::~CDlgEffectProp()
{
	delete m_pPropertyListener;
	m_pPropertyListener = 0;
	if (m_pProperties)
		m_pProperties->Clear();
	delete m_pProperties;
	m_pProperties = 0;
	delete m_pBCGPPropListWrapper;
	m_pBCGPPropListWrapper = 0;
}

void CDlgEffectProp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EFFECT_TYPE, m_strEffectType);
}


BEGIN_MESSAGE_MAP(CDlgEffectProp, CBCGPDialog)
END_MESSAGE_MAP()


// CDlgEffectProp 消息处理程序

BOOL CDlgEffectProp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	if(!m_pEffect)
		return FALSE;

	CRect rc;
	GetDlgItem(IDC_PROP_FRAME)->GetWindowRect(&rc);
	ScreenToClient(&rc);	
	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;

	fillData();
	buildProperty();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgEffectProp::buildProperty()
{
	m_pProperties->Clear();
	s_rangeUnit.SetRange(0.0f, 1.0f);
	s_rangeUnit.SetStep(0.001);

	s_rangePan.SetRange(-1.0f, 1.0f);
	s_rangePan.SetStep(0.001);

	switch (m_pEffect->GetType())
	{
	case AudioEngine::EFFECT_TYPE_NONE:
		return;
	case AudioEngine::EFFECT_TYPE_VOLUME:
		{
			PROP_ITEM pi;
			pi.Val = m_VolumeData.fVolume;
			pi.strName = "Volume";
			pi.strDesc = "";
			pi.pRange = &s_rangeUnit;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}
		break;
	case AudioEngine::EFFECT_TYPE_PITCH:
		{
			PROP_ITEM pi;
			pi.Val = m_PitchData.fPitch;
			pi.strName = "Pitch";
			pi.strDesc = "";
			pi.pRange = &s_rangeUnit;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}
		break;
	case AudioEngine::EFFECT_TYPE_PAN:
		{
			PROP_ITEM pi;
			pi.Val = m_PanData.fPan;
			pi.strName = "Pan";
			pi.strDesc = "";
			pi.pRange = &s_rangePan;
			pi.iWay = BCGP_PROP_WAY_RANGE;
			m_pProperties->AddProperty(pi);
		}
		break;
	case AudioEngine::EFFECT_TYPE_SURROUND_PAN:
		{
			{
				PROP_ITEM pi;
				pi.Val = m_SurroundPanData.fLeftRightPan;
				pi.strName = "Left/Right pan";
				pi.strDesc = "";
				pi.pRange = &s_rangePan;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			{
				PROP_ITEM pi;
				pi.Val = m_SurroundPanData.fBackFrontPan;
				pi.strName = "Back/Front pan";
				pi.strDesc = "";
				pi.pRange = &s_rangePan;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeLFE.SetRange(0.0f, 5.0f);
			s_rangeLFE.SetStep(0.01);
			{
				PROP_ITEM pi;
				pi.Val = m_SurroundPanData.fLFE;
				pi.strName = "LFE send";
				pi.strDesc = "";
				pi.pRange = &s_rangeLFE;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_PARAMEQ:
		{
			s_rangeCenterFreq.SetRange(20.0f, 22000.0f);
			s_rangeCenterFreq.SetStep(1.0);
			s_rangeCenterFreq.SetUnit("Hz");
			{
				PROP_ITEM pi;
				pi.Val = m_ParamEQData.fCenterFreq;
				pi.strName = "Center Freq";
				pi.strDesc = "单位Hz";
				pi.pRange = &s_rangeCenterFreq;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeOctaveRange.SetRange(0.2f, 5.0f);
			s_rangeOctaveRange.SetStep(0.01);
			s_rangeOctaveRange.SetUnit("octaves");
			{
				PROP_ITEM pi;
				pi.Val = m_ParamEQData.fOctaveRange;
				pi.strName = "Octave Range";
				pi.strDesc = "单位octaves";
				pi.pRange = &s_rangeOctaveRange;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeFreqGain.SetRange(-26.02f, 9.54f);
			s_rangeFreqGain.SetStep(0.01);
			s_rangeFreqGain.SetUnit("dB");
			{
				PROP_ITEM pi;
				pi.Val = m_ParamEQData.fFreqGain;
				pi.strName = "Freq Gain";
				pi.strDesc = "单位dB";
				pi.pRange = &s_rangeFreqGain;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_COMPRESSOR:
		{
			s_rangeThreshold.SetRange(-60.0f, 0.0f);
			s_rangeThreshold.SetStep(0.1f);
			s_rangeThreshold.SetUnit("dB");
			s_rangeThreshold.SetTipFunc(GF_VolumeTip);
			{
				PROP_ITEM pi;
				pi.Val = m_CompressorData.fThreshold;
				pi.strName = "Threshold";
				pi.strDesc = "单位dB";
				pi.pRange = &s_rangeThreshold;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeAttack.SetRange(10.0f, 200.0f);
			s_rangeAttack.SetStep(0.1);
			s_rangeAttack.SetUnit("ms");
			{
				PROP_ITEM pi;
				pi.Val = m_CompressorData.fAttack;
				pi.strName = "Attack";
				pi.strDesc = "单位ms";
				pi.pRange = &s_rangeAttack;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}


			s_rangeRelease.SetRange(20.0f, 1000.0f);
			s_rangeRelease.SetStep(0.1);
			s_rangeRelease.SetUnit("ms");
			{
				PROP_ITEM pi;
				pi.Val = m_CompressorData.fRelease;
				pi.strName = "Release";
				pi.strDesc = "单位ms";
				pi.pRange = &s_rangeRelease;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeMakeUpGain.SetRange(0.0f, 30.0f);
			s_rangeMakeUpGain.SetStep(0.1);
			s_rangeMakeUpGain.SetUnit("dB");
			{
				PROP_ITEM pi;
				pi.Val = m_CompressorData.fMakeUpGain;
				pi.strName = "Make Up Gain";
				pi.strDesc = "单位dB";
				pi.pRange = &s_rangeMakeUpGain;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}		
		}
		break;
	case AudioEngine::EFFECT_TYPE_ECHO:
		{
			s_rangeDelayTime.SetRange(1.0f, 5000.0f);
			s_rangeDelayTime.SetStep(0.1);
			s_rangeDelayTime.SetUnit("ms");
			{
				PROP_ITEM pi;
				pi.Val = m_EchoData.fDelay;
				pi.strName = "Delay Time";
				pi.strDesc = "单位ms";
				pi.pRange = &s_rangeDelayTime;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			{
				PROP_ITEM pi;
				pi.Val = m_EchoData.fDecay;
				pi.strName = "Decay";
				pi.strDesc = "";
				pi.pRange = &s_rangeUnit;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			s_rangeMaxChannels.SetRange(0.0f, 16.0f);
			s_rangeMaxChannels.SetStep(0.1);
			s_rangeMaxChannels.SetUnit("channels");
			{
				PROP_ITEM pi;
				pi.Val = m_EchoData.fChannels;
				pi.strName = "MaxChannels";
				pi.strDesc = "channels";
				pi.pRange = &s_rangeMaxChannels;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			{
				PROP_ITEM pi;
				pi.Val = m_EchoData.fDryMix;
				pi.strName = "Dry Mix";
				pi.strDesc = "";
				pi.pRange = &s_rangeUnit;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			{
				PROP_ITEM pi;
				pi.Val = m_EchoData.fWetMix;
				pi.strName = "Wet Mix";
				pi.strDesc = "";
				pi.pRange = &s_rangeUnit;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_OCCLUSION:
		{
			{
				PROP_ITEM pi;
				pi.Val = m_OcclusionData.fDirect;
				pi.strName = "Direct";
				pi.strDesc = "";
				pi.pRange = &s_rangeUnit;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}

			{
				PROP_ITEM pi;
				pi.Val = m_OcclusionData.fReverb;
				pi.strName = "Reverb";
				pi.strDesc = "";
				pi.pRange = &s_rangeUnit;
				pi.iWay = BCGP_PROP_WAY_RANGE;
				m_pProperties->AddProperty(pi);
			}
		}
		break;		
	}
	m_pBCGPPropListWrapper->AttachTo(m_pProperties);
}

void CDlgEffectProp::fillData()
{
	m_strEffectType = m_pEffect->GetName();

	switch (m_pEffect->GetType())
	{
	case AudioEngine::EFFECT_TYPE_NONE:
		return;
	case AudioEngine::EFFECT_TYPE_VOLUME:
		{
			if(!m_pEffect->GetParameter(idVolume, m_VolumeData.fVolume))
				return;			
		}
		break;
	case AudioEngine::EFFECT_TYPE_PITCH:
		{
			if(!m_pEffect->GetParameter(idPitch, m_PitchData.fPitch))
				return;			
		}
		break;
	case AudioEngine::EFFECT_TYPE_PAN:
		{
			if(!m_pEffect->GetParameter(idPan, m_PanData.fPan))
				return;			
		}
		break;
	case AudioEngine::EFFECT_TYPE_SURROUND_PAN:
		{
			if(!m_pEffect->GetParameter(idLeftRightPan, m_SurroundPanData.fLeftRightPan))
				return;
			if(!m_pEffect->GetParameter(idBackFrontPan, m_SurroundPanData.fBackFrontPan))
				return;
			if(!m_pEffect->GetParameter(idLFE, m_SurroundPanData.fLFE))
				return;
		}
		break;
	case AudioEngine::EFFECT_TYPE_PARAMEQ:
		{
			if(!m_pEffect->GetParameter(idCenterFreq, m_ParamEQData.fCenterFreq))
				return;
			if(!m_pEffect->GetParameter(idOctaveRange, m_ParamEQData.fOctaveRange))
				return;
			if(!m_pEffect->GetParameter(idFreqGain, m_ParamEQData.fFreqGain))
				return;
		}
		break;
	case AudioEngine::EFFECT_TYPE_COMPRESSOR:
		{
			if(!m_pEffect->GetParameter(idThreshold, m_CompressorData.fThreshold))
				return;
			if(!m_pEffect->GetParameter(idAttack, m_CompressorData.fAttack))
				return;
			if(!m_pEffect->GetParameter(idRelease, m_CompressorData.fRelease))
				return;
			if(!m_pEffect->GetParameter(idMakeUpGain, m_CompressorData.fMakeUpGain))
				return;
		}
		break;
	case AudioEngine::EFFECT_TYPE_ECHO:
		{
			if(!m_pEffect->GetParameter(idDelay, m_EchoData.fDelay))
				return;
			if(!m_pEffect->GetParameter(idDecay, m_EchoData.fDecay))
				return;
			if(!m_pEffect->GetParameter(idChannels, m_EchoData.fChannels))
				return;
			if(!m_pEffect->GetParameter(idDryMix, m_EchoData.fDryMix))
				return;
			if(!m_pEffect->GetParameter(idWetMix, m_EchoData.fWetMix))
				return;
		}
		break;
	case AudioEngine::EFFECT_TYPE_OCCLUSION:
		{
			if(!m_pEffect->GetParameter(idDirect, m_OcclusionData.fDirect))
				return;
			if(!m_pEffect->GetParameter(idReverb, m_OcclusionData.fReverb))
				return;
		}
		break;
	}
	UpdateData(FALSE);
}

void CDlgEffectProp::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	if(!m_pEffect)
		return;

	switch (m_pEffect->GetType())
	{
	case AudioEngine::EFFECT_TYPE_NONE:
		break;
	case AudioEngine::EFFECT_TYPE_VOLUME:
		{
			if(bSave)
			{
				m_VolumeData.fVolume = any_cast<float>(m_pProperties->GetPropVal(0));
			}
			else
			{
				m_pProperties->SetPropVal(0, m_VolumeData.fVolume);
			}			
		}
		break;
	case AudioEngine::EFFECT_TYPE_PITCH:
		{
			if(bSave)
			{
				m_PitchData.fPitch = any_cast<float>(m_pProperties->GetPropVal(0));
			}
			else
			{
				m_pProperties->SetPropVal(0, m_PitchData.fPitch);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_PAN:
		{
			if(bSave)
			{
				m_PanData.fPan = any_cast<float>(m_pProperties->GetPropVal(0));
			}
			else
			{
				m_pProperties->SetPropVal(0, m_PanData.fPan);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_SURROUND_PAN:
		{
			if(bSave)
			{
				int idx = 0;
				m_SurroundPanData.fLeftRightPan = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_SurroundPanData.fBackFrontPan = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_SurroundPanData.fLFE = any_cast<float>(m_pProperties->GetPropVal(idx++));
			}
			else
			{
				int idx = 0;
				m_pProperties->SetPropVal(idx++, m_SurroundPanData.fLeftRightPan);
				m_pProperties->SetPropVal(idx++, m_SurroundPanData.fBackFrontPan);
				m_pProperties->SetPropVal(idx++, m_SurroundPanData.fLFE);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_PARAMEQ:
		{
			if(bSave)
			{
				int idx = 0;
				m_ParamEQData.fCenterFreq = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_ParamEQData.fOctaveRange = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_ParamEQData.fFreqGain = any_cast<float>(m_pProperties->GetPropVal(idx++));
			}
			else
			{
				int idx = 0;
				m_pProperties->SetPropVal(idx++, m_ParamEQData.fCenterFreq);
				m_pProperties->SetPropVal(idx++, m_ParamEQData.fOctaveRange);
				m_pProperties->SetPropVal(idx++, m_ParamEQData.fFreqGain);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_COMPRESSOR:
		{
			if(bSave)
			{
				int idx = 0;
				m_CompressorData.fThreshold = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_CompressorData.fAttack = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_CompressorData.fRelease = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_CompressorData.fMakeUpGain = any_cast<float>(m_pProperties->GetPropVal(idx++));
			}
			else
			{
				int idx = 0;
				m_pProperties->SetPropVal(idx++, m_CompressorData.fThreshold);
				m_pProperties->SetPropVal(idx++, m_CompressorData.fAttack);
				m_pProperties->SetPropVal(idx++, m_CompressorData.fRelease);
				m_pProperties->SetPropVal(idx++, m_CompressorData.fMakeUpGain);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_ECHO:
		{
			if(bSave)
			{
				int idx = 0;
				m_EchoData.fDelay = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_EchoData.fDecay = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_EchoData.fChannels = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_EchoData.fDryMix = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_EchoData.fWetMix = any_cast<float>(m_pProperties->GetPropVal(idx++));
			}
			else
			{
				int idx = 0;
				m_pProperties->SetPropVal(idx++, m_EchoData.fDelay);
				m_pProperties->SetPropVal(idx++, m_EchoData.fDecay);
				m_pProperties->SetPropVal(idx++, m_EchoData.fChannels);
				m_pProperties->SetPropVal(idx++, m_EchoData.fDryMix);
				m_pProperties->SetPropVal(idx++, m_EchoData.fWetMix);
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_OCCLUSION:
		{
			if(bSave)
			{
				int idx = 0;
				m_OcclusionData.fDirect = any_cast<float>(m_pProperties->GetPropVal(idx++));
				m_OcclusionData.fReverb = any_cast<float>(m_pProperties->GetPropVal(idx++));
			}
			else
			{
				int idx = 0;
				m_pProperties->SetPropVal(idx++, m_OcclusionData.fDirect);
				m_pProperties->SetPropVal(idx++, m_OcclusionData.fReverb);
			}
		}
		break;
	}
}
