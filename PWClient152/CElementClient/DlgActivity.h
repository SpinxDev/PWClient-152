// Filename	: DlgActivity.h
// Creator	: Xu Wenbin
// Date		: 2009/09/28

#pragma once

#include "DlgBase.h"
#include "EC_Counter.h"
#include "EC_AutoTeamConfig.h"

#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"

namespace GNET
{
	class Protocol;
}

class CDlgActivity : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:

	enum
	{
		ACTIVITY_DAILY,
		ACTIVITY_INSTANCE,
		ACTIVITY_HOLIDAY,
	};

	typedef CECAutoTeamConfig::Activity Activity;

	friend class CECAutoTeam;

public:
	CDlgActivity();

	void OnCommand_MainType(const char* szCommand);
	void OnCommand_SignIn(const char* szCommand);
	void OnCommand_Record(const char* szCommand);
	void OnCommand_QuickTeam(const char* szCommand);
	void OnCommand_CancelTeam(const char* szCommand);
	void OnCommand_GotoNPC(const char* szCommand);
	void OnCommand_SortList(const char* szCommand);
	void OnCommand_CheckShowAll(const char* szCommand);

	void OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	bool IsHasActivity();
	void EnableSignIn(bool bEnable);
	void SetSignInUpdateDay(int day) { m_sign_update_day = day; }

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL m_pLbl_Date;
	PAUILISTBOX m_pLst_Activity;
	PAUIIMAGEPICTURE m_pImg_Activity;
	PAUITEXTAREA m_pTxt_Desc;
	PAUIIMAGEPICTURE m_pImgAward[ACTIVITY_AWARD_NUM];
	PAUISTILLIMAGEBUTTON m_pBtn_Daily;
	PAUISTILLIMAGEBUTTON m_pBtn_Instance;
	PAUISTILLIMAGEBUTTON m_pBtn_Holiday;
	PAUISTILLIMAGEBUTTON m_pBtn_QuickTeam;
	PAUISTILLIMAGEBUTTON m_pBtn_Cancel;
	PAUISTILLIMAGEBUTTON m_pBtn_GotoNPC;
	PAUICHECKBOX m_pChk_ShowAllActivty;

	int m_iMainType;		// 当前列表活动类型
	bool m_bSortByDesc;

	int m_year;    // selected year (default to current, but may change at selection or actual time changes)
	int m_month;   // selected month (same as above)
	int m_day;     // selected day (as above)
	int m_sign_update_day; // 签到数据更新日期
	CECCounter m_SingInCounter;

private:
	void UpdateActivityList(bool bUpdateStatus = false);
	void UpdateActivityContent();

	void GetTime(int &year, int &month, int &day, int &hour, int &minute, int &second, int &week);
	void UpdateButtonState();

	ACString GetRequireLevelStr(const Activity* pAct);
	ACString GetRequireCountStr(const Activity* pAct);
	ACString GetRequireTimeStr(const Activity* pAct, bool bFullTime = true);

	bool IsShowAllActivity();
};