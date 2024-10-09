// Filename	: DlgFindPlayer.h
// Creator	: Xu Wenbin
// Date		: 2009/10/09

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgFindPlayer : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	
public:
	CDlgFindPlayer();
	
	void OnCommandQuery(const char *szCommand);
	void OnCommandCancel(const char *szCommand);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
private:
	PAUIOBJECT	m_pEdit_PlayerName;
};