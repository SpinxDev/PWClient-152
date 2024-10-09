// Filename	: DlgOptimizeMem.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#include "DlgOptimizeMem.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Configs.h"
#include "EC_GameUIMan.h"
#include "DlgSetting.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgOptimizeMem, CDlgOptimizeBase)
AUI_ON_COMMAND("Btn_Decide", OnCommand_Confirm)
AUI_ON_COMMAND("Chk_OptimizeMem", OnCommand_OptimizeMem)
AUI_END_COMMAND_MAP()


CDlgOptimizeMem::CDlgOptimizeMem()
{
	m_bAutoOptimize = true;
	m_iMemState = CECMemSimplify::MEMUSAGE_NORMAL;
}

CDlgOptimizeMem::~CDlgOptimizeMem()
{
}

bool CDlgOptimizeMem::OnInitDialog()
{
	m_pChk_Player = (PAUICHECKBOX)GetDlgItem("Chk_Player");
	m_pChk_Surround = (PAUICHECKBOX)GetDlgItem("Chk_Surround");
	m_pChk_Sight = (PAUICHECKBOX)GetDlgItem("Chk_Sight");
	return CDlgOptimizeBase::OnInitDialog();
}

void CDlgOptimizeMem::DoDataExchange(bool bSave)
{
	CDlgOptimizeBase::DoDataExchange(bSave);

	DDX_CheckBox(bSave, "Chk_OptimizeMem", m_bAutoOptimize);
	DDX_RadioButton(bSave, 0, m_iMemState);
}

void CDlgOptimizeMem::OnShowDialog()
{
	UpdateMemView();
	OnCommand_OptimizeMem(NULL);
}

void CDlgOptimizeMem::UpdateMemView()
{
	CECMemSimplify* pMemSimplify = g_pGame->GetGameRun()->GetMemSimplify();
	m_bAutoOptimize = pMemSimplify->IsAutoOptimize();
	m_iMemState = pMemSimplify->GetGlobalMemUsage();

	UpdateData(false);
}

void CDlgOptimizeMem::OnCommand_Confirm(const char *szCommand)
{
	UpdateData();

	CECMemSimplify* pMemSimplify = g_pGame->GetGameRun()->GetMemSimplify();
	pMemSimplify->SetAutoOptimize(m_bAutoOptimize);
	if( !m_bAutoOptimize ) pMemSimplify->TransferUsageAll(m_iMemState);

	g_pGame->GetConfigs()->SetModelOptimize(m_bAutoOptimize, m_iMemState);

    Show(false);
}

void CDlgOptimizeMem::OnCommand_OptimizeMem(const char* szCommand)
{
	UpdateData();
	GetDlgItem("Rdo_MemState1")->Enable(!m_bAutoOptimize);
	GetDlgItem("Rdo_MemState2")->Enable(!m_bAutoOptimize);
	GetDlgItem("Rdo_MemState3")->Enable(!m_bAutoOptimize);
	GetDlgItem("Rdo_MemState4")->Enable(!m_bAutoOptimize);
}
