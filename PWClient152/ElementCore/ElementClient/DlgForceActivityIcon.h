// Filename	: DlgForceActivityIcon.h
// Creator	: Feng Ning
// Date		: 2011/10/25

#pragma once

#include "DlgBase.h"

class CDlgForceActivityIcon : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgForceActivityIcon();
	~CDlgForceActivityIcon();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnEvent_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void RefreshForceGlobalData(bool dataReady);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();

private:
	int m_LastTimeStamp;

	PAUIOBJECT m_pIcon;
};