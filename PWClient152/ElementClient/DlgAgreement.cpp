// File		: DlgAgreement.cpp
// Creator	: Feng Ning
// Date		: 2010/10/21

#include "DlgAgreement.h"
#include "EC_LoginSwitch.h"
#include "EC_LoginUIMan.h"
#include "EC_Configs.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_UIConfigs.h"
#include "Arc/Asia/EC_ArcAsia.h"
#include "DlgLogin.h"

#include "AFI.h"
#include "AIniFile.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAgreement, CDlgTheme)
AUI_ON_COMMAND("decline", OnCommand_Decline)
AUI_ON_COMMAND("agree", OnCommand_Agree)
AUI_ON_COMMAND("IDOK", OnCommand_Confirm)
AUI_ON_COMMAND("confirm", OnCommand_Confirm)
AUI_END_COMMAND_MAP()

CDlgAgreement::CDlgAgreement()
{
}

CDlgAgreement::~CDlgAgreement()
{
}

bool CDlgAgreement::OnInitDialog()
{
	if(!CDlgTheme::OnInitDialog())
	{
		return false;
	}

	//
	return true;
}

void CDlgAgreement::OnShowDialog()
{
	CDlgTheme::OnShowDialog();

	// ensure the login ui is not shown
	CDlgLogin* pDlg = dynamic_cast<CDlgLogin*>(m_pAUIManager->GetDialog("Win_Login"));
	pDlg->Show(false);

	//	检查自动登录方式
	CECLoginSwitch::Instance().CheckAutoLogin();
	
	//	设置始终自动登录标志
	bool bAlwaysTryAutoLogin = false;
	if (CECUIConfig::Instance().GetLoginUI().bAlwaysTryAutoLogin){
		bAlwaysTryAutoLogin = CECLoginSwitch::Instance().CanAutoLogin();
	}
	pDlg->SetAlwaysTryAutoLogin(bAlwaysTryAutoLogin);

	//	尝试自动登录
	if (CECLoginSwitch::Instance().CanAutoLogin())
	{
		//	跳过通告等页面直接登录直至出现角色列表或登录失败
		Show(false);
		OnCommand_Confirm(NULL);
	}
	else if( !LoadText() )
	{
		Show(false);
		OnCommand_Confirm(NULL);
	}
}

bool CDlgAgreement::Release()
{
	//
	return CDlgTheme::Release();
}

void CDlgAgreement::OnTick()
{
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Agreement"));    

	bool bEnable = ( pText->GetFirstLine() + pText->GetLinesPP() == pText->GetLines() );
	GetDlgItem("Btn_Agree")->Enable(bEnable);

	CDlgTheme::OnTick();
}

bool CDlgAgreement::Render()
{
	//
	return CDlgTheme::Render();
}

void CDlgAgreement::OnCommand_Decline(const char* szCommand)
{
	PostQuitMessage(0);
}

void CDlgAgreement::OnCommand_Agree(const char* szCommand)
{
	Show(false);
	OnCommand_Confirm(NULL);
}

void CDlgAgreement::OnCommand_Confirm(const char* szCommand)
{
	CECLoginUIMan *pLoginUIMan = (CECLoginUIMan *)m_pAUIManager;
	pLoginUIMan->ShowLoginDlg();
}

bool CDlgAgreement::LoadText()
{
	// load agreement from file
	FILE *file;
	char szFile[MAX_PATH];
	
	sprintf(szFile, "%s\\Info\\Agreement.txt", af_GetBaseDir());
	file = fopen(szFile, "rb");
	
	if( file )
	{
		int nLen;
		BYTE szHeader[2] = { 0, 0 };
		
		fread(szHeader, sizeof(BYTE), 2, file);
		if( szHeader[0] == 0xFF && szHeader[1] == 0xFE )
		{
			fseek(file, 0, SEEK_END);
			nLen = ftell(file) - 2;
			if( nLen > 0 )
			{
				ACHAR *pszText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (nLen / 2 + 1));
				
				fseek(file, 2, SEEK_SET);
				fread(pszText, sizeof(ACHAR), nLen / 2, file);
				pszText[nLen / 2] = 0;
				GetDlgItem("Txt_Agreement")->SetText(pszText);
				
				a_freetemp(pszText);
			}
		}
		fclose(file);
		
		return true;
	}
	
	return false;
}
