// File		: DlgWarTower.cpp
// Creator	: Xiao Zhou
// Date		: 2006/1/1

#include "DlgWarTower.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Inventory.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWarTower, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgWarTower::CDlgWarTower()
{
}

CDlgWarTower::~CDlgWarTower()
{
}


void CDlgWarTower::OnCommandConfirm(const char *szCommand)
{
	Show(false);
	int i;
	for(i = 0; i < WAR_TOWER_MAX; i++)
	{
		if( m_pRdo_Tower[i]->IsChecked() && 
			GetHostPlayer()->GetMoneyAmount() >= (int)m_pTxt_Money[i]->GetData() &&
			(m_pTxt_Item[i]->GetData() == 0 ||
			GetHostPlayer()->GetPack()->GetItemTotalNum(m_pTxt_Item[i]->GetData()) > 0) )
		{
			GetGameSession()->c2s_CmdNPCSevBuildTower(i, m_pImg_Tower[i]->GetData());
			break;
		}
	}
	GetGameUIMan()->EndNPCService();
}

void CDlgWarTower::OnCommandCancel(const char *szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

bool CDlgWarTower::OnInitDialog()
{
	int i;
	for(i = 0; i < WAR_TOWER_MAX; i++)
	{
		char szName[20];
		sprintf(szName, "Txt_Money%d", i + 1);
		m_pTxt_Money[i] = (PAUILABEL)GetDlgItem(szName);
		sprintf(szName, "Txt_Item%d", i + 1);
		m_pTxt_Item[i] = (PAUILABEL)GetDlgItem(szName);
		sprintf(szName, "Img_Tower%d", i + 1);
		m_pImg_Tower[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		sprintf(szName, "Rdo_Tower%d", i + 1);
		m_pRdo_Tower[i] = (PAUIRADIOBUTTON)GetDlgItem(szName);
	}
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");

	return true;
}

void CDlgWarTower::OnShowDialog()
{
	DATA_TYPE DataType;
	NPC_WAR_TOWERBUILD_SERVICE *pData = (NPC_WAR_TOWERBUILD_SERVICE *)GetGame()->GetElementDataMan()->
		get_data_ptr(GetGameUIMan()->m_pCurNPCEssence->id_war_towerbuild_service, ID_SPACE_ESSENCE, DataType);
	if( DataType != DT_NPC_WAR_TOWERBUILD_SERVICE ) return;
	int i;
	for(i = 0; i < WAR_TOWER_MAX; i++)
		if( pData->build_info[i].id_buildup )
		{
			m_pRdo_Tower[i]->Check(false);
			ACString strText;
			strText.Format(GetStringFromTable(720), pData->build_info[i].fee);
			m_pTxt_Money[i]->SetText(strText);
			m_pTxt_Money[i]->SetData(pData->build_info[i].fee);
			m_pTxt_Item[i]->SetData(pData->build_info[i].id_object_need);
			ACString szName = _AL("");
			ACString szItemName = _AL("");
			MONSTER_ESSENCE *pMonsterData = (MONSTER_ESSENCE *)GetGame()->GetElementDataMan()->
				get_data_ptr(pData->build_info[i].id_buildup, ID_SPACE_ESSENCE, DataType);
			if( pMonsterData && DataType == DT_MONSTER_ESSENCE )
				szName = pMonsterData->name;
			if( pData->build_info[i].id_object_need )
			{
				CECIvtrItem *pItem = CECIvtrItem::CreateItem(pData->build_info[i].id_object_need, 0, 1);
				szItemName = pItem->GetName();
				delete pItem;
			}
			m_pTxt_Item[i]->SetText(szItemName);
			if( szItemName == _AL("") )
				szItemName = GetStringFromTable(786);
			strText.Format(GetStringFromTable(721), szName, pData->build_info[i].time_use,
				pData->build_info[i].fee, szItemName);
			m_pImg_Tower[i]->SetHint(strText);
		}
		else
		{
			m_pImg_Tower[i]->Show(false);
			m_pTxt_Item[i]->Show(false);
			m_pTxt_Money[i]->Show(false);
			m_pRdo_Tower[i]->Show(false);
		}
}

void CDlgWarTower::OnTick()
{
	int i;
	for(i = 0; i < WAR_TOWER_MAX; i++)
	{
		if( GetHostPlayer()->GetMoneyAmount() < (int)m_pTxt_Money[i]->GetData() )
			m_pTxt_Money[i]->SetColor(A3DCOLORRGB(198, 0, 0));
		else
			m_pTxt_Money[i]->SetColor(A3DCOLORRGB(255, 255, 255));
		if( m_pTxt_Item[i]->GetData() &&
			GetHostPlayer()->GetPack()->GetItemTotalNum(m_pTxt_Item[i]->GetData()) == 0 )
			m_pTxt_Item[i]->SetColor(A3DCOLORRGB(198, 0, 0));
		else
			m_pTxt_Item[i]->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	m_pBtn_Confirm->Enable(false);
	for(i = 0; i < WAR_TOWER_MAX; i++)
	{
		if( m_pRdo_Tower[i]->IsChecked() && 
			GetHostPlayer()->GetMoneyAmount() >= (int)m_pTxt_Money[i]->GetData() &&
			(m_pTxt_Item[i]->GetData() == 0 ||
			GetHostPlayer()->GetPack()->GetItemTotalNum(m_pTxt_Item[i]->GetData()) > 0) )
		{
			m_pBtn_Confirm->Enable(true);
			break;
		}
	}
}
