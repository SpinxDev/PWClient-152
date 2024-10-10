// DlgSelectSoundDef.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSelectSoundDef.h"

using AudioEngine::SoundDef;

// CDlgSelectSoundDef 对话框

IMPLEMENT_DYNAMIC(CDlgSelectSoundDef, CBCGPDialog)

CDlgSelectSoundDef::CDlgSelectSoundDef(bool bMultiSelect, const char* szTitle /*= ""*/, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSelectSoundDef::IDD, pParent)
{
	m_bMultiSelect = bMultiSelect;
	m_csTitle = szTitle;
}

CDlgSelectSoundDef::~CDlgSelectSoundDef()
{
}

void CDlgSelectSoundDef::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SOUNDDEF, m_wndTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectSoundDef, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectSoundDef::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_SOUNDDEF, &CDlgSelectSoundDef::OnNMDblclkTreeSounddef)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgSelectSoundDef 消息处理程序

BOOL CDlgSelectSoundDef::OnInitDialog()
{
	EnableVisualManagerStyle();
	CBCGPDialog::OnInitDialog();

	if(!m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 6))
		return -1;
	m_wndTree.SetImageList(&m_ImageList, TVSIL_NORMAL);	

	m_wndTree.EnableMultiSelect(m_bMultiSelect);
	if(!m_soundDefDisplay.BuildImageList(m_ImageList))
		return FALSE;
	if(!m_soundDefDisplay.UpdateTree(&m_wndTree))
		return FALSE;

	m_dscMan.Add(DynSizeCtrl(&m_wndTree, this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDOK), this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDCANCEL), this));

	if(m_csTitle.GetLength() != 0)
		SetWindowText(m_csTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSelectSoundDef::OnBnClickedOk()
{
	m_listSelectSoundDef.clear();
	int iItemNum = m_wndTree.GetSelectedItemNum();
	for (int i=0; i<iItemNum; ++i)
	{
		HTREEITEM hItem = m_wndTree.GetSelectedItemByIndex(i);
		if(!hItem)
			continue;
		SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
		if(type != SOUNDDEF_ITEM_SOUNDDEF)
			continue;
		m_listSelectSoundDef.push_back((SoundDef*)m_wndTree.GetItemData(hItem));
	}
	if(m_listSelectSoundDef.size() == 0)
		goto _failed;
	CBCGPDialog::OnOK();
	return;
_failed:
	MessageBox("请选择声音定义");
}

void CDlgSelectSoundDef::OnNMDblclkTreeSounddef(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		goto _failed;
	SOUNDDEF_ITEM_TYPE type = (SOUNDDEF_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
	if(type != SOUNDDEF_ITEM_SOUNDDEF)
		goto _failed;	
	SoundDef* pSelectSoundDef = (SoundDef*)m_wndTree.GetItemData(hItem);
	if(!pSelectSoundDef)
		goto _failed;
	m_listSelectSoundDef.clear();
	m_listSelectSoundDef.push_back(pSelectSoundDef);
	CBCGPDialog::OnOK();
	*pResult = 0;
	return;
_failed:
	MessageBox("请选择一个声音定义");
	*pResult = 0;
}

void CDlgSelectSoundDef::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	m_dscMan.Resize(cx, cy);
}
