// Filename	: DlgOptimizeOS.cpp
// Creator	: Xu Wenbin
// Date		: 2014/11/10

#include "DlgOptimizeOS.h"
#include "EC_Utility.h"
#include "EC_Game.h"
#include "EC_Configs.h"

#include <AFI.h>

//	local
static AString GetMoreMemoryFileDirectory(){
	return AC2AS(glb_GetExeDirectory());
}
static AString GetEnableMoreMemoryFilePath(){
	return GetMoreMemoryFileDirectory() + "EnableMoreMem_Vista_Win7_32bit.bat";
}
static AString GetDisableMoreMemoryFilePath(){
	return GetMoreMemoryFileDirectory() + "DisableMoreMem_Vista_Win7_32bit.bat";
}

//	CDlgOptimizeOS
AUI_BEGIN_COMMAND_MAP(CDlgOptimizeOS, CDlgBase)
AUI_ON_COMMAND("Btn_Enable", OnCommand_Enable)
AUI_ON_COMMAND("Btn_Disable", OnCommand_Disable)
AUI_END_COMMAND_MAP()

CDlgOptimizeOS::CDlgOptimizeOS()
: m_pBtn_Enable(NULL)
, m_pBtn_Disable(NULL)
{
}

bool CDlgOptimizeOS::OnInitDialog()
{
	DDX_Control("Btn_Enable", m_pBtn_Enable);
	DDX_Control("Btn_Disable", m_pBtn_Disable);
    return CDlgBase::OnInitDialog();
}

bool CDlgOptimizeOS::CanEnable(){
	return !GetGame()->GetConfigs()->GetSystemSettings().bFullScreen
		&& af_IsFileExist(GetEnableMoreMemoryFilePath());
}

bool CDlgOptimizeOS::CanDisable(){
	return !GetGame()->GetConfigs()->GetSystemSettings().bFullScreen
		&& af_IsFileExist(GetDisableMoreMemoryFilePath());
}

void CDlgOptimizeOS::OnTick(){
	CDlgBase::OnTick();
	m_pBtn_Enable->Enable(CanEnable());
	m_pBtn_Disable->Enable(CanDisable());
}

void CDlgOptimizeOS::OnCommand_Enable(const char *szCommand){
	glb_ExploreFile(GetEnableMoreMemoryFilePath());
}

void CDlgOptimizeOS::OnCommand_Disable(const char *szCommand){
	glb_ExploreFile(GetDisableMoreMemoryFilePath());
}