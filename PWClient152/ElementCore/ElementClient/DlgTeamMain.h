// File		: DlgTeamMain.h
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#pragma once

#include "DlgBase.h"

class CDlgTeamMate;
class CDlgTeamMain : public CDlgBase  
{
	friend class CDlgWorldMap;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTeamMain();
	virtual ~CDlgTeamMain();

	void OnCommandCANCAL(const char *szCommand);
	void OnCommandMinimize(const char *szCommand);
	void OnCommandMinimizeBuff(const char *szCommand);
	void OnCommandArrow(const char *szCommand);
	void OnCommandGTTalk(const char *szCommand);

	bool UpdateTeamInfo();
	bool IsShowTeamArrow() { return m_bShowTeamArrow; }

	CDlgTeamMate* GetTeamMateDlg(int index);
	void ProcessGTEnter();

protected:
	virtual bool OnInitDialog();

private:
	bool m_bShowTeamArrow;
	bool m_bShowBuff;

	abase::vector<CDlgTeamMate*> m_pMates;

	enum
	{
		TEAM_STATUS_OPEN = 0,
		TEAM_STATUS_CLOSE
	};

	bool m_bGTWaiting;
};
