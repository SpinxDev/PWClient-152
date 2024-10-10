// Filename	: DlgPKSetting.h
// Creator	: Xiao Zhou
// Date		: 2005/11/18

#pragma once

#include "DlgBase.h"
#include "AUICheckBox.h"

class CDlgPKSetting : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

	void OnCommand_Confirm(const char * szCommand);

public:
	CDlgPKSetting();
	virtual ~CDlgPKSetting();

	void OnChangePVPNoPenalty(bool bEnter);	
	void OnCommand_pk(const char * szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
	PAUICHECKBOX		m_pChk_Gprotect;
	PAUICHECKBOX		m_pChk_Wprotect;
	PAUICHECKBOX		m_pChk_Aprotect;
	PAUICHECKBOX		m_pChk_Allianceprotect;
	PAUICHECKBOX		m_pChk_Fprotect; // 势力保护
	PAUICHECKBOX		m_pChk_Buff1;
	PAUICHECKBOX		m_pChk_Buff2;
	PAUICHECKBOX		m_pChk_Buff3;
	PAUICHECKBOX		m_pChk_Buff4;
	PAUICHECKBOX		m_pChk_Buff5;
	PAUICHECKBOX		m_pChk_Buff6;
	PAUICHECKBOX		m_pChk_BlsNoForce; // 势力屏蔽
	PAUICHECKBOX		m_pChk_SwitchPK;
	PAUIOBJECT			m_pTxt_SwitchPK;
};
