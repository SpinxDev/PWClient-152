// Filename	: DlgDownloadSpeed.h
// Creator	: Shizhenhua
// Date		: 2013/8/3

#pragma once

#include "DlgBase.h"

class AUIComboBox;

class CDlgDownloadSpeed : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgDownloadSpeed();

	void OnCommand_Start(const char* szCommand);

protected:
	AUIComboBox* m_pCmbDownSpeed;
	AUIComboBox* m_pCmbUpSpeed;

	virtual bool OnInitDialog();
};

