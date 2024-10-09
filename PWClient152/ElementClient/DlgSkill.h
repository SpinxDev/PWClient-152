/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   13:22
	file name:	DlgSkill.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

/*
#pragma once

#include <vector.h>
#include "DlgBase.h"
#include "AUIScroll.h"
#include "AUIImagePicture.h"

class CECSkill;

#define CECDLGSKILL_MAX 40
#define CECDLGSKILL_LINEMAX 8

class CDlgSkill : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

protected:
	typedef abase::vector<CECSkill *> SkillVector;

	void UpdatePositiveSkill(const SkillVector &sortedSkills);
	void UpdatePassiveSkill(const SkillVector &sortedSkills);

	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual bool Render();

	virtual bool OnChangeLayoutBegin();

	PAUISCROLL m_pScl_Item;
	PAUIIMAGEPICTURE m_pImgItem[CECDLGSKILL_MAX];
	int	m_nTotalItem;
	int	m_nComboSelect;

public:
	CDlgSkill();
	virtual ~CDlgSkill();
	virtual bool Tick(void);

	void UpdateComboSkill();
	void SetImage(AUIImagePicture *pImage, CECSkill *pSkill);
	void SelectComboSkill(int n);
	
	void OnEventLButtonDownInitiative(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDownPassive(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDownItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnCommandSkillTree(const char * szCommand);
	void OnCommandEdit(const char * szCommand);
	void OnCommandDelete(const char * szCommand);
	void OnCommandNew(const char * szCommand);
	void UpdateView();
};
*/
