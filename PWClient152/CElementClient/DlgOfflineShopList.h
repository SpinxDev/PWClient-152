/*
 * FILE: DlgOfflineShopList.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"
#include "WikiSearcher.h"
#include <AUIListBox.h>
#include <AUIEditBox.h>
#include <AUILabel.h>
#include <AUIStillImageButton.h>
#include "EC_OfflineShopCtrl.h"
#include <vector>

class CDlgOfflineShopList;

//////////////////////////////////////////////////////////////////////////
//
// ËÑË÷¹¦ÄÜ
//
//////////////////////////////////////////////////////////////////////////


class CShopItemSearcher : public WikiSearcher
{
public:
	CShopItemSearcher();
	virtual ~CShopItemSearcher(){}
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual int  GetPageSize() const;
	
	void DoSearch(const ACString& strText);
	
	void SetViewList(PAUILISTBOX pList){m_pList = pList;};
	
	
protected:
	
	ACString m_strLastPattern;
	abase::vector<int> m_ResultID;
	std::vector<int> m_BackupID;
	PAUILISTBOX	m_pList;
};


//------------------------------------------------------------------------
//
// ShopDataProvider:provide the id from a vector
//
//------------------------------------------------------------------------

class ShopDataProvider : public WikiSearcher::ContentProvider
{
public:
	class Entity : public WikiEntity
	{
	public:
		Entity(unsigned  id):m_ID(id) {}
		unsigned  GetID() const {return m_ID;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_ID == ptr->m_ID;
		}
	private:
		unsigned int m_ID;
	};
	
	ShopDataProvider(int* pData,int count);
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	
	virtual WikiSearcher::ContentProvider* Clone() const;
	
	private:
		int* m_pIDArray;
		int m_IDCount;
		int m_iCurIndex;
};

//////////////////////////////////////////////////////////////////////////

class CDlgShopSearchBase: public CDlgBase
{
protected:
	CDlgShopSearchBase();
	~CDlgShopSearchBase(){};

public:	
	ACString GetSearchPattern();
	void UpdateSearchList();
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnEventDBClk_Lst_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommand_SearchItem(const char *szCommand);
	virtual bool BeforeSearchAction(){return true;};
	virtual bool IsSellMode() { return true;}
	
	void ClearSearchHint(bool bClearTxt=false);
protected:
	virtual void OnTick();
	virtual bool OnInitDialog();

protected:
	PAUILISTBOX	m_pLst_Search;
	PAUIEDITBOX	m_pTxt_Search;
	PAUILABEL	m_pLbl_SearchHint;

	CShopItemSearcher m_searcher;
	ACString m_strLastPattern;
};

//////////////////////////////////////////////////////////////////////////

class CDlgOfflineShopList : public CDlgShopSearchBase
{

	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
public:
	enum{
		SHOPNAME_NULL = 0,
			SHOPNAME_REQUESTING,
			SHOPNAME_DONE,
	};
	enum
	{
		SHOWTYPE_NPCSEV = 0,
			SHOWTYPE_SHOPITEMBACK,
			SHOWTYPE_QUERYITEMBACK,
	};

public:
	CDlgOfflineShopList();
	~CDlgOfflineShopList();
	
	void OnCommand_EnterShop(const char *szCommand);
	
	void OnCommand_PageUp(const char *szCommand);
	void OnCommand_PageDown(const char *szCommand);
	void OnCommand_SellList(const char* szCommand);
	void OnCommand_BuyList(const char* szCommand);
	void OnCommandViewShopList(const char *szCommand);
	
	void UpdateControls();

	bool IsSellMode() { return m_bSellOrBuy;}

	bool BeforeSearchAction();

	void OnCommandCANCEL(const char *szCommand);	

	void ClearControls();

	void SetShowType(int type) { m_iShowType = type; }

protected:	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();	
	
	DWORD m_dwLastListTime;	

	PAUISTILLIMAGEBUTTON 	m_pBtn_PageDown;
	PAUISTILLIMAGEBUTTON 	m_pBtn_PageUp;
	PAUILABEL				m_pLbl_ShopName[SHOP_COUNT_PERPAGE];
	PAUISTILLIMAGEBUTTON 	m_pBtn_EnterShop[SHOP_COUNT_PERPAGE];
	
	bool m_bSellOrBuy; // true: ¼ÄÂô£¬ false£ºÊÕ¹º

	int m_iShowType;
};

