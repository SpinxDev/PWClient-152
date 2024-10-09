 // File	: DlgGeneralCardCollection.cpp 
// Creator	: Wang
// Date		: 2013/09/13

#include "DlgGeneralCardCollection.h"
#include "DlgTreasureMap.h"
#include "elementdataman.h"
#include <AUICTranslate.h>
#include <A2DSprite.h>
#include "EC_Game.h"
#include "EC_Global.h"
#include <algorithm>
#include <ITERATOR>
#include "AUIDef.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_Utility.h"
#include <AFI.h>
#include "EC_Inventory.h"
#include "DlgItemDesc.h"
#include "DlgGeneralCard.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGenCardCollection, CDlgBase)

AUI_ON_COMMAND("Btn_PageDown",		OnCommandNext)
AUI_ON_COMMAND("Btn_PageUp",		OnCommandPrev)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgGenCardCollection, CDlgBase)
AUI_ON_EVENT("Img_CardFrame*", WM_LBUTTONDOWN,	 OnEventLButtonDown)
AUI_END_EVENT_MAP()

/*
	C,B,A,S,S+ : 0,1,2,3,4
	破军、破阵、长生、完璧、玄魂、玄命: 0,1,2,3,4,5
*/
struct show_order_functor
{
	bool operator () (const POKER_ESSENCE* rhs1, const POKER_ESSENCE* rhs2)
	{
		if(!rhs1 || !rhs2) return true;		
		if (rhs1->rank > rhs2->rank){
			return true;
		}else if (rhs1->rank < rhs2->rank){
			return false;
		}else{
			return rhs1->show_order < rhs2->show_order;
		}
	}
};


CDlgGenCardCollection::CDlgGenCardCollection():m_cardCountPerPage(0),m_totalPageNum(0)
{
	memset(m_search_filter,0,sizeof(m_search_filter));
}

CDlgGenCardCollection::~CDlgGenCardCollection()
{
	OnHideDialog();
}

bool CDlgGenCardCollection::OnInitDialog()
{
	int i=0;
	m_pImg_Cards.clear();
	m_pLbl_Names.clear();
	m_pImg_CardBorder.clear();
	while(true)
	{
		AString str;
		str.Format("Img_CardFrame%d",++i);
		PAUIIMAGEPICTURE p = (PAUIIMAGEPICTURE)GetDlgItem(str);
		if(!p) break;
		m_pImg_CardBorder.push_back(p);

		str.Format("Img_Card%02d",i);
		p = (PAUIIMAGEPICTURE)GetDlgItem(str);
		if(!p) break;
		m_pImg_Cards.push_back(p);		

		str.Format("Txt_Card%02d",i);
		PAUIOBJECT pName = GetDlgItem(str);
		if(!pName) break;
		m_pLbl_Names.push_back(pName);
	}

	m_cardCountPerPage = m_pImg_Cards.size();

	m_pBtn_Prev = NULL;
	m_pBtn_Next = NULL;
	m_pProg = NULL;
	m_pLbl_PageIndex = NULL;

	DDX_Control("Txt_PageNum",m_pLbl_PageIndex);
	DDX_Control("Pro_Stage",m_pProg);
	DDX_Control("Btn_PageUp",m_pBtn_Prev);
	DDX_Control("Btn_PageDown",m_pBtn_Next);

	memset(m_pCmb_Filter,0,sizeof(m_pCmb_Filter));

	DDX_Control("Combo_Filter1",m_pCmb_Filter[0]);
	DDX_Control("Combo_Filter2",m_pCmb_Filter[1]);
	DDX_Control("Combo_Filter3",m_pCmb_Filter[2]);
	DDX_Control("Combo_Filter4",m_pCmb_Filter[3]);

	cards.clear();
	search_ret_cards.clear();

	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_ESSENCE,DataType);
		
	while(tid)
	{
		if(DataType == DT_POKER_ESSENCE)
		{
			POKER_ESSENCE* pCard = (POKER_ESSENCE*)pDataMan->get_data_ptr(tid,ID_SPACE_ESSENCE, DataType);
			if (pCard && pCard->show_order >= 0)
				cards.push_back(pCard);			
		}
		else if (DataType == DT_POKER_SUITE_ESSENCE)
		{
			POKER_SUITE_ESSENCE* pSuite = (POKER_SUITE_ESSENCE*)pDataMan->get_data_ptr(tid,ID_SPACE_ESSENCE, DataType);
			if (pSuite)
			{
				for (int i=0;i<sizeof(pSuite->list)/sizeof(pSuite->list[0]) && pSuite->list[i];i++)
				{
					m_suite_set.insert(pSuite->list[i]);
				}
			}
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}

	std::sort(cards.begin(),cards.end(),show_order_functor());

	std::copy(cards.begin(),cards.end(),std::back_inserter(search_ret_cards));

	m_curPage = 0;

	m_totalPageNum = ComputePageNum();

	return CDlgBase::OnInitDialog();
}
int CDlgGenCardCollection::ComputePageNum()
{
	return (int)ceil(search_ret_cards.size() / (float)m_cardCountPerPage);
}
void CDlgGenCardCollection::OnShowDialog()
{
	int i=0;
	for(i=0;i<CMB_FILTER_COUNT;i++)
		m_pCmb_Filter[i]->ResetContent();

	for (i=0;i<6;i++)
	{
		m_pCmb_Filter[0]->AddString(GetGameUIMan()->GetStringFromTable(11002+i));
	}

	m_pCmb_Filter[1]->AddString(GetGameUIMan()->GetStringFromTable(11008));
	for (i=0;i<6;i++)
	{
		m_pCmb_Filter[1]->AddString(GetGameUIMan()->GetStringFromTable(360+i));
	}

	for (i=0;i<3;i++)
	{
		m_pCmb_Filter[2]->AddString(GetGameUIMan()->GetStringFromTable(11009+i));
	}

	m_pCmb_Filter[3]->AddString(GetGameUIMan()->GetStringFromTable(11008));
	for (i=0;i<2;i++)
	{
		m_pCmb_Filter[3]->AddString(GetGameUIMan()->GetStringFromTable(11012+i));
	}
	
	for (i=0;i<CMB_FILTER_COUNT;i++)
	{
		m_pCmb_Filter[i]->SetCurSel(0);
	}
	m_bNeedUpdateItems = true;

	for (i=0;i<ELEMENTDATA_NUM_POKER_RANK;i++)
	{
		A2DSprite *pSprite = NULL;
		pSprite = GetCoverDX(CDlgGeneralCard::GetCardFrameFile(i), 
			m_pImg_CardBorder[i]->GetSize().cx, m_pImg_CardBorder[i]->GetSize().cy);
		if (!pSprite) return;	
		pSprite->SetLinearFilter(true);	
		m_pCardBorders.push_back(pSprite);
	}
}
void CDlgGenCardCollection::UpdateItemsCurPage(int index)
{
	if (m_bNeedUpdateItems)
	{
		m_bNeedUpdateItems = false;
		int i=0;
		for (;i<(int)m_pItemsInCurPage.size();i++)
		{
			delete m_pItemsInCurPage[i];
		}
		m_pItemsInCurPage.clear();

		for (i=0;i< m_cardCountPerPage; i++)
		{
			int pos = index * m_cardCountPerPage + i;
			if (pos<(int)search_ret_cards.size())
			{
				POKER_ESSENCE* card = search_ret_cards[pos];
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(card->id,0,1);
				if(pItem)
					pItem->GetDetailDataFromLocal();
				m_pItemsInCurPage.push_back(pItem);
			}
		}
	}
}
void CDlgGenCardCollection::OnHideDialog()
{
	abase::hash_map<AString,A2DSprite*>::iterator itr = m_card_cover.begin();
	for (;itr!=m_card_cover.end();++itr)
	{
		A3DRELEASE(itr->second);
	}
	m_card_cover.clear();

	for (unsigned int i=0;i<m_pItemsInCurPage.size();i++)
	{
		delete m_pItemsInCurPage[i];
	}
	m_pItemsInCurPage.clear();
	for (i=0;i<m_pCardBorders.size();i++)
	{
		A3DRELEASE(m_pCardBorders[i]);
	}
	m_pCardBorders.clear();
}

void CDlgGenCardCollection::OnCommandPrev(const char * szCommand)
{
	if (m_curPage>0)
	{
		m_curPage--;
		m_bNeedUpdateItems = true;
	}
}

void CDlgGenCardCollection::OnCommandNext(const char * szCommand)
{
	if (m_curPage<m_totalPageNum-1)
	{
		m_curPage++;
		m_bNeedUpdateItems = true;
	}
}

void CDlgGenCardCollection::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(AUI_PRESS(VK_SHIFT) && strstr(pObj->GetName(),"Img_CardFrame")>=0)
	{
		int i = atoi(pObj->GetName() + strlen("Img_CardFrame")) - 1;
		if(i >= (int)search_ret_cards.size() || i<0) return;

		POKER_ESSENCE* card = (POKER_ESSENCE*)search_ret_cards[i];
		PutCardToChatWin(card);
	}
}
bool CDlgGenCardCollection::PutCardToChatWin(const POKER_ESSENCE* card)
{
	if(!card) return false;
	// 向聊天栏输入卡牌
	//
	bool bSuccess(false);

	// 查找当前活跃的聊天窗口
	PAUIDIALOG pDlgChat = GetGameUIMan()->GetActiveChatDialog();

	// 向活跃聊天窗口发送LINK信息
	while (pDlgChat)
	{
		PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pDlgChat->GetDlgItem("DEFAULT_Txt_Speech"));
		if (!pEdit)
			pEdit = dynamic_cast<PAUIEDITBOX>(pDlgChat->GetDlgItem("Txt_Content"));
		
		if (!pEdit )		
			break;		
		
		bool bHas = GetHostPlayer()->GetGeneralCardData().HasCard(card->show_order);
		if (bHas && pEdit->GetItemCount(enumEIIvtrlItem) < 1)
		{
			// 每次只能发一个
			//			
			CECInventory* pIvtr = GetHostPlayer()->GetPack(IVTRTYPE_CLIENT_GENERALCARD_PACK);
			if(!pIvtr ) return false;
			if (pIvtr->GetEmptySlotNum()==0)
				pIvtr->Resize(pIvtr->GetSize() + 32);

			int first = -1;
			first = pIvtr->FindItem(card->id);

			CECIvtrGeneralCard* pTempCard = NULL;
			if(first == -1)
			{
				pTempCard = (CECIvtrGeneralCard*)CECIvtrItem::CreateItem(card->id,0,1);
				if (pTempCard)
				{
					pTempCard->GetDetailDataFromLocal();
					first = pIvtr->SearchEmpty();
					pIvtr->PutItem(first,pTempCard);	
				}				
			}
			else 
				pTempCard = (CECIvtrGeneralCard*)pIvtr->GetItem(first);

			if(!pTempCard) return false;

			ACString strName;
			A3DCOLOR clrName;
			GetGameUIMan()->TransformNameColor(pTempCard, strName, clrName);
				
			//	加入ID信息，通过消息传递供GT显示用
			ACString strInfo;
			strInfo.Format(_AL("%s%s"), A3DCOLOR_TO_STRING(clrName), strName);
				
			if (pEdit->AppendItem(enumEIIvtrlItem, clrName, strName, strInfo))
			{
				// 成功添加物品				
				pEdit->SetIvtrItemLocInfo(IVTRTYPE_CLIENT_GENERALCARD_PACK, first);
				bSuccess = true;
			}
		}
		GetGameUIMan()->BringWindowToTop(pDlgChat);
		pDlgChat->ChangeFocus(pEdit);
		break;
	}

	return bSuccess;
}
bool CDlgGenCardCollection::FilterCard(const POKER_ESSENCE* card)
{
	if(m_search_filter[0] && m_search_filter[0] != card->rank + 1) return false;

	if(m_search_filter[1])
	{
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		const POKER_SUB_TYPE* pDBSubType	= (const POKER_SUB_TYPE*)pDataMan->get_data_ptr(card->id_sub_type, ID_SPACE_ESSENCE, DataType);
		if(!pDBSubType) return false;
		if (m_search_filter[1] != pDBSubType->type+1) return false;
	}

	if(m_search_filter[2] && ((m_search_filter[2] == 1) != HasSuite(card))) return false;

	if(m_search_filter[3] && ((m_search_filter[3] == 1) != GetHostPlayer()->GetGeneralCardData().HasCard(card->show_order))) return false;

	return true;
}
bool CDlgGenCardCollection::HasSuite(const POKER_ESSENCE* card)
{
	return m_suite_set.find(card->id) != m_suite_set.end();
}
void CDlgGenCardCollection::UpdateSearchResult()
{
	bool bDirty = false;
	for (int i=0;i<CMB_FILTER_COUNT;i++)
	{
		if(m_search_filter[i] != m_pCmb_Filter[i]->GetCurSel())
		{
			m_search_filter[i] = m_pCmb_Filter[i]->GetCurSel();
			bDirty = true;
		}
	}
	if (bDirty)
	{
		m_bNeedUpdateItems = true;
		search_ret_cards.clear();
		for (int i=0;i<(int)cards.size();i++)
		{
			if (FilterCard(cards[i]))
			{
				search_ret_cards.push_back(cards[i]);
			}
		}
 
		m_totalPageNum = ComputePageNum();
		m_curPage = 0;
	}
}

A2DSprite* CDlgGenCardCollection::GetCardCoverDX(const POKER_ESSENCE* pConfig, int width, int height)
{
	A2DSprite* ret = NULL;
	if (pConfig) {
		AString str = pConfig->file_img;
		AString strFile;
		af_GetFileTitle(str, strFile);
		if (strFile.GetLength()<2)
			return GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY];				
		
		if (m_card_cover.find(pConfig->file_img) != m_card_cover.end())
			return m_card_cover[pConfig->file_img];
		if (m_card_cover.size()> m_pImg_Cards.size())
			ASSERT(FALSE);
		ret = GetCoverDX(pConfig->file_img, width, height);
		if (ret) m_card_cover[pConfig->file_img] = ret;
	}
	return ret;
}

void CDlgGenCardCollection::UpdateCardImgCache(int index)
{
	int pos = index * m_cardCountPerPage;
	abase::hash_map<AString,A2DSprite*>::iterator itr = m_card_cover.begin();
	while(itr!=m_card_cover.end())
	{
		bool bNeed = false;
		for (int i=pos;i<min(pos+m_cardCountPerPage,(int)search_ret_cards.size());i++)
		{
			if( itr->first==search_ret_cards[i]->file_img)
			{
				bNeed = true;
				break;
			}
		}
		if(bNeed)
			++itr;
		else
		{
			A3DRELEASE(itr->second);
			itr = m_card_cover.erase(itr);		
		}
	}
}
void CDlgGenCardCollection::ShowPage(int index)
{
	m_pBtn_Prev->Enable(index>0);
	m_pBtn_Next->Enable(index< m_totalPageNum-1);

	UpdateItemsCurPage(index);

	for (int i=0;i< m_cardCountPerPage; i++)
	{
		int pos = index * m_cardCountPerPage + i;
		if (pos<(int)search_ret_cards.size())
		{
			UpdateCardImgCache(index);
			POKER_ESSENCE* card = search_ret_cards[pos];
			bool bHas = GetHostPlayer()->GetGeneralCardData().HasCard(card->show_order);
			
			m_pImg_Cards[i]->Show(true);
			m_pImg_Cards[i]->SetCover(GetCardCoverDX(card, m_pImg_Cards[i]->GetSize().cx, m_pImg_Cards[i]->GetSize().cy),0);
			m_pImg_Cards[i]->SetDataPtr(card);
			m_pImg_Cards[i]->SetData(bHas ? 1:2);
			m_pImg_Cards[i]->SetRenderTop(bHas);
			CECIvtrItem* pItem = i < (int)m_pItemsInCurPage.size() ? m_pItemsInCurPage[i]:NULL;
			pItem = bHas ? pItem:NULL;
			m_pImg_CardBorder[i]->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, reinterpret_cast<unsigned long>(pItem), NULL, NULL);
			m_pImg_CardBorder[i]->SetDataPtr(pItem,"ptr_CECIvtrItem");
			m_pLbl_Names[i]->SetText(card->name);
			m_pImg_CardBorder[i]->SetRenderTop(bHas);
			if(card->rank<(int)m_pCardBorders.size())			
				m_pImg_CardBorder[i]->SetCover(m_pCardBorders[card->rank],0);			
			else
				m_pImg_CardBorder[i]->ClearCover();
		}
		else
		{
			m_pImg_Cards[i]->ClearCover();
			m_pImg_Cards[i]->Show(false);
			m_pImg_Cards[i]->SetDataPtr(NULL);
			m_pImg_Cards[i]->SetData(0);
			m_pImg_CardBorder[i]->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, NULL, NULL, NULL);
			m_pLbl_Names[i]->SetText(_AL(""));
			m_pImg_CardBorder[i]->ClearCover();	
		}
	}

	int has = 0;
	for (i=0;i<(int)cards.size();i++)
	{
		POKER_ESSENCE* card = cards[i];
		bool bHas = GetHostPlayer()->GetGeneralCardData().HasCard(card->show_order);
		if(bHas)
			has++;
	}
	int pro = cards.size() ? AUIPROGRESS_MAX * has / cards.size():0;
	m_pProg->SetProgress(pro);
	ACString str;
	str.Format(_AL("%d/%d"),has,cards.size());
	GetDlgItem("Txt_Per")->SetText(str);

	
	str.Format(_AL("%d/%d"),m_curPage+1,m_totalPageNum);
	m_pLbl_PageIndex->SetText(str);
}
void CDlgGenCardCollection::OnTick()
{
	UpdateSearchResult();
	ShowPage(m_curPage);
}