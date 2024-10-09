/********************************************************************
	created:	2005/08/17
	created:	17:8:2005   10:25
	file name:	DlgStorageChangePW.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"

class CDlgStorageChangePW : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	ACString m_strOldPW;
	ACString m_strNewPW1;
	ACString m_strNewPW2;
protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
public:
	void OnCommandConfirm(const char *szCommand);
	void OnCommandIDCANCEL(const char * szCommand);
	CDlgStorageChangePW();
	virtual ~CDlgStorageChangePW();

protected:
	void CloseDialog();
};

