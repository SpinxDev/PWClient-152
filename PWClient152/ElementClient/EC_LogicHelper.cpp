// Filename	: EC_LogicHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/8

#include "EC_LogicHelper.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"

namespace CECLogicHelper
{
	CECGame * GetGame()
	{
		return g_pGame;
	}
	CECGameRun * GetGameRun()
	{
		return GetGame()->GetGameRun();
	}
	CECGameSession * GetGameSession()
	{
		return GetGame()->GetGameSession();
	}
	CECHostPlayer* GetHostPlayer()
	{
		return GetGameRun()->GetHostPlayer();
	}
	CECWorld * GetWorld()
	{
		return GetGameRun()->GetWorld();
	}
}
