// Filename	: DlgBuddyState.h
// Creator	: Xiao Zhou
// Date		: 2005/12/13

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CDlgBuddyState : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgBuddyState();
	virtual ~CDlgBuddyState();

	void OnCommand_Cancel(const char * szCommand);
	void OnCommand_BindCancel(const char * szCommand);
	void OnCommand_Kiss(const char * szCommand);

	void SetBuddyID(int idPlayer);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();

	PAUIIMAGEPICTURE	m_pImg_Prof;
	PAUILABEL			m_pTxt_Name;
	PAUISTILLIMAGEBUTTON	m_pBtn_Kiss;
};
