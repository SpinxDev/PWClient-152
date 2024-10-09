// PQSubTaskProp.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "PQSubTaskProp.h"

#include "NumDlg.h"
#include "TemplIDSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPQSubTaskProp dialog
static const char* _monster_path = "BaseData";

CPQSubTaskProp::CPQSubTaskProp(ATaskTempl* pTempl, CWnd* pParent /*=NULL*/)
	: CDialog(CPQSubTaskProp::IDD, pParent), m_pTempl(pTempl)
{
	//{{AFX_DATA_INIT(CPQSubTaskProp)
	m_bClearContrib = pTempl->m_bClearContrib;
	m_ulContribMonsterNum = pTempl->m_ulMonsterContribCnt;
	m_pMonsters = pTempl->m_MonstersContrib;

	//}}AFX_DATA_INIT
}


void CPQSubTaskProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPQSubTaskProp)
	DDX_Control(pDX, IDC_MONSTER_CONTRIB, m_MonsterContribList);
	DDX_Check(pDX, IDC_CLEAR_CONTRIB, m_bClearContrib);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPQSubTaskProp, CDialog)
	//{{AFX_MSG_MAP(CPQSubTaskProp)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPQSubTaskProp message handlers

void CPQSubTaskProp::OnAdd() 
{
	if (m_ulContribMonsterNum >= MAX_CONTRIB_MONSTERS)
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

	for (unsigned char i = 0; i < m_ulContribMonsterNum; i++)
	{
		if (m_pMonsters[i].m_ulMonsterTemplId == dlg.m_ulID)
		{
			AfxMessageBox("已有此怪物", MB_ICONSTOP);
			return;
		}
	}
	
	CNumDlg dlgWhole("输入组队间独享贡献度");

	if (dlgWhole.DoModal() != IDOK)
		return;

	CNumDlg dlgShare("输入组队间分享贡献度");

	if (dlgShare.DoModal() != IDOK)
		return;

	CNumDlg dlgPersonal("输入个人独享贡献度");
	if (dlgPersonal.DoModal() !=IDOK)
		return;

	unsigned char uIndex = m_ulContribMonsterNum;
	m_ulContribMonsterNum++;
	MONSTERS_CONTRIB& mc = m_pMonsters[uIndex];

	mc.m_ulMonsterTemplId = dlg.m_ulID;
	CString strPath(_monster_path);
	strPath.MakeLower();
	strPath += "\\";
	dlg.m_strSelPath.MakeLower();

	mc.m_iWholeContrib = dlgWhole.m_nNum;
	mc.m_iShareContrib = dlgShare.m_nNum;
	mc.m_iPersonalWholeContrib = dlgPersonal.m_nNum;
	

	m_MonsterContribList.InsertItem(uIndex, GetBaseDataNameByID(mc.m_ulMonsterTemplId));
	CString str;
	str.Format("%u", mc.m_ulMonsterTemplId);
	m_MonsterContribList.SetItemText(uIndex, 1, str);
	str.Format("%d", mc.m_iWholeContrib);
	m_MonsterContribList.SetItemText(uIndex, 2, str);
	str.Format("%d", mc.m_iShareContrib);
	m_MonsterContribList.SetItemText(uIndex, 3, str);
	str.Format("%d",mc.m_iPersonalWholeContrib);
	m_MonsterContribList.SetItemText(uIndex, 4, str);
	
}

BOOL CPQSubTaskProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_MonsterContribList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_MonsterContribList.InsertColumn(0, _T("怪物名称"), LVCFMT_LEFT, 110);
	m_MonsterContribList.InsertColumn(1, _T("怪物ID"), LVCFMT_LEFT, 80);
	m_MonsterContribList.InsertColumn(2, _T("组队间独享贡献度"), LVCFMT_LEFT, 80);
	m_MonsterContribList.InsertColumn(3, _T("组队间共享贡献度"), LVCFMT_LEFT, 80);
	m_MonsterContribList.InsertColumn(4, _T("个人独享贡献度"), LVCFMT_LEFT, 80);

	unsigned char i;
	for (i = 0; i < m_ulContribMonsterNum; i++)
	{
		MONSTERS_CONTRIB& mc = m_pMonsters[i];

		m_MonsterContribList.InsertItem(
			i,
			GetBaseDataNameByID(mc.m_ulMonsterTemplId));

		CString strID;
		strID.Format("%d", mc.m_ulMonsterTemplId);
		m_MonsterContribList.SetItemText(i, 1, strID);

		CString strWhole;
		strWhole.Format("%d", mc.m_iWholeContrib);
		m_MonsterContribList.SetItemText(i, 2, strWhole);

		CString strShare;
		strShare.Format("%d", mc.m_iShareContrib);
		m_MonsterContribList.SetItemText(i, 3, strShare);

		CString strPersonal;
		strPersonal.Format("%d",mc.m_iPersonalWholeContrib);
		m_MonsterContribList.SetItemText(i,4,strPersonal);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPQSubTaskProp::OnDel() 
{
	POSITION pos = m_MonsterContribList.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned char uItem = (unsigned char)m_MonsterContribList.GetNextSelectedItem(pos);

	for (unsigned char i = uItem+1; i < m_ulContribMonsterNum; i++)
		m_pMonsters[i-1] = m_pMonsters[i];

	m_ulContribMonsterNum--;
	m_MonsterContribList.DeleteItem(uItem);
}

void CPQSubTaskProp::OnEdit() 
{	
	POSITION pos = m_MonsterContribList.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_MonsterContribList.GetNextSelectedItem(pos);
	
	MONSTERS_CONTRIB& mc = m_pMonsters[uItem];
	CNumDlg dlgWhole("修改组队间独享贡献度");
	dlgWhole.m_nNum = mc.m_iWholeContrib;
	if (dlgWhole.DoModal() != IDOK) return;
	mc.m_iWholeContrib= dlgWhole.m_nNum;

	CNumDlg dlgShare("修改组队间分享贡献度");
	dlgShare.m_nNum = mc.m_iShareContrib;
	if (dlgShare.DoModal() != IDOK) return;
	mc.m_iShareContrib= dlgShare.m_nNum;

	CNumDlg dlgPersonal("修改个人独享贡献度");
	dlgPersonal.m_nNum = mc.m_iPersonalWholeContrib;
	if(dlgPersonal.DoModal() !=IDOK) return;
	mc.m_iPersonalWholeContrib = dlgPersonal.m_nNum;

	CString str;
	str.Format("%d", mc.m_iWholeContrib);
	m_MonsterContribList.SetItemText(uItem, 2, str);
	
	str.Format("%d", mc.m_iShareContrib);
	m_MonsterContribList.SetItemText(uItem, 3, str);

	str.Format("%d",mc.m_iPersonalWholeContrib);
	m_MonsterContribList.SetItemText(uItem,4,str);
}

void CPQSubTaskProp::OnClose() 
{
	UpdateData();

	m_pTempl->m_bClearContrib = (m_bClearContrib != 0);
	m_pTempl->m_ulMonsterContribCnt = m_ulContribMonsterNum;
	m_pTempl->m_MonstersContrib = m_pMonsters;
	
	CDialog::OnClose();
}
