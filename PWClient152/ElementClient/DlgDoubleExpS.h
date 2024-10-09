// File		: DlgDoubleExpS.h
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

class CDlgDoubleExpS : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgDoubleExpS();
	virtual ~CDlgDoubleExpS();

	void OnCommandCancel(const char * szCommand);

	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUIIMAGEPICTURE	m_pImg_Icon;
	PAUIIMAGEPICTURE	m_pImg_Captivation;
	PAUILABEL			m_pTxt_RestTime;
	int					m_nMode;
};
