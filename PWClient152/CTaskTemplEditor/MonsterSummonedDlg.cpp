// MonsterSummonedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "MonsterSummonedDlg.h"

#include "NumDlg.h"
#include "TemplIDSelDlg.h"
#include "FloatDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonsterSummonedDlg dialog

static const char* _monster_path = "BaseData";

// CMonsterSummonedDlg::CMonsterSummonedDlg(unsigned long ulMaxCount, MONSTERS_SUMMONED* pMonsters, 
// 						unsigned long* pCount,CWnd* pParent /*=NULL*/)
// 	:m_ulMaxCount(ulMaxCount),
// 	m_pMonsters(pMonsters),
// 	m_pCount(pCount),
// 	CDialog(CMonsterSummonedDlg::IDD, pParent)
// {
// 	//{{AFX_DATA_INIT(CMonsterSummonedDlg)
// 	m_bRandSel = FALSE;
// 	m_bDisappear = FALSE;
// 	m_iRadius = 0;
// 	m_fTotalProb = 0.0f;
// 	//}}AFX_DATA_INIT
//}

CMonsterSummonedDlg::CMonsterSummonedDlg(AWARD_DATA* pAward, CWnd* pParent /*=NULL*/)
	:m_pAward(pAward),
	CDialog(CMonsterSummonedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterSummonedDlg)
	m_ulMaxCount = MAX_MONSTER_SUMMONED;
	m_pMonsters = m_pAward->m_SummonedMonsters->m_Monsters;
	m_pCount = &(m_pAward->m_SummonedMonsters->m_ulMonsterNum);

	m_bRandSel = m_pAward->m_SummonedMonsters->m_bRandChoose;
	m_bDisappear = m_pAward->m_SummonedMonsters->m_bDeathDisappear;
	m_iRadius = m_pAward->m_SummonedMonsters->m_ulSummonRadius;
	m_fTotalProb = 0.0f;
	//}}AFX_DATA_INIT
}

void CMonsterSummonedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterSummonedDlg)
	DDX_Control(pDX, IDC_MONSTERS, m_MonstersLst);
	DDX_Check(pDX, IDC_RAND_SEL, m_bRandSel);
	DDX_Check(pDX, IDC_DISAPPEAR, m_bDisappear);
	DDX_Text(pDX, IDC_RADIUS, m_iRadius);
	DDX_Text(pDX, IDC_TOTAL_PROB, m_fTotalProb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonsterSummonedDlg, CDialog)
	//{{AFX_MSG_MAP(CMonsterSummonedDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_NUM, OnEditNum)
	ON_BN_CLICKED(IDC_RAND_SEL, OnRandSel)
	ON_BN_CLICKED(IDC_PROB, OnProb)
	ON_BN_CLICKED(IDC_PERIOD, OnPeriod)
	ON_BN_CLICKED(IDC_DISAPPEAR, OnDisappear)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterSummonedDlg message handlers


BOOL CMonsterSummonedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_MonstersLst.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_MonstersLst.InsertColumn(0, _T("怪物或物品名称"), LVCFMT_LEFT, 110);
	m_MonstersLst.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 80);
	m_MonstersLst.InsertColumn(2, _T("数量"), LVCFMT_LEFT, 80);
	m_MonstersLst.InsertColumn(3, _T("出现概率"), LVCFMT_LEFT, 80);
	m_MonstersLst.InsertColumn(4, _T("有效时间"), LVCFMT_LEFT, 80);

	unsigned char i;
	for (i = 0; i < *m_pCount; i++)
	{
		MONSTERS_SUMMONED& ms = m_pMonsters[i];

		m_MonstersLst.InsertItem(
			i,
			GetBaseDataNameByID(ms.m_ulMonsterTemplId));

		CString strID;
		strID.Format("%d", ms.m_ulMonsterTemplId);
		m_MonstersLst.SetItemText(i, 1, strID);

		CString strNum;
		strNum.Format("%d", ms.m_ulMonsterNum);
		m_MonstersLst.SetItemText(i, 2, strNum);

		CString strProb;
		strProb.Format("%f", ms.m_fSummonProb);
		m_MonstersLst.SetItemText(i, 3, strProb);

		m_fTotalProb += ms.m_fSummonProb;

		CString strPeriod;
		strPeriod.Format("%d", ms.m_lPeriod);
		m_MonstersLst.SetItemText(i, 4, strPeriod);
	}

	if(m_fTotalProb > 1.0f && m_bRandSel)
	{
		for (i = 0; i < *m_pCount; i++)
		{
			MONSTERS_SUMMONED& ms = m_pMonsters[i];
			ms.m_fSummonProb /= m_fTotalProb;

			CString strProb;
			strProb.Format("%f", ms.m_fSummonProb);
			m_MonstersLst.SetItemText(i, 3, strProb);
		}

		m_fTotalProb = 1.0f;
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonsterSummonedDlg::OnAdd() 
{
	if (*m_pCount >= m_ulMaxCount)
	{
		AfxMessageBox("怪物数量已到达上限", MB_ICONSTOP);
		return;
	}

	CTemplIDSelDlg dlg;
	dlg.m_strRoot = _monster_path;
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK)
		return;

	for (unsigned char i = 0; i < *m_pCount; i++)
	{
		if (m_pMonsters[i].m_ulMonsterTemplId == dlg.m_ulID)
		{
			AfxMessageBox("已有此怪物", MB_ICONSTOP);
			return;
		}
	}
	
	CNumDlg dlgNum("输入怪物数量");

	if (dlgNum.DoModal() != IDOK)
		return;

	FloatDlg dlgProb;

	if (dlgProb.DoModal() != IDOK)
		return;

	unsigned char uIndex = *m_pCount;
	(*m_pCount)++;
	MONSTERS_SUMMONED& ms = m_pMonsters[uIndex];

	ms.m_ulMonsterTemplId = dlg.m_ulID;
	CString strPath(_monster_path);
	strPath.MakeLower();
	strPath += "\\";
	dlg.m_strSelPath.MakeLower();

	ms.m_ulMonsterNum = dlgNum.m_nNum;
	ms.m_fSummonProb = dlgProb.m_fVal;
	ms.m_lPeriod = 0;

	m_MonstersLst.InsertItem(uIndex, GetBaseDataNameByID(ms.m_ulMonsterTemplId));
	CString str;
	str.Format("%u", ms.m_ulMonsterTemplId);
	m_MonstersLst.SetItemText(uIndex, 1, str);
	str.Format("%u", ms.m_ulMonsterNum);
	m_MonstersLst.SetItemText(uIndex, 2, str);
	str.Format("%f", ms.m_fSummonProb);
	m_MonstersLst.SetItemText(uIndex, 3, str);
	m_MonstersLst.SetItemText(uIndex, 4, "0");
}

void CMonsterSummonedDlg::OnDel() 
{
	POSITION pos = m_MonstersLst.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned char uItem = (unsigned char)m_MonstersLst.GetNextSelectedItem(pos);

	for (unsigned char i = uItem+1; i < *m_pCount; i++)
		m_pMonsters[i-1] = m_pMonsters[i];

	(*m_pCount)--;
	m_MonstersLst.DeleteItem(uItem);
	
}

void CMonsterSummonedDlg::OnEditNum() 
{
	POSITION pos = m_MonstersLst.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_MonstersLst.GetNextSelectedItem(pos);
	
	MONSTERS_SUMMONED& ms = m_pMonsters[uItem];
	CNumDlg dlgNum("输入怪物数量");
	dlgNum.m_nNum = ms.m_ulMonsterNum;
	if (dlgNum.DoModal() != IDOK) return;
	ms.m_ulMonsterNum = dlgNum.m_nNum;

	CString str;
	str.Format("%d", ms.m_ulMonsterNum);
	m_MonstersLst.SetItemText(uItem, 2, str);
	
}

void CMonsterSummonedDlg::OnRandSel() 
{
	UpdateData();
}

void CMonsterSummonedDlg::OnProb() 
{
	POSITION pos = m_MonstersLst.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nItem = m_MonstersLst.GetNextSelectedItem(pos);
	
	MONSTERS_SUMMONED& ms = m_pMonsters[nItem];

	FloatDlg dlg;
	dlg.m_fVal = ms.m_fSummonProb;

	if (dlg.DoModal() != IDOK) return;

	ms.m_fSummonProb = dlg.m_fVal;

	CString str;
	str.Format("%f", ms.m_fSummonProb);
	m_MonstersLst.SetItemText(nItem, 3, str);	
}

void CMonsterSummonedDlg::OnPeriod() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_MonstersLst.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_MonstersLst.GetNextSelectedItem(pos);

	MONSTERS_SUMMONED& ms = m_pMonsters[uItem];
	CNumDlg dlgNum;
	dlgNum.m_nNum = ms.m_lPeriod;
	if (dlgNum.DoModal() != IDOK) return;
	ms.m_lPeriod = dlgNum.m_nNum;
	CString str;
	str.Format("%d", ms.m_lPeriod);
	m_MonstersLst.SetItemText(uItem, 4, str);
}

void CMonsterSummonedDlg::OnDisappear() 
{
	UpdateData();
}

void CMonsterSummonedDlg::OnClose() 
{	
	UpdateData();
	m_pAward->m_SummonedMonsters->m_bRandChoose = (m_bRandSel != 0);
	m_pAward->m_SummonedMonsters->m_bDeathDisappear = (m_bDisappear != 0);
	m_pAward->m_SummonedMonsters->m_ulSummonRadius = m_iRadius;
	m_pAward->m_ulSummonedMonsters = *m_pCount;
	
	CDialog::OnClose();
}
