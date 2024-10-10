/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   14:53
	file name:	DlgGMConsole.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "DlgBase.h"
#include <AAssist.h>
#include "SDBDateTime.h"

class CGMCommand;
class AUIListBox;
class AUIEditBox;
class GMCDBMan;
class CGMCommandListBox;

class CDlgGMConsole : public CDlgBase  
{
	friend class CDlgGMDelMsgReason;
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
private:
	AUIListBox * m_pLbxPlayerMsg;
	AUIListBox * m_pLbxCommand;
	PAUIOBJECT m_pEbxPlayerID;
	AUIListBox * m_pLbxPlayerNearby;
	bool m_bInited;
	GMCDBMan * m_pGMCDBMan;
	CGMCommandListBox * m_pGMCommandListBox;
	int m_nIDForMsgListBoxItem;
private:
	ACString RetrivePlayerNameFromMsg(ACString strMsg);
	int RetrivePlayerIDFromName(ACString strPlayerName);
	void SetEditboxPlayerID(int nPlayerID);
	void ReFillListboxPlayerNearby();
	bool InitGMCDBMan();
	
	void FillUserMsgListboxFromDB();
	int GetGMID();
	void GetParamFromPlayerMsgListItem(int nIndex, int & nPlayerID, SimpleDB::DateTime *& pReceivedTime);
	void AddItemToListboxPlayerMsg(int nPlayerID, ACHAR type, ACString msg, SimpleDB::DateTime *pReceivedTime);
	void DeleteItemFromListboxPlayerMsg(int nIndex);
	void ConfiremCommandIgnore(int nSelReasonIndex, ACString strCustomReason);
	void LocalInit();
	bool CheckIsMsgItemAssocedWithAGMFinishMsgDlg(int nIndex);
protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
public:
	CDlgGMConsole();
	virtual ~CDlgGMConsole();

	void OnCommandClose(const char * szCommand);
	void OnCommandFinish(const char * szCommand);
	void OnCommandIgnore(const char * szCommand);
	void OnCommandDblClickCommand(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnLButtonUpListboxPlayerNearby(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommandNewGMUI(const char* szCommand);

	virtual bool Release();
	bool AddUserMessage(ACString strMsg, ACHAR type, int nPlayerID);
	GMCDBMan * GetGMCDBMan();
	void ConfirmCommandFinish(DWORD dwItemIDInMsgListBox, bool bFinished);

	bool SaveGMCDBMan(bool bRelease);
};
