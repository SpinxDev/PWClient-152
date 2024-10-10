#ifndef _FX_BINDING_H_
#define _FX_BINDING_H_

#include "A3DCombinedAction.h"
#include "CriticalSectionWrapper.h"

namespace AudioEngine
{
	class EventInstance;
}

class MODEL_SCALE_BINDING : public FX_BINDING_BASE
{
public:
	MODEL_SCALE_BINDING(A3DCombActDynData* pDynData, const BoneScaleChange* pModelScaleEvent, float fModelScale);
	virtual ~MODEL_SCALE_BINDING();

protected:
	
	const BoneScaleChange* m_pModelScaleEvent;
	AString m_strCurAction;
	DWORD m_dwTickTime;
	float m_fScale;
	bool m_bUseFileScale;

protected:

	int CalcCurTimeOfCurPeriod(const BoneScaleChange::BoneScaleParam* pBoneSclParam, float& fCurTimeOfCurPeriod);
	void RetriveBoneScale() const;
	virtual void Stop() const;
	virtual bool IsStop() const;
	virtual void Render(A3DViewport* pView) {};
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);
};

class MATTRANS_BINDING : public FX_BINDING_BASE
{
public:
	MATTRANS_BINDING(A3DCombActDynData* pDynData, const MaterialScaleTrans* pTransEvent);
	virtual ~MATTRANS_BINDING();

protected:

	const MaterialScaleTrans* m_pTransEvent;
	A3DCOLORVALUE m_CurColor;
	DWORD m_dwTimeSpan;
	DWORD m_dwTimeEnd;

public:

	const A3DCOLORVALUE& GetScaleCur() const { return m_CurColor; }
	virtual bool IsStop() const;
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class AUDIOEVENT_BINDING
//	
///////////////////////////////////////////////////////////////////////////

#if defined(_USEAUDIOENGINE)
class AUDIOEVENT_BINDING : public FX_BINDING_BASE, public AudioEngine::EventInstance::EventInstanceListener
#else
class AUDIOEVENT_BINDING : public FX_BINDING_BASE
#endif
{
public:
	AUDIOEVENT_BINDING(A3DCombActDynData* pDynData, AudioEngine::EventInstance* pEventInstance);
	~AUDIOEVENT_BINDING();

	virtual void Stop(bool bForce) const;
	virtual bool IsStop() const;
	virtual void UpdateParam(CECModel* pECModel, int nDeltaTime);

	virtual bool OnStop(AudioEngine::EventInstance* pEventInstance);
	AudioEngine::EventInstance* GetEventInstance() const { return m_pEventInstance; }

protected:
	AudioEngine::EventInstance*		m_pEventInstance;
	A3DVECTOR3 m_vLastTickPos;

	bool	m_bPosInited;
	CSWrapper m_csForThreadSafe;
};

typedef AUDIOEVENT_BINDING* PAUDIOEVENT_BINDING;

#endif