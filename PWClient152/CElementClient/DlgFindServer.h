// Filename	: DlgFindServer.h
// Creator	: Xu Wenbin
// Date		: 2009/06/24

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgFindServer : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgFindServer();
	virtual ~CDlgFindServer();

	void OnCommandQuery(const char *szCommand);
	void OnCommandCancel(const char *szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

private:
	bool FindServerImpl(const ACString &serverToFind, ACString &serverNew);

private:
	PAUIOBJECT	m_pEdit_ServerName;
};