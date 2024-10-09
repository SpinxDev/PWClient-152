// NpcSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NpcSelDlg.h"
#include "elementeditor.h"
#include "NpcPropertyDlg.h"
#include "SceneAIGenerator.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CNpcSelDlg dialog


CNpcSelDlg::CNpcSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNpcSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSceneAIGenerator = NULL;
}


void CNpcSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNpcSelDlg)
	DDX_Control(pDX, IDC_LIST_AI, m_listAI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNpcSelDlg, CDialog)
	//{{AFX_MSG_MAP(CNpcSelDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_NEW_AI, OnNewAi)
	ON_BN_CLICKED(IDC_MODIFY_AI, OnModifyAi)
	ON_BN_CLICKED(IDC_DELETE_AI, OnDeleteAi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNpcSelDlg message handlers

BOOL CNpcSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	FreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNpcSelDlg::FreshList()
{
	int num = m_listAI.GetCount();
	
	for(int i = 0; i < num; i++)
		m_listAI.DeleteString(0);

	if(m_pSceneAIGenerator)
	{
		int num = m_pSceneAIGenerator->GetNPCNum();
		for(int i = 0; i < num; i++)
		{
			NPC_PROPERTY *pProperty = m_pSceneAIGenerator->GetNPCProperty(i);
			int idx = m_listAI.AddString(pProperty->strNpcName);
			m_listAI.SetItemData(idx,(unsigned  long)pProperty);
		}
	}
}

AString CNpcSelDlg::GetNpcName(DWORD id)
{
	AString strName;
	
	DATA_TYPE type = DT_MONSTER_ESSENCE;
	MONSTER_ESSENCE* pData;
	pData = (MONSTER_ESSENCE*)g_dataMan.get_data_ptr(id,ID_SPACE_ESSENCE,type);
	if(pData) strName = WC2AS(pData->name);
	return  strName;
}

void CNpcSelDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
}

void CNpcSelDlg::OnNewAi() 
{
	// TODO: Add your control notification handler code here
	CNpcPropertyDlg dlg;
	dlg.Init(&g_dataMan);
	if(dlg.DoModal()==IDOK)
	{
		/*
		NPC_PROPERTY *pnew = m_pSceneAIGenerator->GetNPCProperty(dlg.m_npcData.strNpcName);
		if(pnew)
		{
			MessageBox("该类型的怪物已经加入到怪物产生器中，加入失败!");
			return;
		}*/
		NPC_PROPERTY* npc = new NPC_PROPERTY;
		*npc = dlg.m_npcData;
		m_pSceneAIGenerator->AddNPC(npc);
		FreshList();
	}
}



void CNpcSelDlg::OnModifyAi() 
{
	// TODO: Add your control notification handler code here
	CNpcPropertyDlg dlg;
	dlg.Init(&g_dataMan);
	int index = m_listAI.GetCurSel();
	if(index==-1) return;
	//CString name;
	//m_listAI.GetText(index,name);
	NPC_PROPERTY* pro = (NPC_PROPERTY*)m_listAI.GetItemData(index);
	
	CString str;
	dlg.m_bModified = true;
	dlg.m_npcData = *pro;
	if(dlg.DoModal()==IDOK)
	{
		/*
		NPC_PROPERTY *pnew = m_pSceneAIGenerator->GetNPCProperty(dlg.m_npcData.strNpcName);
		if(pnew)
		{
			if(pnew!=pro)
			{
				MessageBox("该类型的怪物已经加入到怪物产生器中，修改无效!");
				return;
			}
		}*/
		*pro = dlg.m_npcData;
		FreshList();
	}
}

void CNpcSelDlg::OnDeleteAi() 
{
	// TODO: Add your control notification handler code here
	int index = m_listAI.GetCurSel();
	if(index==-1) return;
	m_pSceneAIGenerator->DeleteNPC(index);
	FreshList();
}
