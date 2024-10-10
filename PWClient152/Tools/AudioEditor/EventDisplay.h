#pragma once

typedef enum
{
	EVENT_ITEM_NONE,
	EVENT_ITEM_EVENT,
	EVENT_ITEM_GROUP,
}EVENT_ITEM_TYPE;

#define IMAGE_EVENT_GROUP	0
#define IMAGE_EVENT			1

class CTreeCtrlEx;

namespace AudioEngine
{
	class Event;
	class EventManager;
}

using AudioEngine::Event;
using AudioEngine::EventManager;

class CEventDisplay
{
public:
	CEventDisplay(void);
	~CEventDisplay(void);
public:
	bool			BuildImageList(CImageList& imageList);
	bool			UpdateTree(CTreeCtrlEx* pWndTree, EventManager* pEventManager);
	bool			BuildTree(HTREEITEM hItem);
	bool			SetSelection(Event* pEvent);
protected:
	CTreeCtrlEx*	m_pWndTree;
private:
	HTREEITEM		FindSiblingItemByName(HTREEITEM hCurItem, const char* szName);
};
