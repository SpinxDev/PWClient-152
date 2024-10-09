// Filename	: DlgInventoryDetail.cpp
// Creator	: Feng Ning
// Date		: 2011/10/26

#include "DlgInventoryDetail.h"
#include "EC_Inventory.h"
#include "EC_GameRun.h"
#include "EC_IvtrItem.h"
#include "AUICTranslate.h"
#include "AUILabel.h"
#include "EC_GameUIMan.h"

#include "AFI.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInventoryDetail, CDlgBase)
AUI_END_COMMAND_MAP()


CDlgInventoryDetail::CDlgInventoryDetail()
{
	m_pIvtr = NULL;
	m_pSubItems = NULL;
}

CDlgInventoryDetail::~CDlgInventoryDetail()
{
}

bool CDlgInventoryDetail::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
		return false;

	m_pIvtr = new CECInventory();
	
	// initialize icon set
	DDX_Control("Sub_Items", m_pSubItems);

	OnChangeLayoutEnd(true);

	return true;
}

bool CDlgInventoryDetail::Release()
{
	A3DRELEASE(m_pIvtr);
	return CDlgBase::Release();
}

void CDlgInventoryDetail::OnInventoryDetail(S2C::cmd_player_inventory_detail* pDetail)
{
	if( pDetail &&
		m_pIvtr->ResetWithDetailData(pDetail->inv_size, (void*)pDetail->content, pDetail->content_length) )
	{
		// update the inventory UI
		RefreshResult();
		
		// update title
		PAUILABEL pTxtTitle = (PAUILABEL)GetDlgItem("Txt_Title");
		if(pTxtTitle)
		{
			ACString strTitle;
			strTitle.Format(GetStringFromTable(9383), GetGameRun()->GetPlayerName(pDetail->id, true));
			pTxtTitle->SetText(strTitle);
		}

		if(!IsShow()) Show(true);
	}
	else
	{
		m_pIvtr->RemoveAllItems();
	}

	// hide when empty
	if( IsShow() && m_pIvtr->GetEmptySlotNum() == m_pIvtr->GetSize())
	{
		Show(false);
	}
}

void CDlgInventoryDetail::OnTick()
{
	CDlgBase::OnTick();
}

void CDlgInventoryDetail::OnShowDialog()
{
	CDlgBase::OnShowDialog();
}

void CDlgInventoryDetail::OnChangeLayoutEnd(bool bAllDone)
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

	RefreshResult();
}


void CDlgInventoryDetail::RefreshResult()
{
	// clear the icon list
	size_t iconIndex=0;
	for(;iconIndex<m_pIcons.size();iconIndex++)
	{
		PAUIIMAGEPICTURE pIcon = m_pIcons[iconIndex];
		pIcon->SetCover(NULL, -1);
		pIcon->SetText(_AL(""));
		pIcon->SetHint(_AL(""));
		pIcon->SetData(-1);
	}

	// fill content
	iconIndex = 0;
	for(int itr=0;itr<m_pIvtr->GetSize() && iconIndex<m_pIcons.size();itr++)
	{
		CECIvtrItem* pItem = m_pIvtr->GetItem(itr);
		if(!pItem) continue;

		// fill sub dialog
		PAUIIMAGEPICTURE pIcon = m_pIcons[iconIndex++];

		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		pIcon->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
						 GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY,strFile));

		ACString strItem;
		pIcon->SetText(strItem.Format(_AL("%d"), pItem->GetCount()));

		AUICTranslate trans;
		pIcon->SetHint(trans.Translate(pItem->GetDesc()));

		pIcon->SetData(itr);
	}
}