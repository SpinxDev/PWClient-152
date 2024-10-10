#pragma once

#include "DlgBase.h"

#define CDLGSETTINGSCRIPTHELP_TIPTYPES_PP			7

class AUICheckBox;
class CECScriptContext;

class CDlgSettingScriptHelp : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgSettingScriptHelp();
	virtual ~CDlgSettingScriptHelp();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();

	void SetScriptContext(CECScriptContext * pContext);
	
	void OnCommandIDCANCEL(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);

private:
	CECScriptContext *		m_pContext;
	bool					m_bTipType[CDLGSETTINGSCRIPTHELP_TIPTYPES_PP];
};




















