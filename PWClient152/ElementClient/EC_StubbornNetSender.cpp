// File		: EC_StubbornNetSender.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/28

#include "EC_StubbornNetSender.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"

extern CECGame * g_pGame;

CECStubbornFactionInfoSender::CECStubbornFactionInfoSender(){
	m_timer.Reset(4000);
}

CECStubbornFactionInfoSender::~CECStubbornFactionInfoSender(){
}

bool CECStubbornFactionInfoSender::IsFinished()const{
	return m_factionIDs.empty();
}

void CECStubbornFactionInfoSender::Tick(){
	if (m_factionIDs.empty()){
		return;
	}
	CECGameSession *pGameSession = g_pGame->GetGameSession();
	if (!pGameSession->IsConnected() ||
		g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME ||
		!g_pGame->GetGameRun()->GetHostPlayer()->HostIsReady()){
		return;
	}
	if (!m_timer.IsTimeArrived()){
		return;
	}
	m_timer.Reset();
	pGameSession->GetFactionInfoImmediately(m_factionIDs.size(), &m_factionIDs.front());
}

void CECStubbornFactionInfoSender::Reset(){
	m_factionIDs.clear();
	m_timer.Reset();
}

void CECStubbornFactionInfoSender::Add(int iNumFaction, const int* aFactionIDs){
	if (iNumFaction <= 0 || !aFactionIDs){
		ASSERT(false);
		return;
	}
	for (int i(0); i < iNumFaction; ++ i){
		int idFaction = aFactionIDs[i];
		if (idFaction > 0 &&
			std::find(m_factionIDs.begin(), m_factionIDs.end(), idFaction) == m_factionIDs.end()){
			m_factionIDs.push_back(idFaction);
		}
	}
}

void CECStubbornFactionInfoSender::OnModelChange(const CECFactionMan *p, const CECObservableChange *q){
	if (m_factionIDs.empty()){
		return;
	}
	const CECFactionManChange *pChange = dynamic_cast<const CECFactionManChange *>(q);
	if (!pChange){
		ASSERT(false);
	}else if (pChange->ChangeMask() != CECFactionManChange::ADD_FACTION){
		return;
	}
	const CECFactionManFactionChange *pFactionChange = dynamic_cast<const CECFactionManFactionChange *>(pChange);
	int idFaction = pFactionChange->FactionID();
	m_factionIDs.erase(std::remove(m_factionIDs.begin(), m_factionIDs.end(), idFaction), m_factionIDs.end());
}