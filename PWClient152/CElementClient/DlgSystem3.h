/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   15:12
	file base:	DlgSystem3
	file ext:	h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgSystem.h"

class CDlgSystem3 : public CDlgHorizontalVertical  
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandQuit(const char * szCommand);
	void OnCommandRepick(const char * szCommand);
	void OnCommandSetting(const char * szCommand);
	void OnCommandHelp(const char * szCommand);
	void OnCommandGM(const char * szCommand);
	void OnCommandServerType(const char * szCommand);
	void OnCommandChat(const char *szCommand);
	void OnCommandQuickKey(const char *szCommand);

	CDlgSystem3();
	virtual ~CDlgSystem3();

};
