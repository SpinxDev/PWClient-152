// Filename	: DlgTaskList.cpp
// Creator	: Feng Ning
// Date		: 2010/09/01

#include "DlgTaskList.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "elementdataman.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_GameSession.h"
#include "EC_TaskInterface.h"
#include "TaskTemplMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTaskList, CDlgTask)

AUI_ON_COMMAND("accept",		OnCommand_Accept)
AUI_ON_COMMAND("refresh",	OnCommand_Refresh)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTaskList, CDlgTask)
AUI_ON_EVENT("Tv_Quest",		WM_LBUTTONDOWN,		OnEventLButtonDown_Tv_Quest)
AUI_END_EVENT_MAP()

CDlgTaskList::CDlgTaskList():m_iTaskToDeliverPerDay(-1)
{
}

bool CDlgTaskList::OnInitDialog()
{
	if (!CDlgTask::OnInitDialog())
		return false;
	
	return true;
}

void CDlgTaskList::RefreshTaskList()
{
	PAUITREEVIEW pTree = m_pTv_Quest;
	pTree->DeleteAllItems();
	
	const NPC_TASK_OUT_SERVICE* pSvr = GetCurrentTask();
	if (pSvr)
	{
		ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
		CECTaskInterface *pTaskInterface = GetHostPlayer()->GetTaskInterface();
		const unsigned short* pTasks = pTaskInterface->GetStorageTasks(pSvr->storage_id);
		PAUIOBJECT pObj = GetDlgItem("Txt_Title");
		if (pObj) {
			pObj->SetText(pSvr->name);
		}

		ACString strName;
		for(int i=0;i<TASK_STORAGE_LEN;i++)
		{
			const unsigned long idTask = *(pTasks++);
			if(idTask > 0 && 0 == pTaskInterface->CanDeliverTask(idTask))
			{
				ATaskTempl* pTmpl = pMan->GetTaskTemplByID(idTask);
				P_AUITREEVIEW_ITEM pItem = pTree->InsertItem( pTmpl ? pTmpl->GetName() : strName.Format(_AL("%d"), idTask)  );
				pTree->Expand(pItem, AUITREEVIEW_EXPAND_EXPAND);
				pTree->SetItemData(pItem, idTask);
			}
			//
		}
		
		StorageTaskList* pStorage = static_cast<StorageTaskList*>(GetHostPlayer()->GetTaskInterface()->GetStorageTaskList());
		unsigned long lRefreshCount = pStorage->m_StoragesRefreshCount[pSvr->storage_id - 1];//当前玩家刷新的次数
		int iRefreshPerDay = GetTaskTemplMan()->GetStorageRefreshPerDay(pSvr->storage_id);//一共可以刷新的次数
		//显示的是iRefreshPerDay - lRefreshCount 
		int refreshLeft = iRefreshPerDay - lRefreshCount + 1;
		ACString strText;
		PAUIOBJECT pNum = GetDlgItem("Lab_NumberRefresh");
		if(pNum) pNum->SetText(strText.Format(_AL("%d"), max(refreshLeft, 0)));

		int iMaxDeliverPerDay = GetTaskTemplMan()->GetMaxDeliverPerDay(pSvr->storage_id);

		if (GetDlgItem("LabelMaxDeliverTaskNumPerDay") && GetDlgItem("LabelDeliverTaskNumRemain"))
		{
			if (iMaxDeliverPerDay == 0)
			{
				GetDlgItem("LabelMaxDeliverTaskNumPerDay")->Show(false);
				GetDlgItem("LabelDeliverTaskNumRemain")->Show(false);
				m_iTaskToDeliverPerDay = -1;
			}
			else
			{
				GetDlgItem("LabelMaxDeliverTaskNumPerDay")->Show(true);
				GetDlgItem("LabelDeliverTaskNumRemain")->Show(true);
				m_iTaskToDeliverPerDay = max(iMaxDeliverPerDay - pStorage->m_StoragesReceivePerDay[pSvr->storage_id - 1],0);
				GetDlgItem("LabelDeliverTaskNumRemain")->SetText(strText.Format(_AL("%d"), m_iTaskToDeliverPerDay));
			}
		}	
		
		PAUIOBJECT pBtn = GetDlgItem("Btn_Refresh");
		if(pBtn) pBtn->Enable(refreshLeft > 0);
		
		// show the refresh item icon
		PAUIIMAGEPICTURE pIcon = (PAUIIMAGEPICTURE)GetDlgItem("Img_Questlist");
		if(pIcon)
		{
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(pSvr->storage_refresh_item, 0, 1);
			if( !pItem )
			{
				pIcon->SetCover(NULL, -1);
				pIcon->SetHint(_AL(""));
			}
			else
			{
				AString strFile;
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pIcon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
				pItem->GetDetailDataFromLocal();
				AUICTranslate trans;
				pIcon->SetHint( trans.Translate(pItem->GetDesc()) );
				delete pItem;
			}
		}
	}
	
	// show the first task
	P_AUITREEVIEW_ITEM pItem = pTree->GetFirstChildItem(pTree->GetRootItem());
	if(pItem)
	{
		ShowTaskDetail(pTree->GetItemData(pItem));
	}
	
	// a hack to show the title
	GetDlgItem("Txt_SearchQuest")->Show(true);
}

void CDlgTaskList::OnShowDialog()
{
	GetHostPlayer()->GetTaskInterface()->NotifyServerStorageTasks();

	RefreshTaskList();

	CDlgTask::OnShowDialog();
}

bool CDlgTaskList::Release(void)
{
	//
	return CDlgTask::Release();
}

void CDlgTaskList::OnCommand_Accept(const char * szCommand)
{
	PAUITREEVIEW pTree = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	
	if( pItem )
	{
		const NPC_TASK_OUT_SERVICE* pSvr = GetCurrentTask();
		if (pSvr)
		{
			ACString strMsg;
			PAUIDIALOG pDlg;
			GetGameUIMan()->MessageBox(	"Game_TaskListAccept",
										strMsg.Format(GetStringFromTable(983),pTree->GetItemText(pItem)),
										MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pDlg);
			pDlg->SetDataPtr((void*)pSvr);
			pDlg->SetData(pTree->GetItemData(pItem));
		}
	}
}

void CDlgTaskList::ConfirmAccept(PAUIDIALOG pDlg, int ret)
{
	const NPC_TASK_OUT_SERVICE* pSvr = (const NPC_TASK_OUT_SERVICE*)pDlg->GetDataPtr();
	unsigned int idTask = pDlg->GetData();
	if(ret == IDOK && pSvr)
	{
		GetGameSession()->c2s_CmdNPCSevAcceptTask(idTask, pSvr->storage_id, 0);
		
		// hack, delete from UI directly
		PAUITREEVIEW pTree = m_pTv_Quest;
		P_AUITREEVIEW_ITEM pItem = pTree->GetFirstChildItem(pTree->GetRootItem());
		while(pItem)
		{
			if(pTree->GetItemData(pItem) == idTask)
			{
				pTree->DeleteItem(pItem);
				break;
			}
			pItem = pTree->GetNextSiblingItem(pItem);
		}
		
		if (m_iTaskToDeliverPerDay != -1)
		{
			m_iTaskToDeliverPerDay--;
			ACString strText;
			PAUIOBJECT pObj = GetDlgItem("LabelDeliverTaskNumRemain");
			if (pObj)
				pObj->SetText(strText.Format(_AL("%d"), m_iTaskToDeliverPerDay));
		}
	}
}

void CDlgTaskList::OnCommand_Refresh(const char * szCommand)
{
	const NPC_TASK_OUT_SERVICE* pSvr = GetCurrentTask();
	if (!pSvr)
	{
		return;
	}

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(pSvr->storage_refresh_item, 0, 1);
	if(!pItem)
	{
		return;
	}

	// try to refresh the task
	ACString strMsg;

	if( pSvr->storage_id != 0 && 
		GetHostPlayer()->GetPack()->GetItemTotalNum(pSvr->storage_refresh_item) > 0 )
	{
		PAUIDIALOG pDlg;
		GetGameUIMan()->MessageBox(	"Game_TaskRefresh",
									strMsg.Format(GetStringFromTable(981),pItem->GetName()),
									MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pDlg);
		pDlg->SetDataPtr((void*)pSvr);
	}
	else 
	{
		GetGameUIMan()->MessageBox(	"",
									strMsg.Format(GetStringFromTable(982),pItem->GetName()),
									MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	delete pItem;
}

void CDlgTaskList::ConfirmRefresh(PAUIDIALOG pDlg, int ret)
{
	const NPC_TASK_OUT_SERVICE* pSvr = (const NPC_TASK_OUT_SERVICE*)pDlg->GetDataPtr();
	if(ret == IDOK && pSvr)
	{
		GetGameSession()->c2s_CmdNPCSevAcceptTask(0, pSvr->storage_id, pSvr->storage_refresh_item);
	}
}

void CDlgTaskList::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

void CDlgTaskList::ShowTaskDetail(int idTask)
{
	m_iType = 1;
	for( size_t i = 0; i < m_ImgCount; i++ )
	{
		m_pImg_Item[i]->Show(false);
	}
	m_pTxt_BaseAward->Show(false);

	if(idTask >= 0)
	{
		CDlgTask::SearchForTask(idTask);
	}
	else
	{
		m_idLastTask = -2;
		m_pTxt_Content->SetText(_AL(""));
		m_pTxt_QuestItem->SetText(_AL(""));
	}
}

void CDlgTaskList::OnEventLButtonDown_Tv_Quest(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
	P_AUITREEVIEW_ITEM pItem = pTree->HitTest(x, y);

	if(pItem)
	{
		ShowTaskDetail(pTree->GetItemData(pItem));
	}
}

const NPC_TASK_OUT_SERVICE* CDlgTaskList::GetCurrentTask()
{
	NPC_ESSENCE* pEssence = GetGameUIMan()->m_pCurNPCEssence;
	if (pEssence)
	{
		DATA_TYPE DataType;
		const NPC_TASK_OUT_SERVICE* pSvr = 
			(const NPC_TASK_OUT_SERVICE*)g_pGame->GetElementDataMan()->get_data_ptr(
			pEssence->id_task_out_service, ID_SPACE_ESSENCE, DataType);
		
		if (pSvr && DataType == DT_NPC_TASK_OUT_SERVICE)
		{
			return pSvr;
		}
	}
	
	return NULL;
}

bool CDlgTaskList::Tick()
{
	CDlgTask::Tick();

	PAUITREEVIEW pTree = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	ShowTaskDetail(pItem ? pTree->GetItemData(pItem) : -1);

	PAUIOBJECT pAccept = GetDlgItem("Btn_OK");
	if(pAccept)
	{
		ActiveTaskList* pLst = (ActiveTaskList*)GetHostPlayer()->GetTaskInterface()->GetActiveTaskList();
		int iMaxTaskCount = pLst->GetMaxSimultaneousCount();
		pAccept->Enable(pTree->GetCount() > 0 && 
						GetHostPlayer()->GetTaskInterface()->GetTaskCount() < iMaxTaskCount && m_iTaskToDeliverPerDay != 0);
	}
	
	return true;
}