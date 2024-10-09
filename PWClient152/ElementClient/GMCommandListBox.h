/********************************************************************
	created:	2005/10/21
	created:	21:10:2005   17:26
	file name:	GMCommandListBox.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "AUIListBox.h"
#include "SDBDateTime.h"

struct GMCDatabase;
class CGMCommand;
class AUIListBox;
class CECGameUIMan;
class AUIDialog;

class CGMCommandListBox
{
private:
	AUIListBox * m_pAssocListBox;
	GMCDatabase * m_pDatabase;
	AUIDialog * m_pParentDlg;
	int m_nPlayerID;
	const SimpleDB::DateTime *m_pdtReceivedMsg;
private:
	void FillContent();
	CECGameUIMan* GetGameUIMan() const;
public:
	CGMCommandListBox();
	bool Init(AUIListBox *pAssocListBox, AUIDialog *pParentDlg);
	void Release();
	void OnDblClick();
	void SetAssocData(GMCDatabase *pDatabase, int nPlayerID, const SimpleDB::DateTime *pTimeReceivedMsg);
	bool ExecuteCommand(CGMCommand *pGMCommand);
};