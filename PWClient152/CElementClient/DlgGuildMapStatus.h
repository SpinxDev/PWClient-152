// Filename	: DlgGuildMapStatus.h
// Creator	: Xiao Zhou
// Date		: 2005/1/5

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"

class CDlgGuildMapStatus : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgGuildMapStatus();
	virtual ~CDlgGuildMapStatus();

	void OnCommandCancel(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnCommandAllBattleTime(const char * szCommand);
	void OnCommandMax(const char* szCommand);

	void OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	

	void SetIndex(int n);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	void PropertyAdd(AUIObject *pObj);
	void PropertyMinus(AUIObject *pObj);

	PAUILABEL	m_pTxt_Name;
	PAUILABEL	m_pTxt_Level;
	PAUILABEL	m_pTxt_Win;
	PAUILABEL	m_pTxt_Attach;
	PAUILABEL	m_pTxt_Attack;
	PAUILABEL	m_pTxt_Time;
	PAUIOBJECT	m_pTxt_NewMoney;
	int			m_nIndex;
	
	int m_Bonus_id;
	int m_Bonus_Count1;
	int m_Bonus_Count2;	
	int m_Bonus_Count3;

	PAUIOBJECT m_pButtonPress;
	DWORD m_dwStartTime;
	DWORD m_dwLastTime;
	int m_iIntervalTime;
	BOOL m_bAdd;
};