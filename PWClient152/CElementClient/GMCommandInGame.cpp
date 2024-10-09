#include "GMCommandInGame.h"
#include "GMCommandFactory.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Configs.h"
#include "EC_HostPlayer.h"
#include "privilege.hxx"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_CrossServer.h"
#include "forbid.hxx"
#include "elementdataman.h"

#define GETSTRINGFROMTABLE(i) g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(i)

//////////////////////////////////////////////////////////////////////////
// CGMCommandInGameBase
//////////////////////////////////////////////////////////////////////////

GNET::Privilege * CGMCommandInGameBase::GetPrivilege()
{
	return g_pGame->GetPrivilege();
}

CECGameSession * CGMCommandInGameBase::GetGameSession()
{
	return g_pGame->GetGameSession();
}

CECConfigs * CGMCommandInGameBase::GetConfigs()
{
	return g_pGame->GetConfigs();
}

CECHostPlayer * CGMCommandInGameBase::GetHostPlayer()
{
	return g_pGame->GetGameRun()->GetHostPlayer();
}

CECGameUIMan * CGMCommandInGameBase::GetGameUIMan()
{
	return g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandKickOutRole
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("KickOutRole", CGMCommandKickOutRole)


CGMCommandKickOutRole::CGMCommandKickOutRole()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6302)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6303)));
}

bool CGMCommandKickOutRole::HasPrivilege()
{
	return GetPrivilege()->Has_Force_Offline();
}

bool CGMCommandKickOutRole::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	int nDuration = GetParamTrueValue<int>(m_paramArray[1]);
	if (nDuration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6305);
		return false;
	}
	ACString strReason = GetParamTrueValue<ACString>(m_paramArray[2]);
	if (strReason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}

	strDetail.Format(
		_AL("playerID = [%d], duration = [%d], reason = [%s]"),
		nPlayerID,
		nDuration, 
		strReason);

	GetGameSession()->gm_KickOutRole(nPlayerID, nDuration, strReason);
	return true;
}

ACString CGMCommandKickOutRole::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6307);
}

void CGMCommandKickOutRole::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandKickOutUser
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("KickOutUser", CGMCommandKickOutUser)


CGMCommandKickOutUser::CGMCommandKickOutUser()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6302)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6303)));
}

bool CGMCommandKickOutUser::HasPrivilege()
{
	return GetPrivilege()->Has_Force_Offline();
}

bool CGMCommandKickOutUser::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	int nDuration = GetParamTrueValue<int>(m_paramArray[1]);
	if (nDuration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6305);
		return false;
	}
	ACString strReason = GetParamTrueValue<ACString>(m_paramArray[2]);
	if (strReason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}

	strDetail.Format(
		_AL("playerID = [%d], duration = [%d], reason = [%s]"),
		nPlayerID,
		nDuration, 
		strReason);

	GetGameSession()->gm_KickOutUser(nPlayerID, nDuration, strReason);
	return true;
}

ACString CGMCommandKickOutUser::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6308);
}

void CGMCommandKickOutUser::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandShowOnlineNum
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ShowOnlineNum", CGMCommandShowOnlineNum)

CGMCommandShowOnlineNum::CGMCommandShowOnlineNum()
{

}

bool CGMCommandShowOnlineNum::HasPrivilege()
{
	return GetPrivilege()->Has_ListUser();
}

bool CGMCommandShowOnlineNum::Execute(ACString & strErr, ACString &strDetail)
{
	GetGameSession()->gm_OnlineNumber();
	return true;
}

ACString CGMCommandShowOnlineNum::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6309);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandRestartServer
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("RestartServer", CGMCommandRestartServer)

CGMCommandRestartServer::CGMCommandRestartServer()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6310)));
}

bool CGMCommandRestartServer::HasPrivilege()
{
	return GetPrivilege()->Has_Shutdown_GameServer();
}

bool CGMCommandRestartServer::Execute(ACString & strErr, ACString &strDetail)
{
	int nDelay = GetParamTrueValue<int>(m_paramArray[0]);
	if (nDelay < 0)
	{
		strErr = GETSTRINGFROMTABLE(6311);
		return false;
	}

	strDetail.Format(
		_AL("delay = [%d]"), 
		nDelay);

	GetGameSession()->gm_RestartServer(-1, nDelay);
	return true;
}

ACString CGMCommandRestartServer::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6312);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandShutupRole
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ShutupRole", CGMCommandShutupRole)

CGMCommandShutupRole::CGMCommandShutupRole()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6313)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6303)));
}

bool CGMCommandShutupRole::HasPrivilege()
{
	return GetPrivilege()->Has_Forbid_Talk();
}

bool CGMCommandShutupRole::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	int nDuration = GetParamTrueValue<int>(m_paramArray[1]);
	if (nDuration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6314);
		return false;
	}
	ACString strReason = GetParamTrueValue<ACString>(m_paramArray[2]);
	if (strReason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}

	strDetail.Format(
		_AL("playerID = [%d], duration = [%d], reason = [%s]"),
		nPlayerID,
		nDuration, 
		strReason);

	GetGameSession()->gm_ShutupRole(nPlayerID, nDuration, strReason);
	return true;
}

ACString CGMCommandShutupRole::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6315);
}

void CGMCommandShutupRole::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandShutupUser
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ShutupUser", CGMCommandShutupUser)

CGMCommandShutupUser::CGMCommandShutupUser()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6313)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6303)));
}

bool CGMCommandShutupUser::HasPrivilege()
{
	return GetPrivilege()->Has_Forbid_Talk();
}

bool CGMCommandShutupUser::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	int nDuration = GetParamTrueValue<int>(m_paramArray[1]);
	if (nDuration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6314);
		return false;
	}
	ACString strReason = GetParamTrueValue<ACString>(m_paramArray[2]);
	if (strReason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}
	
	strDetail.Format(
		_AL("playerID = [%d], duration = [%d], reason = [%s]"),
		nPlayerID,
		nDuration, 
		strReason);

	GetGameSession()->gm_ShutupUser(nPlayerID, nDuration, strReason);
	return true;
}

ACString CGMCommandShutupUser::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6316);
}

void CGMCommandShutupUser::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandMoveToPlayer
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("MoveToPlayer", CGMCommandMoveToPlayer)

CGMCommandMoveToPlayer::CGMCommandMoveToPlayer()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
}

bool CGMCommandMoveToPlayer::HasPrivilege()
{
	return GetPrivilege()->Has_MoveTo_Role();	
}

bool CGMCommandMoveToPlayer::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}

	strDetail.Format(
		_AL("playerID = [%d]"),
		nPlayerID);

	GetGameSession()->gm_MoveToPlayer(nPlayerID);
	return true;
}

ACString CGMCommandMoveToPlayer::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6317);
}

void CGMCommandMoveToPlayer::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandCallInPlayer
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("CallInPlayer", CGMCommandCallInPlayer)

CGMCommandCallInPlayer::CGMCommandCallInPlayer()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));

}

bool CGMCommandCallInPlayer::HasPrivilege()
{
	return GetPrivilege()->Has_Fetch_Role();	
}

bool CGMCommandCallInPlayer::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}

	strDetail.Format(
		_AL("playerID = [%d]"),
		nPlayerID);

	GetGameSession()->gm_CallInPlayer(nPlayerID);
	return true;
}

ACString CGMCommandCallInPlayer::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6318);
}

void CGMCommandCallInPlayer::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandBroadcast
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("Broadcast", CGMCommandBroadcast)

CGMCommandBroadcast::CGMCommandBroadcast()
{
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6319)));
}

bool CGMCommandBroadcast::HasPrivilege()
{
	return GetPrivilege()->Has_Broadcast();
}

bool CGMCommandBroadcast::Execute(ACString & strErr, ACString &strDetail)
{
	ACString strContent = GetParamTrueValue<ACString>(m_paramArray[0]);
	if (strContent.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6320);
		return false;
	}

	strDetail.Format(
		_AL("content = [%s]"),
		strContent);


	GetGameSession()->SendChatData(GP_CHAT_BROADCAST, strContent);
	return true;
}

ACString CGMCommandBroadcast::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6321);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandToggleID
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ToggleID", CGMCommandToggleID)

CGMCommandToggleID::CGMCommandToggleID()
{
	
}

bool CGMCommandToggleID::HasPrivilege()
{
	return GetPrivilege()->Has_Toggle_NameID();
}

bool CGMCommandToggleID::Execute(ACString & strErr, ACString &strDetail)
{
	GetConfigs()->SetShowIDFlag(!GetConfigs()->GetShowIDFlag());
	return true;
}

ACString CGMCommandToggleID::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6322);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandForbidRole
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ForbidRole", CGMCommandForbidRole)

CGMCommandForbidRole::CGMCommandForbidRole()
{
	CParamArray typeValues;
	typeValues.push_back(new CParamImp<int>(GETSTRINGFROMTABLE(6323), 100));
	typeValues.push_back(new CParamImp<int>(GETSTRINGFROMTABLE(6324), 101));
	typeValues.push_back(new CParamImp<int>(GETSTRINGFROMTABLE(6325), 102));
	typeValues.push_back(new CParamImp<int>(GETSTRINGFROMTABLE(6326), 103));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6327), 100, CParam::TYPE_LIST, &typeValues));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6328)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6329)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6330)));
}

bool CGMCommandForbidRole::HasPrivilege()
{
	return GetHostPlayer()->IsGM();
}

bool CGMCommandForbidRole::Execute(ACString & strErr, ACString &strDetail)
{
	// todo : what's the rule for type param?
	int nType = GetParamTrueValue<int>(m_paramArray[0]);
	if (GNET::Forbid::FBD_FORBID_LOGIN != nType &&
		GNET::Forbid::FBD_FORBID_TALK != nType &&
		GNET::Forbid::FBD_FORBID_TRADE != nType &&
		GNET::Forbid::FBD_FORBID_SELL != nType)
	{
		strErr = GETSTRINGFROMTABLE(6331);
		return false;
	}

	int nPlayerID = GetParamTrueValue<int>(m_paramArray[1]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}

	int nDuration = GetParamTrueValue<int>(m_paramArray[2]);
	if (nDuration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6332);
		return false;
	}

	ACString strReason = GetParamTrueValue<ACString>(m_paramArray[3]);
	if (strReason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}

	strDetail.Format(
		_AL("type = [%d], playerID = [%d], duration = [%d], reason = [%s]"),
		nType,
		nPlayerID,
		nDuration, 
		strReason);

	GetGameSession()->gm_ForbidRole(nType, nPlayerID, nDuration, strReason);
	return true;
}

ACString CGMCommandForbidRole::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6333);
}

void CGMCommandForbidRole::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[1], nPlayerID);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandTriggerChat
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("TriggerChat", CGMCommandTriggerChat)

CGMCommandTriggerChat::CGMCommandTriggerChat()
{
	AddParam(new CParamImp<bool>(GETSTRINGFROMTABLE(6334)));
}

bool CGMCommandTriggerChat::HasPrivilege()
{
	return GetPrivilege()->Has_Chat_OrNot();
}

bool CGMCommandTriggerChat::Execute(ACString & strErr, ACString &strDetail)
{
	GetGameSession()->gm_TriggerChat(GetParamTrueValue<bool>(m_paramArray[0]));
	return true;
}

ACString CGMCommandTriggerChat::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6335);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandGenerate
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("Generate", CGMCommandGenerate)

CGMCommandGenerate::CGMCommandGenerate()
{
	elementdataman *pEDMan = g_pGame->GetElementDataMan();
	ASSERT(pEDMan);
	CParamArray objIDValues;
	DATA_TYPE dataType = DT_INVALID;
	unsigned int uDataID = pEDMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (uDataID != 0)
	{
		if (DT_GM_GENERATOR_ESSENCE == dataType)
		{
			const GM_GENERATOR_ESSENCE *pEssence = 
				static_cast<const GM_GENERATOR_ESSENCE *>(
					pEDMan->get_data_ptr(uDataID, ID_SPACE_ESSENCE, dataType));
			ASSERT(pEssence);

			objIDValues.push_back(
				new CParamImp<unsigned int>(WC2AC(pEssence->name), uDataID));
		}
		uDataID = pEDMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}
	ASSERT(objIDValues.size() > 0);
	AddParam(new CParamImp<unsigned int>(GETSTRINGFROMTABLE(6336), 0, CParam::TYPE_LIST, &objIDValues));
}

bool CGMCommandGenerate::HasPrivilege()
{
	return GetHostPlayer()->IsGM();
}

bool CGMCommandGenerate::Execute(ACString & strErr, ACString &strDetail)
{
	int nObjID = static_cast<int>(
		GetParamTrueValue<unsigned int>(m_paramArray[0]));
	if (nObjID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6337);
		return false;
	}

	strDetail.Format(
		_AL("objID = [%d]"),
		nObjID);


	GetGameSession()->gm_Generate(nObjID);
	return true;
}

ACString CGMCommandGenerate::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6338);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandControlSpawnerBase
// CGMCommandActiveSpawner
// CGMCommandDeactiveSpawner
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ActiveSpawner", CGMCommandActiveSpawner)
REGISTER_TO_GMCOMMAND_FACTORY("DeactiveSpawner", CGMCommandDeactiveSpawner)

CGMCommandControlSpawnerBase::CGMCommandControlSpawnerBase()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6339)));
}

bool CGMCommandControlSpawnerBase::HasPrivilege()
{
	return GetPrivilege()->Has_ActivityManager();
}

bool CGMCommandControlSpawnerBase::Execute(ACString &strErr, ACString &strDetail)
{
	int spawnerID = GetParamTrueValue<int>(m_paramArray[0]);

	strDetail.Format(
		_AL("spawnerID = [%d]"),
		spawnerID);

	GetGameSession()->gm_ActiveSpawner(ToActive(), spawnerID);

	return true;
}

bool CGMCommandActiveSpawner::ToActive()
{
	return true;
}
ACString CGMCommandActiveSpawner::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6340);
}

bool CGMCommandDeactiveSpawner::ToActive()
{
	return false;
}
ACString CGMCommandDeactiveSpawner::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6341);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandGenerateMob
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("GenerateMob", CGMCommandGenerateMob)

CGMCommandGenerateMob::CGMCommandGenerateMob()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6342)));
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6343)));
	AddParam(new CParamImp<short>(GETSTRINGFROMTABLE(6344)));
	AddParam(new CParamImp<short>(GETSTRINGFROMTABLE(6345)));
	AddParam(new CParamImp<ACString>(GETSTRINGFROMTABLE(6346)));
}

bool CGMCommandGenerateMob::HasPrivilege()
{
	return GetHostPlayer()->IsGM();
}

bool CGMCommandGenerateMob::Execute(ACString &strErr, ACString &strDetail)
{
	int mobID = GetParamTrueValue<int>(m_paramArray[0]);
	int visID = GetParamTrueValue<int>(m_paramArray[1]);
	short mobNumber = GetParamTrueValue<short>(m_paramArray[2]);
	short lifeTime = GetParamTrueValue<short>(m_paramArray[3]);
	ACString mobName = GetParamTrueValue<ACString>(m_paramArray[4]);

	strDetail.Format(
		_AL("mobID = [%d], visID = [%d], mobNumber = [%u], lifeTime = [%u], mobName = [%s]"),
		mobID,
		visID,
		mobNumber,
		lifeTime,
		mobName);

	GetGameSession()->gm_GenerateMob(mobID, visID, mobNumber, lifeTime, mobName);

	return true;
}

ACString CGMCommandGenerateMob::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6347);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandInvisible
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("Invisible", CGMCommandInvisible)

CGMCommandInvisible::CGMCommandInvisible()
{
}

bool CGMCommandInvisible::HasPrivilege()
{
	return GetHostPlayer()->IsGM();
}

bool CGMCommandInvisible::Execute(ACString &strErr, ACString &strDetail)
{
	GetGameSession()->gm_TriggerInvisible();

	return true;
}

ACString CGMCommandInvisible::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6348);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandInvincible
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("Invincible", CGMCommandInvincible)

CGMCommandInvincible::CGMCommandInvincible()
{
}

bool CGMCommandInvincible::HasPrivilege()
{
	return GetHostPlayer()->IsGM();
}

bool CGMCommandInvincible::Execute(ACString &strErr, ACString &strDetail)
{
	GetGameSession()->gm_TriggerInvincible();

	return true;
}

ACString CGMCommandInvincible::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6349);
}

//////////////////////////////////////////////////////////////////////////
// CGMCommandQuerySpecItem
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("QuerySpecItem", CGMCommandQuerySpecItem)

CGMCommandQuerySpecItem::CGMCommandQuerySpecItem()
{
	m_nPlayerID = 0;
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6351)));
}

bool CGMCommandQuerySpecItem::HasPrivilege()
{
	return GetPrivilege()->Has_ActivityManager();
}

bool CGMCommandQuerySpecItem::Execute(ACString &strErr, ACString &strDetail)
{
	if (m_nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}

	int itemID = GetParamTrueValue<int>(m_paramArray[0]);
	if (itemID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6352);
		return false;
	}
	
	GetGameSession()->gm_QuerySpecItem(m_nPlayerID, itemID);

	return true;
}

ACString CGMCommandQuerySpecItem::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6350);
}

void CGMCommandQuerySpecItem::TrySetPlayerIDParam(int nPlayerID)
{
	m_nPlayerID = nPlayerID;
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandPermanentLock
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("PermanentLock", CGMCommandPermanentLock)


CGMCommandPermanentLock::CGMCommandPermanentLock()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
}

bool CGMCommandPermanentLock::HasPrivilege()
{
	return GetPrivilege()->Has_Forbid_Trade();
}

bool CGMCommandPermanentLock::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	
	strDetail.Format(_AL("Permanent lock playerID = [%d]"),	nPlayerID);
	
	GetGameSession()->gm_PermanentLock(nPlayerID, true);
	return true;
}

ACString CGMCommandPermanentLock::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6380);
}

void CGMCommandPermanentLock::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandUnLock
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("UnLockRole", CGMCommandUnLock)


CGMCommandUnLock::CGMCommandUnLock()
{
	AddParam(new CParamImp<int>(GETSTRINGFROMTABLE(6301)));
}

bool CGMCommandUnLock::HasPrivilege()
{
	return GetPrivilege()->Has_Forbid_Trade();	
}

bool CGMCommandUnLock::Execute(ACString & strErr, ACString &strDetail)
{
	int nPlayerID = GetParamTrueValue<int>(m_paramArray[0]);
	if (nPlayerID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}
	
	strDetail.Format(_AL("Unlock playerID = [%d]"),	nPlayerID);
	
	GetGameSession()->gm_PermanentLock(nPlayerID, false);
	return true;
}

ACString CGMCommandUnLock::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6381);
}

void CGMCommandUnLock::TrySetPlayerIDParam(int nPlayerID)
{
	SetParamTrueValue(m_paramArray[0], nPlayerID);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandOpenActivity
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("OpenActivity", CGMCommandOpenActivity)


CGMCommandOpenActivity::CGMCommandOpenActivity()
{
	m_iCurSel = -1;
	elementdataman *pDTMan = g_pGame->GetElementDataMan();
	DATA_TYPE dataType = DT_INVALID;
	int configID = pDTMan->get_first_data_id(ID_SPACE_CONFIG, dataType);
	
	while(configID)
	{
		if(dataType == DT_GM_ACTIVITY_CONFIG)
		{
			GM_ACTIVITY_CONFIG* pConfig = (GM_ACTIVITY_CONFIG*)pDTMan->get_data_ptr(configID, ID_SPACE_CONFIG, dataType);
			
			if(pConfig->disabled == 0)
				AddParam(new CParamImp<int>(pConfig->name,pConfig->id));
		}
		configID = pDTMan->get_next_data_id(ID_SPACE_CONFIG, dataType);
	}
}

bool CGMCommandOpenActivity::HasPrivilege()
{
	return GetPrivilege()->Has_ActivityManager();	
}

bool CGMCommandOpenActivity::Execute(ACString & strErr, ACString &strDetail)
{
	if (m_iCurSel<0 || m_iCurSel>=m_paramArray.size())
	{
		strErr = GETSTRINGFROMTABLE(6384);
		return false;
	}

	int nActID = GetParamTrueValue<int>(m_paramArray[m_iCurSel]);
	if (nActID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6384);
		return false;
	}
	
	strDetail.Format(_AL("Open Activity ID = [%d]"), nActID);
	
	GetGameSession()->gm_OpenActivity(nActID, true);
	return true;
}

ACString CGMCommandOpenActivity::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6382);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandCloseActivity
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("CloseActivity", CGMCommandCloseActivity)


CGMCommandCloseActivity::CGMCommandCloseActivity()
{
	m_iCurSel = -1;
	elementdataman *pDTMan = g_pGame->GetElementDataMan();
	DATA_TYPE dataType = DT_INVALID;
	int configID = pDTMan->get_first_data_id(ID_SPACE_CONFIG, dataType);
	
	while(configID)
	{
		if(dataType == DT_GM_ACTIVITY_CONFIG)
		{
			GM_ACTIVITY_CONFIG* pConfig = (GM_ACTIVITY_CONFIG*)pDTMan->get_data_ptr(configID, ID_SPACE_CONFIG, dataType);
			
			if(pConfig->disabled == 0)
				AddParam(new CParamImp<int>(pConfig->name,pConfig->id));
		}
		configID = pDTMan->get_next_data_id(ID_SPACE_CONFIG, dataType);
	}
}

bool CGMCommandCloseActivity::HasPrivilege()
{
	return GetPrivilege()->Has_ActivityManager();
}

bool CGMCommandCloseActivity::Execute(ACString & strErr, ACString &strDetail)
{
	if (m_iCurSel<0 || m_iCurSel>=m_paramArray.size())
	{
		strErr = GETSTRINGFROMTABLE(6384);
		return false;
	}

	int nActID = GetParamTrueValue<int>(m_paramArray[m_iCurSel]);
	if (nActID <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6384);
		return false;
	}
	
	strDetail.Format(_AL("Close Activity ID = [%d]"),	nActID);
	
	GetGameSession()->gm_OpenActivity(nActID, false);
	return true;
}

ACString CGMCommandCloseActivity::GetDisplayName()
{
	return GETSTRINGFROMTABLE(6383);
}


//////////////////////////////////////////////////////////////////////////
// CGMCommandChangeDS
//////////////////////////////////////////////////////////////////////////

REGISTER_TO_GMCOMMAND_FACTORY("ChangeDS", CGMCommandChangeDS)


CGMCommandChangeDS::CGMCommandChangeDS()
{
}

bool CGMCommandChangeDS::HasPrivilege()
{
	return GetPrivilege()->Has_ActivityManager();
}

bool CGMCommandChangeDS::Execute(ACString & strErr, ACString &strDetail)
{	
	bool bToCrossServer = !CECCrossServer::Instance().IsOnSpecialServer();
	strDetail.Format(_AL("Change DS(%s)"), bToCrossServer ? "To CrossServer" : "Go Back");	
	GetGameSession()->gm_ChangeDS(bToCrossServer);
	return true;
}

ACString CGMCommandChangeDS::GetDisplayName()
{
	int idString = CECCrossServer::Instance().IsOnSpecialServer() ? 10871 : 10870;
	return GETSTRINGFROMTABLE(idString);
}