#pragma once
#include "FAudioEngine.h"
#include "FExportDecl.h"
#include <string>
#include <vector>

namespace AudioEngine
{
	class Reverb;
	class EventSystem;


	struct _EXPORT_DLL_ REVERB
	{
		std::string					strName;
		REVERB_PROPERTIES			prop;
	};

	typedef std::vector<REVERB*> ReverbArray;

	class _EXPORT_DLL_ ReverbDataManager
	{
	public:
		ReverbDataManager(void);
		~ReverbDataManager(void);
	public:
		bool						Init(EventSystem* pEventSystem);
		bool						LoadXML(const char* szPath);
		bool						SaveXML(const char* szPath);	
		int							GetReverbNum() const;
		REVERB*						GetReverbByIndex(int idx) const;
		REVERB*						GetReverbByName(const char* szName) const;
		REVERB*						CreateReverb(const char* szName);
		bool						DeleteReverb(REVERB* pReverb);
	protected:
		void						release();
		void						createPresets();
	protected:
		ReverbArray					m_arrReverb;
		EventSystem*				m_pEventSystem;
	};

}