// File		: DlgEPEquip.h
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

class CDlgEPEquip : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgEPEquip();
	virtual ~CDlgEPEquip();
	void UpdateEquip(int idPlayer);
	void OnCommandTry(const char * szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUIIMAGEPICTURE	m_pImgEquip[SIZE_EQUIPIVTR];
	PAUILABEL			m_pTxt_Name;
	PAUILABEL			m_pTxt_Prof;
	PAUILABEL			m_pTxt_Level;
	PAUILABEL			m_pTxt_Faction;
	PAUILABEL			m_pTxt_Reincarnation;
};
