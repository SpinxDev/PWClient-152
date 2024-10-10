// File		: DlgLogin.h
// Creator	: Feng Ning
// Date		: 2010/10/20

#pragma once

#include "DlgBase.h"

class AUIEditBox;
class AUIListBox;
class AUICheckBox;
class CECLoginUIMan;
class CDlgCurrentServer;
class CDlgLogin : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgLogin();
	virtual ~CDlgLogin();

	void OnEventKeyDown_TxtAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventKeyDown_LstAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp_LstAccount(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_ChangeServer(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommand_SoftKey(const char* szCommand);
	void OnCommand_Switch(const char* szCommand);
	void OnCommand_LstAccount(const char *szCommand);
	void OnCommand_ChkRecord(const char *szCommand);

	void OnCommand_Cancel(const char* szCommand);
	void OnCommand_Confirm(const char* szCommand);
	
	void OnCommand_Sina(const char *szCommand);
	void OnCommand_QQ(const char *szCommand);
	void OnCommand_360(const char *szCommand);
	void OnCommand_Taobao(const char *szCommand);
	void OnCommand_Renren(const char *szCommand);

	bool GetKickoutUser();
	void SetKickoutUser(bool bKick);

	ACString GetUser();
	void SetUser(const ACString &user);
	void FocusUser();

	ACString GetPassword();
	void SetPassword(const ACString &pwd);
	void FocusPassword();

	void ResetUserPassword(bool bClearUserPwd);
	void SetAlwaysTryAutoLogin(bool bAuto) { m_bAlwaysTryAutoLogin = bAuto; }

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Release();
	
	void WebLogin(const AString &strAddress);
	void UpdateAccountList();
	void ResetAccountList();

	AString	m_strSinaAddress;
	AString	m_strQQAddress;
	AString	m_str360Address;
	AString	m_strTaobaoAddress;
	AString	m_strRenrenAddress;

	AUIEditBox *	m_pTxt_Account;
	AUIEditBox *	m_pTxt_Password;
	AUIListBox *	m_pLst_Account;
	AUICheckBox *	m_pChk_Record;

	ACString		m_strAccountLastInput;

	CDlgCurrentServer *	m_pDlgCurrentServer;
	bool			m_bAlwaysTryAutoLogin;
};
