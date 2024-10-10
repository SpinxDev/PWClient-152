// workspace.cpp : implementation of the CEventBar class
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "EventBar.h"
#include "DlgEditName.h"
#include "Engine.h"
#include "Project.h"
#include "FEvent.h"
#include "FEventGroup.h"
#include "FEventManager.h"
#include "PropertyBar.h"
#include "MainFrm.h"
#include "Global.h"
#include "AudioEditorView.h"
#include "../AudioEngine/xml/xmlcommon.h"
#include "DlgSelectSoundDef.h"
#include "DlgEventPropSet.h"
#include "Render.h"
#include "DlgSelectEvent.h"
#include <AFileDialogWrapper.h>

using AudioEngine::EventGroup;
using AudioEngine::EventLayer;
using AudioEngine::SOUND_INSTANCE_PROPERTY;

#define MENU_NEW_GROUP			4001
#define MENU_NEW_EVENT			4002
#define MENU_DELETE_EVENT		4003
#define MENU_DELETE_GROUP		4004
#define MENU_REFRESH			4005
#define MENU_SAVE_PRESET		4006
#define MENU_LOAD_PRESET		4007
#define MENU_BATCH_NEW_EVENT	4008
#define MENU_BATCH_DELETE_EVENT	4009

BEGIN_MESSAGE_MAP(CEventBar, CTreeBar)
	//{{AFX_MSG_MAP(CEventBar)
	ON_COMMAND(MENU_NEW_GROUP, &CEventBar::OnNewGroup)
	ON_COMMAND(MENU_NEW_EVENT, &CEventBar::OnNewEvent)
	ON_COMMAND(MENU_BATCH_NEW_EVENT, &CEventBar::OnBatchNewEvent)
	ON_COMMAND(MENU_DELETE_GROUP, &CEventBar::OnDeleteGroup)
	ON_COMMAND(MENU_DELETE_EVENT, &CEventBar::OnDeleteEvent)
	ON_COMMAND(MENU_BATCH_DELETE_EVENT, &CEventBar::OnBatchDeleteEvent)
	ON_COMMAND(MENU_REFRESH, &CEventBar::OnRefresh)
	ON_COMMAND(MENU_SAVE_PRESET, &CEventBar::OnSavePreset)
	ON_COMMAND(MENU_LOAD_PRESET, &CEventBar::OnLoadPreset)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnTreeKeyDown)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventBar construction/destruction

CEventBar::CEventBar()
: m_hRClickItem(0)
{
}

CEventBar::~CEventBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEventBar message handlers

void CEventBar::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(!Engine::GetInstance().GetCurProject())
		return;
	CPoint pos;
	GetCursorPos(&pos);
	m_wndTree.ScreenToClient(&pos);
	m_hRClickItem = m_wndTree.HitTest(pos);

	CMenu menu;
	menu.CreatePopupMenu();
	if(!m_hRClickItem)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_GROUP, "添加事件组");
		m_wndTree.ClientToScreen(&pos);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_REFRESH, "刷新");
		menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		return;
	}
	if(!m_wndTree.SelectItem(m_hRClickItem))
		return;

	//////////////////////////////////////////////////////////////////////////
	// 右键菜单代码
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(m_hRClickItem);	
	if(type == EVENT_ITEM_GROUP)
	{
		menu.AppendMenu(MF_STRING, MENU_NEW_EVENT, "添加事件");
		menu.AppendMenu(MF_STRING, MENU_BATCH_NEW_EVENT, "批量创建事件");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_NEW_GROUP, "添加事件组");
		menu.AppendMenu(MF_STRING, MENU_DELETE_GROUP, "删除事件组");
	}
	else if(type == EVENT_ITEM_EVENT)
	{
		menu.AppendMenu(MF_STRING, MENU_DELETE_EVENT, "删除事件");
		menu.AppendMenu(MF_STRING, MENU_BATCH_DELETE_EVENT, "批量删除事件...");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_SAVE_PRESET, "保存预设...");
		menu.AppendMenu(MF_STRING, MENU_LOAD_PRESET, "加载预设...");
	}
	m_wndTree.ClientToScreen(&pos);
	menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
	//////////////////////////////////////////////////////////////////////////
	*pResult = 0;
}

void CEventBar::newGroup(HTREEITEM hItem)
{
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return;
	EventGroup* pEventGroup = 0;
	if(hItem)
		pEventGroup = (EventGroup*)m_wndTree.GetItemData(hItem);

	GF_Log(LOG_NORMAL, "新建事件组...");

	CDlgEditName dlg("事件组命名");
_EditName:
	if(IDOK != dlg.DoModal())
		return;
	if(pEventGroup)
	{
		if(pEventManager->IsEventGroupExist(pEventGroup, dlg.GetName()))
		{
			GF_Log(LOG_WARNING, "已存在事件组名“%s”", dlg.GetName());
			goto _EditName;
		}
	}
	else
	{
		if(pEventManager->IsEventGroupExist(dlg.GetName()))
		{
			GF_Log(LOG_WARNING, "已存在事件组名“%s”", dlg.GetName());
			goto _EditName;
		}
	}

	EventGroup* pNewGroup = 0;
	if(pEventGroup)
	{
		pNewGroup = pEventManager->CreateEventGroup(pEventGroup, dlg.GetName());
	}
	else
	{
		pNewGroup = pEventManager->CreateEventGroup(dlg.GetName());
	}
	if(!pNewGroup)
		return;
	HTREEITEM hChildItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_EVENT_GROUP, IMAGE_EVENT_GROUP, hItem, TVI_SORT);
	m_wndTree.SetItemData(hChildItem, (DWORD_PTR)pNewGroup);
	m_wndTree.SetExtraData(hChildItem, EVENT_ITEM_GROUP);
	m_wndTree.Expand(hItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);

	GF_Log(LOG_NORMAL, "成功创建事件组“%s”", dlg.GetName());
}

void CEventBar::OnNewGroup()
{
	newGroup(m_hRClickItem);
}

void CEventBar::OnNewEvent()
{
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return;
	EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pEventGroup)
		return;

	GF_Log(LOG_NORMAL, "新建事件...");

	CDlgEditName dlg("事件命名");
_EditName:
	if(IDOK != dlg.DoModal())
		return;
	if(pEventManager->IsEventExist(pEventGroup, dlg.GetName()))
	{
		GF_Log(LOG_WARNING, "已存在事件名“%s”", dlg.GetName());
		goto _EditName;
	}
	Event* pNewEvent = pEventManager->CreateEvent(pEventGroup, dlg.GetName());
	if(!pNewEvent)
		return;
	HTREEITEM hItem = m_wndTree.InsertItem(dlg.GetName(), IMAGE_EVENT, IMAGE_EVENT, m_hRClickItem, TVI_SORT);
	m_wndTree.SetItemData(hItem, (DWORD_PTR)pNewEvent);
	m_wndTree.SetExtraData(hItem, EVENT_ITEM_EVENT);
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功创建事件“%s”", dlg.GetName());
}

void CEventBar::OnBatchNewEvent()
{
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return;
	EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pEventGroup)
		return;

	GF_Log(LOG_NORMAL, "批量创建事件...");
	CDlgSelectSoundDef dlgSelSoundDef(true);
	if(IDOK != dlgSelSoundDef.DoModal())
		return;
	SoundDefList listSoundDef = dlgSelSoundDef.GetSelectSoundDefList();
	if(listSoundDef.size() == 0)
		return;
	CDlgEventPropSet dlgEventPropSet;
	dlgEventPropSet.DoModal();
	EVENT_PROPERTY prop = dlgEventPropSet.GetProp();
	SoundDefList::const_iterator it = listSoundDef.begin();
	for (; it!=listSoundDef.end(); ++it)
	{
		SoundDef* pSoundDef = *it;
		if(pEventManager->IsEventExist(pEventGroup, pSoundDef->GetName()))
		{
			GF_Log(LOG_WARNING, "已存在事件名“%s”", pSoundDef->GetName());
			continue;
		}
		Event* pNewEvent = pEventManager->CreateEvent(pEventGroup, pSoundDef->GetName());
		if(!pNewEvent)
			return;
		pNewEvent->SetProperty(prop);
		HTREEITEM hEventItem = m_wndTree.InsertItem(pSoundDef->GetName(), IMAGE_EVENT, IMAGE_EVENT, m_hRClickItem, TVI_SORT);
		m_wndTree.SetItemData(hEventItem, (DWORD_PTR)pNewEvent);
		m_wndTree.SetExtraData(hEventItem, EVENT_ITEM_EVENT);
		EventLayer* pEventLayer = pNewEvent->CreateLayer("layer1");
		if(!pEventLayer)
			return;
		if(!pEventLayer->AttachParameter("default"))
			return;
		SOUND_INSTANCE_PROPERTY soundInsOrop;
		soundInsOrop.fStartPosition = 0;
		soundInsOrop.fLength = 1;
		SoundInstanceTemplate* pSoundInstanceTemplate = pEventLayer->AddSoundInstanceTemplate(pSoundDef);
		if(!pSoundInstanceTemplate)
			return;
		pSoundInstanceTemplate->SetProperty(soundInsOrop);
	}
	m_wndTree.Expand(m_hRClickItem, TVM_EXPAND);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "批量创建事件成功");
}

void CEventBar::OnDeleteGroup()
{
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(hSelItem);
	if(!pEventGroup)
		return;

	GF_Log(LOG_NORMAL, "删除事件组...");

	CString csName = pEventGroup->GetName();
	char szInfo[1024] = {0};
	sprintf_s(szInfo, "确定删除事件组\"%s\"吗？", csName);
	if(IDYES != MessageBox(szInfo, "警告", MB_ICONWARNING|MB_YESNO))
		return;
	if(pEventGroup->GetEventGroupNum() || pEventGroup->GetEventNum())
	{
		sprintf_s(szInfo, "事件组\"%s\"不为空，确定删除吗？", csName);
		if(IDYES != MessageBox(szInfo, "警告", MB_ICONWARNING|MB_YESNO))
			return;
	}
	if(!pEventManager->DeleteEventGroup(pEventGroup))
	{
		GF_Log(LOG_ERROR, "删除事件组“%s”失败", csName);
		return;
	}
	
	m_wndTree.DeleteItem(hSelItem);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功删除事件组“%s”", csName);
}

void CEventBar::OnDeleteEvent()
{	
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	Event* pEvent = (Event*)m_wndTree.GetItemData(hSelItem);
	if(!pEvent)
		return;

	GF_Log(LOG_NORMAL, "删除事件...");
	
	char szInfo[1024] = {0};
	sprintf_s(szInfo, "确定删除事件\"%s\"吗？", pEvent->GetName());
	if(IDYES != MessageBox(szInfo, "警告", MB_ICONWARNING|MB_YESNO))
		return;
	if(!deleteEvent(pEvent))
		return;
	m_wndTree.DeleteItem(hSelItem);
}

void CEventBar::OnBatchDeleteEvent()
{
	CDlgSelectEvent dlg(true);
	if(IDOK != dlg.DoModal())
		return;
	EventList listEvent = dlg.GetSelectEventList();
	EventList::const_iterator it = listEvent.begin();
	for (; it!=listEvent.end(); ++it)
	{
		Event* pEvent = *it;
		if(!pEvent)
			continue;
		if(!deleteEvent(pEvent))
			continue;
	}
	UpdateTree();
}

bool CEventBar::deleteEvent(Event* pEvent)
{
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return false;
	CString csName = pEvent->GetName();
	if(!pEventManager->DeleteEvent(pEvent))
	{
		GF_Log(LOG_ERROR, "删除事件“%s”失败。", csName);
		return false;
	}	
	GF_GetView()->GetEventView()->SetEvent(0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功删除事件“%s”", csName);
	return true;
}

bool CEventBar::UpdateTree()
{
	if(!Engine::GetInstance().GetCurProject())
	{
		m_wndTree.DeleteAllItems();
		return false;
	}
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return false;
	if(!m_eventDisplay.UpdateTree(&m_wndTree, pEventManager))
		return false;
	return false;
}

void CEventBar::OnRefresh()
{
	UpdateTree();
}

void CEventBar::OnTreeLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bDragging)
		return;
	m_bDragging = false;
	CImageList::DragLeave(&m_wndTree);
	CImageList::EndDrag();
	ReleaseCapture();
	delete m_pDragImage;
	m_pDragImage = 0;

	m_wndTree.SelectDropTarget(NULL);
	if(m_hItemDragSrc == m_hItemDragDest)
		goto _end;
	
	m_wndTree.Expand(m_hItemDragDest, TVE_EXPAND);
	if(m_wndTree.IsChildItem(m_hItemDragDest, m_hItemDragSrc))
		goto _end;
	//////////////////////////////////////////////////////////////////////////
	// 填入数据处理代码
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		goto _end;
	EVENT_ITEM_TYPE src_type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(m_hItemDragSrc);
	if(!m_hItemDragDest)
	{
		if(src_type != EVENT_ITEM_GROUP)
			goto _end;
		EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(m_hItemDragSrc);
		if(!pEventManager->MoveEventGroup(pEventGroup))
		{
			GF_Log(LOG_ERROR, "移动事件组“%s”失败。", pEventGroup->GetName());
			goto _end;
		}
		GF_Log(LOG_NORMAL, "成功移动事件组“%s”到根目录", pEventGroup->GetName());

		UpdateTree();
	}
	else
	{
		EVENT_ITEM_TYPE dest_type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(m_hItemDragDest);
		if(dest_type != EVENT_ITEM_GROUP)
			goto _end;
		EventGroup* pDestGroup = (EventGroup*)m_wndTree.GetItemData(m_hItemDragDest);		
		if(src_type == EVENT_ITEM_EVENT)
		{
			Event* pEvent = (Event*)m_wndTree.GetItemData(m_hItemDragSrc);
			if(!pEventManager->MoveEvent(pEvent, pDestGroup))
			{
				GF_Log(LOG_ERROR, "移动事件“%s”到“%s”失败。", pEvent->GetName(), pDestGroup->GetName());
				goto _end;
			}

			GF_Log(LOG_NORMAL, "成功移动事件“%s”到“%s”", pEvent->GetName(), pDestGroup->GetName());
		}
		else if(src_type == EVENT_ITEM_GROUP)
		{
			EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(m_hItemDragSrc);
			if(!pEventManager->MoveEventGroup(pEventGroup, pDestGroup))
			{
				GF_Log(LOG_ERROR, "移动事件组“%s”到“%s”失败。", pEventGroup->GetName(), pDestGroup->GetName());
				goto _end;
			}
			GF_Log(LOG_NORMAL, "成功移动事件组“%s”到“%s”", pEventGroup->GetName(), pDestGroup->GetName());
		}
		m_wndTree.DeleteItem(m_hItemDragSrc);
		m_wndTree.DeleteChildren(m_hItemDragDest);
		if(!m_eventDisplay.BuildTree(m_hItemDragDest))
			goto _end;
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	//////////////////////////////////////////////////////////////////////////
_end:
	KillTimer(m_nScrollTimerID);
}

void CEventBar::OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPropertyBar* pWndPropBar = GF_GetMainFrame()->GetPropertyBar();
	if(!pWndPropBar)
		return;
	PROP_TYPE prop_type = PROP_NONE;
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
	GF_GetView()->SetCurView(VIEW_EVENT);
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == EVENT_ITEM_GROUP)
	{
		EventGroup* pEventGroup = (EventGroup*)m_wndTree.GetItemData(hItem);
		if(!pEventGroup)
			return;
		prop_type = PROP_EVENT_GROUP;
		GF_Log(LOG_NORMAL, "选中事件组“%s”", pEventGroup->GetName());
	}
	else if(type == EVENT_ITEM_EVENT)
	{
		Event* pEvent = (Event*)m_wndTree.GetItemData(hItem);
		if(!pEvent)
			return;		
		prop_type = PROP_EVENT;
		GF_GetView()->GetEventView()->SetEvent(pEvent);
		GF_Log(LOG_NORMAL, "选中事件“%s”", pEvent->GetName());
	}

	if(type != EVENT_ITEM_EVENT)
	{
		GF_GetView()->GetEventView()->SetEvent(0);
	}

	pWndPropBar->BuildProperty(prop_type, hItem);

	*pResult = 0;
}

BOOL CEventBar::SetItemText(HTREEITEM hItem, LPCTSTR lpszText)
{
	if(!hItem || !lpszText || !lpszText[0])
		return FALSE;
	EventManager* pEventManager = Engine::GetInstance().GetCurProject()->GetEventManager();
	if(!pEventManager)
		return FALSE;
	DWORD_PTR pData = m_wndTree.GetItemData(hItem);
	if(!pData)
		return FALSE;
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type == EVENT_ITEM_EVENT)
	{
		Event* pEvent = (Event*)pData;
		if(pEventManager->IsEventExist(pEvent->GetParentGroup(), lpszText))
		{
			GF_Log(LOG_WARNING, "已存在事件组“%s”", lpszText);
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pEvent->SetName(lpszText);
	}
	else if(type == EVENT_ITEM_GROUP)
	{
		bool bExist = false;
		EventGroup* pEventGroup = (EventGroup*)pData;
		if(pEventGroup->GetParentGroup())
		{
			if(pEventManager->IsEventGroupExist(pEventGroup->GetParentGroup(), lpszText))
				bExist = true;
		}
		else
		{
			if(pEventManager->IsEventGroupExist(lpszText))
				bExist = true;
		}
		if(bExist)
		{
			GF_Log(LOG_WARNING, "已存在事件组“%s”", lpszText);			
			return FALSE;
		}
		if(!CTreeBar::SetItemText(hItem, lpszText))
			return FALSE;
		pEventGroup->SetName(lpszText);
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
	return TRUE;
}
void CEventBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CTreeBar::OnPaint()
	CTreeBar::OnPaint();
	if(!GF_GetView()->GetEventView()->IsWindowVisible())
	{
		GF_GetView()->SetCurView(VIEW_EVENT);
		HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
		if(!hSelItem)
			return;
		m_wndTree.SelectItem(m_wndTree.GetRootItem());
		m_wndTree.SelectItem(hSelItem);
	}	
}

int CEventBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_eventDisplay.BuildImageList(m_ImageList))
		return -1;

	return 0;
}

void CEventBar::OnSavePreset()
{
	Event* pEvent = (Event*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pEvent)
		return;	

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),FALSE, g_Configs.szEventPresetPath, "保存Event预设文件","",this);	
	if(IDOK != dlg.DoModal())
		return;

	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("EventPreset");
	doc.LinkEndChild(root);
	pEvent->SaveXML(root, true);
	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "w"))
	{
		return;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	GF_Log(LOG_NORMAL, "成功保存预设%s", dlg.GetRelativePathName());
}

void CEventBar::OnLoadPreset()
{
	Event* pEvent = (Event*)m_wndTree.GetItemData(m_hRClickItem);
	if(!pEvent)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),TRUE, g_Configs.szEventPresetPath, "选择Event预设文件","",this);	
	if(IDOK != dlg.DoModal())
		return;

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "r"))
	{
		return;
	}

	fseek(pFile, 0, SEEK_END);
	long lLength = ftell(pFile);
	char* pBuf = new char[lLength + 1];
	memset(pBuf, 0, lLength + 1);
	fseek(pFile, 0, SEEK_SET);
	fread(pBuf, lLength, 1, pFile);	
	fclose(pFile);
	pBuf[lLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	delete [] pBuf;

	TiXmlNode* pNode = doc.FirstChild("EventPreset");
	if (!pNode)
	{
		GF_Log(LOG_ERROR, "%s不是Event预设文件", dlg.GetFileName());
		return;
	}

	TiXmlElement* root = pNode->ToElement();
	pEvent->LoadXML(root->FirstChild("Event")->ToElement(), true);
	GF_GetView()->GetEventView()->SetEvent(0);
	GF_GetView()->GetEventView()->SetEvent(pEvent);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功加载预设%s", dlg.GetFileName());
}

void CEventBar::OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVKEYDOWN* pKeyDown = reinterpret_cast<NMTVKEYDOWN*>(pNMHDR);
	if (!pKeyDown)
		return;

	WORD nChar = pKeyDown->wVKey;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hSelItem);
	if(nChar == VK_DELETE)
	{
		switch(type)
		{
		case EVENT_ITEM_EVENT:
			OnDeleteEvent();
			break;
		case EVENT_ITEM_GROUP:
			OnDeleteGroup();
			break;
		}
	}
	else if((nChar == 'a' || nChar == 'A') && GetKeyState(VK_MENU) < 0)
	{
		if(type == EVENT_ITEM_GROUP)
			newGroup(hSelItem);
	}
}