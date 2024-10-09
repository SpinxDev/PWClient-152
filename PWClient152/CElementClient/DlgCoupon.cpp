// Filename	: DlgCoupon.cpp
// Creator	: Feng Ning
// Date		: 2011/02/18

#include "DlgCoupon.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "AUIManager.h"

#include "usercoupon_re.hpp"
#include "usercouponexchange_re.hpp"
#include "AIniFile.h"

#define COUPON_RATIO 100
#define COUPON_REFRESH_INTERVAL 60

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCoupon, CDlgBase)

AUI_ON_COMMAND("Btn_OK",		OnCommand_Confirm)
AUI_ON_COMMAND("Btn_BuyCoupon",	OnCommand_Buy)
AUI_ON_COMMAND("Btn_CouponRecord",	OnCommand_Record)

AUI_END_COMMAND_MAP()

#define USERCOUPON_MSGBOX(str) m_pAUIManager->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

enum ERR_COUPON
{   
	ERR_COUPON_NOT_ENOUGH = 401,
	ERR_COUPON_EXCHANGE_ONGOING,
	ERR_COUPON_EXCHANGE_DAYLIMIT,
	ERR_COUPON_EXCHANGE_SERVER_FORBID,
	ERR_COUPON_EXCHANGE_NOT_ACTIVE,
	ERR_COUPON_EXCHANGE_TOO_OFTEN,
};

CDlgCoupon::CDlgCoupon()
{
	memset(&m_Info, 0, sizeof(m_Info));
}

CDlgCoupon::~CDlgCoupon()
{
}

bool CDlgCoupon::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	PAUIOBJECT pBtn = GetDlgItem("Btn_OK");
	if(pBtn) pBtn->Enable(false);

	memset(&m_Info, 0, sizeof(m_Info));
	return true;
}

void CDlgCoupon::OnShowDialog()
{
	RefreshInfo(false);
}

void CDlgCoupon::OnCommand_Confirm(const char * szCommand)
{
	int coupon = GetInputCoupon();
	if(coupon <= 0)
	{
		USERCOUPON_MSGBOX(GetStringFromTable(5651));
		return;
	}

	// check the input number
	if(m_Info.refresh_time != 0)
	{
		if( m_Info.remain < coupon )
		{
			USERCOUPON_MSGBOX(GetStringFromTable(5659));
			return;
		}

		if(m_Info.today_remain < coupon)
		{
			USERCOUPON_MSGBOX(GetStringFromTable(5658));
			return;
		}
	}

	// pop msgbox to confirm
	PAUIDIALOG pMsgBox;
	ACString strNum, strMsg;
	strNum.Format(GetStringFromTable(5660), coupon);
	strMsg.Format(GetStringFromTable(5650), strNum);

	m_pAUIManager->MessageBox( "Game_Coupon_Confirm", strMsg, 
							   MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(coupon);
}

void CDlgCoupon::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(!stricmp(pDlg->GetName(), "Game_Coupon_Confirm"))
	{
		if(iRetVal == IDYES)
		{
			int coupon = pDlg->GetData();
			GetGameSession()->userCoupon_Exchange(coupon);

			PAUIOBJECT pBtn = GetDlgItem("Btn_OK");
			if(pBtn) pBtn->Enable(false);
		}
	}
}

int CDlgCoupon::GetInputCoupon()
{
	// get the input coupon
	PAUIOBJECT pObjCoupon = GetDlgItem("Edit_Coupon");
	return !pObjCoupon ? 0 : a_atoi(pObjCoupon->GetText()) * COUPON_RATIO;
}

void CDlgCoupon::OnTick()
{
	PAUIOBJECT pLab_Cash = GetDlgItem("Lab_Cash");
	if(pLab_Cash)
	{
		int coupon = GetInputCoupon();
		ACString strNum;
		pLab_Cash->SetText(strNum.Format(_AL("%d"), coupon / COUPON_RATIO));
	}
}

bool CDlgCoupon::RefreshInfo(bool bForce)
{
	int curTime = GetGame()->GetServerAbsTime();
	// account info can be refreshed every 10 seconds
	if(bForce || m_Info.refresh_time == 0 || curTime - m_Info.refresh_time > COUPON_REFRESH_INTERVAL)
	{
		GetGameSession()->userCoupon_Account();
		m_Info.refresh_time = curTime;
		return true;
	}

	return false;
}

void CDlgCoupon::SetInfo(const CouponInfo& info)
{
	m_Info.today_exchange = info.today_exchange;
	m_Info.today_remain = info.today_remain;
	m_Info.remain = info.remain;
	if(m_Info.remain < m_Info.today_remain)
	{
		m_Info.today_remain = m_Info.remain;
	}
	m_Info.refresh_time = GetGame()->GetServerAbsTime();

	// update UI
	ACString strNum;
	PAUIOBJECT pLab_Remain = GetDlgItem("Lab_Remain");
	if(pLab_Remain) pLab_Remain->SetText(strNum.Format(GetStringFromTable(5660), m_Info.remain));
	
	PAUIOBJECT pLab_TodayExchange = GetDlgItem("Lab_TodayExchange");
	if(pLab_TodayExchange) pLab_TodayExchange->SetText(strNum.Format(GetStringFromTable(5660), m_Info.today_exchange));
	
	PAUIOBJECT pLab_TodayRemain = GetDlgItem("Lab_TodayRemain");
	if(pLab_TodayRemain) pLab_TodayRemain->SetText(strNum.Format(GetStringFromTable(5660), m_Info.today_remain));
	
	PAUIOBJECT pBtn = GetDlgItem("Btn_OK");
	if(pBtn) pBtn->Enable(m_Info.today_remain >= COUPON_RATIO);
	
	PAUIEDITBOX pObjCoupon = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Edit_Coupon"));
	int numLen = (int)log10((double)max(m_Info.today_remain / COUPON_RATIO, 1)) + 1;
	if(pObjCoupon) pObjCoupon->SetMaxLength(max(5, numLen)); // at least 5 number
}

void CDlgCoupon::OnUserCouponAction(int idAction, void* pData)
{
	ASSERT(pData);
	int retcode = 0;

	PAUIOBJECT pBtn = GetDlgItem("Btn_OK");

	switch (idAction)
	{
	case PROTOCOL_USERCOUPON_RE:
		{
			GNET::UserCoupon_Re* p = (GNET::UserCoupon_Re*)pData;
			retcode = p->retcode;
			if(!retcode)
			{ 
				CouponInfo info;
				info.today_exchange = p->today_exchange_coupon;
				info.today_remain = p->today_remain_coupon;
				info.remain = p->remain_coupon;
				SetInfo(info);
			}
		}
		break;
		
	case PROTOCOL_USERCOUPONEXCHANGE_RE:
		{
			GNET::UserCouponExchange_Re* p = (GNET::UserCouponExchange_Re*)pData;
			retcode = p->retcode;
			if(!retcode)
			{
				CouponInfo info;
				info.today_exchange = p->today_exchange_coupon;
				info.today_remain = p->today_remain_coupon;
				info.remain = p->remain_coupon;
				SetInfo(info);
				USERCOUPON_MSGBOX(GetStringFromTable(5657));
			}
		}
		break;

	default:
		ASSERT(0);
	}

	// show the error message
	switch(retcode)
	{
	case ERR_COUPON_NOT_ENOUGH:
		USERCOUPON_MSGBOX(GetStringFromTable(5652));
		// wait for account updating
		RefreshInfo(true);
		if(pBtn) pBtn->Enable(false);
		break;
	case ERR_COUPON_EXCHANGE_ONGOING:
		USERCOUPON_MSGBOX(GetStringFromTable(5653));
		break;
	case ERR_COUPON_EXCHANGE_DAYLIMIT:
		USERCOUPON_MSGBOX(GetStringFromTable(5654));
		// wait for account updating
		RefreshInfo(true);
		if(pBtn) pBtn->Enable(false);
		break;
	case ERR_COUPON_EXCHANGE_SERVER_FORBID:
		USERCOUPON_MSGBOX(GetStringFromTable(5655));
		break;
	case ERR_COUPON_EXCHANGE_NOT_ACTIVE:
		USERCOUPON_MSGBOX(GetStringFromTable(5656));
		break;

	case ERR_COUPON_EXCHANGE_TOO_OFTEN:
		USERCOUPON_MSGBOX(GetStringFromTable(5661));
		SetInfo(m_Info); // to unlock the UI
		break;
	}
}

void CDlgCoupon::OnCommand_Record(const char *szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("Coupon", "RECORD");
	GetBaseUIMan()->NavigateURL(strText);
}

void CDlgCoupon::OnCommand_Buy(const char *szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("Coupon", "BUY");
	GetBaseUIMan()->NavigateURL(strText);
}
