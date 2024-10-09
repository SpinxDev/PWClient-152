// Filename	: DlgBackOthers.cpp
// Creator	: Xu Wenbin
// Date		: 2009/08/13

#include "AFI.h"
#include "DlgBackOthers.h"
#include "DlgQShop.h"
#include "DlgBackHelp.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "reflistreferrals_re.hpp"
#include "refgetreferencecode_re.hpp"
#include "refwithdrawbonus_re.hpp"
#include <AIniFile.h>
#include "gameclient.h"
#include "A3DDevice.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBackOthers, CDlgBase)

AUI_ON_COMMAND("link",      OnCommand_Link)
AUI_ON_COMMAND("get",       OnCommand_Get)
AUI_ON_COMMAND("first",     OnCommand_First)
AUI_ON_COMMAND("prev",      OnCommand_Prev)
AUI_ON_COMMAND("next",      OnCommand_Next)
AUI_ON_COMMAND("end",       OnCommand_End)
AUI_ON_COMMAND("refresh",   OnCommand_Refresh)
AUI_ON_COMMAND("copy",      OnCommand_Copy)

AUI_END_COMMAND_MAP()

CDlgBackOthers::CDlgBackOthers()
{
	m_pLst_Member = NULL;
	m_pTxt_ID = NULL;
	m_pTxt_Have = NULL;
	m_pTxt_May = NULL;
	m_pTxt_Page = NULL;
	m_pTxt_Name = NULL;

	m_bSortNameAscent = false;
	m_bSortLevelAscent = false;
	m_bSortPointAscent = false;
	
	m_referralStartIndex = -1;
	m_referralCount = 0;
	m_referralBonusAvailable = 0;
}

bool CDlgBackOthers::OnInitDialog()
{
	DDX_Control("List_Member", m_pLst_Member);
	m_pLst_Member->ResetContent();
	
	DDX_Control("Txt_Name", m_pTxt_Name);
	m_pTxt_Name->SetText(_AL(""));

	DDX_Control("Txt_ID", m_pTxt_ID);
	m_pTxt_ID->SetText(_AL(""));

	DDX_Control("Txt_HaveGold", m_pTxt_Have);
	m_pTxt_Have->SetText(_AL(""));

	DDX_Control("Txt_MayGold", m_pTxt_May);
	m_pTxt_May->SetText(_AL(""));

	DDX_Control("Txt_Page", m_pTxt_Page);
	m_pTxt_Page->SetText(_AL(""));

	return true;
}

void CDlgBackOthers::OnShowDialog()
{
	UpdateText();
	UpdateList();
	GetGameSession()->dividend_GetReferenceCode();
}

void CDlgBackOthers::OnTick()
{
	UpdateButton();
	UpdateText();
}

void CDlgBackOthers::UpdateButton()
{
	// Referral list related
	//
	bool notFirstPage = m_referralStartIndex>0;
	bool notLastPage = m_referralCount>REFERRAL_PAGE_SIZE
		&& m_referralStartIndex<(m_referralCount-REFERRAL_PAGE_SIZE);

	GetDlgItem("Btn_FirstPage")->Enable(notFirstPage);
	GetDlgItem("Btn_PrevPage")->Enable(notFirstPage);
	GetDlgItem("Btn_NextPage")->Enable(notLastPage);
	GetDlgItem("Btn_EndPage")->Enable(notLastPage);

	// Referral query qualification related
	const ROLEBASICPROP &prop = GetHostPlayer()->GetBasicProps();
	bool canQuery = CanBeReferral();

	GetDlgItem("Btn_rsh")->Enable(canQuery);
	GetDlgItem("Btn_GetLink")->Enable(canQuery);

	// Others
	GetDlgItem("Btn_GetGold")->Enable(m_referralBonusAvailable>0);
	GetDlgItem("Btn_Copy")->Enable(GetReferralName()!=NULL);
}

void CDlgBackOthers::UpdateText()
{	
	const GNET::RoleInfo &info = GetGameRun()->GetSelectedRoleInfo();
	if (info.referrer_role > 0)
	{
		const ACHAR *szReferralName = GetReferralName();
		m_pTxt_Name->SetText(szReferralName ? szReferralName : _AL(""));
	}
	else if (info.referrer_role == -1)
	{
		m_pTxt_Name->SetText(GetStringFromTable(7974));
	}
	else
	{
		m_pTxt_Name->SetText(GetStringFromTable(7972));
	}
	
	const ROLEBASICPROP &prop = GetHostPlayer()->GetBasicProps();
	m_pTxt_ID->SetText(CanBeReferral() ? m_referralCode : GetStringFromTable(7973));

	ACString strMoney = GetCashText(GetHostPlayer()->GetDividend());
	m_pTxt_Have->SetText(strMoney);

	ACString strTemp;
	strTemp.Format(_AL("%d"), m_referralBonusAvailable);
	m_pTxt_May->SetText(strTemp);
	
	int iPage(0), nPage(0);
	if (m_referralCount > 0)
	{
		iPage = m_referralStartIndex/REFERRAL_PAGE_SIZE + 1;
		nPage = m_referralCount/REFERRAL_PAGE_SIZE;
		if (m_referralCount % REFERRAL_PAGE_SIZE)
			nPage ++;
	}
	strTemp.Format(_AL("%d/%d"), iPage, nPage);
	m_pTxt_Page->SetText(strTemp);
}

void CDlgBackOthers::UpdateList()
{
	if (m_referralStartIndex < 0)
		m_referralStartIndex = 0;

	GetGameSession()->dividend_GetListReferrals(m_referralStartIndex);
}

bool CDlgBackOthers::CanBeReferral()
{
	const ROLEBASICPROP &prop = GetHostPlayer()->GetBasicProps();
	return prop.iLevel>=20 && GetHostPlayer()->GetReputation()>=20;
}

const ACHAR * CDlgBackOthers::GetReferralName()
{
	const GNET::RoleInfo &info = GetGameRun()->GetSelectedRoleInfo();
	return GetGameRun()->GetPlayerName(info.referrer_role, false);
}

ACString CDlgBackOthers::GetCashText(int nCash)
{
	return GetGameUIMan()->GetCashText(nCash);
}

void CDlgBackOthers::OnCommand_Link(const char *szCommand)
{
	if (m_referralCode.IsEmpty())
		return;

	AString strFormat = GetBaseUIMan()->GetURL("REFERRAL", "LinkFormat");
	if (strFormat.IsEmpty())
		return;

	AString strURL;
	strURL.Format(
		strFormat,
		GetUTF8URLFrom(m_referralCode),
		GetUTF8URLFrom(GetHostPlayer()->GetName()),
		GetGameSession()->GetZoneID());

	GetBaseUIMan()->NavigateURL(strURL);
}

AString CDlgBackOthers::GetUTF8URLFrom(const ACString &rhs)
{	
	int UTF8BufferSize = rhs.GetLength()*3 + 1;
	char *UTF8Buffer = (char*)a_malloctemp(sizeof(char) * UTF8BufferSize);
	::memset(UTF8Buffer, 0, UTF8BufferSize);	

	// Convert whole address to utf8
	WideCharToMultiByte(CP_UTF8, 0, rhs, -1, UTF8Buffer, UTF8BufferSize, NULL, NULL);

	// Get URL from utf8 buffer
	AString http;
	AString temp;
	int UTF8Length = strlen(UTF8Buffer);
	for (int i = 0; i < UTF8Length;)
	{
		char ch = UTF8Buffer[i];
		if ((unsigned char)ch <= (unsigned char)0x7f)
		{
			if (ch>='0' && ch<='9' ||
				ch>='A' && ch<='Z' ||
				ch>='a' && ch<='z')
			{
				http += ch;
			}
			else if (ch == ' ')
			{
				http += '+';
			}
			else
			{
				temp.Format("%%%02X", (unsigned char)ch);
				http += temp;
			}
			++ i;
		}
		else
		{
			temp.Format("%%%02X%%%02X%%%02X",
				(unsigned char)UTF8Buffer[i],
				(unsigned char)UTF8Buffer[i+1],
				(unsigned char)UTF8Buffer[i+2]);
			http += temp;
			i += 3;
		}
	}

	a_freetemp(UTF8Buffer);

	return http;
}

void CDlgBackOthers::OnCommand_Get(const char *szCommand)
{
	GetGameSession()->dividend_WithdrawDividend();
}

void CDlgBackOthers::OnCommand_First(const char *szCommand)
{
	if (m_referralCount > 0)
	{
		m_referralStartIndex = 0;
		UpdateList();
	}
}

void CDlgBackOthers::OnCommand_Prev(const char *szCommand)
{
	if (m_referralCount > 0)
	{
		if (m_referralStartIndex - REFERRAL_PAGE_SIZE >= 0)
			m_referralStartIndex -= REFERRAL_PAGE_SIZE;
		UpdateList();
	}
}

void CDlgBackOthers::OnCommand_Next(const char *szCommand)
{
	if (m_referralCount > 0)
	{
		if (m_referralStartIndex + REFERRAL_PAGE_SIZE < m_referralCount)
			m_referralStartIndex += REFERRAL_PAGE_SIZE;
		UpdateList();
	}
}

void CDlgBackOthers::OnCommand_End(const char *szCommand)
{
	if (m_referralCount > 0)
	{		
		m_referralStartIndex = m_referralCount;
		
		m_referralStartIndex -=
			(m_referralCount % REFERRAL_PAGE_SIZE)
			?m_referralCount % REFERRAL_PAGE_SIZE
			:REFERRAL_PAGE_SIZE;

		UpdateList();
	}
}

void CDlgBackOthers::OnCommand_Refresh(const char *szCommand)
{
	UpdateList();
}

void CDlgBackOthers::OnCommand_Copy(const char *szCommand)
{
	const ACHAR * szName = GetReferralName();
	if (szName &&
		OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
	{
		HLOCAL hLocal = GlobalAlloc(LMEM_ZEROINIT, a_strlen(szName) * 2 + 2);
		if( hLocal )
		{
			ACHAR *pszName = (ACHAR *)GlobalLock(hLocal);
			a_strcpy(pszName, szName);
			EmptyClipboard();
#ifdef UNICODE
			SetClipboardData(CF_UNICODETEXT, pszName);
#else
			SetClipboardData(CF_TEXT, pszName);
#endif
			GlobalUnlock(hLocal);
		}
		
		CloseClipboard();
	}
}

void CDlgBackOthers::OnPrtcRefGetReferenceCodeRe(GNET::RefGetReferenceCode_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
		return;

	m_referralCode = ACString((const ACHAR *)p->refcode.begin(), p->refcode.size()/sizeof(ACHAR));
}

void CDlgBackOthers::OnPrtcRefListReferralsRe(GNET::RefListReferrals_Re *p)
{
	if (p->retcode == ERR_OP_DOWN_OFFLINE ||
		p->retcode == ERR_OP_UP_NOT_LOAD  ||
		p->retcode == ERR_OP_PICKINGUP)
	{
		m_pLst_Member->ResetContent();
		m_referralStartIndex = -1;
		m_referralCount = 0;
		m_referralBonusAvailable = 0;
		return;
	}

	if (p->retcode != ERR_SUCCESS)
		return;

	if (!m_pLst_Member)
		return;

	m_pLst_Member->ResetContent();

	m_referralStartIndex = p->start_index;
	m_referralCount = p->total;
	m_referralBonusAvailable = p->bonus_avail_today;

	ACString strText, strHint;
	ACString strTemp;
	size_t count = p->referrals.size();
	for (size_t i = 0; i < count; ++i)
	{
		const ReferralBrief& temp = p->referrals[i];

		if (temp.rolenames.size() < 1)
			continue;

		// Generate content
		strTemp = ACString((const ACHAR *)temp.rolenames[0].begin(), temp.rolenames[0].size()/sizeof(ACHAR));
		strText.Format(GetStringFromTable(temp.rolenames.size()<=1 ? 7961 : 7962),
			strTemp,
			temp.max_level,
			temp.bonus_avail,
			temp.bonus_left,
			temp.bonus_total1,
			temp.bonus_total2);

		// Generate hint
		if (temp.rolenames.size() == 1)
		{
			strHint = _AL("");
		}
		else
		{
			strHint = GetStringFromTable(7960);
			size_t count = temp.rolenames.size();
			for (size_t j = 0; j < count; ++ j)
			{
				const Octets & name = temp.rolenames[j];
				strHint += _AL("\r");
				strHint += ACString((const ACHAR *)name.begin(), name.size()/sizeof(ACHAR));
			}
		}

		m_pLst_Member->AddString(strText);
		m_pLst_Member->SetItemHint(m_pLst_Member->GetCount()-1, strHint);
	}
}

void CDlgBackOthers::OnPrtcRefWithdrawBonusRe(GNET::RefWithdrawBonus_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
		return;

	UpdateList();
}