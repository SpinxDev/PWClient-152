// GatherPropertyDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "GatherPropertyDlg.h"
#include "elementdataman.h"
#include "FindMonsterDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGatherPropertyDlg dialog
#define GATHER_NUM 1
CString gather_type[GATHER_NUM] = {"¿óÊ¯"};
DWORD   gather_id[GATHER_NUM] = { DT_MINE_ESSENCE };


CGatherPropertyDlg::CGatherPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGatherPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGatherPropertyDlg)
	m_nFresh = 0;
	m_nNum = 0;
	m_fOffset = 0.0f;
	//}}AFX_DATA_INIT
	m_data.dwId = 0;
	m_data.dwType = 0;
	m_data.dwRefurbishTime = 150;
	m_data.dwNum = 1;
	m_data.strName = "";
	m_data.fOffset = 0.0f;
	m_nType = DT_MINE_ESSENCE;
	m_bModified = false;
}


void CGatherPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGatherPropertyDlg)
	DDX_Control(pDX, IDC_LIST_GATHER_OBJECT, m_listObject);
	DDX_Control(pDX, IDC_COMBO_GATHER_TYPE, m_cbType);
	DDX_Text(pDX, IDC_EDIT_FRESH, m_nFresh);
	DDX_Text(pDX, IDC_EDIT_NUM, m_nNum);
	DDX_Text(pDX, IDC_EDIT_OFFSET_Y, m_fOffset);
	DDV_MinMaxFloat(pDX, m_fOffset, 0.f, 10000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGatherPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CGatherPropertyDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_GATHER_TYPE, OnSelchangeComboGatherType)
	ON_BN_CLICKED(IDC_BUTTON_FINDER, OnButtonFinder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGatherPropertyDlg message handlers

void CGatherPropertyDlg::OnSelchangeComboGatherType() 
{
	// TODO: Add your control notification handler code here
	int index = m_cbType.GetCurSel();
	if(index==-1) return;
	CString str;
	m_cbType.GetLBText(index,str);
	for(int i = 0; i < GATHER_NUM; i++)
	{
		if(strcmp(str,gather_type[i])==0)
		{
			m_nType = gather_id[i];
			break;
		}
	}
	FreshObjectList();
}

BOOL CGatherPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString type;
	// TODO: Add extra initialization here
	for( int i = 0; i < GATHER_NUM; i++)
	{
		m_cbType.AddString(gather_type[i]);
	}
	if(m_bModified)
	{	
		for( int j = 0; j < GATHER_NUM; j++)
		{
			if(gather_id[j] == m_data.dwType)
			{
				type = gather_type[j];
				break;
			}
		}

		int index = m_cbType.FindString(0,type);
		if(index!=-1) m_cbType.SetCurSel(index);
		else m_cbType.SetCurSel(0);
	}else
	{
		m_cbType.SetCurSel(0);
	}
	FreshObjectList();
	
	CString txt;
	CString name;
	int num = m_listObject.GetCount();
	for(i = 0; i < num; i++)
	{
		m_listObject.GetText(i,txt);
		name.Format("%s->%d",m_data.strName,m_data.dwId);
		if(strcmp(txt,name)==0)
		{
			m_listObject.SetCurSel(i);
			break;
		}
	}

	m_nNum = m_data.dwNum;
	m_nFresh = m_data.dwRefurbishTime;
	m_fOffset = m_data.fOffset;
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGatherPropertyDlg::FreshObjectList()
{
	int num = m_listObject.GetCount();
	 for(int i = 0; i < num; i++)
		m_listObject.DeleteString(0);

	AString strName;
	AString strID;
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	if(dwNum == 0) return;
	for(i = 0; i < dwNum; i++)
	{
		DATA_TYPE type = DT_MONSTER_ESSENCE;
		DWORD dwid = g_dataMan.get_data_id(ID_SPACE_ESSENCE,i,type);
		if(type == m_nType)
		{
			MINE_ESSENCE* pData;
			pData = (MINE_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strID.Format("->%d",pData->id);
			strName += strID;
			m_listObject.AddString(strName);
		}
	}
}

void CGatherPropertyDlg::OnOK()
{
	UpdateData(true);
	int cur = m_listObject.GetCurSel();
	if(cur == -1) return;
	CString txt;
	m_listObject.GetText(cur,txt);
	if(txt.IsEmpty()) return;
	
	AString strName;
	AString strID;
	AString strTemp;
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	if(dwNum == 0) return;
	for(int i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = g_dataMan.get_data_id(ID_SPACE_ESSENCE,i,type);
		if(type == m_nType)
		{
			MINE_ESSENCE* pData;
			pData = (MINE_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strTemp = strName;
			strID.Format("->%d",pData->id);
			strName +=strID;
			if(strcmp(strName,txt)==0)
			{
				m_data.dwType = m_nType;
				m_data.dwId = dwid;
				m_data.strName = strTemp;
				m_data.dwRefurbishTime = m_nFresh;
				m_data.dwNum = m_nNum;
				m_data.fOffset = m_fOffset;
				CDialog::OnOK();	
			}
		}
	}
}

void CGatherPropertyDlg::OnButtonFinder() 
{
	// TODO: Add your control notification handler code here
	CFindMonsterDlg dlg;
	dlg.m_nType = DT_MINE_ESSENCE;
	if(dlg.DoModal()==IDOK)
	{
		int sel = m_listObject.FindString(0,dlg.m_strSelectedMonster);
		if(sel!=-1) m_listObject.SetCurSel(sel);
	}
}
