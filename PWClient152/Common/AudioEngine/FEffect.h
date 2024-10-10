#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include <vector>

namespace FMOD
{
	class ChannelGroup;	
}

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class PropCurves;
	class EventSystem;

	typedef std::vector<PropCurves*> PropArray;

	class _EXPORT_DLL_ Effect
	{
	public:
		Effect(void);
		virtual ~Effect(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem) = 0;
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue) = 0;
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup) = 0;
		virtual bool			SetParameter(int index, float value) = 0;
		virtual bool			GetParameter(int index, float& value) = 0;
		virtual int				GetNumParameters() const = 0;
		EFFECT_TYPE				GetType() const { return m_EffectType; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		virtual const char*		GetName() = 0;
		PropCurves*				GetPropCurve(int idx) const;
		bool					IsEnable() const { return m_bEnable; }
		void					Enable(bool bEnable) { m_bEnable = bEnable; }
	protected:
		virtual void			release();
	protected:
		EFFECT_TYPE				m_EffectType;
		PropArray				m_arrProp;
		bool					m_bEnable;
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectDsp
//////////////////////////////////////////////////////////////////////////
namespace FMOD
{
	class DSP;
}

namespace AudioEngine
{
	class _EXPORT_DLL_ EffectDsp : public Effect
	{
	public:
		EffectDsp(void);
		virtual ~EffectDsp(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem) = 0;
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const;
	protected:
		virtual void			release();
	protected:
		FMOD::DSP*				m_pDSP;
		FMOD::ChannelGroup*		m_pChannelGroup;
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectVolume
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectVolume : public Effect
	{
	public:
		EffectVolume(void);
		~EffectVolume(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const { return 1; };
		virtual const char*		GetName() { return "Volume"; }
	protected:
		virtual void			release();
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectPitch
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectPitch : public Effect
	{
	public:
		EffectPitch(void);
		~EffectPitch(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const { return 1; };
		virtual const char*		GetName() { return "Pitch"; }
	protected:
		virtual void			release();
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectPan
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectPan : public Effect
	{
	public:
		EffectPan(void);
		~EffectPan(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const;
		virtual const char*		GetName() { return "Pan"; }
	protected:
		virtual void			release();
	};
}



//////////////////////////////////////////////////////////////////////////
//EffectSurroundPan
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectSurroundPan : public Effect
	{
	public:
		EffectSurroundPan(void);
		~EffectSurroundPan(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const;
		virtual const char*		GetName() { return "Surround Pan"; }
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectParamEQ
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectParamEQ : public EffectDsp
	{
	public:
		EffectParamEQ(void);
		~EffectParamEQ(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual const char*		GetName() { return "ParamEQ"; }
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectCompressor
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectCompressor : public EffectDsp
	{
	public:
		EffectCompressor(void);
		~EffectCompressor(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual const char*		GetName() { return "Compressor"; }
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectEcho
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectEcho : public EffectDsp
	{
	public:
		EffectEcho(void);
		~EffectEcho(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual const char*		GetName() { return "Echo"; }
	};
}


//////////////////////////////////////////////////////////////////////////
//EffectOcclusion
//////////////////////////////////////////////////////////////////////////
namespace AudioEngine
{
	class _EXPORT_DLL_ EffectOcclusion : public Effect
	{
	public:
		EffectOcclusion(void);
		~EffectOcclusion(void);
	public:
		virtual bool			Init(EventSystem* pEventSystem);
		virtual bool			Apply(FMOD::ChannelGroup* pChannelGroup, float fParamValue);
		virtual bool			Remove(FMOD::ChannelGroup* pChannelGroup);
		virtual bool			SetParameter(int index, float value);
		virtual bool			GetParameter(int index, float& value);
		virtual int				GetNumParameters() const { return 2; };
		virtual const char*		GetName() { return "Occlusion"; }
	protected:
		virtual void			release();
	};
}