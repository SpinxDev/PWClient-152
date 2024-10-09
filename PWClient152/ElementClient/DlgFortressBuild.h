// Filename	: DlgFortressBuild.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUISubDialog.h"

class CDlgFortressBuildSubList;

class CDlgFortressBuild : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressBuild();
	
	void OnCommand_CANCEL(const char *szCommand);

	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool OnChangeLayoutBegin();
	
private:
	CDlgFortressBuildSubList *m_pDlgBuildList;	//	×Ó¶Ô»°¿ò
};