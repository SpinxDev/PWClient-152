/*
 * FILE: DlgBroadCast.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgBroadCast.h"
#include "AUIListBox.h"
#include "AIniFile.h"
#include "AWIniFile.h"
#include "vector.h"

#include "EC_GameUIMan.h"

AUI_BEGIN_COMMAND_MAP(CDlgBroadCast, CDlgBase)
AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBroadCast, CDlgBase)
AUI_END_EVENT_MAP()

#ifdef _UNICODE 
#define ACIniFile AWIniFile
#else
#define ACIniFile AIniFile
#endif

//-----------------------------------------------------------
CDlgBroadCast::CDlgBroadCast()
{

}

//-----------------------------------------------------------
CDlgBroadCast::~CDlgBroadCast()
{
	ClearListBox();
}

//-----------------------------------------------------------
bool CDlgBroadCast::OnInitDialog()
{
	m_pListBoxChannel = (AUIListBox*)GetDlgItem("Lst_Channel");
	
	AWIniFile iniFile;
	if(iniFile.Open("configs\\broadcast.ini"))
	{
		//ACString strSection = _AL("Channel");
		int nNum = 0;
		nNum = iniFile.GetValueAsInt(_AL("Channel"), _AL("Number"), nNum);
		
		for(int i = 0; i < nNum; i++)
		{
			ACString strChannelNameKey;
			ACString strChannelURLKey;

			strChannelNameKey.Format(_AL("ChannelName%d"), i);
			strChannelURLKey.Format(_AL("ChannelURL%d"), i);


			ACString *strChannelName = new ACString;
			ACString *strChannelURL = new ACString;
			
			*strChannelName = iniFile.GetValueAsString( _AL("Channel"), strChannelNameKey);
			*strChannelURL = iniFile.GetValueAsString( _AL("Channel"), strChannelURLKey);

			if( !strChannelURL->IsEmpty() && !strChannelName->IsEmpty())
			{
				m_vecStringW.push_back(strChannelName);
				m_vecStringA.push_back(strChannelURL);
				
				m_pListBoxChannel->InsertString(i, *strChannelName);
				m_pListBoxChannel->SetItemDataPtr(i, (void*)strChannelURL);
			}
			else
			{
				delete strChannelName;
				delete strChannelURL;
			}
		}
		return true;
	}
	return false;
}

//判断注册表操作是否成功
#define MAKESURE_FOUND_REALPLAYER(val) \
if( val != ERROR_SUCCESS)\
{\
	GetGameUIMan()->MessageBox("", GetStringFromTable(779), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));\
	return ;\
}

//-----------------------------------------------------------
void CDlgBroadCast::OnCommandConfirm(const char* szCommand)
{
	int nCurSel = m_pListBoxChannel->GetCurSel();
	ACString* szFilePath = (ACString*)m_pListBoxChannel->GetItemDataPtr(nCurSel);
	
	if( szFilePath)
	{
		ACString strTemp = *szFilePath;
		strTemp.MakeLower();
		if (strTemp.Find(_AL("http://")) == 0 || strTemp.Find(_AL("https://")) == 0)
			GetBaseUIMan()->NavigateURL(*szFilePath, "Win_ExplorerRadio");
		else
			ShellExecute(NULL,_AL("open"), _AL("wmplayer.exe"), *szFilePath, NULL, SW_SHOWNORMAL);
	}

	/*
	HKEY hKey, hKey1, hKey2;
	char szBuf[300];
	long k=300;
	
	MAKESURE_FOUND_REALPLAYER(RegOpenKeyA(HKEY_CLASSES_ROOT,".ram",&hKey))
	MAKESURE_FOUND_REALPLAYER(RegQueryValueA(hKey,NULL,szBuf,&k))
	MAKESURE_FOUND_REALPLAYER(RegCloseKey(hKey))
	
	MAKESURE_FOUND_REALPLAYER(RegOpenKeyA(HKEY_CLASSES_ROOT,szBuf,&hKey))
	MAKESURE_FOUND_REALPLAYER(RegOpenKeyA(hKey,"shell",&hKey1))
	MAKESURE_FOUND_REALPLAYER(RegOpenKeyA(hKey1,"open",&hKey2))
	
	k=300;
	MAKESURE_FOUND_REALPLAYER(RegQueryValueA(hKey2,"command",szBuf,&k))
	MAKESURE_FOUND_REALPLAYER(RegCloseKey(hKey2))
	MAKESURE_FOUND_REALPLAYER(RegCloseKey(hKey1))
	MAKESURE_FOUND_REALPLAYER(RegCloseKey(hKey))
	
	char szFile[MAX_PATH];
	char* pTail = strstr(szBuf, ".exe");

	if( !pTail)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(779), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));\
		return;
	}

	*(pTail+strlen(".exe")) = '\0';
	strcpy(szFile, szBuf+1);
	
	if( szFilePath)
	{
		ShellExecuteA(NULL,"open", szFile, szFilePath, NULL, SW_SHOWNORMAL);
	}
	*/
	
	this->Show(false);
}

//-----------------------------------------------------------
void CDlgBroadCast::OnCommandCancel(const char* szCommand)
{
	

	this->Show(false);
}

//-----------------------------------------------------------
void CDlgBroadCast::ClearListBox()
{
	//Clear W String vector
	abase::vector<ACString*>::iterator iterW = m_vecStringW.begin();
	for( ;iterW != m_vecStringW.end(); ++iterW)
	{
		if (*iterW)
		{
			delete (*iterW);
			*iterW = NULL;
		}
	}

	m_vecStringW.clear();

 	//Clear A String vector
 	abase::vector<ACString*>::iterator iterA = m_vecStringA.begin();
 	for( ;iterA != m_vecStringA.end(); ++iterA)
 	{
 		if (*iterA)
 		{
 			delete (*iterA);
 			*iterA = NULL;
 		}
 	}

	m_vecStringA.clear();

}

//---------------------------------------------------------------
void CDlgBroadCast::OnShowDialog()
{
	CDlgBase::OnShowDialog();
}