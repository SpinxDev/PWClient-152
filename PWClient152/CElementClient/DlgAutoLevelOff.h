/*
 * FILE: DlgAutoLevelOff.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_LEVEL_OFF_H_
#define _DLG_AUTO_LEVEL_OFF_H_

#include "DlgHomeBase.h"

class AUIStillImageButton;

class CDlgAutoLevelOff : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoLevelOff();
	virtual ~CDlgAutoLevelOff();
	
	virtual AUIStillImageButton * GetSwithButton();
	void OnCommandCreateFlat(const char* szCommand);
	

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext(){return true;};
	virtual void DoDataExchange(bool bSave){};
};

#endif