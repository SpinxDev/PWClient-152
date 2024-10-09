// PlayerWanted.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "PlayerWanted.h"
#include "TaskTempl.h"
#include "PlayerProb.h"
#include "TemplIDSelDlg.h"
#include "Occupation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlayerWanted dialog


PlayerWanted::PlayerWanted(PLAYER_WANTED* pPlayer, unsigned long* pCount, CWnd* pParent /*=NULL*/)
	:m_pPlayerWanted(pPlayer), m_pCount(pCount), CDialog(PlayerWanted::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlayerWanted)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PlayerWanted::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlayerWanted)
	DDX_Control(pDX, IDC_PLAYERS, m_PlayerList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PlayerWanted, CDialog)
	//{{AFX_MSG_MAP(PlayerWanted)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT_PLAYER, OnEditPlayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayerWanted message handlers

void PlayerWanted::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if (*m_pCount >= MAX_PLAYER_WANTED)
	{
		AfxMessageBox("已到达上限", MB_ICONSTOP);
		return;
	}
	
	PLAYER_WANTED& pw = m_pPlayerWanted[*m_pCount];
	pw.m_fDropProb = 1.0f;
	pw.m_bDropCmnItem = false;
	pw.m_ulDropItemCount = 1;
	pw.m_ulDropItemId = 0;
	pw.m_ulPlayerNum = 1;
	
	PlayerProb dlg(&m_pPlayerWanted[*m_pCount],*m_pCount);
	
	if (dlg.DoModal() == IDOK)
	{
		(*m_pCount)++;
		UpdateList();
	}
}

void PlayerWanted::OnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_PlayerList.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;
	
	unsigned long uItem = m_PlayerList.GetNextSelectedItem(pos);
	
	for (unsigned long i = uItem+1; i < *m_pCount; i++)
		m_pPlayerWanted[i-1] = m_pPlayerWanted[i];
	
	(*m_pCount)--;
	UpdateList();		
}

void PlayerWanted::OnEditPlayer() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_PlayerList.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned long uItem = m_PlayerList.GetNextSelectedItem(pos);
	
	PLAYER_WANTED& pw = m_pPlayerWanted[uItem];
	
	PlayerProb dlg(&pw,uItem);
	
	if (dlg.DoModal() == IDOK)
		UpdateList();	
}

BOOL PlayerWanted::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_PlayerList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_PlayerList.InsertColumn(0, _T("玩家类别"), LVCFMT_LEFT, 100);
	m_PlayerList.InsertColumn(1, _T("数量"), LVCFMT_LEFT, 40);
	m_PlayerList.InsertColumn(2, _T("掉落物品名称"), LVCFMT_LEFT, 100);
	m_PlayerList.InsertColumn(3, _T("掉落数量"), LVCFMT_LEFT, 40);
	m_PlayerList.InsertColumn(4, _T("概率"), LVCFMT_LEFT, 60);
	
	UpdateList();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
static const char* _occup[] =
{
	"武侠",
		"法师",
		"巫师",
		"妖精",
		"妖兽",
		"刺客",
		"羽芒",
		"羽灵",
		"剑灵",
		"魅灵",
		"夜影",
		"月仙"
};
static const char* _force[] = 
{
	"华光会",
	"暗隐会",
	"辉夜军",
};
void PlayerWanted::UpdateList()
{
	m_PlayerList.DeleteAllItems();
	
	for (unsigned long i = 0; i < *m_pCount; i++)
	{
		PLAYER_WANTED& pw = m_pPlayerWanted[i];
		int j = 0;
		CString strOccupation;
		for (j = 0;j < MAX_OCCUPATIONS; ++j)
		{
			if (pw.m_Requirements.IsMeetAllOccupation())
			{
				break;
			}
			if (pw.m_Requirements.m_ulOccupations & 1 << j)
			{
				if (strOccupation.GetLength() > 0)
				{
					strOccupation += ("/");
				}
				strOccupation += _occup[j];
			}
		}
		if (strOccupation.GetLength() > 0)
		{
			strOccupation += "; ";
		}
		CString strLevel;
		strLevel.Format("%d-%d级; ",pw.m_Requirements.m_iMinLevel,pw.m_Requirements.m_iMaxLevel);
		
		CString strGender;
		if (pw.m_Requirements.m_iGender == 1)
		{
			strGender = "男性; ";
		}
		if (pw.m_Requirements.m_iGender == 2)
		{
			strGender = "女性; ";
		}

		CString strForce;
		for (j = 0;j < 3; ++j)
		{
			if (pw.m_Requirements.m_iForce & 1 << j)
			{
				if (strForce.GetLength() > 0)
				{
					strForce += "/";
				}
				strForce += _force[j];
			}
		}

		m_PlayerList.InsertItem(i,strOccupation + strLevel + strGender + strForce);
		
		CString strNum;
		strNum.Format("%d", pw.m_ulPlayerNum);
		m_PlayerList.SetItemText(i, 1, strNum);
		
		m_PlayerList.SetItemText(i,2,GetBaseDataNameByID(pw.m_ulDropItemId));
		
		strNum.Format("%d", pw.m_ulDropItemCount);
		m_PlayerList.SetItemText(i, 3, strNum);
		
		CString strProb;
		strProb.Format("%f", pw.m_fDropProb);
		m_PlayerList.SetItemText(i, 4, strProb);
	}
}
