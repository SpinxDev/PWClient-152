// Filename	: DlgReincarnationBook.h
// Creator	: Han Guanghui
// Date		: 2013/08/06

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgReincarnationBook : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgReincarnationBook();
	virtual ~CDlgReincarnationBook();

	void OnCommandConfirm(const char* szCommand);
	void OnCommandRewrite(const char* szCommand);
	void OnCommandCancel(const char* szCommand);
	void Update();

	// 以下函数供DlgReincarnationRewrite复用
	static void UpdateSleepWakeBtn(AUIObject* pObj);
	static void UpdateHistoryMaxLv(AUIObject* pObj);
	static void UpdateTxtBookExp(AUIObject* pObj);
	static void UpdateLblBookExp(AUIObject* pObj);
	static void UpdateWakeImg(AUIObject* pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
