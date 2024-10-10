// DlgSelectEvent.cpp : implementation file
//

#include "stdafx.h"
#include "EventSelector.h"
#include "DlgSelectEvent.h"
#include <FEventSystem.h>
#include <FEventProject.h>
#include <FEventManager.h>
#include <FEvent.h>
#include <FEventInstance.h>
#include "..\AudioEditor\DynSizeCtrl.h"

#define _EVENT_SYSTEM_TIMER_ 1

using AudioEngine::EventProject;

// CDlgSelectEvent dialog

IMPLEMENT_DYNAMIC(CDlgSelectEvent, CDialog)

CDlgSelectEvent::CDlgSelectEvent(EventSystem* pEventSystem, const VECTOR& vListenerPos, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectEvent::IDD, pParent)
	, m_pEventSystem(pEventSystem)
	, m_pSelectEvent(0)
	, m_pEventInstance(0)
	, m_pPlayingEvent(0)
	, m_vListenerPos(vListenerPos)
{
	m_pDscMan = new DynSizeCtrlMan;
}

CDlgSelectEvent::~CDlgSelectEvent()
{
	delete m_pDscMan;
	m_pDscMan = 0;
}

void CDlgSelectEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_EVENT, m_wndTree);
	DDX_Control(pDX, IDC_PROJ_LIST, m_ProjLst);
}


BEGIN_MESSAGE_MAP(CDlgSelectEvent, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectEvent::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_EVENT, &CDlgSelectEvent::OnNMDblclkTreeEvent)
	ON_LBN_SELCHANGE(IDC_PROJ_LIST, &CDlgSelectEvent::OnLbnSelchangeProjList)
	ON_BN_CLICKED(IDC_PLAY, &CDlgSelectEvent::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CDlgSelectEvent::OnBnClickedStop)
	ON_COMMAND(IDCANCEL, &CDlgSelectEvent::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_NULL, &CDlgSelectEvent::OnBnClickedBtnNull)
	ON_WM_TIMER()
END_MESSAGE_MAP()

const char* CDlgSelectEvent::GetSelectEventGuid() const
{
	if (m_pSelectEvent)
		return m_pSelectEvent->GetGuidString();

	return NULL;
}

Event* CDlgSelectEvent::GetSelectEvent() const
{
	return m_pSelectEvent;
}

// return false if not exist or event not found
bool CDlgSelectEvent::SetInitSelectEvent(const char* szGuid)
{
	m_pSelectEvent = m_pEventSystem->GetEvent(szGuid);
	return true;
}

BOOL CDlgSelectEvent::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(!m_pEventSystem)
		return FALSE;

	m_pDscMan->Add(DynSizeCtrl(&m_wndTree, this));
	m_pDscMan->Add(DynSizeCtrl(&m_ProjLst, this));
	m_pDscMan->Add(DynSizeCtrl(GetDlgItem(IDOK), this));
	m_pDscMan->Add(DynSizeCtrl(GetDlgItem(IDCANCEL), this));
	m_pDscMan->Add(DynSizeCtrl(GetDlgItem(IDC_PLAY), this));
	m_pDscMan->Add(DynSizeCtrl(GetDlgItem(IDC_STOP), this));

	if(!m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 6))
		return -1;
	m_wndTree.SetImageList(&m_ImageList, TVSIL_NORMAL);	

	m_wndTree.ModifyStyle(0, TVS_SHOWSELALWAYS);

	for(int projIdx = 0; projIdx < m_pEventSystem->GetProjectNum(); ++projIdx)
	{
		EventProject* pEventProj = m_pEventSystem->GetProjectByIndex(projIdx);
		EventManager* pEventManger = pEventProj->GetEventManager();
		m_ProjLst.InsertString(projIdx, pEventProj->GetName());
		m_ProjLst.SetItemData(projIdx, (DWORD_PTR)pEventManger);
	}

	if(!m_EventDisplay.BuildImageList(m_ImageList))
		return FALSE;

	if(m_pSelectEvent)
	{
		// select the initial event
		EventProject* pEventProj = m_pSelectEvent->GetEventProject();
		for (int iIdx = 0; iIdx < m_ProjLst.GetCount(); ++iIdx)
		{
			CString strName;
			m_ProjLst.GetText(iIdx, strName);
			if (strName == pEventProj->GetName())
			{
				m_ProjLst.SetCurSel(iIdx);
				m_EventDisplay.UpdateTree(&m_wndTree, pEventProj->GetEventManager());
				m_EventDisplay.SetSelection(m_pSelectEvent);
				break;
			}
		}		
	}

	SetTimer(_EVENT_SYSTEM_TIMER_, 80, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectEvent::OnBnClickedOk()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		goto _failed;
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type != EVENT_ITEM_EVENT)
		goto _failed;
	m_pSelectEvent = (Event*)m_wndTree.GetItemData(hItem);
	if(!m_pSelectEvent)
		goto _failed;	
	KillTimer(_EVENT_SYSTEM_TIMER_);
	release();
	CDialog::OnOK();
	return;
_failed:
	MessageBox("请选择一个事件");	
}

void CDlgSelectEvent::OnNMDblclkTreeEvent(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedOk();
	*pResult = 0;
}

void CDlgSelectEvent::OnLbnSelchangeProjList()
{
	int iCurSel = m_ProjLst.GetCurSel();
	if (iCurSel < 0)
	{
		m_EventDisplay.UpdateTree(&m_wndTree, NULL);
	}
	else
	{
		EventManager* pEventMan = reinterpret_cast<EventManager*>(m_ProjLst.GetItemData(iCurSel));
		m_EventDisplay.UpdateTree(&m_wndTree, pEventMan);
	}
}

void CDlgSelectEvent::OnBnClickedPlay()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		goto _failed;
	EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type != EVENT_ITEM_EVENT)
		goto _failed;
	m_pSelectEvent = (Event*)m_wndTree.GetItemData(hItem);
	if(!m_pSelectEvent)
		goto _failed;
	OnBnClickedStop();
	m_pPlayingEvent = m_pSelectEvent;
	m_pEventInstance = m_pSelectEvent->CreateInstance(false);
	m_pEventInstance->Set3DAttributes(m_vListenerPos, 0);
	m_pEventInstance->Start();
	return;
_failed:
	MessageBox("请选择一个事件");
}

void CDlgSelectEvent::OnBnClickedStop()
{
	release();
}

void CDlgSelectEvent::OnBnClickedCancel()
{
	release();
	KillTimer(_EVENT_SYSTEM_TIMER_);
	CDialog::OnCancel();
}

void CDlgSelectEvent::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_pDscMan->Resize(cx, cy);
}

void CDlgSelectEvent::release()
{
	if(m_pEventInstance && m_pPlayingEvent)
	{
		m_pEventInstance->Stop(true);
		m_pPlayingEvent->DestroyInstance(m_pEventInstance);
		m_pEventInstance = 0;
		m_pPlayingEvent = 0;
	}
}

void CDlgSelectEvent::OnBnClickedBtnNull()
{
	m_pSelectEvent = 0;
	KillTimer(_EVENT_SYSTEM_TIMER_);
	CDialog::OnOK();
}

void CDlgSelectEvent::OnTimer(UINT_PTR nIDEvent)
{
	if(m_pEventSystem)
		m_pEventSystem->Update();

	CDialog::OnTimer(nIDEvent);
}
