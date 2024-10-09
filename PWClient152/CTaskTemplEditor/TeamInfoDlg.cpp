// TeamInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TeamInfoDlg.h"
#include "TaskTempl.h"
#include "TaskIDSelDlg.h"
#include "MemInfoDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTeamInfoDlg dialog


CTeamInfoDlg::CTeamInfoDlg(ATaskTempl* pTempl, CWnd* pParent /*=NULL*/)
	: m_pTempl(pTempl), CDialog(CTeamInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTeamInfoDlg)
	m_bRcvByTeam = FALSE;
	m_bSharedTask = FALSE;
	m_bSharedAchieved = FALSE;
	m_bAllFail = FALSE;
	m_bCapFail = FALSE;
	m_bDismAsSelfFail = FALSE;
	m_bCheckTeammate = FALSE;
	m_fDist = 0.0f;
	m_bCntByMemPos = FALSE;
	m_bRcvChckMemPos = FALSE;
	m_fCntByMemDist = 0.0f;
	m_fRcvChckMemDist = 0.0f;
	m_bCapSuccess = FALSE;
	m_fSuccDist = 0.0f;
	m_bAllSuccess = FALSE;
	m_bCoupleOnly = FALSE;
	m_bDistinguishedOcc = FALSE;
	//}}AFX_DATA_INIT
}


void CTeamInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTeamInfoDlg)
	DDX_Control(pDX, IDC_MEM_LIST, m_MemList);
	DDX_Check(pDX, IDC_TEAM_RCV, m_bRcvByTeam);
	DDX_Check(pDX, IDC_SHARE_TASK, m_bSharedTask);
	DDX_Check(pDX, IDC_SHARE_ACQUIRED, m_bSharedAchieved);
	DDX_Check(pDX, IDC_ALL_FAIL, m_bAllFail);
	DDX_Check(pDX, IDC_CAPTAIN_FAIL, m_bCapFail);
	DDX_Check(pDX, IDC_DISMISS_SELF_FAIL, m_bDismAsSelfFail);
	DDX_Check(pDX, IDC_CHECK_TEAMMATE, m_bCheckTeammate);
	DDX_Text(pDX, IDC_DIST, m_fDist);
	DDX_Check(pDX, IDC_CNT_BY_MEM, m_bCntByMemPos);
	DDX_Check(pDX, IDC_RCV_CHCK_MEM, m_bRcvChckMemPos);
	DDX_Text(pDX, IDC_CNT_BY_MEM_DIST, m_fCntByMemDist);
	DDX_Text(pDX, IDC_RCV_MEM_DIST, m_fRcvChckMemDist);
	DDX_Check(pDX, IDC_CAP_SUCCESS, m_bCapSuccess);
	DDX_Text(pDX, IDC_SUCCESS_DIST, m_fSuccDist);
	DDX_Check(pDX, IDC_ALL_SUCCESS, m_bAllSuccess);
	DDX_Check(pDX, IDC_COUPLE_ONLY, m_bCoupleOnly);
	DDX_Check(pDX,IDC_DISTINGUISHED_OCC,m_bDistinguishedOcc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTeamInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CTeamInfoDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeamInfoDlg message handlers

static const char* szRace[] =
{
	"（无）",
	"人族",
	"妖族",
	"汐族",
	"羽族",
	"灵族",
	"胧族"
};

static const char* szOccup[] =
{
	"无",
	"武侠",
	"法师",
	"巫师",
	"妖精",
	"妖兽",
	"刺客",
	"羽芒",
	"羽灵",
	"剑灵",
	"魅灵",
	"夜影",
	"月仙"
};

static const char* szGender[] =
{
	"无",
	"男",
	"女"
};

void CTeamInfoDlg::UpdateRow(int nRow, const TEAM_MEM_WANTED* pInfo)
{
	CString str;

	m_MemList.SetItemText(nRow, 0, szRace[pInfo->m_ulRace]);
	m_MemList.SetItemText(nRow, 1, szOccup[pInfo->m_ulOccupation+1]);
	m_MemList.SetItemText(nRow, 2, szGender[pInfo->m_ulGender]);
	
	str.Format("%d", pInfo->m_ulLevelMin);
	m_MemList.SetItemText(nRow, 3, str);

	str.Format("%d", pInfo->m_ulLevelMax);
	m_MemList.SetItemText(nRow, 4, str);

	str.Format("%d", pInfo->m_ulMinCount);
	m_MemList.SetItemText(nRow, 5, str);

	str.Format("%d", pInfo->m_ulMaxCount);
	m_MemList.SetItemText(nRow, 6, str);

	if (pInfo->m_ulTask) m_MemList.SetItemText(nRow, 7, GetTaskNameByID(pInfo->m_ulTask));
}

BOOL CTeamInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_MemList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_MemList.InsertColumn(0, _T("种族"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(1, _T("职业"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(2, _T("性别"), LVCFMT_LEFT, 40);
	m_MemList.InsertColumn(3, _T("最低级别"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(4, _T("最高级别"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(5, _T("最少人数"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(6, _T("最多人数"), LVCFMT_LEFT, 60);
	m_MemList.InsertColumn(7, _T("队员任务"), LVCFMT_LEFT, 120);

	m_bRcvByTeam		= m_pTempl->m_bRcvByTeam;
	m_bSharedTask		= m_pTempl->m_bSharedTask;
	m_bSharedAchieved	= m_pTempl->m_bSharedAchieved;
	m_bCheckTeammate	= m_pTempl->m_bCheckTeammate;
	m_fDist				= (float)sqrt(m_pTempl->m_fTeammateDist);
	m_bRcvChckMemPos	= m_pTempl->m_bRcvChckMem;
	m_fRcvChckMemDist	= (float)sqrt(m_pTempl->m_fRcvMemDist);
	m_bCntByMemPos		= m_pTempl->m_bCntByMemPos;
	m_fCntByMemDist		= (float)sqrt(m_pTempl->m_fCntMemDist);
	m_bAllFail			= m_pTempl->m_bAllFail;
	m_bCapFail			= m_pTempl->m_bCapFail;
	m_bCapSuccess		= m_pTempl->m_bCapSucc;
	m_fSuccDist			= (float)sqrt(m_pTempl->m_fSuccDist);
	m_bAllSuccess		= m_pTempl->m_bAllSucc;
	m_bDismAsSelfFail	= m_pTempl->m_bDismAsSelfFail;
	m_bCoupleOnly		= m_pTempl->m_bCoupleOnly;
	m_bDistinguishedOcc = m_pTempl->m_bDistinguishedOcc;

	unsigned long i;

	for (i = 0; i < m_pTempl->m_ulTeamMemsWanted; i++)
	{
		const TEAM_MEM_WANTED& tmw = m_pTempl->m_TeamMemsWanted[i];

		m_MemList.InsertItem(i, "");
		UpdateRow(i, &tmw);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTeamInfoDlg::OnAdd()
{
	if (m_pTempl->m_ulTeamMemsWanted >= MAX_TEAM_MEM_WANTED)
	{
		AfxMessageBox("到达最大成员类型数目！", MB_ICONSTOP);
		return;
	}

	TEAM_MEM_WANTED tmw;
	tmw.Init();

	CMemInfoDlg dlg(&tmw);

	if (dlg.DoModal() == IDOK)
	{
		if (!tmw.IsValid())
		{
			AfxMessageBox("请输入有意义属性！");
			return;
		}

		m_MemList.InsertItem(m_pTempl->m_ulTeamMemsWanted, "");
		UpdateRow(m_pTempl->m_ulTeamMemsWanted, &tmw);

		m_pTempl->m_TeamMemsWanted[m_pTempl->m_ulTeamMemsWanted] = tmw;
		m_pTempl->m_ulTeamMemsWanted++;
	}
}

void CTeamInfoDlg::OnDelete()
{
	if (AfxMessageBox("确定吗？", MB_YESNO) != IDYES)
		return;

	POSITION pos = m_MemList.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_MemList.GetNextSelectedItem(pos);
	m_MemList.DeleteItem(nIndex);

	unsigned long i = (unsigned long)nIndex + 1;

	for (; i < m_pTempl->m_ulTeamMemsWanted; i++)
		m_pTempl->m_TeamMemsWanted[i-1] = m_pTempl->m_TeamMemsWanted[i];

	memset(&m_pTempl->m_TeamMemsWanted[i-1], 0, sizeof(TEAM_MEM_WANTED));
	m_pTempl->m_ulTeamMemsWanted--;
}

void CTeamInfoDlg::OnEdit()
{
	POSITION pos = m_MemList.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nIndex = m_MemList.GetNextSelectedItem(pos);
	CMemInfoDlg dlg(&m_pTempl->m_TeamMemsWanted[nIndex]);

	dlg.DoModal();
	UpdateRow(nIndex, &m_pTempl->m_TeamMemsWanted[nIndex]);
}

void CTeamInfoDlg::OnOK()
{
	UpdateData();

	m_pTempl->m_bRcvByTeam		= (m_bRcvByTeam != 0);
	m_pTempl->m_bSharedTask		= (m_bSharedTask != 0);
	m_pTempl->m_bSharedAchieved	= (m_bSharedAchieved != 0);
	m_pTempl->m_bCheckTeammate	= (m_bCheckTeammate != 0);
	m_pTempl->m_fTeammateDist	= (float)pow(m_fDist, 2);
	m_pTempl->m_bRcvChckMem		= (m_bRcvChckMemPos != 0);
	m_pTempl->m_fRcvMemDist		= (float)pow(m_fRcvChckMemDist, 2);
	m_pTempl->m_bCntByMemPos	= (m_bCntByMemPos != 0);
	m_pTempl->m_fCntMemDist		= (float)pow(m_fCntByMemDist, 2);
	m_pTempl->m_bAllFail		= (m_bAllFail != 0);
	m_pTempl->m_bCapFail		= (m_bCapFail != 0);
	m_pTempl->m_bCapSucc		= (m_bCapSuccess != 0);
	m_pTempl->m_fSuccDist		= (float)pow(m_fSuccDist, 2);
	m_pTempl->m_bAllSucc		= (m_bAllSuccess != 0);
	m_pTempl->m_bDismAsSelfFail	= (m_bDismAsSelfFail != 0);
	m_pTempl->m_bCoupleOnly		= (m_bCoupleOnly != 0);
	m_pTempl->m_bDistinguishedOcc = (m_bDistinguishedOcc != 0);
	
	CDialog::OnOK(); 
}
