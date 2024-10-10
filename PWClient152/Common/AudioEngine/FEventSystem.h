#pragma once
#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FCriticalSectionWrapper.h"
#include <string>
namespace FMOD
{
	class System;
	class Event;
	class DSP;
}

namespace AudioEngine
{
	class EffectFactory;
	class LogManager;
	class ReverbDataManager;

	class _EXPORT_DLL_ EventSystem
	{
	public:
		EventSystem(void);
		~EventSystem(void);
	public:
		bool				Init(int maxchannels, INITFLAGS flags, void *extradriverdata);
		EventProject*		CreateProject(const char* szName);		
		bool				Load(const char* szPath, EventProject*& pEventProject);
		bool				LoadBinary(const char* szPath, EventProject*& pEventProject);
		bool				LoadProjects();
		bool				LoadBinaryProjects();
		FMOD::System*		GetSystem() const;
		bool				Update();
		EffectFactory*		GetEffectFactory() const;
		Reverb*				CreateReverb(const char* szName);
		bool				DestroyReverb(Reverb* pReverb);
		int					GetProjectNum() const;
		EventProject*		GetProjectByIndex(int idx) const;
		EventProject*		GetProjectByName(const char* szName) const;
		bool				Set3DListenerAttributes(const VECTOR& pos, const VECTOR& vel, const VECTOR& forward, const VECTOR& up);
		Event*				GetEvent(const char* szGuid);
		void				SetRootFolder(const char* szFolder);
		LogManager*			GetLogManager() const;
		bool				InitReverbDataManager(const char* szPath);
		ReverbDataManager*	GetReverbDataManager() const;
		bool				SetClassVolume(int iClass, float fVolume);
		bool				SetClassMute(int iClass, bool bMute);
		unsigned int		GetUsedMemoryBits() const;
		bool				GetCPUUsage(float* pfDsp, float* pfStream, float* pfGeometry, float* pfUpdate, float* pfTotal) const;
		int					GetPlayingChannelsNum() const;
		CriticalSectionWrapper*	GetCriticalSectionWrapper() { return &m_csForThreadSafe; }
	protected:
		void				release();
		void				releaseProjects();
	protected:
		FMOD::System*		m_pSystem;
		EventProjectList	m_listEventProject;
		EffectFactory*		m_pEffectFactory;
		ReverbList			m_listReverb;
		std::string			m_strFolder;
		LogManager*			m_pLogManager;
		ReverbDataManager*	m_pReverbDataManager;
		bool				m_bFailedInit;
		unsigned long		m_dwTickCount;
		FMOD::DSP*			m_pCompressorDsp;
		CriticalSectionWrapper m_csForThreadSafe;
	};
}

bool StopChannelGroup(FMOD::ChannelGroup* pChannelGroup);