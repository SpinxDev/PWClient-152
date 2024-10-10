#include "FEffect.h"
#include "FPropCurves.h"
#include "xml/xmlcommon.h"
#include "FEventSystem.h"
#include <fmod.hpp>
#include <AFileImage.h>

static const unsigned int version = 0x100001;

using namespace AudioEngine;

Effect::Effect(void)
: m_EffectType(EFFECT_TYPE_NONE)
, m_bEnable(true)
{
}

Effect::~Effect(void)
{
	release();
}

void Effect::release()
{
	for (size_t i=0; i<m_arrProp.size(); ++i)
	{
		delete m_arrProp[i];
	}
	m_arrProp.clear();
}

PropCurves* Effect::GetPropCurve(int idx) const
{
	return m_arrProp[idx];
}

bool Effect::Load(AFileImage* pFile)
{
	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iParamNum = 0;
	pFile->Read(&iParamNum, sizeof(iParamNum), &dwReadLen);
	for (int i=0; i<iParamNum; ++i)
	{
		if(!m_arrProp[i]->Load(pFile))
			return false;
		if(!SetParameter(i, m_arrProp[i]->GetDefault()))
			return false;
	}
	return true;
}

bool Effect::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iParamNum = GetNumParameters();
	pFile->Write(&iParamNum, sizeof(iParamNum), &dwWriteLen);
	for (int i=0; i<iParamNum; ++i)
	{
		if(!m_arrProp[i]->Save(pFile))
			return false;
	}
	return true;
}

bool Effect::LoadXML(TiXmlElement* root)
{
	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	int iParamNum = 0;
	QueryElement(root, "ParamNum", iParamNum);

	TiXmlNode* pNode = root->FirstChild("PropCurves");
	for (int i=0; i<iParamNum; ++i)
	{
		if(!m_arrProp[i]->LoadXML(pNode->ToElement()))
			return false;
		if(!SetParameter(i, m_arrProp[i]->GetDefault()))
			return false;
		pNode = pNode->NextSibling("PropCurves");
	}
	return true;
}

bool Effect::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("Effect");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	int iParamNum = GetNumParameters();
	AddElement(root, "ParamNum", iParamNum);

	for (int i=0; i<iParamNum; ++i)
	{
		if(!m_arrProp[i]->SaveXML(root))
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//EffectDsp
//////////////////////////////////////////////////////////////////////////
EffectDsp::EffectDsp(void)
{
	m_pDSP = 0;
	m_pChannelGroup = 0;
}

EffectDsp::~EffectDsp(void)
{
	release();
}

bool EffectDsp::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup || !m_pDSP)
		return false;
	int iParamNum = GetNumParameters();
	for (int i=0; i<iParamNum; ++i)
	{
		SetParameter(i, m_arrProp[i]->GetValue(fParamValue));
	}
	if(m_pChannelGroup != pChannelGroup)
	{
		if(m_pChannelGroup && !Remove(m_pChannelGroup))
			return false;
		m_pChannelGroup = pChannelGroup;
		if(FMOD_OK != pChannelGroup->addDSP(m_pDSP, 0))
			return false;
	}
	return true;
}

bool EffectDsp::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup || !m_pDSP)
		return false;
	if(FMOD_OK != m_pDSP->remove())
		return false;
	m_pChannelGroup = 0;
	return true;
}

bool EffectDsp::SetParameter(int index, float value)
{
	if(!m_pDSP)
		return false;
	m_arrProp[index]->SetDefault(value);
	if(FMOD_OK != m_pDSP->setParameter(index, value))
		return false;
	return true;
}

bool EffectDsp::GetParameter(int index, float& value)
{
	int iNum = GetNumParameters();
	if(index < 0 || index >= iNum)
		return false;
	if(!m_pDSP)
		return false;
	value = m_arrProp[index]->GetDefault();
	if(!SetParameter(index, value))
		return false;
	return true;
}

int EffectDsp::GetNumParameters() const
{
	if(!m_pDSP)
		return 0;
	int iNum = 0;
	if(FMOD_OK != m_pDSP->getNumParameters(&iNum))
		return 0;
	return iNum;
}

void EffectDsp::release()
{
	Effect::release();
	if(m_pDSP)
		m_pDSP->release();
	m_pDSP = 0;
}

//////////////////////////////////////////////////////////////////////////
//EffectVolume
//////////////////////////////////////////////////////////////////////////
EffectVolume::EffectVolume(void)
{
}

EffectVolume::~EffectVolume(void)
{
	release();
}

bool EffectVolume::Init(EventSystem* pEventSystem)
{
	release();

	if(!pEventSystem)
		return false;
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(1, 0, 1);
	pPropCurves->SetName("Volume");
	m_EffectType = EFFECT_TYPE_VOLUME;
	m_arrProp.push_back(pPropCurves);
	return true;
}

bool EffectVolume::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup)
		return false;
	float fVolume = m_arrProp[0]->GetValue(fParamValue);
	if(FMOD_OK != pChannelGroup->setVolume(fVolume))
		return false;
	return true;
}

bool EffectVolume::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup)
		return false;
	if(FMOD_OK != pChannelGroup->setVolume(1.0f))
		return false;
	return true;
}

bool EffectVolume::SetParameter(int index, float value)
{
	if(index != 0)
		return false;
	if(value < 0 || value > 1)
		return false;
	m_arrProp[index]->SetDefault(value);
	return true;
}

bool EffectVolume::GetParameter(int index, float& value)
{
	if(index != 0)
		return false;
	value = m_arrProp[index]->GetDefault();
	return true;
}

void EffectVolume::release()
{
	Effect::release();
}


//////////////////////////////////////////////////////////////////////////
//EffectPitch
//////////////////////////////////////////////////////////////////////////
EffectPitch::EffectPitch(void)
{
}

EffectPitch::~EffectPitch(void)
{
	release();
}

bool EffectPitch::Init(EventSystem* pEventSystem)
{
	release();

	if(!pEventSystem)
		return false;
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(1, 0, 1);
	pPropCurves->SetName("Pitch");
	m_EffectType = EFFECT_TYPE_PITCH;
	m_arrProp.push_back(pPropCurves);
	return true;
}

bool EffectPitch::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup)
		return false;
	float fPitch = m_arrProp[0]->GetValue(fParamValue);
	if(FMOD_OK != pChannelGroup->setPitch(fPitch))
		return false;
	return true;
}

bool EffectPitch::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup)
		return false;
	if(FMOD_OK != pChannelGroup->setPitch(1.0f))
		return false;
	return true;
}

bool EffectPitch::SetParameter(int index, float value)
{
	if(index != 0)
		return false;
	if(value < 0 || value > 1)
		return false;
	m_arrProp[index]->SetDefault(value);
	return true;
}

bool EffectPitch::GetParameter(int index, float& value)
{
	if(index != 0)
		return false;
	value = m_arrProp[index]->GetDefault();
	return true;
}

void EffectPitch::release()
{
	Effect::release();
}

//////////////////////////////////////////////////////////////////////////
//EffectPan
//////////////////////////////////////////////////////////////////////////
EffectPan::EffectPan(void)
{
}

EffectPan::~EffectPan(void)
{
	release();
}

bool EffectPan::Init(EventSystem* pEventSystem)
{
	release();

	if(!pEventSystem)
		return false;	
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(0, -1, 1);
	pPropCurves->SetName("Pan");
	m_EffectType = EFFECT_TYPE_PAN;
	m_arrProp.push_back(pPropCurves);
	return true;
}

bool EffectPan::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup)
		return false;
	float fPan = m_arrProp[0]->GetValue(fParamValue);
	if(FMOD_OK != pChannelGroup->overridePan(fPan))
		return false;
	return true;
}

bool EffectPan::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup)
		return false;
	if(FMOD_OK != pChannelGroup->overridePan(0))
		return false;
	return true;
}

bool EffectPan::SetParameter(int index, float value)
{
	if(index != 0)
		return false;
	if(value < -1 || value > 1)
		return false;
	m_arrProp[index]->SetDefault(value);
	return true;
}

bool EffectPan::GetParameter(int index, float& value)
{
	if(index != 0)
		return false;
	value = m_arrProp[index]->GetDefault();
	return true;
}

int EffectPan::GetNumParameters() const
{
	return 1;
}

void EffectPan::release()
{
	Effect::release();
}

//////////////////////////////////////////////////////////////////////////
//EffectSurroundPan
//////////////////////////////////////////////////////////////////////////
enum
{
	PARAM_LEFT_RIGHT_PAN,
	PARAM_BACK_FRONT_PAN,
	PARAM_LFE
};


EffectSurroundPan::EffectSurroundPan(void)
{
}

EffectSurroundPan::~EffectSurroundPan(void)
{
	release();
}

bool EffectSurroundPan::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_EffectType = EFFECT_TYPE_SURROUND_PAN;
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(0, -1, 1);
	pPropCurves->SetName("Left/Right Pan");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0, -1, 1);
	pPropCurves->SetName("Back/Front Pan");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0, 0, 5);
	pPropCurves->SetName("LFE");
	m_arrProp.push_back(pPropCurves);
	return true;
}

bool EffectSurroundPan::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup)
		return false;
	float fFrontLeft = 1.0f;
	float fFrontRight = 1.0f;
	float fBackLeft = 1.0f;
	float fBackRight = 1.0f;
	float fSideLeft = 1.0f;
	float fSideRight = 1.0f;
	float fLFE = m_arrProp[PARAM_LFE]->GetValue(fParamValue);
	float fCenter = 1.0f;

	float fLeftRightPan = m_arrProp[PARAM_LEFT_RIGHT_PAN]->GetValue(fParamValue);
	float fBackFrontPan = m_arrProp[PARAM_BACK_FRONT_PAN]->GetValue(fParamValue);

	if(fLeftRightPan > 0)
	{
		fSideRight = 1.0f;
		fBackRight = 1.0f;
		fFrontRight = 1.0f;

		fSideLeft = 1.0f - fLeftRightPan;
		fBackLeft = 1.0f - fLeftRightPan;
		fFrontLeft = 1.0f - fLeftRightPan;
		fCenter = 1.0f - fLeftRightPan;
	}
	else
	{
		fSideLeft = 1.0f;
		fBackLeft = 1.0f;
		fFrontLeft = 1.0f;

		fSideRight = 1.0f + fLeftRightPan;
		fBackRight = 1.0f + fLeftRightPan;
		fFrontRight = 1.0f + fLeftRightPan;
		fCenter = 1.0f + fLeftRightPan;
	}

	if(fBackFrontPan > 0)
	{
		fSideLeft *= (1.0f - fBackFrontPan);
		fSideRight *= (1.0f - fBackFrontPan);
		fCenter *= (1.0f - fBackFrontPan);
		fBackLeft *= (1.0f - fBackFrontPan);
		fBackRight *= (1.0f - fBackFrontPan);
	}
	else
	{
		fSideLeft *= (1.0f + fBackFrontPan);
		fSideRight *= (1.0f + fBackFrontPan);
		fCenter *= (1.0f + fBackFrontPan);
		fFrontLeft *= (1.0f + fBackFrontPan);
		fFrontRight *= (1.0f + fBackFrontPan);
	}

	int iChannelNum = 0;
	if(FMOD_OK != pChannelGroup->getNumChannels(&iChannelNum))
		return false;
	for (int i=0; i<iChannelNum; ++i)
	{
		FMOD::Channel* pChannel = 0;
		if(FMOD_OK != pChannelGroup->getChannel(i, &pChannel))
			return false;
		if(FMOD_OK != pChannel->setSpeakerMix(fFrontLeft, fFrontRight, fCenter, fLFE, fBackLeft, fBackRight, fSideLeft, fSideRight))
			return false;
	}

	int iChannelGroupNum = 0;
	if(FMOD_OK != pChannelGroup->getNumGroups(&iChannelGroupNum))
		return false;
	for (int i=0; i<iChannelGroupNum; ++i)
	{
		FMOD::ChannelGroup* pSubChannelGroup = 0;
		if(FMOD_OK != pChannelGroup->getGroup(i, &pSubChannelGroup))
			return false;
		if(!Apply(pSubChannelGroup, fParamValue))
			return false;
	}
	return true;
}

bool EffectSurroundPan::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup)
		return false;
	int iChannelNum = 0;
	if(FMOD_OK != pChannelGroup->getNumChannels(&iChannelNum))
		return false;
	for (int i=0; i<iChannelNum; ++i)
	{
		FMOD::Channel* pChannel = 0;
		if(FMOD_OK != pChannelGroup->getChannel(i, &pChannel))
			return false;
		if(FMOD_OK != pChannel->setSpeakerMix(1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f))
			return false;
	}

	int iChannelGroupNum = 0;
	if(FMOD_OK != pChannelGroup->getNumGroups(&iChannelGroupNum))
		return false;
	for (int i=0; i<iChannelGroupNum; ++i)
	{
		FMOD::ChannelGroup* pSubChannelGroup = 0;
		if(FMOD_OK != pChannelGroup->getGroup(i, &pSubChannelGroup))
			return false;
		if(!Remove(pSubChannelGroup))
			return false;
	}
	return true;
}

bool EffectSurroundPan::SetParameter(int index, float value)
{
	if(index < 0 || index >2)
		return false;
	if(index == 0 || index == 1)
	{
		if(value < -1 || value > 1)
			return false;
	}
	else
	{
		if(value < 0 || value > 5)
			return false;
	}	
	m_arrProp[index]->SetDefault(value);
	return true;
}

bool EffectSurroundPan::GetParameter(int index, float& value)
{
	if(index < 0 || index >2)
		return false;
	value = m_arrProp[index]->GetDefault();
	return true;
}

int EffectSurroundPan::GetNumParameters() const
{
	return 3;
}


//////////////////////////////////////////////////////////////////////////
//EffectParamEQ
//////////////////////////////////////////////////////////////////////////
enum
{
	PARAM_CENTER_FREQ,
	PARAM_OCTAVE_RANGE,
	PARAM_FREQ_GAIN
};

using namespace AudioEngine;

EffectParamEQ::EffectParamEQ(void)
{
}

EffectParamEQ::~EffectParamEQ(void)
{
}

bool EffectParamEQ::Init(EventSystem* pEventSystem)
{
	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(FMOD_OK != pSystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &m_pDSP))
		return false;
	m_EffectType = EFFECT_TYPE_PARAMEQ;

	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(8000 , 20, 22000);
	pPropCurves->SetName("Center Freq");
	pPropCurves->SetUnit("hz");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(1 , 0.2f, 5.0f);
	pPropCurves->SetName("Octave Range");
	pPropCurves->SetUnit("octaves");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(1 , -26.02f, 9.54f);
	pPropCurves->SetName("Freq Gain");
	pPropCurves->SetUnit("dB");
	m_arrProp.push_back(pPropCurves);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//EffectCompressor
//////////////////////////////////////////////////////////////////////////
enum
{
	PARAM_THRESHOLD,
	PARAM_ATTACK,
	PARAM_RELEASE,
	PARAM_MAKE_UP_GAIN
};

using namespace AudioEngine;

EffectCompressor::EffectCompressor(void)
{
}

EffectCompressor::~EffectCompressor(void)
{
}

bool EffectCompressor::Init(EventSystem* pEventSystem)
{
	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(FMOD_OK != pSystem->createDSPByType(FMOD_DSP_TYPE_COMPRESSOR, &m_pDSP))
		return false;
	m_EffectType = EFFECT_TYPE_COMPRESSOR;

	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(0 , -60.0f, 0.0f);
	pPropCurves->SetName("Threshold");
	pPropCurves->SetUnit("dB");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(50 , 10.0f, 200.0f);
	pPropCurves->SetName("Attack");
	pPropCurves->SetUnit("ms");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(50 , 20.0f, 1000.0f);
	pPropCurves->SetName("Release");
	pPropCurves->SetUnit("ms");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0 , 0.0f, 30.0f);
	pPropCurves->SetName("Make Up Gain");
	pPropCurves->SetUnit("dB");
	m_arrProp.push_back(pPropCurves);	

	return true;
}


//////////////////////////////////////////////////////////////////////////
//EffectEcho
//////////////////////////////////////////////////////////////////////////
enum
{
	PARAM_DELAY,
	PARAM_DECAY,
	PARAM_CHANNELS,
	PARAM_DRY_MIX,
	PARAM_WET_MIX
};

using namespace AudioEngine;

EffectEcho::EffectEcho(void)
{
}

EffectEcho::~EffectEcho(void)
{
}

bool EffectEcho::Init(EventSystem* pEventSystem)
{
	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(FMOD_OK != pSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &m_pDSP))
		return false;
	m_EffectType = EFFECT_TYPE_ECHO;
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(500 , 1.0f, 5000.0f);
	pPropCurves->SetName("Delay");
	pPropCurves->SetUnit("ms");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0.5f , 0.0f, 1.0f);
	pPropCurves->SetName("Decay");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0 , 0.0f, 16.0f);
	pPropCurves->SetName("Channels");
	pPropCurves->SetUnit("channels");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(1 , 0.0f, 1.0f);
	pPropCurves->SetName("Dry Mix");
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(1 , 0.0f, 1.0f);
	pPropCurves->SetName("Wet Mix");
	m_arrProp.push_back(pPropCurves);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//EffectOcclusion
//////////////////////////////////////////////////////////////////////////
EffectOcclusion::EffectOcclusion(void)
{
}

EffectOcclusion::~EffectOcclusion(void)
{
	release();
}

bool EffectOcclusion::Init(EventSystem* pEventSystem)
{
	release();

	if(!pEventSystem)
		return false;
	PropCurves* pPropCurves = new PropCurves;
	pPropCurves->Init(0, 0, 1);
	pPropCurves->SetName("Direct");	
	m_arrProp.push_back(pPropCurves);

	pPropCurves = new PropCurves;
	pPropCurves->Init(0, 0, 1);
	pPropCurves->SetName("Reverb");	
	m_arrProp.push_back(pPropCurves);

	m_EffectType = EFFECT_TYPE_OCCLUSION;
	return true;
}

bool EffectOcclusion::Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue)
{
	if(!pChannelGroup)
		return false;
	float fDirect = m_arrProp[0]->GetValue(fParamValue);
	float fReverb = m_arrProp[1]->GetValue(fParamValue);
	if(FMOD_OK != pChannelGroup->set3DOcclusion(fDirect, fReverb))
		return false;
	return true;
}

bool EffectOcclusion::Remove(FMOD::ChannelGroup* pChannelGroup)
{
	if(!pChannelGroup)
		return false;
	if(FMOD_OK != pChannelGroup->set3DOcclusion(0, 0))
		return false;
	return true;
}

bool EffectOcclusion::SetParameter(int index, float value)
{
	if(index != 0 && index != 1)
		return false;
	if(value < 0 || value > 1)
		return false;
	m_arrProp[index]->SetDefault(value);
	return true;
}

bool EffectOcclusion::GetParameter(int index, float& value)
{
	if(index != 0 && index != 1)
		return false;
	value = m_arrProp[index]->GetDefault();
	return true;
}

void EffectOcclusion::release()
{
	Effect::release();
}