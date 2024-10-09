// Filename	: DlgForceNPC.h
// Creator	: Feng Ning
// Date		: 2011/10/19

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

struct FORCE_CONFIG;
class CDlgForceNPC : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgForceNPC();
	virtual ~CDlgForceNPC();

	void OnCommand_Join(const char * szCommand);
	void OnCommand_Quit(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	const FORCE_CONFIG* m_pConfig;
	bool UpdateAll(int forceID);
};
