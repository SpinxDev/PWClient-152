/*
 * FILE: DlgOfflineShopList.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#include "DlgBase.h"
#include "DlgOfflineShopList.h"
#include "AUIDef.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "WikiSearchCommand.h"
#include "ExpTypes.h"
#include "WikiItemProp.h"
#include "DlgOfflineShopCreate.h"

#include "DlgOfflineShopItemsList.h"

#define LIST_TICK 5000


AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopList, CDlgBase)
AUI_ON_COMMAND("entershop*",	OnCommand_EnterShop)
AUI_ON_COMMAND("Btn_Search",	OnCommand_SearchItem)
AUI_ON_COMMAND("Btn_PageUp",	OnCommand_PageUp)
AUI_ON_COMMAND("Btn_PageDown",	OnCommand_PageDown)
AUI_ON_COMMAND("Rdo_Sell",		OnCommand_SellList)
AUI_ON_COMMAND("Rdo_Buy",		OnCommand_BuyList)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Set",		OnCommandViewShopList)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgOfflineShopList, CDlgBase)
AUI_ON_EVENT("*",		WM_MOUSEWHEEL,			OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,			OnEventMouseWheel)
AUI_ON_EVENT("Edt_Search",	WM_KEYDOWN,			OnEventKeyDown_Txt_Search)
AUI_ON_EVENT("Lst_Search",	WM_LBUTTONDBLCLK,	OnEventDBClk_Lst_Search)

AUI_END_EVENT_MAP()

CDlgOfflineShopList::CDlgOfflineShopList():
m_dwLastListTime(0),
m_bSellOrBuy(true),
m_pBtn_PageDown(NULL),
m_pBtn_PageUp(NULL),
m_iShowType(SHOWTYPE_NPCSEV)
{
	memset(m_pBtn_EnterShop,0,sizeof(m_pBtn_EnterShop));
	memset(m_pLbl_ShopName,0,sizeof(m_pLbl_ShopName));
}

CDlgOfflineShopList::~CDlgOfflineShopList(){}
	
bool CDlgOfflineShopList::OnInitDialog()
{
	DDX_Control("Lst_Search",m_pLst_Search);
	DDX_Control("Edt_Search",m_pTxt_Search);
	
	DDX_Control("Btn_PageUp",m_pBtn_PageUp);
	DDX_Control("Btn_PageDown",m_pBtn_PageDown);
	
	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		AString str;
		
		str.Format("Lbl_Name%d",i+1);		
		DDX_Control(str,m_pLbl_ShopName[i]);
		
		str.Format("Btn_%d",i+1);
		DDX_Control(str,m_pBtn_EnterShop[i]);
	}
	DDX_Control("Lbl_Search",m_pLbl_SearchHint);

	return CDlgShopSearchBase::OnInitDialog();
}
void CDlgOfflineShopList::OnShowDialog()
{
	COfflineShopCtrl* pCtlr = GetHostPlayer()->GetOfflineShopCtrl();
	if (!pCtlr) return;

	if (m_iShowType == SHOWTYPE_NPCSEV)
	{
		if (pCtlr->GetSellShopCount())
		{
			CheckRadioButton(0,0);
			OnCommand_SellList(NULL);
		}
		else if(pCtlr->GetBuyShopCount())
		{
			CheckRadioButton(0,1);
			OnCommand_BuyList(NULL);
		}
		else
		{
			CheckRadioButton(0,0);
			OnCommand_SellList(NULL);
		}
	}
	else 
	{
		if (m_bSellOrBuy)
		{
			CheckRadioButton(0,0);
			OnCommand_SellList(NULL);
		}
		else
		{
			CheckRadioButton(0,1);
			OnCommand_BuyList(NULL);
		}
	}
	
	m_pTxt_Search->SetText(_AL(""));
	m_pLbl_SearchHint->SetText(_AL(""));
	m_pLst_Search->ResetContent();
}
void CDlgOfflineShopList::OnCommandCANCEL(const char *szCommand)
{
	GetHostPlayer()->GetOfflineShopCtrl()->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_NULL);	
	Show(false);
}
void CDlgOfflineShopList::OnCommand_EnterShop(const char *szCommand)
{
	int idx = atoi(szCommand + strlen("entershop")) - 1;
	if(idx<0 || idx >= SHOP_COUNT_PERPAGE) return;

	int shop_id = m_pBtn_EnterShop[idx]->GetData();
	
	ASSERT(shop_id > 0);
	
	if (shop_id)
	{
		GetGameSession()->OffShop_GetELShopInfo(shop_id);

		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if(pCtrl)
			pCtrl->SetItemState(COfflineShopCtrl::ITEMSTATE_SEND_ENTERSHOP);
	}
	else
		a_LogOutput(1, "CDlgOfflineShopList::OnCommand_EnterShop, error shop id!");
}

void CDlgOfflineShopList::OnCommand_SellList(const char *szCommand)
{
	m_bSellOrBuy = true;
	ClearControls();
}
void CDlgOfflineShopList::OnCommand_BuyList(const char *szCommand)
{
	m_bSellOrBuy = false;
	ClearControls();
}
void CDlgOfflineShopList::OnCommand_PageUp(const char *szCommand)
{
	GetHostPlayer()->GetOfflineShopCtrl()->PreShopPage(m_bSellOrBuy);
	ClearControls();
}

void CDlgOfflineShopList::OnCommand_PageDown(const char *szCommand)
{
	GetHostPlayer()->GetOfflineShopCtrl()->NextShopPage(m_bSellOrBuy);
	ClearControls();
}

void CDlgOfflineShopList::UpdateControls()
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	int ids[SHOP_COUNT_PERPAGE] = {0};
	int send_count = 0;
	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		int id = pCtrl->GetShopID(m_bSellOrBuy,i);
		int flag = m_pLbl_ShopName[i]->GetData();
		if(flag != SHOPNAME_DONE && id!=0)
		{
			const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(id, false);
			if( szName )		
			{	
				m_pLbl_ShopName[i]->SetText(szName);				
				m_pLbl_ShopName[i]->SetData(SHOPNAME_DONE); // 已得到名字
				m_pBtn_EnterShop[i]->SetData(id);
				m_pBtn_EnterShop[i]->Enable(true);
			}	
			else if(flag == SHOPNAME_NULL)
			{
				ids[send_count++] = id;
				m_pLbl_ShopName[i]->SetData(SHOPNAME_REQUESTING);
				m_pLbl_ShopName[i]->SetText(_AL("--"));

				m_pBtn_EnterShop[i]->SetData(id);
				m_pBtn_EnterShop[i]->Enable(true);
			}
		}
		if (m_pLbl_ShopName[i]->GetData() == SHOPNAME_NULL)
		{
			m_pBtn_EnterShop[i]->Enable(false);
			m_pBtn_EnterShop[i]->SetData(0);
			m_pLbl_ShopName[i]->SetText(GetGameUIMan()->GetStringFromTable(10521));	
		}
	}

	if(send_count)
		g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(send_count,ids, 2);
	
	m_pBtn_PageUp->Enable(!pCtrl->IsHeadShopPage(m_bSellOrBuy));
	m_pBtn_PageDown->Enable(!pCtrl->IsTailShopPage(m_bSellOrBuy));
}

void CDlgOfflineShopList::ClearControls()
{
	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		m_pBtn_EnterShop[i]->Enable(false);
		m_pBtn_EnterShop[i]->SetData(0);
		m_pLbl_ShopName[i]->SetText(GetGameUIMan()->GetStringFromTable(10521));		
		m_pLbl_ShopName[i]->SetData(SHOPNAME_NULL); // need update
	}
}

void CDlgOfflineShopList::OnTick()
{
	CDlgShopSearchBase::OnTick();
		
//	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
//	if(!pCtrl) return;
	
// 	DWORD dwTick = GetTickCount();
// 	
// 	if( CECTimeSafeChecker::ElapsedTime(dwTick, m_dwLastListTime) > LIST_TICK && pCtrl->IsTailShopPage(m_bSellOrBuy) ) //
// 	{
// 		// Get shop list
// 		pCtrl->QueryShops();
// 		
// 		m_dwLastListTime = dwTick;
// 	}
	
	UpdateControls();

	GetDlgItem("Lbl_Name1")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Lbl_Name3")->Show(!m_pLst_Search->IsShow());
	
	GetDlgItem("Rdo_Sell")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Rdo_Buy")->Show(!m_pLst_Search->IsShow());
}
bool CDlgOfflineShopList::BeforeSearchAction()
{
	CDlgOfflineShopItemsList* pDlg = GetGameUIMan()->m_pDlgOffShopItemList;
	if (pDlg)
	{		
		pDlg->m_pTxt_Search->SetText(m_pTxt_Search->GetText());
		pDlg->ChangeFocus(pDlg->m_pTxt_Search);
	}

	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(pCtrl)
		pCtrl->SetItemState(COfflineShopCtrl::ITEMSTATE_SEND_QUERYITEM_FROMSHOP);

	return true;
}
void CDlgOfflineShopList::OnCommandViewShopList(const char *szCommand)
{
	COfflineShopCtrl* pCtlr = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtlr) return;

	CDlgOfflineShopType* pType = dynamic_cast<CDlgOfflineShopType*>(GetGameUIMan()->GetDialog("Win_ShopType"));
	if(!pType) return;

	if(pType->IsShow())
		pType->Show(false);
	else
	{		
		pType->SetShopTypeMask(pCtlr->GetShopListViewType());
		pType->Show(true);
	}
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
CDlgShopSearchBase::CDlgShopSearchBase():m_pLst_Search(NULL),m_pTxt_Search(NULL),m_pLbl_SearchHint(NULL){};
ACString CDlgShopSearchBase::GetSearchPattern()
{
	// 
	ACString strPattern;
	if (m_pTxt_Search)
	{
		strPattern = m_pTxt_Search->GetText();
		strPattern.TrimLeft();  
		strPattern.TrimRight();
		
		// 
		strPattern.TrimRight(_AL("?*"));
		strPattern.TrimLeft(_AL("?*"));
	}
	return strPattern;
}

void CDlgShopSearchBase::UpdateSearchList()
{
	if (!m_pTxt_Search)
	{
		m_pLst_Search->Show(false);
		return;
	}
	
	if (GetFocus() == m_pLst_Search)
	{
		return;
	}

	if (GetFocus() != m_pTxt_Search)
	{
		m_pLst_Search->Show(false);
		return;
	}
	
	ACString strPattern = GetSearchPattern();
	if (strPattern != m_strLastPattern)
	{
		//
		m_pLst_Search->ResetContent();
		
		if (!strPattern.IsEmpty())			
			m_searcher.DoSearch(strPattern);
		
		m_strLastPattern = strPattern;
	}	

	m_pLst_Search->Show(m_pLst_Search->GetCount() > 0);
}

void CDlgShopSearchBase::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
// 	if (pObj == m_pLst_Search)
// 	{
// 		return;
// 	}
// 
// 	int x = GET_X_LPARAM(lParam);
// 	int y = GET_Y_LPARAM(lParam);
// 	int zDelta = (short)HIWORD(wParam);
// 	if( zDelta > 0 )
// 		zDelta = 1;
// 	else
// 		zDelta = -1;
}


void CDlgShopSearchBase::OnEventKeyDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_RETURN)
	{
		OnCommand_SearchItem("");
		ChangeFocus(NULL);
	}
	else if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);	//	释放焦点
	else if (m_pLst_Search && m_pLst_Search->GetCount() > 1)
	{
		//	使用上下箭头选择
		int nCurSel = m_pLst_Search->GetCurSel();
		int nNewSel = nCurSel;
		if (nNewSel < 0 || nNewSel >= m_pLst_Search->GetCount())
			nNewSel = 0;

		if (wParam == VK_UP)
		{
			if (nNewSel > 0) nNewSel --;
		}
		else if (wParam == VK_DOWN)
		{
			if (nNewSel+1 < m_pLst_Search->GetCount())
				nNewSel ++;
		}
		if (nNewSel != nCurSel)
		{
			m_pLst_Search->SetCurSel(nNewSel);
			m_pLst_Search->EnsureVisible(nNewSel);
			
		//	m_pTxt_Search->SetText(m_pLst_Search->GetText(nNewSel));
		//	m_pTxt_Search->SetData(m_pLst_Search->GetItemData(nNewSel));			
		}
	}
}	
void CDlgShopSearchBase::OnEventDBClk_Lst_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	//
	if (!m_pLst_Search ||m_pLst_Search->GetCount() <= 0) return;
	int nCurSel = m_pLst_Search->GetCurSel();
	if (nCurSel < 0 || nCurSel >= m_pLst_Search->GetCount())
		nCurSel = 0;
//	ShowSearchItem(itemIndex);

	m_pTxt_Search->SetText(m_pLst_Search->GetText(nCurSel));
	m_pTxt_Search->SetData(m_pLst_Search->GetItemData(nCurSel));			
			
	ChangeFocus(NULL);	//
}

void CDlgShopSearchBase::OnCommand_SearchItem(const char *szCommand)
{	
	m_pTxt_Search->SetText(m_pLst_Search->GetText(m_pLst_Search->GetCurSel()));
	unsigned int id = m_pLst_Search->GetItemData(m_pLst_Search->GetCurSel());
	ACString str = m_pTxt_Search->GetText();
	if (str.GetLength()<1)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10530),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}
	if(id)
	{
		BeforeSearchAction();
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (pCtrl)
		{
			pCtrl->SetQueryItemID(id);
			m_pLbl_SearchHint->SetText(GetGameUIMan()->GetStringFromTable(10517));
			GetGameSession()->OffShop_QueryItemListPage(id,IsSellMode(),0);
		}		
	}
}
void CDlgShopSearchBase::OnTick()
{
	CDlgBase::OnTick();
	
	UpdateSearchList();
}
bool CDlgShopSearchBase::OnInitDialog()
{
	m_searcher.SetViewList(m_pLst_Search);

	return CDlgBase::OnInitDialog();
}
void CDlgShopSearchBase::ClearSearchHint(bool bClearTxt) 
{
	m_pLbl_SearchHint->SetText(_AL(""));

	if (bClearTxt)
	{
		m_pTxt_Search->SetText(_AL(""));
		m_pLst_Search->ResetContent();
	}
}
//////////////////////////////////////////////////////////////////////////


CShopItemSearcher::CShopItemSearcher():m_pList(NULL)
{
	ResetSearchCommand();
	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	SetSearchCommand(&WikiSearchItemByName(false)); // including equipment
}

void CShopItemSearcher::OnBeginSearch()
{
	m_ResultID.clear();	
	m_pList->ResetContent();

//	m_pList->SetAlpha(500);
}

void CShopItemSearcher::OnEndSearch()
{
	// 
	m_BackupID.assign(m_ResultID.begin(),m_ResultID.end());
}

bool CShopItemSearcher::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	unsigned int id = pEE->GetID();
	
	ASSERT(m_pList->GetCount() < GetPageSize());
	
	// get data from entity first
	WikiItemProp* pResult = NULL;
	WikiItemProp* pEP = NULL;
	if (!(pEP = pEE->GetData(pResult)))
	{
		pEP = WikiItemProp::CreateProperty(id);
	}
	ASSERT(pEP);
	if (!pEP) return false;
	
	ACString strItem;	
	strItem.Format(_AL("%s"), pEP->GetName());
	int csel = m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, id);

	WikiSearchByName* pCommand = dynamic_cast<WikiSearchByName*>(GetSearchCommand());
	if(pCommand)
	{
		ACString pat = pCommand->GetPattern();
		pat.CutLeft(1);
		pat.CutRight(1);
		if (strItem == pat)
		{
			m_pList->SetCurSel(csel-1);
		}
	}

	m_ResultID.push_back(id);
	
	// set hint info
	//AUICTranslate trans;
	//m_pList->SetItemHint(m_pList->GetCount()-1, trans.Translate(pEP->GetDesc()));
	
	if(!pResult) delete pEP;
	return true;
}
void CShopItemSearcher::DoSearch(const ACString& strText)
{
	// seal the direct call
	if(!WikiSearchByName::IsSearchPatternValid(strText))
	{
		return;
	}	
	// set pattern to name search command if existed
	WikiSearchByName* pCommand = dynamic_cast<WikiSearchByName*>(GetSearchCommand());
	if(pCommand)
	{
		ACString strOld = pCommand->GetPattern();
		if(!strOld.IsEmpty() && m_ResultID.size() && strText.Find(pCommand->GetPattern())>=0)
			SetContentProvider(&ShopDataProvider(m_BackupID.begin(),m_BackupID.size()));
		else
			SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
		
		pCommand->SetPattern(strText);
	}
	
	TurnPageHome();
}

int CShopItemSearcher::GetPageSize() const
{
	return 256;
}


///////////////////////////////////////////////////////////
//------------------------------------------------------------------------
//
// ShopDataProvider
//
//------------------------------------------------------------------------
ShopDataProvider::ShopDataProvider(int* pData,int count)
:m_pIDArray(pData),m_IDCount(count),m_iCurIndex(0)
{
}

WikiEntityPtr ShopDataProvider::Next(WikiEntityPtr p)
{	
	m_iCurIndex++;
	
	if(m_iCurIndex<m_IDCount)
		return new Entity(m_pIDArray[m_iCurIndex]);
	else return NULL;
}

WikiEntityPtr ShopDataProvider::Begin()
{
	return new Entity(m_pIDArray[m_iCurIndex]);
}

WikiSearcher::ContentProvider* ShopDataProvider::Clone() const
{
	return new ShopDataProvider(m_pIDArray,m_IDCount);
}