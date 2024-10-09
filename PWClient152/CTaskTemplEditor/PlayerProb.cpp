// PlayerProb.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "PlayerProb.h"
#include "TaskTempl.h"
#include "TemplIDSelDlg.h"
#include "PlayerRequirement.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlayerProb dialog


PlayerProb::PlayerProb(PLAYER_WANTED* pPlayer, unsigned long ulIndex, CWnd* pParent /*=NULL*/)
	: m_pPlayerWanted(pPlayer),
	m_ulIndex(ulIndex),
	CDialog(PlayerProb::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlayerProb)
	m_ItemNum = 0;
	m_DropRate = 0.0f;
	m_ItemName = _T("");
//	m_PlayerClass = _T("");
	m_ulPlayerNum = 0;
	//}}AFX_DATA_INIT
}


void PlayerProb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlayerProb)
	DDX_Text(pDX, IDC_ITEM_NUM, m_ItemNum);
	DDV_MinMaxInt(pDX, m_ItemNum, 0, 999);
	DDX_Text(pDX, IDC_DROP_PROB, m_DropRate);
	DDV_MinMaxFloat(pDX, m_DropRate, 0.f, 1.f);
	DDX_Text(pDX, IDC_ITEM, m_ItemName);
//	DDX_Text(pDX, IDC_PLAYER, m_PlayerClass);
	DDX_Text(pDX, IDC_PLAYER_NUM, m_ulPlayerNum);
	DDV_MinMaxUInt(pDX, m_ulPlayerNum, 0, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PlayerProb, CDialog)
	//{{AFX_MSG_MAP(PlayerProb)
	ON_BN_CLICKED(IDC_SEL_PLAYER, OnSelPlayer)
	ON_BN_CLICKED(IDC_SEL_ITEM, OnSelItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayerProb message handlers

void PlayerProb::OnSelPlayer() 
{
	// TODO: Add your control notification handler code here
	PlayerRequirement dlg(&m_pPlayerWanted->m_Requirements);
	dlg.DoModal();
}

void PlayerProb::OnSelItem() 
{
	// TODO: Add your control notification handler code here
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = "BaseData\\物品";
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = m_pPlayerWanted->m_ulDropItemId;
	
	if (dlg.DoModal() != IDOK) return;
	
	m_pPlayerWanted->m_ulDropItemId = dlg.m_ulID;
	m_pPlayerWanted->m_bDropCmnItem = (dlg.m_strSelPath.Find("任务物品\\") == -1);
	
	GetDlgItem(IDC_ITEM)->SetWindowText(dlg.m_strName);	
}

BOOL PlayerProb::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_PlayerClass	= m_ulIndex;
	m_ulPlayerNum	= m_pPlayerWanted->m_ulPlayerNum;
	m_ItemName		= GetBaseDataNameByID(m_pPlayerWanted->m_ulDropItemId);
	m_ItemNum		= m_pPlayerWanted->m_ulDropItemCount;
	m_DropRate		= m_pPlayerWanted->m_fDropProb;
	CString str;
	str.Format("%d",m_ulIndex + 1);
	GetDlgItem(IDC_PLAYER)->SetWindowText(str);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PlayerProb::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	m_pPlayerWanted->m_ulPlayerNum = m_ulPlayerNum;
	m_pPlayerWanted->m_ulDropItemCount = m_ItemNum;
	m_pPlayerWanted->m_fDropProb = m_DropRate;

	CDialog::OnOK();
}
