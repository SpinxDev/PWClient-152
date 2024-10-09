// Filename	: DlgBackOthers.h
// Creator	: Xu Wenbin
// Date		: 2009/08/13

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"

namespace GNET
{
	class RefGetReferenceCode_Re;
	class RefListReferrals_Re;
	class RefWithdrawBonus_Re;
}
class CDlgBackOthers : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgBackOthers();

	void OnCommand_Link(const char *szCommand);
	void OnCommand_Get(const char *szCommand);
	void OnCommand_First(const char *szCommand);
	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_End(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_Copy(const char *szCommand);
	
	void OnPrtcRefGetReferenceCodeRe(GNET::RefGetReferenceCode_Re *p);
	void OnPrtcRefListReferralsRe(GNET::RefListReferrals_Re *p);
	void OnPrtcRefWithdrawBonusRe(GNET::RefWithdrawBonus_Re *p);
	
	static AString GetUTF8URLFrom(const ACString &rhs);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
	void UpdateButton();
	void UpdateText();
	void UpdateList();

	bool CanBeReferral();
	const ACHAR *GetReferralName();

	ACString GetCashText(int nCash);

	PAUILISTBOX m_pLst_Member;
	PAUILABEL m_pTxt_Name;
	PAUILABEL m_pTxt_ID;
	PAUILABEL m_pTxt_Have;
	PAUILABEL m_pTxt_May;
	PAUILABEL m_pTxt_Page;

	bool m_bSortNameAscent;
	bool m_bSortLevelAscent;
	bool m_bSortPointAscent;
	
	enum {REFERRAL_PAGE_SIZE = 15};
	int  m_referralStartIndex;
	int  m_referralCount;
	int  m_referralBonusAvailable;
	ACString m_referralCode;
};