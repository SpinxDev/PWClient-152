// File		: DlgWebTradeOption.cpp
// Creator	: Feng Ning
// Date		: 2010/07/06

#include "DlgWebTradeOption.h"
#include "DlgWebTradeBase.h"
#include "EC_GameUIMan.h"
#include "DlgChat.h"
#include "gwebtradeitem"
#include "AIniFile.h"
#include "A3DDevice.h"
#include "AAssist.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWebTradeOption, CDlgBase)

AUI_ON_COMMAND("Whisper",	OnCommandWhisper)
AUI_ON_COMMAND("CopyID",	OnCommandCopyID)
AUI_ON_COMMAND("CopyURL",	OnCommandCopyURL)
AUI_ON_COMMAND("GotoWeb",	OnCommandGotoWeb)

AUI_END_COMMAND_MAP()

CDlgWebTradeOption::CDlgWebTradeOption()
{
}

CDlgWebTradeOption::~CDlgWebTradeOption()
{
}

void CDlgWebTradeOption::OnCommandWhisper(const char *szCommand)
{
	Show(false);
	CDlgWebTradeBase::ItemInfo* pInfo = (CDlgWebTradeBase::ItemInfo*)GetDataPtr();
	if(!pInfo)
	{
		return;
	}

	CDlgChat *pDlgWhisper = GetGameUIMan()->m_pDlgChatWhisper;
	if( pDlgWhisper->IsShow() && pDlgWhisper->GetData() == GetData() ) return;

	pDlgWhisper->Show(true);
	pDlgWhisper->SetData(pInfo->GetBasicInfo().seller_roleid);
	pDlgWhisper->GetDlgItem("DEFAULT_Txt_Speech")->SetText(_AL(""));

	PAUIOBJECT pText = pDlgWhisper->GetDlgItem("Txt_Chat");
	pText->SetText(_AL(""));

	pDlgWhisper->RefreshWhisperPlayerName();
}

void CDlgWebTradeOption::OnCommandCopyID(const char *szCommand)
{
	Show(false);

	CDlgWebTradeBase::ItemInfo* pInfo = (CDlgWebTradeBase::ItemInfo*)GetDataPtr();
	if(!pInfo)
	{
		return;
	}

	ACString strTxt;
	strTxt.Format(_AL("%d"), pInfo->GetBasicInfo().commodity_id);

	CopyToClipboard(strTxt);
}

void CDlgWebTradeOption::OnCommandCopyURL(const char *szCommand)
{
	Show(false);
	ACString szURL = CreateItemURL();
	
	if( !szURL.IsEmpty() )
	{
		CopyToClipboard(szURL);
	}
}

void CDlgWebTradeOption::OnCommandGotoWeb(const char *szCommand)
{
	Show(false);
	ACString szURL = CreateItemURL();
	
	if( !szURL.IsEmpty() )
	{
		CopyToClipboard(szURL);
		GetBaseUIMan()->NavigateURL(szURL, NULL);
	}
}

bool CDlgWebTradeOption::CopyToClipboard(const ACString& str)
{
	if( !OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
	{
		return false;
	}
	
	HLOCAL hLocal = GlobalAlloc(LMEM_ZEROINIT, str.GetLength() * 2 + 2);
	if( hLocal )
	{
		ACHAR *pszName = (ACHAR *)GlobalLock(hLocal);
		a_strcpy(pszName, str);
		EmptyClipboard();
#ifdef UNICODE
		SetClipboardData(CF_UNICODETEXT, pszName);
#else
		SetClipboardData(CF_TEXT, pszName);
#endif
		GlobalUnlock(hLocal);
	}
	
	CloseClipboard();
	
	return true;
}

ACString CDlgWebTradeOption::CreateItemURL()
{
	AString strURL = "";
	
	CDlgWebTradeBase::ItemInfo* pInfo = (CDlgWebTradeBase::ItemInfo*)GetDataPtr();
	if(pInfo)
	{
		AString strText = GetBaseUIMan()->GetURL("OnlineShop", "ITEM_URL");
		if(!strText.IsEmpty())
		{
			int commid = pInfo->GetBasicInfo().commodity_id;
			if(commid > 0)
			{
				strURL.Format(strText, commid);
			}
			else
			{
				// TODO: goto homepage?
				strURL = GetBaseUIMan()->GetURL("OnlineShop", "URL");
			}
		}
	}
	
	return AS2AC(strURL);
}
