 // TaskTemplEditorView.cpp : implementation of the CTaskTemplEditorView class
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTemplEditorDoc.h"
#include "TaskTemplEditorView.h"
#include "TaskServer.h"
#include "TaskTemplMan.h"
#include "NewTaskDlg.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "VSSOperation.h"
#include "shlwapi.h"
#include "AVariant.h"
#include "FindTaskDlg.h"
#include "MonWantedDlg.h"
#include "TaskLinkDlg.h"
#include "NPCInfoDlg.h"
#include "CopySubTaskDlg.h"
#include "ExcelConverter.h"

#include <sys/stat.h>
#include <io.h>

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL _view_mode = TRUE;
BOOL _recover_mode = FALSE;
static bool _tick = FALSE;

CTaskTemplEditorView* g_pView = 0;

TaskCacheMap g_TaskCacheMap;
HANDLE       g_LoadThread = 0;
bool		 g_DataReady  = false;
bool		 g_Running    = true;
bool		_bIsClearing = false;
//GroupItemMap g_GroupItem;

static const int KEY_TASK_SUM = 106;

// test
void TaskQueryGlobalData(
	unsigned long ulTaskId,
	unsigned long ulPlayerId,
	const void* pPreservedData,
	size_t size)
{
}

// 更新任务全局数据
void TaskUpdateGlobalData(
	unsigned long ulTaskId,
	const unsigned char pData[TASK_GLOBAL_DATA_SIZE])
{
}

CRITICAL_SECTION g_CS;
void LockCS()
{
	EnterCriticalSection(&g_CS);
}

void UnlockCS()
{
	LeaveCriticalSection(&g_CS);
}

int task_load_total = 0;

bool LoadItemTask(CString strFile,HTREEITEM hItem,bool bMainThread = false)
{
	bool bRet = false;
	if (g_TaskCacheMap[hItem].Mask == 0)
	{	
		LockCS();

		bRet = g_TaskCacheMap[hItem].Task->LoadFromTextFile(g_TaskCacheMap[hItem].File);

		if(bRet) 
		{
			task_load_total++;
			g_TaskCacheMap[hItem].Mask = 1;
		}
		else
		{
			g_TaskCacheMap[hItem].Mask = -1;
		}

		UnlockCS();
	}
	
	return bRet;
}


void WaitDataReady()
{
	if (g_LoadThread && !g_DataReady)
	{
		WaitForSingleObject(g_LoadThread,INFINITE);
	}
}


struct test_task : public TaskInterface
{
	virtual unsigned long GetPlayerLevel() { return 1; }
	virtual void* GetActiveTaskList()
	{
		static char _list[TASK_ACTIVE_LIST_BUF_SIZE];
		static int i = 0;
		if (i++ == 0) memset(&_list, 0, sizeof(_list));
		return &_list;
	}
	virtual void* GetFinishedTaskList()
	{
		static char _list[TASK_FINISHED_LIST_BUF_SIZE];
		static int i = 0;
		if (i++ == 0) memset(&_list, 0, sizeof(_list));
		return &_list;
	}
	virtual void* GetFinishedTimeList() { return 0; }
	virtual unsigned long* GetTaskMask()
	{
		static unsigned long _mask = 0;
		return &_mask;
	}
	virtual void DeliverGold(unsigned long ulGoldNum) {}
	virtual void DeliverExperience(unsigned long ulExp) {}
	virtual void DeliverSP(unsigned long ulSP) {}
	virtual void DeliverReputation(long lReputation) {}
	virtual int GetTaskItemCount(unsigned long ulTaskItem) { return 1; }
	virtual int GetCommonItemCount(unsigned long ulCommonItem) { return 1; }
	virtual bool IsInFaction(unsigned long ulFactionId) { return true; }
	virtual unsigned long GetGoldNum() { return 10000; }
	virtual void TakeAwayGold(unsigned long ulNum) {}
	virtual void TakeAwayTaskItem(unsigned long ulTemplId, unsigned long ulNum) {}
	virtual void TakeAwayCommonItem(unsigned long ulTemplId, unsigned long ulNum) {}
	virtual long GetReputation() { return 1; }
	virtual unsigned long GetCurPeriod() { return 1; }
	virtual unsigned long GetPlayerId() { return 1; }
	virtual unsigned long GetPlayerRace() { return 1; }
	virtual unsigned long GetPlayerOccupation() { return 1; }
	virtual bool CanDeliverCommonItem(unsigned long) { return true; }
	virtual bool CanDeliverTaskItem(unsigned long) { return true; }
	virtual void DeliverCommonItem(unsigned long ulItem, unsigned long) {}
	virtual void DeliverTaskItem(unsigned long ulItem, unsigned long) {}
	virtual int GetTeamMemberNum() { return 1; }
	virtual void GetTeamMemberInfo(int nIndex, task_team_member_info* pInfo)
	{
		pInfo->m_ulId		= 1;
		pInfo->m_ulLevel	= 1;
		pInfo->m_bMale		= true;
		pInfo->m_ulOccupation	= 1;
	}
	virtual bool IsDeliverLegal() { return true; }
	virtual bool IsCaptain() { return true; }
	virtual bool IsInTeam() { return true; }
	virtual unsigned long GetTeamMemberId(int nIndex) { return 1; }
	virtual bool IsMale() { return true; }
	virtual unsigned long GetPos(float pos[3]) { return 1;}
	virtual bool HasLivingSkill(unsigned long) { return false; }
	virtual long GetLivingSkillProficiency(unsigned long) { return 0; }
	virtual long GetLivingSkillLevel(unsigned long) { return 0; }
	virtual void GetSpecailAwardInfo(special_award* p) { p->id1 = 1; p->id2 = 0; }
	virtual bool IsGM() { return true; }
	virtual bool IsMarried() { return true; }

#ifdef _TASK_CLIENT
	virtual void NotifyServer(const void* pBuf, size_t sz) {}
#else
	virtual void NotifyClient(const void* pBuf, size_t sz) {}
	virtual void NotifyPlayer(unsigned long ulPlayerId, const void* pBuf, size_t sz) {}
	virtual float UnitRand() { return 1.0f; }
	virtual void GetTeamMemberPos(int nIndex, float pos[3]) {}
#endif
};

#ifdef _TASK_CLIENT
	void TaskInterface::UpdateTitleUI(unsigned long ulTask) {}
	void TaskInterface::UpdateTaskUI(unsigned long idTask, int reason) {}
	void TaskInterface::TraceTask(unsigned long idTask) {}
	void TaskInterface::ShowTaskMessage(unsigned long ulTask, int reason) {}
	void TaskInterface::PopupTaskFinishDialog(unsigned long ulTaskId, const talk_proc* pTalk) {}
	int TaskInterface::GetTimeZoneBias() { return 0; }
	void TaskInterface::ShowPunchBagMessage(bool bSucced,bool bMax,unsigned long MonsterTemplID,int dps,int dph){};
	void TaskInterface::PopEmotionUI(unsigned int task_id,unsigned int uiEmotionMask,bool bShow){};
#endif

//test_task task;

unsigned long TaskInterface::GetCurTime() { return 1; }
void TaskInterface::WriteLog(int nPlayerId,	int nTaskId, int nType, const char* szLog) {}

// History Part
const TCHAR _his_file_format[] = _T("_gfx_op_%d.his");
extern CString g_strTempHisDir;

BOOL IsReadOnly(LPCTSTR szFile)
{
	DWORD dwFlag = GetFileAttributes(szFile);
	if (dwFlag == (DWORD)-1) return FALSE;
	return dwFlag & FILE_ATTRIBUTE_READONLY;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplClass

class CTaskTemplClass
{
protected:
	CTaskTemplClass() {}
	const char* m_szClassName;

public:
	virtual ~CTaskTemplClass() {}
	const char* GetClassName() const { return m_szClassName; }
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		pTempl->m_bCanRedo		= false;
		pTempl->m_bNeedRecord	= true;
		pTempl->m_bClearAsGiveUp= false;
		pTempl->m_enumFinishType= enumTFTNPC;

		pTempl->m_DelvTaskTalk.num_window = 1;
		talk_proc::window* pWin = new talk_proc::window[1];
		g_ulNewCount++;
		pWin->id				= 0;
		pWin->id_parent			= -1;
		pWin->talk_text_len		= 1;
		pWin->talk_text			= new namechar[1];
		g_ulNewCount++;
		wcscpy(pWin->talk_text, L"");
		pWin->num_option		= 2;
		pWin->options			= new talk_proc::option[2];
		g_ulNewCount++;
		pWin->options[0].id		= NPC_GIVE_TASK;
		pWin->options[0].param	= pTempl->m_ID;
		wcscpy(pWin->options[0].text, L"接受");
		pWin->options[1].id		= TALK_RETURN;
		pWin->options[1].param	= 0;
		wcscpy(pWin->options[1].text, L"拒绝");
		pTempl->m_DelvTaskTalk.windows = pWin;

		pTempl->m_ExeTalk.num_window = 1;
		pWin = new talk_proc::window[1];
		g_ulNewCount++;
		pWin->id				= 0;
		pWin->id_parent			= -1;
		pWin->talk_text_len		= 1;
		pWin->talk_text			= new namechar[1];
		g_ulNewCount++;
		wcscpy(pWin->talk_text, L"");
		pWin->num_option		= 1;
		pWin->options			= new talk_proc::option[1];
		g_ulNewCount++;
		pWin->options[0].id		= TALK_RETURN;
		pWin->options[0].param	= 0;
		wcscpy(pWin->options[0].text, L"");
		pTempl->m_ExeTalk.windows = pWin;

		pTempl->m_AwardTalk.num_window = 1;
		pWin = new talk_proc::window[1];
		g_ulNewCount++;
		pWin->id				= 0;
		pWin->id_parent			= -1;
		pWin->talk_text_len		= 1;
		pWin->talk_text			= new namechar[1];
		g_ulNewCount++;
		wcscpy(pWin->talk_text, L"");
		pWin->num_option		= 1;
		pWin->options			= new talk_proc::option[1];
		g_ulNewCount++;
		pWin->options[0].id		= NPC_COMPLETE_TASK;
		pWin->options[0].param	= pTempl->GetID();
		wcscpy(pWin->options[0].text, L"接受");
		pTempl->m_AwardTalk.windows = pWin;

		pTempl->m_pwstrDescript = new task_char[1];
		*pTempl->m_pwstrDescript = 0;
		g_ulNewCount++;

		pTempl->m_pwstrOkText = new task_char[1];
		*pTempl->m_pwstrOkText = 0;
		g_ulNewCount++;

		pTempl->m_pwstrNoText = new task_char[1];
		*pTempl->m_pwstrNoText = 0;
		g_ulNewCount++;

		pTempl->m_pwstrTribute = new task_char[1];
		*pTempl->m_pwstrTribute = 0;
		g_ulNewCount++;
	}
};

typedef abase::vector<CTaskTemplClass*> TaskTemplClassArray;
TaskTemplClassArray _task_class_array;

class CTaskClassKillMonster : public CTaskTemplClass
{
public:
	CTaskClassKillMonster() { m_szClassName = "杀怪"; }
	~CTaskClassKillMonster() {}

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_enumMethod = enumTMKillNumMonster;
		pTempl->m_bTeamwork = true;
	}
};

class CTaskClassTeamKillMonster : public CTaskTemplClass
{
public:
	CTaskClassTeamKillMonster() { m_szClassName = "组队杀怪"; }
	~CTaskClassTeamKillMonster() {}

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_enumMethod = enumTMKillNumMonster;
		pTempl->m_bTeamwork = true;
	}
};

class CTaskClassCollectItem : public CTaskTemplClass
{
public:
	CTaskClassCollectItem() { m_szClassName = "跑路或收集物品"; }
	~CTaskClassCollectItem() {}

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_enumMethod = enumTMTalkToNPC;
	}
};

class CTaskClassSpecialAward : public CTaskTemplClass
{
public:
	CTaskClassSpecialAward() { m_szClassName = "运营特殊活动"; }
	~CTaskClassSpecialAward() {}

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_enumMethod = enumTMTalkToNPC;
		pTempl->m_enumFinishType = enumTFTNPC;
		pTempl->m_bCanGiveUp = false;
		pTempl->m_bAutoDeliver = true;
		pTempl->m_DynTaskType = enumDTTSpecialAward;
	}
};

class CTaskClassKillMonsterDyn : public CTaskTemplClass
{
public:
	CTaskClassKillMonsterDyn() { m_szClassName = "杀怪_动态任务"; }

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_bCanRedo = true;
		pTempl->m_bNeedRecord = false;
		pTempl->m_enumMethod = enumTMKillNumMonster;
		pTempl->m_DynTaskType = enumDTTNormal;
	}
};

class CTaskClassCollectItemDyn: public CTaskTemplClass
{
public:
	CTaskClassCollectItemDyn() { m_szClassName = "收集物品_动态任务"; }

public:
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);

		pTempl->m_bCanRedo = true;
		pTempl->m_bNeedRecord = false;
		pTempl->m_enumMethod = enumTMTalkToNPC;
		pTempl->m_DynTaskType = enumDTTNormal;
	}
};
class CTaskClassGiftCardDyn : public CTaskTemplClass
{
public:
	CTaskClassGiftCardDyn() { m_szClassName = "礼品卡_动态任务"; }
	virtual void InitTempl(ATaskTempl* pTempl)
	{
		CTaskTemplClass::InitTempl(pTempl);
		
		pTempl->m_bCanRedo = true;
		pTempl->m_bNeedRecord = false;
		pTempl->m_enumFinishType = enumTFTDirect;
		pTempl->m_enumMethod = enumTMWaitTime;
		pTempl->m_ulWaitTime = 0;
		pTempl->m_DynTaskType = enumDTTGiftCard;
		pTempl->m_bAutoDeliver = false;
	}
};
void InitTaskClassArray()
{
	_task_class_array.push_back(new CTaskClassKillMonster);
	_task_class_array.push_back(new CTaskClassTeamKillMonster);
	_task_class_array.push_back(new CTaskClassCollectItem);
	_task_class_array.push_back(new CTaskClassSpecialAward);
	_task_class_array.push_back(new CTaskClassKillMonsterDyn);
	_task_class_array.push_back(new CTaskClassCollectItemDyn);
	_task_class_array.push_back(new CTaskClassGiftCardDyn);
}

void ReleaseTaskClassArray()
{
	for (size_t i = 0; i < _task_class_array.size(); i++)
		delete _task_class_array[i];

	_task_class_array.clear();
}

// 检查任务tkt文件是否存在
bool FileExist(const char* strFileName)
{
	struct stat tempStat;
	return (stat(strFileName, &tempStat) == 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorView

static BOOL _class_mode = FALSE;

IMPLEMENT_DYNCREATE(CTaskTemplEditorView, CFormView)

BEGIN_MESSAGE_MAP(CTaskTemplEditorView, CFormView)
	//{{AFX_MSG_MAP(CTaskTemplEditorView)
	ON_NOTIFY(NM_RCLICK, IDC_TASK_TREE, OnRclickTaskTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TASK_TREE, OnSelchangedTaskTree)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_NEW_TASK, OnNewTask)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EXPORT, OnExport)
	ON_COMMAND(ID_VIEW, OnView)
	ON_UPDATE_COMMAND_UI(ID_VIEW, OnUpdateView)
	ON_COMMAND(ID_FIND_TASK, OnFindTask)
	ON_COMMAND(ID_TEST_PACK, OnTestPack)
	ON_COMMAND(ID_EXPORT_DESCRIPT, OnExportDescript)
	ON_COMMAND(ID_CLASS_MODE, OnClassMode)
	ON_UPDATE_COMMAND_UI(ID_CLASS_MODE, OnUpdateClassMode)
	ON_NOTIFY(NM_RCLICK, IDC_TASK_TEMPL_CLASS, OnRclickTaskTemplClass)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TASK_TEMPL_CLASS, OnSelchangedTaskTemplClass)
	ON_BN_CLICKED(IDC_APPLY_CLASS, OnApplyClass)
	ON_COMMAND(ID_TASK_LINK, OnTaskLink)
	ON_UPDATE_COMMAND_UI(ID_SAVEALL, OnUpdateSaveall)
	ON_UPDATE_COMMAND_UI(ID_NEW_TASK, OnUpdateNewTask)
	ON_COMMAND(ID_FIND_NEXT, OnFindNext2)
	ON_COMMAND(ID_FIND_NAME, OnFindName2)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CHECK_NPC, OnCheckNpc)
	ON_COMMAND(ID_FIND_BY_ID, OnFindById2)
	ON_COMMAND(ID_TEST_DYN_PACK, OnTestDynPack)
	ON_COMMAND(ID_TEST_EXPORT_DYN, OnTestExportDyn)
	ON_COMMAND(ID_VIEW_NPC_INFO, OnViewNpcInfo)
	ON_COMMAND(ID_EDIT_NPC_INFO, OnEditNpcInfo)
	ON_COMMAND(ID_EXPORT_NPC_INFO, OnExportNpcInfo)
	ON_COMMAND(ID_RECORD, OnRecord)
	ON_UPDATE_COMMAND_UI(ID_EXPORT, OnUpdateExport)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_DESCRIPT, OnUpdateExportDescript)
	ON_UPDATE_COMMAND_UI(ID_TASK_LINK, OnUpdateTaskLink)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NPC_INFO, OnUpdateViewNpcInfo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NPC_INFO, OnUpdateEditNpcInfo)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_NPC_INFO, OnUpdateExportNpcInfo)
	ON_COMMAND(IDD_LOAD_FORM_EXCEL, OnLoadFromExcel)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_INSERT_TASK, OnInsertTask)
	ON_COMMAND(ID_ADD_SUB_TASK, OnAddSubTask)
	ON_COMMAND(ID_DEL_SUB_TASK, OnDelSubTask)
	ON_COMMAND(ID_SAVE_TASK, OnSaveTask)
	ON_COMMAND(ID_SAVEALL, OnSaveAll)
	ON_UPDATE_COMMAND_UI(ID_SAVE_TASK, OnUpdateSaveTask)
	ON_COMMAND(ID_CHECK_IN, OnCheckIn)
	ON_COMMAND(ID_CHECK_OUT, OnCheckOut)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_ADD_CLASS_TEMPL, OnCreateClassTask)
	ON_COMMAND(ID_CHECK_IN_CLASS, OnCheckInClass)
	ON_COMMAND(ID_CHECK_OUT_CLASS, OnCheckOutClass)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorView construction/destruction

CTaskTemplEditorView::CTaskTemplEditorView()
	: CFormView(CTaskTemplEditorView::IDD),
	  m_Sheet(_T("任务属性表")), m_ClassSheet("任务模板")
{
	//{{AFX_DATA_INIT(CTaskTemplEditorView)
	//}}AFX_DATA_INIT

	memset(&g_CS,0,sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(&g_CS);
	
	g_pView = this;
}

CTaskTemplEditorView::~CTaskTemplEditorView()
{
}

void CTaskTemplEditorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskTemplEditorView)
	DDX_Control(pDX, IDC_TASK_TEMPL_CLASS, m_TemplClassTree);
	DDX_Control(pDX, IDC_TASK_TREE, m_TaskTree);
	//}}AFX_DATA_MAP
}

BOOL CTaskTemplEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

const int _per_time = 250;

void CTaskTemplEditorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if (m_Sheet.m_hWnd == NULL)
	{
		m_Sheet.AddPage(&m_PropertyPage);
		m_Sheet.AddPage(&m_PremisePage);
		m_Sheet.AddPage(&m_MethodPage);
		m_Sheet.AddPage(&m_AwardPage);
		m_Sheet.Create(this, WS_CHILD | DS_MODALFRAME);
		m_Sheet.SetWindowPos(NULL, 250, 18, 750, 820, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		m_Sheet.SetActivePage(1);
		m_Sheet.SetActivePage(2);
		m_Sheet.SetActivePage(3);
		m_Sheet.SetActivePage(0);
		GetParentFrame()->RecalcLayout();
		ResizeParentToFit(FALSE);
		m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
		m_TaskTree.SetImageList(&m_ImageList, TVSIL_NORMAL);
	}

	if (m_ClassSheet.m_hWnd == NULL)
	{
		InitTaskClassArray();

		m_ClassSheet.AddPage(&m_ClassPage);
		m_ClassSheet.Create(this, WS_CHILD | DS_MODALFRAME);
		m_ClassSheet.SetWindowPos(NULL, 250, 18, 750, 670, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		m_ClassSheet.SetActivePage(0);

		m_TemplClassTree.SetImageList(&m_ImageList, TVSIL_NORMAL);
		m_TemplClassTree.ShowWindow(SW_HIDE);
		m_ClassSheet.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_APPLY_CLASS)->ShowWindow(SW_HIDE);
	}

	SetTimer(1234, _per_time, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorView diagnostics

#ifdef _DEBUG
void CTaskTemplEditorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTaskTemplEditorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTaskTemplEditorDoc* CTaskTemplEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTaskTemplEditorDoc)));
	return (CTaskTemplEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorView message handlers
void CTaskTemplEditorView::OnRclickTaskTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	CPoint pt;
	UINT uFlag;
	
	if(!g_DataReady)
		return;

	GetCursorPos(&pt);
	m_TaskTree.ScreenToClient(&pt);
	HTREEITEM hItem = m_TaskTree.HitTest(pt, &uFlag);
	m_TaskTree.ClientToScreen(&pt);

	if (hItem && (TVHT_ONITEM & uFlag))
	{
		if (m_SelectedTempls.find(hItem) == m_SelectedTempls.end())
			m_TaskTree.Select(hItem, TVGN_CARET);
		
		ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hItem);
		CMenu menu;
		menu.LoadMenu(IDR_TASK_TEMPL);
		CMenu* pSub = menu.GetSubMenu(0);
		if (pTask == NULL)
		{
			return;
		}
		if (!pTask->m_pParent)
		{
			pSub->EnableMenuItem(ID_INSERT_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSub->EnableMenuItem(ID_DEL_SUB_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			if (IsReadOnly(pTask->GetFilePath()))
			{
				pSub->EnableMenuItem(ID_ADD_SUB_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				pSub->EnableMenuItem(ID_CHECK_IN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			else
				pSub->EnableMenuItem(ID_CHECK_OUT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			ATaskTempl* pParent = pTask;
			while (pParent->m_pParent) pParent = pParent->m_pParent;
			if (IsReadOnly(pParent->GetFilePath()))
			{
				pSub->EnableMenuItem(ID_ADD_SUB_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				pSub->EnableMenuItem(ID_INSERT_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				pSub->EnableMenuItem(ID_DEL_SUB_TASK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			pSub->EnableMenuItem(ID_CHECK_IN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSub->EnableMenuItem(ID_CHECK_OUT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}

		pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
			pt.x,
			pt.y,
			this);
	}
}

void CTaskTemplEditorView::OnInsertTask()
{
#ifdef _TASK_CLIENT
	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	if (!pTask) return;
	ATaskTempl* pSub = CreateNewTask(FALSE);
	if (!pSub) return;
	pTask->InsertTaskTemplBefore(pSub);
	HTREEITEM hPrev = m_TaskTree.GetPrevSiblingItem(hSel);
	if (!hPrev) hPrev = TVI_FIRST;
	HTREEITEM hSub = m_TaskTree.InsertItem(CSafeString(pSub->GetName()), m_TaskTree.GetParentItem(hSel), hPrev);
	m_TaskTree.SetItemImage(hSub, 4, 4);
	m_TaskTree.SetItemData(hSub, (DWORD)pSub);
	m_TaskTree.Expand(hSel, TVE_EXPAND);
	m_TaskTree.Select(hSub, TVGN_CARET);
	TreeData sData(hSub,pSub->GetName(),pSub->m_DynTaskType);
	GetTaskTemplMan()->AddOneTreeItem(pSub,sData,false);
	OnSaveTask();
#endif
}

void CTaskTemplEditorView::OnCreateClassTask()
{
	HTREEITEM hSel = m_TemplClassTree.GetSelectedItem();
	if (!hSel) return;

	CTaskTemplClass* pClass = reinterpret_cast<CTaskTemplClass*>(m_TemplClassTree.GetItemData(hSel));
	CString strRelPath = pClass->GetClassName();
	strRelPath += "\\";

	CNewTaskDlg dlg;

	if (dlg.DoModal() != IDOK || dlg.m_strName.IsEmpty())
		return;

	AString strKey = strRelPath + dlg.m_strName;
	AString strFile = AString("BaseData\\TaskTemplate\\" + strKey + ".tkt");
	unsigned long ulID = g_TaskIDMan.CreateID(strKey);

	if (ulID == 0)
	{
		AfxMessageBox(_T("创建新任务ID失败！，请检查任务名是否重复"), MB_ICONSTOP);
		return;
	}

	ATaskTempl* pTask = new ATaskTempl;
	g_ulNewCount++;

	pTask->Init();
	pTask->m_ID = ulID;
	pTask->SetName(CSafeString(dlg.m_strName));
	pClass->InitTempl(pTask);
	
//	if(pTask->m_pParent->m_bPQTask)
//		pTask->m_bPQSubTask = true;

	pTask->SetFilePath(strFile);

	if (!pTask->SaveToTextFile(strFile))
	{
		AfxMessageBox(_T("保存新任务失败！"), MB_ICONSTOP);
		delete pTask;
		g_ulDelCount++;
		return;
	}

	GetTaskTemplMan()->AddOneTaskTempl(pTask);

	HTREEITEM hItem = m_TemplClassTree.InsertItem(CSafeString(pTask->GetName()), hSel);
	m_TemplClassTree.SetItemData(hItem, reinterpret_cast<DWORD>(pTask));
	m_TemplClassTree.Select(hItem, TVGN_CARET);
	
	TreeData sData(hItem,pTask->GetName(),pTask->m_DynTaskType,true);
	GetTaskTemplMan()->AddOneTreeItem(pTask,sData,true);

	CString strProj = "BaseData\\TaskTemplate\\";
	strProj += pClass->GetClassName();

	g_VSS.SetProjectPath(strProj);
	g_VSS.AddFile(pTask->GetFilePath());
	g_VSS.SetProjectPath(strProj);
	g_VSS.CheckOutFile(pTask->GetFilePath());

	if (IsReadOnly(pTask->GetFilePath()))
		m_TemplClassTree.SetItemImage(hItem, 2, 2);
	else
		m_TemplClassTree.SetItemImage(hItem, 3, 3);
}

void CTaskTemplEditorView::OnAddSubTask()
{
#ifdef _TASK_CLIENT
	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	if (!pTask) return;
	ATaskTempl* pSub = CreateNewTask(FALSE);
	if (!pSub) return;

	//Begin: Init PQ sub task. [Yongdong:2010-1-21]
	if(pTask->m_bPQTask)
	{
		pSub->m_bPQSubTask = true;
		pTask->m_bSwitchSceneFail = true;
	}
	//End.

	pTask->AddSubTaskTempl(pSub);
	HTREEITEM hSub = m_TaskTree.InsertItem(CSafeString(pSub->GetName()), hSel);
	m_TaskTree.SetItemImage(hSub, 4, 4);
	m_TaskTree.SetItemData(hSub, (DWORD)pSub);
	m_TaskTree.Expand(hSel, TVE_EXPAND);
	m_TaskTree.Select(hSub, TVGN_CARET);
	TreeData sData(hSub,pSub->GetName(),pSub->m_DynTaskType);
	GetTaskTemplMan()->AddOneTreeItem(pSub,sData,false);
	OnSaveTask();
#endif
}

static void GetSubIDArray(ATaskTempl* pSub, abase::vector<unsigned long>& IdArray)
{
	IdArray.push_back(pSub->GetID());

	pSub = pSub->m_pFirstChild;
	while (pSub)
	{
		GetSubIDArray(pSub, IdArray);
		pSub = pSub->m_pNextSibling;
	}
}

void CTaskTemplEditorView::OnDelSubTask()
{
	if (AfxMessageBox("确定要删除吗？", MB_YESNO) !=IDYES) return;

	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	if (!pTask) return;

	ATaskTempl* pParent = pTask->m_pParent;
	if (!pParent) return;

	while (pParent->m_pParent) pParent = pParent->m_pParent;

	abase::vector<unsigned long> IdArray;
	GetSubIDArray(pTask, IdArray);
	
	int iSize = IdArray.size();
	for (int i = 0;i < iSize;++i)
	{
		TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().find(IdArray[i]);
		if (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
		{
			GetTaskTemplMan()->GetAllTreeItemMap().erase(iter);
		}
	}

	g_TaskIDMan.WipeOffByID(IdArray);
	delete pTask;
	g_ulDelCount++;
	m_TaskTree.DeleteItem(hSel);
	pParent->SaveToTextFile(pParent->GetFilePath());
}

void CTaskTemplEditorView::OnSaveTask()
{
#ifdef _TASK_CLIENT
	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	if (!pTask) return;
	while (pTask->m_pParent) pTask = pTask->m_pParent;

	DWORD attr = GetFileAttributes(pTask->GetFilePath());
	if (attr == (DWORD)-1 || (attr & FILE_ATTRIBUTE_READONLY))
		return;

	if (!pTask->SaveToTextFile(pTask->GetFilePath()))
		AfxMessageBox("保存模板失败！", MB_ICONSTOP);
#endif
}

void CTaskTemplEditorView::OnSaveAll()
{
	if (_recover_mode)
		return;

//	WaitDataReady();

#ifdef _TASK_CLIENT
	HTREEITEM hChild = m_TaskTree.GetChildItem(TVI_ROOT);
	while (hChild)
	{
		ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hChild);
		if (pTask && !IsReadOnly(pTask->GetFilePath()))
		{
			if (!pTask->SaveToTextFile(pTask->GetFilePath()))
				AfxMessageBox("保存模板失败！", MB_ICONSTOP);
		}
		hChild = m_TaskTree.GetNextItem(hChild, TVGN_NEXT);
	}
#endif
}

void CTaskTemplEditorView::OnUpdateSaveTask(CCmdUI* pCmdUI)
{
	if (g_DataReady)
	{
		if (_class_mode)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
		
		HTREEITEM hSel = m_TaskTree.GetSelectedItem();
		if (!hSel) return;
		ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
		if (!pTask) return;
		ATaskTempl* pParent = pTask;
		while (pParent->m_pParent) pParent = pParent->m_pParent;
		pCmdUI->Enable(!IsReadOnly(pParent->GetFilePath()));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CTaskTemplEditorView::OnCheckIn()
{
	SelectedTempls::iterator iter;
	for (iter = m_SelectedTempls.begin();iter != m_SelectedTempls.end();++iter)
	{
		ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(iter->first);
		HTREEITEM hSel = iter->first;

		if (hSel == m_TaskTree.GetSelectedItem())
		{
			UpdateSheet(pTask,true);
		}

		if (IsReadOnly(pTask->GetFilePath()))
		{
			continue;
		}
		
		ATaskTempl* pParent = pTask;
		while (pParent->m_pParent) pParent = pParent->m_pParent;
		
		DWORD attr = GetFileAttributes(pParent->GetFilePath());
		if (attr == (DWORD)-1 || (attr & FILE_ATTRIBUTE_READONLY))
			return;
		
		if (!pParent->SaveToTextFile(pParent->GetFilePath()))
			AfxMessageBox("保存模板失败！", MB_ICONSTOP);
		
		g_VSS.SetProjectPath("BaseData\\TaskTemplate");
		g_VSS.CheckInFile(PathFindFileName(pTask->GetFilePath()));
		
		if (IsReadOnly(pTask->GetFilePath()))
		{
			m_TaskTree.SetItemImage(hSel, 2, 2);
			GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
			GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);		
		}
	}

}

void UpdateWhenCheckOut(ATaskTempl* pTask)
{
	TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().find(pTask->GetID());
	if (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
		(iter->second).SetName(pTask->GetName());
	
	ATaskTempl* pChild = pTask->m_pFirstChild;
	while (pChild)
	{
		UpdateWhenCheckOut(pChild);
		pChild = pChild->m_pNextSibling;
	}
	
}

void CTaskTemplEditorView::OnCheckOut()
{
	SelectedTempls::iterator iter;
	for (iter = m_SelectedTempls.begin();iter != m_SelectedTempls.end();++iter)
	{
		ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(iter->first);
		HTREEITEM hSel = iter->first;

		if(!IsReadOnly(pTask->GetFilePath()))
			continue;
		g_VSS.SetProjectPath("BaseData\\TaskTemplate");
		g_VSS.CheckOutFile(pTask->GetFilePath());
		
		if (!IsReadOnly(pTask->GetFilePath()))
		{
			m_TaskTree.SetItemImage(hSel, 3, 3);
			ATaskTempl* pNew = new ATaskTempl;
			g_ulNewCount++;
			if (!pNew->LoadFromTextFile(pTask->GetFilePath()))
			{
				AfxMessageBox("读取模板文件失败！", MB_ICONSTOP);
				delete pNew;
				g_ulDelCount++;
				return;
			}
			
			GetTaskTemplMan()->DelOneTaskTempl(pTask);
			GetTaskTemplMan()->AddOneTaskTempl(pNew);
			
			UpdateWhenCheckOut(pNew);
			
			ClearTaskTree(hSel, FALSE);
			m_TaskTree.SetItemData(hSel, (DWORD)pNew);
			ATaskTempl* pChild = pNew->m_pFirstChild;
			while (pChild)
			{
				UpdateTaskTree(hSel, pChild);
				pChild = pChild->m_pNextSibling;
			}
			if (hSel == m_TaskTree.GetSelectedItem())
			{
				UpdateSheet(pNew, FALSE, FALSE);
			}
			
			GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
			GetDlgItem(IDC_CANCEL)->EnableWindow(TRUE);
		}
	}
	
}

extern BOOL RefreshBaseDataSettings();

void RefreshTaskData()
{
	g_VSS.SetProjectPath("BaseData");
	g_VSS.GetFile("BaseData\\TemplID.dat");
	g_VSS.GetFile("BaseData\\TaskID.dat");
	g_VSS.GetFile("BaseData\\TaskNPCInfo.dat");
	g_VSS.SetProjectPath("BaseData\\TaskTemplate");
	g_VSS.GetAllFiles("BaseData\\TaskTemplate");
	//g_VSS.SetProjectPath("TalkData");
	//g_VSS.GetAllFiles("TalkData");
}

void CTaskTemplEditorView::OnRefresh()
{
	RefreshTaskData();
	RefreshBaseDataSettings();
//	UpdateTaskTree();
	InitTaskTree();
	UpdateTemplClassTree();
}

void CTaskTemplEditorView::UpdateSheet(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	BOOL bChanged = FALSE;
	if (m_PropertyPage.UpdatePage(pTask, bUpdateData, bReadOnly)) bChanged = TRUE;
	if (m_PremisePage.UpdatePage(pTask, bUpdateData, bReadOnly)) bChanged = TRUE;
	if (m_MethodPage.UpdatePage(pTask, bUpdateData, bReadOnly)) bChanged = TRUE;
	if (m_AwardPage.UpdatePage(pTask, bUpdateData, bReadOnly)) bChanged = TRUE;
}

void CTaskTemplEditorView::OnSelchangedTaskTree(NMHDR* pNMHDR, LRESULT* pResult)
{

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hOld = pNMTreeView->itemOld.hItem;
	
	if (_bIsClearing)
	{
		return;
	}

	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(pNMTreeView->itemNew.hItem);
	ATaskTempl *pTaskOld = 0;
	if (pNMTreeView->itemOld.hItem)
	{
		pTaskOld = (ATaskTempl*)m_TaskTree.GetItemData(pNMTreeView->itemOld.hItem);
	}
	

	AString strName = m_TaskTree.GetItemText(pNMTreeView->itemNew.hItem);
	if (pTask == NULL)
	{
		pTask = LoadTaskTempl(strName,pNMTreeView->itemNew.hItem);
	}
	
	if (pTask && pTask->m_pParent == NULL && pTaskOld && pTaskOld->m_pParent == NULL && GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		SelectedTempls::iterator iter = m_SelectedTempls.find(pNMTreeView->itemNew.hItem);
		if (iter == m_SelectedTempls.end())
		{
			m_SelectedTempls[pNMTreeView->itemNew.hItem] = pTask;
		}
		else
		{
			m_SelectedTempls.erase(pNMTreeView->itemNew.hItem);
			m_TaskTree.SetItemState(pNMTreeView->itemNew.hItem,0,TVIS_SELECTED);
		}
		//CTreeCtrl把上次选择的item置为未选择状态，现在要置为选择状态
		if (m_SelectedTempls.find(pNMTreeView->itemOld.hItem) != m_SelectedTempls.end())
		{
			m_TaskTree.SetItemState(pNMTreeView->itemOld.hItem,TVIS_SELECTED,TVIS_SELECTED);
		}
	}
	else if (pTask && pTask->m_pParent == NULL && pTaskOld && pTaskOld->m_pParent == NULL && GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		HTREEITEM hFromItem = m_ShiftStartItem;
		HTREEITEM hToItem = pNMTreeView->itemNew.hItem;
		HTREEITEM hItem = m_TaskTree.GetRootItem();
		ATaskTempl* pBetween = 0;

		while ( hItem && hItem!=hFromItem && hItem!=hToItem )
			hItem = m_TaskTree.GetNextSiblingItem( hItem );

		bool bReverse = hItem==hToItem;

		hItem = m_TaskTree.GetRootItem();
		bool bSelect = false;
		
		while (hItem)
		{
			if (hItem == (bReverse ? hToItem : hFromItem))
				bSelect = true;

			SelectedTempls::iterator iter = m_SelectedTempls.find(hItem);
			if (bSelect)
			{
				if(iter == m_SelectedTempls.end())
				{
					pBetween = (ATaskTempl*)m_TaskTree.GetItemData(hItem);		
					
					AString strName = m_TaskTree.GetItemText(hItem);
					if (pBetween == NULL)
					{
						pBetween = LoadTaskTempl(strName,hItem);
					}
					m_SelectedTempls[hItem] = pBetween;
				}
				m_TaskTree.SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
			}
			else
			{
				if(m_TaskTree.GetItemState(hItem,TVIS_SELECTED) & TVIS_SELECTED)
				{
					m_SelectedTempls.erase(hItem);
					m_TaskTree.SetItemState(hItem,0,TVIS_SELECTED);
				}
			}
			
			if (hItem == (bReverse ? hFromItem : hToItem))
				bSelect = false;
			
			hItem = m_TaskTree.GetNextSiblingItem(hItem);
		}

	}
	else
	{
		SelectedTempls::iterator iter = m_SelectedTempls.begin();
		while (iter != m_SelectedTempls.end())
		{
			m_TaskTree.SetItemState(iter->first,0,TVIS_SELECTED);
			++iter;
		}
		m_SelectedTempls.clear();
		m_SelectedTempls[pNMTreeView->itemNew.hItem] = pTask;
		m_ShiftStartItem = pNMTreeView->itemNew.hItem;
	}
	BOOL bReadOnly;
	if (_view_mode) bReadOnly = FALSE; 
	else
	{
		ATaskTempl* pParent = pTask;
		while (pParent->m_pParent) pParent = pParent->m_pParent;
		bReadOnly = IsReadOnly(pParent->GetFilePath());
	}
	UpdateSheet(pTask, false, bReadOnly);
	GetDlgItem(IDC_APPLY)->EnableWindow(!bReadOnly);
	GetDlgItem(IDC_CANCEL)->EnableWindow(!bReadOnly);
	*pResult = 0;
}

bool _is_give_item_task(ATaskTempl* pTask, bool& bChild)
{
	if (pTask->m_ulGivenTskCount)
		return true;

	ATaskTempl* pChild = pTask->m_pFirstChild;

	while (pChild)
	{
		if (_is_give_item_task(pChild, bChild))
		{
			bChild = true;
			return true;
		}

		pChild = pChild->m_pNextSibling;
	}

	return false;
}

bool _has_award(ATaskTempl* pTask, bool bSucc)
{
	if (bSucc && pTask->HasSuccAward()
	 ||!bSucc && pTask->HasFailAward())
		return true;

	ATaskTempl* pChild = pTask->m_pFirstChild;

	while (pChild)
	{
		if (_has_award(pChild, bSucc))
			return true;

		pChild = pChild->m_pNextSibling;
	}

	return false;
}

bool _check_last_step_award(ATaskTempl* pTask)
{
	if (pTask->m_bExeChildInOrder)
	{
		ATaskTempl* pChild = pTask->m_pFirstChild;

		while (pChild)
		{
			if (pChild->m_pNextSibling && _has_award(pChild, true))
				return false;

			pChild = pChild->m_pNextSibling;
		}
	}
	else if (pTask->m_bRandOne || pTask->m_bChooseOne)
	{
		ATaskTempl* pChild = pTask->m_pFirstChild;

		while (pChild)
		{
			if (!_check_last_step_award(pChild))
				return false;

			pChild = pChild->m_pNextSibling;
		}
	}
	else
	{
		ATaskTempl* pChild = pTask->m_pFirstChild;

		while (pChild)
		{
			if (_has_award(pChild, true))
				return false;

			pChild = pChild->m_pNextSibling;
		}
	}

	return true;
}

bool _check_clear_acquired(ATaskTempl* pTempl)
{
	if (!pTempl->m_bClearAcquired)
		return false;

	ATaskTempl* pChild = pTempl->m_pFirstChild;

	while (pChild)
	{
		if (!_check_clear_acquired(pChild))
			return false;

		pChild = pChild->m_pNextSibling;
	}

	return true;
}

bool _is_team_shared(ATaskTempl* pTempl)
{
	if (pTempl->m_bSharedTask)
		return true;

	ATaskTempl* pChild = pTempl->m_pFirstChild;

	while (pChild)
	{
		if (_is_team_shared(pChild))
			return true;

		pChild = pChild->m_pNextSibling;
	}

	return false;
}

bool _check_kill_monster(ATaskTempl* pTempl)
{
	if (pTempl->m_enumMethod == enumTMKillNumMonster)
	{
		for (unsigned long i = 0; i < pTempl->m_ulMonsterWanted; i++)
		{
			MONSTER_WANTED* p = &pTempl->m_MonsterWanted[i];

			if (p->m_bDropCmnItem && p->m_ulDropItemId)
				return false;
		}
	}

	ATaskTempl* pChild = pTempl->m_pFirstChild;
	while (pChild)
	{
		if (!_check_kill_monster(pChild))
			return false;

		pChild = pChild->m_pNextSibling;
	}

	return true;
}

bool _check_cmn_items(ATaskTempl* pTempl)
{
	if (pTempl->m_enumMethod == enumTMCollectNumArticle)
	{
		for (unsigned char i = 0; i < pTempl->m_ulItemsWanted; i++)
			if (pTempl->m_ItemsWanted[i].m_bCommonItem)
				return true;
	}
	else if (pTempl->m_enumMethod == enumTMKillNumMonster)
	{
		for (unsigned long i = 0; i < pTempl->m_ulMonsterWanted; i++)
		{
			const MONSTER_WANTED& mw = pTempl->m_MonsterWanted[i];
			if (!mw.m_ulDropItemId) continue;
			if (mw.m_bDropCmnItem)
				return true;
		}
	}

	ATaskTempl* pChild = pTempl->m_pFirstChild;
	while (pChild)
	{
		if (_check_cmn_items(pChild))
			return true;

		pChild = pChild->m_pNextSibling;
	}

	return false;
}

#include "hashmap.h"

void _check_dup_npc(FILE* fp, FILE* fpTask, bool bDelv, BaseDataTempl& svr)
{
	abase::hash_map<int, int> old;

	for (int j = 0; j < svr.GetItemNum(); j++)
	{
		unsigned long ulTaskId = svr.GetItemValue(j);

		if (ulTaskId)
		{
			TaskTemplMap::iterator it = GetTaskTemplMan()->GetAllTemplMap().find(ulTaskId);

			if (it != GetTaskTemplMan()->GetAllTemplMap().end())
			{
				ATaskTempl* pTempl = it->second;

				if (old.find(ulTaskId) != old.end())
				{
					fprintf(
						fpTask,
						"%s: %s %s\r\n",
						bDelv ? "发放" : "完成",
						svr.GetName(),
						g_TaskIDMan.GetPathByID(pTempl->GetID()));
				}
				else
					old[ulTaskId] = ulTaskId;

				unsigned long npc_id = bDelv ? pTempl->m_ulDelvNPC : pTempl->m_ulAwardNPC;

				if (npc_id)
				{
					unsigned long ulSvrID;
					BaseDataTempl npc;

					if (npc.Load(g_BaseIDMan.GetPathByID(npc_id)) == 0)
					{
						int nItem = npc.GetItemIndex(bDelv ? "发放任务服务" : "验证完成任务服务");
						if (nItem >= 0) ulSvrID = npc.GetItemValue(nItem);
						npc.Release();

						if (ulSvrID != svr.GetID())
						{
							fprintf(
								fp,
								"%s: %s %s\r\n",
								bDelv ? "发放" : "完成",
								svr.GetName(),
								g_TaskIDMan.GetPathByID(pTempl->GetID()));
						}
					}
				}
			}
		}
	}
}

void _check_dup_npc()	
{
	FILE* fp_dup_npc = fopen("task_info_对应多个NPC.txt", "wb");
	FILE* fp_dup_task = fopen("task_info_NPC挂多个相同任务.txt", "wb");

	if (g_BaseIDMan.Load("BaseData\\TemplID.dat") != 0)
		return;

	abase::vector<AString> PathArray;
	g_BaseIDMan.GeneratePathArray(PathArray);

	for (size_t i = 0; i < PathArray.size(); i++)
	{
		BaseDataTempl svr;
		bool bDelv;
		bool bFind = false;

		if (strstr(PathArray[i], "BaseData\\NPC\\功能\\发放任务\\") != 0)
		{
			bFind = true;
			bDelv = true;
		}
		else if (strstr(PathArray[i], "BaseData\\NPC\\功能\\验证完成任务\\") != 0)
		{
			bFind = true;
			bDelv = false;
		}

		if (bFind)
		{
			if (svr.Load(PathArray[i]) == 0)
			{
				_check_dup_npc(fp_dup_npc, fp_dup_task, bDelv, svr);
				svr.Release();
			}
		}
	}

	fclose(fp_dup_npc);
	fclose(fp_dup_task);
	g_BaseIDMan.Release();
}

ATaskTempl* CTaskTemplEditorView::LoadTaskTempl(AString strFn,HTREEITEM hItem)
{
	HTREEITEM hParent = hItem;
	//	HTREEITEM hParent = m_TaskTree.GetParentItem(hItem);
	//	HTREEITEM hRoot = m_TaskTree.GetRootItem();
	
	if (m_TaskTree.GetItemData(hItem))
	{
		return (ATaskTempl*)m_TaskTree.GetItemData(hItem);
	}
	
	while (m_TaskTree.GetParentItem(hParent) != NULL)
	{
		hParent = m_TaskTree.GetParentItem(hParent);
	}
	strFn = m_TaskTree.GetItemText(hParent);
	CString strFile = "BaseData\\TaskTemplate\\" + strFn + ".tkt";
	ATaskTempl* pTask = new ATaskTempl;
	g_ulNewCount++;
	
	if (!pTask->LoadFromTextFile(strFile))
	{
		delete pTask;
		pTask = NULL;
		g_ulDelCount++;
		return NULL;
	}
	
	unsigned long id = g_TaskIDMan.GetIDByPath(strFn);
	if (id != pTask->GetID())
	{
		delete pTask;
		pTask = NULL;
		g_ulDelCount++;
		return NULL;
	}
	
	GetTaskTemplMan()->AddOneTaskTempl(pTask);
	
	SetTaskTreeItemData(hParent, pTask);
	
	if (IsReadOnly(strFile))
		m_TaskTree.SetItemImage(hParent, 2, 2);
	else
		m_TaskTree.SetItemImage(hParent, 3, 3);
	
	return (ATaskTempl*)m_TaskTree.GetItemData(hItem);
}
HTREEITEM CTaskTemplEditorView::SetTaskTreeItemData(HTREEITEM hItem,ATaskTempl* pTask)
{
	
	m_TaskTree.SetItemData(hItem, (DWORD)pTask);

	if (m_TaskTree.GetParentItem(hItem) != NULL) 
	{
		m_TaskTree.SetItemImage(hItem, 4, 4);
	}

	hItem = m_TaskTree.GetChildItem(hItem);
	
	ATaskTempl* pSub = pTask->m_pFirstChild;
	while (pSub)
	{
		SetTaskTreeItemData(hItem, pSub);
		pSub = pSub->m_pNextSibling;
		hItem = m_TaskTree.GetNextSiblingItem(hItem);
	}
	
	return hItem;
}

void CTaskTemplEditorView::InitTaskTree()
{

	ClearTaskTree();

	m_iKeyTaskCount = 0;

	FILE* fp_auto				= fopen("task_info_自动触发.txt", "wb");
	FILE* fp_clear_acquired		= fopen("task_info_不清除所得物品.txt", "wb");
	FILE* fp_given_item			= fopen("task_info_预先发放物品.txt", "wb");
	FILE* fp_succ_redo			= fopen("task_info_成功重复.txt", "wb");
	FILE* fp_team				= fopen("task_info_组队共享.txt", "wb");
	FILE* fp_record				= fopen("task_info_记录的任务数目.txt", "wb");
	FILE* fp_deliver_time		= fopen("task_info_发放时间间隔.txt", "wb");
	FILE* fp_fail_award			= fopen("task_错误_失败奖励.txt", "wb");
	FILE* fp_key				= fopen("task_错误_失败不能重复且能放弃.txt", "wb");
	FILE* fp_need_record		= fopen("task_错误_应该记录但没记录.txt", "wb");
	FILE* fp_noneed_record		= fopen("task_错误_不该记录结果.txt", "wb");
	FILE* fp_award				= fopen("task_错误_非最后一步子任务奖励.txt", "wb");
	FILE* fp_kill_monster		= fopen("task_错误_杀怪掉普通物品.txt", "wb");
	FILE* fp_wrong_id			= fopen("task_错误_id错误.txt", "wb");
	FILE* fp_cmn_items			= fopen("task_错误_收集普通物品但放弃不清空.txt", "wb");

	abase::vector<AString> PathArray;
	g_TaskIDMan.GeneratePathArray(PathArray);
	int record_count = 0;

	for (size_t i = 0; i < PathArray.size(); i++)
	{
		if (PathArray[i].Find('\\') != -1)
			continue;

		CString strFile = "BaseData\\TaskTemplate\\" + PathArray[i] + ".tkt";
		ATaskTempl* pTask = new ATaskTempl;
		g_ulNewCount++;

		if (!pTask->LoadFromTextFile(strFile))
		{
			delete pTask;
			g_ulDelCount++;
			continue;
		}

		if (GetComboBoxFromTaskType(pTask->GetType()) == 1)
		{
			m_iKeyTaskCount++;
		}
		
		unsigned long id = g_TaskIDMan.GetIDByPath(PathArray[i]);
		if (id != pTask->GetID())
		{
			fprintf(fp_wrong_id, "%s: self id = %d, man id = %d", PathArray[i], pTask->GetID(), id);
			delete pTask;
			g_ulDelCount++;
			continue;
		}
		
		GetTaskTemplMan()->AddOneTaskTempl(pTask);

		if (fp_auto && pTask->IsAutoDeliver())
			fprintf(fp_auto, "%s: \t%s\r\n", pTask->CanShowPrompt() ? "显示" : "不显示", strFile);

		if (fp_given_item)
		{
			bool bChild = false;

			if (_is_give_item_task(pTask, bChild))
				fprintf(fp_given_item, "%s\r\n", strFile);
		}

		if (fp_fail_award)
			if (_has_award(pTask, false))
				fprintf(fp_fail_award, "%s\r\n", strFile);

		if (fp_key)
			if (!pTask->m_bCanRedoAfterFailure && pTask->m_bCanGiveUp)
				fprintf(fp_key, "%s\r\n", strFile);

		if (fp_succ_redo)
			if (pTask->m_bCanRedo)
				fprintf(fp_succ_redo, "%s\r\n", strFile);

		if (fp_need_record)
			if ((!pTask->m_bCanRedo || !pTask->m_bCanRedoAfterFailure) && !pTask->m_bNeedRecord)
				fprintf(fp_need_record, "%s\r\n", strFile);

		if (pTask->m_lAvailFrequency != enumTAFNormal)
			fprintf(fp_deliver_time, "%s 成功不重复:%d 失败不重复:%d 记录:%d\r\n", strFile, !pTask->m_bCanRedo, !pTask->m_bCanRedoAfterFailure, pTask->m_bNeedRecord);

		if (fp_noneed_record)
			if (pTask->m_bCanRedo && pTask->m_bCanRedoAfterFailure && pTask->m_bNeedRecord)
				fprintf(fp_noneed_record, "%s\r\n", strFile);

		if (fp_award)
			if (pTask->m_bCanGiveUp && pTask->CanRedo() && !_check_last_step_award(pTask))
				fprintf(fp_award, "%s\r\n", strFile);

		if (fp_clear_acquired && !_check_clear_acquired(pTask))
			fprintf(fp_clear_acquired, "放弃清空:%d %s\r\n", pTask->m_bClearAsGiveUp, strFile);

		if (_is_team_shared(pTask))
			fprintf(fp_team, "%s\r\n", strFile);

		if (pTask->m_bNeedRecord)
			record_count++;

		if (!_check_kill_monster(pTask))
			fprintf(fp_kill_monster, "%s\r\n", strFile);

		if (!pTask->m_bClearAsGiveUp && _check_cmn_items(pTask))
			fprintf(fp_cmn_items, "%s\r\n", strFile);

		HTREEITEM hItem = InitTaskTree(TVI_ROOT, pTask);

		ReleaseTreeTopMemory(hItem);
		if (IsReadOnly(strFile))
			m_TaskTree.SetItemImage(hItem, 2, 2);
		else
			m_TaskTree.SetItemImage(hItem, 3, 3);
	}

	if (fp_auto) fclose(fp_auto);
	if (fp_given_item) fclose(fp_given_item);
	if (fp_fail_award) fclose(fp_fail_award);
	if (fp_key) fclose(fp_key);
	if (fp_succ_redo) fclose(fp_succ_redo);
	if (fp_need_record) fclose(fp_need_record);
	if (fp_award) fclose(fp_award);
	if (fp_clear_acquired) fclose(fp_clear_acquired);
	if (fp_noneed_record) fclose(fp_noneed_record);
	if (fp_team) fclose(fp_team);
	if (fp_record)
	{
		fprintf(fp_record, "count = %d", record_count);
		fclose(fp_record);
	}

	if (fp_kill_monster) fclose(fp_kill_monster);
	if (fp_wrong_id) fclose(fp_wrong_id);
	fclose(fp_cmn_items);

	ClearTreeData();
	//GetTaskTemplMan()->Release();
	GetTaskTemplMan()->GetAllTemplMap().clear();
	GetTaskTemplMan()->GetTopTemplMap().clear();

	g_DataReady = true;

	HTREEITEM hFirst = m_TaskTree.GetChildItem(TVI_ROOT);
	if (hFirst) m_TaskTree.Select(hFirst, TVGN_CARET);
}
HTREEITEM CTaskTemplEditorView::InitTaskTree(HTREEITEM hParent, ATaskTempl* pTask)
{
	HTREEITEM hItem;
	
	bool bIsTopItem = false;
	if (hParent == TVI_ROOT)
	{
		hItem = m_TaskTree.InsertItem(g_TaskIDMan.GetPathByID(pTask->m_ID), hParent, TVI_SORT);
		bIsTopItem = true;
	}
	else
		hItem = m_TaskTree.InsertItem(g_TaskIDMan.GetPathByID(pTask->m_ID), hParent);

	TreeData sData(hItem,pTask->GetName(),pTask->m_DynTaskType);
	
	GetTaskTemplMan()->AddOneTreeItem(pTask,sData,bIsTopItem);
	
	m_TaskTree.SetItemData(hItem, (DWORD)pTask);

	if (hParent != TVI_ROOT) m_TaskTree.SetItemImage(hItem, 4, 4);
	
	TaskNPCInfoMap& npc_map = GetTaskTemplMan()->GetNPCInfoMap();
	
	if (pTask->m_ulDelvNPC)
	{
		TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulDelvNPC);
		
		if (it == npc_map.end())
		{
			NPC_INFO info = { pTask->m_ulDelvNPC, 0, 0, 0};
			npc_map[pTask->m_ulDelvNPC] = info;
		}
	}
	
	if (pTask->m_ulAwardNPC)
	{
		TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulAwardNPC);
		
		if (it == npc_map.end())
		{
			NPC_INFO info = { pTask->m_ulAwardNPC, 0, 0, 0};
			npc_map[pTask->m_ulAwardNPC] = info;
		}
	}
	
	ATaskTempl* pSub = pTask->m_pFirstChild;
	while (pSub)
	{
		InitTaskTree(hItem, pSub);
		pSub = pSub->m_pNextSibling;
	}

	return hItem;
}
int  LoadTaskFunc()
{
	//先Sleep避免主线程卡
	Sleep(10);
	
	task_load_total = 0;

	int iCount = 0;

	FILE* fp_auto				= fopen("task_info_自动触发.txt", "wb");
	FILE* fp_clear_acquired		= fopen("task_info_不清除所得物品.txt", "wb");
	FILE* fp_given_item			= fopen("task_info_预先发放物品.txt", "wb");
	FILE* fp_succ_redo			= fopen("task_info_成功重复.txt", "wb");
	FILE* fp_team				= fopen("task_info_组队共享.txt", "wb");
	FILE* fp_deliver_time		= fopen("task_info_发放时间间隔.txt", "wb");
	FILE* fp_record		        = fopen("task_info_完成记录结果.txt", "wb");
	FILE* fp_record_time_list   = fopen("task_info_发放时间和完成计数.txt", "wb");
	FILE* fp_fail_award			= fopen("task_错误_失败奖励.txt", "wb");
	FILE* fp_key				= fopen("task_错误_失败不能重复且能放弃.txt", "wb");
	FILE* fp_need_record		= fopen("task_错误_应该记录但没记录.txt", "wb");
	FILE* fp_noneed_record		= fopen("task_错误_不该记录结果.txt", "wb");
	FILE* fp_award				= fopen("task_错误_非最后一步子任务奖励.txt", "wb");
	FILE* fp_wrong_id			= fopen("task_错误_id错误.txt", "wb");
	FILE* fp_cmn_items			= fopen("task_错误_收集普通物品但放弃不清空.txt", "wb");

	TaskCacheMap::iterator it = g_TaskCacheMap.begin();

	FILE* pErrFile = fopen("task_load_err.txt","wb");
	int   iLoadSucc = 0;

	if(pErrFile)
	{
		fprintf(pErrFile,"加载失败的任务列表：");
		fprintf(pErrFile,"\r\n");
		fflush(pErrFile);
	}

	abase::vector<AString> PathArray;
	g_TaskIDMan.GeneratePathArray(PathArray);

	int record_count = 0;
	int record_time_list_count = 0;
	int num_thread_load = 0;
	int load_index = 0;

	for (;it!=g_TaskCacheMap.end() && g_Running;++it)
	{
		iCount++;

		TASK_CACHE* pCahce = &it->second;

		if (it->second.Mask != 0) continue;

		if (it->second.Path.Find('\\') != -1) 
			continue;

		CString strFile = it->second.File;
		ATaskTempl* pTask = it->second.Task;
		
		load_index++;

	//	int idTask = it->second.TaskID;

		if (!LoadItemTask(strFile,it->second.Item))
		{
			if(pErrFile)
			{
				fprintf(pErrFile,strFile.GetBuffer(0));
				fprintf(pErrFile,"\r\n");
				fflush(pErrFile);
			}

			g_pView->OnLoadError(it->second.Item);
		
			it->second.Delete = true;

			delete pTask;
			g_ulDelCount++;
			continue;
		}
		
		num_thread_load++;

	//	unsigned long id = g_TaskIDMan.GetIDByPath(it->second.Path);
		unsigned long id = pTask->m_ID;

		if (id != (unsigned long)it->second.TaskID)
		{
			g_pView->OnLoadError(it->second.Item);
		
			it->second.Delete = true;

			fprintf(fp_wrong_id, "%s: self id = %d, man id = %d", it->second.Path, pTask->GetID(), (unsigned long)it->second.TaskID);
			delete pTask;
			g_ulDelCount++;
			continue;
		}		

		g_pView->OnLoadSucc(it->second.Item);


		it->second.Delete = true;
		iLoadSucc++;

		GetTaskTemplMan()->AddOneTaskTempl(pTask);
		
		if (fp_auto && pTask->IsAutoDeliver())
			fprintf(fp_auto, "%s: \t%s\r\n", pTask->CanShowPrompt() ? "显示" : "不显示", strFile);

		if (fp_given_item)
		{
			bool bChild = false;

			if (_is_give_item_task(pTask, bChild))
				fprintf(fp_given_item, "%s\r\n", strFile);
		}

		if (fp_fail_award)
			if (_has_award(pTask, false))
				fprintf(fp_fail_award, "%s\r\n", strFile);

		if (fp_key)
			if (!pTask->m_bCanRedoAfterFailure && pTask->m_bCanGiveUp)
				fprintf(fp_key, "%s\r\n", strFile);

		if (fp_succ_redo)
			if (pTask->m_bCanRedo)
				fprintf(fp_succ_redo, "%s\r\n", strFile);

		if (fp_need_record)
			if ((!pTask->m_bCanRedo || !pTask->m_bCanRedoAfterFailure) && !pTask->m_bNeedRecord)
				fprintf(fp_need_record, "%s\r\n", strFile);

		if (fp_noneed_record)
			if (pTask->m_bCanRedo && pTask->m_bCanRedoAfterFailure && pTask->m_bNeedRecord)
				fprintf(fp_noneed_record, "%s\r\n", strFile);

		if (pTask->m_lAvailFrequency != enumTAFNormal)
		{
			fprintf(fp_deliver_time, "%s 成功不重复:%d 失败不重复:%d 记录:%d\r\n", strFile, !pTask->m_bCanRedo, !pTask->m_bCanRedoAfterFailure, pTask->m_bNeedRecord);
		}		

		if (fp_award)
			if (pTask->m_bCanGiveUp && pTask->CanRedo() && !_check_last_step_award(pTask))
				fprintf(fp_award, "%s\r\n", strFile);

		if (fp_clear_acquired && !_check_clear_acquired(pTask))
			fprintf(fp_clear_acquired, "放弃清空:%d %s\r\n", pTask->m_bClearAsGiveUp, strFile);

		if (_is_team_shared(pTask))
			fprintf(fp_team, "%s\r\n", strFile);

		if (pTask->m_bNeedRecord)
		{
			if (pTask->m_pParent)
				continue;
			fprintf(fp_record, "%s\r\n", strFile);
			record_count++;
		}

		if (pTask->m_lAvailFrequency != enumTAFNormal || pTask->m_ulTimetable /*|| pTask->m_bRecFinishCount*/)
		{
			if (pTask->m_pParent)
				continue;
			fprintf(fp_record_time_list, "%s\r\n", strFile);
			record_time_list_count++;
		}

		if (!pTask->m_bClearAsGiveUp && _check_cmn_items(pTask))
			fprintf(fp_cmn_items, "%s\r\n", strFile);

		if (iCount == 10)
		{
	//		Sleep(1);

			iCount = 0;
		}
		
	}

	if(pErrFile)
	{
		char cSucc[1024] = {0};
		sprintf(cSucc,"加载成功的任务数目: %d",iLoadSucc);
		fprintf(pErrFile,cSucc);
		fprintf(pErrFile,"\r\n");

		sprintf(cSucc,"总任务数目: %d",g_TaskCacheMap.size());
		fprintf(pErrFile,cSucc);
		fprintf(pErrFile,"\r\n");

		fflush(pErrFile);

		fclose(pErrFile);
		ShellExecute(0,"open","task_load_err.txt",0,0,SW_SHOWNORMAL);
	}

	if (fp_auto) fclose(fp_auto);
	if (fp_given_item) fclose(fp_given_item);
	if (fp_fail_award) fclose(fp_fail_award);
	if (fp_key) fclose(fp_key);
	if (fp_succ_redo) fclose(fp_succ_redo);
	if (fp_need_record) fclose(fp_need_record);
	if (fp_award) fclose(fp_award);
	if (fp_clear_acquired) fclose(fp_clear_acquired);
	if (fp_noneed_record) fclose(fp_noneed_record);
	if (fp_team) fclose(fp_team);
	if (fp_record)
	{
		fprintf(fp_record, "完成记录结果的任务个数 = %d, 上限为:%d", record_count, TASK_FINISHED_LIST_MAX_LEN);
		fclose(fp_record);
	}
	if (fp_record_time_list)
	{
		fprintf(fp_record_time_list, "发放时间和完成计数的任务个数 = %d", record_time_list_count);
		fclose(fp_record_time_list);
	}

	fclose(fp_deliver_time);

	if (fp_wrong_id) fclose(fp_wrong_id);
	fclose(fp_cmn_items);	

	g_DataReady = true;

	return 0;
}


void CTaskTemplEditorView::OnLoadSucc (HTREEITEM hItem)
{
	if (g_TaskCacheMap.find(hItem) == g_TaskCacheMap.end() || !hItem)
		return;
	
	ATaskTempl* pTask = g_TaskCacheMap[hItem].Task;
	
	TaskNPCInfoMap& npc_map = GetTaskTemplMan()->GetNPCInfoMap();
	
	if (pTask->m_ulDelvNPC)
	{
		TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulDelvNPC);
		
		if (it == npc_map.end())
		{
			NPC_INFO info = { pTask->m_ulDelvNPC, 0, 0, 0};
			npc_map[pTask->m_ulDelvNPC] = info;
		}
	}
	
	if (pTask->m_ulAwardNPC)
	{
		TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulAwardNPC);
		
		if (it == npc_map.end())
		{
			NPC_INFO info = { pTask->m_ulAwardNPC, 0, 0, 0};
			npc_map[pTask->m_ulAwardNPC] = info;
		}
	}
	
	ATaskTempl* pSub = pTask->m_pFirstChild;
	while (pSub)
	{
		UpdateTaskTree(hItem, pSub);
		pSub = pSub->m_pNextSibling;
	}
}

void CTaskTemplEditorView::OnLoadError(HTREEITEM hItem)
{
	if (g_TaskCacheMap.find(hItem) == g_TaskCacheMap.end() || !hItem)
		return;
	
	m_TaskTree.DeleteItem(hItem);
}

void CTaskTemplEditorView::UpdateTaskTree()
{
	ClearTaskTree();
	
/*	GroupItemMap::iterator it = g_GroupItem.begin();
	for (;it!=g_GroupItem.end();++it)
	{
		m_TaskTree.DeleteItem(it->second);
	}
	g_GroupItem.clear();
*/
	g_DataReady = false;

	g_TaskCacheMap.clear();
	
	if (g_LoadThread)
	{
		CloseHandle(g_LoadThread);
		g_LoadThread = 0;
	}

	abase::vector<AString> PathArray;
	g_TaskIDMan.GeneratePathArray(PathArray);
	int record_count = 0;
	int record_time_list_count = 0;
	
	g_ulDelCount = 0;
	g_ulNewCount = 0;

	int task_num = 0;
	
	for (size_t i = 0; i < PathArray.size(); i++)
	{
		if (PathArray[i].Find('\\') != -1)
			continue;
		
		CString strFile = "BaseData\\TaskTemplate\\" + PathArray[i] + ".tkt";
		ATaskTempl* pTask = 0;
		HTREEITEM hItem = 0;
		
		g_ulNewCount++;

		if (_access(strFile.GetBuffer(0),0) != -1)
		{
			pTask = new ATaskTempl;
			pTask->m_ID = g_TaskIDMan.GetIDByPath(PathArray[i]);			
			
			hItem = UpdateTaskTree(TVI_ROOT, pTask,false);

			TASK_CACHE cache;
			cache.File = strFile;
			cache.Path = PathArray[i];
			cache.Item = hItem;
			cache.Mask = 0;
			cache.Task = pTask;
			cache.TaskID = pTask->m_ID;
			cache.Mutex= false;
			cache.Delete = false;
			cache.Index = task_num++;			

			if(!hItem)
			{
				OutputDebugString("UpdateTaskTree Error Item......\r\n");
			}

			g_TaskCacheMap[hItem] = cache;

			CString sMsg;
			sMsg.Format(_T("Cache TaskTemplate(%d - %d):  %s\r\n"),cache.Index,task_num,strFile);
			OutputDebugString(sMsg);

			
			if (IsReadOnly(strFile))
				m_TaskTree.SetItemImage(hItem, 2, 2);
			else
				m_TaskTree.SetItemImage(hItem, 3, 3);
		}
		else
		{
			g_ulDelCount++;

			continue;
		}
	}

	DWORD dwThreadID = 0;

	//LoadTaskFunc();

	g_LoadThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)LoadTaskFunc,0,0,&dwThreadID);
}

void CTaskTemplEditorView::UpdateTaskTreeFromPack()
{
	TaskTemplMap& tasks = GetTaskTemplMan()->GetTopTemplMap();
	TaskTemplMap::iterator it = tasks.begin();

	for (; it != tasks.end(); ++it)
	{
		ATaskTempl* pTempl = it->second;
		pTempl->SetFilePath(AString("BaseData\\TaskTemplate\\") + g_TaskIDMan.GetPathByID(pTempl->GetID()) + ".tkt");
		HTREEITEM hItem = UpdateTaskTree(TVI_ROOT, pTempl);

		if (IsReadOnly(pTempl->GetFilePath()))
			m_TaskTree.SetItemImage(hItem, 2, 2);
		else
			m_TaskTree.SetItemImage(hItem, 3, 3);
	}

	HTREEITEM hFirst = m_TaskTree.GetChildItem(TVI_ROOT);
	if (hFirst) m_TaskTree.Select(hFirst, TVGN_CARET);
}

HTREEITEM CTaskTemplEditorView::UpdateTaskTree(HTREEITEM hParent, ATaskTempl* pTask,bool bCalcTask)
{
	HTREEITEM hItem;

	if (hParent == TVI_ROOT)
		hItem = m_TaskTree.InsertItem(g_TaskIDMan.GetPathByID(pTask->m_ID), hParent, TVI_SORT);
	else
		hItem = m_TaskTree.InsertItem(g_TaskIDMan.GetPathByID(pTask->m_ID), hParent);

	m_TaskTree.SetItemData(hItem, (DWORD)pTask);
	if (hParent != TVI_ROOT) m_TaskTree.SetItemImage(hItem, 4, 4);

	if (bCalcTask)
	{
		TaskNPCInfoMap& npc_map = GetTaskTemplMan()->GetNPCInfoMap();
		
		if (pTask->m_ulDelvNPC)
		{
			TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulDelvNPC);
			
			if (it == npc_map.end())
			{
				NPC_INFO info = { pTask->m_ulDelvNPC, 0, 0, 0};
				npc_map[pTask->m_ulDelvNPC] = info;
			}
		}
		
		if (pTask->m_ulAwardNPC)
		{
			TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulAwardNPC);
			
			if (it == npc_map.end())
			{
				NPC_INFO info = { pTask->m_ulAwardNPC, 0, 0, 0};
				npc_map[pTask->m_ulAwardNPC] = info;
			}
		}
		
		ATaskTempl* pSub = pTask->m_pFirstChild;
		while (pSub)
		{
			UpdateTaskTree(hItem, pSub);
			pSub = pSub->m_pNextSibling;
		}
	}

	return hItem;
}

void CTaskTemplEditorView::UpdateTemplClassTree()
{
	m_TemplClassTree.DeleteAllItems();

	for (size_t i = 0; i < _task_class_array.size(); i++)
	{
		const CTaskTemplClass* pClass = _task_class_array[i];
		HTREEITEM hClass = m_TemplClassTree.InsertItem(pClass->GetClassName());
		m_TemplClassTree.SetItemData(hClass, reinterpret_cast<DWORD>(pClass));

		CString strFolder;
		strFolder = "BaseData\\TaskTemplate\\";
		strFolder += pClass->GetClassName();
		strFolder += "\\";

		CFileFind finder;
		BOOL bWorking = finder.FindFile(strFolder + "*.tkt");

		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if (finder.IsDots() || finder.IsDirectory()) continue;
			else
			{
				CString str = finder.GetFileName();
				CString strKey = pClass->GetClassName() + ("\\" + str.Left(str.GetLength() - 4));

				if (g_TaskIDMan.GetIDByPath(LPCTSTR(strKey)) == 0)
					continue;

				str = strFolder + str;
				ATaskTempl* pTask = new ATaskTempl;
				g_ulNewCount++;

				if (!pTask->LoadFromTextFile(str))
				{
					delete pTask;
					g_ulDelCount++;
					continue;
				}

	//			assert(pTask->m_enumFinishType == enumTFTNPC);

				assert(pTask->m_DelvTaskTalk.num_window == 1);
				talk_proc::window* pWin = pTask->m_DelvTaskTalk.windows;
				assert(pWin->num_option == 2);
				assert(pWin->options[0].id == NPC_GIVE_TASK);
				assert(pWin->options[0].param == pTask->m_ID);
				assert(pWin->options[1].id == TALK_RETURN);
				assert(pWin->options[1].param == 0);

				assert(pTask->m_ExeTalk.num_window == 1);
				pWin = pTask->m_ExeTalk.windows;
				assert(pWin->num_option	== 1);
				assert(pWin->options[0].id == TALK_RETURN);
				assert(pWin->options[0].param == 0);

				assert(pTask->m_AwardTalk.num_window == 1);
				pWin = pTask->m_AwardTalk.windows;
				assert(pWin->num_option	== 1);
				assert(pWin->options[0].id == NPC_COMPLETE_TASK);
				assert(pWin->options[0].param == pTask->GetID());

				pTask->m_bDismAsSelfFail = false;
				pTask->m_bCapFail = false;
				pTask->m_bSharedAchieved = false;
				pTask->m_bSharedTask = false;
				pTask->m_bCheckTeammate = false;

				GetTaskTemplMan()->AddOneTaskTempl(pTask);
				HTREEITEM hItem = m_TemplClassTree.InsertItem(::PathFindFileName(str), hClass, TVI_SORT);
				m_TemplClassTree.SetItemData(hItem, reinterpret_cast<DWORD>(pTask));

				TreeData sData(hItem,pTask->GetName(),pTask->m_DynTaskType, true);
				GetTaskTemplMan()->AddOneTreeItem(pTask,sData,true);

				if (IsReadOnly(str))
					m_TaskTree.SetItemImage(hItem, 2, 2);
				else
					m_TaskTree.SetItemImage(hItem, 3, 3);

				TaskNPCInfoMap& npc_map = GetTaskTemplMan()->GetNPCInfoMap();

				if (pTask->m_ulDelvNPC)
				{
					TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulDelvNPC);

					if (it == npc_map.end())
					{
						NPC_INFO info = { pTask->m_ulDelvNPC, 0, 0, 0};
						npc_map[pTask->m_ulDelvNPC] = info;
					}
				}

				if (pTask->m_ulAwardNPC)
				{
					TaskNPCInfoMap::iterator it = npc_map.find(pTask->m_ulAwardNPC);

					if (it == npc_map.end())
					{
						NPC_INFO info = { pTask->m_ulAwardNPC, 0, 0, 0};
						npc_map[pTask->m_ulAwardNPC] = info;
					}
				}
			}
		}
	}
}
void CTaskTemplEditorView::ClearTreeData()
{
	TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().begin();
	while (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
	{
		m_TaskTree.SetItemData((iter->second).m_hItem,0);
		++iter;
	}
}

void CTaskTemplEditorView::ReleaseTreeTopMemory(HTREEITEM hItem)
{
	TaskTemplMap& MapTask = GetTaskTemplMan()->GetTopTemplMap();
	
	ATaskTempl *pTask = (ATaskTempl*)m_TaskTree.GetItemData(hItem);
	if (MapTask.find(pTask->GetID()) != MapTask.end())
	{
		MapTask[pTask->GetID()] = 0;
		if (pTask)
		{
			delete pTask;
			g_ulDelCount++;
		}
	}
}

void CTaskTemplEditorView::ClearTaskTree()
{
	HTREEITEM hNext;
	HTREEITEM hChild = m_TaskTree.GetChildItem(TVI_ROOT);
	_bIsClearing = true;
	while (hChild)
	{
		hNext = m_TaskTree.GetNextItem(hChild, TVGN_NEXT);
		ClearTaskTree(hChild);
		hChild = hNext;
	}
	
	GetTaskTemplMan()->Release();
	_bIsClearing = false;
}

void CTaskTemplEditorView::ClearTaskTree(HTREEITEM hItem, BOOL bClearSelf)
{
	HTREEITEM hNext;
	HTREEITEM hChild = m_TaskTree.GetChildItem(hItem);
	
	while (hChild)
	{
		hNext = m_TaskTree.GetNextItem(hChild, TVGN_NEXT);
		m_TaskTree.SetItemData(hChild, 0);
		m_TaskTree.DeleteItem(hChild);
		hChild = hNext;
	}
	
	if (bClearSelf)
	{
		m_TaskTree.SetItemData(hItem, 0);
		m_TaskTree.DeleteItem(hItem);
	}
}

void CTaskTemplEditorView::OnApply()
{
	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (hSel == 0) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	UpdateSheet(pTask, true);
	OnSaveTask();

	TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().find(pTask->GetID());
	if (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
		(iter->second).SetName(pTask->GetName());

}

void CTaskTemplEditorView::OnCancel()
{
	HTREEITEM hSel = m_TaskTree.GetSelectedItem();
	if (hSel == 0) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hSel);
	UpdateSheet(pTask, false);
}

ATaskTempl* CTaskTemplEditorView::CreateNewTask(BOOL bTop)
{
#ifdef _TASK_CLIENT

	CNewTaskDlg dlg;

	if (dlg.DoModal() == IDOK && !dlg.m_strName.IsEmpty())
	{
		AString strFile = AString("BaseData\\TaskTemplate\\" + dlg.m_strName + ".tkt");
		unsigned long ulID = g_TaskIDMan.CreateID(AString(dlg.m_strName));

		if (ulID == 0)
		{
			AfxMessageBox(_T("创建新任务ID失败！，请检查任务名是否重复"), MB_ICONSTOP);
			return NULL;
		}

		ATaskTempl* pTask = new ATaskTempl;
		g_ulNewCount++;
		pTask->Init();

		if (dlg.m_ulSrc)
		{
			ATaskTempl* pSrc = GetTaskTemplMan()->GetTaskTemplByID(dlg.m_ulSrc);
			if (pSrc) 
				*pTask = *pSrc;
			else
			{
				CString strPath = g_TaskIDMan.GetPathByID(dlg.m_ulSrc);
				strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";
				
				ATaskTempl* pSrc = new ATaskTempl;
				g_ulNewCount++;
				
				if(!pSrc->LoadFromTextFile(strPath))
				{
					AfxMessageBox("找不到此任务，请输入正确的任务ID。");
					delete pTask;
					g_ulDelCount++;
					return 0;
				}
				
				*pTask = *pSrc;
				delete pSrc;
				g_ulDelCount++;
			}
		}
		
		pTask->SetName(CSafeString(dlg.m_strName));
		pTask->m_ID = ulID;

		if (bTop)
		{
			pTask->SetFilePath(strFile);

			if (!pTask->SaveToTextFile(strFile))
			{
				AfxMessageBox(_T("保存新任务失败！"), MB_ICONSTOP);
				delete pTask;
				g_ulDelCount++;
				return NULL;
			}

			GetTaskTemplMan()->AddOneTaskTempl(pTask);
		}

		return pTask;
	}

#endif

	return NULL;
}

void CTaskTemplEditorView::OnNewTask()
{
#ifdef _TASK_CLIENT
	ATaskTempl* pNew = CreateNewTask(TRUE);
	if (!pNew)
		return;

	HTREEITEM hItem = m_TaskTree.InsertItem(CSafeString(pNew->GetName()), TVI_ROOT, TVI_SORT);
	m_TaskTree.SetItemData(hItem, (DWORD)pNew);
	m_TaskTree.Select(hItem, TVGN_CARET);

	TreeData sData(hItem,pNew->GetName(),pNew->m_DynTaskType);
	GetTaskTemplMan()->AddOneTreeItem(pNew,sData,true);

	g_VSS.SetProjectPath("BaseData\\TaskTemplate");
	g_VSS.AddFile(pNew->GetFilePath());
	g_VSS.SetProjectPath("BaseData\\TaskTemplate");
	g_VSS.CheckOutFile(pNew->GetFilePath());

	if (IsReadOnly(pNew->GetFilePath()))
		m_TaskTree.SetItemImage(hItem, 2, 2);
	else
		m_TaskTree.SetItemImage(hItem, 3, 3);
#endif
}

extern void _release_templ_tree();
void CTaskTemplEditorView::OnDestroy() 
{
	if (g_LoadThread)
	{
		g_Running = false;
		
		Sleep(3000);
	}
	
	TaskCacheMap::iterator it = g_TaskCacheMap.begin();
	for (;it!=g_TaskCacheMap.end();++it)
	{
		if(it->second.Delete == false)
		{
			delete it->second.Task;
		}		
	}
	g_TaskCacheMap.clear();

	ClearTaskTree();
	ReleaseTaskClassArray();
	_release_templ_tree();
	CFormView::OnDestroy();
}

void CTaskTemplEditorView::OnAppAbout() 
{
}

void CTaskTemplEditorView::OnExport()
{
	WaitDataReady();

#ifdef _TASK_CLIENT

	if (m_iKeyTaskCount < KEY_TASK_SUM)
	{
		AfxMessageBox("加载的模板中主线任务不全，导出失败");
		return;
	}

	CFileDialog dlg(
		FALSE,
		NULL,
		"tasks.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlg.DoModal() == IDOK)
	{
		if (!GetTaskTemplMan()->SaveTasksToPack2(dlg.GetPathName()))
			AfxMessageBox("输出tasks失败！");
	}

	CFileDialog dlgDyn(
		FALSE,
		NULL,
		"dyn_tasks.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlgDyn.DoModal() == IDOK)
	{
		if (!GetTaskTemplMan()->SaveDynTasksToPack2(dlgDyn.GetPathName()))
			AfxMessageBox("输出dyn_task失败！");
	}
#endif
}

void CTaskTemplEditorView::OnView() 
{
	_view_mode = !_view_mode;
}

void CTaskTemplEditorView::OnUpdateView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(_view_mode ? 1 : 0);
}

CString _task_to_find;
BOOL _task_by_name = FALSE;

void CTaskTemplEditorView::OnFindTask()
{
	CFindTaskDlg dlg;
	if (dlg.DoModal() != IDOK) return;

	_task_to_find = dlg.m_strTask;
	_task_by_name = FALSE;
	FindTask(TVI_ROOT, dlg.m_strTask, false);

	if (_class_mode)
		m_TemplClassTree.SetFocus();
	else
		m_TaskTree.SetFocus();
}

void CTaskTemplEditorView::OnFindName2()
{
	if (_recover_mode)
	{
		OnFindName();
		return;
	}
	CFindTaskDlg dlg;
	if (dlg.DoModal() != IDOK) return;
	
	_task_to_find = dlg.m_strTask;
	_task_by_name = TRUE;
	
	FindTask2(TVI_ROOT,_task_to_find);
}

HTREEITEM CTaskTemplEditorView::FindTask2(HTREEITEM hParent, const CString& strTask)
{
	TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().begin();
	if (hParent != TVI_ROOT)
	{
		while (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
		{
			if ((iter->second).m_hItem == hParent)
			{
				++iter;
				break;
			}
			++iter;
		}
	}
	while (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
	{
		CString strName = CSafeString((iter->second).GetName()).GetAnsi();
		
		if (strName.Find(_task_to_find) != -1)
		{
			bool bDynTask = (iter->second).m_bIsClassType;
			CTreeCtrl* pTree = bDynTask ? &m_TemplClassTree : &m_TaskTree;

			pTree->Select((iter->second).m_hItem, TVGN_CARET);
			pTree->Expand((iter->second).m_hItem, TVE_EXPAND);
			
			if (bDynTask != (_class_mode == TRUE))
				OnClassMode();
			if (_class_mode)
				m_TemplClassTree.SetFocus();
			else
				m_TaskTree.SetFocus();
			return (iter->second).m_hItem;
		}
		++iter;
	}
	return NULL;
}
void CTaskTemplEditorView::OnFindById2()
{
	if (_recover_mode)
	{
		OnFindById();
		return;
	}
	CFindTaskDlg dlg;
	if (dlg.DoModal() != IDOK) return;
	
	_task_by_name = false;
	
	unsigned long ulID = atoi(dlg.m_strTask);
	
	TreeItemMap::iterator iter = GetTaskTemplMan()->GetAllTreeItemMap().find(ulID);
	
	if (iter != GetTaskTemplMan()->GetAllTreeItemMap().end())
	{
		bool bDynTask = (iter->second).m_bIsClassType;
		CTreeCtrl* pTree = bDynTask ? &m_TemplClassTree : &m_TaskTree;

		pTree->Select((iter->second).m_hItem,TVGN_CARET);
		pTree->Expand((iter->second).m_hItem,TVE_EXPAND);
	
		if (bDynTask != (_class_mode == TRUE))
			OnClassMode();
	
		if (_class_mode)
			m_TemplClassTree.SetFocus();
		else
			m_TaskTree.SetFocus();
	
		return;
	}
}

void CTaskTemplEditorView::OnFindNext2()
{
	if (_recover_mode)
	{
		OnFindNext();
		return;
	}
	if (_task_by_name)
	{
		CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;
		HTREEITEM hPrev = pTree->GetSelectedItem();
		if (hPrev == NULL)
		{
			FindTask2(TVI_ROOT,_task_to_find);
		}
		else
		{
			if (FindTask2(hPrev,_task_to_find))
			{
				return;
			}
			else
				FindTask2(TVI_ROOT,_task_to_find);
		}
	}		
}

void CTaskTemplEditorView::OnFindName() 
{
	CFindTaskDlg dlg;
	if (dlg.DoModal() != IDOK) return;

	_task_to_find = dlg.m_strTask;
	_task_by_name = TRUE;
	FindTask(TVI_ROOT, dlg.m_strTask, true);

	if (_class_mode)
		m_TemplClassTree.SetFocus();
	else
		m_TaskTree.SetFocus();
}

void CTaskTemplEditorView::OnFindById() 
{
	CFindTaskDlg dlg;
	if (dlg.DoModal() != IDOK) return;

	FindTaskByID(TVI_ROOT, atoi(dlg.m_strTask));

	if (_class_mode)
		m_TemplClassTree.SetFocus();
	else
		m_TaskTree.SetFocus();
}

bool CTaskTemplEditorView::CompareTask(HTREEITEM h, const CString& strTask, bool bByName)
{
	CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;

	if (bByName)
	{
		ATaskTempl* pTempl = reinterpret_cast<ATaskTempl*>(pTree->GetItemData(h));

		if (pTempl)
		{
			CString strName = CSafeString(pTempl->GetName()).GetAnsi();

			if (strName.Find(strTask) != -1)
			{
				pTree->Select(h, TVGN_CARET);
				pTree->Expand(h, TVE_EXPAND);

				return true;
			}
		}
	}
	else if (pTree->GetItemText(h).Find(strTask) != -1)
	{
		pTree->Select(h, TVGN_CARET);
		pTree->Expand(h, TVE_EXPAND);
		return true;
	}

	return false;
}

HTREEITEM CTaskTemplEditorView::FindTaskByID(HTREEITEM hParent, unsigned long ulID)
{
	CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;
	HTREEITEM hChild = pTree->GetChildItem(hParent);

	while (hChild)
	{
		ATaskTempl* pTempl = reinterpret_cast<ATaskTempl*>(pTree->GetItemData(hChild));

		if (pTempl->GetID() == ulID)
		{
			pTree->Select(hChild, TVGN_CARET);
			pTree->Expand(hChild, TVE_EXPAND);
			return hChild;
		}

		HTREEITEM h = FindTaskByID(hChild, ulID);
		if (h) return h;

		hChild = pTree->GetNextSiblingItem(hChild);
	}

	return NULL;
}

HTREEITEM CTaskTemplEditorView::FindTask(HTREEITEM hParent, const CString& strTask, bool bByName)
{
	CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;
	HTREEITEM hChild = pTree->GetChildItem(hParent);

	while (hChild)
	{
		if (CompareTask(hChild, strTask, bByName))
			return hChild;

		HTREEITEM h = FindTask(hChild, strTask, bByName);
		if (h) return h;
		hChild = pTree->GetNextSiblingItem(hChild);
	}

	return NULL;
}

void CTaskTemplEditorView::OnFindNext()
{
	if (_task_to_find.IsEmpty())
		return;

	CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;
	HTREEITEM hPrev = pTree->GetSelectedItem();

	if (hPrev == NULL)
		FindTask(TVI_ROOT, _task_to_find,_task_by_name != FALSE);
	else
	{
		if (FindTask(hPrev, _task_to_find, _task_by_name != FALSE))
			return;

		if (FindNext(hPrev, _task_to_find, _task_by_name != FALSE) == NULL)
			FindTask(TVI_ROOT, _task_to_find, _task_by_name != FALSE);
	}
}

HTREEITEM CTaskTemplEditorView::FindNext(HTREEITEM hPrev, const CString& strTask, bool bByName)
{
	CTreeCtrl* pTree = _class_mode ? &m_TemplClassTree : &m_TaskTree;
	HTREEITEM hNext = pTree->GetNextSiblingItem(hPrev);

	while (hNext)
	{
		if (CompareTask(hNext, strTask, bByName))
			return hNext;

		HTREEITEM h = FindTask(hNext, strTask, bByName);
		if (h) return h;
		hNext = pTree->GetNextSiblingItem(hNext);
	}

	HTREEITEM hParent = pTree->GetParentItem(hPrev);
	if (hParent)
		return FindNext(hParent, strTask, bByName);

	return NULL;
}

void CTaskTemplEditorView::OnTestPack()
{
	if (AfxMessageBox("操作会造成错误，确定吗?", MB_YESNO) != IDYES)
		return;

	CFileDialog dlg(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlg.DoModal() != IDOK) return;

	_recover_mode = TRUE;

	ClearTaskTree();

	if (!GetTaskTemplMan()->LoadTasksFromPack(dlg.GetPathName(), true))
		AfxMessageBox("测试加载失败！");

	UpdateTaskTreeFromPack();
}

void CTaskTemplEditorView::OnTestDynPack() 
{
	CFileDialog dlg(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlg.DoModal() != IDOK) return;

	ATaskTemplMan man;
	if (!man.LoadDynTasksFromPack(dlg.GetPathName()))
		AfxMessageBox("测试加载失败！");
	man.Release();
}

void CTaskTemplEditorView::OnExportDescript()
{
	WaitDataReady();

#ifdef _TASK_CLIENT
	CFileDialog dlg(
		FALSE,
		"txt",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		"任务描述(*.txt)|*.txt||");

	if (dlg.DoModal() != IDOK) return;

	FILE* fp = fopen(dlg.GetPathName(), "wb");
	if (fp)
	{
		TaskTemplMap& m = GetTaskTemplMan()->GetTopTemplMap();
		TreeItemMap& mapTop = GetTaskTemplMan()->GetTopTreeItemMap();
		TreeItemMap::iterator iterMap = mapTop.begin();
		for (;iterMap != mapTop.end();++iterMap)
		{
			if (m.find(iterMap->first) == m.end())
			{
				ATaskTempl* pTempl = new ATaskTempl;
				g_ulNewCount++;
				CString strPath = g_TaskIDMan.GetPathByID(iterMap->first);
				strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";
				if (!pTempl->LoadFromTextFile(strPath))
				{
					AfxMessageBox("读取硬盘上的模板文件失败！", MB_ICONSTOP);
					delete pTempl;
					g_ulDelCount++;
					return;
				}
				pTempl->SaveAllText(fp);
				delete pTempl;
				g_ulDelCount++;
			}
		}

		for (TaskTemplMap::iterator it = m.begin(); it != m.end(); ++it)
			it->second->SaveAllText(fp);

		fclose(fp);
	}

#endif
}

void CTaskTemplEditorView::ExportDescript(HTREEITEM hItem, FILE* fp)
{
	ATaskTempl* pTask = (ATaskTempl*)m_TaskTree.GetItemData(hItem);
	if (pTask)
	{
		CString strName;
		strName.Format("[%s]\r\n", CString(pTask->GetName()));
		fprintf(fp, strName);

		CString strDescript;
		strDescript.Format("%s\r\n", CString(pTask->GetDescription()));
		fprintf(fp, strDescript);
	}

	HTREEITEM hChild = m_TaskTree.GetChildItem(hItem);
	while (hChild)
	{
		ExportDescript(hChild, fp);
		hChild = m_TaskTree.GetNextSiblingItem(hChild);
	}
}

void CTaskTemplEditorView::OnClassMode() 
{
	if (_class_mode)
	{
		_class_mode = false;

		m_TaskTree.ShowWindow(SW_SHOW);
		m_Sheet.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_APPLY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CANCEL)->ShowWindow(SW_SHOW);

		m_TemplClassTree.ShowWindow(SW_HIDE);
		m_ClassSheet.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_APPLY_CLASS)->ShowWindow(SW_HIDE);
	}
	else
	{
		_class_mode = true;

		m_TaskTree.ShowWindow(SW_HIDE);
		m_Sheet.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_APPLY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CANCEL)->ShowWindow(SW_HIDE);

		m_TemplClassTree.ShowWindow(SW_SHOW);
		m_ClassSheet.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_APPLY_CLASS)->ShowWindow(SW_SHOW);
	}
}

void CTaskTemplEditorView::OnUpdateClassMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(_class_mode ? 1 : 0);
}

void CTaskTemplEditorView::OnRclickTaskTemplClass(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	CPoint pt;
	UINT uFlag;

	if(!g_DataReady)
		return;

	GetCursorPos(&pt);
	m_TemplClassTree.ScreenToClient(&pt);
	HTREEITEM hItem = m_TemplClassTree.HitTest(pt, &uFlag);
	m_TemplClassTree.ClientToScreen(&pt);

	if (hItem && (TVHT_ONITEM & uFlag))
	{
		m_TemplClassTree.Select(hItem, TVGN_CARET);

		CMenu menu;
		menu.LoadMenu(IDR_TASK_CLASS);
		CMenu* pSub = menu.GetSubMenu(0);	

		if (!m_TemplClassTree.GetParentItem(hItem))
		{
			pSub->EnableMenuItem(ID_CHECK_IN_CLASS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSub->EnableMenuItem(ID_CHECK_OUT_CLASS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
			pSub->EnableMenuItem(ID_ADD_CLASS_TEMPL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

		pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
			pt.x,
			pt.y,
			this);
	}
}

void CTaskTemplEditorView::OnSelchangedTaskTemplClass(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (!m_TemplClassTree.GetParentItem(pNMTreeView->itemNew.hItem))
	{
		GetDlgItem(IDC_APPLY_CLASS)->EnableWindow(FALSE);
		return;
	}

	ATaskTempl* pTask = reinterpret_cast<ATaskTempl*>(m_TaskTree.GetItemData(pNMTreeView->itemNew.hItem));
	if (pTask == NULL) return;
	BOOL bReadOnly;

	if (_view_mode)
		bReadOnly = FALSE;
	else
		bReadOnly = IsReadOnly(pTask->GetFilePath());

	GetDlgItem(IDC_APPLY_CLASS)->EnableWindow(!bReadOnly);

	m_ClassPage.UpdatePage(pTask, FALSE, bReadOnly);
}

void CTaskTemplEditorView::OnApplyClass() 
{
	HTREEITEM hSel = m_TemplClassTree.GetSelectedItem();
	if (hSel == 0 || !m_TemplClassTree.GetParentItem(hSel))
		return;

	ATaskTempl* pTask = reinterpret_cast<ATaskTempl*>(m_TemplClassTree.GetItemData(hSel));
	m_ClassPage.UpdatePage(pTask, TRUE, FALSE);
	pTask->SaveToTextFile(pTask->GetFilePath());
}

void CTaskTemplEditorView::OnCheckInClass()
{
	HTREEITEM hSel = m_TemplClassTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = reinterpret_cast<ATaskTempl*>(m_TemplClassTree.GetItemData(hSel));
	if (!pTask) return;

	OnApplyClass();

	CString strPath = pTask->GetFilePath();
	int nFind = strPath.ReverseFind('\\');
	strPath = strPath.Left(nFind);

	g_VSS.SetProjectPath(strPath);
	g_VSS.CheckInFile(PathFindFileName(pTask->GetFilePath()));

	if (IsReadOnly(pTask->GetFilePath()))
		m_TemplClassTree.SetItemImage(hSel, 2, 2);

	m_ClassPage.UpdatePage(pTask, FALSE, TRUE);
}

void CTaskTemplEditorView::OnCheckOutClass()
{
	HTREEITEM hSel = m_TemplClassTree.GetSelectedItem();
	if (!hSel) return;
	ATaskTempl* pTask = (ATaskTempl*)m_TemplClassTree.GetItemData(hSel);
	if (!pTask) return;

	CString strPath = pTask->GetFilePath();
	int nFind = strPath.ReverseFind('\\');
	strPath = strPath.Left(nFind);

	g_VSS.SetProjectPath(strPath);
	g_VSS.CheckOutFile(pTask->GetFilePath());

	if (!IsReadOnly(pTask->GetFilePath()))
	{
		m_TemplClassTree.SetItemImage(hSel, 3, 3);

		ATaskTempl* pNew = new ATaskTempl;
		g_ulNewCount++;
		if (!pNew->LoadFromTextFile(pTask->GetFilePath()))
		{
			AfxMessageBox("读取模板文件失败！", MB_ICONSTOP);
			delete pNew;
			g_ulDelCount++;
			return;
		}

		GetTaskTemplMan()->DelOneTaskTempl(pTask);
		GetTaskTemplMan()->AddOneTaskTempl(pNew);

		m_TemplClassTree.SetItemData(hSel, reinterpret_cast<DWORD>(pNew));
		m_ClassPage.UpdatePage(pNew, FALSE, FALSE);
	}
}

void CTaskTemplEditorView::OnTaskLink() 
{
	CTaskLinkDlg dlg;
	dlg.DoModal();
}

void CTaskTemplEditorView::OnUpdateSaveall(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(!_class_mode);
	if(g_DataReady)
	{
		pCmdUI->Enable(!_class_mode);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTaskTemplEditorView::OnUpdateNewTask(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(!_class_mode);
	if (g_DataReady)
	{
		pCmdUI->Enable(!_class_mode);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

bool npc_has_task(unsigned long npc, unsigned long task, bool bDelv)
{
	AString strNPC = g_BaseIDMan.GetPathByID(npc);

	if (strNPC.IsEmpty())
		return false;

	BaseDataTempl npc_tmpl;

	if (npc_tmpl.Load(strNPC) != 0)
		return false;

	unsigned long ulSvrID = 0;
	int nItem = npc_tmpl.GetItemIndex(bDelv ? "发放任务服务" : "验证完成任务服务");
	if (nItem >= 0) ulSvrID = npc_tmpl.GetItemValue(nItem);
	npc_tmpl.Release();
	if (!ulSvrID) return false;

	AString strSvr = g_BaseIDMan.GetPathByID(ulSvrID);
	BaseDataTempl tmpl;

	if (tmpl.Load(strSvr) != 0)
		return false;

	bool ret = false;

	for (int j = 0; j < tmpl.GetItemNum(); j++)
	{
		unsigned long ulId = tmpl.GetItemValue(j);

		if (ulId == task)
		{
			ret = true;
			break;
		}
	}

	tmpl.Release();
	return ret;
}

void clear_task_err_npc()
{
	TaskTemplMap& task_map = GetTaskTemplMan()->GetAllTemplMap();
	TaskTemplMap::iterator it = task_map.begin();
	g_BaseIDMan.Load("BaseData\\TemplID.dat");

	for (; it != task_map.end(); ++it)
	{
		ATaskTempl* pTempl = it->second;
		bool bDelvErr = false;
		bool bAwardErr = false;

		if (pTempl->m_ulDelvNPC)
			bDelvErr = !npc_has_task(pTempl->m_ulDelvNPC, pTempl->GetID(), true);

		if (pTempl->m_ulAwardNPC)
			bAwardErr = !npc_has_task(pTempl->m_ulAwardNPC, pTempl->GetID(), false);

		if (bDelvErr || bAwardErr)
		{
			ATaskTempl* pTopTask = const_cast<ATaskTempl*>(pTempl->GetTopTask());
			CString strPath = pTopTask->GetFilePath();
			int nFind = strPath.ReverseFind('\\');
			strPath = strPath.Left(nFind);
			bool bCurCheckOut;

			if (IsReadOnly(pTopTask->GetFilePath()))
			{
				g_VSS.SetProjectPath(strPath);
				g_VSS.CheckOutFile(pTopTask->GetFilePath());
				bCurCheckOut = false;
			}
			else
				bCurCheckOut = true;

			if (!IsReadOnly(pTopTask->GetFilePath()))
			{
				if (bDelvErr)
					pTempl->m_ulDelvNPC = 0;
				
				if (bAwardErr)
					pTempl->m_ulAwardNPC = 0;

				pTopTask->SaveToTextFile(pTopTask->GetFilePath());
			}

			if (!bCurCheckOut)
			{
				g_VSS.SetProjectPath(strPath);
				g_VSS.CheckInFile(PathFindFileName(pTopTask->GetFilePath()));
			}
		}
	}

	g_BaseIDMan.Release();
}

extern CString GetBaseDataNameByID(unsigned long ulID);

void check_task_npc(
	bool bDelv,
	bool bCheckOnly = false,
	CUIntArray* pNoTaskArr = NULL,
	CStringArray* pNoTaskNPCArr = NULL,
	CStringArray* pWrongTaskArr = NULL,
	CStringArray* pWrongTaskNPCArr = NULL,
	CStringArray* pWrongTaskPath = NULL
	)
{
	abase::vector<AString> PathArray;

	g_BaseIDMan.Load("BaseData\\TemplID.dat");
	g_BaseIDMan.GeneratePathArray(PathArray);
	
	if (bCheckOnly)
	{
		TaskTemplMap& all_map = GetTaskTemplMan()->GetAllTemplMap();

		for (TaskTemplMap::iterator it = all_map.begin(); it != all_map.end(); ++it)
		{
			const ATaskTempl* pTask = it->second;

			if (bDelv)
			{
				if (pTask->m_ulDelvNPC && !npc_has_task(pTask->m_ulDelvNPC, pTask->GetID(), true))
				{
					pNoTaskArr->Add(pTask->GetID());
					pNoTaskNPCArr->Add(GetBaseDataNameByID(pTask->m_ulDelvNPC));
				}
			}
			else
			{
				if (pTask->m_ulAwardNPC && !npc_has_task(pTask->m_ulAwardNPC, pTask->GetID(), false))
				{
					pNoTaskArr->Add(pTask->GetID());
					pNoTaskNPCArr->Add(GetBaseDataNameByID(pTask->m_ulAwardNPC));
				}
			}
		}
	}

	for (size_t i = 0; i < PathArray.size(); i++)
	{
		BaseDataTempl tmpl;

		if (PathArray[i].Find("BaseData\\NPC\\功能NPC\\") == -1
		 || tmpl.Load(PathArray[i]) != 0)
			continue;

		AString strNPC = tmpl.GetName();
		unsigned long ulNPC = tmpl.GetID();

		unsigned long ulSvrID = 0;
		int nItem = tmpl.GetItemIndex(bDelv ? "发放任务服务" : "验证完成任务服务");
		if (nItem >= 0) ulSvrID = tmpl.GetItemValue(nItem);
		tmpl.Release();
		if (!ulSvrID) continue;

		AString strSvr = g_BaseIDMan.GetPathByID(ulSvrID);

		if (tmpl.Load(strSvr) != 0)
			continue;

		strNPC = PathFindFileName(tmpl.GetFilePath());

		for (int j = 0; j < tmpl.GetItemNum(); j++)
		{
			unsigned long ulId = tmpl.GetItemValue(j);
			if (!ulId) continue;

			ATaskTempl* pTask = GetTaskTemplMan()->GetTaskTemplByID(ulId);

			if (!pTask)
			{
				if (bCheckOnly)
				{
					pNoTaskArr->Add(ulId);
					pNoTaskNPCArr->Add(strNPC);
				}
				else
				{
					CString strPath = tmpl.GetFilePath();
					int nFind = strPath.ReverseFind('\\');
					strPath = strPath.Left(nFind);
					bool bCurCheckOut;

					if (IsReadOnly(tmpl.GetFilePath()))
					{
						g_VSS.SetProjectPath(strPath);
						g_VSS.CheckOutFile(LPCTSTR(tmpl.GetFilePath()));
						bCurCheckOut = false;
					}
					else
						bCurCheckOut = true;

					if (!IsReadOnly(tmpl.GetFilePath()))
					{
						tmpl.SetItemValue(j, 0);
						tmpl.Save(tmpl.GetFilePath());
						WriteMd5CodeToTemplate(tmpl.GetFilePath());
					}

					if (!bCurCheckOut)
					{
						g_VSS.SetProjectPath(strPath);
						g_VSS.CheckInFile(PathFindFileName(tmpl.GetFilePath()));
					}
				}
			}

			/*
			if (bDelv && pTask->m_ulDelvNPC == ulNPC
			|| !bDelv && pTask->m_ulAwardNPC == ulNPC)
				continue;

			if (bDelv)
			{
				if (pTask->m_ulDelvNPC && npc_has_task(pTask->m_ulDelvNPC, pTask->GetID(), true))
					continue;
			}
			else
			{
				if (pTask->m_ulAwardNPC && npc_has_task(pTask->m_ulAwardNPC, pTask->GetID(), false))
					continue;
			}

			ATaskTempl* pTopTask = const_cast<ATaskTempl*>(pTask->GetTopTask());
			CString strPath = pTopTask->GetFilePath();
			int nFind = strPath.ReverseFind('\\');
			strPath = strPath.Left(nFind);
			bool bCurCheckOut;

			if (bCheckOnly)
			{
				// if (bDelv && pTask->m_ulDelvNPC
				// || !bDelv && pTask->m_ulAwardNPC)
				{
					pWrongTaskArr->Add(pTask->GetName());
					pWrongTaskNPCArr->Add(strNPC);
					pWrongTaskPath->Add(PathFindFileName(pTopTask->GetFilePath()));
				}
			}
			else
			{
				if (IsReadOnly(pTopTask->GetFilePath()))
				{
					g_VSS.SetProjectPath(strPath);
					g_VSS.CheckOutFile(pTopTask->GetFilePath());
					bCurCheckOut = false;
				}
				else
					bCurCheckOut = true;

				if (!IsReadOnly(pTopTask->GetFilePath()))
				{
					if (bDelv)
						pTask->m_ulDelvNPC = ulNPC;
					else
						pTask->m_ulAwardNPC = ulNPC;

					pTopTask->SaveToTextFile(pTopTask->GetFilePath());
				}

				if (!bCurCheckOut)
				{
					g_VSS.SetProjectPath(strPath);
					g_VSS.CheckInFile(PathFindFileName(pTopTask->GetFilePath()));
				}
			}
			*/
		}

		tmpl.Release();
	}

	g_BaseIDMan.Release();
}

void CTaskTemplEditorView::OnCheckNpc()
{
	if (AfxMessageBox("操作不可恢复，确定吗！！！", MB_YESNO) != IDYES)
		return;

	check_task_npc(true);
	check_task_npc(false);
	clear_task_err_npc();
}

void CTaskTemplEditorView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDraw)
{
}

void CTaskTemplEditorView::OnTimer(UINT nIDEvent) 
{
	CFormView::OnTimer(nIDEvent);
}

void CTaskTemplEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CFormView::OnLButtonDown(nFlags, point);
}

void CTaskTemplEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CFormView::OnRButtonDown(nFlags, point);
}

BOOL CTaskTemplEditorView::PreTranslateMessage(MSG* pMsg)
{
	return CFormView::PreTranslateMessage(pMsg);
}

void CTaskTemplEditorView::OnTestExportDyn() 
{
	CFileDialog dlgDyn(
		FALSE,
		NULL,
		"test.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlgDyn.DoModal() != IDOK)
		return;

	if (!GetTaskTemplMan()->SaveDynTasksToPack2(dlgDyn.GetPathName(), true))
		AfxMessageBox("test失败！");
}

void CTaskTemplEditorView::OnViewNpcInfo() 
{
	WaitDataReady();

	CNPCInfoDlg dlg;
	dlg.DoModal();
}

extern const char* _npc_info_path;

void CTaskTemplEditorView::OnEditNpcInfo()
{
	WaitDataReady();

	if (AfxMessageBox("确定吗？", MB_YESNO) != IDYES)
		return;

	g_VSS.SetProjectPath("BaseData");
	g_VSS.CheckOutFile(_npc_info_path);

	if (!IsReadOnly(_npc_info_path))
	{
		CNPCInfoDlg dlg;
		dlg.m_bReadOnly = false;
		dlg.DoModal();
	}
}

void CTaskTemplEditorView::OnExportNpcInfo()
{
	WaitDataReady();

	CFileDialog dlgFile(
		FALSE,
		NULL,
		"task_npc.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlgFile.DoModal() != IDOK)
		return;

	CNPCInfoDlg dlg;
	if (dlg.LoadAndUpdateNPCInfo(false))
		GetTaskTemplMan()->SaveNPCInfoToPack(dlgFile.GetPathName());
}

void CTaskTemplEditorView::OnRecord() 
{
	_check_dup_npc();
}

void CTaskTemplEditorView::OnUpdateExport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnUpdateExportDescript(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnUpdateTaskLink(CCmdUI* pCmdUI) 
{
 	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnUpdateViewNpcInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnUpdateEditNpcInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnUpdateExportNpcInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(g_DataReady?TRUE:FALSE);	
}

void CTaskTemplEditorView::OnLoadFromExcel()
{
	CFileDialog dlg(
		TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);
	
	if (dlg.DoModal() != IDOK) return;
	CExcelConverter con;
	con.ConvertExcel(dlg.GetPathName());
	const CExcelConverter::NewTaskTempls& templs = con.GetTempls();
	HTREEITEM hItem;
	g_TaskIDMan.Release();
	if (g_TaskIDMan.Load("BaseData\\TaskID.dat") != 0)
	{
		AfxMessageBox("打开TaskID文件失败", MB_ICONSTOP);
		return;
	}
	for (int i = 0; i < templs.size(); ++i)
	{
		ATaskTempl* pNew = templs[i];
		if (pNew)
		{
			GetTaskTemplMan()->AddOneTaskTempl(pNew);
			hItem = m_TaskTree.InsertItem(g_TaskIDMan.GetPathByID(pNew->m_ID), TVI_ROOT, TVI_SORT);
			m_TaskTree.SetItemData(hItem, (DWORD)pNew);
			
			TreeData sData(hItem,pNew->GetName(),pNew->m_DynTaskType);
			GetTaskTemplMan()->AddOneTreeItem(pNew,sData,true);
			
			g_VSS.SetProjectPath("BaseData\\TaskTemplate");
			g_VSS.AddFile(pNew->GetFilePath());
			g_VSS.SetProjectPath("BaseData\\TaskTemplate");
			g_VSS.CheckOutFile(pNew->GetFilePath());
		
			if (IsReadOnly(pNew->GetFilePath()))
				m_TaskTree.SetItemImage(hItem, 2, 2);
			else
				m_TaskTree.SetItemImage(hItem, 3, 3);
		}
	}
	m_TaskTree.Select(hItem, TVGN_CARET);
}