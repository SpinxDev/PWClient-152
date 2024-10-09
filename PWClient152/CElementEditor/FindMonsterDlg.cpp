// FindMonsterDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "FindMonsterDlg.h"
#include "A3D.h"
#include "MonsterFaction.h"
#include "elementdataman.h"
//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CFindMonsterDlg dialog


CFindMonsterDlg::CFindMonsterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindMonsterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindMonsterDlg)
	m_strKey = _T("");
	m_bExa = FALSE;
	//}}AFX_DATA_INIT
	m_nType = DT_MONSTER_ESSENCE;
}


void CFindMonsterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindMonsterDlg)
	DDX_Control(pDX, IDC_LIST_MONSTER, m_listMonster);
	DDX_Text(pDX, IDC_MONSTER_KEY, m_strKey);
	DDX_Check(pDX, IDC_CHECK_EXA, m_bExa);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindMonsterDlg, CDialog)
	//{{AFX_MSG_MAP(CFindMonsterDlg)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindMonsterDlg message handlers

void CFindMonsterDlg::OnButtonFind() 
{
	// TODO: Add your control notification handler code here
	int itemNum = m_listMonster.GetCount();
	for(int i = 0; i < itemNum ; i++)
		m_listMonster.DeleteString(0);

	UpdateData(true);
	
	AString strName;
	AString strID;
	AString strTemp;
	
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	if(dwNum == 0) return;
	for(i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = g_dataMan.get_data_id(ID_SPACE_ESSENCE,i,type);
		if(type == m_nType && type == DT_MONSTER_ESSENCE)
		{
			MONSTER_ESSENCE* pData;
			pData = (MONSTER_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strTemp = strName;
			strID.Format("->%d",pData->id);
			strName +=strID;
			if(m_bExa)
			{
				if(stricmp(strTemp,m_strKey)==0)
					m_listMonster.AddString(strName);
			}else
			{
				if(strName.Find(m_strKey)!=-1)
					m_listMonster.AddString(strName);
			}
		}else if(type == m_nType && type == DT_NPC_ESSENCE)
		{
			NPC_ESSENCE* pData;
			pData = (NPC_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strTemp = strName;
			strID.Format("->%d",pData->id);
			strName +=strID;
			if(m_bExa)
			{
				if(stricmp(strTemp,m_strKey)==0)
					m_listMonster.AddString(strName);
			}else
			{
				if(strName.Find(m_strKey)!=-1)
					m_listMonster.AddString(strName);
			}	
		}else if(type == m_nType && type == DT_MINE_ESSENCE)
		{
			MINE_ESSENCE* pData;
			pData = (MINE_ESSENCE*)g_dataMan.get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strTemp = strName;
			strID.Format("->%d",pData->id);
			strName +=strID;
			if(m_bExa)
			{
				if(stricmp(strTemp,m_strKey)==0)
					m_listMonster.AddString(strName);
			}else
			{
				if(strName.Find(m_strKey)!=-1)
					m_listMonster.AddString(strName);
			}	
		}
	}
}

void CFindMonsterDlg::OnOK()
{
	int index = m_listMonster.GetCurSel();
	if(index!=-1)
		m_listMonster.GetText(index,m_strSelectedMonster);
	else m_strSelectedMonster = "";
	
	CDialog::OnOK();
}

BOOL CFindMonsterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_MONSTER_KEY);
	if(pEdit) pEdit->SetFocus();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
