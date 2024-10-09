// TalkCreateDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef TASK_TEMPL_EDITOR
	#include "../../CTaskTemplEditor/TaskTemplEditor.h"
#else
	#include "ElementData.h"
#endif

#include "TalkCreateDlg.h"
#include "TemplIDSelDlg.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"
#include "TalkModifyDlg.h"
#include "TaskIDSelDlg.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTalkCreateDlg dialog

#define SERVICE_COUNT 22

CString m_strService[SERVICE_COUNT] =
{
	"交谈",
	"出售商品",
	"收购商品",
	"修理商品",
	"镶嵌",
	"拆除",
	"发放任务",
	"完成任务",
	"发放任务物品",
	"教授技能",
	"治疗",
	"传送",
	"运输",
	"代售",
	"存储",
	"生产",
	"分解",
	"返回",
	"结束",
	"仓库密码",
	"鉴定",
	"放弃任务"
};

CTalkCreateDlg::CTalkCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTalkCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTalkCreateDlg)
	m_strName = _T("");
	m_strDesc = _T("");
	m_dwID = 0;
	m_strCommandName = _T("");
	m_strLinkName = _T("");
	//}}AFX_DATA_INIT
	m_bEnableWnd = false;
	m_dwServiceID = 0x80000000;
	m_bWindow = true;
	m_bModified = false;
	m_strExt = "tmpl";

	m_bPreset = false;
	m_dwParamToSel = 0;
	m_dwParamToSel2 = 0;
	m_dwSvrToSel = 0;
}


void CTalkCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkCreateDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_WINDOW_DESC, m_strDesc);
	DDX_Text(pDX, IDC_EDIT_WINDOW_ID, m_dwID);
	DDX_Text(pDX, IDC_EDIT_COMMAND_NAME, m_strCommandName);
	DDX_Text(pDX, IDC_EDIT_SERVICE_LINK, m_strLinkName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTalkCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CTalkCreateDlg)
	ON_BN_CLICKED(IDC_RADIO_TALK_TYPE1, OnRadioTalkType1)
	ON_BN_CLICKED(IDC_RADIO_TALK_TYPE2, OnRadioTalkType2)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVICE, OnSelchangeComboService)
	ON_BN_CLICKED(IDC_BUTTON_SERVICE_LINK, OnButtonServiceLink)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVICE_PARAMS, OnSelchangeComboServiceParams)
	ON_BN_CLICKED(IDC_RADIO_TALK_TYPE3, OnRadioTalkType3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkCreateDlg message handlers

void CTalkCreateDlg::EnableCreateWindow(bool bEnable)
{
	CWnd *pWnd = NULL;
	
	DWORD wID[5] = {IDC_EDIT_WINDOW_ID,IDC_EDIT_WINDOW_DESC,IDC_STATIC_WINDOW,
					IDC_STATIC_ID,IDC_STATIC_DESC};
	
	//WINDOW
	int i(0);
	for(i = 0; i< 5; i++)
	{
		pWnd = GetDlgItem(wID[i]);
		if(pWnd) pWnd->EnableWindow(bEnable);
	}
	DWORD sID[3] = {IDC_COMBO_SERVICE, IDC_STATIC_SERVICE, IDC_STATIC_SERVICE_TYPE};
	//Service
	for(i = 0; i<3; i++)
	{
		pWnd = GetDlgItem(sID[i]);
		if(pWnd) pWnd->EnableWindow(!bEnable);
	}

	if( m_dwServiceID == NPC_GIVE_TASK_MATTER || 
		m_dwServiceID == NPC_GIVE_TASK || 
		m_dwServiceID == NPC_COMPLETE_TASK || 
		m_dwServiceID == NPC_SKILL ||
		m_dwServiceID == TALK_GIVEUP_TASK)
		EnableLinkCtrl(!bEnable);
	else EnableLinkCtrl(false);
}

void CTalkCreateDlg::EnableLinkCtrl(bool bEnable)
{
	CWnd *pWnd = NULL;
	
	DWORD wID[3] = {IDC_STATIC_SERVICE_TYPE2,IDC_EDIT_SERVICE_LINK,IDC_BUTTON_SERVICE_LINK};
	
	//WINDOW
	for(int i = 0; i< 3; i++)
	{
		pWnd = GetDlgItem(wID[i]);
		if(pWnd) pWnd->EnableWindow(bEnable);
	}
}

void CTalkCreateDlg::EnableParamsCtrl(bool bEnable)
{
	CWnd *pWnd = NULL;
	
	DWORD wID[2] = {IDC_STATIC_SERVICE_PARAMS,IDC_COMBO_SERVICE_PARAMS};
	
	//WINDOW
	for(int i = 0; i< 2; i++)
	{
		pWnd = GetDlgItem(wID[i]);
		if(pWnd) pWnd->EnableWindow(bEnable);
	}
}

BOOL CTalkCreateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	
	CWnd* pWnd = NULL;
	if(m_bEnableWnd)
	{
		if(m_dwID & 0xc0000000)
		{
			pWnd = GetDlgItem(IDC_RADIO_TALK_TYPE1);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_RADIO_TALK_TYPE2);
			pWnd->EnableWindow(false);
		}else
		{
			pWnd = GetDlgItem(IDC_RADIO_TALK_TYPE2);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_RADIO_TALK_TYPE3);
			pWnd->EnableWindow(false);
		}
	}
	EnableCreateWindow(m_bWindow);
	
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_TALK_TYPE1);
	pButton->SetCheck(1);
	
	if(m_bModified)
	{
		pButton->EnableWindow(false);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TALK_TYPE2);
		pButton->EnableWindow(false);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TALK_TYPE3);
		pButton->EnableWindow(false);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_TALK_TYPE1);
		pButton->EnableWindow(false);
	}
	
	InitServiceCombox();
	InitParamsCombox();
	

	if(m_dwServiceLinkID) m_strLinkName = GetTaskNameByID(m_dwServiceLinkID);

	UpdateData(false);
	pWnd  = GetDlgItem(IDC_EDIT_COMMAND_NAME);
	if(pWnd) pWnd->SetFocus();

	m_dwOldID = m_dwID;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTalkCreateDlg::InitServiceCombox()
{
	CComboBox *pc = (CComboBox *)GetDlgItem(IDC_COMBO_SERVICE);
	EnableLinkCtrl(false);

	int index = m_dwServiceID - 0x80000000;

	if (m_bPreset)
	{
		const unsigned int ret_index	= (unsigned int)TALK_RETURN		- 0x80000000;
		const unsigned int exit_index	= (unsigned int)TALK_EXIT		- 0x80000000;
		const unsigned int giveup_index = (unsigned int)TALK_GIVEUP_TASK- 0x80000000;
		pc->AddString(m_strService[m_dwSvrToSel-0x80000000]);
		pc->AddString(m_strService[ret_index]);
		pc->AddString(m_strService[exit_index]);

		if (index == ret_index)
			index = 1;
		else if (index == exit_index)
			index = 2;
		else
			index = 0;

		if (m_dwSvrToSel == NPC_COMPLETE_TASK)
		{
			pc->AddString(m_strService[giveup_index]);
			if (m_dwServiceID == TALK_GIVEUP_TASK)
				pc->SetCurSel(pc->GetCount()-1);
			else
				pc->SetCurSel(index);
		}
		else
			pc->SetCurSel(index);
	}
	else
	{
		for(int i = 0; i< SERVICE_COUNT; i++)
			pc->AddString(m_strService[i]);

		pc->SetCurSel(index);
	}

	if(m_bModified)
	{
		if(m_dwServiceID == NPC_SKILL)
		{
			m_strRootDir = "BaseData\\技能";
			EnableLinkCtrl(true);
		}else if(m_dwServiceID == NPC_GIVE_TASK)
		{
			m_strRootDir = "BaseData\\TaskTemplate";
			EnableLinkCtrl(true);
		}else if(m_dwServiceID == NPC_COMPLETE_TASK)
		{
			m_strRootDir = "BaseData\\TaskTemplate";
			EnableLinkCtrl(true);
		}else if(m_dwServiceID == NPC_GIVE_TASK_MATTER)
		{
			m_strRootDir = "BaseData\\TaskTemplate";
			EnableLinkCtrl(true);
		}
		else if (m_dwServiceID == TALK_GIVEUP_TASK)
		{
			m_strRootDir = "BaseData\\TaskTemplate";
			EnableLinkCtrl(true);
		}
		return;
	}
}

void CTalkCreateDlg::InitParamsCombox()
{
	CComboBox *pc = (CComboBox *)GetDlgItem(IDC_COMBO_SERVICE_PARAMS);
	pc->ResetContent();
	for(int i = 0; i< 32; i++)
	{
		CString str;
		if(m_dwServiceID == NPC_TRANSMIT)
			str.Format("传送点:%d",i);
		else if(m_dwServiceID == NPC_TRANSPORT)
			str.Format("路线:%d",i);
		pc->AddString(str);
	}
	EnableParamsCtrl(false);
	if(m_bModified)
	{
		if(m_dwServiceID == NPC_TRANSMIT)
		{
			EnableParamsCtrl(true);
		}else
		if(m_dwServiceID == NPC_TRANSPORT)
		{
			EnableParamsCtrl(true);
		}
	}
	pc->SetCurSel(m_dwServiceLinkID);
}

void CTalkCreateDlg::OnOK()
{
	UpdateData(true);
	if( m_dwServiceID == NPC_GIVE_TASK_MATTER || 
		m_dwServiceID == NPC_GIVE_TASK || 
		m_dwServiceID == NPC_COMPLETE_TASK ||
		m_dwServiceID == NPC_SKILL ||
		m_dwServiceID == TALK_GIVEUP_TASK)
	{
		if(m_strLinkName.IsEmpty())
		{
			MessageBox("注意: 功能的相关连接模板不能为空!");
		}
	}
	CDialog::OnOK();
}

void CTalkCreateDlg::OnRadioTalkType1() 
{
	// TODO: Add your control notification handler code here
	m_dwID = m_dwOldID;
	m_bWindow = true;
	EnableCreateWindow(m_bWindow);
	UpdateData(false);
}

void CTalkCreateDlg::OnRadioTalkType2() 
{
	// TODO: Add your control notification handler code here
	m_bWindow = false;
	EnableCreateWindow(m_bWindow);
	m_dwID = m_dwOldID;
	UpdateData(false);
}

void CTalkCreateDlg::OnSelchangeComboService() 
{
	// TODO: Add your control notification handler code here
	CComboBox *pc = (CComboBox *)GetDlgItem(IDC_COMBO_SERVICE);
	EnableLinkCtrl(false);
	EnableParamsCtrl(false);
	UpdateData(true);
	int n = pc->GetCurSel();
	CString txt;
	pc->GetLBText(n,txt);
	for(int i = 0; i<SERVICE_COUNT; i++)
	{
		if(strcmp(m_strService[i],txt)==0)
		{
			m_dwServiceID = 0x80000000 + i;
			
			m_dwServiceLinkID = 0;
			m_strLinkName = "";
			
			if(m_dwServiceID == NPC_SKILL)
			{
				m_strRootDir = "BaseData\\技能";
				EnableLinkCtrl(true);
			}else if(m_dwServiceID == NPC_GIVE_TASK)
			{
				m_strRootDir = "BaseData\\TaskTemplate";
				EnableLinkCtrl(true);
			}else if(m_dwServiceID == NPC_COMPLETE_TASK)
			{
				m_strRootDir = "BaseData\\TaskTemplate";
				EnableLinkCtrl(true);
			}else if(m_dwServiceID == TALK_GIVEUP_TASK)
			{
				m_strRootDir = "BaseData\\TaskTemplate";
				EnableLinkCtrl(true);
			}else if(m_dwServiceID == NPC_GIVE_TASK_MATTER)
			{
				m_strRootDir = "BaseData\\TaskTemplate";
				EnableLinkCtrl(true);
			}else if(m_dwServiceID == NPC_TRANSMIT)
			{
				InitParamsCombox();
				EnableParamsCtrl(true);
			}else if(m_dwServiceID == NPC_TRANSPORT)
			{
				InitParamsCombox();
				EnableParamsCtrl(true);
			}
			UpdateData(false);
			return;
		}
	}
	m_dwServiceID = 0x80000000;
}

void CTalkCreateDlg::OnButtonServiceLink() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CTaskIDSelDlg dlg;
	//dlg.m_strRoot = m_strRootDir;
	//dlg.m_strExt = m_strExt;

	dlg.m_bOneOnly = true;
	dlg.m_ulTopTaskToSel = m_dwParamToSel2;
	dlg.m_ulID = m_dwServiceLinkID;
	
	if (dlg.DoModal() != IDOK) return;
	
	m_strLinkName = dlg.m_strName;
	m_dwServiceLinkID = dlg.m_ulID;
	UpdateData(false);
}

void CTalkCreateDlg::OnSelchangeComboServiceParams() 
{
	// TODO: Add your control notification handler code here
	CComboBox *pc = (CComboBox *)GetDlgItem(IDC_COMBO_SERVICE_PARAMS);
	int n = pc->GetCurSel();
	CString txt;
	pc->GetLBText(n,txt);
	
	if(m_dwServiceID == NPC_TRANSMIT)
			txt.Replace("传送点:","");
	else if(m_dwServiceID == NPC_TRANSPORT)
			txt.Replace("路线:","");

	m_dwServiceLinkID = (int)atof(txt);
}

void CTalkCreateDlg::OnRadioTalkType3() 
{
	// TODO: Add your control notification handler code here
	m_bWindow = true;
	m_dwID = 0xc0000000 + m_dwOldID;
	UpdateData(false);
	EnableCreateWindow(m_bWindow);
}
