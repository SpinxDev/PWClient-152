/********************************************************************
	created:	2005/08/16
	created:	16:8:2005   20:36
	file name:	DlgStoragePW.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"

class CDlgStoragePW : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	void CloseDialog();
	ACString m_strText;
protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
public:
	void OnCommandIDCANCEL(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	CDlgStoragePW();
	virtual ~CDlgStoragePW();

};

