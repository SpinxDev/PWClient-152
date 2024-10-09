// File		: DlgArrangeTeam.h
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUICheckBox.h"
#include "AUIRadioButton.h"
#include "AUIStillImageButton.h"

class CECElsePlayer;
class CDlgArrangeTeam : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgArrangeTeam();
	virtual ~CDlgArrangeTeam();

	void OnCommandKick(const char * szCommand);
	void OnCommandInvite(const char * szCommand);
	void OnCommandLeave(const char * szCommand);
	void OnCommandRandom(const char * szCommand);
	void OnCommandSlogan(const char * szCommand);
	void OnCommandShowSelf(const char * szCommand);
	void OnCommandLfg(const char * szCommand);
	void OnCommandMakeLeader(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnCommandAcceptAutoTeam(const char * szCommand);

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool UpdateTeam(bool bUpdateNear = false);
	bool CheckShowCondition(const CECElsePlayer* pPlayer);

	// 1£∫Õ¨√À 2£∫µ–∂‘
 	int CheckFactionAlliance( const CECElsePlayer* pPlayer );

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	
	AUIListBox *			m_pLstCurrentTeamMate;
	AUIListBox *			m_pLstNearbypp;
	PAUIOBJECT			m_pTxtName;
	PAUIOBJECT			m_pTxtGroupMsg;
	AUICheckBox *			m_pChkSlogan;
	AUICheckBox *			m_pChkShowself;
	AUIRadioButton *		m_pRdoLfgroup;
	AUIRadioButton *		m_pRdoLfmember;
	AUIRadioButton *		m_pRdoRandom;
	AUIRadioButton *		m_pRdoFree;
	AUIStillImageButton *	m_pBtnLeave;
	AUIStillImageButton *	m_pBtnKick;
	AUIStillImageButton *	m_pBtnInvite;
	AUIStillImageButton *	m_pBtnDisband;
	AUICheckBox *			m_pChkShowSlogan;
	AUICheckBox *			m_pChkHostile;
	AUICheckBox *			m_pChkAutoTeam;

private:
	int m_nPickupMode;
};
