// File		: DlgHostPet.h
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUILabel.h"

class CDlgHostPet : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgHostPet();
	virtual ~CDlgHostPet();

	void UpdatePet(int nSlot = -1);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommand_CANCEL(const char * szCommand);
	
protected:
	virtual bool Render();
	virtual bool OnInitDialog();

	PAUIIMAGEPICTURE	m_pImg_Icon;
	PAUIPROGRESS		m_pPro_hp;
	PAUIPROGRESS		m_pPro_exp;
	PAUIPROGRESS		m_pPro_loyalty;
	PAUILABEL			m_pTxt_Level;

	int		m_nSlot;

};
