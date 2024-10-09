// Filename	: DlgAward.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "AUIStillImageButton.h"
#include "DlgBase.h"

class CDlgAward : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgAward();
	virtual ~CDlgAward();

	void OnCommand_confirm(const char * szCommand);
	void OnCommand_select(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void UpdateAwardItem(unsigned short idTask, bool throghNPC, bool spendContribution = false);

protected:
	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;

	virtual bool OnInitDialog();
	bool m_bThroughNPC;
	bool m_bSpendContribution;
};
