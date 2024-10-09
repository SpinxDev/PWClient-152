// Filename	: DlgWebList.h
// Creator	: Xu Wenbin
// Date		: 2012/05/25

#pragma once

#include "DlgBase.h"
#include "EC_Handler.h"
#include "EC_URLOSNavigator.h"

namespace GNET{
	class Octets;
}

class CDlgWebListSSOURLNavigator{
	AUIObject * m_pClickButton;
public:
	CDlgWebListSSOURLNavigator(AUIObject *pClickButton);
	virtual ~CDlgWebListSSOURLNavigator(){}
	virtual void EnableClickButton(bool bEnable);
	virtual bool IsClickButtonEnabled()const;
	void ClickNavigateButton(bool bOSNavigate);
	bool ShouldNavigateInOSForTicketContext(const GNET::Octets &context)const;
	bool IsMySSOTicketContext(const GNET::Octets &context)const;
	GNET::Octets GetSSOTicketContext(bool bForOSNavigate)const;
	AString GetNavigateURL(const GNET::Octets &ticket)const;

	virtual bool CanNavigateInGame()const=0;
	virtual GNET::Octets GetSSOTicketBaseContext()const=0;
	virtual GNET::Octets GetSSOTicketInfo()const=0;
	virtual AString GetNavigateURLFormat()const=0;
};

class CDlgWebListSSOPayNavigator : public CDlgWebListSSOURLNavigator{
public:
	CDlgWebListSSOPayNavigator(AUIObject *pClickButton);
	
	virtual bool CanNavigateInGame()const{ return false; }
	virtual GNET::Octets GetSSOTicketBaseContext()const;
	virtual GNET::Octets GetSSOTicketInfo()const;
	virtual AString GetNavigateURLFormat()const;
};

class CDlgWebListSSOActivityNavigator : public CDlgWebListSSOURLNavigator{
public:
	CDlgWebListSSOActivityNavigator(AUIObject *pClickButton);
	virtual bool CanNavigateInGame()const{ return true; }
	virtual GNET::Octets GetSSOTicketBaseContext()const;
	virtual GNET::Octets GetSSOTicketInfo()const;
	virtual AString GetNavigateURLFormat()const;
};

class CDlgWebListSSOActivity2Navigator : public CDlgWebListSSOURLNavigator{
public:
	CDlgWebListSSOActivity2Navigator(AUIObject *pClickButton);
	virtual bool CanNavigateInGame()const{ return false; }
	virtual GNET::Octets GetSSOTicketBaseContext()const;
	virtual GNET::Octets GetSSOTicketInfo()const;
	virtual AString GetNavigateURLFormat()const;
};

class CDlgWebList : public CDlgBase, public CECSSOTicketHandler, public CECURLOSNavigator
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWebList();
	
	void OnCommand_Btn_Radio(const char *szCommand);
	void OnCommand_Btn_Pay(const char *szCommand);
	void OnCommand_Btn_Activity(const char *szCommand);
	void OnCommand_Btn_Activity2(const char *szCommand);

	//	override CECSSOTicketHandler
	virtual void HandleRequest(const CECSSOTicketHandler::Request *p);

	//	override CECURLOSNavigator
	virtual bool CanNavigateNow()const;
	virtual void OnClickNavigate();
	virtual bool AutoRelease()const;

protected:
	virtual bool OnInitDialog();
	virtual bool Release();

	enum {
		SSO_NAVIGATE_PAY,
		SSO_NAVIGATE_ACTIVITY,
		SSO_NAVIGATE_ACTIVITY2,
		SSO_NAVIGATE_NUM,
	};
	CDlgWebListSSOURLNavigator * m_pSSOURLNavigator[SSO_NAVIGATE_NUM];
	CDlgWebListSSOURLNavigator * m_pLastSSOURLNavigator;
};