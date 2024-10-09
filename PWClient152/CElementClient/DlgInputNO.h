// Filename	: DlgInputNO.h
// Creator	: Tom Zhou
// Date		: October 10, 2005

#pragma once

#include "DlgBase.h"
#include <AUIEditBox.h>

class CDlgInputNO : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInputNO();
	virtual ~CDlgInputNO();

	void OnCommand_max(const char * szCommand);
	void OnCommand_add(const char * szCommand);
	void OnCommand_minus(const char * szCommand);
	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void SetType(int nType);
	void SetInfo(int nCur, int nMax);

	int   GetType(){ return m_nInputNO; }
	void GetInfo(int &nCur, int &nMax);

	enum
	{
		INPUTNO_NULL = 0,
		INPUTNO_BUY_ADD,
		INPUTNO_BUY_REMOVE,
		INPUTNO_SELL_ADD,
		INPUTNO_SELL_REMOVE,
		INPUTNO_DROP_ITEM,
		INPUTNO_DROP_MONEY,
		INPUTNO_TRADE_MONEY,
		INPUTNO_TRADE_MOVE,
		INPUTNO_TRADE_ADD,
		INPUTNO_TRADE_REMOVE,
		INPUTNO_MOVE_ITEM,
		INPUTNO_STORAGE_TRASH_MONEY,
		INPUTNO_STORAGE_IVTR_MONEY,
		INPUTNO_STORAGE_GET_ITEMS,
		INPUTNO_STORAGE_PUT_ITEMS,
		INPUTNO_STORAGE_MOVE_ITEMS,
		INPUTNO_MOVE_MAILATTACH,
		INPUTNO_ATTACH_MONEY,
		INPUTNO_FACTION_MATERIAL,
		INPUTNO_GM_REMOVE_ITEM,
		INPUTNO_CLICK_SHORTCUT,
		INPUTNO_OFFLINESHOP_SELL,
		INPUTNO_OFFLINESHOP_BUY,
		INPUTNO_OFFLINESHOP_SHOPITEM_SELL,
		INPUTNO_OFFLINESHOP_SHOPITEM_BUY,
		INPUTNO_OFFLINESHOP_SEARCHITEM_SELL,
		INPUTNO_OFFLINESHOP_SEARCHITEM_BUY,
		INPUTNO_TOUCHSHOP_ITEM_NUM,
	};

protected:
	int m_nInputNO;

	virtual bool OnInitDialog();
};

//////////////////////////////////////////////////////////////////////////


class CDlgInputNO3 : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgInputNO3();
	virtual ~CDlgInputNO3();
	
	static void ComputeValue(PAUIEDITBOX pText,__int64& ret);
	
	void OnCommand_max(const char * szCommand);
	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnEventChar_Txt_M(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventChar_Txt_Y(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void SetType(int nType);
	int  GetType(){ return m_nInputNO; }
	
	
	void SetMoney(int v);
	__int64 GetMoney() const { return m_iInputMoney;}
	void SetYinPiao(int v);
	int GetYinPiao() const { return m_iInputYinPiao;}

	void SetMaxValue(int money,int yinpiao);
	
	enum
	{
		INPUTNO_NULL = 0,
		INPUTNO_OFFLINESHOP_GETMONEY,
		INPUTNO_OFFLINESHOP_SAVEMONEY,		
	};
	
protected:
	int m_nInputNO;
	
	__int64 m_iInputMoney;
	int m_iInputYinPiao;
	
	PAUIEDITBOX m_pTextMoney;
	PAUIEDITBOX m_pTextYinPiao;
	
	virtual bool OnInitDialog();
	virtual void OnHideDialog();
	virtual void OnShowDialog();	
};