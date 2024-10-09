// Filename	: DlgFindServer.cpp
// Creator	: Xu Wenbin
// Date		: 2009/06/24

#include "EC_Global.h"
#include "DlgFindServer.h"
#include "DlgLoginServerList.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include <AWIniFile.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFindServer, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_ON_COMMAND("confirm", OnCommandQuery)

AUI_END_COMMAND_MAP()

CDlgFindServer::CDlgFindServer()
{
	m_pEdit_ServerName = NULL;
}

CDlgFindServer::~CDlgFindServer()
{
}

bool CDlgFindServer::OnInitDialog()
{
	DDX_Control("Edt_ServerName", m_pEdit_ServerName);
	return true;
}

void CDlgFindServer::OnShowDialog()
{
	m_pEdit_ServerName->SetText(_AL(""));
	ChangeFocus(m_pEdit_ServerName);
}

void CDlgFindServer::OnCommandCancel(const char *szCommand)
{
	Show(false);
}

void CDlgFindServer::OnCommandQuery(const char *szCommand)
{
	// Query server merge record
	//
	
	// Ignore empty input
	ACString serverOld = m_pEdit_ServerName->GetText();
	if (serverOld.GetLength() < 1)
	{
		return;
	}

	// Find server name in merge record file
	ACString serverNew;
	if (!FindServerImpl(serverOld, serverNew))
	{
		m_pAUIManager->MessageBox("ServerList_Query", GetStringFromTable(3000), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	// Switch to the right server in the LoginServerList dialog
	CDlgLoginServerList *pDlgServerList = (CDlgLoginServerList *)m_pAUIManager->GetDialog("Win_LoginServerList");
	pDlgServerList->SelectServerByName(serverNew);

	// Hide to show the found server in server list box
	Show(false);
	
	// Notify the player about the result
	ACString strNotify;
	strNotify.Format(GetStringFromTable(3001), serverNew);	
	m_pAUIManager->MessageBox("", strNotify, MB_OK, A3DCOLORRGB(255, 255, 255));
}

bool CDlgFindServer::FindServerImpl(const ACString &serverToFind, ACString &serverNew)
{	
	// Validate not empty
	if (serverToFind.GetLength() < 1)
		return false;
	
	// Find server name in merge record file
	ACString serverOld = serverToFind;
	const char * szFileName = "configs\\servermerge.ini";
	bool found(false);
	AWIniFile file;
	if (file.Open(szFileName))
	{
		int iSect(1);
		int iKey(1);
		ACHAR szSect[40];
		ACHAR szKey[40];
		ACString curName;
		bool foundInCurSect(false);
		while (true)
		{
			// For each sect
			a_sprintf(szSect, _AL("MERGE_%d"), iSect);
			iKey = 1;
			foundInCurSect = false;
			while (true)
			{
				// for each key
				a_sprintf(szKey, _AL("Server_%d"), iKey);
				curName = file.GetValueAsString(szSect, szKey);
				
				if (curName.GetLength() < 1)
				{
					// No more server
					break;
				}
				
				if (curName == serverOld)
				{
					// Found it
					found = true;
					foundInCurSect = true;
					serverNew = file.GetValueAsString(szSect, _AL("Final"));
					serverOld = serverNew;
					break;
				}
				
				// Move to next server
				++ iKey;
			}
			
			if (iKey == 1 && !foundInCurSect)
			{
				// No more sect
				break;
			}
			
			// Move to next sect
			++ iSect;
		}
		
		file.Close();
	}

	return found;
}