// FWDDialogBase.cpp: implementation of the FWDBase class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogBase.h"
#include "FWAUIManager.h"
#include "FWView.h"
#include "AUIListBox.h"
#include "FWAlgorithm.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWDialogBase, FWObject)

BEGIN_EVENT_MAP_FW_BASE(FWDialogBase)

ON_EVENT_FW("BTN_OK", WM_LCLICK, FWDialogBase::OnClickBtnOK)
ON_EVENT_FW("BTN_CANCEL", WM_LCLICK, FWDialogBase::OnClickBtnCancel)

END_EVENT_MAP_FW()


BEGIN_COMMAND_MAP_FW_BASE(FWDialogBase)

END_COMMAND_MAP_FW()



#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))


FWDialogBase::FWDialogBase() : 
	m_pDlg(NULL), 
	m_pObjWhenLButtonDown(NULL), 
	m_nCloseID(FW_IDCANCEL),
	m_pDoModalCallback(NULL),
	m_pDoModalCallbackData(NULL)
{
	
}


void FWDialogBase::SetCapture()
{
	GetFWAUIManager()->SetCapture(this);
}

bool FWDialogBase::ReleaseCapture()
{
	return GetFWAUIManager()->ReleaseCapture();
}

FWDialogBase * FWDialogBase::GetCapture()
{
	return GetFWAUIManager()->GetCapture();
}

bool FWDialogBase::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj) 
{
	// since GetEventMap() is a virtual function
	// we can get the event map of current class
	const EVENT_MAP_TYPE_FW *pMap = GetEventMap();

	do
	{
		// find in my event map
		if (ProcessEventMap(pMap->pMapEntries, uMsg, wParam, lParam, pObj))
			break;
		// not found in my event map
		// find in the base class'
		pMap = pMap->pBaseMap;
	} while (pMap);
	
	// we process speciel messages here, so that we don't need to 
	// define virtual functions which the derived class should overwrite
	// and call the base version when they are processing these messages

	// generate WM_LCLICK message
	if (WM_LBUTTONDOWN == uMsg)
	{
		m_pObjWhenLButtonDown = pObj;
	}
	else if (WM_LBUTTONUP == uMsg)
	{
		if (pObj == m_pObjWhenLButtonDown)
			OnEvent(WM_LCLICK, wParam, lParam, pObj);
	}

	// Return and ESC key is mapped to OnOK() and OnCancel()
	if (WM_KEYDOWN == uMsg)
	{
		if (wParam == VK_RETURN)
			OnOK();
		else if (wParam == VK_ESCAPE)
			OnCancel();
	}


	return true; 
} 

bool FWDialogBase::ProcessEventMap(const EVENT_MAP_ENTRY_FW *pMapEntry, UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	while (pMapEntry->pFunc) 
	{ 
		if (uMsg == pMapEntry->uMessage &&	// message is match
			( 
			(!pObj && !pMapEntry->szObjectName) ||	// a  mesage to dialog
			(pObj && AString(pMapEntry->szObjectName) == pObj->GetName())  // a message to object in dialog
			)) 
		{ 
			// call the handler
			(this->*(pMapEntry->pFunc))(wParam, lParam); 
			return true; 
		} 
		pMapEntry++; 
	} 
	return false;
}

bool FWDialogBase::ProcessCommandMap(const COMMAND_MAP_ENTRY_FW *pMapEntry, const char * szCommand)
{
	AString strCommand = szCommand;

	while (pMapEntry->pFunc) 
	{ 
		if (strCommand == pMapEntry->szCommand)
		{ 
			// call the handler
			(this->*(pMapEntry->pFunc))(); 
				return true; 
		}
		pMapEntry++; 
	} 
	return false;
}

bool FWDialogBase::OnCommand(const char *szCommand)
{
	// since GetCommandMap() is a virtual function
	// we can get the command map of current class
	const COMMAND_MAP_TYPE_FW *pMap = GetCommandMap();
	
	do
	{
		// find in my command map
		if (ProcessCommandMap(pMap->pMapEntries, szCommand))
			break;
		// not found in my command map
		// find in the base class'
		pMap = pMap->pBaseMap;
	} while (pMap);

	return true;
}

FWView * FWDialogBase::GetView()
{
	return GetFWAUIManager()->GetView();
}

FWDoc * FWDialogBase::GetDocument()
{
	return GetView()->GetDocument();
}

FWAUIManager * FWDialogBase::GetFWAUIManager()
{
	return (FWAUIManager *)GetAUIDialog()->GetAUIManager();
}

void FWDialogBase::SendEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	OnEvent(uMsg, wParam, lParam, pObj);
}

void FWDialogBase::Show(bool bShow, bool bModal)
{
	GetAUIDialog()->Show(bShow, bModal);
}

bool FWDialogBase::IsShow()
{
	return GetAUIDialog()->IsShow();
}

void FWDialogBase::DoModal(FuncDoModalCallBack pCallback, void *pData)
{
	// position dialog in center of the game screen
//	HWND hWnd = GetView()->GetHwnd();
//	RECT rectTmp;
//	::GetClientRect(hWnd, &rectTmp);
//	ARectI rectW(rectTmp.left, rectTmp.top, rectTmp.right, rectTmp.bottom);
//
//	A3DRECT rectD = GetAUIDialog()->GetRect();
//
//	GetAUIDialog()->SetPos(
//		(rectW.Width() - rectD.Width()) / 2,
//		(rectW.Height() - rectD.Height()) / 2);
	GetAUIDialog()->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	// show dialog
	Show(true, true);
	
	// record the callback
	// this will be used when dialog is closed
	m_pDoModalCallback = pCallback;
	m_pDoModalCallbackData = pData;
}

void FWDialogBase::OnOK()
{
	m_nCloseID = FW_IDOK;
	
	// trigger callback if any
	if (m_pDoModalCallback)
		(*m_pDoModalCallback)(m_pDoModalCallbackData);
}

void FWDialogBase::OnCancel()
{
	m_nCloseID = FW_IDCANCEL;
	
	// trigger callback if any
	if (m_pDoModalCallback)
		(*m_pDoModalCallback)(m_pDoModalCallbackData);
}

void FWDialogBase::OnClickBtnOK(WPARAM wParam, LPARAM lParam)
{
	OnOK();
}

void FWDialogBase::OnClickBtnCancel(WPARAM wParam, LPARAM lParam)
{
	OnCancel();
}

PAUIOBJECT FWDialogBase::GetDlgItem (const char *pszItemName)
{
	return GetAUIDialog()->GetDlgItem(pszItemName);
}

void FWDialogBase::OnLButtonDownListboxTemplate(WPARAM wParam, LPARAM lParam, AUIListBox *pListbox, int *pnLastSelIndex, FuncOnSelChangeListbox pFuncOnSel)
{
	// convert screen coordinate to listbox coordinate
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	A3DRECT rect = pListbox->GetRect();
	pt = GetRelativePointToRect(&rect, &pt);
	
	// hittest
	if (pListbox->GetHitArea(pt.x, pt.y) != AUILISTBOX_RECT_TEXT) return;
	
	int nSel = pListbox->GetCurSel();
	
	// call selchange if necessary
	if (nSel != *pnLastSelIndex)
	{
		(this->*pFuncOnSel)(nSel);
		*pnLastSelIndex = nSel;
	}
}

POINT FWDialogBase::GetPos()
{
	return GetAUIDialog()->GetPos();
}

void FWDialogBase::SetCanMove(bool bCanMove)
{
	GetAUIDialog()->SetCanMove(bCanMove);
}

void FWDialogBase::SetPos(int x, int y, int alignModeX, int alignModeY, FWDialogBase *pParentDlg)
{
	GetAUIDialog()->SetPosEx(x, y, alignModeX, alignModeY, (pParentDlg ? pParentDlg->GetAUIDialog() : NULL));
}

void FWDialogBase::SetEnable(bool bEnable)
{
	GetAUIDialog()->SetEnable(bEnable);
}

bool FWDialogBase::IsEnabled()
{
	return GetAUIDialog()->IsEnabled();
}

void FWDialogBase::EnableMenuItem(const char *pszCommand, bool bEnable)
{
	GetAUIDialog()->EnableMenuItem(pszCommand, bEnable);
}

void FWDialogBase::CheckMenuItem(const char *pszCommand, bool bCheck)
{
	GetAUIDialog()->CheckMenuItem(pszCommand, bCheck);
}
 
SIZE FWDialogBase::GetSize(void)
{
	return GetAUIDialog()->GetSize();
}

bool FWDialogBase::Render(void)
{
	return GetAUIDialog()->Render();
}

bool FWDialogBase::IsModal (void)
{
	return GetAUIDialog()->IsModal();
}

FWDialogBase * FWDialogBase::GetFWDialog(const char *szClassName)
{
	return GetFWAUIManager()->GetFWDialog(szClassName);
}

A3DRECT FWDialogBase::GetRect(void)
{
	return GetAUIDialog()->GetRect();
}
