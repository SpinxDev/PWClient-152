#include "stdafx.h"
#include "EventDisplay.h"
#include "FEvent.h"
#include "FEventManager.h"
#include "FEventGroup.h"
#include "resource.h"
#include "TreeCtrlEx.h"
#include <stack>
#include <string>
#include <AAssist.h>

#ifdef _ASSIST_DLL_
#include "..\AudioAssistDll\AudioAssistDll.h"
extern CAudioAssistDllApp theApp;
#else
#include "AudioEditor.h"
extern CAudioEditorApp theApp;
#endif // _DLL

using AudioEngine::EVENT_PROPERTY;
using AudioEngine::EventGroup;
using AudioEngine::MODE_2D;
using AudioEngine::MODE_3D;

CEventDisplay::CEventDisplay(void)
: m_pWndTree(0)
{
}

CEventDisplay::~CEventDisplay(void)
{
}

bool CEventDisplay::BuildImageList(CImageList& imageList)
{
	imageList.Add(theApp.LoadIcon(IDI_ICON_EVENT_GROUP));
	imageList.Add(theApp.LoadIcon(IDI_ICON_EVENT));
	return true;
}

bool CEventDisplay::UpdateTree(CTreeCtrlEx* pWndTree, EventManager* pEventManager)
{
	if (!pWndTree)
		return false;

	m_pWndTree = pWndTree;
	m_pWndTree->DeleteAllItems();

	if (!pEventManager)
		return false;

	int iEventGroupNum = pEventManager->GetEventGroupNum();
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = pEventManager->GetEventGroupByIndex(i);
		if(!pEventGroup)
			return false;
		HTREEITEM hItem = m_pWndTree->InsertItem(AS2AC(pEventGroup->GetName()), IMAGE_EVENT_GROUP, IMAGE_EVENT_GROUP, TVI_ROOT, TVI_SORT);
		m_pWndTree->SetItemData(hItem, (DWORD_PTR)pEventGroup);
		m_pWndTree->SetExtraData(hItem, EVENT_ITEM_GROUP);
		if(!BuildTree(hItem))
			return false;
	}
	return true;
}

bool CEventDisplay::BuildTree(HTREEITEM hItem)
{
	if(m_pWndTree->GetExtraData(hItem) == EVENT_ITEM_GROUP)
	{
		EventGroup* pEventGroup = (EventGroup*)m_pWndTree->GetItemData(hItem);
		int iEventNum = pEventGroup->GetEventNum();
		for (int i=0; i<iEventNum; ++i)
		{
			Event* pEvent = pEventGroup->GetEventByIndex(i);
			if(!pEvent)
				return false;
			CString csName = AS2AC(pEvent->GetName());
			EVENT_PROPERTY prop;
			pEvent->GetProperty(prop);
			if(prop.mode == MODE_2D)
				csName += "(2D)";
			else if(prop.mode == MODE_3D)
				csName += "(3D)";
			HTREEITEM hNewItem = m_pWndTree->InsertItem(csName, IMAGE_EVENT, IMAGE_EVENT, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pEvent);
			m_pWndTree->SetExtraData(hNewItem, EVENT_ITEM_EVENT);
		}

		int iGroupNum = pEventGroup->GetEventGroupNum();
		for (int i=0; i<iGroupNum; ++i)
		{
			EventGroup* pGroup = pEventGroup->GetEventGroupByIndex(i);
			if(!pGroup)
				return false;
			HTREEITEM hNewItem = m_pWndTree->InsertItem(AS2AC(pGroup->GetName()), IMAGE_EVENT_GROUP, IMAGE_EVENT_GROUP, hItem, TVI_SORT);
			m_pWndTree->SetItemData(hNewItem, (DWORD_PTR)pGroup);
			m_pWndTree->SetExtraData(hNewItem, EVENT_ITEM_GROUP);
			if(!BuildTree(hNewItem))
				return false;
		}
	}
	return true;
}

bool CEventDisplay::SetSelection(Event* pEvent)
{
	if (!pEvent)
	{
		ASSERT( pEvent );
		return false;
	}
	
	std::stack<std::string> aGroups;
	CString csName = AS2AC(pEvent->GetName());
	EVENT_PROPERTY prop;
	pEvent->GetProperty(prop);
	if(prop.mode == MODE_2D)
		csName += "(2D)";
	else if(prop.mode == MODE_3D)
		csName += "(3D)";
	aGroups.push(AC2AS(csName).GetBuffer(csName.GetLength()));

	EventGroup* pGroup = pEvent->GetParentGroup();
	while (pGroup)
	{
		aGroups.push(pGroup->GetName());
		pGroup = pGroup->GetParentGroup();
	}

	HTREEITEM hItem = m_pWndTree->GetRootItem();
	m_pWndTree->Expand(hItem, TVE_EXPAND);
	while (!aGroups.empty())
	{
		std::string strCur = aGroups.top();
		HTREEITEM hCurSel = FindSiblingItemByName(hItem, strCur.c_str());
		m_pWndTree->SetItemState( hCurSel, TVIS_DROPHILITED, TVIS_DROPHILITED );
		m_pWndTree->SelectItem(hCurSel);
		m_pWndTree->SelectSetFirstVisible(hCurSel);
		if (hCurSel == NULL)
			break;

		m_pWndTree->Expand(hCurSel, TVE_EXPAND);
		hItem = m_pWndTree->GetChildItem(hCurSel);
		aGroups.pop();
	}
	return true;
}

HTREEITEM CEventDisplay::FindSiblingItemByName(HTREEITEM hCurItem, const char* szName)
{
	for (HTREEITEM hSibling = hCurItem; hSibling; hSibling = m_pWndTree->GetNextSiblingItem(hSibling))
	{
		CString strCurName = m_pWndTree->GetItemText(hSibling);
		if (strCurName == szName)
			return hSibling;
	}

	return NULL;
}