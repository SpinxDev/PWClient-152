// Filename	: DlgExp.h
// Creator	: Xu Wenbin
// Date		: 2009/12/23

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "AUIRadioButton.h"

class CDlgExp : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	virtual bool Tick(void);

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_Begin(const char * szCommand);
	void OnCommand_Stop(const char * szCommand);	
	
	void OnStateChange(bool firstTime);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	bool CanBegin();
	bool CanStop();
	bool IsUISetExp();

	PAUILABEL m_pTxt_Time;
	PAUILABEL m_pTxt_Wait;
	PAUILABEL m_pTxt_Over;

	PAUISTILLIMAGEBUTTON m_pBtn_Begin;
	PAUISTILLIMAGEBUTTON m_pBtn_Stop;
};
