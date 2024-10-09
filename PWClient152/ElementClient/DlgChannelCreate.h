// Filename	: DlgChannalCreate.h
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#pragma once

#include "DlgBase.h"
#include "AUICheckBox.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"

class CDlgChannelCreate : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgChannelCreate();
	virtual ~CDlgChannelCreate();

	void OnCommandConfirm(const char* szCommand);
	void OnCommandEnablePW(const char* szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUICHECKBOX			m_pChk_EnablePw;
	PAUIOBJECT				m_pTxt_ChName;
	PAUIOBJECT				m_pTxt_ChPw;
	PAUISTILLIMAGEBUTTON	m_pBtn_Create;
};
