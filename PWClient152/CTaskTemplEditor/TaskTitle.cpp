// TaskTitle.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "TaskTitle.h"
#include "NumDlg.h"
#include "TemplIDSelDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskTitle dialog
static const unsigned long MAX_SECONDS = 0x80000000;
static abase::vector<AString> PathArray;
CTaskTitle::CTaskTitle(ATaskTempl* pTempl, int type, CWnd* pParent /*=NULL*/)
	: CDialog(CTaskTitle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskTitle)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTask = pTempl;
	m_iType = type;
	m_pCount = NULL;
	m_pPremTitles = NULL;
	m_AwardTitles = NULL;
}


void CTaskTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskTitle)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_LST_TITLE, m_lstTitle);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CTaskTitle, CDialog)
	//{{AFX_MSG_MAP(CTaskTitle)
	ON_BN_CLICKED(ID_ADD, OnAdd)
	ON_BN_CLICKED(ID_DEL, OnDel)
	ON_BN_CLICKED(ID_PERIOD, OnPeriod)
	ON_NOTIFY(NM_CLICK, IDC_LST_TITLE, OnClickChangeDateType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskTitle message handlers
BOOL LoadBaseDataHeader(const char* szPath, unsigned long& ulID, AString& strName);
void CTaskTitle::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if(*m_pCount>= MAX_TITLE_NUM)
	{
		AfxMessageBox("超出称号个数!",MB_OK);
		return;
	}
	
	CTemplIDSelDlg dlg;
	
	dlg.m_strRoot = "BaseData\\Config\\称号\\简单称号";
	
	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = 0;
	
	if (dlg.DoModal() != IDOK)
		return;
	
	for (unsigned char i = 0; i < *m_pCount; i++) {
		if ((m_iType == TITLE_FOR_PREM && m_pTask->m_PremTitles[i] == dlg.m_ulID)
			|| (m_iType == TITLE_FOR_AWARD_SUCCED && m_pTask->m_Award_S->m_pTitleAward[i].m_ulTitleID == dlg.m_ulID)
			|| (m_iType == TITLE_FOR_AWARD_FAILED && m_pTask->m_Award_F->m_pTitleAward[i].m_ulTitleID == dlg.m_ulID)) {
			AfxMessageBox("已有此称号", MB_ICONSTOP);
			return;
		}
	}

	if (m_iType != TITLE_FOR_PREM) {
		m_AwardTitles[*m_pCount].m_ulTitleID = dlg.m_ulID;
		
	} else {
		m_pPremTitles[*m_pCount] = dlg.m_ulID;
	}
	AString strName;

	LoadBaseDataHeader(dlg.m_strSelPath, dlg.m_ulID, strName);
	m_lstTitle.InsertItem(*m_pCount, CString(strName));
	CString strNum;
	strNum.Format("%u", dlg.m_ulID);
	m_lstTitle.SetItemText(*m_pCount, 1, strNum);
	(*m_pCount)++;
//	UpdateList();	
}

void CTaskTitle::OnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_lstTitle.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;
	
	unsigned char uItem = (unsigned char)m_lstTitle.GetNextSelectedItem(pos);
	
	for (unsigned char i = uItem+1; i < *m_pCount; i++) {
		if (m_pPremTitles)
			m_pPremTitles[i-1] = m_pPremTitles[i];
		if (m_AwardTitles)
			m_AwardTitles[i-1] = m_AwardTitles[i-1];
	}
	
	(*m_pCount)--;
	m_lstTitle.DeleteItem(uItem);
}

void CTaskTitle::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CTaskTitle::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	g_ConfigIDMan.Release();
}

BOOL CTaskTitle::OnInitDialog() 
{
	CDialog::OnInitDialog();

	g_ConfigIDMan.Load("BaseData\\ConfigID.dat");
	g_ConfigIDMan.GeneratePathArray(PathArray);
	// TODO: Add extra initialization here
	if (m_iType == TITLE_FOR_PREM) {
		m_pCount = &m_pTask->m_iPremTitleNumTotal;
		m_pPremTitles = m_pTask->m_PremTitles;
		m_lstTitle.InsertColumn(0, "称号名称", LVCFMT_LEFT, 100);
		m_lstTitle.InsertColumn(1, "称号ID", LVCFMT_LEFT, 50);	
		GetDlgItem(ID_PERIOD)->ShowWindow(SW_HIDE);
	}
	else if (m_iType == TITLE_FOR_AWARD_SUCCED) {
		m_pCount = &m_pTask->m_Award_S->m_ulTitleNum;
		m_AwardTitles = m_pTask->m_Award_S->m_pTitleAward;
		m_lstTitle.InsertColumn(0, "称号名称", LVCFMT_LEFT, 100);
		m_lstTitle.InsertColumn(1, "称号ID", LVCFMT_LEFT, 50);
		m_lstTitle.InsertColumn(2, "时间类型", LVCFMT_LEFT, 100);
		m_lstTitle.InsertColumn(3, "时间", LVCFMT_LEFT, 100);
		m_strList.AddTail("有效时间");
		m_strList.AddTail("截止日期");
		GetDlgItem(ID_PERIOD)->ShowWindow(SW_SHOW);
	}
	else if (m_iType == TITLE_FOR_AWARD_FAILED) {
		m_pCount = &m_pTask->m_Award_F->m_ulTitleNum;
		m_AwardTitles = m_pTask->m_Award_F->m_pTitleAward;
		m_lstTitle.InsertColumn(0, "称号名称", LVCFMT_LEFT, 100);
		m_lstTitle.InsertColumn(1, "称号ID", LVCFMT_LEFT, 50);
		m_lstTitle.InsertColumn(2, "时间类型", LVCFMT_LEFT, 100);
		m_lstTitle.InsertColumn(3, "时间", LVCFMT_LEFT, 100);
		m_strList.AddTail("有效时间");
		m_strList.AddTail("截止日期");
		GetDlgItem(ID_PERIOD)->ShowWindow(SW_SHOW);
	}

	m_lstTitle.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	UpdateList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskTitle::UpdateList()
{
	m_lstTitle.DeleteAllItems();
	unsigned long i;
	AString strName;
	unsigned long title_id;
	for (i=0;i<*m_pCount;i++)
	{
		strName.Empty();
		if (m_iType == TITLE_FOR_PREM) {
			title_id = m_pPremTitles[i];
			CString strPath = g_ConfigIDMan.GetPathByID(title_id);
			LoadBaseDataHeader(strPath, title_id, strName);
			m_lstTitle.InsertItem(i, CString(strName));
			CString strNum;
			strNum.Format("%u", m_pPremTitles[i]);
			m_lstTitle.SetItemText(i, 1, strNum);
		} else {
			title_id = m_AwardTitles[i].m_ulTitleID;
			CString strPath = g_ConfigIDMan.GetPathByID(title_id);
			LoadBaseDataHeader(strPath, title_id, strName);
			m_lstTitle.InsertItem(i, CString(strName));
			CString strNum;
			strNum.Format("%u", m_AwardTitles[i].m_ulTitleID);
			m_lstTitle.SetItemText(i, 1, strNum);
			m_lstTitle.SetItemText(i, 2, static_cast<unsigned long>(m_AwardTitles[i].m_lPeriod) > MAX_SECONDS ? "截止日期" : "有效时间");
			
			CString strPeriod;
			if (static_cast<unsigned long>(m_AwardTitles[i].m_lPeriod) > MAX_SECONDS)
			{
				int year = ((m_AwardTitles[i].m_lPeriod & ~MAX_SECONDS) >> 24) + 2000;
				int mon = (m_AwardTitles[i].m_lPeriod & 0x00FF0000) >> 16;
				int day = (m_AwardTitles[i].m_lPeriod & 0x0000FF00) >> 8;
				int hour = m_AwardTitles[i].m_lPeriod & 0x000000FF;
				strPeriod.Format("%04d%02d%02d%02d", year,mon,day,hour);
			}
			else
			{
				strPeriod.Format("%d", m_AwardTitles[i].m_lPeriod);
			}
			m_lstTitle.SetItemText(i, 3, strPeriod);
		}
	}	
}

void CTaskTitle::OnClickChangeDateType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 2)
	{
		m_lstTitle.ComboItem(pNMListView->iItem, pNMListView->iSubItem, m_strList);
		m_lstTitle.SetItemText(pNMListView->iItem,3,"0");
	}
	
	*pResult = 0;
}

void CTaskTitle::OnPeriod() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_lstTitle.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_lstTitle.GetNextSelectedItem(pos);
	
	TITLE_AWARD& ta = m_AwardTitles[uItem];
	CNumDlg dlgNum;
	dlgNum.m_nNum = 0;
	if (dlgNum.DoModal() != IDOK) return;
	CString strTime;
	strTime.Format("%d", dlgNum.m_nNum);
	CString str = m_lstTitle.GetItemText(uItem,2);
	if (str != CString("有效时间"))
	{
		tm endTime;
		memset(&endTime, 0, sizeof(tm));
		int year = dlgNum.m_nNum / 1000000;
		if (year < 2011 || year > 2127)
		{
			AfxMessageBox("年份输入有误！");
			return;
		}
		int month = (dlgNum.m_nNum - year * 1000000) / 10000;
		if ( month < 1 || month > 12)
		{
			AfxMessageBox("月份输入有误！");
			return;
		}
		int day = (dlgNum.m_nNum - year * 1000000 - month * 10000) / 100;
		if (day < 1 || day > 31)
		{
			AfxMessageBox("天数输入有误！");
			return;
		}
		int hour = dlgNum.m_nNum % 100;
		if (hour > 23)
		{
			AfxMessageBox("小时输入有误！");
			return;
		}
		char cyear = year - 2000;
		char cmon = month;
		char cday = day;
		char chour = hour;
		ta.m_lPeriod = cyear << 24;
		ta.m_lPeriod |= cmon << 16;
		ta.m_lPeriod |= cday << 8;
		ta.m_lPeriod |= chour;
		ta.m_lPeriod |= MAX_SECONDS;
	}
	else
	{
		if (static_cast<unsigned long>(dlgNum.m_nNum) > MAX_SECONDS)
		{
			AfxMessageBox("有效时间不能大于68年！");
			return;
		}
		ta.m_lPeriod = dlgNum.m_nNum;
	}
	m_lstTitle.SetItemText(uItem, 3, strTime);


}