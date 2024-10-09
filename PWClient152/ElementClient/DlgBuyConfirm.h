// Filename	: DlgBuyConfirm.h
// Creator	: Xu Wenbin
// Date		: 2012/8/29

#pragma once

#include "DlgBase.h"

//	class CECBuyCommandObserver
class CECBuyCommand;
class CECBuyCommandObserver
{
public:
	virtual ~CECBuyCommandObserver(){}
	virtual void OnShow(const CECBuyCommand *p)=0;
	virtual void OnConfirm(const CECBuyCommand *p)=0;
	virtual void OnCancel(const CECBuyCommand *p)=0;
	typedef void (CECBuyCommandObserver::*Action)(const CECBuyCommand *p);
};

//	class CECBuyCommand
class CECBuyCommand
{	
	struct Observer
	{
		CECBuyCommandObserver*	m_p;
		bool					m_once;	//	是否只通知一次物品购买交互就自动删除
		
		Observer(CECBuyCommandObserver *p, bool once) : m_p(p), m_once(once) {}
		bool operator == (CECBuyCommandObserver *p)const{
			return m_p == p;
		}
		bool operator == (const Observer &rhs)const{
			return m_p == rhs.m_p;
		}
	};
	typedef abase::vector<Observer>	Observers;
	Observers	m_observers;

	void NotifyObservers(CECBuyCommandObserver::Action a, bool bClearNotifyOnceObserver);

protected:	
	virtual void OnShow()=0;
	virtual void OnConfirm()=0;
	virtual void OnCancel()=0;

public:
	virtual ~CECBuyCommand(){}
	
	void RegisterObserver(CECBuyCommandObserver *pObserver, bool once=true);
	void UnregisterObserver(CECBuyCommandObserver *pObserver);

	virtual ACString GetMessage()const=0;
	void Show(){
		OnShow();
		NotifyObservers(&CECBuyCommandObserver::OnShow, false);
	}
	void Confirm(){
		OnConfirm();
		NotifyObservers(&CECBuyCommandObserver::OnConfirm, true);
	}
	void Cancel(){
		NotifyObservers(&CECBuyCommandObserver::OnCancel, true);
		OnCancel();
	}
};

//	class CECShopBuyCommand
class CECShopBase;
class CECShopBuyCommand : public CECBuyCommand
{
	CECShopBase	* m_pShop;
	int			m_itemIndex;
	int			m_buyIndex;
	
protected:	
	virtual void OnShow();
	virtual void OnConfirm();
	virtual void OnCancel();

public:
	CECShopBuyCommand(CECShopBase *pShop, int itemIndex, int buyIndex);
	virtual ACString GetMessage()const;

	int		GetID()const;
	int		GetItemIndex()const;
	int		GetBuyIndex()const;
};
//	class CECShopBuyCommandObserver
class CECShopBuyCommandObserver : public CECBuyCommandObserver
{
public:
	virtual void OnShow(const CECBuyCommand *p);
	virtual void OnConfirm(const CECBuyCommand *p);
	virtual void OnCancel(const CECBuyCommand *p);
protected:	
	virtual void OnBuyConfirmShow(int id, int itemIndex, int buyIndex)=0;
	virtual void OnBuyConfirmed(int id, int itemIndex, int buyIndex)=0;
	virtual void OnBuyCancelled(int id, int itemIndex, int buyIndex)=0;
};

//	class CECRandShopBuyCommand
class RandMallShopping;
class CECRandShopBuyCommand : public CECBuyCommand
{
	RandMallShopping	* m_pShopping;
protected:	
	virtual void OnShow();
	virtual void OnConfirm();
	virtual void OnCancel();
	
public:
	CECRandShopBuyCommand(RandMallShopping *p);
	virtual ACString GetMessage()const;
};

class AUITextArea;
class CDlgBuyConfirm : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgBuyConfirm();
	virtual ~CDlgBuyConfirm(){}
	
	void OnCommandConfirm(const char * szCommand);
	void OnCommandProtocol(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void Buy(CECShopBase *pShop, int itemIndex, int buyIndex, bool needConfirm=true, CECShopBuyCommandObserver *pObserver=NULL, bool once=true);
	void Buy(CECBuyCommand *pCmd, bool needConfirm=true, CECBuyCommandObserver *pObserver=NULL, bool once=true);
	
protected:	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();	
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual void OnChangeLayoutEnd(bool bAllDone);

	int  GetContentMaxHeight();
	int	 GetContentRequiredHeight();
	int	 GetCotentCurrentHeight();
	int	 CalculateContentHeight();
	bool GetShowContentScroll();
	void AdjustContent(int deltaHeight);
	void AdjustButtons(int deltaHeight);
	void AdjustDialog(int deltaHeight);
	void ResizeByContent();
	void Clear();
	
	AUITextArea*	m_pTxt_Message;
	void SetMessage(const ACHAR *szText);

	PAUIOBJECT	m_pBtn_Confirm;
	PAUIOBJECT	m_pBtn_Cancel;
	PAUIOBJECT	m_pBtn_Protocol;
	AString		m_strProtocol;

	CECBuyCommand	*	m_pBuyCmd;

	int			m_messageDefaultHeight;			//	m_pTxt_Message 的默认高度
};
