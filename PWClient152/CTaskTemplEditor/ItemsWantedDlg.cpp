// ItemsWantedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "ItemsWantedDlg.h"
#include "NumDlg.h"
#include "TemplIDSelDlg.h"
#include "FloatDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemsWantedDlg dialog

static const char* _task_item_path = "BaseData\\物品\\任务物品";

CItemsWantedDlg::CItemsWantedDlg(unsigned long ulMaxCount, ITEM_WANTED* pItems, unsigned long* pCount, BOOL bOnlyTaskItem, BOOL bFlag, CWnd* pParent /*=NULL*/,BOOL bPremItem)
	: m_ulMaxCount(ulMaxCount),
	  m_pItems(pItems),
	  m_pCount(pCount),
	  m_bOnlyTaskItem(bOnlyTaskItem),
	  m_bFlag(bFlag),
	  m_bPremItem(bPremItem),
	  CDialog(CItemsWantedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemsWantedDlg)
	m_bRandOne = FALSE;
	m_fTotalProb = 0.0f;
	//}}AFX_DATA_INIT
}


void CItemsWantedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemsWantedDlg)
	DDX_Control(pDX, IDC_ITEMS, m_ItemsLst);
	DDX_Check(pDX, IDC_RAND_ONE, m_bRandOne);
	DDX_Text(pDX, IDC_TOTAL_PROB, m_fTotalProb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemsWantedDlg, CDialog)
	//{{AFX_MSG_MAP(CItemsWantedDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_NUM, OnEditNum)
	ON_BN_CLICKED(IDC_PROB, OnProb)
	ON_BN_CLICKED(IDC_RAND_ONE, OnRandOne)
	ON_BN_CLICKED(IDC_PERIOD, OnPeriod)
	ON_NOTIFY(NM_CLICK, IDC_ITEMS, OnClickChangeDateType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemsWantedDlg message handlers

CString GetBaseDataNameByID(unsigned long ulID);

static const char* _type[] =
{
	"普通物品",
	"任务物品"
};
static const unsigned long MAX_SECONDS = 0x80000000;
BOOL CItemsWantedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ItemsLst.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ItemsLst.InsertColumn(0, _T("物品名称"), LVCFMT_LEFT, 110);
	m_ItemsLst.InsertColumn(1, _T("物品类型"), LVCFMT_LEFT, 80);
	m_ItemsLst.InsertColumn(2, _T("物品数量"), LVCFMT_LEFT, 80);
	m_ItemsLst.InsertColumn(3, _T("出现概率"), LVCFMT_LEFT, 80);
	if (m_bFlag)
	{
		m_ItemsLst.InsertColumn(4, _T("有效时间"), LVCFMT_LEFT, 80);
	}
	else
	{
		m_ItemsLst.InsertColumn(4, _T("时间方式"), LVCFMT_LEFT, 80);
		m_ItemsLst.InsertColumn(5, _T("时间设置"), LVCFMT_LEFT, 80);
		m_strList.AddTail("有效时间");
		m_strList.AddTail("截止日期");
	}

	for (unsigned char i = 0; i < *m_pCount; i++)
	{
		ITEM_WANTED& wi = m_pItems[i];

		CString strID;
		strID.Format("%d", wi.m_ulItemTemplId);

		m_ItemsLst.InsertItem(
			i,
			GetBaseDataNameByID(wi.m_ulItemTemplId) + _T("   ") + strID);
		
		m_ItemsLst.SetItemText(i, 1, wi.m_bCommonItem ? _type[0] : _type[1]);

		CString strNum;
		strNum.Format("%d", wi.m_ulItemNum);
		m_ItemsLst.SetItemText(i, 2, strNum);

		if (m_bFlag)
		{
			m_ItemsLst.SetItemText(i, 3, "1.000000");
			m_ItemsLst.SetItemText(i, 4, "0");
		}
		else
		{
			CString strProb;
			strProb.Format("%f", wi.m_fProb);
			m_ItemsLst.SetItemText(i, 3, strProb);
			
			if (wi.m_fProb != 1.0f)
				m_fTotalProb += wi.m_fProb;

			m_ItemsLst.SetItemText(i, 4, static_cast<unsigned long>(wi.m_lPeriod) > MAX_SECONDS ? "截止日期" : "有效时间");
			
			CString strPeriod;
			if (static_cast<unsigned long>(wi.m_lPeriod) > MAX_SECONDS)
			{
				int year = ((wi.m_lPeriod & ~MAX_SECONDS) >> 24) + 2000;
				int mon = (wi.m_lPeriod & 0x00FF0000) >> 16;
				int day = (wi.m_lPeriod & 0x0000FF00) >> 8;
				int hour = wi.m_lPeriod & 0x000000FF;
				strPeriod.Format("%04d%02d%02d%02d", year,mon,day,hour);
			}
			else
			{
				strPeriod.Format("%d", wi.m_lPeriod);
			}
			m_ItemsLst.SetItemText(i, 5, strPeriod);
		}		
	}

	UpdateData(FALSE);
	if (m_bPremItem)
	{
		GetDlgItem(IDC_RAND_ONE)->SetWindowText("单项道具开启");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemsWantedDlg::OnAdd()
{
	if (*m_pCount >= m_ulMaxCount)
	{
		AfxMessageBox("物品已到达上限", MB_ICONSTOP);
		return;
	}

	CTemplIDSelDlg dlg;

	if (m_bOnlyTaskItem)
		dlg.m_strRoot = _task_item_path;
	else
		dlg.m_strRoot = "BaseData";

	dlg.m_strExt = ".tmpl";
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK)
		return;

	for (unsigned char i = 0; i < *m_pCount; i++)
	{
		if (m_pItems[i].m_ulItemTemplId == dlg.m_ulID)
		{
			AfxMessageBox("已有此物品", MB_ICONSTOP);
			return;
		}
	}

	CNumDlg dlgNum;

	if (dlgNum.DoModal() != IDOK)
		return;

	FloatDlg dlgProb;

	if (dlgProb.DoModal() != IDOK)
		return;

	unsigned char uIndex = *m_pCount;
	(*m_pCount)++;
	ITEM_WANTED& wi = m_pItems[uIndex];

	wi.m_ulItemTemplId = dlg.m_ulID;
	CString strPath(_task_item_path);
	strPath.MakeLower();
	strPath += "\\";
	dlg.m_strSelPath.MakeLower();
	wi.m_bCommonItem = dlg.m_strSelPath.Find(strPath) == -1;

	wi.m_ulItemNum = dlgNum.m_nNum;
	wi.m_fProb = dlgProb.m_fVal;
	wi.m_lPeriod = 0;

	CString strID;
	strID.Format("%d", wi.m_ulItemTemplId);

	m_ItemsLst.InsertItem(uIndex, GetBaseDataNameByID(wi.m_ulItemTemplId) + _T("   ") + strID);
	m_ItemsLst.SetItemText(uIndex, 1, wi.m_bCommonItem ? _type[0] : _type[1]);
	CString str;
	str.Format("%u", wi.m_ulItemNum);
	m_ItemsLst.SetItemText(uIndex, 2, str);
	str.Format("%f", wi.m_fProb);
	m_ItemsLst.SetItemText(uIndex, 3, str);
	if (m_bFlag)
	{
		m_ItemsLst.SetItemText(uIndex, 4, "0");
	}
	else
	{
		m_ItemsLst.SetItemText(uIndex, 4, "有效时间");
		m_ItemsLst.SetItemText(uIndex, 5, "0");
	}
	
}

void CItemsWantedDlg::OnDel()
{
	POSITION pos = m_ItemsLst.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned char uItem = (unsigned char)m_ItemsLst.GetNextSelectedItem(pos);

	for (unsigned char i = uItem+1; i < *m_pCount; i++)
		m_pItems[i-1] = m_pItems[i];

	(*m_pCount)--;
	m_ItemsLst.DeleteItem(uItem);
}

void CItemsWantedDlg::OnEditNum() 
{
	POSITION pos = m_ItemsLst.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_ItemsLst.GetNextSelectedItem(pos);
	
	ITEM_WANTED& wi = m_pItems[uItem];
	CNumDlg dlgNum;
	dlgNum.m_nNum = wi.m_ulItemNum;
	if (dlgNum.DoModal() != IDOK) return;
	wi.m_ulItemNum = dlgNum.m_nNum;
	CString str;
	str.Format("%d", wi.m_ulItemNum);
	m_ItemsLst.SetItemText(uItem, 2, str);
}

void CItemsWantedDlg::OnProb() 
{
	POSITION pos = m_ItemsLst.GetFirstSelectedItemPosition();
	if (!pos) return;

	int nItem = m_ItemsLst.GetNextSelectedItem(pos);
	
	ITEM_WANTED& wi = m_pItems[nItem];

	FloatDlg dlg;
	dlg.m_fVal = wi.m_fProb;

	if (dlg.DoModal() != IDOK) return;

	wi.m_fProb = dlg.m_fVal;

	CString str;
	str.Format("%f", wi.m_fProb);
	m_ItemsLst.SetItemText(nItem, 3, str);
}

void CItemsWantedDlg::OnRandOne() 
{
	UpdateData();
}

void CItemsWantedDlg::OnPeriod() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ItemsLst.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned char uItem = (unsigned char)m_ItemsLst.GetNextSelectedItem(pos);
	
	ITEM_WANTED& wi = m_pItems[uItem];
	CNumDlg dlgNum;
	dlgNum.m_nNum = 0;
	if (dlgNum.DoModal() != IDOK) return;
	CString strTime;
	strTime.Format("%d", dlgNum.m_nNum);
	if (!m_bFlag)
	{
		CString str = m_ItemsLst.GetItemText(uItem,4);
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
			wi.m_lPeriod = cyear << 24;
			wi.m_lPeriod |= cmon << 16;
			wi.m_lPeriod |= cday << 8;
			wi.m_lPeriod |= chour;
			wi.m_lPeriod |= MAX_SECONDS;
		}
		else
		{
			if (static_cast<unsigned long>(dlgNum.m_nNum) > MAX_SECONDS)
			{
				AfxMessageBox("有效时间不能大于68年！");
				return;
			}
			wi.m_lPeriod = dlgNum.m_nNum;
		}
		m_ItemsLst.SetItemText(uItem, 5, strTime);
	}
	else
	{
		wi.m_lPeriod = dlgNum.m_nNum;
		m_ItemsLst.SetItemText(uItem, 4, strTime);
	}

}
void CItemsWantedDlg::OnClickChangeDateType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_bFlag)
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if (pNMListView->iSubItem == 4)
		{
			m_ItemsLst.ComboItem(pNMListView->iItem, pNMListView->iSubItem, m_strList);
			m_ItemsLst.SetItemText(pNMListView->iItem,5,"0");
		}
		
		*pResult = 0;
	}
}