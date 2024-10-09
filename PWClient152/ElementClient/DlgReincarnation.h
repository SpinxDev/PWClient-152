// Filename	: DlgReincarnation.h
// Creator	: Han Guanghui
// Date		: 2013/08/06

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgReincarnation : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgReincarnation();
	virtual ~CDlgReincarnation();

	void OnCommandConfirm(const char* szCommand);
	void OnCommandCancel(const char* szCommand);

protected:
	void Update();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
extern const int ATTRIBUTE_POINT[];