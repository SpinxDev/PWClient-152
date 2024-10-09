// NpcPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "NpcPropertyDlg.h"
#include "A3D.h"
#include "MonsterFaction.h"
#include "FindMonsterDlg.h"
#include "BezierSelDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CNpcPropertyDlg dialog


CNpcPropertyDlg::CNpcPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNpcPropertyDlg)
	//}}AFX_DATA_INIT
	m_bModified = false;
	m_dwInitiactive = 0;
	m_nLoopType = 0;
	m_nSpeedType = 0;
}


void CNpcPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNpcPropertyDlg)
	DDX_Control(pDX, IDC_EDIT_NPC_DIE_DISAPEAR_TIME, m_editDisapearTime);
	DDX_Control(pDX, IDC_COMBO_PATH_SPEED_TYPE, m_cbSpeedType);
	DDX_Control(pDX, IDC_EDIT_PATH_ID, m_editPathID);
	DDX_Control(pDX, IDC_COMBO_PATH_LOOP_TYPE, m_cbLoopType);
	DDX_Control(pDX, IDC_EDIT_NPC_ID, m_editID);
	DDX_Control(pDX, IDC_EDIT_NPC_FRESH, m_editFresh);
	DDX_Control(pDX, IDC_EDIT_NPC_FRESHMIN,m_editFreshMin);
	DDX_Control(pDX, IDC_EDIT_NPC_OFFSET_T, m_editOffsetT);
	DDX_Control(pDX, IDC_EDIT_NPC_OFFSET_W, m_editOffsetW);
	DDX_Control(pDX, IDC_EDIT_NPC_NUM, m_editNum);
	DDX_Control(pDX, IDC_EDIT_NPC_DIE_NUM, m_editDieNum);
	DDX_Control(pDX, IDC_COMBO_NPC, m_comboNpc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNpcPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CNpcPropertyDlg)
	ON_BN_CLICKED(IDC_RADIO_INITIACTIVE, OnRadioInitiactive)
	ON_BN_CLICKED(IDC_RADIO_NOACTIVE, OnRadioNoactive)
	ON_BN_CLICKED(IDC_RADIO_NORMAL, OnRadioNormal)
	ON_CBN_SELCHANGE(IDC_COMBO_NPC, OnSelchangeComboNpc)
	ON_BN_CLICKED(IDC_MONSTER_TYPE, OnMonsterType)
	ON_BN_CLICKED(IDC_MONSTER_HELP, OnMonsterHelp)
	ON_BN_CLICKED(IDC_MONSTER_FREND, OnMonsterFrend)
	ON_BN_CLICKED(IDC_RADIO_HELP_NEED, OnRadioHelpNeed)
	ON_BN_CLICKED(IDC_RADIO_HELP_NO, OnRadioHelpNo)
	ON_BN_CLICKED(IDC_MONSTER_DEFAULT_1, OnMonsterDefault1)
	ON_BN_CLICKED(IDC_MONSTER_DEFAULT_2, OnMonsterDefault2)
	ON_BN_CLICKED(IDC_MONSTER_DEFAULT_3, OnMonsterDefault3)
	ON_BN_CLICKED(IDC_MONSTER_FIND, OnMonsterFind)
	ON_CBN_SELCHANGE(IDC_COMBO_PATH_LOOP_TYPE, OnSelchangeComboPathLoopType)
	ON_BN_CLICKED(IDC_SEL_PATH_BROWSE, OnSelPathBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO_PATH_SPEED_TYPE, OnSelchangeComboPathSpeedType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNpcPropertyDlg message handlers

BOOL CNpcPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AUX_FlatEditBox(&m_editID);
	//AUX_FlatEditBox(&m_editFresh);
	AUX_FlatEditBox(&m_editOffsetT);
	AUX_FlatEditBox(&m_editOffsetW);
	AUX_FlatEditBox(&m_editNum);
	AUX_FlatEditBox(&m_editDieNum);
	AUX_FlatEditBox(&m_editPathID);
	AUX_FlatEditBox(&m_editDisapearTime);

	AString strName;
	AString strID;
	AString strCur;
	DWORD dwNum = m_pDataMan->get_data_num(ID_SPACE_ESSENCE);
	if(dwNum == 0) return true;
	int i;
	for(i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = m_pDataMan->get_data_id(ID_SPACE_ESSENCE,i,type);
		if(type == DT_MONSTER_ESSENCE)
		{
			MONSTER_ESSENCE* pData;
			pData = (MONSTER_ESSENCE*)m_pDataMan->get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			strName = WC2AS(pData->name);
			strID.Format("->%d",pData->id);
			strName +=strID;
			m_comboNpc.AddString(strName);
			if(pData->id == m_npcData.dwID) strCur = strName;
		}
	}

	m_cbLoopType.AddString("终点停止");
	m_cbLoopType.AddString("原路返回");
	m_cbLoopType.AddString("始终循环");

	m_cbSpeedType.AddString("慢速移动");
	m_cbSpeedType.AddString("快速移动");
	
	CString strtemp[3] = {"终点停止", "原路返回", "始终循环"};
	CString strtemp2[2] = { "慢速移动","快速移动"};
	if(m_npcData.nLoopType<0 || m_npcData.nLoopType>2) m_npcData.nLoopType = 0;
	if(m_npcData.nSpeedFlags < 0 || m_npcData.nSpeedFlags > 1) m_npcData.nSpeedFlags = 0;
	for(i = 0; i < m_cbLoopType.GetCount(); i++)
	{
		CString txt;
		m_cbLoopType.GetLBText(i,txt);
		if(strcmp(txt,strtemp[m_npcData.nLoopType])==0) 
		{
			m_cbLoopType.SetCurSel(i);
			break;
		}
	}

	for(i = 0; i < m_cbSpeedType.GetCount(); i++)
	{
		CString txt;
		m_cbSpeedType.GetLBText(i,txt);
		if(strcmp(txt,strtemp2[m_npcData.nSpeedFlags])==0) 
		{
			m_cbSpeedType.SetCurSel(i);
			break;
		}
	}
	


	if(m_bModified)
	{
		CString str;
		int index = m_comboNpc.FindString(0,strCur);
		if(index > -1) m_comboNpc.SetCurSel(index);
		
		str.Format("%d",m_npcData.dwID);
		m_editID.SetWindowText(str);
		
		str.Format("%d",m_npcData.iRefurbish);
		m_editFresh.SetWindowText(str);

		str.Format("%d",m_npcData.iRefurbishMin);
		m_editFreshMin.SetWindowText(str);
		
		str.Format("%4.2f",m_npcData.fOffsetTerrain);
		m_editOffsetT.SetWindowText(str);
		
		str.Format("%4.2f",m_npcData.fOffsetWater);
		m_editOffsetW.SetWindowText(str);
		
		str.Format("%d",m_npcData.dwNum);
		m_editNum.SetWindowText(str);
		
		str.Format("%d",m_npcData.dwDiedTimes);
		m_editDieNum.SetWindowText(str);

		str.Format("%s(%d)",m_npcData.strPathName,m_npcData.nPathID);
		m_editPathID.SetWindowText(str);

		str.Format("%d",m_npcData.dwDisapearTime);
		m_editDisapearTime.SetWindowText(str);
		

		m_nLoopType     = m_npcData.nLoopType;
		m_nSpeedType    = m_npcData.nSpeedFlags;
		m_dwInitiactive = m_npcData.dwInitiative;

		EnableNeedHelp(m_npcData.bNeedHelp);
	}else
	{
		CString str;
		m_editFresh.SetWindowText("30");
		m_editFreshMin.SetWindowText("0");
		m_editOffsetT.SetWindowText("0");
		m_editOffsetW.SetWindowText("0");
		m_editPathID.SetWindowText("空(-1)");
		m_editNum.SetWindowText("1");
		m_editDieNum.SetWindowText("50");
		m_editDisapearTime.SetWindowText("0");
		m_comboNpc.SetCurSel(0);
		m_comboNpc.GetLBText(0,str);
		m_npcData.dwMounsterFaction = 0;
		m_npcData.dwMounsterFactionAccept = 0;
		m_npcData.dwMounsterFactionHelper = 0;
		m_npcData.bUseDefautFaction = true;
		m_npcData.bUseDefautFactionAccept = true;
		m_npcData.bUseDefautFactionHelper = true;
		m_npcData.nSpeedFlags = 0;
		m_npcData.nPathID = -1;
		m_npcData.nLoopType = 1;
		DWORD dwID = GetMonsterID(AString(str));
		str.Format("%d",dwID);
		m_editID.SetWindowText(str);
		EnableNeedHelp(false);
	}
	
	bool temp[3] = { false , false , false };
	temp[m_dwInitiactive] = true;
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_INITIACTIVE);
	if(pButton) pButton->SetCheck(temp[1]);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NOACTIVE);
	if(pButton) pButton->SetCheck(temp[2]);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NORMAL);
	if(pButton) pButton->SetCheck(temp[0]);
	pButton = (CButton *)GetDlgItem(IDC_MONSTER_DEFAULT_1);
	if(pButton) pButton->SetCheck(m_npcData.bUseDefautFaction);
	pButton = (CButton *)GetDlgItem(IDC_MONSTER_DEFAULT_2);
	if(pButton) pButton->SetCheck(m_npcData.bUseDefautFactionHelper);
	pButton = (CButton *)GetDlgItem(IDC_MONSTER_DEFAULT_3);
	if(pButton) pButton->SetCheck(m_npcData.bUseDefautFactionAccept);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

DWORD CNpcPropertyDlg::GetMonsterID(AString strName)
{
	AString name;
	AString strID;
	DWORD dwNum = m_pDataMan->get_data_num(ID_SPACE_ESSENCE);
	for(int i = 0; i < dwNum; i++)
	{
		DATA_TYPE type;
		DWORD dwid = m_pDataMan->get_data_id(ID_SPACE_ESSENCE,i,type);
		if(type == DT_MONSTER_ESSENCE)
		{
			MONSTER_ESSENCE* pData;
			pData = (MONSTER_ESSENCE*)m_pDataMan->get_data_ptr(dwid,ID_SPACE_ESSENCE,type);
			name = WC2AS(pData->name);
			strID.Format("->%d",pData->id);
			name +=strID;
			if(strcmp(name,strName)==0)
				return dwid;
		}
	}
	return 0;
}

void CNpcPropertyDlg::OnRadioInitiactive() 
{
	// TODO: Add your control notification handler code here
	m_dwInitiactive = CSceneAIGenerator::INDIVIDUALITY_INITIATIVE;
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_NOACTIVE);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NORMAL);
	if(pButton) pButton->SetCheck(false);
}

void CNpcPropertyDlg::OnRadioNoactive() 
{
	// TODO: Add your control notification handler code here
	m_dwInitiactive = CSceneAIGenerator::INDIVIDUALITY_PASSIVITY;
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_INITIACTIVE);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NORMAL);
	if(pButton) pButton->SetCheck(false);
}

void CNpcPropertyDlg::OnOK()
{
	int index = m_comboNpc.GetCurSel();
	if(index!=-1)
	{	
		CString str;
		m_comboNpc.GetLBText(index,str);
		int pos = str.FindOneOf("->");
		if(pos==-1) 
		{
			MessageBox("怪物的名字中没有->符号!");
			CDialog::OnCancel();
		}
		str = str.Left(pos);
		m_npcData.strNpcName = AString(str);
		m_npcData.dwInitiative = m_dwInitiactive;
		m_editDieNum.GetWindowText(str);
		m_npcData.dwDiedTimes = (DWORD)atof(str);
		m_editID.GetWindowText(str);
		m_npcData.dwID = (DWORD)atof(str);
		m_editNum.GetWindowText(str);
		m_npcData.dwNum = (DWORD)atof(str);
		m_editFresh.GetWindowText(str);
		m_npcData.iRefurbish = (int)atof(str);
		m_editFreshMin.GetWindowText(str);
		m_npcData.iRefurbishMin = (int)atof(str);
		m_editOffsetT.GetWindowText(str);
		m_npcData.fOffsetTerrain = (float)atof(str);
		m_editOffsetW.GetWindowText(str);
		m_npcData.fOffsetWater = (float)atof(str);
		m_editDisapearTime.GetWindowText(str);
		m_npcData.dwDisapearTime = (DWORD)atof(str);
		m_npcData.nLoopType = m_nLoopType;
		m_npcData.nSpeedFlags = m_nSpeedType;
		CDialog::OnOK();
	}
}

void CNpcPropertyDlg::OnRadioNormal() 
{
	// TODO: Add your control notification handler code here
	m_dwInitiactive = CSceneAIGenerator::INDIVIDUALITY_NORMAL;
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_INITIACTIVE);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_NOACTIVE);
	if(pButton) pButton->SetCheck(false);
}

void CNpcPropertyDlg::OnSelchangeComboNpc() 
{
	// TODO: Add your control notification handler code here
	int index = m_comboNpc.GetCurSel();
	if(index==-1) return;
	CString str;
	m_comboNpc.GetLBText(index,str);
	DWORD dwID = GetMonsterID(AString(str));
	str.Format("%d",dwID);
	m_editID.SetWindowText(str);
}

void CNpcPropertyDlg::OnMonsterType() 
{
	// TODO: Add your control notification handler code here
	CMonsterFaction dlg;
	dlg.m_ulFactionsMask = m_npcData.dwMounsterFaction;
	if(dlg.DoModal()==IDOK)
	{
		m_npcData.dwMounsterFaction = dlg.m_ulFactionsMask;
	}
}

void CNpcPropertyDlg::OnMonsterHelp() 
{
	// TODO: Add your control notification handler code here
	CMonsterFaction dlg;
	dlg.m_ulFactionsMask = m_npcData.dwMounsterFactionHelper;
	if(dlg.DoModal()==IDOK)
	{
		m_npcData.dwMounsterFactionHelper = dlg.m_ulFactionsMask;
	}
}

void CNpcPropertyDlg::OnMonsterFrend() 
{
	// TODO: Add your control notification handler code here
	CMonsterFaction dlg;
	dlg.m_ulFactionsMask = m_npcData.dwMounsterFactionAccept;
	if(dlg.DoModal()==IDOK)
	{
		m_npcData.dwMounsterFactionAccept = dlg.m_ulFactionsMask;
	}
	
}

void  CNpcPropertyDlg::EnableNeedHelp(bool bNeed)
{
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HELP_NEED);
	if(pButton) pButton->SetCheck(bNeed);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_HELP_NO);
	if(pButton) pButton->SetCheck(!bNeed);
	m_npcData.bNeedHelp = bNeed;
}

void CNpcPropertyDlg::OnRadioHelpNeed() 
{
	// TODO: Add your control notification handler code here
	EnableNeedHelp(true);
}

void CNpcPropertyDlg::OnRadioHelpNo() 
{
	// TODO: Add your control notification handler code here
	EnableNeedHelp(false);
}

void CNpcPropertyDlg::OnMonsterDefault1() 
{
	// TODO: Add your control notification handler code here
	m_npcData.bUseDefautFaction = !m_npcData.bUseDefautFaction;
}

void CNpcPropertyDlg::OnMonsterDefault2() 
{
	// TODO: Add your control notification handler code here
	m_npcData.bUseDefautFactionHelper = !m_npcData.bUseDefautFactionHelper;
}

void CNpcPropertyDlg::OnMonsterDefault3() 
{
	// TODO: Add your control notification handler code here
	m_npcData.bUseDefautFactionAccept = !m_npcData.bUseDefautFactionAccept;
}

void CNpcPropertyDlg::OnMonsterFind() 
{
	// TODO: Add your control notification handler code here
	CFindMonsterDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		int n = m_comboNpc.GetCount();
		for(int i = 0; i < n; i++)
		{
			CString str;
			m_comboNpc.GetLBText(i,str);
			if(strcmp(str,dlg.m_strSelectedMonster)==0)
			{
				m_comboNpc.SetCurSel(i);
				DWORD dwID = GetMonsterID(AString(str));
				str.Format("%d",dwID);
				m_editID.SetWindowText(str);
				break;
			}
		}
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NPC_NUM);
		if(pEdit) pEdit->SetFocus();
	}
}

void CNpcPropertyDlg::OnSelchangeComboPathLoopType() 
{
	// TODO: Add your control notification handler code here
	int index = m_cbLoopType.GetCurSel();
	if(index==-1) return;
	CString str;
	m_cbLoopType.GetLBText(index,str);
	
	if(strcmp(str,"终点停止")==0)  m_nLoopType = 0;
	else if(strcmp(str,"原路返回")==0)  m_nLoopType = 1;
	else if(strcmp(str,"始终循环")==0)  m_nLoopType = 2;
	else  m_nLoopType = -1;
}

void CNpcPropertyDlg::OnSelPathBrowse() 
{
	// TODO: Add your control notification handler code here
	CBezierSelDlg dlg;
	dlg.m_nBezierID = m_npcData.nPathID;
	dlg.m_strBezierName = m_npcData.strPathName;
	if(dlg.DoModal()==IDOK)
	{
		m_npcData.strPathName = dlg.m_strBezierName;
		m_npcData.nPathID = dlg.m_nBezierID;
		CString str;
		str.Format("%s(%d)",m_npcData.strPathName,m_npcData.nPathID);
		m_editPathID.SetWindowText(str);
	}
}

void CNpcPropertyDlg::OnSelchangeComboPathSpeedType() 
{
	// TODO: Add your control notification handler code here
	int index = m_cbSpeedType.GetCurSel();
	if(index==-1) return;
	CString str;
	m_cbSpeedType.GetLBText(index,str);
	
	if(strcmp(str,"慢速移动")==0)  m_nSpeedType = 0;
	else if(strcmp(str,"快速移动")==0)  m_nSpeedType = 1;
	else  m_nLoopType = -1;
}
