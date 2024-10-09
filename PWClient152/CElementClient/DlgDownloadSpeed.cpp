// Filename	: DlgDownloadSpeed.cpp
// Creator	: Shizhenhua
// Date		: 2013/8/3

#include "DlgDownloadSpeed.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_MCDownload.h"

#include <AUIComboBox.h>

#define new A_DEBUG_NEW


static int l_iSpeedValue[] =
{ 65536, 262144, 524288, 1048576, 2097152, 4194304, 0 };

AUI_BEGIN_COMMAND_MAP(CDlgDownloadSpeed, CDlgBase)
AUI_ON_COMMAND("Btn_Start", OnCommand_Start)
AUI_END_COMMAND_MAP()

CDlgDownloadSpeed::CDlgDownloadSpeed()
{
	m_pCmbDownSpeed = NULL;
	m_pCmbUpSpeed = NULL;
}

bool CDlgDownloadSpeed::OnInitDialog()
{
	DDX_Control("Combo_DownSpeed", m_pCmbDownSpeed);
	DDX_Control("Combo_UpSpeed", m_pCmbUpSpeed);

	for( int i=0;i<sizeof(l_iSpeedValue)/sizeof(int);i++ )
	{
		m_pCmbDownSpeed->AddString(GetStringFromTable(10705+i));
		m_pCmbUpSpeed->AddString(GetStringFromTable(10705+i));
	}

	m_pCmbDownSpeed->SetCurSel(3);
	m_pCmbUpSpeed->SetCurSel(1);
	return true;
}

void CDlgDownloadSpeed::OnCommand_Start(const char* szCommand)
{
	int iDownSel = m_pCmbDownSpeed->GetCurSel();
	int iUpSel = m_pCmbUpSpeed->GetCurSel();
	if( iDownSel >= 0 && iDownSel < sizeof(l_iSpeedValue)/sizeof(int) &&
		iUpSel >= 0 && iUpSel < sizeof(l_iSpeedValue)/sizeof(int) )
	{
		CECMCDownload::GetInstance().SendDownload(l_iSpeedValue[iDownSel], l_iSpeedValue[iUpSel]);
	}

	Show(false);
}
