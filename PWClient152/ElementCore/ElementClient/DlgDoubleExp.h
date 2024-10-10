// File		: DlgDoubleExp.h
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#pragma once

#include "DlgBase.h"
#include "AUIRadioButton.h"
#include "AUILabel.h"

class CDlgDoubleExp : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgDoubleExp();
	virtual ~CDlgDoubleExp();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUIRADIOBUTTON	m_pRdo_1;
	PAUIRADIOBUTTON	m_pRdo_2;
	PAUIRADIOBUTTON	m_pRdo_3;
	PAUIRADIOBUTTON	m_pRdo_4;
	PAUILABEL		m_pTxt_Remain;
};
