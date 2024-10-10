// Filename	: DlgAuctionSearch.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/10

#include "AFileImage.h"
#include "DlgAuctionSearch.h"
#include "DlgAuctionBuyList.h"
#include "CSplit.h"
#include "AUICommon.h"
#include "AUICTranslate.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "A3DDevice.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

#define LIST_TICK 5000

AUI_BEGIN_COMMAND_MAP(CDlgAuctionSearch, CDlgBase)

AUI_ON_COMMAND("searchname",		OnCommandSearchName)
AUI_ON_COMMAND("searchall",			OnCommandSearchAll)
AUI_ON_COMMAND("searchitem",		OnCommandSearchItem)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAuctionSearch, CDlgBase)

AUI_ON_EVENT("Lst_Item",WM_LBUTTONDOWN,		OnEventLButtonDown_Lst_Item)
AUI_ON_EVENT("Tv_Item",	WM_LBUTTONDOWN,		OnEventLButtonDown_Tv_Item)
AUI_ON_EVENT("Tv_Item",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Tv_Item)

AUI_END_EVENT_MAP()

CDlgAuctionSearch::CDlgAuctionSearch()
{
	m_pTv_Item = NULL;
	m_pLst_Item = NULL;
	m_pTxt_Name = NULL;
	m_pBtn_SearchItem =  NULL;
}

CDlgAuctionSearch::~CDlgAuctionSearch()
{
}

bool CDlgAuctionSearch::OnInitDialog()
{
	m_pItemOld = NULL;
	DDX_Control("Tv_Item", m_pTv_Item);
	DDX_Control("Lst_Item", m_pLst_Item);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Btn_SearchItem", m_pBtn_SearchItem);
	m_pTv_Item->DeleteAllItems();

	AFileImage fList;
	if( fList.Open("surfaces\\ingame\\auctiontree.txt",AFILE_OPENEXIST | AFILE_TYPE_BINARY | AFILE_TEMPMEMORY) )
	{
		
		ACHAR *szData = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (fList.GetFileLength() / sizeof(ACHAR) + 1));
		DWORD dwLen;
		fList.Read(szData, fList.GetFileLength(), &dwLen);
		fList.Close();
		dwLen /= sizeof(ACHAR);
		ACHAR szCurLine[100];
		int nLastTab = -1;
		P_AUITREEVIEW_ITEM pItem = m_pTv_Item->GetRootItem();
		int start = 1;
		DWORD pos;
		do
		{
			pos = start;
			if( pos == dwLen )
				break;
			while( pos < dwLen && szData[pos] != '\r' && szData[pos] != '\n' )
				pos++;
			a_strncpy(szCurLine, szData + start, pos - start);
			szCurLine[pos - start] = 0;

			while( pos < dwLen && (szData[pos] == '\r' || szData[pos] == '\n') )
				pos++;
			start = pos;
			ACHAR *p = szCurLine;
			int nTab = 0;
			while( *p == '\t' )
			{
				p++;
				nTab++;
			}
			int i;
			for( i = nTab - 1; i < nLastTab; i++ )
				pItem = m_pTv_Item->GetParentItem(pItem);
			nLastTab = nTab;
			CSplit s(p);
			CSplit::VectorAWString vec = s.Split(_AL("\t"));
			if (vec.empty())
			{
				a_freetemp(szData);
				a_LogOutput(1, "Invalid line at Line %d in surfaces\\ingame\\auctiontree.txt.", m_pTv_Item->GetCount());
				return false;
			}
			pItem = m_pTv_Item->InsertItem(vec[0], pItem);
			m_pTv_Item->Expand(pItem, AUITREEVIEW_EXPAND_COLLAPSE);
			if( vec.size() == 2 )
				m_pTv_Item->SetItemData(pItem, a_atoi(vec[1]));
			else
				m_pTv_Item->SetItemData(pItem, 0);
		} while(true);
		fList.Close();
		a_freetemp(szData);
	}
	else
		AUI_ReportError(__LINE__, 1,"CDlgAuctionSearch::OnInitDialog(), auction tree file error!");

	return true;
}

void CDlgAuctionSearch::OnShowDialog()
{
	ChangeFocus(m_pTxt_Name);
}

void CDlgAuctionSearch::OnEventLButtonDown_Tv_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pLst_Item->SetCurSel(m_pLst_Item->GetCount());
}

void CDlgAuctionSearch::OnEventLButtonDBLCLK_Tv_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int x, y;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	x = GET_X_LPARAM(lParam) - p->X;
	y = GET_Y_LPARAM(lParam) - p->Y;
	bool bOnIcon;
	POINT ptObj = m_pTv_Item->GetPos();
	P_AUITREEVIEW_ITEM pItem = m_pTv_Item->HitTest(
		x - ptObj.x, y - ptObj.y, &bOnIcon);
	if( pItem && m_pTv_Item->GetItemData(pItem) != 0 )
		OnCommandSearchItem("");
}

void CDlgAuctionSearch::OnEventLButtonDown_Lst_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pTv_Item->SelectItem(NULL);
}

void CDlgAuctionSearch::OnTick()
{
	POINT ptPos;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetCursorPos(&ptPos);
	ScreenToClient(m_pA3DDevice->GetDeviceWnd(), &ptPos);
	ptPos.x -= p->X;
	ptPos.y -= p->Y;
	ACString strHint;
	bool bOnIcon;
	POINT ptObj = m_pTv_Item->GetPos();
	P_AUITREEVIEW_ITEM pItem = m_pTv_Item->HitTest(
		ptPos.x - ptObj.x, ptPos.y - ptObj.y, &bOnIcon);
	m_pItemOld = pItem;
	m_pBtn_SearchItem->Enable(false);
	pItem =  m_pTv_Item->GetSelectedItem();
	if( pItem && m_pTv_Item->GetItemData(pItem) != 0 )
		m_pBtn_SearchItem->Enable(true);
	else
	{
		int nSel = m_pLst_Item->GetCurSel();
		if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
			m_pBtn_SearchItem->Enable(true);
	}
}

void CDlgAuctionSearch::OnCommandSearchName(const char* szCommand)
{
	ACString szName = m_pTxt_Name->GetText();
	if( szName.GetLength() == 0 )
		return;

	m_pLst_Item->ResetContent();
	AuctionNameMap::iterator it = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionName.begin();
	for (; it != GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionName.end(); ++it)
		if( a_strstr(it->second, szName) != NULL )
		{
			m_pLst_Item->AddString(it->second);
			m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, it->first);
			CECIvtrItem *pIvtr = CECIvtrItem::CreateItem(it->first, 0, 1);
			pIvtr->GetDetailDataFromLocal();
			pIvtr->SetPriceScale(CECIvtrItem::SCALE_BUY, 1.0f);

			ACString strHint = GetGameUIMan()->GetItemDescHint(pIvtr, true, true);
			m_pLst_Item->SetItemHint(m_pLst_Item->GetCount() - 1, strHint);
			delete pIvtr;
		}
	if( m_pLst_Item->GetCount() > 0 )
		m_pTv_Item->SelectItem(NULL);
}

void CDlgAuctionSearch::OnCommandSearchItem(const char* szCommand)
{
	P_AUITREEVIEW_ITEM pItem =  m_pTv_Item->GetSelectedItem();
	if( pItem && m_pTv_Item->GetItemData(pItem) != 0 )
	{
		GetGameUIMan()->m_pDlgAuctionBuyList->m_nSubID = 0;
		GetGameUIMan()->m_pDlgAuctionBuyList->m_nIDNext = m_pTv_Item->GetItemData(pItem);
		GetGameUIMan()->m_pDlgAuctionBuyList->m_nPageBegin = 0;
		GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_Search->SetText(m_pTv_Item->GetItemText(pItem));
		ACString szText;
		szText.Format(GetStringFromTable(3501), m_pTv_Item->GetItemText(pItem));
		GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_RefreshStatus->SetText(szText);
		Show(false);
	}
	else
	{
		int nSel = m_pLst_Item->GetCurSel();
		if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
		{
			int subID = m_pLst_Item->GetItemData(nSel);
			GetGameUIMan()->m_pDlgAuctionBuyList->m_nSubID = subID;
			GetGameUIMan()->m_pDlgAuctionBuyList->m_nIDNext = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionID[subID];
			GetGameUIMan()->m_pDlgAuctionBuyList->m_nPageBegin = 0;
			GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_Search->SetText(m_pLst_Item->GetText(nSel));
			ACString szText;
			szText.Format(GetStringFromTable(3501), m_pLst_Item->GetText(nSel));
			GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_RefreshStatus->SetText(szText);
			Show(false);
		}
	}
}

void CDlgAuctionSearch::OnCommandSearchAll(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionBuyList->m_nIDNext = 0;
	GetGameUIMan()->m_pDlgAuctionBuyList->m_nSubID = 0;
	GetGameUIMan()->m_pDlgAuctionBuyList->m_nPageBegin = 0;
	GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_Search->
		SetText(GetStringFromTable(3509));
	ACString szText;
	szText.Format(GetStringFromTable(3501), GetStringFromTable(3509));
	GetGameUIMan()->m_pDlgAuctionBuyList->m_pTxt_RefreshStatus->SetText(szText);
	Show(false);
}
