#include "GMCommandListBox.h"
#include "GMCommand.h"
#include "GMCommandFactory.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "DlgGMParam.h"
#include "GMCDatabase.h"


CGMCommandListBox::CGMCommandListBox()
: m_pAssocListBox(NULL), 
	m_pDatabase(NULL), 
	m_nPlayerID(-1),
	m_pdtReceivedMsg(NULL)
{
}


bool CGMCommandListBox::Init(AUIListBox *pAssocListBox, AUIDialog *pParentDlg)
{
	m_pAssocListBox = pAssocListBox;
	m_pParentDlg = pParentDlg;

	FillContent();	

	return true;
}

CECGameUIMan* CGMCommandListBox::GetGameUIMan() const
{
	return g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
}

void CGMCommandListBox::FillContent()
{
	ASSERT(m_pAssocListBox);

	// init command listbox with all registered commands in factory
	CGMCommandKeyArray keyArray;
	TheGMCommandFactory::Instance()->GetRegisteredCommandKeys(keyArray);
	CGMCommandKeyArray::iterator iter = keyArray.begin();
	for (; keyArray.end() != iter; ++iter)
	{
		CGMCommand *pCommand = 
			TheGMCommandFactory::Instance()->CreateObject(*iter);
		if (pCommand->HasPrivilege())
		{
			int nIndex = m_pAssocListBox->AddString(pCommand->GetDisplayName()) - 1;
			m_pAssocListBox->SetItemDataPtr(nIndex, pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
}



void CGMCommandListBox::OnDblClick()
{
	ASSERT(m_pAssocListBox);

	int nIndex = m_pAssocListBox->GetCurSel();
	if (-1 == nIndex || nIndex >= m_pAssocListBox->GetCount()) return;
	
	CGMCommand *pGMCommand = 
		reinterpret_cast<CGMCommand *>(m_pAssocListBox->GetItemDataPtr(nIndex));
	ASSERT(pGMCommand);
	if (pGMCommand->GetParamArray().size() > 0)
	{
		// try set playerid if any
		pGMCommand->TrySetPlayerIDParam(m_nPlayerID);
		
		// show param dialog
		CDlgGMParam *pDlgGMParam = dynamic_cast<CDlgGMParam *>(GetGameUIMan()->GetDialog("Win_GMParam"));
		ASSERT(pDlgGMParam);
		pDlgGMParam->SetAssocData(pGMCommand, this);
		pDlgGMParam->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter, m_pParentDlg);
		pDlgGMParam->Show(true, true);
	}
	else
	{
		ExecuteCommand(pGMCommand);
	}
}

void CGMCommandListBox::SetAssocData(GMCDatabase *pDatabase, int nPlayerID, const SimpleDB::DateTime *pTimeReceivedMsg)
{
	m_pDatabase = pDatabase;
	m_nPlayerID = nPlayerID;
	m_pdtReceivedMsg = pTimeReceivedMsg;
}

bool CGMCommandListBox::ExecuteCommand(CGMCommand *pGMCommand)
{
	ASSERT(m_pDatabase);

	ACString strErr, strDetail;
	bool bResult = pGMCommand->Execute(strErr, strDetail);
	if (bResult)
	{
		ASSERT(g_pGame->GetGameRun()->GetHostPlayer()->IsGM());
		int nGMID = g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID();

		if (m_pdtReceivedMsg)
		{
			m_pDatabase->LogExecutedCommand(
				pGMCommand->GetDisplayName(),
				strDetail,
				nGMID,
				SimpleDB::DateTime::GetSystemTime(),
				m_nPlayerID,
				*m_pdtReceivedMsg);
		}
		else
		{
			m_pDatabase->LogExecutedCommand(
				pGMCommand->GetDisplayName(),
				strDetail,
				nGMID);
		}
	}
	else
	{
		GetGameUIMan()->MessageBox("", strErr, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	return bResult;
}

void CGMCommandListBox::Release()
{
	int nCount = m_pAssocListBox->GetCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CGMCommand *pCommand = reinterpret_cast<CGMCommand *>(m_pAssocListBox->GetItemDataPtr(nIndex));
		delete pCommand;
	}
	m_pAssocListBox->ResetContent();
}
