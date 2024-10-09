// File		: DlgQuickBarPet.h
// Creator	: Xiao Zhou
// Date		: 2006/3/22

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

#define MAX_PET_SKILL_SLOT 4
#define MAX_PET_NATURE_SKILL_SLOT 2
#define MAX_PET_ALL_SKILL_SLOT 7
#define MAX_PET_SPECIAL_SKILL_SLOT 1

class CDlgQuickBarPet : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgQuickBarPet();
	virtual ~CDlgQuickBarPet();

	void OnCommandSwitch(const char * szCommand);
	void OnCommandAttack(const char * szCommand);
	void OnCommandFollow(const char * szCommand);
	void OnCommandStop(const char * szCommand);
	void OnCommandOffensive(const char * szCommand);
	void OnCommandDefensive(const char * szCommand);
	void OnCommandCombat(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown_Skill(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_Skill(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	static int				m_nAutoCastID;
	static void ResetAutoCastSkill();

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	PAUIIMAGEPICTURE		m_pImg_Skill[MAX_PET_SKILL_SLOT + MAX_PET_NATURE_SKILL_SLOT + MAX_PET_SPECIAL_SKILL_SLOT];

	PAUISTILLIMAGEBUTTON	m_pBtn_Combat;
	PAUISTILLIMAGEBUTTON	m_pBtn_Offensive;
	PAUISTILLIMAGEBUTTON	m_pBtn_Defensive;
	PAUISTILLIMAGEBUTTON	m_pBtn_Stop;
	PAUISTILLIMAGEBUTTON	m_pBtn_Follow;
	int						m_idLastTarget;
	int						m_idLastSkill;
	DWORD					m_dwLastSkillTime;

	SIZE					m_Original;
	SIZE					m_IconMargin;
};
