// File		: DlgGeneralCardCollection.h 
// Creator	: Wang
// Date		: 2013/09/14

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "AUIProgress.h"
#include "AUIComboBox.h"
#include "ExpTypes.h"
#include <vector>
#include <hashmap.h>
#include <set>
#define  CMB_FILTER_COUNT 4

// ¿¨ÅÆÍ¼¼ø
class CDlgGenCardCollection : public CDlgBase  
{
	friend class CECGameUIMan;
	
	AUI_DECLARE_COMMAND_MAP()
		AUI_DECLARE_EVENT_MAP()
		
		
public:
	CDlgGenCardCollection();
	virtual ~CDlgGenCardCollection();
	
	void OnCommandPrev(const char * szCommand);
	void OnCommandNext(const char * szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	
	bool PutCardToChatWin(const POKER_ESSENCE* card);
	void UpdateItemsCurPage(int index);
	
	void ShowPage(int index);
	bool FilterCard(const POKER_ESSENCE* card);
	bool HasSuite(const POKER_ESSENCE* card);
	int ComputePageNum();
	void UpdateSearchResult();
	A2DSprite* GetCardCoverDX(const POKER_ESSENCE* pConfig, int width, int height);

	void UpdateCardImgCache(int index);

protected:
	std::vector<PAUIIMAGEPICTURE> m_pImg_Cards;
	std::vector<PAUIIMAGEPICTURE> m_pImg_CardBorder;

	std::vector<PAUIOBJECT>			m_pLbl_Names;
	std::vector<CECIvtrItem*>	m_pItemsInCurPage;

	std::vector<A2DSprite*> m_pCardBorders;

	bool	m_bNeedUpdateItems;

	abase::hash_map<AString,A2DSprite*> m_card_cover;
	
	PAUIOBJECT m_pLbl_PageIndex;

	PAUISTILLIMAGEBUTTON m_pBtn_Next;
	PAUISTILLIMAGEBUTTON m_pBtn_Prev;

	PAUIPROGRESS	m_pProg;

	std::vector<POKER_ESSENCE*> cards;
	std::vector<POKER_ESSENCE*> search_ret_cards;

	int m_curPage;
	int m_cardCountPerPage;
	int m_totalPageNum;

	int m_search_filter[4]; // rank, type, suite, has

	std::set<int> m_suite_set;

	PAUICOMBOBOX m_pCmb_Filter[CMB_FILTER_COUNT];
};
