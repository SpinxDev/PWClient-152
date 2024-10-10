#pragma once

#ifdef _AUDIO_ENGINE_DLL_
	#define _EXPORT_DLL_ __declspec(dllexport)
#else
	#define _EXPORT_DLL_
#endif

#pragma warning (disable: 4251)