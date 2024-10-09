// Filename	: DlgGMQueryItem.cpp
// Creator	: Feng Ning
// Date		: 2011/03/25

#include "DlgGMQueryItem.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_IvtrItem.h"
#include "elementdataman.h"
#include "EC_GameUIMan.h"
#include "DlgInputNO.h"

#include "AFI.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGMQueryItem, CDlgBase)

AUI_ON_COMMAND("Btn_Delete",	OnCommand_DeleteItems)
AUI_ON_COMMAND("Btn_Type*",		OnCommand_ShowType)
AUI_ON_COMMAND("Btn_All",		OnCommand_ShowType)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGMQueryItem, CDlgBase)

AUI_ON_EVENT("Lst_Result",	WM_LBUTTONDBLCLK,	OnEventLButtonDBCLK_Result)
AUI_ON_EVENT("Item_*",	WM_LBUTTONDBLCLK,		OnEventLButtonDBCLK_Items)

AUI_END_EVENT_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGMQueryItemSub, CDlgBase)

AUI_ON_EVENT("Item_*",	WM_LBUTTONDBLCLK,		OnEventLButtonDBCLK_Items)
AUI_ON_EVENT("Item_*",	WM_LBUTTONDOWN,			OnEventLButtonDown_Items)

AUI_END_EVENT_MAP()

#define INVALID_TAG _AL("--")
#define COLOR_SELECTED A3DCOLORRGB(192, 0, 0)
#define COLOR_NORMAL A3DCOLORRGB(255, 255, 255)

CDlgGMQueryItem::CDlgGMQueryItem()
:m_ItemID(0)
,m_PlayerID(0)
,m_FilterType(FILTER_MAX)
,m_State(STATE_WAIT)

,m_pLstResult(NULL)
,m_pQuerying(NULL)
,m_pBtnDelete(NULL)
,m_pSubItems(NULL)
{
	memset(m_pFilters, 0, sizeof(m_pFilters));
}

CDlgGMQueryItem::~CDlgGMQueryItem()
{
}

bool CDlgGMQueryItem::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
	{
		return false;
	}

	DDX_Control("Btn_Delete", m_pBtnDelete);

	DDX_Control("Lab_Query", m_pQuerying);
	m_pQuerying->Show(false);

	// initialize listbox
	DDX_Control("Lst_Result", m_pLstResult);

	AString strName;

	// initialize filter buttons
	DDX_Control("Btn_All", m_pFilters[FILTER_MAX]);
	m_pFilters[FILTER_MAX]->SetPushed(true);
	for(int i=0;i<FILTER_MAX;i++)
	{
		strName.Format("Btn_Type%d", i);
		PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(strName));
		if(!pBtn) break;

		m_pFilters[i] = pBtn;
		pBtn->SetPushed(false);
	}

	// initialize icon set
	DDX_Control("Sub_Items", m_pSubItems);

	// init data
	m_State = STATE_WAIT;
	m_Infos.clear();
	m_FilterType = FILTER_MAX;

	UpdateTarget(0, 0);

	OnChangeLayoutEnd(true);

	return true;
}

void CDlgGMQueryItem::OnShowDialog()
{
	CDlgBase::OnShowDialog();
}

void CDlgGMQueryItem::OnTick()
{
	bool bEnable = (m_PlayerID != 0 && m_State == STATE_WAIT);
	m_pLstResult->Enable(bEnable);
	m_pSubItems->Enable(bEnable);
	m_pBtnDelete->Enable(bEnable);

	m_pQuerying->Show(m_State == STATE_DELETING);
}

void CDlgGMQueryItem::UpdateTarget(int idPlayer, int idItem)
{
	ACString strName;

	m_PlayerID = idPlayer;
	strName.Format(GetStringFromTable(6004), GetGameRun()->GetPlayerName(idPlayer, true), m_PlayerID);
	GetDlgItem("Lab_PlayerID")->SetText(strName);
	
	m_ItemID = max(0, idItem);
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(m_ItemID, 0, 1, ID_SPACE_ESSENCE);
	strName.Format(GetStringFromTable(6353), pItem->GetName(), m_ItemID);

	af_GetFileTitle(pItem->GetIconFile(), m_IconFile);
	m_IconFile.MakeLower();
	
	delete pItem;
	GetDlgItem("Lab_ItemID")->SetText(strName);
}

void CDlgGMQueryItem::RefreshResult(int type)
{
	ASSERT(type >= 0 && type <= FILTER_MAX);
	m_FilterType = type;

	for(int i=0;i<=FILTER_MAX;i++)
	{
		m_pFilters[i]->SetPushed(type == i);
	}

	// clear the list box
	m_pLstResult->ResetContent();
	m_pLstResult->Show(type == FILTER_MAX);
	GetDlgItem("Lab_Title1")->Show(m_pLstResult->IsShow());
	GetDlgItem("Lab_Title2")->Show(m_pLstResult->IsShow());
	GetDlgItem("Lab_Title3")->Show(m_pLstResult->IsShow());

	// clear the icon list
	int iconIndex=0;
	for(;iconIndex<(int)m_pIcons.size();iconIndex++)
	{
		PAUIIMAGEPICTURE pIcon = m_pIcons[iconIndex];
		pIcon->SetCover(NULL, -1);
		pIcon->SetText(_AL(""));
		pIcon->SetHint(_AL(""));
		pIcon->SetData(-1);
		pIcon->SetColor(COLOR_NORMAL);
	}
	m_pSubItems->Show(!m_pLstResult->IsShow());
	GetDlgItem("Sub_BG")->Show(!m_pLstResult->IsShow());

	// fill content
	for(size_t itr=0;itr<m_Infos.size();itr++)
	{
		const ItemInfo& info = m_Infos[itr];

		// filter by button
		if(type != FILTER_MAX && type != info.where)
		{
			continue;
		}

		ACString strItem;

		// fill listbox
		if(m_pLstResult->IsShow())
		{
			strItem.Format(	_AL("%s\t%d\t%d"), 
				// storage
				info.where < FILTER_MAX ? m_pFilters[info.where]->GetText():INVALID_TAG,
				// index
				info.index + 1,
				// count
				info.count );
			
			m_pLstResult->InsertString(m_pLstResult->GetCount(), strItem);
			m_pLstResult->SetItemData(m_pLstResult->GetCount()-1, itr);
		}

		// fill sub dialog
		if(m_pSubItems->IsShow() && info.index < m_pIcons.size())
		{
			PAUIIMAGEPICTURE pIcon = m_pIcons[info.index];
			
			pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
							GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][m_IconFile]);

			pIcon->SetText(strItem.Format(_AL("%d"), info.count));

			strItem.Format(	GetStringFromTable(6356), 
							// storage
							info.where < FILTER_MAX ? m_pFilters[info.where]->GetText():INVALID_TAG,
							// index
							info.index + 1,
							// count
							info.count );
			pIcon->SetHint(strItem);

			pIcon->SetData(itr);
		}
	}
}

void CDlgGMQueryItem::OnGMQueryResult(int iType, const void* pData, DWORD dwSize)
{
	using namespace S2C;

	//
	switch(iType)
	{
	case PLAYER_SPEC_ITEM_LIST:
		{
			if(m_State != STATE_WAIT)
			{
				return;
			}

			const player_spec_item_list* pCmd = (const player_spec_item_list*)pData;
			UpdateTarget(pCmd->roleid, pCmd->type);
			
			// copy result to vector
			m_Infos.clear();
			size_t count = dwSize - sizeof(pCmd->type) - sizeof(pCmd->roleid);
			count /= sizeof(player_spec_item_list::entry_t);
			const player_spec_item_list::entry_t* pStart = pCmd->list;
			
			while(count > 0)
			{
				ItemInfo info;
				info.where = pStart->where;
				info.index = pStart->index;
				info.count = pStart->count;
				
				m_Infos.push_back(info);
				pStart++;
				count--;
			}
			
			// show all result
			RefreshResult(m_FilterType);
		}
		break;

	default:
		break;
	}

}

void CDlgGMQueryItem::OnCommand_ShowType(const char * szCommand)
{
	int iType = FILTER_MAX;
	if( strstr(szCommand, "Btn_Type") )
	{
		iType = min(FILTER_MAX, atoi(szCommand + strlen("Btn_Type")));
	}

	RefreshResult(iType);
}

void CDlgGMQueryItem::OnCommand_DeleteItems(const char * szCommand)
{
	// the selected row
	abase::vector<int> aSels;

	if(m_pLstResult->IsShow())
	{
		if(m_pLstResult->IsMultipleSelection())
		{
			for(int row=0; row<m_pLstResult->GetCount(); row++)
			{
				if(m_pLstResult->GetSel(row))
				{
					aSels.push_back(m_pLstResult->GetItemData(row));
				}
			}
		}
		else
		{
			aSels.push_back(m_pLstResult->GetItemData(m_pLstResult->GetCurSel()));
		}
	}
	else
	{
		for(int iconIndex=0;iconIndex<(int)m_pIcons.size();iconIndex++)
		{
			PAUIIMAGEPICTURE pIcon = m_pIcons[iconIndex];
			if(pIcon->GetColor() == COLOR_SELECTED)
			{
				aSels.push_back(pIcon->GetData());
			}
		}
	}
	

	ItemInfos infos;
	for(int i=0;i<(int)aSels.size();i++)
	{
		int infoIndex = aSels[i];
		if(infoIndex < 0 || infoIndex >= (int)m_Infos.size())
		{
			continue;
		}
		
		infos.push_back(m_Infos[infoIndex]);
	}
	
	// delete from a listbox
	TryToDelete(infos);	
}

void CDlgGMQueryItem::TryToDelete(const ItemInfos& infos)
{
	unsigned int nMax = 0;
	for(int i=0;i<(int)infos.size();i++)
	{
		const ItemInfo& info = infos[i];
		if(nMax < info.count) nMax = info.count;
	}

	// show number input when delete only one item
	if(nMax > 1 && infos.size() == 1)
	{
		CDlgInputNO* pDlg = (CDlgInputNO*)GetGameUIMan()->GetDialog("Win_InputNO");
		ASSERT(pDlg);
		if(pDlg)
		{
			// copy to local storage			
			if(&infos != &m_Deleteing)
			{
				m_Deleteing = infos;
			}
			
			pDlg->SetInfo(1, nMax);
			pDlg->SetType(CDlgInputNO::INPUTNO_GM_REMOVE_ITEM);
			pDlg->Show(true, true);
			return;
		}
	}

	// del directly if no input dialog or not necessary to set number
	ConfirmDelete(infos);
}

void CDlgGMQueryItem::ConfirmCount(int count)
{
	ASSERT(m_Deleteing.size() > 0);

	// set the deleting info to specific count
	ItemInfos infos;
	for(int i=0;i<(int)m_Deleteing.size();i++)
	{
		ItemInfo info = m_Deleteing[i];
		if((int)info.count > count)
		{
			info.count = count;
		}
		infos.push_back(info);
	}

	ConfirmDelete(infos);
}

void CDlgGMQueryItem::ConfirmDelete(const ItemInfos& infos)
{
	if(infos.size() > 0)
	{
		ACString strList;
		for(int i=0;i<(int)infos.size();i++)
		{
			strList += GetItemDesc(infos[i]) + _AL("\r");
		}
		
		ACString strMsg;
		PAUIDIALOG pMsgBox = NULL;
		GetGameUIMan()->MessageBox( "GMQueryItem_DeleteAll", strMsg.Format(GetStringFromTable(6354), strList),
								    MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);

		m_State = STATE_DELETING;

		// copy to local storage
		if(&infos != &m_Deleteing)
		{
			m_Deleteing = infos;
		}

		pMsgBox->SetDataPtr(&m_Deleteing);
	}
}

void CDlgGMQueryItem::OnEventLButtonDBCLK_Result(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommand_DeleteItems(NULL);
}

void CDlgGMQueryItem::OnEventLButtonDBCLK_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int index = pObj->GetData();
	if(index >= 0 && index < (int)m_Infos.size())
	{
		ItemInfos infos;
		infos.push_back(m_Infos[index]);
		TryToDelete(infos);
	}
}

void CDlgGMQueryItem::OnEventLButtonDown_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if(!pIcon) return;
	
	if(wParam & MK_CONTROL)
	{
		// multi-selection
		if(pIcon->GetColor() != COLOR_SELECTED)
		{
			pIcon->SetColor(COLOR_SELECTED);
		}
		else
		{
			pIcon->SetColor(COLOR_NORMAL);
		}
	}
	else
	{
		// single selection
		pIcon->SetColor(COLOR_SELECTED);
		for(int iconIndex=0;iconIndex<(int)m_pIcons.size();iconIndex++)
		{
			if(m_pIcons[iconIndex] != pIcon)
			{
				m_pIcons[iconIndex]->SetColor(COLOR_NORMAL);
			}
		}
	}
}

void CDlgGMQueryItemSub::OnEventLButtonDBCLK_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CDlgGMQueryItem* pDlg = (CDlgGMQueryItem*)GetGameUIMan()->GetDialog("Win_GMQueryItem");
	pDlg->OnEventLButtonDBCLK_Items(wParam, lParam, pObj);
}

void CDlgGMQueryItemSub::OnEventLButtonDown_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CDlgGMQueryItem* pDlg = (CDlgGMQueryItem*)GetGameUIMan()->GetDialog("Win_GMQueryItem");
	pDlg->OnEventLButtonDown_Items(wParam, lParam, pObj);
}

void CDlgGMQueryItem::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(iRetVal == IDYES)
	{
		ItemInfos* pInfos = (ItemInfos*)pDlg->GetDataPtr();
		if(!pInfos) return;

		const ItemInfos& infos = *pInfos;
		for(int i=0;i<(int)infos.size();i++)
		{
			const ItemInfo& info = infos[i];
			GetGameSession()->gm_RemoveSpecItem(m_PlayerID, m_ItemID, info.where, info.index, info.count);
		}

		if(infos.size() > 0)
		{
			GetGameSession()->gm_QuerySpecItem(m_PlayerID, m_ItemID);
		}
	}

	m_State = STATE_WAIT;
}

ACString CDlgGMQueryItem::GetItemDesc(const ItemInfo& info)
{
	ACString strItem;
	strItem.Format(	GetStringFromTable(6355), 
					// storage
					info.where < FILTER_MAX ? m_pFilters[info.where]->GetText():INVALID_TAG,
					// index
					info.index + 1,
					// count
					info.count );
	return strItem;
}

void CDlgGMQueryItem::OnChangeLayoutEnd(bool bAllDone)
{
	AString strName;

	m_pIcons.clear();
	int iconIndex = 1;
	while(true)
	{
		strName.Format("Item_%03d", iconIndex);
		PAUIIMAGEPICTURE pIcon = (PAUIIMAGEPICTURE)m_pSubItems->GetSubDialog()->GetDlgItem(strName);
		if(!pIcon) break;
		m_pIcons.push_back(pIcon);
		iconIndex++;
	}
	
	RefreshResult(m_FilterType);
}