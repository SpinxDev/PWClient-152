// File		: DlgWebViewProduct.cpp
// Creator	: Feng Ning
// Date		: 2010/3/18

#include "DlgWebViewProduct.h"
#include "DlgWebMyShop.h"
#include "DlgAutoLock.h"
#include "AFileImage.h"
#include "CSplit.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_UIConfigs.h"
#include "EC_Global.h"

#include "webtradelist_re.hpp"
#include "webtradeattendlist_re.hpp"
#include "webtradegetitem_re.hpp"
#include "webtradegetdetail_re.hpp"
#include "webtradeprecancelpost_re.hpp"
#include "webtradeupdate_re.hpp"
#include "Network/ssogetticket_re.hpp"

#define new A_DEBUG_NEW

//	局部函数
static GNET::Octets GetWebTradeTicketContext()
{
	const char *szContext = "ticket_webtrade";
	return GNET::Octets(szContext, strlen(szContext));
}

//	class CDlgWebViewProduct
#define ONLYFORME_CATEGORY_ID		-1
#define ALLITEM_CATEGORY_ID         0
#define UNKNOWN_CATEGORY_ID         1

AUI_BEGIN_COMMAND_MAP(CDlgWebViewProduct, CDlgWebTradeBase)

AUI_ON_COMMAND("goto_myshop",		OnCommandGotoMyShop)
AUI_ON_COMMAND("search_byid",		OnCommandSearchByID)
AUI_ON_COMMAND("search_itemforme",	OnCommandSearchItemforme)
AUI_ON_COMMAND("sort_*",			OnCommandSortByCommand)
AUI_ON_COMMAND("sortdata_*",		OnCommandSortDataByCommand)
AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWebViewProduct, CDlgWebTradeBase)

AUI_ON_EVENT("Tree_Item",	WM_LBUTTONDOWN,		OnEventLButtonDown_Tree_Item)
AUI_ON_EVENT("Tree_Item",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Tree_Item)

AUI_END_EVENT_MAP()

#define WEB_VIEWPRODUCT_MSGBOX(str) GetGameUIMan()->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

CDlgWebViewProduct::CDlgWebViewProduct()
:m_pTxt_TradeID(NULL)
,m_pTv_Item(NULL)
,m_PageBegin(0)
,m_PageEnd(-1)
,m_Category(0)
,m_nColumnClick(-1)
{

}

CDlgWebViewProduct::~CDlgWebViewProduct()
{
}

bool CDlgWebViewProduct::OnInitDialog()
{
	DDX_Control("Txt_SearchID", m_pTxt_TradeID);
	DDX_Control("Tree_Item", m_pTv_Item);

	// tab style
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Viewproduct"))->SetPushed(true);

	InitCategory();
	
	// TODO: temp hide these two controls
	if(m_pTxt_TradeID) m_pTxt_TradeID->Show(false);
	PAUIOBJECT pBtn = GetDlgItem("Btn_searchID");
	if(pBtn) pBtn->Show(false);

	return CDlgWebTradeBase::OnInitDialog();
}

void CDlgWebViewProduct::InitCategory()
{
	m_pTv_Item->DeleteAllItems();
	
	AFileImage fList;
	if( !fList.Open("surfaces\\ingame\\webtradetree.txt",AFILE_OPENEXIST | AFILE_TYPE_BINARY | AFILE_TEMPMEMORY) )
	{
		AUI_ReportError(__LINE__, 1,"CDlgWebViewProduct::InitCatalog(), web trade tree file error!");
	}
	else
	{
		// copy from auction tree parsing

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
		int iCategory;
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
			pItem = m_pTv_Item->InsertItem(vec[0], pItem);
			m_pTv_Item->Expand(pItem, AUITREEVIEW_EXPAND_COLLAPSE);
			if( vec.size() == 2 )
			{
				iCategory = a_atoi(vec[1]);
				m_pTv_Item->SetItemData(pItem, iCategory);
			}
			else
			{
				m_pTv_Item->SetItemData(pItem, 0);
			}
		} while(true);
		fList.Close();
		a_freetemp(szData);
	}
}

void CDlgWebViewProduct::OnShowDialog()
{
	CDlgWebTradeBase::OnShowDialog();

	ShowCategory(ALLITEM_CATEGORY_ID);
}

void CDlgWebViewProduct::OnCommandGotoMyShop(const char* szCommand)
{
	GetGameUIMan()->m_pDlgWebViewProduct->Show(false);
	GetGameUIMan()->m_pDlgWebMyShop->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgWebMyShop->Show(true);
}

__int64 CDlgWebViewProduct::GetInputTradeID()
{
	ACString id = m_pTxt_TradeID->GetText();
	id.TrimLeft();
	id.TrimRight();
	if(id.GetLength() > 20 || id.GetLength() < 1)
	{
		return 0;
	}

	__int64 ret = 0;
	for(int i=0;i<id.GetLength();i++)
	{
		ACHAR pos = id[i];
		
		if(pos > _AL('9') || pos < _AL('0'))
		{
			return 0;
		}

		ret = ret * 10 + (pos - _AL('0'));
	}

	return ret;
}
void CDlgWebViewProduct::OnCommandSearchByID(const char* szCommand)
{
	__int64 nID = GetInputTradeID();
	if(nID > 0)
	{
		GetGameSession()->webTrade_GetItemDetail(nID);
	}
	else
	{
		WEB_VIEWPRODUCT_MSGBOX(GetStringFromTable(5515));
	}
}

void CDlgWebViewProduct::OnCommandSearchItemforme(const char* szCommand)
{
	ShowCategory(ONLYFORME_CATEGORY_ID);
}

void CDlgWebViewProduct::OnCommandCancel(const char* szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

void CDlgWebViewProduct::OnEventLButtonDown_Tree_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pLst_Item->SetCurSel(m_pLst_Item->GetCount());
}

void CDlgWebViewProduct::OnEventLButtonDBLCLK_Tree_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int x, y;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	x = GET_X_LPARAM(lParam) - p->X;
	y = GET_Y_LPARAM(lParam) - p->Y;

	bool bOnIcon;
	POINT ptObj = m_pTv_Item->GetPos();
	P_AUITREEVIEW_ITEM pItem = m_pTv_Item->HitTest(x - ptObj.x, y - ptObj.y, &bOnIcon);

	if( pItem && !m_pTv_Item->GetFirstChildItem(pItem))
	{
		int iCategory = m_pTv_Item->GetItemData(pItem);
		ShowCategory(iCategory);

		if (CECUIConfig::Instance().GetGameUI().bEnableWebTradeSort)
		{
			m_strCurGroup.clear();

			P_AUITREEVIEW_ITEM pRoot = m_pTv_Item->GetRootItem();
			P_AUITREEVIEW_ITEM pTemp = pItem;
			while (pTemp != pRoot)
			{
				P_AUITREEVIEW_ITEM pParent = m_pTv_Item->GetParentItem(pTemp);
				if (pParent == pRoot ||
					m_pTv_Item->GetNextSiblingItem(m_pTv_Item->GetFirstChildItem(pParent)))
				{
					//	忽略单个子节点的名称，以处理“角色”下单个“角色”、“游戏币”下单个“金币”节点
					m_strCurGroup.insert(m_strCurGroup.begin(), m_pTv_Item->GetItemText(pTemp));
				}
				pTemp = pParent;
			}
		}
	}
}

void CDlgWebViewProduct::ShowCategory(int iCategory)
{
	if(m_Category != iCategory)
	{
		// clear before show the list
		UpdateItemList(NULL, 0, true);
	}

	m_Category = iCategory;
	OnPageSwitch(PAGE_HOME_FLAG);
}

void CDlgWebViewProduct::WebTradeAction(int idAction, void *pData)
{
	CDlgWebTradeBase::WebTradeAction(idAction, pData);

	if( idAction == PROTOCOL_WEBTRADELIST_RE) {

		if(m_Category != ONLYFORME_CATEGORY_ID)
		{
			WebTradeList_Re *pList = (WebTradeList_Re *)pData;
			
			if(GetPageMode() == PAGE_UP_FLAG && pList->items.size() < WEBTRADE_PAGE_SIZE)
			{
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);

				// NOTICE:
				// the reverse search may get a partial result if we do search on homepage.
				// we have to refresh the homepage to get a full result.
				OnPageSwitch(PAGE_HOME_FLAG);

			}
			else
			{
				if(pList->items.size() > 0)
				{
					if(GetPageMode() == PAGE_HOME_FLAG)
					{
						m_PageBegin = 0;
						m_PageEnd = pList->end;
					}
					else if(GetPageMode() == PAGE_END_FLAG)
					{
						m_PageBegin = pList->end + 1;
						m_PageEnd = -1;
					}
					else if(GetPageMode() == PAGE_DOWN_FLAG)
					{
						m_PageBegin = m_PageEnd;
						m_PageEnd = pList->end;
					}
					else if(GetPageMode() == PAGE_UP_FLAG)
					{
						m_PageEnd = m_PageBegin;
						m_PageBegin = pList->end + 1;
					}

					m_Category = pList->category;
					UpdateItemList(pList->items.empty() ? NULL : &pList->items[0], pList->items.size(), true);
				}

				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}

	} else if( idAction == PROTOCOL_WEBTRADEATTENDLIST_RE) {

		if(m_Category == ONLYFORME_CATEGORY_ID)
		{
			WebTradeAttendList_Re *pList = (WebTradeAttendList_Re *)pData;
			if(!pList->getsell)
			{
				if(pList->items.size() > 0)
				{
					if(GetPageMode() == PAGE_HOME_FLAG)
					{
						m_PageBegin = 0;
					}
					else if(GetPageMode() == PAGE_END_FLAG)
					{
						m_PageBegin = max(0, pList->end - WEBTRADE_PAGE_SIZE);
					}
					if(GetPageMode() == PAGE_DOWN_FLAG)
					{
						m_PageBegin += WEBTRADE_PAGE_SIZE;
					}
					else if(GetPageMode() == PAGE_UP_FLAG)
					{
						m_PageBegin = max(0, m_PageBegin - WEBTRADE_PAGE_SIZE);
					}
					
					m_PageEnd = (GetPageMode() == PAGE_END_FLAG || pList->items.size() < WEBTRADE_PAGE_SIZE) ? 
								 -1:m_PageBegin + WEBTRADE_PAGE_SIZE;

					UpdateItemList(pList->items.empty() ? NULL : &pList->items[0], pList->items.size(), true);
				}
				
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}	

	} else if( idAction == PROTOCOL_WEBTRADEGETDETAIL_RE) {

		WebTradeGetDetail_Re* pDetail = (WebTradeGetDetail_Re*)pData;
		if(!pDetail->retcode)
		{
			UpdateItemList(&pDetail->detail.info, 1, true);
		}

	} else if( idAction == PROTOCOL_WEBTRADEUPDATE_RE) {
		WebTradeUpdate_Re* pInfo = (WebTradeUpdate_Re*)pData;
		
		if(!pInfo->retcode)
		{
			// update item only
			UpdateItemList(&pInfo->item, 1, false);
		}
		else
		{
			DeleteTradeInfo(pInfo->sn);
		}
	}
}

void CDlgWebViewProduct::OnPageSwitch(int mode)
{
	if(GetPageMode() != PAGE_INVALID)
	{
		return;
	}

	CDlgWebTradeBase::OnPageSwitch(mode);

	//GetGameUIMan()->AddChatMessage(GetStringFromTable(5514), GP_CHAT_MISC);

	if(m_Category == ONLYFORME_CATEGORY_ID) // in this case we do not need this parameter
	{
		if(PAGE_HOME_FLAG == mode)
		{
			GetGameSession()->webTrade_AttendList(0, false);
		}
		else if(PAGE_END_FLAG == mode)
		{
			GetGameSession()->webTrade_AttendList(-1, false);
		}
		else if(PAGE_UP_FLAG == mode)
		{
			if(m_PageBegin > 0)
			{
				GetGameSession()->webTrade_AttendList(max(0, m_PageBegin - WEBTRADE_PAGE_SIZE), false);
			}
			else
			{
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}
		else if(PAGE_DOWN_FLAG == mode)
		{
			if(m_PageEnd != -1)
			{
				GetGameSession()->webTrade_AttendList(m_PageBegin + WEBTRADE_PAGE_SIZE, false);
			}
			else
			{
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}
		else if(PAGE_REFRESH_FLAG == mode)
		{
			GetGameSession()->webTrade_AttendList(m_PageBegin, false);
		}
	}
	else
	{
		if(PAGE_HOME_FLAG == mode)
		{
			GetGameSession()->webTrade_List(m_Category, 0, false);
		}
		else if(PAGE_END_FLAG == mode)
		{
			GetGameSession()->webTrade_List(m_Category, -1, true);
		}
		else if(PAGE_UP_FLAG == mode)
		{
			if(m_PageBegin > 0)
			{
				GetGameSession()->webTrade_List(m_Category, max(0, m_PageBegin-1), true);
			}
			else
			{
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}
		else if(PAGE_DOWN_FLAG == mode)
		{
			if(m_PageEnd != -1)
			{
				GetGameSession()->webTrade_List(m_Category, m_PageEnd, false);
			}
			else
			{
				CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
			}
		}
		else if(PAGE_REFRESH_FLAG == mode)
		{
			GetGameSession()->webTrade_List(m_Category, m_PageBegin, false);
		}
	}
}

ACString CDlgWebViewProduct::GetRowString(const CDlgWebTradeBase::ItemInfo& tradeItem)
{
	ACString szText;
	
	szText.Format( _AL("%s\t%s\t%s\t%s"),
		tradeItem.GetItemName(),
		tradeItem.GetTime(),
		tradeItem.GetFormatedPrice(),
		tradeItem.GetSellerName());
	
	return szText;
}

void CDlgWebViewProduct::InsertTradeInfo(int i, ItemInfo* pInfo)
{
	CDlgWebTradeBase::InsertTradeInfo(i, pInfo);
	
	m_pLst_Item->SetItemData(i, pInfo->GetTimeValue(),1);
	m_pLst_Item->SetItemData(i, pInfo->GetBasicInfo().price,2);
}

void CDlgWebViewProduct::EnableGetTicket(bool bEnable)
{
	const char *szButtons[] = {"Lab_Title", "Lab_Remainder", "Lab_Price", "Lab_Sellname"};
	for (int i(0); i < ARRAY_SIZE(szButtons); ++ i)
	{
		PAUIOBJECT pObj = GetDlgItem(szButtons[i]);
		if (pObj) pObj->Enable(bEnable);
	}
}

void CDlgWebViewProduct::OnCommandSortByCommand(const char* szCommand)
{
	// command format must be "sort_<type>_<id>"

	if (!CECUIConfig::Instance().GetGameUI().bEnableWebTradeSort)
	{
		CDlgWebTradeBase::OnCommandSortByCommand(szCommand);
		return;
	}

	//	记录访问参数
	if (strlen(szCommand) < 10) return;
	m_nColumnClick = atoi(szCommand + 9);	//	skip sort_str_ etc.
	m_strGroupClick = m_strCurGroup;

	//	获取 ticket 用于访问寻宝网站
	GetGameSession()->sso_GetTicket(GetWebTradeTicketContext());

	//	等待返回
	EnableGetTicket(false);
}

void CDlgWebViewProduct::OnCommandSortDataByCommand(const char* szCommand)
{
	// command format must be "sortdata_<type>_<id>"

	if (!CECUIConfig::Instance().GetGameUI().bEnableWebTradeSort)
	{
		CDlgWebTradeBase::OnCommandSortDataByCommand(szCommand);
		return;
	}
	
	//	记录访问参数
	if (strlen(szCommand) < 14) return;
	m_nColumnClick = atoi(szCommand + 13);	//	skip sortdata_int_ etc.
	m_strGroupClick = m_strCurGroup;
	
	//	获取 ticket 用于访问寻宝网站
	GetGameSession()->sso_GetTicket(GetWebTradeTicketContext());
	
	//	等待返回
	EnableGetTicket(false);
}

void CDlgWebViewProduct::HandleRequest(const CECSSOTicketHandler::Request *p)
{
	bool bProcessed(false);
	
	while (p)
	{
		if (p->local_context != GetWebTradeTicketContext())
			break;

		EnableGetTicket(true);
		
		if (p->retcode != 0)
			break;	//	retcode 其它情况传递给 CECGameUIMan 统一处理
		
		bProcessed = true;

		//	判断合法性
		const char *szSortType[][2] = {
			{"itemname-asc",	"itemname-desc"},
			{"uptime-asc",		"uptime-desc"},
			{"unitprice-asc",	"unitprice-desc"},
			{"sellername-asc",	"sellername-desc"},
		};
		if (m_nColumnClick < 0 || m_nColumnClick >= ARRAY_SIZE(szSortType))
		{
			a_LogOutput(1, "CDlgWebViewProduct::OnSSOGetTicket_Re, invalid column = %d", m_nColumnClick);
			break;
		}

		if (m_strGroupClick.size() > 2)
		{
			a_LogOutput(1, "CDlgWebViewProduct::OnSSOGetTicket_Re, invalid group, size = %d", m_strGroupClick.size());
			break;
		}

		//	构造访问链接
		AString strURLFormat = GetBaseUIMan()->GetURL("WEB", "WEBTRADE");
		AString strKey;
		strKey.Format("WEBTRADE_LOCATION%d", m_strGroupClick.size()+1);
		AString strLocationFormat = GetBaseUIMan()->GetURL("WEB", strKey);
		if (strURLFormat.IsEmpty() || strLocationFormat.IsEmpty())
		{
			a_LogOutput(1, "CDlgWebViewProduct::HandleRequest, empty url format");
			break;
		}

		AString strTicket((const char *)p->ticket.begin(), p->ticket.size());
		if (strTicket.IsEmpty())
		{
			a_LogOutput(1, "CDlgWebViewProduct::HandleRequest, ticket is empty.");
			break;
		}

		//	构造Loation参数
		ACString strLocation;
		if (m_strGroupClick.empty())
			strLocation.Format(AS2AC(strLocationFormat), AS2AC(szSortType[m_nColumnClick][0]));
		else if (m_strGroupClick.size() == 1)
			strLocation.Format(AS2AC(strLocationFormat), m_strGroupClick[0], AS2AC(szSortType[m_nColumnClick][0]));
		else
			strLocation.Format(AS2AC(strLocationFormat), m_strGroupClick[0], m_strGroupClick[1], AS2AC(szSortType[m_nColumnClick][0]));
		AString strLocationUTF8 = glb_ConverToUTF8(strLocation);
		AString strLocationUTF8Hex = glb_ConverToHex(strLocationUTF8);

		//	记录用于调试
		glb_LogURL(AC2AS(strLocation));
		
		//	构造完整链接
		AString strURL;
		strURL.Format(strURLFormat, strLocationUTF8Hex, strTicket, GetGameSession()->GetTicketAccount());
		
		//	使用外部链接打开
		GetBaseUIMan()->NavigateURL(strURL, NULL);
		break;
	}

	if (bProcessed){
		m_nColumnClick = -1;
		m_strGroupClick.clear();
	}else{		
		CECSSOTicketHandler::HandleRequest(p);
	}
}
