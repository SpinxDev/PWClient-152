// Filename	: DlgExclusiveAward.h
// Creator	: Han Guanghui
// Date		: 2013/08/24

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "EC_Counter.h"
#include <vector>
#include <AUICheckBox.h>

class ATaskTempl;
class CDlgExclusiveAward : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgExclusiveAward();
	virtual ~CDlgExclusiveAward();

	void				OnCommandConfirm(const char* szCommand);
	void				OnCommandCancel(const char* szCommand);
	void				OnCommandUp(const char* szCommand);
	void				OnCommandDown(const char* szCommand);
	void				OnCommandCheckMode(const char* szCommand);

	void				OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	

	bool				HasTaskToShow();
	bool				GetExclusiveAwardMode();

protected:
	int					CalculatePageSize();
	void				Update();
	void				UpdatePage();
	void				GenerateTaskVec();
	void				ClampPageStartIndex();
	bool				HasTask();
	int					GetTaskCount();
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnTick();
	void				UpdateOneTask(const ATaskTempl* pTempl, int iLine, unsigned long ulCurTime);
	void				ShowLine(int iLine, bool bShow);
	bool				IsValidTaskIndex(int index);
	int					LastPageStartIndex();
	int					MaxTaskIndex();
	bool				HasPreviousPage();
	bool				HasNextPage();
	typedef std::vector<const ATaskTempl*> TASKS;
	typedef std::vector<TASKS*> TASKS_TO_SHOW;
	TASKS_TO_SHOW		GetTasksToShow();
	const ATaskTempl*	GetOneTaskTempl(TASKS_TO_SHOW& task_to_show, int index);
	void				ScrollPage(bool bUp, int count);
	int					m_nPageSize;
	int					m_iStartIndex;
	CECCounter			m_UpdateCounter;

	PAUICHECKBOX m_pChk_NoAlways;

	TASKS task_ready_to_finish; 
	TASKS task_realm;
	TASKS task_level;
	TASKS task_time; 
};
