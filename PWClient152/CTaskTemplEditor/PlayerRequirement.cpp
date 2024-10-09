// PlayerRequirement.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "PlayerRequirement.h"
#include "TaskTempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlayerRequirement dialog


PlayerRequirement::PlayerRequirement(Kill_Player_Requirements* pRequirement, CWnd* pParent /*=NULL*/)
	: m_pRequirement(pRequirement),CDialog(PlayerRequirement::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlayerRequirement)
	m_bWuxia = FALSE;
	m_Jianling = FALSE;
	m_Cike = FALSE;
	m_Yuling = FALSE;
	m_Yaoshou = FALSE;
	m_Fashi = FALSE;
	m_Meiling = FALSE;
	m_Wushi = FALSE;
	m_Yumang = FALSE;
	m_Yaojing = FALSE;
	m_Anyin = FALSE;
	m_Huaguang = FALSE;
	m_Huiye = FALSE;
	m_MinLevel = 0;
	m_MaxLevel = 0;
	m_nGender = 0;
	m_YueXian = FALSE;
	m_YeYing = FALSE;
	//}}AFX_DATA_INIT
}


void PlayerRequirement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlayerRequirement)
	DDX_Check(pDX, IDC_WUXIA, m_bWuxia);
	DDX_Check(pDX, IDC_JIANLING, m_Jianling);
	DDX_Check(pDX, IDC_CIKE, m_Cike);
	DDX_Check(pDX, IDC_YULING, m_Yuling);
	DDX_Check(pDX, IDC_YAOSHOU, m_Yaoshou);
	DDX_Check(pDX, IDC_FASHI, m_Fashi);
	DDX_Check(pDX, IDC_MEILING, m_Meiling);
	DDX_Check(pDX, IDC_WUSHI, m_Wushi);
	DDX_Check(pDX, IDC_YUMANG, m_Yumang);
	DDX_Check(pDX, IDC_YAOJING, m_Yaojing);
	DDX_Check(pDX, IDC_ANYIN, m_Anyin);
	DDX_Check(pDX, IDC_HUAGUANG, m_Huaguang);
	DDX_Check(pDX, IDC_HUIYE, m_Huiye);
	DDX_Check(pDX, IDC_CHK_SHADOW, m_YeYing);
	DDX_Check(pDX, IDC_CHK_FAIRY, m_YueXian);
	DDX_Text(pDX, IDC_MINLEVEL, m_MinLevel);
	DDV_MinMaxUInt(pDX, m_MinLevel, 0, 150);
	DDX_Text(pDX, IDC_MAXLEVEL, m_MaxLevel);
	DDV_MinMaxUInt(pDX, m_MaxLevel, 1, 150);
	DDX_Radio(pDX, IDC_RADIO1, m_nGender);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PlayerRequirement, CDialog)
	//{{AFX_MSG_MAP(PlayerRequirement)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayerRequirement message handlers

BOOL PlayerRequirement::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bWuxia = (m_pRequirement->m_ulOccupations & 0x1) != 0;
	m_Fashi = (m_pRequirement->m_ulOccupations & 0x2) != 0;
	m_Wushi = (m_pRequirement->m_ulOccupations & 0x4) != 0;
	m_Yaojing = (m_pRequirement->m_ulOccupations & 0x8) != 0;
	m_Yaoshou = (m_pRequirement->m_ulOccupations & 0x10) != 0;
	m_Cike = (m_pRequirement->m_ulOccupations & 0x20) != 0;
	m_Yumang = (m_pRequirement->m_ulOccupations & 0x40) != 0;
	m_Yuling = (m_pRequirement->m_ulOccupations & 0x80) != 0;
	m_Jianling = (m_pRequirement->m_ulOccupations & 0x100) != 0;
	m_Meiling = (m_pRequirement->m_ulOccupations & 0x200) != 0;
	m_YeYing = (m_pRequirement->m_ulOccupations & 0x400) != 0;
	m_YueXian = (m_pRequirement->m_ulOccupations & 0x800) != 0;

	m_Huaguang = (m_pRequirement->m_iForce & 0x1) != 0;
	m_Anyin = (m_pRequirement->m_iForce & 0x2) != 0;
	m_Huiye = (m_pRequirement->m_iForce & 0x4) != 0;
	m_nGender = m_pRequirement->m_iGender;
	m_MinLevel = m_pRequirement->m_iMinLevel;
	m_MaxLevel = m_pRequirement->m_iMaxLevel;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PlayerRequirement::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	m_pRequirement->m_ulOccupations = 0;
	m_pRequirement->m_ulOccupations |= m_bWuxia ? 1 << 0 : 0;
	m_pRequirement->m_ulOccupations |= m_Fashi ? 1 << 1 : 0;
	m_pRequirement->m_ulOccupations |= m_Wushi ? 1 << 2 : 0;
	m_pRequirement->m_ulOccupations |= m_Yaojing ? 1 << 3 : 0;
	m_pRequirement->m_ulOccupations |= m_Yaoshou ? 1 << 4 : 0;
	m_pRequirement->m_ulOccupations |= m_Cike ? 1 << 5 : 0;
	m_pRequirement->m_ulOccupations |= m_Yumang ? 1 << 6 : 0;
	m_pRequirement->m_ulOccupations |= m_Yuling ? 1 << 7 : 0;
	m_pRequirement->m_ulOccupations |= m_Jianling ? 1 << 8 : 0;
	m_pRequirement->m_ulOccupations |= m_Meiling ? 1 << 9 : 0;
	m_pRequirement->m_ulOccupations |= m_YeYing ? 1 << 10 : 0;
	m_pRequirement->m_ulOccupations |= m_YueXian ? 1 << 11 : 0;


	m_pRequirement->m_iForce = 0;
	m_pRequirement->m_iForce |= m_Huaguang ? 1 << 0 : 0;
	m_pRequirement->m_iForce |= m_Anyin ? 1 << 1 : 0;
	m_pRequirement->m_iForce |= m_Huiye ? 1 << 2 : 0;
	m_pRequirement->m_iGender = m_nGender;
	m_pRequirement->m_iMinLevel = m_MinLevel;
	m_pRequirement->m_iMaxLevel = m_MaxLevel;
	CDialog::OnOK();
}
