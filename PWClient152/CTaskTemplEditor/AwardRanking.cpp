// AwardRanking.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "AwardRanking.h"

#include "RankingAward.h"
#include "TemplIDSelDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAwardRanking dialog

static const char* _type[] =
{
	"普通物品",
	"任务物品"
};

CAwardRanking::CAwardRanking(AWARD_DATA* pAward, CWnd* pParent /*=NULL*/)
	: CDialog(CAwardRanking::IDD, pParent), 
	m_pAward(pAward)
{
	//{{AFX_DATA_INIT(CAwardRanking)
	m_bAwardByProf = FALSE;
	m_ulPQRankingAwardCnt = 0;
	//}}AFX_DATA_INIT
}


void CAwardRanking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardRanking)
	DDX_Control(pDX, IDC_RANKING_LIST, m_RankingList);
	DDX_Check(pDX, IDC_AWARD_BY_PROF, m_bAwardByProf);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardRanking, CDialog)
	//{{AFX_MSG_MAP(CAwardRanking)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnDel)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwardRanking message handlers

void CAwardRanking::OnAdd() 
{
	if (m_RankingList.GetItemCount() >= MAX_AWARD_PQ_RANKING)
	{
		AfxMessageBox("PQ子任务奖励最大排名数已到达上限", MB_ICONSTOP);
		return;
	}

	CRankingAward dlg;
	if(dlg.DoModal() != IDOK)
		return;

	unsigned char uIndex = m_ulPQRankingAwardCnt;
	m_ulPQRankingAwardCnt++;
	RANKING_AWARD& ra = m_pAward->m_PQRankingAward->m_RankingAward[uIndex];
	
	ra.m_ulAwardItemId = dlg.m_ulItemID;
	ra.m_iRankingStart = dlg.m_iRankStart;
	ra.m_iRankingEnd = dlg.m_iRankEnd;
	ra.m_bCommonItem = dlg.m_bCommonItem;
	ra.m_ulAwardItemNum = dlg.m_iItemNum;
	ra.m_lPeriod = dlg.m_lPeriod;


	CString str;
	str.Format("%d", uIndex+1);
	m_RankingList.InsertItem(uIndex, str);

	str.Format("%d - %d", ra.m_iRankingStart, ra.m_iRankingEnd);
	m_RankingList.SetItemText(uIndex, 1, str);

	m_RankingList.SetItemText(uIndex, 2, GetBaseDataNameByID(ra.m_ulAwardItemId));

	str.Format("%d", ra.m_ulAwardItemId);
	m_RankingList.SetItemText(uIndex, 3, str);

	m_RankingList.SetItemText(uIndex, 4, ra.m_bCommonItem ? _type[0] : _type[1]);

	str.Format("%d", ra.m_ulAwardItemNum);
	m_RankingList.SetItemText(uIndex, 5, str);

	str.Format("%d", ra.m_lPeriod);
	m_RankingList.SetItemText(uIndex, 6, str);

	SortRankingList();
}

void CAwardRanking::OnEdit() 
{
	POSITION pos = m_RankingList.GetFirstSelectedItemPosition();
	if (!pos) 
		return;

 	unsigned char uItem = (unsigned char)m_RankingList.GetNextSelectedItem(pos);
 	RANKING_AWARD& ra = m_pAward->m_PQRankingAward->m_RankingAward[uItem];
	
 	CRankingAward dlg(&ra);
 	if(dlg.DoModal() != IDOK)
 		return;
	
	ra.m_ulAwardItemId = dlg.m_ulItemID;
	ra.m_iRankingStart = dlg.m_iRankStart;
	ra.m_iRankingEnd = dlg.m_iRankEnd;
	ra.m_bCommonItem = dlg.m_bCommonItem;
	ra.m_ulAwardItemNum = dlg.m_iItemNum;
	ra.m_lPeriod = dlg.m_lPeriod;

	CString str;

	str.Format("%d - %d", ra.m_iRankingStart, ra.m_iRankingEnd);
	m_RankingList.SetItemText(uItem, 1, str);

	m_RankingList.SetItemText(uItem, 2, GetBaseDataNameByID(ra.m_ulAwardItemId));

	str.Format("%d", ra.m_ulAwardItemId);
	m_RankingList.SetItemText(uItem, 3, str);

	m_RankingList.SetItemText(uItem, 4, ra.m_bCommonItem ? _type[0] : _type[1]);

	str.Format("%d", ra.m_ulAwardItemNum);
	m_RankingList.SetItemText(uItem, 5, str);

	str.Format("%d", ra.m_lPeriod);
	m_RankingList.SetItemText(uItem, 6, str);

	SortRankingList();
}

void CAwardRanking::OnDel() 
{
	POSITION pos = m_RankingList.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned char uItem = (unsigned char)m_RankingList.GetNextSelectedItem(pos);

	CString str;
	for (unsigned char i = uItem+1; i < m_ulPQRankingAwardCnt; i++)
	{
		m_pAward->m_PQRankingAward->m_RankingAward[i-1] = m_pAward->m_PQRankingAward->m_RankingAward[i];
		str.Format("%d", i);
		m_RankingList.SetItemText(i, 0, str);
	}

	m_ulPQRankingAwardCnt--;
	m_RankingList.DeleteItem(uItem);

	SortRankingList();
}

BOOL CAwardRanking::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bAwardByProf = m_pAward->m_PQRankingAward->m_bAwardByProf;
	m_ulPQRankingAwardCnt = m_pAward->m_ulPQRankingAwardCnt;

	m_RankingList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_RankingList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
	m_RankingList.InsertColumn(1, _T("排名范围"), LVCFMT_LEFT, 80);
	m_RankingList.InsertColumn(2, _T("物品名称"), LVCFMT_LEFT, 110);
	m_RankingList.InsertColumn(3, _T("物品ID"), LVCFMT_LEFT, 80);
	m_RankingList.InsertColumn(4, _T("物品类型"), LVCFMT_LEFT, 80);
	m_RankingList.InsertColumn(5, _T("数量"), LVCFMT_LEFT, 80);
	m_RankingList.InsertColumn(6, _T("有效时间"), LVCFMT_LEFT, 80);

	SortRankingList();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardRanking::OnClose() 
{
	SortRankingList();

	for(int i=0;i<m_ulPQRankingAwardCnt-1;i++)
	{
		if(m_pAward->m_PQRankingAward->m_RankingAward[i].m_iRankingEnd +1 != m_pAward->m_PQRankingAward->m_RankingAward[i+1].m_iRankingStart)
		{
			char temp[260];
 			sprintf(temp, "排名奖励有名次重叠或遗漏！\n\n出错的行数：%d, %d", i+1, i+2);
			AfxMessageBox(temp, MB_ICONSTOP);
			return;
		}		
	}

	UpdateData();
	m_pAward->m_PQRankingAward->m_bAwardByProf = (m_bAwardByProf != 0);
	m_pAward->m_PQRankingAward->m_ulRankingAwardNum = m_ulPQRankingAwardCnt;
	m_pAward->m_ulPQRankingAwardCnt = m_ulPQRankingAwardCnt;
		
	CDialog::OnClose();
}

void CAwardRanking::SortRankingList()
{
	int i(0);
	for(i=0;i<m_ulPQRankingAwardCnt;i++)
	{
		RANKING_AWARD& rai = m_pAward->m_PQRankingAward->m_RankingAward[i];
		for(int j=i+1;j<m_ulPQRankingAwardCnt;j++)
		{
			RANKING_AWARD& raj = m_pAward->m_PQRankingAward->m_RankingAward[j];

			if(rai.m_iRankingStart > raj.m_iRankingStart)
			{
				RANKING_AWARD temp = rai;
				rai = raj;
				raj = temp;
			}			
		}
	}

	for (i=0;i<m_ulPQRankingAwardCnt;i++)
		m_RankingList.DeleteItem(0);

	for (i = 0; i < m_ulPQRankingAwardCnt; i++)
	{
		RANKING_AWARD& pr = m_pAward->m_PQRankingAward->m_RankingAward[i];

		CString strCnt;
		strCnt.Format("%d", i+1);
		m_RankingList.InsertItem(i, strCnt);

		CString strRank;
		strRank.Format("%d - %d", pr.m_iRankingStart, pr.m_iRankingEnd);
		m_RankingList.SetItemText(i, 1, strRank);

		m_RankingList.SetItemText(i, 2, GetBaseDataNameByID(pr.m_ulAwardItemId));

		CString strID;
		strID.Format("%d", pr.m_ulAwardItemId);
		m_RankingList.SetItemText(i, 3, strID);

		m_RankingList.SetItemText(i, 4, pr.m_bCommonItem ? _type[0] : _type[1]);

		CString strNum;
		strNum.Format("%d", pr.m_ulAwardItemNum);
		m_RankingList.SetItemText(i, 5, strNum);

		CString strPeriod;
		strPeriod.Format("%d", pr.m_lPeriod);
		m_RankingList.SetItemText(i, 6, strPeriod);
 	}
}