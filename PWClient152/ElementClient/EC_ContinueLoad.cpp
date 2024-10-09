// File		: EC_ContinueLoad.cpp
// Creator	: Xu Wenbin
// Date		: 2014/10/11

#include "EC_ContinueLoad.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_CrossServer.h"
#include "EC_Reconnect.h"

extern CECGame * g_pGame;

ELEMENTCLIENT_DEFINE_SINGLETON_NO_CTOR(CECContinueLoad)

CECContinueLoad::CECContinueLoad()
: m_iSaveBackImage(0)
, m_iSaveTipIndex(-1)
, m_fSaveLoadPos(0.0f)
{
}

bool CECContinueLoad::IsContinueLastLoad()const
{
	//	�����ϴ���תλ�ü������ر�����ת������ true
	//	
	return IsInMergeLoad()
		&& g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME;
}

bool CECContinueLoad::IsInMergeLoad()const
{
#ifdef _DEBUG
	return false;	//	��¼ѡ�˽���ÿ���޸�ʱ����Ӧ��Կ���ȹ��ܽ��в���
#else
	if (CECCrossServer::Instance().IsWaitLogin()){
		return CECCrossServer::Instance().IsGameToSpecial()
			|| CECCrossServer::Instance().IsGameToOriginal()
			|| CECCrossServer::Instance().IsLoginToSpecial() && CECReconnect::Instance().IsReconnectFromGame();
	}else if (CECReconnect::Instance().IsReconnecting()){
		return CECReconnect::Instance().IsReconnectFromGame();
	}
	return false;
#endif
}
