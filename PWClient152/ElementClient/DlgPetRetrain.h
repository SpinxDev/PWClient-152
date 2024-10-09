// File		: DlgPetRetrain.h
// Creator	: Xiao Zhou
// Date		: 2006/6/22

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgPetRetrain : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetRetrain();
	virtual ~CDlgPetRetrain();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDownSkill(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL			m_pTxt_PetName;
	PAUILABEL			m_pTxt_PetLevel;
	PAUILABEL			m_pTxt_no;
	PAUIIMAGEPICTURE	m_pImg_Item;
	PAUIIMAGEPICTURE	m_pImg_Itema;
	PAUIIMAGEPICTURE	m_pImg_Skill[4];
	PAUILABEL			m_pTxt_Skill[4];
	int					m_nSkillID;
};
