/********************************************************************
	created:	2005/10/21
	created:	21:10:2005   17:26
	file name:	DlgGMFinishMsg.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "DlgBase.h"
#include "SDBDateTime.h"

struct GMCDatabase;
class AUIListBox;
class AUILabel;
class CGMCommandListBox;

class CDlgGMFinishMsg : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
private:
	AUIListBox * m_pLbxCommand;
	AUILabel * m_pLblMsg;
	CGMCommandListBox * m_pGMCommandListBox;
	DWORD m_dwItemIDInMsgListBox;
	bool m_bDataReady;
protected:
	virtual bool Release();
	virtual void DoDataExchange(bool bSave);
public:
	CDlgGMFinishMsg();
	~CDlgGMFinishMsg();
	void SetAssocData(GMCDatabase *pDatabase, ACString strMsg, int nPlayerID, SimpleDB::DateTime * pdtReceivedMsg, DWORD dwItemIDInMsgListBox);	
	
	void OnCommandOk(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	void OnCommandDblClickCommand(const char * szCommand);
	void OnCommandIDCancel(const char * szCommand);
};