// Filename	: DlgELFGeniusReset.h
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUIStillImageButton.h"
#include "AUITextArea.h"
#include "AUIEditBox.h"
#include "EC_IvtrItem.h"

class CDlgELFTransaction : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFTransaction();
	virtual ~CDlgELFTransaction();
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_ToProtect(const char * szCommand);
	void OnCommand_ToFree(const char * szCommand);
	void OnCommand_StopChange(const char * szCommand);
	virtual void OnTick(void);
	
protected:
	PAUILABEL	m_pTxt_Change;
	PAUILABEL	m_pLab_ChangeTime;
	PAUILABEL	m_pTxt_ChangeTime;
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
