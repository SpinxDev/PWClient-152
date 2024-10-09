// File		: DlgWebViewProduct.h
// Creator	: Feng Ning
// Date		: 2010/3/18

#pragma once

#include "DlgWebTradeBase.h"
#include "EC_Handler.h"

#include "AUIEditBox.h"
#include "AUITreeView.h"

namespace GNET{
	class Octets;
	class SSOGetTicket_Re;
}

class CDlgWebViewProduct : public CDlgWebTradeBase, public CECSSOTicketHandler
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
		
public:
	CDlgWebViewProduct();
	virtual ~CDlgWebViewProduct();

	void OnCommandGotoMyShop(const char* szCommand);
	void OnCommandSearchByID(const char* szCommand);
	void OnCommandSearchItemforme(const char* szCommand);
	void OnCommandCancel(const char* szCommand);
	
	void OnCommandSortByCommand(const char* szCommand);
	void OnCommandSortDataByCommand(const char* szCommand);

	void OnEventLButtonDown_Tree_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDBLCLK_Tree_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	virtual void WebTradeAction(int idAction, void *pData);

	virtual void HandleRequest(const CECSSOTicketHandler::Request *p);

protected:
	virtual void OnPageSwitch(int mode);
	virtual ACString GetRowString(const CDlgWebTradeBase::ItemInfo& tradeItem);
	virtual void InsertTradeInfo(int i, ItemInfo* pInfo);
	__int64 GetInputTradeID();

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void InitCategory();
	void ShowCategory(int iCategory);

	void EnableGetTicket(bool bEnable);

private:
	PAUIOBJECT				m_pTxt_TradeID;
	PAUITREEVIEW			m_pTv_Item;
	int						m_Category;
	int						m_PageBegin, m_PageEnd;
	int						m_nColumnClick;		//	记录点击的排序列，用于 SSO ticket 返回时执行动作
	abase::vector<ACString> m_strCurGroup;		//	记录当前物品分类（从左至右为从父亲结点至子结点）
	abase::vector<ACString> m_strGroupClick;	//	记录点击排序列时物品分类（同上）
};