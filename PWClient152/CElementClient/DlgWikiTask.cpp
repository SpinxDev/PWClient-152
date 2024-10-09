// Filename	: DlgWikiTask.cpp
// Creator	: Feng Ning
// Date		: 2010/05/11

#include "AFI.h"
#include "DlgWikiTask.h"
#include "DlgWikiTaskDetail.h"
#include "DlgWikiItem.h"
#include "DlgWikiEquipment.h"
#include "WikiItemProp.h"
#include "WikiEquipmentProp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrEquip.h"
#include "WikiSearchCommand.h"
#include "WikiSearchTask.h"
#include "elementdataman.h"
#include "EC_TaskInterface.h"
#include "EC_HostPlayer.h"
#include "AUICTranslate.h"
#include "EC_Utility.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiTask, CDlgWikiByNameBase)
AUI_ON_COMMAND("detail", OnCommand_Detail)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiTask, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Task",	WM_LBUTTONDOWN,	OnEventLButtonDown_List)
AUI_ON_EVENT("List_Task",	WM_KEYDOWN,		OnEventKeyDown_List)
AUI_ON_EVENT("Img_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Icon)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiTask::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int task_id, const ACString& name)
:m_TaskID(task_id)
{
	// try to get the color info in name
	ACString szColor = _AL("^FFFFFF");
	ACString szName = name;
	A3DCOLOR color = A3DCOLORRGB(255, 255, 255);
	const int COLOR_STR_SIZE = 7;
	while(szName[0] == _AL('^') && szName.GetLength() > COLOR_STR_SIZE)
	{
		szColor = szName.Mid(0, COLOR_STR_SIZE);
		if(!STRING_TO_A3DCOLOR(szColor, color))
		{
			break;
		}
		// skip the color
		szName = szName.Mid(COLOR_STR_SIZE);
		m_Color = szColor;
	}

	m_Name = szName;
}

CDlgNameLink::LinkCommand* CDlgWikiTask::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_TaskID, m_Color + m_Name);
}

bool CDlgWikiTask::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	PAUITEXTAREA pObj = GetTxtArea();
	
	// goto search item
	CDlgWikiTask* pDlg = dynamic_cast<CDlgWikiTask*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiTaskSearch"));
	ASSERT(pDlg); // should always get this dialog

	if(pDlg)
	{
		// ensure the provider is correct
		pDlg->SetContentProvider(&WikiTaskDataProvider());
		pDlg->RefreshByNewCommand(&WikiSearchTaskBySpecificID(m_TaskID), true);
		return true;
	}

	return false;
}

ACString CDlgWikiTask::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	strLinkedName.Format(_AL("%s&%s&^FFFFFF"), m_Color, m_Name);
	return strLinkedName;
}

// =======================================================================
CDlgWikiTask::CDlgWikiTask()
:m_pList(NULL)

,m_pBtn_Detail(NULL)
{
}
bool CDlgWikiTask::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	DDX_Control("List_Task", m_pList);
	
	DDX_Control("Btn_Detail", m_pBtn_Detail);
	
	return true;
}

void CDlgWikiTask::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();
	SetContentProvider(&WikiTaskDataProvider());
	SetSearchCommand(&WikiSearchTaskByName());
}

void CDlgWikiTask::OnBeginSearch()
{
 	m_pList->ResetContent();
 	OnEventLButtonDown_List(0, 0, m_pList);
}

bool CDlgWikiTask::OnAddSearch(WikiEntityPtr p)
{
	WikiTaskDataProvider::Entity* pEE = dynamic_cast<WikiTaskDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	ATaskTempl *pTempl = pEE->GetIterator()->second;

	ASSERT(m_pList->GetCount() < GetPageSize());

	// try to get the color info in name
//	ACString szName = pTempl->GetName();
	ACString szName = CDlgTask::GetTaskNameWithColor(pTempl);
	A3DCOLOR color = CDlgTask::GetTaskColor(pTempl);
	const int COLOR_STR_SIZE = 7;
	while(szName[0] == _AL('^') && szName.GetLength() > COLOR_STR_SIZE)
	{
		ACString szColor = szName.Mid(0, COLOR_STR_SIZE);
		if(!STRING_TO_A3DCOLOR(szColor, color))
		{
			break;
		}
		// skip the color
		szName = szName.Mid(COLOR_STR_SIZE);
	}

	ACString strItem;
	strItem.Format(_AL("%s"), szName);
	m_pList->AddString(strItem);
	m_pList->SetItemDataPtr(m_pList->GetCount()-1, pTempl);
	m_pList->SetItemTextColor(m_pList->GetCount()-1, color);
	
	// set hint info
	
	return true;
}

void CDlgWikiTask::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);

	OnEventLButtonDown_List(0, 0, m_pList);
}

ACString& CDlgWikiTask::GetClassName(unsigned long ulOccupation, unsigned long* pOccupations, ACString& szTxt)
{
	unsigned int class_masks = 0;
	if(pOccupations && ulOccupation <= MAX_OCCUPATIONS)
	{
		for (size_t i=0; i < ulOccupation; i++)
		{
			int limit = pOccupations[i];
			if (limit >= 0 && limit <= MAX_OCCUPATIONS)
			{
				class_masks |= (1 << limit);
			}
		}
	}

	szTxt = GetGameUIMan()->GetFormatClass(class_masks, true);
	
	return szTxt;
}

ACString& CDlgWikiTask::GetNPCName(unsigned long id, ACString& szTxt)
{
	szTxt = GetStringFromTable(8720);

	DATA_TYPE dt;
	NPC_ESSENCE *pNPC = (NPC_ESSENCE *)GetGame()->GetElementDataMan()->get_data_ptr(
		id, ID_SPACE_ESSENCE, dt);

	if( dt == DT_NPC_ESSENCE )
	{
		szTxt = pNPC->name;
	}

	return szTxt;
}

ACString& CDlgWikiTask::GetLevelRequire(unsigned long minLevel, unsigned long maxLevel, ACString& szTxt)
{
	szTxt.Empty();

	if(minLevel == 0 && maxLevel == 0)
	{
		szTxt = GetStringFromTable(8720);
	}
	else if(maxLevel > 0 && minLevel > 0)
	{
		szTxt.Format(GetStringFromTable(8659), minLevel, maxLevel);
	}
	else if(minLevel > 0)
	{
		szTxt.Format(GetStringFromTable(8658), minLevel);
	}
	else
	{
		szTxt.Format(GetStringFromTable(8657), maxLevel);
	}

	return szTxt;
}

bool CDlgWikiTask::SetItemIcon(PAUIIMAGEPICTURE pIcon, unsigned int item_id, unsigned int count)
{
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(item_id, 0, count);
	if( !pItem )
	{
		return false;
	}

	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	pIcon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	
	AUICTranslate trans;
	pItem->GetDetailDataFromLocal();
	const ACHAR *pszDesc = pItem->GetDesc(CECIvtrItem::DESC_REWARD);
	if( pszDesc )
		pIcon->SetHint(trans.Translate(pszDesc));
	else
		pIcon->SetHint(pItem->GetName());
	
	delete pItem;
	
	ACString szTxt;
	szTxt.Format(_AL("%d"), count);
	pIcon->SetText(szTxt);
	pIcon->Show(true);
	pIcon->SetData(item_id);

	return true;
}

void CDlgWikiTask::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// mapping to the properties, put4f them here to make the header clean.
	PAUILABEL pTxt_TaskName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_TaskName"));
	PAUILABEL pTxt_Type = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Type"));
	PAUILABEL pTxt_Gender = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Gender"));
	PAUILABEL pTxt_Class = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Class"));
	PAUILABEL pTxt_LevelRequire = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_LevelRequire"));
	PAUILABEL pTxt_Repeat = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Repeat"));
	PAUITEXTAREA pTxt_BeginNpc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_BeginNpc"));
	PAUITEXTAREA pTxt_EndNpc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_EndNpc"));
	PAUILABEL pTxt_Gold = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Gold"));
	PAUILABEL pTxt_Exp = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Exp"));
	PAUILABEL pTxt_Sp = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Sp"));

	// calc the size of icon set
	size_t iObj = 0;
	AString szName;
	while(true)
	{
		szName.Format("Img_Item%d", iObj++);
		PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szName));
		if(!pIcon) break;
		pIcon->SetCover(NULL, -1);
		pIcon->Show(false);
		pIcon->SetData(0);
	}

	// refresh the information
	GetGameUIMan()->m_pDlgWikiTaskDetail->Show(false);
	ATaskTempl* pTempl = GetSeletedTask();

	if (pTempl)
	{
		// refresh the detail window
		GetGameUIMan()->m_pDlgWikiTaskDetail->SetDataPtr((void *)pTempl);
		m_pBtn_Detail->Enable(true);

//		pTxt_TaskName->SetText(GetColorName(pTempl->GetType()) + pTempl->GetName());
		pTxt_TaskName->SetText(CDlgTask::GetTaskNameWithColor(pTempl));
		pTxt_Type->SetText(GetStringFromTable(3101 + pTempl->GetType() - 100));

		ACString szTxt;

		pTxt_Gender->SetText(pTempl->m_ulGender == TASK_GENDER_NONE  ? 
			GetStringFromTable(8720) : GetStringFromTable(8650 + pTempl->m_ulGender - 1));

 		pTxt_Class->SetText(
 			GetClassName(pTempl->m_ulOccupations, pTempl->m_Occupations, szTxt));

		pTxt_LevelRequire->SetText(
			GetLevelRequire(pTempl->m_ulPremise_Lev_Min, pTempl->m_ulPremise_Lev_Max, szTxt));

		pTxt_Repeat->SetText(pTempl->m_bCanRedo ? 
			GetStringFromTable(7502) : GetStringFromTable(7501));

		BindLinkCommand(pTxt_BeginNpc, NULL,
									  &CDlgNameLink::MoveToLinkCommand(pTempl->GetDeliverNPC(), GetNPCName(pTempl->GetDeliverNPC(), szTxt)));

		BindLinkCommand(pTxt_EndNpc, NULL,
									  &CDlgNameLink::MoveToLinkCommand(pTempl->GetAwardNPC(), GetNPCName(pTempl->GetAwardNPC(), szTxt)));

		CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
		Task_Award_Preview award;
		pTask->GetTaskAwardPreview(pTempl->GetID(), &award, false);

		pTxt_Gold->SetText(szTxt.Format((_AL("%d")), award.m_ulGold));

		pTxt_Exp->SetText(szTxt.Format((_AL("%d")), award.m_ulExp));

		pTxt_Sp->SetText(szTxt.Format((_AL("%d")), award.m_ulSP));

		// set the award icon
		if(award.m_bHasItem && award.m_bItemKnown)
		{
			iObj = 0;
			PAUIIMAGEPICTURE pIcon = NULL;
			for(DWORD i = 0 ; i < award.m_ulItemTypes ; i++ )
			{
				if(pIcon == NULL)
				{
					// show the available icon
					szName.Format("Img_Item%d", iObj++);
					pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szName));
					if(!pIcon) break;
				}

				if(SetItemIcon(pIcon, award.m_ItemsId[i], award.m_ItemsNum[i]))
				{
					// clear pointer to get next icon
					pIcon = NULL;
				}
			}
		}
	}
	else
	{
		m_pBtn_Detail->Enable(false);

		pTxt_TaskName->SetText(_AL(""));
		pTxt_Type->SetText(_AL(""));
		pTxt_Gender->SetText(_AL(""));
		pTxt_Class->SetText(_AL(""));
		pTxt_LevelRequire->SetText(_AL(""));
		pTxt_Repeat->SetText(_AL(""));
		
		BindLinkCommand(pTxt_BeginNpc, NULL, NULL);
		pTxt_BeginNpc->SetText(_AL(""));
		BindLinkCommand(pTxt_EndNpc, NULL, NULL);
		pTxt_EndNpc->SetText(_AL(""));

		pTxt_Gold->SetText(_AL(""));
		pTxt_Exp->SetText(_AL(""));
		pTxt_Sp->SetText(_AL(""));
	}
}

void CDlgWikiTask::OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		OnEventLButtonDown_List(0, 0, NULL);
	}
}

ATaskTempl* CDlgWikiTask::GetSeletedTask()
{
	ATaskTempl* pTempl = NULL;

	if (m_pList->GetCount() > 0)
	{		
		int nCurSel = m_pList->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList->GetCount())
		{
			pTempl = (ATaskTempl*)m_pList->GetItemDataPtr(nCurSel);
		}
	}

	return pTempl;
}

void CDlgWikiTask::OnCommand_Detail(const char *szCommand)
{
	CDlgWikiTaskDetail* pDlg = GetGameUIMan()->m_pDlgWikiTaskDetail;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgWikiTask::OnCommandCancel()
{
	CDlgWikiByNameBase::OnCommandCancel();
	GetGameUIMan()->m_pDlgWikiTaskDetail->Show(false);
}

bool CDlgWikiTask::Release(void)
{
	// reset content
	m_pList->SetCurSel(-1);
	OnEventLButtonDown_List(0, 0, NULL);
	return CDlgWikiByNameBase::Release();
}

void CDlgWikiTask::OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(!pObj) return;
	
	DWORD id = pObj->GetData();
	if(id == 0) return;
	
	WikiEquipmentProp* pEqu = WikiEquipmentProp::CreateProperty(id);
	if(pEqu)
	{
		CDlgWikiEquipment::ShowSpecficLinkCommand(id, pEqu->GetName())(NULL);
		delete pEqu;
	}
	else
	{
		WikiItemProp* pItem = WikiItemProp::CreateProperty(id);
		if(pItem)
		{
			CDlgWikiItem::ShowSpecficLinkCommand(id, pItem->GetName())(NULL);
			delete pItem;
		}
	}
}

ACString CDlgWikiTask::GetColorName(int idType)
{
	switch(idType)
	{
	case enumTTLevel2:
		return _AL("^FFCB4A");

	case enumTTQiShaList:
		return _AL("^FF0000");

	default:
		return _AL("^FFFFFF");
	}
}