// DlgSelectAudio.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSelectAudio.h"
#include "FAudio.h"

// CDlgSelectAudio 对话框

IMPLEMENT_DYNAMIC(CDlgSelectAudio, CBCGPDialog)

CDlgSelectAudio::CDlgSelectAudio(bool bMultiSelect, const char* szTitle /*= ""*/, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSelectAudio::IDD, pParent)
	, m_bMultiSelect(bMultiSelect)
	, m_csTitle(szTitle)
{
}

CDlgSelectAudio::~CDlgSelectAudio()
{
}

void CDlgSelectAudio::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BANK, m_wndTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectAudio, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectAudio::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_BANK, &CDlgSelectAudio::OnNMDblclkTreeBank)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgSelectAudio 消息处理程序

BOOL CDlgSelectAudio::OnInitDialog()
{
	EnableVisualManagerStyle();
	CBCGPDialog::OnInitDialog();
	m_wndTree.EnableMultiSelect(m_bMultiSelect);

	if(!m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 6))
		return -1;
	m_wndTree.SetImageList(&m_ImageList, TVSIL_NORMAL);	

	if(!m_bankDisplay.BuildImageList(m_ImageList))
		return FALSE;
	if(!m_bankDisplay.UpdateTree(&m_wndTree))
		return FALSE;

	m_dscMan.Add(DynSizeCtrl(&m_wndTree, this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDOK), this));
	m_dscMan.Add(DynSizeCtrl(GetDlgItem(IDCANCEL), this));

	if(m_csTitle.GetLength() != 0)
		SetWindowText(m_csTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSelectAudio::OnBnClickedOk()
{
	m_listSelectAudio.clear();
	int iItemNum = m_wndTree.GetSelectedItemNum();
	for (int i=0; i<iItemNum; ++i)
	{
		HTREEITEM hItem = m_wndTree.GetSelectedItemByIndex(i);
		if(!hItem)
			continue;
		BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
		if(type != BANK_ITEM_AUDIO)
			continue;
		m_listSelectAudio.push_back((Audio*)m_wndTree.GetItemData(hItem));
	}
	if(m_listSelectAudio.size() == 0)
		goto _failed;
	CBCGPDialog::OnOK();
	return;
_failed:
	MessageBox("请选择至少一个音频文件");
}

void CDlgSelectAudio::OnNMDblclkTreeBank(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_listSelectAudio.clear();
	int iItemNum = m_wndTree.GetSelectedItemNum();
	for (int i=0; i<iItemNum; ++i)
	{
		HTREEITEM hItem = m_wndTree.GetSelectedItemByIndex(i);
		if(!hItem)
			continue;
		BANK_ITEM_TYPE type = (BANK_ITEM_TYPE)m_wndTree.GetExtraData(hItem);
		if(type != BANK_ITEM_AUDIO)
			continue;
		m_listSelectAudio.push_back((Audio*)m_wndTree.GetItemData(hItem));
	}
	if(m_listSelectAudio.size() == 0)
		goto _failed;
	CBCGPDialog::OnOK();
	*pResult = 0;
	return;
_failed:
	MessageBox("请选择至少一个音频文件");
	*pResult = 0;
}

void CDlgSelectAudio::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	m_dscMan.Resize(cx, cy);
}
