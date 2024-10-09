/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   15:31
	file base:	DlgAskHelpToGM
	file ext:	h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
class AUIComboBox;

class CDlgAskHelpToGM : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	AUIComboBox * m_pCbxType;
	PAUIEDITBOX m_pEbxMemo;
	
	DWORD m_dwSelectedType;
	ACString m_strPlugin;

	int				m_idPluginUser;
	ACString	m_strPluginUserName;
protected:
	void DoDataExchange(bool bSave);
	bool OnInitDialog();

	virtual void OnShowDialog();
	
	bool GetMsgToSend(ACString &strMsg);
	bool IsReportWithFeedback();
public:
	void OnCommandConfirm(const char * szCommand);
	void OnCommandType(const char * szCommand);

	void OnCommandSpeak(const char * szCommand);
	
	enum {
		GM_REPORT_STUCK	=	0x01,	//	被卡住了
		GM_REPORT_PLUGIN=	0x02,	//	举报外挂
		GM_REPORT_TALK		=	0x04,	//	给GM留言
	};

	void ShowWithHelpMessage(unsigned int mask, const ACHAR * szPluginMsg = NULL, int idPluginUser=0, const ACHAR *szPluginUserName=NULL);

	CDlgAskHelpToGM();
	virtual ~CDlgAskHelpToGM();
};

