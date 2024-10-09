// Filename	: EC_LogicHelper.h
// Creator	: Xu Wenbin
// Date		: 2015/1/8

#pragma once

#include <ABaseDef.h>
#include <AAssist.h>
#include <A3DTypes.h>
#include "vector.h"

class CECGame;
class CECGameRun;
class CECGameSession;
class CECHostPlayer;
class CECWorld;

//	�ṩ����Ϸ�߼�/�߼�����ķ��ʺ͸�������
namespace CECLogicHelper
{
	CECGame *		GetGame();
	CECGameRun *	GetGameRun();
	CECGameSession *GetGameSession();
	CECHostPlayer*	GetHostPlayer();
	CECWorld *		GetWorld();
};
