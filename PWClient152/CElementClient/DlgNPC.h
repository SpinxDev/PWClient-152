// Filename	: DlgNPC.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUITextArea.h"
#include "AUIStillImageButton.h"
#include "elementdataman.h"
#include "EC_TimeSafeChecker.h"

class CDlgNPC : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

	struct TASK_ITEM
	{
		int task_id;
		int service;
		
		TASK_ITEM( int id, int serv ) : task_id(id), service(serv) {}
	};

public:
	CDlgNPC();
	virtual ~CDlgNPC();

	void OnCommand_back(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown_Lst_Main(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Lst_Main(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void PopupDialog(const NPC_ESSENCE *pEssence);
	void PopupNPCDialog(talk_proc *pTalk);
	void PopupNPCDialog(const NPC_ESSENCE *pEssence);
	void PopupTaskDialog(bool bTaskNPC);
	void PopupTaskFinishDialog(int idTask, talk_proc *pTalk);
	bool PopupCorrespondingServiceDialog(int idFunction, int iService, void *pData);
	void PopupStorageTaskDialog(NPC_TASK_OUT_SERVICE *pService, bool bTaskNPC);
	bool PopupTracedTaskDialog( const NPC_ESSENCE *pEssence );

	void SetTraceTaskInfo(int tid, int taskid);
	void ResetTraceTaskInfo();
	bool HasTracedTask();

	enum
	{
		NPC_DIALOG_NULL = 0,
		NPC_DIALOG_ESSENCE,
		NPC_DIALOG_TALK,
		NPC_DIALOG_TASK_LIST,
		NPC_DIALOG_TASK_TALK,
		NPC_DIALOG_TASK_AWARD
	};

protected:
	PAUILABEL m_pTxt_npc;
	PAUILISTBOX m_pLst_Main;
	PAUITEXTAREA m_pTxt_Content;
	PAUISTILLIMAGEBUTTON m_pBtn_Back;
	
	unsigned int m_lastNPCID;

	bool	m_bLButtonDown;
	
	int					m_iTracedTaskID;
	int					m_iTracedTaskNPCID;
	CECTimeSafeChecker	m_tracedTaskTimer;

	virtual bool OnInitDialog();

	void CheckTaskService(const NPC_ESSENCE *pEssence);
	void SelectListItem(int index);
};
