/********************************************************************
	created:	2005/10/14
	created:	14:10:2005   15:56
	file name:	DlgGMDelMsgReason.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once


#include "DlgBase.h"

class AUIComboBox;

class CDlgGMDelMsgReason : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
private:
	AUIComboBox * m_pCbxReason;
	int m_nSelReasonIndex;
	ACString m_strCustomReason;
protected:
	virtual bool OnInitDialog();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();

public:
	static const int predefinedReason[];

	CDlgGMDelMsgReason();
	void OnCommandOK(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	void OnCommandSelectReason(const char * szCommand);
};

