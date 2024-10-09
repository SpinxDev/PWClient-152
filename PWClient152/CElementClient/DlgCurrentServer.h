// Filename	: DlgCurrentServer.h
// Creator	: XuWenbin
// Date		: 2013/9/7

#pragma once

class AUIDialog;
class AUIObject;
class CDlgCurrentServer
{
public:
	CDlgCurrentServer(AUIDialog *pDialog);	
	void OnTick();

private:
	AUIDialog *		m_pParent;
	AUIObject *		m_pTxt_ServerName;
	AUIObject *		m_pTxt_Ping;
};