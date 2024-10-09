// Filename	: DlgELFProp.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
class CDlgELFGenius : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFGenius();
	virtual ~CDlgELFGenius();
	
	void OnCommand_add(const char * szCommand);
	void RefreshHostDetails();
	void OnCommand_skillview(const char * szCommand);
	virtual bool Render(void);
	
protected:
	
	PAUIIMAGEPICTURE	m_pImg_Char;
	PAUIIMAGEPICTURE	m_pImg_Genius[5];
	PAUILABEL			m_pTxt_Genius[5];
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
