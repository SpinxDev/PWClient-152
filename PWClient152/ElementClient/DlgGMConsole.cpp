/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   15:16
	file name:	DlgGMConsole.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/


#include "DlgGMConsole.h"
#include "AUIListBox.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"
#include "EC_ElsePlayer.h"
#include "EC_HostPlayer.h"
#include "GMCommandFactory.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "DlgGMParam.h"
#include "PredefinedGMHelpMsg.h"
#include "GMCDBMan.h"
#include "DlgGMDelMsgReason.h"
#include "GMCommandListBox.h"
#include "CodeTemplate.h"
#include "DlgGMFinishMsg.h"
#include "DlgGMConsoleForbid.h"
#include "EC_TimeSafeChecker.h"

AUI_BEGIN_COMMAND_MAP(CDlgGMConsole, CDlgBase)

AUI_ON_COMMAND("finish", OnCommandFinish)
AUI_ON_COMMAND("ignore", OnCommandIgnore)
AUI_ON_COMMAND("select_command", OnCommandDblClickCommand)
AUI_ON_COMMAND("close", OnCommandClose)
AUI_ON_COMMAND("refresh", OnCommandRefresh)
AUI_ON_COMMAND("Btn_Gmconsole2", OnCommandNewGMUI)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgGMConsole, CDlgBase)

//AUI_ON_EVENT("lbxPlayerMsg", WM_LBUTTONDOWN, OnLButtonUpListboxPlayerMsg)
AUI_ON_EVENT("lbxPlayerNearby", WM_LBUTTONDOWN, OnLButtonUpListboxPlayerNearby)

AUI_END_EVENT_MAP()

CDlgGMConsole::CDlgGMConsole() :
	m_pLbxPlayerMsg(NULL),
	m_pLbxCommand(NULL),
	m_pEbxPlayerID(NULL),
	m_pLbxPlayerNearby(NULL),
	m_bInited(false),
	m_pGMCDBMan(NULL),
	m_pGMCommandListBox(NULL)
{

}

CDlgGMConsole::~CDlgGMConsole()
{
	
}

void CDlgGMConsole::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("lbxPlayerMsg", m_pLbxPlayerMsg);
	DDX_Control("lbxCommand", m_pLbxCommand);
	DDX_Control("ebxPlayerID", m_pEbxPlayerID);
	DDX_Control("lbxPlayerNearby", m_pLbxPlayerNearby);
}

ACString CDlgGMConsole::RetrivePlayerNameFromMsg(ACString strMsg)
{
	if (strMsg.IsEmpty())
		return _AL("");
	
	int nLength = strMsg.GetLength();
	int nIndex = 0;
	int nBegin = 0, nEnd = 0;
	// find first '&'
	while (nIndex < nLength)
	{
		if (strMsg[nIndex] == _AL('&'))
			break;
	}
	if (nIndex == nLength)
		return _AL("");
	nBegin = nIndex;

	// find second '&'
	while (nIndex < nLength)
	{
		if (strMsg[nIndex] == _AL('&'))
			break;
	}
	if (nIndex == nLength)
		return _AL("");
	nEnd = nIndex;

	if (nEnd - nBegin <= 1)
		return _AL("");

	return strMsg.Mid(nBegin + 1, nEnd - nBegin - 1);
}

int CDlgGMConsole::RetrivePlayerIDFromName(ACString strPlayerName)
{
	CECGameRun *pGameRun = g_pGame->GetGameRun();
	ASSERT(pGameRun);

	int nPlayerID = pGameRun->GetPlayerID(strPlayerName);
	
	return nPlayerID;
}

bool CDlgGMConsole::AddUserMessage(ACString strMsg, ACHAR type, int nPlayerID)
{
	if (!IsShow())	Show(true);

	ASSERT(TheGMHelpMsgArray::Instance()->FindByType(type, NULL));
	ASSERT(nPlayerID > 0);

	// compute data
	ACString msg;
	msg.Format(GetStringFromTable(6004), strMsg, nPlayerID);
	
	SimpleDB::DateTime *pTime = new SimpleDB::DateTime;
	pTime->LoadSystemTime();


	// add to db
	GetGMCDBMan()->GetDatabase().AddNewReceivedPlayerMsg(
		nPlayerID,
		type,
		msg,
		GetGMID(),
		*pTime);

	// add to listbox
	AddItemToListboxPlayerMsg(
		nPlayerID,
		type,
		msg,
		pTime);

	return true;
}

void CDlgGMConsole::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	
	LocalInit();
	ReFillListboxPlayerNearby();
}

void CDlgGMConsole::LocalInit()
{
	// note : we put init codes here but not OnInitDialog(), because 
	// ReFillListboxPlayerNearby() need to access some object that 
	// is not yet available in OnInitDialog() (they are inited after UI).
	if (m_bInited) return;

	if (!InitGMCDBMan())
	{
		throw "Init GM working log database failed!";
	}
	FillUserMsgListboxFromDB();
	
	SAFE_DELETE(m_pGMCommandListBox);
	m_pGMCommandListBox = new CGMCommandListBox;
	if (!m_pGMCommandListBox->Init(m_pLbxCommand, this))
	{
		delete m_pGMCommandListBox;
		throw "Init GMCommandListBox failed!";
	}

	m_bInited = true;		

}

bool CDlgGMConsole::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	m_nIDForMsgListBoxItem = 0;

	return true;
}

bool CDlgGMConsole::Release()
{
	// close database
	if (m_pGMCDBMan)
	{
		if (!SaveGMCDBMan(true))
		{
			a_LogOutput(1, "CDlgGMConsole::Release(), failed to call SaveGMCDBMan()");
		}
	}

	// clear playermsg listbox
	{
		int nCount = m_pLbxPlayerMsg->GetCount();
		while (nCount-- > 0)
		{
			DeleteItemFromListboxPlayerMsg(nCount);
		}
	}

//	SAFE_RELEASE(m_pGMCommandListBox);
	if (m_pGMCommandListBox)
	{
		m_pGMCommandListBox->Release();
		delete m_pGMCommandListBox;
		m_pGMCommandListBox = NULL;
	}
	
	
	m_bInited = false;

	return CDlgBase::Release();
}


void CDlgGMConsole::OnCommandDblClickCommand(const char * szCommand)
{
	ACString strPlayerID = m_pEbxPlayerID->GetText();
	int nPlayerID = strPlayerID.ToInt();

	m_pGMCommandListBox->SetAssocData(&GetGMCDBMan()->GetDatabase(), nPlayerID, NULL);
	m_pGMCommandListBox->OnDblClick();
}



void CDlgGMConsole::OnCommandFinish(const char * szCommand)
{
	int nSelIndex = m_pLbxPlayerMsg->GetCurSel();
	if (-1 == nSelIndex || nSelIndex >= m_pLbxPlayerMsg->GetCount())
		return;

	if (GetGameUIMan()->m_pDlgGMFinishMsg->IsShow())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(6008), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if (CheckIsMsgItemAssocedWithAGMFinishMsgDlg(nSelIndex))
		return;

	
	int nPlayerID = 0;
	SimpleDB::DateTime *pReceivedTime = NULL;
	GetParamFromPlayerMsgListItem(nSelIndex, nPlayerID, pReceivedTime);
	DWORD dwItemID = m_pLbxPlayerMsg->GetItemData(nSelIndex, 2);

	GetGameUIMan()->m_pDlgGMFinishMsg->SetAssocData(
		&GetGMCDBMan()->GetDatabase(),
		m_pLbxPlayerMsg->GetText(nSelIndex),
		nPlayerID,
		pReceivedTime,
		dwItemID);
	GetGameUIMan()->m_pDlgGMFinishMsg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter, this);
	GetGameUIMan()->m_pDlgGMFinishMsg->Show(true);

	// set item flag to be owned
	m_pLbxPlayerMsg->SetItemData(nSelIndex, 1, 3);
}


void CDlgGMConsole::OnCommandClose(const char * szCommand)
{
	SaveGMCDBMan(false);
	Show(false);
}

void CDlgGMConsole::SetEditboxPlayerID(int nPlayerID)
{
	m_pEbxPlayerID->SetData(static_cast<DWORD>(nPlayerID));
	ACString strText;
	strText.Format(_AL("%d"), nPlayerID);
	m_pEbxPlayerID->SetText(strText);
}

//void CDlgGMConsole::OnLButtonUpListboxPlayerMsg(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
//{
//	int nSelMsgIndex = m_pLbxPlayerMsg->GetCurSel();
//	int nPlayerID = -1;
//	if (nSelMsgIndex >= 0 && nSelMsgIndex < m_pLbxPlayerMsg->GetCount())
//	{
//		nPlayerID = static_cast<int>(m_pLbxPlayerMsg->GetItemData(nSelMsgIndex));
//	}
//	SetEditboxPlayerID(nPlayerID);
//}

void CDlgGMConsole::OnLButtonUpListboxPlayerNearby(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nSelIndex = m_pLbxPlayerNearby->GetCurSel();
	int nPlayerID = -1;
	if (nSelIndex >= 0 && nSelIndex < m_pLbxPlayerNearby->GetCount())
	{
		nPlayerID = static_cast<int>(m_pLbxPlayerNearby->GetItemData(nSelIndex));
	}
	SetEditboxPlayerID(nPlayerID);
}

void CDlgGMConsole::ReFillListboxPlayerNearby()
{
	CECWorld *pWorld = GetWorld();
	CECHostPlayer *pHost = GetGameRun()->GetHostPlayer();
	CECPlayerMan *pPlayerMan = pWorld->GetPlayerMan();
	const APtrArray<CECElsePlayer *> &playersNearby = pPlayerMan->GetPlayersInMiniMap();
	
	m_pLbxPlayerNearby->ResetContent();
	for (int nIndex = 0; nIndex < playersNearby.GetSize(); ++nIndex)
	{
		int nPlayerID = playersNearby[nIndex]->GetCharacterID();
		ACString strName;
		strName.Format(GetStringFromTable(6004), playersNearby[nIndex]->GetName(), nPlayerID);
		
		m_pLbxPlayerNearby->AddString(strName);
		m_pLbxPlayerNearby->SetItemData(nIndex, nPlayerID);
	}
}

void CDlgGMConsole::OnCommandRefresh(const char * szCommand)
{
	ReFillListboxPlayerNearby();
}
void CDlgGMConsole::OnCommandNewGMUI(const char* szCommand)
{
	if(!GetGameUIMan()->m_pDlgGmConsoleForbid->IsShow())
		GetGameUIMan()->m_pDlgGmConsoleForbid->Show(true);

	Show(false);
}
GMCDBMan * CDlgGMConsole::GetGMCDBMan()
{
	return m_pGMCDBMan;
}

bool CDlgGMConsole::InitGMCDBMan()
{
	ASSERT(!m_pGMCDBMan);

	int nGMID = GetHostPlayer()->GetCharacterID();
	m_pGMCDBMan = new GMCDBMan(nGMID);
	if (!m_pGMCDBMan->Load())
	{
		a_LogOutput(1, "CDlgGMConsole::InitGMCDBMan(), failed to call GMCDBMan::Load()");
		return false;
	}

	return true;
}

bool CDlgGMConsole::SaveGMCDBMan(bool bRelease)
{
	ASSERT(m_pGMCDBMan);

	bool bSaveOK = m_pGMCDBMan->Save();
	if (!bSaveOK)
	{
		a_LogOutput(1, "CDlgGMConsole::SaveGMCDBMan(), failed to call GMCDBMan::Save()");
	}
	if (bRelease)
	{
		delete m_pGMCDBMan;
		m_pGMCDBMan = NULL;
	}

	return bSaveOK;
}

namespace CDlgGMConsolePrivate
{
	struct FillUserMsgListboxFromDBHelperFunc
	{
		// note : use member function pointer to call private member function
		typedef void (CDlgGMConsole::*AddToListboxFunctionType)(int nPlayerID, ACHAR type, ACString msg, SimpleDB::DateTime *pReceivedTime);
	private:
		AddToListboxFunctionType pFunc_;
		CDlgGMConsole *pDlg_;
	public:
		FillUserMsgListboxFromDBHelperFunc(AddToListboxFunctionType pFunc, CDlgGMConsole *pDlg)
			: pFunc_(pFunc), pDlg_(pDlg)
		{
		}
		void operator () (const UserMsgTable::RecordType &record)
		{
			// ignore processed msg
			if (!record[FIELD_INDEX(UserMsgTable::isProcessed)].IsNull())
				return;

			// add to list
			SimpleDB::DateTime *pDateTime = new SimpleDB::DateTime;
			*pDateTime = record.GetFieldValue(FIELD_INDEX(UserMsgTable::timeReceivedMsg));
			(pDlg_->*pFunc_)(
				record.GetFieldValue(FIELD_INDEX(UserMsgTable::playerID)),
				record.GetFieldValue(FIELD_INDEX(UserMsgTable::msgType)),
				record.GetFieldValue(FIELD_INDEX(UserMsgTable::playerMsg)),
				pDateTime);
		}
	};
}

void CDlgGMConsole::FillUserMsgListboxFromDB()
{
	const UserMsgTable & userMsgTable = 
		GetGMCDBMan()->GetDatabase().GetTableAt(
			STATIC_DWORD_INDEX(GMCDatabase::userMsgTable));

	ForEach(
		userMsgTable.Begin(), 
		userMsgTable.End(),
		CDlgGMConsolePrivate::FillUserMsgListboxFromDBHelperFunc(
		static_cast<CDlgGMConsolePrivate::FillUserMsgListboxFromDBHelperFunc::AddToListboxFunctionType>(&CDlgGMConsole::AddItemToListboxPlayerMsg), 
			this));
}

int CDlgGMConsole::GetGMID()
{
	ASSERT(GetHostPlayer()->IsGM());
	return GetHostPlayer()->GetCharacterID();
}

void CDlgGMConsole::OnCommandIgnore(const char * szCommand)
{
	int nSelIndex = m_pLbxPlayerMsg->GetCurSel();
	if (-1 == nSelIndex || nSelIndex >= m_pLbxPlayerMsg->GetCount())
		return;

	if (CheckIsMsgItemAssocedWithAGMFinishMsgDlg(nSelIndex))
		return;

	GetGameUIMan()->m_pDlgGMDelMsgReason->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter, this);
	GetGameUIMan()->m_pDlgGMDelMsgReason->Show(true, true);
}

void CDlgGMConsole::AddItemToListboxPlayerMsg(int nPlayerID, ACHAR type, ACString msg, SimpleDB::DateTime *pReceivedTime)
{
	GMHelpMsg msgInfo;
	TheGMHelpMsgArray::Instance()->FindByType(type, &msgInfo);

	ACString finalString;
	finalString.Format(_AL("[%s] %s"), pReceivedTime->ToString(), msg);

	int nIndex =  m_pLbxPlayerMsg->AddString(finalString) - 1;
	m_pLbxPlayerMsg->SetItemTextColor(nIndex, msgInfo.color);
	m_pLbxPlayerMsg->SetItemData(nIndex, nPlayerID, 0); // playerid who sent this msg
	m_pLbxPlayerMsg->SetItemData(nIndex, reinterpret_cast<DWORD>(pReceivedTime), 1); // the datetime we received this msg
	m_pLbxPlayerMsg->SetItemData(nIndex, m_nIDForMsgListBoxItem++, 2);	// id to identify this item
	m_pLbxPlayerMsg->SetItemData(nIndex, 0, 3);	// mark the item as not owner by a CDlgGMFinishMsg
	m_pLbxPlayerMsg->SetItemHint(nIndex, finalString);
}

void CDlgGMConsole::DeleteItemFromListboxPlayerMsg(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_pLbxPlayerMsg->GetCount());
	SimpleDB::DateTime *pTime = reinterpret_cast<SimpleDB::DateTime *>(
		m_pLbxPlayerMsg->GetItemData(nIndex, 1));
	delete pTime;

	m_pLbxPlayerMsg->DeleteString(nIndex);
	int nNextIndex = nIndex;
	if (nNextIndex >= m_pLbxPlayerMsg->GetCount())
		nNextIndex = m_pLbxPlayerMsg->GetCount() - 1;
	m_pLbxPlayerMsg->SetCurSel(nNextIndex);
}

void CDlgGMConsole::ConfiremCommandIgnore(int nSelReasonIndex, ACString strCustomReason)
{
	// prepare data
	ACString msg;
	if (0 == nSelReasonIndex)
		msg = strCustomReason;
	else
		msg = GetStringFromTable(CDlgGMDelMsgReason::predefinedReason[nSelReasonIndex]);

	int nSelIndex = m_pLbxPlayerMsg->GetCurSel();
	if (-1 == nSelIndex || nSelIndex >= m_pLbxPlayerMsg->GetCount())
		return;

	int nPlayerID = 0;
	SimpleDB::DateTime *pReceivedTime = NULL;
	GetParamFromPlayerMsgListItem(nSelIndex, nPlayerID, pReceivedTime);

	// update db
	GetGMCDBMan()->GetDatabase().FinishPlayerMsg(nPlayerID, *pReceivedTime, false, msg);

	// delete from listbox
	DeleteItemFromListboxPlayerMsg(nSelIndex);
}	

void CDlgGMConsole::GetParamFromPlayerMsgListItem(int nIndex, int & nPlayerID, SimpleDB::DateTime *& pReceivedTime)
{
	ASSERT(nIndex >= 0 && nIndex < m_pLbxPlayerMsg->GetCount());

	nPlayerID = m_pLbxPlayerMsg->GetItemData(nIndex, 0);
	pReceivedTime = reinterpret_cast<SimpleDB::DateTime *>(m_pLbxPlayerMsg->GetItemData(nIndex, 1));
}

void CDlgGMConsole::OnTick()
{
	CDlgBase::OnTick();

	if (m_bInited)
	{
		static DWORD dwLastTime = ::GetTickCount();
		if (CECTimeSafeChecker::ElapsedTimeFor(dwLastTime) > 15 * 60 * 1000)
		{
			SaveGMCDBMan(false);
			dwLastTime = ::GetTickCount();
		}
	}
}


void CDlgGMConsole::ConfirmCommandFinish(DWORD dwItemIDInMsgListBox, bool bFinished)
{
	int nCount = m_pLbxPlayerMsg->GetCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_pLbxPlayerMsg->GetItemData(nIndex, 2) == dwItemIDInMsgListBox)
		{
			if (bFinished)
			{
				// update db
				int nPlayerID = 0;
				SimpleDB::DateTime *pReceivedTime = NULL;
				GetParamFromPlayerMsgListItem(nIndex, nPlayerID, pReceivedTime);
				GetGMCDBMan()->GetDatabase().FinishPlayerMsg(nPlayerID, *pReceivedTime, true);
				
				// delete in listbox
				DeleteItemFromListboxPlayerMsg(nIndex);
			}
			else
			{
				m_pLbxPlayerMsg->SetItemData(nIndex, 0, 3);
			}
			return;
		}
	}


}

bool CDlgGMConsole::CheckIsMsgItemAssocedWithAGMFinishMsgDlg(int nIndex)
{
	if (-1 == nIndex || nIndex >= m_pLbxPlayerMsg->GetCount())
		return false;

	bool bOwned = (m_pLbxPlayerMsg->GetItemData(nIndex, 3) != 0);
	if (bOwned)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(6006), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
	}

	return bOwned;
}
