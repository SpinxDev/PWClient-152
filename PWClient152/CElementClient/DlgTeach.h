// File		: DlgTeach.h
// Creator	: Xiao Zhou
// Date		: 2005/8/16
#pragma once

#include "DlgBase.h"
#include "EC_GameUIMan.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"

#define CDLGTEACH_TEACHS_PP			8

class CDlgTeach : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:

	void OnCommandCANCAL(const char * szCommand);
	void OnCommandMovel(const char * szCommand);
	void OnCommandMover(const char * szCommand);
	void OnCommandSkilltree(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool UpdateTeach(int nPage = -1);

	CDlgTeach();
	virtual ~CDlgTeach();
	
protected:
	virtual void DoDataExchange(bool bSave);
	
	PAUIOBJECT		m_pTxtRestSP;
	AUIImagePicture *	m_pSkillIcon[CDLGTEACH_TEACHS_PP];
	PAUIOBJECT		m_pSkillName[CDLGTEACH_TEACHS_PP];
	PAUIOBJECT		m_pSkillLevel[CDLGTEACH_TEACHS_PP];
	PAUIOBJECT		m_pSkillCostSP[CDLGTEACH_TEACHS_PP];
	PAUIOBJECT		m_pSkillCostMoney[CDLGTEACH_TEACHS_PP];

private:
	int m_nCurTeachPage;
};
