// File		: DlgSystem2.h
// Creator	: Xiao Zhou
// Date		: 2005/8/15

#pragma once

#include "DlgSystem.h"

class CDlgSystem2 : public CDlgHorizontalVertical  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgSystem2();
	virtual ~CDlgSystem2();

	void OnCommandPet(const char * szCommand);
	void OnCommandAction(const char * szCommand);
	void OnCommandTeam(const char * szCommand);
	void OnCommandFriend(const char * szCommand);
	void OnCommandFaction(const char * szCommand);
	void OnCommandBBS(const char * szCommand);
	void OnCommandAddExp(const char *szCommand);
	void OnCommandAddExp2(const char* szCommand);
	void OnCommandBroadcast(const char * szCommand);
	void OnCommandForce(const char * szCommand);
	void OnCommandMatch(const char* szCommand);
};

//////////////////////////////////////////////////////////////////////////

class CDlgSystem4 : public CDlgHorizontalVertical  
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgSystem4();
	virtual ~CDlgSystem4();
	
	void OnCommandPet(const char * szCommand);
	void OnCommandAction(const char * szCommand);
	void OnCommandWiki(const char* szCommand);
	void OnCommandBroadcast(const char * szCommand);
	void OnCommandAutoRobot(const char* szCommand);
	void OnCommandOfflineShop(const char* szCommand);
};

//////////////////////////////////////////////////////////////////////////

class CDlgSystem5 : public CDlgHorizontalVertical  
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgSystem5(){};
	virtual ~CDlgSystem5(){};

	void OnCommandSkill(const char * szCommand);
	void OnCommandAddExp(const char *szCommand);
	void OnCommandAddExp2(const char* szCommand);
	void OnCommandCard(const char* szCommand); // ¿¨ÅÆ
	void OnCommandMeridian(const char* szCommand); // ÁéÂö

	void ShowNewImg(bool bShow);
protected:
	virtual bool OnInitDialog();
};
