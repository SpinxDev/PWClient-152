// Filename	: DlgQuestionTask.h
// Creator	: zhangyitian
// Date		: 2014/11/11

#ifndef _ELEMENTCLIENT_DLGQUESTIONTASK_H_
#define _ELEMENTCLIENT_DLGQUESTIONTASK_H_

#include "DlgBase.h"

class AUILabel;
class AUITextArea;
class AUIStillImageButton;

class CDlgQuestionTask : public CDlgBase {
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgQuestionTask();
	void AddQuestionTask(int iTaskID);
	void RemoveQuestionTask(int iTaskID);
	void OnCommand_Next(const char* szCommand);
	void OnCommand_Prev(const char* szCommand);
	void OnCommand_Minmize(const char* szCommand);
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
private:
	void UpdateView();
	void UpdateQuestDesc();
	void UpdateTimeLimit();
	void UpdateSwitchBtn();

	int GetIndexOfTask(int iTaskID);
	bool IsLegalTask(int iTaskID, bool bNewTask);
private:
	abase::vector<int> m_vecQuestionTasks;		// 当前存在的答题任务。正常情况下应该只有一个，但不排除玩家使用外挂等特殊情况。存储的是父任务id
	int m_iCurQuestionTask;						// 当前显示的答题任务，父任务id

	AUITextArea* m_pTxtQuest;
	AUILabel* m_pLblTime;
	AUIStillImageButton* m_pBtnNext;
	AUIStillImageButton* m_pBtnPrev;

};

#endif