#include "DlgFortressInfo.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "Network\\ids.hxx"
#include "ExpTypes.h"
#include <AUICTranslate.h>
#include <AFI.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressInfo, CDlgBase)
AUI_ON_COMMAND("Btn_Levelup", OnCommand_Levelup)
AUI_ON_COMMAND("Btn_0*",	OnCommand_Upgrade)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressInfo, CDlgBase)
AUI_ON_EVENT("Btn_0*", WM_RBUTTONDOWN, OnEvent_RButtonDown)
AUI_ON_EVENT("*", WM_RBUTTONUP, OnEvent_RButtonUp)
AUI_END_EVENT_MAP()

CDlgFortressInfo::CDlgFortressInfo()
{
	m_pImg_Reset = NULL;
}

bool CDlgFortressInfo::OnInitDialog()
{
	m_pBtn_Levelup = GetDlgItem("Btn_Levelup");
	m_pLab_GuildName = GetDlgItem("Lab_GuildName");
	m_pTxt_Level= GetDlgItem("Txt_Level");
	m_pLab_Exp	= GetDlgItem("Lab_Exp");
	m_pPro_Exp	= (PAUIPROGRESS)GetDlgItem("Pro_Exp");
	m_Lab_Technology= GetDlgItem("Lab_Technology");

	int i(0);
	AString strName;
	for (i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
	{
		strName.Format("Lab_M%02d", i+1);
		m_pLab_M[i] = GetDlgItem(strName);
	}
	
	for (i = 0; i < TECHNOLOGY_COUNT; ++ i)
	{
		strName.Format("Lab_%02d", i+1);
		m_pLab_Tech[i] = GetDlgItem(strName);
		
		strName.Format("Img_%02d", i+1);
		m_pImg_Tech[i] = (PAUIIMAGEPICTURE)GetDlgItem(strName);
	}

	DDX_Control("Img_Reset", m_pImg_Reset);
	return true;
}

void CDlgFortressInfo::OnShowDialog()
{
	//	显示最新结果
	UpdateInfo();

	//	发送协议更新帮派基地信息
	GetGame()->GetGameSession()->c2s_CmdGetFactionFortressInfo();

	bool bShowLevelup = false;
	if (GetGameUIMan()->m_pCurNPCEssence
		&& (GetGameUIMan()->m_pCurNPCEssence->combined_services & 0x10000000))
	{
		//	基地的第2个NPC服务处可升级、销毁设施
		
		if (GetHostPlayer()->GetFactionID() > 0
			&& GetHostPlayer()->GetFRoleID() == GNET::_R_MASTER)
		{
			//	只有帮主可升级、销毁设施
			bShowLevelup = true;
		}
	}
	m_pBtn_Levelup->Show(bShowLevelup);
}

void CDlgFortressInfo::UpdateInfo()
{
	m_pBtn_Levelup->Enable(false);
	m_pLab_GuildName->SetText(_AL(""));
	m_pTxt_Level->SetText(_AL(""));
	m_pLab_Exp->SetText(_AL(""));
	m_pPro_Exp->SetProgress(0);
	m_Lab_Technology->SetText(_AL(""));

	int i(0);
	for (i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		m_pLab_M[i]->SetText(_AL(""));

	for (i = 0; i < TECHNOLOGY_COUNT; ++ i)
	{
		m_pLab_Tech[i]->SetText(_AL(""));
		m_pImg_Tech[i]->FixFrame(0);
	}
		
	CECHostPlayer *pHost = GetHostPlayer();
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	int idFaction = pHost->GetFactionID();
	const Faction_Info *pFInfo = pFMan->GetFaction(idFaction);
	if (idFaction)
		m_pLab_GuildName->SetText(pFInfo->GetName());

	const CECHostPlayer::FACTION_FORTRESS_INFO * pInfo = pHost->GetFactionFortressInfo();
	if (pInfo)
	{
		ACString strText;

		//	基地等级
		strText.Format(GetStringFromTable(9120), pInfo->level);
		m_pTxt_Level->SetText(strText);

		const FACTION_FORTRESS_CONFIG *pConfig = pHost->GetFactionFortressConfig();
		if (pConfig)
		{
			//	经验值
			int expLevelup = pConfig->level[pInfo->level-1].exp;
			strText.Format(_AL("%d/%d"), pInfo->exp, expLevelup);
			m_pLab_Exp->SetText(strText);

			m_pBtn_Levelup->Enable(expLevelup > 0 && pInfo->exp >= expLevelup);

			int nProgress(0);
			if (expLevelup > 0)
				nProgress = (int)(pInfo->exp/(float)expLevelup*100);
			else
				nProgress = 100;
			m_pPro_Exp->SetProgress(nProgress);
		}

		//	剩余科技点数
		strText.Format(_AL("%d"), pInfo->tech_point);
		m_Lab_Technology->SetText(strText);

		//	各科技天赋
		for (i = 0; i < TECHNOLOGY_COUNT; ++ i)
		{
			strText.Format(_AL("%d"), pInfo->technology[i]);
			m_pLab_Tech[i]->SetText(strText);
			m_pImg_Tech[i]->FixFrame(pInfo->technology[i]);
		}

		//	剩余材料数
		for (int j = 0; j < BUILDING_MATERIAL_COUNT; ++ j)
		{
			strText.Format(_AL("%d"), pInfo->material[j]);
			m_pLab_M[j]->SetText(strText);
		}
	}

	UpdateResetItem();
}

void CDlgFortressInfo::UpdateResetItem()
{
	AString strFile;
	CECInventory* pPack = GetHostPlayer()->GetPack();
	int iCount = pPack->GetItemTotalNum(RESETTECH_ITEM);

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(RESETTECH_ITEM, 0, 1);
	if( !pItem ) return;

	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Reset->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	m_pImg_Reset->SetHint(trans.Translate(pItem->GetDesc()));

	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), iCount);
	m_pImg_Reset->SetText(szText);
	m_pImg_Reset->SetColor(iCount > 0 ? A3DCOLORRGB(255, 255, 255) : A3DCOLORRGB(128, 128, 128));
}

void CDlgFortressInfo::OnCommand_CANCEL(const char *szCommand)
{
	//	为避免下面关闭相关对话框时发生循环，先隐藏当前对话框
	Show(false);

	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
	{
		//	关闭 NPC 服务
		GetGameUIMan()->EndNPCService();

		//	关闭相关对话框
		AString strDlgNames[] = {
			"Win_FortressBuild", "Win_FortressContrib", "Win_FortressExchange",
			"Win_FortressMaterial", "Win_FortressWar",
			"Win_FortressWarList"
		};
		for (int i = 0; i < sizeof(strDlgNames)/sizeof(strDlgNames[0]); ++ i)
		{
			PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(strDlgNames[i]);
			if (pDlg && pDlg->IsShow())
				pDlg->OnCommand("IDCANCEL");
		}
	}
}

void CDlgFortressInfo::OnCommand_Levelup(const char *szCommand)
{
	GetGameSession()->c2s_CmdNPCSevFactionFortressLevelup();
}

void CDlgFortressInfo::OnCommand_Upgrade(const char *szCommand)
{
	if (!GetGameUIMan()->m_pCurNPCEssence ||
		!(GetGameUIMan()->m_pCurNPCEssence->combined_services & 0x10000000) ||
		!GetHostPlayer()->GetFactionID() ||
		GetHostPlayer()->GetFRoleID() != GNET::_R_MASTER)
	{
		//	只有帮主在基地的第2个NPC基地服务处可加科技点，
		return;
	}

	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_FORTRESS_CONFIG * pConfig = GetHostPlayer()->GetFactionFortressConfig();
	if (!pInfo || !pConfig)
		return;

	int iTech = atoi(szCommand + strlen("Btn_")) -1;
	int iTechLevel = pInfo->technology[iTech];
	const int MAX_TECH_LEVEL = sizeof(pConfig->tech_point_cost[0])/sizeof(pConfig->tech_point_cost[0][0]);
	if (iTechLevel >= MAX_TECH_LEVEL)
	{
		//	无法继续升级
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9128));
		return;
	}
	int nTechPointNeeded = pConfig->tech_point_cost[iTech][iTechLevel];
	if (pInfo->tech_point < nTechPointNeeded)
	{
		//	点数不够
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9121));
		return;
	}

	//	发送协议
	ACString strMsg;
	strMsg.Format(GetStringFromTable(9125), nTechPointNeeded);
	PAUIDIALOG pMsgBox = NULL;
	GetGameUIMan()->MessageBox("Fortress_TechLevelup", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(iTech);
}

void CDlgFortressInfo::OnEvent_RButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	if( !pObj || !strstr(pObj->GetName(), "Btn_0") )
		return;

	AUIStillImageButton* pBtn = dynamic_cast<AUIStillImageButton*>(pObj);
	if( pBtn ) pBtn->SetState(AUISTILLIMAGEBUTTON_STATE_CLICK);
}

void CDlgFortressInfo::OnEvent_RButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	for( int i=0;i<TECHNOLOGY_COUNT;i++ )
	{
		AString strName;
		strName.Format("Btn_0%d", i+1);
		AUIStillImageButton* pBtn = dynamic_cast<AUIStillImageButton*>(GetDlgItem(strName));
		if( pBtn ) pBtn->SetState(AUISTILLIMAGEBUTTON_STATE_NORMAL);
	}

	if( !pObj || !strstr(pObj->GetName(), "Btn_0") )
		return;

	if (!GetGameUIMan()->m_pCurNPCEssence ||
		!(GetGameUIMan()->m_pCurNPCEssence->combined_services & 0x10000000) ||
		!GetHostPlayer()->GetFactionID() ||
		GetHostPlayer()->GetFRoleID() != GNET::_R_MASTER)
	{
		//	只有帮主在基地的第2个NPC基地服务处可加科技点，
		return;
	}

	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
	const FACTION_FORTRESS_CONFIG * pConfig = GetHostPlayer()->GetFactionFortressConfig();
	if (!pInfo || !pConfig)
		return;

	int iTech = atoi(pObj->GetName() + strlen("Btn_0")) - 1;
	ASSERT(iTech >= 0 && iTech < TECHNOLOGY_COUNT);

	CECInventory* pPack = GetHostPlayer()->GetPack();
	int iSlot = pPack->FindItem(RESETTECH_ITEM);
	if( iSlot < 0 )
	{
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(10560));
		return;
	}

	int iTechPoint = pInfo->technology[iTech];
	if( iTechPoint )
	{
		// 发送协议
		ACString strMsg;
		strMsg.Format(GetStringFromTable(10561), GetStringFromTable(9260+iTech), GetStringFromTable(9260+iTech), GetStringFromTable(10550+iTech));
		PAUIDIALOG pMsgBox = NULL;
		GetGameUIMan()->MessageBox("Fortress_TechReset", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData((iTech << 16) | iSlot);
	}
	else
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10562), MB_OK, A3DCOLORRGB(255, 255, 255));
	}
}
