// Filename	: EC_ShortcutMgr.cpp
// Creator	: Feng Ning
// Date		: 2011/07/11

#include "EC_ShortcutMgr.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "DlgInputNO.h"

//#define new A_DEBUG_NEW

CECShortcutMgr::CECShortcutMgr()
{
	
}

CECShortcutMgr::~CECShortcutMgr()
{
	ShortCutsMap::iterator itr = m_Shortcuts.begin();
	for(;itr != m_Shortcuts.end(); ++itr)
	{
		ShortCuts* pArray = itr->second;
		for(size_t i=0;i<pArray->size();i++)
		{
			ClickShortcut* pCS = (*pArray)[i];
			delete pCS;
		}
		delete pArray;
	}
}


bool CECShortcutMgr::RegisterShortCut(ClickShortcut* pCS)
{
	AString strName;
	if(pCS->GetGroupName()) strName = pCS->GetGroupName();

	ShortCuts* pArray = NULL;
	ShortCutsMap::iterator itr = m_Shortcuts.find(strName);
	if(itr == m_Shortcuts.end())
	{
		pArray = new ShortCuts();
		m_Shortcuts[strName] = pArray;
	}
	else
	{
		pArray = itr->second;
	}

	pArray->push_back(pCS);
	return true;
}

bool CECShortcutMgr::TriggerShortcuts(const char* pGroupName, PAUIOBJECT pSrcObj)
{
	AString strName;
	if(pGroupName) strName = pGroupName;
	ShortCutsMap::iterator itr = m_Shortcuts.find(strName);
	if(itr != m_Shortcuts.end())
	{
		ShortCuts* pArray = itr->second;
		for(size_t i=0;i<pArray->size();i++)
		{
			ClickShortcut* pCS = (*pArray)[i];

			// check whether could trigger this shortcut
			if(!pCS->CanTrigger(pSrcObj))
				continue;
			
			// check the number limitation
			int nCountLeft = pCS->CheckNumber(pSrcObj);
			if(nCountLeft > 1)
			{
				// pop-up number input dialog
				CECGameUIMan* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();	
				pUIMan->InvokeNumberDialog(pSrcObj, pCS, CDlgInputNO::INPUTNO_CLICK_SHORTCUT, nCountLeft);
			}
			else if(nCountLeft > 0)
			{
				// process directly
				ASSERT(nCountLeft <= 1);
				pCS->Trigger(pSrcObj, nCountLeft);
			}
			
			return true;
		}
	}
	
	return false;
}


bool CECShortcutMgr::CheckDragClickTarget(PAUIDIALOG pDlg, PAUIOBJECT& pObjOver, const char* pName)
{
	if(!pDlg)
	{
		return false;
	}
	
	// for right-click scene
	if( !pObjOver )
	{
		pObjOver = pDlg->GetDlgItem(pName);
	}
	
	// for drag-drop scene
	if( !pObjOver || 0 != stricmp(pObjOver->GetName(), pName) )
	{
		return false;
	}
	
	return true;
}

//------------------------------------------------------------------------
// Right-Click Shortcut for Simple Usage
//------------------------------------------------------------------------

// default name constrain is used in Win_Inventory
const char* CECShortcutMgr::SimpleClickShortcutBase::GetItemName() { return "ptr_CECIvtrItem"; }
const char* CECShortcutMgr::SimpleClickShortcutBase::GetItemPrefix() { return "Item_"; }
const char* CECShortcutMgr::SimpleClickShortcutBase::GetGroupName() { return "Win_Inventory"; }

CDlgBase* CECShortcutMgr::SimpleClickShortcutBase::GetDialog() { return NULL; }

int CECShortcutMgr::SimpleClickShortcutBase::CheckNumber(PAUIOBJECT pSrcObj) { return GetDialog() ? 1 : 0; }		

bool CECShortcutMgr::SimpleClickShortcutBase::CanTrigger(PAUIOBJECT pSrcObj) 
{
	CDlgBase* pDlg = GetDialog();

	// the target dialog is shown
	if(!pDlg || !pDlg->IsShow() )
		return false;
	
	// contains a item pointer
	if(!pSrcObj || !pSrcObj->GetDataPtr(GetItemName()))
		return false;
	
	// must be a item icon
	if(!strstr(pSrcObj->GetName(), GetItemPrefix()))
		return false;
	
	return true;
}

void CECShortcutMgr::SimpleClickShortcutBase::Trigger(PAUIOBJECT pSrcObj, int num) 
{ 
	CDlgBase* pDlg = GetDialog();

	CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr(GetItemName());
	int iSrc = atoi(pSrcObj->GetName() + strlen(GetItemPrefix()));
	// make the dialog get focus
	pDlg->GetAUIManager()->BringWindowToTop(pDlg);
	pDlg->GetAUIManager()->BringWindowToTop(pSrcObj->GetParent());
	// use same logic as drag-drop
	pDlg->OnItemDragDrop(pItem, iSrc, pSrcObj, NULL);
}