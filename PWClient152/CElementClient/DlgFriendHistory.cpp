// File		: DlgFriendHistory.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "AFI.h"
#include "time.h"
#include "CSplit.h"
#include "DlgFriendHistory.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgFriendHistory, CDlgBase)

AUI_ON_EVENT("List_Message",	WM_LBUTTONUP,		OnEventLButtonUp)

AUI_END_EVENT_MAP()

CDlgFriendHistory::CDlgFriendHistory()
{
	m_pTxtContent = NULL;
	m_pListMessage = NULL;
}

CDlgFriendHistory::~CDlgFriendHistory()
{
}

void CDlgFriendHistory::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_Content", m_pTxtContent);
	DDX_Control("List_Message", m_pListMessage);
}

void CDlgFriendHistory::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	if( nCurSel < 0 || nCurSel >= pList->GetCount() )
	{
		m_pTxtContent->SetText(_AL(""));
		return;
	}

	ACString strItem = pList->GetText(nCurSel);
	CSplit s(strItem);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
	if ((int)vec.size() >= 3 )
	{
		ACString str = vec[2];
		m_pTxtContent->SetText(vec[2]);
	}
	else
		m_pTxtContent->SetText(_AL(""));
}

void CDlgFriendHistory::BuildChatHistory(int idPlayer)
{
	FILE *file;
	char szFile[MAX_PATH];
	CECHostPlayer *pHost = GetHostPlayer();
	PAUILISTBOX pList = m_pListMessage;

	pList->ResetContent();
	m_pTxtContent->SetText(_AL(""));

	sprintf(szFile, "%s\\Userdata\\Msg\\%d\\%d.txt",
		af_GetBaseDir(), pHost->GetCharacterID(), idPlayer);
	file = fopen(szFile, "rb");
	if( !file ) return;

	DWORD dwTime;
	int nLen, nRead;
	ACString strItem;
	ACHAR szName[40], szText[512];

	while( true )
	{
		// Player name.
		nRead = fread(&nLen, sizeof(int), 1, file);
		if( nRead <= 0 ) break;		// Error or end of file.
		memset(szName, 0, sizeof(ACHAR) * 40);
		nRead = fread(szName, sizeof(ACHAR), nLen, file);

		// Message time.
		nRead = fread(&dwTime, sizeof(DWORD), 1, file);

		// Message content.
		nRead = fread(&nLen, sizeof(int), 1, file);
		memset(szText, 0, sizeof(ACHAR) * 512);
		nRead = fread(szText, sizeof(ACHAR), nLen, file);

		long stime = dwTime;
		stime -= GetGame()->GetTimeZoneBias() * 60; // localtime = UTC - bias, in which bias is in minutes
		const tm* ptm = gmtime(&stime);
		strItem.Format(_AL("%s\t%04d.%02d.%02d\t%s"), szName,
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, szText);
		pList->AddString(strItem);
	}

	fclose(file);

	pList->SetCurSel(0);
	OnEventLButtonUp(0, 0, pList);
}
void CDlgFriendHistory::OnChangeLayoutEnd(bool bAllDone)
{
	if(IsShow())
	{
		GetGameUIMan()->BuildFriendList(this);
	}
}