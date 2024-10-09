// File		: DlgPwdProtect.h
// Creator	: Feng Ning
// Date		: 2010/10/25

#pragma once

#include "DlgBase.h"

namespace GNET
{
	class MatrixChallenge;
}

class CECLoginUIMan;
class CDlgPwdProtect : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommand_Cancel(const char* szCommand);
	void OnCommand_Link(const char* szCommand);
	void OnCommand_Confirm(const char* szCommand);
	void OnCommand_Clear(const char* szCommand);

	bool ShowMatrix(GNET::MatrixChallenge* p);
	void HideMatrix();

protected:
	virtual void OnTick();

	int  GetProtectType();
	bool CanConfirm();
};
