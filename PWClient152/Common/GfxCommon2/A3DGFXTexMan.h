#ifndef _A3DGFXTEXMAN_H_
#define _A3DGFXTEXMAN_H_

#include <hashmap.h>
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DGFXCachedItemMan.h"

class A3DGFXSharedTexMan
{
public:
	A3DGFXSharedTexMan() : m_bInit(false), m_pDevice(0) { ::InitializeCriticalSection(&m_csTex); }
	~A3DGFXSharedTexMan() { ::DeleteCriticalSection(&m_csTex); }

protected:
	A3DDevice*		m_pDevice;
	bool			m_bInit;
	CachedItemMan<A3DTexture*> m_TexMan;
	CRITICAL_SECTION	m_csTex;

protected:
	void LockTex() { ::EnterCriticalSection(&m_csTex); }
	void UnlockTex() { ::LeaveCriticalSection(&m_csTex); }

public:
	bool Init(A3DDevice* pDevice);
	A3DTexture* QueryTexture(const AString& strTex, bool bNoDownSample, bool bAbsTexPath);
	void Release();
};

inline bool A3DGFXSharedTexMan::Init(A3DDevice* pDevice)
{
	if (m_bInit) return true;
	m_pDevice = pDevice;
	m_bInit = true;
	return true;
}

#endif
