/*
 * FILE: DlgMatchResult.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"

#include <AUIComboBox.h>
#include <AUIListBox.h>
#include "EC_Counter.h"

namespace GNET{
	class Protocol;
}

//////////////////////////////////////////////////////////////////////////

class CDlgMatchInfoHint : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	
	void OnCommandInfo(const char *szCommand); // 完善资料

	void OnServerNotify(GNET::Protocol* pProtocol);
	
};
//////////////////////////////////////////////////////////////////////////

class CDlgMatchModeSelect : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);	
	void OnCommandDo(const char *szCommand); // 开始匹配
	void OnCommandInfo(const char *szCommand); //完善资料
	
	void ResetCounter(DWORD tick) { m_matchtime = tick;}
	int GetMatchMode() ;

	void CheckButtonState();
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();	

protected:
	PAUICOMBOBOX m_pComList;
	
	DWORD m_matchtime;
};

//////////////////////////////////////////////////////////////////////////

class CDlgMatchResult : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

	enum
	{
		MATCH_NONAME = 0,
		MATCH_HASNAME,
	};

public:
	void OnCommandSayHi(const char *szCommand);
	void OnCommandChat(const char *szCommand);
	void OnCommandTeam(const char *szCommand);
	void OnCommandFriend(const char *szCommand);

	void OnCommandNext(const char *szCommand);

	void OnMatchResultRe(GNET::Protocol* pProtocl);

	void CheckButtonState();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();	

	int GetSelRoleid();

protected:
	PAUILISTBOX m_pLst;
	
	DWORD m_matchtime;
};

//////////////////////////////////////////////////////////////////////////

class CDlgMatchAward : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:

	void OnCommandAward(const char *szCommand);
	void SetType(bool bOne) { m_bOneAward = bOne;}
	
protected:
	virtual void OnShowDialog();
	
protected:
	bool m_bOneAward;
};

