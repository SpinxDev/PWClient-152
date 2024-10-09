/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   18:15
	file name:	DlgReportToGM.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class AUILabel;

class CDlgReportToGM : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	AUILabel * m_pLblName;
	PAUIEDITBOX m_pEbxContent;
	ACString m_strContent;
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);
public:
	void SetNameLabel(const ACHAR *szName);
	void OnCommandConfirm(const char * szCommand);

	CDlgReportToGM();
	virtual ~CDlgReportToGM();
};
