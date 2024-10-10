/********************************************************************
	created:	2012/11/12
	created:	12:11:2012   16:31
	file base:	DlgAskHelpToGM2
	file ext:	h
	author:		zhougaomin01305
	
	purpose:	举报发言
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"

class AUIEditBox;
class AUIComboBox;

class CDlgAskHelpToGM2 : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
protected:
	
	int				m_idPluginUser;
	ACString	m_strPluginUserName;
	abase::vector<ACString> m_indexForOrigions;
	int m_index;
protected:
	void DoDataExchange(bool bSave);
	bool OnInitDialog();

	virtual void OnShowDialog();
public:
	void OnCommandConfirm(const char * szCommand);

	void OnCommandGACD(const char * szCommand);
	
	void OnEventLButtonDown_ListBox_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	enum {
		GM_REPORT_SPEAK		=	0x08,	//	举报发言
	};

	ACString AUI_FilterEditboxItem(const ACHAR *szText, const CECGameUIMan::AUI_EditboxItemFilter& filter,CECIvtrItem* pChatItem);

	void ShowWithHelpMessage(int idPluginUser=0, const ACHAR *szPluginUserName=NULL);

	CDlgAskHelpToGM2();
	virtual ~CDlgAskHelpToGM2();
};

