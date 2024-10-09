// Filename	: DlgShop.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

struct NPC_SELL_SERVICE;

class CDlgShop : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgShop();
	virtual ~CDlgShop();
	virtual bool Tick(void);

	void OnCommand_set(const char * szCommand);
	void OnCommand_buy(const char * szCommand);
	void OnCommand_sell(const char * szCommand);
	void OnCommand_repair(const char * szCommand);
	void OnCommand_repairall(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclk(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	int GetCurShopSet();
	bool UpdateShop(int nSet);
	bool UpdatePlayer();
	int UpdatePages();
	void UpdateHint(PAUIOBJECT pObj, CECIvtrItem* pItem, const ACHAR* pszHint);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	int m_nCurShopSet;

	PAUILABEL m_pTxt_Gold;
	PAUILABEL m_pTxt_Worth1;
	PAUILABEL m_pTxt_Worth2;
	PAUIOBJECT	m_pTxt_Contrib;
	PAUIOBJECT	m_pTxt_Contrib01;
	PAUIOBJECT	m_pTxt_PlayerForceContrib;
	PAUIOBJECT	m_pTxt_TotalForceContrib;
	PAUIIMAGEPICTURE m_pImgU[IVTRSIZE_NPCPACK];
	PAUIIMAGEPICTURE m_pImgBuy[IVTRSIZE_BUYPACK];
	PAUIIMAGEPICTURE m_pImgSell[IVTRSIZE_SELLPACK];
	PAUISTILLIMAGEBUTTON m_pBtnSet[NUM_NPCIVTR];

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
	int		GetConsumeContrib();
	int		GetCumulateContrib();
	int		GetForceContrib();
	bool	IsPageInvalid(int page);

	NPC_SELL_SERVICE * m_pNPCSellService;
	void UpdateNPCSellService();

	virtual void OnChangeLayoutEnd(bool bAllDone);

	bool UpdatePageButton(int pageIndex, PAUISTILLIMAGEBUTTON pButton);
	int GetTotalContrib(int itemID, int amount, int* pCnotrib, int* pForceContrib);
};
