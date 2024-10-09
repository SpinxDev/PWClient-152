// File		: DlgTextHelp.h
// Creator	: Feng Ning
// Date		: 2010/11/18

#pragma once

#include "DlgBase.h"

class AUITextArea;
class CDlgTextHelp : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTextHelp();
	virtual ~CDlgTextHelp();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	void RefreshHelpContent();
	bool LoadText(const char* pFile, AUITextArea* pText);
};
