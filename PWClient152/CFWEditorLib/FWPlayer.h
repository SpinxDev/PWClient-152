#ifndef _FWPLAYER_H_
#define _FWPLAYER_H_

#include "CodeTemplate.h"
#include "FWConfig.h"

class FWAssemblySet;
class A3DGFXExMan;
class FWGlyphList;
class A3DViewport;

class FWPlayer  
{
protected:
	int m_nState;
	bool m_bActive;
	FWAssemblySet * m_pAssemblySet;
	A3DViewport * m_pViewport;
public:
	FWPlayer();
	virtual ~FWPlayer();

	FWAssemblySet * GetAssemblySet();
	enum {PLAYER_PLAY, PLAYER_PAUSE, PLAYER_STOP};
	void Start();
	void Pause();
	void Stop();
	bool Init(A3DViewport * pViewport);
	void Tick(DWORD dwTickTime);
	void Render(A3DGFXExMan * pGfxExMan);
	bool IsActive();
	void SetActive(bool bActive);
	int GetPlayerState();
};

#endif 