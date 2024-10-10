/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   17:07
	file name:	GMCommandInGame.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "GMCommand.h"

namespace GNET
{
	class Privilege;
}
class CECGameSession;
class CECConfigs;
class CECHostPlayer;
class CECGameUIMan;
class CGMCommandInGameBase : public CGMCommand
{
protected:
	GNET::Privilege * GetPrivilege();
	CECGameSession * GetGameSession();
	CECConfigs * GetConfigs();
	CECHostPlayer * GetHostPlayer();
	CECGameUIMan * GetGameUIMan();
};

class CGMCommandKickOutRole : public CGMCommandInGameBase
{
public:
	CGMCommandKickOutRole();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandKickOutUser : public CGMCommandInGameBase
{
public:
	CGMCommandKickOutUser();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandShowOnlineNum : public CGMCommandInGameBase
{
public:
	CGMCommandShowOnlineNum();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandRestartServer : public CGMCommandInGameBase
{
public:
	CGMCommandRestartServer();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandShutupRole : public CGMCommandInGameBase
{
public:
	CGMCommandShutupRole();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandShutupUser : public CGMCommandInGameBase
{
public:
	CGMCommandShutupUser();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandMoveToPlayer : public CGMCommandInGameBase
{
public:
	CGMCommandMoveToPlayer();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandCallInPlayer : public CGMCommandInGameBase
{
public:
	CGMCommandCallInPlayer();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandBroadcast : public CGMCommandInGameBase
{
public:
	CGMCommandBroadcast();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandToggleID : public CGMCommandInGameBase
{
public:
	CGMCommandToggleID();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandForbidRole : public CGMCommandInGameBase
{
public:
	CGMCommandForbidRole();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandTriggerChat : public CGMCommandInGameBase
{
public:
	CGMCommandTriggerChat();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandGenerate : public CGMCommandInGameBase
{
public:
	CGMCommandGenerate();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandControlSpawnerBase : public CGMCommandInGameBase
{
public:
	CGMCommandControlSpawnerBase();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
protected:
	virtual bool ToActive() = 0;
};
class CGMCommandActiveSpawner : public CGMCommandControlSpawnerBase
{
public:
	virtual ACString GetDisplayName();
protected:
	virtual bool ToActive();
};
class CGMCommandDeactiveSpawner : public CGMCommandControlSpawnerBase
{
public:
	virtual ACString GetDisplayName();
protected:
	virtual bool ToActive();
};

class CGMCommandGenerateMob : public CGMCommandInGameBase
{
public:
	CGMCommandGenerateMob();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandInvisible : public CGMCommandInGameBase
{
public:
	CGMCommandInvisible();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandInvincible : public CGMCommandInGameBase
{
public:
	CGMCommandInvincible();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};

class CGMCommandQuerySpecItem : public CGMCommandInGameBase
{
public:
	CGMCommandQuerySpecItem();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
private:
	int m_nPlayerID;
};

class CGMCommandPermanentLock : public CGMCommandInGameBase
{
public:
	CGMCommandPermanentLock();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandUnLock : public CGMCommandInGameBase
{
public:
	CGMCommandUnLock();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void TrySetPlayerIDParam(int nPlayerID);
};

class CGMCommandOpenActivity : public CGMCommandInGameBase
{
public:
	CGMCommandOpenActivity();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void SetCurSelParam(int idx) {m_iCurSel = idx;}
protected:
	int m_iCurSel;
};

class CGMCommandCloseActivity : public CGMCommandInGameBase
{
public:
	CGMCommandCloseActivity();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
	virtual void SetCurSelParam(int idx) {m_iCurSel = idx;}
protected:
	int m_iCurSel;
};

class CGMCommandChangeDS : public CGMCommandInGameBase
{
public:
	CGMCommandChangeDS();
	virtual bool HasPrivilege();
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();
};