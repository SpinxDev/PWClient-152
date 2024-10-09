// Filename	: DlgTask.h
// Creator	: Tom Zhou
// Date		: October 12, 2005

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUITreeView.h"
#include "AUITextArea.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include "hashmap.h"
#include "EC_Game.h"
#include "EC_Counter.h"

#define CDLGTASK_AWARDITEM_MAX 8

struct Task_State_info;
struct MINE_ESSENCE;
struct Task_Award_Preview;
class CECQuickBuyPopActivityTrigger;
class ATaskTempl;

class CDlgTask : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:

	struct TASK_OBJECT_POS
	{
		int x, y, z, mapid;
	};
	struct TASK_FINISHED_TIME
	{
		int iTaskID;
		DWORD dwTime;
	};

	CDlgTask();
	virtual ~CDlgTask();
	virtual bool Tick(void);

	void OnCommand_searchquest(const char * szCommand);
	void OnCommand_havequest(const char * szCommand);
	void OnCommand_showtrace(const char * szCommand);
	void OnCommand_focus(const char * szCommand);
	void OnCommand_abandon(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_TreasureMap(const char * szCommand);
	void OnCommand_FinishTask(const char * szCommand);
	void OnCommand_GotoNPC(const char * szCommand);

	void OnEventLButtonDown_Tv_Quest(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Txt_QuestItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Txt_QuestItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Award_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void GetItemLinkItemOn(int x, int y, PAUIOBJECT pObj, AUITEXTAREA_EDITBOX_ITEM *pLink);

	// get formatted data
	static ACString FormatTaskText(const ACHAR* szText, A3DCOLOR background);
	static A3DCOLOR GetTaskColor(int idType);
	static A3DCOLOR GetTaskColor(const ATaskTempl *pTempl);
	static ACString FormatTime(int nSec, const ACString& desc, int timeLimit);
	static ACString GetTaskNameWithColor(const ATaskTempl* pTempl);
	static ACString GetTaskNameWithOutColor(const ATaskTempl* pTempl);

	void TickTaskTrace();

	void RefreshTaskTrace();
	bool UpdateTask(int idTask = -1);

	bool SearchForTask(int idTask = -1);

	//更新整个任务列表，包括可接任务和已接任务 zhangyitian
	bool UpdateQuestView();
	
	bool IsPQTaskOrSubTask(int idTask);
	bool IsTreasureMapTask(int idTask);

	bool TraceTask(int idTask);
	void SyncTrace(void* pData, bool fromServer);
	bool IsShowTrace(){return m_bShowTrace;}

	typedef CECGame::ObjectCoords ObjectCoords;
	static const ObjectCoords& GetObjectCoords() { return m_TargetCoord; }
	static const ACString& GetTraceName() { return m_strTraceName; }
	static void SetTraceObjects(const ObjectCoords& objs, const ACString& name);
	static const MINE_ESSENCE* SearchTaskMine(int idTask);

	ACString GetKillPlayerRequirements(const Task_State_info& tsi,int iIndex);

	void SwitchTaskTrace(int idTask);
	void OnTaskPush(); // 有新的可接任务
	void OnTaskProcessUpdated(int idTask); // 已接任务中有需要提前显示的
	void OnTaskItemGained(int idItem);

protected:
	int m_idLastTask;
	int m_idSelTask;
	
	bool m_bTraceNew;
	bool m_bShowTrace;

	int m_iType;

	PAUILABEL m_pTxt_QuestNO;
	PAUITREEVIEW m_pTv_Quest;
	PAUITEXTAREA m_pTxt_Content;
	PAUITEXTAREA m_pTxt_QuestItem;
	PAUISTILLIMAGEBUTTON m_pBtn_Abandon;
	PAUISTILLIMAGEBUTTON m_pBtn_MainQuest;
	PAUISTILLIMAGEBUTTON m_pBtn_NormalQuest;
	PAUISTILLIMAGEBUTTON m_pBtn_SearchQuest;
	PAUISTILLIMAGEBUTTON m_pBtn_HaveQuest;
	PAUISTILLIMAGEBUTTON m_pBtn_bShowTrace;
	PAUISTILLIMAGEBUTTON m_pBtn_FinishTask;
	PAUILABEL m_pTxt_BaseAward;
	PAUIIMAGEPICTURE m_pImg_Item[CDLGTASK_AWARDITEM_MAX];
	unsigned int m_ImgCount;

	PAUISTILLIMAGEBUTTON m_pBtn_GotoNPC;
	CECQuickBuyPopActivityTrigger *m_pQuickBuyTrigger;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnChangeLayout(PAUIOBJECT pMine, PAUIOBJECT pTheir);
	virtual void OnChangeLayoutEnd(bool bAllDone);

	void InsertTaskChildren(P_AUITREEVIEW_ITEM pRoot, unsigned long idTask, bool bExpand, bool bKey);
	void SetTextItemText(const ACString &strNewTextItem, bool keepScrollPos, const ACString& strNewHintItem);
	void SetTaskText(PAUIOBJECT pObj, ACString* pStr);
	int	 GetTaskIndex(int idTask);

	bool IsQuest()const;
	bool IsShowHaveQuest()const;
	int  GetSelectedTaskFromUI();
	bool IsTreasureMapSelected();

	// update task content in dialog
	void UpdateBaseAward(const Task_Award_Preview& award);
	void UpdateItemAward(const Task_Award_Preview& award);
	void UpdateTaskBaseDesc(ACString& strText, const Task_State_info& tsi);
	void UpdateDeliverNPC(ACString& strText, int nDNPC);
	A3DVECTOR3 UpdateAwardNPC(ACString& strText, int nANPC);
	void UpdateCompleteCondition(ACString& strText, ACString& strHint, const Task_State_info& tsi);
	void UpdateItemWanted(ACString& strText, const Task_State_info& tsi, int idTask);
	void UpdateTreasureMap(ACString& strText);
	void UpdateTaskConfirm(int idTask, bool bFinishType);
	void UpdateGotoNPC();
	void ClearGotoNPC();

	// clear the task content in dialog
	void ClearContent(bool clearNPC);

	// add node to task tree
	void AddTaskNode(int id);
	// 任务按类型排序
	struct TypeNode{
		DWORD type;
		P_AUITREEVIEW_ITEM item;
		
		TypeNode():type(0), item(NULL){}
		TypeNode(DWORD t, P_AUITREEVIEW_ITEM p):type(t), item(p){}
		bool operator < (const TypeNode& rhs) const {
			return type < rhs.type;
		}
	};
	// 任务按所需等级排序
	struct LevelNode{
		int level;
		P_AUITREEVIEW_ITEM item;

		LevelNode():level(0), item(NULL){}
		LevelNode(int l, P_AUITREEVIEW_ITEM p):level(l), item(p){}
		bool operator < (const LevelNode& rhs) const {
			return level > rhs.level;
		}
	};
	void SortTaskNodeByType();
	

	// whether the task can be traced
	bool IsTaskTraceable(int idTask);

private:
	static abase::vector<int> m_vecTasksUnFinish;
	static abase::vector<int> m_vecTasksCanFinish;
	static abase::vector<CECGame::OBJECT_COORD> m_TargetCoord;
	static ACString m_strTraceName;

	// store the mine related to tasks
	typedef abase::hash_map<int, const MINE_ESSENCE*> MineMap;
	static MineMap m_TaskMines;
	CECCounter m_TaskTraceCounter;
};
