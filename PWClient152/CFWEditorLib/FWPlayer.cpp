// FWPlayer.cpp: implementation of the FWPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "FWPlayer.h"
#include "FWAssemblySet.h"
#include "A3DGFXExMan.h"


#define new A_DEBUG_NEW





FWPlayer::FWPlayer()
{
	m_nState = PLAYER_STOP;
	m_bActive = false;
	m_pViewport = NULL;
}

FWPlayer::~FWPlayer()
{
	SAFE_DELETE(m_pAssemblySet);
}

void FWPlayer::Start()
{
	if (GetPlayerState() == PLAYER_PAUSE)
		m_pAssemblySet->Start(false);
	else
		m_pAssemblySet->Start(true);

	m_nState = PLAYER_PLAY;
}

void FWPlayer::Pause()
{
	m_nState = PLAYER_PAUSE;
	m_pAssemblySet->Pause();
}

void FWPlayer::Stop()
{
	m_nState = PLAYER_STOP;
	m_pAssemblySet->Stop();
}

bool FWPlayer::Init(A3DViewport *pViewport)
{
	m_pViewport = pViewport;
	m_pAssemblySet = new FWAssemblySet;

	return true;
}

void FWPlayer::Tick(DWORD dwTickTime)
{
	if (!IsActive()) return;

	m_pAssemblySet->TickAnimation(dwTickTime);
}

void FWPlayer::Render(A3DGFXExMan * pGfxExMan)
{
	if (!IsActive()) return;

	m_pAssemblySet->RegisterGfx(pGfxExMan);
	pGfxExMan->RenderAllGfx(m_pViewport);
}

bool FWPlayer::IsActive()
{
	return m_bActive;
}

void FWPlayer::SetActive(bool bActive)
{
	m_bActive = bActive;
}

int FWPlayer::GetPlayerState()
{
	return m_nState;
}

FWAssemblySet * FWPlayer::GetAssemblySet()
{
	return m_pAssemblySet;
}
