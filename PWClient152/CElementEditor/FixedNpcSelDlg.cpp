// FixedNpcSelDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "FixedNpcSelDlg.h"
#include "FindMonsterDlg.h"
#include "A3D.h"


//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CFixedNpcSelDlg dialog


CFixedNpcSelDlg::CFixedNpcSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixedNpcSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixedNpcSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFixedNpcSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixedNpcSelDlg)
	DDX_Control(pDX, IDC_COMBO_SEL, m_comboSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixedNpcSelDlg, CDialog)
	//{{AFX_MSG_MAP(CFixedNpcSelDlg)
	ON_COMMAND(ID_FILE_SORT_MAP, OnFileSortMap)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_NPC, OnRadioNpc)
	ON_BN_CLICKED(IDC_RADIO_MONSTER, OnRadioMonster)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedNpcSelDlg message handlers

BOOL CFixedNpcSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	FreshList();
	CString strTemp;
	strTemp.Format("%s->%d",m_strName,m_dwSelId);
	int index = m_comboSel.FindString(0,m_strName);
	m_comboSel.SetCurSel(index);
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_NPC);
	if(m_bMonster) pButton->SetCheck(0);
	else pButton->SetCheck(1);

	pButton = (CButton *)GetDlgItem(IDC_RADIO_MONSTER);
	if(m_bMonster) pButton->SetCheck(1);
	else pButton->SetCheck(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedNpcSelDlg::OnOK()
{
	int index = m_comboSel.GetCurSel();
	if(index!=-1)
	{	
		CString str;
		m_comboSel.GetLBText(index,str);
		m_strName = AString(str);
		m_dwSelId = GetNpcID(m_strName);
		int pos = m_strName.FindOneOf("->");
		if(pos==-1) 
		{
			MessageBox("NPC的名字中没有->符号!");
			CDialog::OnCancel();
		}
		m_strName = m_strName.Left(pos);
	}
	CDialog::OnOK();
}

DWORD CFixedNpcSelDlg::GetNpcID(CString strName)
{
	AString name;
	AString strID;
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	for(int i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = g_dataMan.get_data_id(ID_SPACE_ESSENCE,i,type);
		if(m_bMonster)
		{
			if(type == DT_MONSTER_ESSENCE)
			{
				MONSTER_ESSENCE* pData;
				pData = (MONSTER_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
				name = WC2AS(pData->name);
				strID.Format("->%d",pData->id);
				name +=strID;
				if(strcmp(name,strName)==0)
					return dwid;
			}
		}else
		{
			if(type == DT_NPC_ESSENCE)
			{
				NPC_ESSENCE* pData;
				pData = (NPC_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
				name = WC2AS(pData->name);
				strID.Format("->%d",pData->id);
				name +=strID;
				if(strcmp(name,strName)==0)
					return dwid;
			}
		}
	}
	return 0;
}

void CFixedNpcSelDlg::OnFileSortMap() 
{
	// TODO: Add your command handler code here
	
}

void CFixedNpcSelDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CFixedNpcSelDlg::OnRadioNpc() 
{
	// TODO: Add your control notification handler code here
	if(m_bMonster)
	{
		m_bMonster = false;
		m_dwSelId = -1;
	    m_strName = "";
		FreshList();
	}
}

void CFixedNpcSelDlg::OnRadioMonster() 
{
	// TODO: Add your control notification handler code here
	if(!m_bMonster)
	{
		m_bMonster = true;
		m_dwSelId = -1;
	    m_strName = "";
		FreshList();
	}
}

void  CFixedNpcSelDlg::FreshList()
{
	int num = m_comboSel.GetCount();
	for(int n = 0; n < num; n++) m_comboSel.DeleteString(0);
	AString strName;
	AString strID;
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	
	for(int i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = g_dataMan.get_data_id(ID_SPACE_ESSENCE,i,type);
		if(m_bMonster)
		{
			if(type == DT_MONSTER_ESSENCE)
			{
				MONSTER_ESSENCE* pData;
				pData = (MONSTER_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
				strName = WC2AS(pData->name);
				strID.Format("->%d",pData->id);
				strName +=strID;
				m_comboSel.AddString(strName);
			}
		}else
		{
			if(type == DT_NPC_ESSENCE)
			{
				NPC_ESSENCE* pData;
				pData = (NPC_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
				strName = WC2AS(pData->name);
				strID.Format("->%d",pData->id);
				strName +=strID;
				m_comboSel.AddString(strName);
			}
		}
	}
	m_comboSel.SetCurSel(0);
}

void CFixedNpcSelDlg::OnButtonFind() 
{
	// TODO: Add your control notification handler code here
	CFindMonsterDlg dlg;
	if(!m_bMonster) dlg.m_nType = DT_NPC_ESSENCE;
	if(dlg.DoModal()==IDOK)
	{
		int sel = m_comboSel.FindString(0,dlg.m_strSelectedMonster);
		if(sel!=-1) m_comboSel.SetCurSel(sel);
	}
}
