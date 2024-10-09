// Filename	: DlgTaskTrace.h
// Creator	: Feng Ning
// Date		: 2011/11/15

#pragma once

#include "DlgNameLink.h"
#include "AUITextArea.h"
#include <AUICheckBox.h>

struct Task_Region;
class CDlgTaskTrace : public CDlgNameLink  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTaskTrace();
	virtual ~CDlgTaskTrace();	
	
	void OnCommand_SwitchCollapse(const char * szCommand);
	void OnCommand_SwitchShowType(const char * szCommand);
	void OnCommand_UnTraceTask(const char * szCommand);
	void OnCommand_Chat(const char * szCommand);
	void OnCommand_Map(const char * szCommand);
	void OnCommand_FinishTask(const char * szCommand);

	void RefreshTaskTrace(const int* traced, int nTraced, const int* titletasks, int num, bool withName);
	void RefreshTaskTrace(const int* tasks, size_t size, bool withName);
	void OnEventDBClk_Title(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButton_Txt_Link_Trace(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButton_Txt_Link_Trace(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	enum ShowType
	{
		ST_NONE,				//	��ʼֵ
		ST_TRACED,				//	��ʾ��ǰ��׷������
		ST_TITLE,				//	��ʾ�ѽӳƺ�����
		ST_CONTRIBUTION,		//	��ʾ���������ѵĹ��׶�����
	};
	ShowType GetShowType();
	void SetBtnUnTraceY(int pos_y, int task_id);
	void OnTaskCompleted(int id);
	void UpdateContributionTask();
	void OnTaskNew(unsigned long idTask);

	virtual void OnChangeLayoutEnd(bool bAllDone);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	virtual void OnEventMouseMove(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink);
	virtual void OnEventLButtonClick(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink);
	virtual bool PtInRect(const AUITEXTAREA_NAME_LINK &name, int x, int y);

	void FormatTaskTraceDesc(int idTask, bool withName, const ACString& strIndentIn, bool requireActive=true, 
		int index = 0, bool bCanContributionFinish = false, int contribution = 0);
	void SwitchCollapse();
	bool IsCollapsed();

	void ResetNameUnique();
	ACString MakeNameUnique(const ACString &strName);
	void AppendCommand(int worldid, const Task_Region* pRegions, int size);
	void AppendCommand(const ACHAR* pName, int idTarget, int idTask = 0);
	void AppendText(const ACString & text);

	void DeleteColorStr(ACString& strName);

	void AddTaskSetString(unsigned int id, bool hasContributionAward, int idStorage);
	void AddTaskString(unsigned int id, bool hasContributionAward, const ACString& strIndentIn, bool topLevel = false);
	bool TaskListHasID(int listID, int taskID);
	void FinishTaskSpendingContribution(int taskID);
	void AppendColorText(ACString& strTask, const ACString& strColor);
	void KeepScrollBarPosition();

	void SetShowType(ShowType rhs);

	void PrepareRebuildTaskTrace();

	ACString m_Buffer;
	PAUITEXTAREA m_pTxt_Desc;
	PAUICHECKBOX m_pChk_Collapse;

	typedef abase::hash_map<ACString, int>	UniqueNameCountMap;
	UniqueNameCountMap	m_uniqueNameCountMap;

	int			m_nLastTracedTasks;				//	����ʾ���ݶ�Ӧ��׷��������������ĳЩ����������������б���л���

	ShowType	m_nLastShowType;		//	����ʾ���ݶ�Ӧ���б�����
	int			m_nTraceWorldID;		//	����ʾ��Ϣ��׷����Ϣ��Ӧ�ĵ�ͼID
	PAUIOBJECT	m_pBtnUnTrace;
	PAUIOBJECT	m_pBtnMap;
	PAUIOBJECT	m_pBtnChat;
	PAUIOBJECT	m_pBtnFinishTaskByContribution;
	PAUIOBJECT	m_pBtnContributionTaskHelp;
	int			m_iContributionCurrentPage;
	int			m_iContributionTotalPage;
	int			m_iIndexForContribution;

	bool		m_bShowTrace;
};
