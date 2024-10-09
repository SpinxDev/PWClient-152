/*
 * File:	DlgSysModuleQuickBar.cpp
 * Created: WYD
 * Date:	2013/09/03
 */

#include "DlgSysModuleQuickBar.h"
#include "EC_GameUIMan.h"
#include "EC_ShortcutMgr.h"
#include "EC_ShortcutSet.h"
#include "AUIManager.h"
#include "EC_Shortcut.h"
#include "EC_HostPlayer.h"
#include "DlgDragDrop.h"
#include "DlgTrade.h"
#include "DlgShop.h"
#include "EC_Global.h"
#include "EC_FixedMsg.h "
#include "EC_Game.h"
#include <A3DEngine.h>
#include <A3DViewport.h>
//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgInventory
//------------------------------------------------------------------------
class SysModClickShortcut : public CECShortcutMgr::ClickShortcut
{
public:
	SysModClickShortcut(const char* pFromDlgName, CDlgSysModQuickBar* pToDlg)
		:m_pDlg(pToDlg)
		,m_GroupName(pFromDlgName)
	{}
	
	virtual const char* GetGroupName() { return m_GroupName; }
	
	virtual bool CanTrigger(PAUIOBJECT pSrcObj) 
	{
		return // the target dialog is shown
			m_pDlg && m_pDlg->IsShow() &&  dynamic_cast<CDlgSysModQuickBar*>(m_pDlg) &&
			// the source dialog is correct
			pSrcObj->GetParent() == m_pDlg->GetAUIManager()->GetDialog(GetGroupName()) &&
			// contains a item pointer
			pSrcObj && pSrcObj->GetDataPtr("ptr_CECSCSysModule");
	}

	virtual void Trigger(PAUIOBJECT pSrcObj, int num) 
	{ 
		CECSCSysModule* pItem = (CECSCSysModule*)pSrcObj->GetDataPtr("ptr_CECSCSysModule");
		CDlgSysModQuickBar* pOverDlg = (CDlgSysModQuickBar*)m_pDlg;
		PAUIOBJECT pObjOver = pOverDlg->GetFirstEmptySlot();
		if(pObjOver)
		{
			PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
			CDlgDragDrop* pDragDrop = dynamic_cast<CDlgDragDrop*>(pUIMan->GetDialog("DragDrop"));
			// use the same logic as drag-drop
			pDragDrop->OnSysModDragDrop(pSrcObj->GetParent(), pSrcObj, pObjOver->GetParent(), pObjOver, NULL);
		}
	}
	
protected:
	CDlgSysModQuickBar* m_pDlg;
	AString m_GroupName;
};

//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_EVENT_MAP(CDlgSysModule, CDlgBase)

AUI_ON_EVENT("Icon_*", WM_RBUTTONUP,	OnEventRButtonUp)
AUI_ON_EVENT("Icon_*", WM_LBUTTONDBLCLK,OnEventRButtonUp)
AUI_ON_EVENT("Icon_*", WM_LBUTTONDOWN,  OnEventLButtonDown)

AUI_END_EVENT_MAP()

//////////////////////////////////////////////////////////////////////////
CDlgSysModule::~CDlgSysModule()
{
	for (int i=0;i<CECSCSysModule::FM_NUM;i++)
	{
		delete m_sysmods[i];
	}
}
bool CDlgSysModule::OnInitDialog()
{
	AString str;
	for (int i=0;i<CECSCSysModule::FM_NUM;i++)
	{
		m_sysmods[i] = new CECSCSysModule;
		m_sysmods[i]->Init(i);
		str.Format("Icon_%02d",i+1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(str);
		if(pObj)		
			pObj->SetDataPtr(m_sysmods[i],"ptr_CECSCSysModule");	
		
		m_pLbl_SysName[i] = NULL;
		str.Format("Label_Sys%d",i+1);
		DDX_Control(str,m_pLbl_SysName[i]);
	}

	return CDlgBase::OnInitDialog();
}

void CDlgSysModule::OnShowDialog()
{
	CECStringTab* pStrTab = g_pGame->GetItemDesc();	
	AString str;
	for (int i=0;i<CECSCSysModule::FM_NUM;i++)
	{
		str.Format("Icon_%02d",i+1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(str);
		if(pObj)		
			pObj->SetCover(GetGameUIMan()->GetSysModuleIcon(i),0);	
		
			
		const wchar_t* pStr = pStrTab->GetWideString(ITEMDESC_SYSMODULE_FM_GT + CECSCSysModule::g_SysIndexMap[i]);
		if(pStr)
			m_pLbl_SysName[i]->SetText(pStr);
	}
}
void CDlgSysModule::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}


void CDlgSysModule::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{	
	if(!strstr(pObj->GetName(), "Icon_") )	
		return;	
	
	int iSrc = atoi(pObj->GetName() + strlen("Icon_"));	
	
	// try to trigger shortcuts
	GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(GetName(), pObj);
}

//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgSysModQuickBar, CDlgBase)
AUI_ON_COMMAND("Btn_Add",		OnCommand_add)
AUI_ON_COMMAND("Btn_Minus",		OnCommand_minus)
AUI_ON_COMMAND("Btn_AddIcon",	OnCommand_sysmod)
AUI_ON_COMMAND("Chk_temp",		OnCommand_mini)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSysModQuickBar, CDlgBase)
AUI_ON_EVENT("Item_*", WM_LBUTTONDOWN,  OnEventLButtonDown)
AUI_END_EVENT_MAP()

bool CDlgSysModQuickBar::OnInitDialog()
{	
	m_pBtn_AddIcon = NULL;
	m_pBtn_Add = NULL;
	m_pBtn_Minus = NULL;
	m_pChk_Mini = NULL;
	m_pBtn_Bg = NULL;

	CECShortcutMgr* pShortcut = GetGameUIMan()->GetShortcutMgr();
	pShortcut->RegisterShortCut(new SysModClickShortcut("Win_SystemMod", this));

	DDX_Control("Btn_AddIcon",m_pBtn_AddIcon);
	DDX_Control("Btn_Add",m_pBtn_Add);
	DDX_Control("Btn_Minus",m_pBtn_Minus);
	DDX_Control("Chk_temp",m_pChk_Mini);
	DDX_Control("Img_Back",m_pBtn_Bg);
	AString str;
	for(int i=0;i<SIZE_SYSMODSCSET;i++)
	{
		m_pBtn_Item[i] = NULL;
		str.Format("Item_%d",i+1);
		DDX_Control(str,m_pBtn_Item[i]);	
	}	
	
	m_bFlash = true;

	return CDlgBase::OnInitDialog();
}
void CDlgSysModQuickBar::OnCommand_minus(const char * szCommand)
{
	int i = GetHostPlayer()->GetCurSysModShortcutSetIndex();
	GetHostPlayer()->SetCurSysModShortcutSetIndex(i-1);
}
void CDlgSysModQuickBar::OnCommand_add(const char * szCommand)
{
	int i = GetHostPlayer()->GetCurSysModShortcutSetIndex();
	GetHostPlayer()->SetCurSysModShortcutSetIndex(i+1);
}
void CDlgSysModQuickBar::OnCommand_sysmod(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_SystemMod");
	if(pDlg)
	{
		pDlg->Show(!pDlg->IsShow());
		m_bFlash = false;
	}
}
void CDlgSysModQuickBar::OnCommand_mini(const char * szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;

	m_pBtn_AddIcon->Show(!m_pChk_Mini->IsChecked());
	m_pBtn_Add->Show(!m_pChk_Mini->IsChecked());
	m_pBtn_Minus->Show(!m_pChk_Mini->IsChecked());
	m_pBtn_Bg->Show(!m_pChk_Mini->IsChecked());
	for(int i=0;i<SIZE_SYSMODSCSET;i++)
	{
		m_pBtn_Item[i]->Show(!m_pChk_Mini->IsChecked());
	}
}
void CDlgSysModQuickBar::SetMiniMode(bool bMini)
{	
	m_pChk_Mini->Check(bMini);
	OnCommand_mini(NULL);
}
void CDlgSysModQuickBar::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	void *ptr;
	AString szType;
	pObj->ForceGetDataPtr(ptr,szType);
	if( !ptr ) return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

PAUIOBJECT CDlgSysModQuickBar::GetFirstEmptySlot()
{
	PAUIOBJECT ret = NULL;
	AString str;
	int i=0;

	while(true)
	{
		str.Format("Item_%d",++i);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p)
			break;
		if(p->GetDataPtr("ptr_CECSCSysModule"))
			continue;
		ret = p;
		break;
	}
	return ret;
}
void CDlgSysModQuickBar::OnTick()
{
	CECShortcutSet* pSCS = GetHostPlayer()->GetCurSysModShortcutSet();
	if(!pSCS) return ;
	
	AUIClockIcon *pClock = NULL;
	
	for( int j = 0; j < SIZE_SYSMODSCSET; j++ )
	{
		PAUIIMAGEPICTURE pCell = m_pBtn_Item[j];
		
		CECShortcut* pSC = pSCS->GetShortcut(j);
		CECSCSysModule* pSCModule = dynamic_cast<CECSCSysModule*>(pSC);
		pClock = pCell->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);
		if( pSCModule )
		{				
			pCell->SetColor(A3DCOLORRGB(255, 255, 255));
			pCell->SetDataPtr(pSC,"ptr_CECSCSysModule");
			pCell->SetCover(GetGameUIMan()->GetSysModuleIcon(pSCModule->GetSysModID()),0);	
			pCell->SetHint(pSCModule->GetDesc());
		}
		else
		{
			pCell->SetCover(NULL, -1);
			pCell->SetText(_AL(""));
			pCell->SetDataPtr(NULL);
			pCell->SetColor(A3DCOLORRGB(255, 255, 255));
			pCell->SetHint(_AL(""));
		}
	}

	if (m_pBtn_AddIcon)
	{
		m_pBtn_AddIcon->SetFlash(m_bFlash);

		ACString str;
		str.Format(_AL("%d"),GetHostPlayer()->GetCurSysModShortcutSetIndex()+1);
		m_pBtn_AddIcon->SetText(str);
	}
}