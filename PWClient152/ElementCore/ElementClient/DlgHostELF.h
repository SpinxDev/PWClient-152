// File		: DlgHostELF.h
// Creator	: Chen Zhixin
// Date		: 2008/11/07

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUILabel.h"
#include "EC_Skill.h"

class CDlgELFProp;
class CDlgHostELF : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgHostELF();
	virtual ~CDlgHostELF();
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventSkill(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Activation(const char * szCommand);
	void OnCommand_Max(const char * szCommand);
	void OnCommand_Min(const char * szCommand);

	void SetImage(AUIImagePicture *pImage, CECSkill *pSkill);
protected:
	virtual bool Render();
	virtual bool OnInitDialog();
	
	PAUIIMAGEPICTURE	m_pImg_Icon;
	PAUIPROGRESS		m_pPro_energy;
	PAUIPROGRESS		m_pPro_power;
	PAUIPROGRESS		m_pPro_loyalty;
	PAUILABEL			m_pTxt_name;
	PAUIIMAGEPICTURE	m_pImg_Activation;
	
	int		m_nSlot;
	
};
