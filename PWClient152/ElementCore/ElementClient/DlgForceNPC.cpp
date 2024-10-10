// Filename	: DlgForceNPC.cpp
// Creator	: Feng Ning
// Date		: 2011/10/19

#include "DlgForceNPC.h"
#include "EC_Player.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include "AFI.h"
#include "A2DSprite.h"
#include "A3DDevice.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUITextarea.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "AUICTranslate.h"
#include "EC_Utility.h"
#include "AUIEditbox.h"
#include "EC_ForceMgr.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgForceNPC, CDlgBase)
AUI_ON_COMMAND("Btn_Join",		OnCommand_Join)
AUI_ON_COMMAND("Btn_Quit",		OnCommand_Quit)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgForceNPC, CDlgBase)
AUI_END_EVENT_MAP()

CDlgForceNPC::CDlgForceNPC()
{
	m_pConfig = NULL;
}

CDlgForceNPC::~CDlgForceNPC()
{
}

void CDlgForceNPC::OnCommand_CANCEL(const char * szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgForceNPC::OnCommand_Join(const char * szCommand)
{
	if(!m_pConfig || GetHostPlayer()->GetForce() != 0)
		return;

	ACString strMsg;
	
	strMsg.Format(GetStringFromTable(9402), A3DCOLOR_TO_STRING(m_pConfig->color), m_pConfig->name);

	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_ForceJoin", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetDataPtr((void*)m_pConfig);
}

void CDlgForceNPC::OnCommand_Quit(const char * szCommand)
{
	if(!m_pConfig || GetHostPlayer()->GetForce() != m_pConfig->id)
		return;

	ACString strMsg;
	strMsg.Format(GetStringFromTable(9403), A3DCOLOR_TO_STRING(m_pConfig->color), m_pConfig->name);

	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_ForceQuit", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetDataPtr((void*)m_pConfig);
}

void CDlgForceNPC::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(iRetVal == IDYES)
	{
		const FORCE_CONFIG *pConfig = pDlg ? (const FORCE_CONFIG *)pDlg->GetDataPtr() : NULL;
		if(pConfig)
		{
			enum REQ_TYPE
			{
				RT_JOIN = 0,
				RT_LEAVE,
				RT_MAX,
			};

			if (!stricmp(pDlg->GetName(), "Game_ForceJoin"))
			{
				GetGameSession()->c2s_CmdNPCSevForce(RT_JOIN, pConfig->id);
			}
			else if (!stricmp(pDlg->GetName(), "Game_ForceQuit"))
			{
				GetGameSession()->c2s_CmdNPCSevForce(RT_LEAVE, pConfig->id);
			}

			OnCommand("IDCANCEL");
		}
		else
		{
			// no valid data binded
			ASSERT(false);
		}
	}
}

void CDlgForceNPC::OnTick()
{
	CDlgBase::OnTick();

	// update the cool time for join/quit button
	PAUIOBJECT pBtn = NULL;
	int ct = 0;
	if(pBtn = GetDlgItem("Btn_Join"))
	{
		// for join
		ct = GetHostPlayer()->GetCoolTime(GP_CT_JOIN_PLAYER_FORCE);
	}
	else if(pBtn = GetDlgItem("Btn_Quit"))
	{
		// for quit
		ct = GetHostPlayer()->GetCoolTime(GP_CT_LEAVE_PLAYER_FORCE);
	}
	ACString strHint;
	if(ct > 0)
	{
		strHint.Format(GetStringFromTable(9404), GetGameUIMan()->GetFormatTime(max(1, ct/1000)));
	}
	pBtn->Enable(ct <= 0);
	pBtn->SetHint(strHint);
}

void CDlgForceNPC::OnShowDialog()
{
	int forceID = GetData();
	if(!UpdateAll(forceID))
	{
		// invalid force data
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9420), GP_CHAT_MISC);
		OnCommand("IDCANCEL");
		return;
	}
}

bool CDlgForceNPC::OnInitDialog()
{
	if( !CDlgBase::OnInitDialog() )
		return false;

	return true;
}

bool CDlgForceNPC::UpdateAll(int forceID)
{
	const FORCE_CONFIG *pConfig = CECForceMgr::GetForceData(forceID);
	if(!pConfig)
	{
		return false;
	}
	m_pConfig = pConfig;

	// init force flag
	PAUIIMAGEPICTURE pImgForce = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Force"));
	if(pImgForce)
	{
		A2DSprite* pSprite = NULL;
		CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
		if(pMgr) pSprite = pMgr->GetForceIcon(m_pA3DDevice, pConfig, CECForceMgr::FORCE_ICON_FLAG);
		pImgForce->SetCover(pSprite, 0);
	}
	
	PAUILABEL pTxtTitle = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Title"));
	if(pTxtTitle)
	{
		pTxtTitle->SetText(pConfig->name);
	}

	PAUITEXTAREA pTxtIntro = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Intro"));
	if(pTxtIntro)
	{
		AUICTranslate trans;
		pTxtIntro->SetText(trans.Translate(pConfig->desc));
	}

	ACString strTxt;

	// for join
	PAUIOBJECT pTxtGold = dynamic_cast<PAUIOBJECT>(GetDlgItem("Txt_Gold"));
	if(pTxtGold)
	{
		strTxt = GetGameUIMan()->GetFormatNumber(pConfig->join_money_cost);
		pTxtGold->SetText(strTxt);
	}

	// update cost item
	PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Item_Force"));
	if(pIcon)
	{
		CECIvtrItem* pItem = pConfig->join_item_cost ? CECIvtrItem::CreateItem(pConfig->join_item_cost, 0, 1) : NULL;
		if(pItem)
		{
			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pIcon->SetCover(GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));

			AUICTranslate trans;
			const wchar_t* szDesc = pItem->GetDesc();
			if( szDesc )
				pIcon->SetHint(trans.Translate(szDesc));
			else
				pIcon->SetHint(pItem->GetName());
			delete pItem;
		}
		else
		{
			pIcon->ClearCover();
			pIcon->SetHint(_AL(""));
		}
	}

	// for quit
	PAUIOBJECT pTxtRepution = dynamic_cast<PAUIOBJECT>(GetDlgItem("Txt_Repution"));
	if(pTxtRepution)
	{
		strTxt.Format(_AL("%d%%"), min(100, max(pConfig->quit_repution_cost, 0)));
		pTxtRepution->SetText(strTxt);
	}
	PAUIOBJECT pTxtContribution = dynamic_cast<PAUIOBJECT>(GetDlgItem("Txt_Contribution"));
	if(pTxtContribution)
	{
		strTxt.Format(_AL("%d%%"), min(100, max(pConfig->quit_contribution_cost, 0)));
		pTxtContribution->SetText(strTxt);
	}

	return true;
}