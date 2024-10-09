// File		: DlgWebTradeOption.h
// Creator	: Feng Ning
// Date		: 2010/07/06

#pragma once

#include "DlgBase.h"

class CDlgWebTradeOption : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandWhisper(const char *szCommand);
	void OnCommandCopyID(const char *szCommand);
	void OnCommandCopyURL(const char *szCommand);
	void OnCommandGotoWeb(const char *szCommand);

	CDlgWebTradeOption();
	virtual ~CDlgWebTradeOption();
private:

	bool CopyToClipboard(const ACString& str);
	ACString CreateItemURL();
};
