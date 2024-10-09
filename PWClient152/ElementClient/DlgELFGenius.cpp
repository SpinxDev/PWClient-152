// Filename	: DlgELFProp.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#include "WindowsX.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFGenius.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "elementdataman.h"

#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUIStillImageButton.h"
#include "EC_HostGoblin.h"


#define new A_DEBUG_NEW
#define MAX_LEVEL 8

AUI_BEGIN_COMMAND_MAP(CDlgELFGenius, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_*",			OnCommand_add)
AUI_ON_COMMAND("skillview",		OnCommand_skillview)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFGenius, CDlgBase)

AUI_END_EVENT_MAP()

CDlgELFGenius::CDlgELFGenius()
{
}

CDlgELFGenius::~CDlgELFGenius()
{
}

bool CDlgELFGenius::OnInitDialog()
{
	char szName[20];
	for (int i = 0; i < 5; i++)
	{
		sprintf(szName, "Img_%d", i);
		m_pImg_Genius[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		sprintf(szName, "Txt_%d", i);
		m_pTxt_Genius[i] = (PAUILABEL)GetDlgItem(szName);
	}
	return true;
}

void CDlgELFGenius::OnShowDialog()
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	int iGenius[5];
	char szName[20];
	int i, iTep = 0;
	ACHAR szTmp[20];
	for (i = 0; i < 5; i++)
	{
		iGenius[i] = pHostGoblin->GetGenius(i);
		m_pImg_Genius[i]->FixFrame(iGenius[i]);
		a_sprintf(szTmp, _AL("%d"), iGenius[i]);
		m_pTxt_Genius[i]->SetText(szTmp);
		iTep += iGenius[i];
	}
	int iRemain = pHostGoblin->UnusedGeniusPt();
	a_sprintf(szTmp, _AL("%d"), iRemain);
	((PAUILABEL)GetDlgItem("Txt_Remain"))->SetText(szTmp);
	int iTotal = iTep + iRemain;
	a_sprintf(szTmp, _AL("%d"), iTotal);
	((PAUILABEL)GetDlgItem("Txt_Total"))->SetText(szTmp);
	for (i = 0; i < 5; i++)
	{
		sprintf(szName, "Btn_%d", i);
		if (iRemain == 0 || iGenius[i] == MAX_LEVEL)
		{
			((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->Enable(false);
		}
		else
			((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->Enable(true);
	}
}

void CDlgELFGenius::OnCommand_add(const char * szCommand)
{
	//int pt[5] = {0, 0, 0, 0, 0};
	int i = atoi(szCommand + strlen("Btn_"));
	PAUIDIALOG pMsgBox;
	ACString str;
	str.Format(GetStringFromTable(7153), GetStringFromTable(7110 + i));
	GetGameUIMan()->MessageBox("Game_GoblinAddGenius", str, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(i);
}

void CDlgELFGenius::OnCommand_skillview(const char * szCommand)
{
	PAUIDIALOG pDlg1 = GetGameUIMan()->GetDialog("Win_Elf_Skilltree1");
	PAUIDIALOG pDlg2 = GetGameUIMan()->GetDialog("Win_Elf_Skilltree2");
	if (!pDlg1->IsShow() && !pDlg2->IsShow())
	{
		pDlg1->Show(true);
	}
}

bool CDlgELFGenius::Render(void)
{
	if( !IsShow()) return false;
	CECHostGoblin *pHostGobin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGobin)
	{
		Show(false);
	}
	RefreshHostDetails();
	return CDlgBase::Render();
}

void CDlgELFGenius::RefreshHostDetails()
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		OnCommand_CANCEL("IDCANCEL");
		return;
	}
	int iGenius[5];
	int i, iTep = 0;
	ACHAR szTmp[20];
	char szName[20];

	for (i = 0; i < 5; i++)
	{
		iGenius[i] = pHostGoblin->GetGenius(i);
		m_pImg_Genius[i]->FixFrame(iGenius[i]);
		a_sprintf(szTmp, _AL("%d"), iGenius[i]);
		m_pTxt_Genius[i]->SetText(szTmp);
		iTep += iGenius[i];
	}
	int iRemain = pHostGoblin->UnusedGeniusPt();
	a_sprintf(szTmp, _AL("%d"), iRemain);
	((PAUILABEL)GetDlgItem("Txt_Remain"))->SetText(szTmp);

	int iTotal = iRemain + iTep;
	a_sprintf(szTmp, _AL("%d"), iTotal);
	((PAUILABEL)GetDlgItem("Txt_Total"))->SetText(szTmp);

	
	for (i = 0; i < 5; i++)
	{
		sprintf(szName, "Btn_%d", i);
		if (iRemain == 0 || iGenius[i] == MAX_LEVEL)
		{
			((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->Enable(false);
		}
		else
			((PAUISTILLIMAGEBUTTON)GetDlgItem(szName))->Enable(true);
	}
}