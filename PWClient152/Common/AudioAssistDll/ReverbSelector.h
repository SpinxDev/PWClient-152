#pragma once
#include <string>
#include "FAudioEngine.h"

namespace AudioEngine
{
	class EventSystem;
	struct REVERB;
}

using AudioEngine::EventSystem;
using AudioEngine::REVERB;

class CDlgSelectReverb;

class __declspec(dllexport) ReverbSelector
{
public:
	ReverbSelector(EventSystem* pEventSystem);
	~ReverbSelector(void);
public:
	INT_PTR					DoModal();
	REVERB*					GetSelectedReverb() const;
protected:
	CDlgSelectReverb*		m_pDlgSelectReverb;
};
