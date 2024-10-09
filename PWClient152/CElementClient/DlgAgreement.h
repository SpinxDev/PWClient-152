// File		: DlgAgreement.h
// Creator	: Feng Ning
// Date		: 2010/10/21

#pragma once

#include "DlgTheme.h"

class CECLoginUIMan;
class CDlgAgreement : public CDlgTheme  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgAgreement();
	virtual ~CDlgAgreement();

	void OnCommand_Decline(const char* szCommand);
	void OnCommand_Agree(const char* szCommand);
	void OnCommand_Confirm(const char* szCommand);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Render();
	virtual bool Release();

	bool LoadText();
};
