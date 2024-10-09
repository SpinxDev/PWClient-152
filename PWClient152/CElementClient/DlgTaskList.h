// Filename	: DlgTaskList.h
// Creator	: Feng Ning
// Date		: 2010/09/01

#pragma once

#include "DlgTask.h"

struct NPC_TASK_OUT_SERVICE;
class CDlgTaskList : public CDlgTask
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTaskList();

	void OnCommand_Accept(const char * szCommand);
	void OnCommand_Refresh(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnEventLButtonDown_Tv_Quest(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void ConfirmRefresh(PAUIDIALOG pDlg, int ret);
	void ConfirmAccept(PAUIDIALOG pDlg, int ret);
	void RefreshTaskList();

protected:
	virtual bool Tick(void);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release(void);
	void ShowTaskDetail(int idTask);

	const NPC_TASK_OUT_SERVICE* GetCurrentTask();

	int m_iTaskToDeliverPerDay;
};