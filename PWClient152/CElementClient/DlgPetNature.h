// File		: DlgPetNature.h
// Creator	: WYD
// Date		: 2013\03\08

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

struct PET_EVOLVED_SKILL_RAND_CONFIG;

class CDlgPetNature : public CDlgBase  
{
	enum
	{
		NATURE_NUM_PER_PAGE = 8,
		MAX_NATURE = 30
	};

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgPetNature();
	virtual ~CDlgPetNature(){};
	
	void OnCommandPre(const char * szCommand);
	void OnCommandNext(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void SetPetIndex(int i) { m_iPetIndex = i;}
	
protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();

	void UpateUI(int idx);
	
	PAUIIMAGEPICTURE	m_pImgSkill[NATURE_NUM_PER_PAGE*2];
	PAUILABEL			m_pSkillName[NATURE_NUM_PER_PAGE*2];
	PAUILABEL			m_pLabelPage;
	PAUILABEL			m_pImgName[NATURE_NUM_PER_PAGE];

	int m_iCurPage; // 1,2,...
	int m_iPetIndex;
	int m_iSkillCount;
	int m_iMaxPage; // 1, 2,...
	PET_EVOLVED_SKILL_RAND_CONFIG* m_pNatureList;
};
