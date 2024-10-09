// RankingAward.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "RankingAward.h"

#include "TemplIDSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char* _item_path = "BaseData\\物品";
static const char* _task_item_path = "BaseData\\物品\\任务物品";
/////////////////////////////////////////////////////////////////////////////
// CRankingAward dialog


CRankingAward::CRankingAward(RANKING_AWARD* rAward, CWnd* pParent /*=NULL*/)
	: CDialog(CRankingAward::IDD, pParent), m_pRankAward(rAward)
{
	//{{AFX_DATA_INIT(CRankingAward)
		m_iRankStart = 0;
		m_iRankEnd = 0;
		m_lPeriod = 0;
		m_iItemNum = 0;
		m_ulItemID = 0;

		m_bCommonItem = true;
	//}}AFX_DATA_INIT
}


void CRankingAward::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRankingAward)
	DDX_Text(pDX, IDC_RANK_START, m_iRankStart);
	DDX_Text(pDX, IDC_RANK_END, m_iRankEnd);
	DDX_Text(pDX, IDC_ITEM_PERIOD, m_lPeriod);
	DDX_Text(pDX, IDC_ITEM_NUM, m_iItemNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRankingAward, CDialog)
	//{{AFX_MSG_MAP(CRankingAward)
	ON_BN_CLICKED(IDC_SEL_ITEM, OnSelItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRankingAward message handlers

void CRankingAward::OnSelItem() 
{
	CTemplIDSelDlg dlg;
	dlg.m_strRoot = _item_path;
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK)
		return;
	m_ulItemID = dlg.m_ulID;

	CString strPath(_task_item_path);
	strPath.MakeLower();
	strPath += "\\";
	dlg.m_strSelPath.MakeLower();
	m_bCommonItem = dlg.m_strSelPath.Find(strPath) == -1;

	GetDlgItem(IDC_AWARD_ITEM)->SetWindowText(dlg.m_strName);
}

BOOL CRankingAward::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pRankAward)
	{
		m_iRankStart = m_pRankAward->m_iRankingStart;
		m_iRankEnd	 = m_pRankAward->m_iRankingEnd;
		m_lPeriod 	 = m_pRankAward->m_lPeriod;
		m_iItemNum	 = m_pRankAward->m_ulAwardItemNum;
		m_ulItemID	 = m_pRankAward->m_ulAwardItemId;
	}

	if(m_ulItemID)
		GetDlgItem(IDC_AWARD_ITEM)->SetWindowText(GetBaseDataNameByID(m_ulItemID));

		
	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRankingAward::OnOK() 
{	
	CDialog::OnOK();
}
