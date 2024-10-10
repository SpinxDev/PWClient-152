#pragma once

#include "FAudioEngine.h"
#include "PropListCtrl/PropertyInterface.h"

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

using AudioEngine::Effect;

enum // Volume
{
	idVolume
};

enum // Pitch
{
	idPitch
};

enum // Occlusion
{
	idDirect,
	idReverb
};

enum // Pan
{
	idPan
};

enum // Surround Pan
{
	idLeftRightPan,
	idBackFrontPan,
	idLFE
};

enum // ParamEQ
{
	idCenterFreq,
	idOctaveRange,
	idFreqGain
};

enum // Compressor
{
	idThreshold,
	idAttack,
	idRelease,
	idMakeUpGain
};

enum // Echo
{
	idDelay,
	idDecay,
	idChannels,
	idDryMix,
	idWetMix
};

struct EFFECT_VOLUME_DATA 
{
	float fVolume;
};

struct EFFECT_PITCH_DATA 
{
	float fPitch;
};

struct EFFECT_OCCLUSION_DATA 
{
	float fDirect;
	float fReverb;
};

struct EFFECT_PAN_DATA 
{
	float fPan;
};

struct EFFECT_SURROUND_PAN_DATA 
{
	float fLeftRightPan;
	float fBackFrontPan;
	float fLFE;
};

struct EFFECT_PARAMEQ_DATA
{
	float fCenterFreq;
	float fOctaveRange;
	float fFreqGain;
};

struct EFFECT_COMPRESSOR_DATA
{
	float fThreshold;
	float fAttack;
	float fRelease;
	float fMakeUpGain;
};

struct EFFECT_ECHO_DATA
{
	float fDelay;
	float fDecay;
	float fChannels;
	float fDryMix;
	float fWetMix;
};

class CDlgEffectProp : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgEffectProp)

public:
	CDlgEffectProp(Effect* pEffect, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEffectProp();

// 对话框数据
	enum { IDD = IDD_EFFECT_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	void fillData();
	void buildProperty();
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	Effect* m_pEffect;
	EFFECT_VOLUME_DATA m_VolumeData;
	EFFECT_PITCH_DATA m_PitchData;
	EFFECT_OCCLUSION_DATA m_OcclusionData;
	EFFECT_PAN_DATA m_PanData;
	EFFECT_SURROUND_PAN_DATA m_SurroundPanData;
	EFFECT_PARAMEQ_DATA m_ParamEQData;
	EFFECT_COMPRESSOR_DATA m_CompressorData;
	EFFECT_ECHO_DATA m_EchoData;
	CString m_strEffectType;
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
public:
	EFFECT_VOLUME_DATA& GetVolumeData() { return m_VolumeData; }
	EFFECT_PITCH_DATA& GetPitchData() { return m_PitchData; }
	EFFECT_OCCLUSION_DATA& GetOcclusionData() { return m_OcclusionData; }
	EFFECT_PAN_DATA& GetPanData() { return m_PanData; }
	EFFECT_SURROUND_PAN_DATA& GetSurroundPanData() { return m_SurroundPanData; }
	EFFECT_PARAMEQ_DATA& GetParamEQData() { return m_ParamEQData; }
	EFFECT_COMPRESSOR_DATA& GetCompressorData() { return m_CompressorData; }
	EFFECT_ECHO_DATA& GetEchoData() { return m_EchoData; }
};
