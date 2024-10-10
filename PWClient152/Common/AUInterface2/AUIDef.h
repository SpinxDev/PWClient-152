// Filename	: AUIDef.h
// Creator	: Tom Zhou
// Date		: May 18, 2004

#ifndef _AUIDEF_H_
#define _AUIDEF_H_

#define AUI_VERSION 2

#ifdef AUINTERFACE_EXPORTS
#define DllClass	__declspec(dllexport)
#else
#define DllClass	__declspec(dllimport)
#endif

#ifndef AUI_PRESS
#define AUI_PRESS(vk)	(GetKeyState(vk) & 0x8000)
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x020A
#endif

#define AUI_RELEASE_GFX(__macro_pGFX) { if((__macro_pGFX)) { AfxGetGFXExMan()->CacheReleasedGfx((__macro_pGFX)); (__macro_pGFX) = NULL; } }

#endif //_AUIDEF_H_