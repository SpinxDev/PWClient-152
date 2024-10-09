// Filename	: DlgOptimizeBase.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#include "DlgOptimizeBase.h"
#include "EC_Optimize.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgOptimizeBase, CDlgBase)
AUI_ON_COMMAND("Btn_SwitchMem", OnCommand_Switch)
AUI_ON_COMMAND("Btn_SwitchGfx", OnCommand_Switch)
AUI_ON_COMMAND("Btn_SwitchFunction", OnCommand_Switch)
AUI_ON_COMMAND("Btn_OptimizeOS", OnCommand_OptimizeOS)
AUI_END_COMMAND_MAP()

CDlgOptimizeBase::CDlgOptimizeBase()
:m_pTxt_Memory(NULL)
,m_pTxt_FPS(NULL)
{
}

CDlgOptimizeBase::~CDlgOptimizeBase()
{
}

bool CDlgOptimizeBase::OnInitDialog()
{
	DDX_Control("Txt_Memory", m_pTxt_Memory);
	DDX_Control("Txt_FPS", m_pTxt_FPS);
    return true;
}

void CDlgOptimizeBase::OnTick()
{
    UpdateSysView();
}

void CDlgOptimizeBase::UpdateSysView()
{
    ACString str;
    SIZE_T mem(0);
    if (CECOptimize::Instance().GetVirtualSize(mem)){
		str = GetBaseUIMan()->GetFormatNumber(mem/1024);
    }else{
        str.Empty();
    }
	m_pTxt_Memory->SetText(str);
    
    str.Format(_AL("%.1f"), GetGame()->GetAverageFrameRate());
    m_pTxt_FPS->SetText(str);    
}

void CDlgOptimizeBase::OnCommand_Switch(const char *szCommand)
{
	PAUIDIALOG pDlgOptimizeMem = GetGameUIMan()->GetDialog("Win_OptimizeMem");
	PAUIDIALOG pDlgOptimizeGfx = GetGameUIMan()->GetDialog("Win_OptimizeGfx");
	PAUIDIALOG pDlgOptimizeFunc = GetGameUIMan()->GetDialog("Win_OptimizeFunc");

	PAUIDIALOG pDlgCurrent = NULL;
	if (pDlgOptimizeMem->IsShow()){
		pDlgCurrent = pDlgOptimizeMem;
	} else if (pDlgOptimizeGfx->IsShow()){
		pDlgCurrent = pDlgOptimizeGfx;
	} else if (pDlgOptimizeFunc->IsShow()) {
		pDlgCurrent = pDlgOptimizeFunc;
	}
	if (!pDlgCurrent) return;

	PAUIDIALOG pDlgSwitchTo = NULL;
	AString strCommand = szCommand;
	if (strCommand == "Btn_SwitchMem"){
		pDlgSwitchTo = pDlgOptimizeMem;
	}else if (strCommand == "Btn_SwitchGfx"){
		pDlgSwitchTo = pDlgOptimizeGfx;
	} else if (strCommand == "Btn_SwitchFunction") {
		pDlgSwitchTo = pDlgOptimizeFunc;
	}

	if (!pDlgSwitchTo) return;
	if (pDlgCurrent == pDlgSwitchTo) return;
	
	POINT pos = pDlgCurrent->GetPos();
	pDlgCurrent->OnCommand("IDCANCEL");

	pDlgSwitchTo->SetPosEx(pos.x, pos.y);	
	pDlgSwitchTo->Show(true);
	GetGameUIMan()->m_pDlgOptimizeCurrent = pDlgSwitchTo;
}

void CDlgOptimizeBase::OnCommand_OptimizeOS(const char *szCommand)
{
	if (PAUIDIALOG pDlgOptimizeOS = GetGameUIMan()->GetDialog("Win_OptimizeOS")){
		pDlgOptimizeOS->Show(!pDlgOptimizeOS->IsShow());
	}
}