// DlgSelectEvent.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSelectEvent.h"
#include "Engine.h"
#include "Project.h"

using AudioEngine::Event;
// CDlgSelectEvent dialog

IMPLEMENT_DYNAMIC(CDlgSelectEvent, CBCGPDialog)

CDlgSelectEvent::CDlgSelectEvent(bool bMultiSelect, const char* szTitle /*= ""*/, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSelectEvent::IDD, pParent)
	, m_bMultiSelect(bMultiSelect)
	, m_csTitle(szTitle)
{
}

CDlgSelectEvent::~CDlgSelectEvent()
{
}

void CDlgSelectEvent::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_EVENT, m_wndTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectEvent, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectEvent::OnBnClickedOk)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CDlgSelectEvent::OnInitDialog()
{
	EnableVisualManagerStyle();
	CBCGPDialog::OnInitDialog();

	if(!m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 6))
		return -1;
	m_wndTree.SetImageList(&m_ImageList, TVSIL_NORMAL);	

	m_wndTree.EnableMultiSelect(m_bMultiSelect);
	if(!m_eventDisplay.BuildImageList(m_ImageList))
		return FALSE;
	if(!m_eventDisplay.UpdateTree(&m_wndTree, Engine::GetInstance().GetCurProject()->GetEventManager()))
		return FALSE;

	m_dscMan.Add(DynSizeCtrl(&m_wndTree, this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDOK), this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDCANCEL), this));

	if(m_csTitle.GetLength() != 0)
		SetWindowText(m_csTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSelectEvent::OnBnClickedOk()
{
	m_listSelectEvent.clear();
	int iItemNum = m_wndTree.GetSelectedItemNum();
	for (int i=0; i<iItemNum; ++i)
	{
		HTREEITEM hItem = m_wndTree.GetSelectedItemByIndex(i);
		if(!hItem)
			continue;
		EVENT_ITEM_TYPE type = (EVENT_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
		if(type != EVENT_ITEM_EVENT)
			continue;
		m_listSelectEvent.push_back((Event*)m_wndTree.GetItemData(hItem));
	}
	if(m_listSelectEvent.size() == 0)
		goto _failed;
	CBCGPDialog::OnOK();
	return;
_failed:
	MessageBox("请选择Event");
}

void CDlgSelectEvent::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	m_dscMan.Resize(cx, cy);
}

