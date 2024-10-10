// Filename	: DlgCoupon.h
// Creator	: Feng Ning
// Date		: 2011/02/18

#pragma once

#include "DlgBase.h"

class CDlgCoupon : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCoupon();
	virtual ~CDlgCoupon();

	void OnCommand_Confirm(const char * szCommand);
	void OnUserCouponAction(int idAction, void* pData);
	void OnCommand_Record(const char *szCommand);
	void OnCommand_Buy(const char *szCommand);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	virtual void OnTick();

	int GetInputCoupon();

	bool RefreshInfo(bool bForce);
	struct CouponInfo
	{
		int remain;
		int today_exchange;
		int today_remain;
		int refresh_time;
	};
	CouponInfo m_Info;
	void SetInfo(const CouponInfo& info);
};
