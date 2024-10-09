// Filename	: DlgOnlineAward.h
// Creator	: Wang Yongdong
// Date		: 2012/05/17


#pragma once

#include "DlgBase.h"
#include "ExpTypes.h"
#include "EC_GPDataType.h"
#include "AUIRadioButton.h"
#include "AUIProgress.h"

enum
{
	AWARD_TYPE_EXP1 = 0,
	AWARD_TYPE_EXP2,
	AWARD_TYPE_EXP3,
	AWARD_TYPE_EXP4,
	MAX_AWARD_TYPE,
};

class CECHostPlayer;

class COnlineAwardCtrl
{
public:
	COnlineAwardCtrl(CECHostPlayer* player);
	virtual ~COnlineAwardCtrl(){}

	void OnRecvOnlineAwardData(int total_time,int count,S2C::cmd_online_award_data::_entry* pEntry);
	void OnToggleOnlineAward(int type,int active);

	void OnCommand_Type1(const char* szCommand);

	int ChargeAwardTime(int type,int count);
	void ToggleAwardType(int type,int active);

	int GetActiveType() const ;
	int GetAwardTime(int type) const;
	int GetCurTotalAwardTime() const { return m_cur_total_time;};
	int GetTotalAwardTime() const { return m_total_time;}

	int GetTotalItemCount(int type);

protected:
	S2C::cmd_online_award_data::_entry m_award_data[MAX_AWARD_TYPE];
	int m_cur_total_time;
	int m_total_time;
	int m_type_state[MAX_AWARD_TYPE];

	CECHostPlayer* m_pHostPlayer;

	ONLINE_AWARDS_CONFIG * m_pAwardData;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

class CDlgOnlineAward : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgOnlineAward();
	virtual ~CDlgOnlineAward();
	
	void OnCommand_Add(const char* szCommand);
	void OnCommand_Confirm(const char* szCommand);
	void OnCommand_AddExp2(const char* szCommand);
	void OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	

	void ToggleStartFlag(bool bStart, bool bInit) ;
	void UpdateTime(COnlineAwardCtrl* pCtrl);

	bool Is2QDlg();
	void SwitchGuajiIsland();
	void RestartWhenLevelup();

protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void DoDataExchange(bool bSave);

	void PropertyAdd(AUIObject *pObj);
	void PropertyMinus(AUIObject *pObj);
	ACString GetDetailTime(int t);

protected:
	PAUIOBJECT m_pButtonPress;

	PAUIRADIOBUTTON m_pRadion1;
	PAUIRADIOBUTTON m_pRadion2;
	PAUIPROGRESS m_pProgress;

	DWORD m_dwStartTime;
	DWORD m_dwLastTime;
	int m_iIntervalTime;
	BOOL m_bAdd;

	int m_award_type;
	int m_item_count[MAX_AWARD_TYPE];

	bool m_bStart;

	bool m_bShowFlag;
};
