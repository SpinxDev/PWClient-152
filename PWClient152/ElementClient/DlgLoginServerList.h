// Filename	: DlgLoginServerList.h
// Creator	: Xiao Zhou
// Date		: 2005/11/4
#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIManager.h"

class CDlgCurrentServer;
class CDlgLoginServerList : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLoginServerList();

	void OnCommandCancel(const char* szCommand);
	void OnCommandConfirm(const char* szCommand);
	void OnCommandSortName(const char* szCommand);
	void OnCommandSortSpeed(const char* szCommand);
	void OnCommandSortStats(const char* szCommand);
	void OnCommandSortOther(const char* szCommand);
	void OnCommandQuery(const char *szCommand);
	
	void OnEventSelect_LstServer(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventSelect_LstServerGroup(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	bool SetServerGroup(const ACString &strGroup);	
	void SelectServerByName(ACString strServer);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	virtual bool Release();

	void SelectGroup(int nGroup);

	enum{
		COL_PINGTIME,
		COL_SERVERINDEX,
		COL_LINE,
	};
	
	bool								m_bSortName;
	bool								m_bSortSpeed;
	bool								m_bSortStats;
	bool								m_bSortOther;
	PAUILISTBOX							m_pLst_Server;
	PAUILISTBOX							m_pLst_ServerGroup;
	
	CDlgCurrentServer *					m_pDlgCurrentServer;
};

class CDlgLoginServerListButton : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	
public:	
	void OnCommandCancel(const char* szCommand);
	void OnCommandConfirm(const char* szCommand);
	void OnCommandQuery(const char *szCommand);
	
protected:
};

