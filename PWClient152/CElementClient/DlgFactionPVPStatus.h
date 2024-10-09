// Filename	: DlgFactionPVPStatus.h
// Creator	: Xu Wenbin
// Date		: 2014/4/2

#ifndef _ELEMENTCLIENT_DLGFACTIONPVPSTATUS_H_
#define _ELEMENTCLIENT_DLGFACTIONPVPSTATUS_H_

#include "DlgBase.h"
#include "EC_FactionPVP.h"

class CDlgFactionPVPStatus : public CDlgBase, public CECFactionPVPModelObserver
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgFactionPVPStatus();
	
	void OnCommandViewReward(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	bool ShouldShow();
	void Align();
	
	//	¼Ì³Ð×Ô FactionPVPModelObserver
	virtual void OnRegistered(const CECFactionPVPModel *p);
	virtual void OnModelChange(const CECFactionPVPModel *p, const CECObservableChange *q);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void UpdateStatus(const CECFactionPVPModel *p);

	void ResetInBattleUI();
		
	AUIObject *	m_pTxt_GuildName;
	AUIObject *	m_pTxt_Score;
	AUIObject *	m_pTxt_MineCar;
	AUIObject *	m_pTxt_MineBase;
	AUIObject *	m_pTxt_Win;
	AUIObject *	m_pBtn_Reward;
};

#endif