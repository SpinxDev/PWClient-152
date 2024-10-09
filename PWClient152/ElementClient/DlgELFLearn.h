// File		: DlgELFLearn.h
// Creator	: Chen Zhixin
// Date		: 2008/12/01

#pragma once

#include "DlgBase.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "AUIEditBox.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"

#define CDLGELFLEARN_PP			5

class CDlgELFLearn : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		AUI_DECLARE_EVENT_MAP()
public:
	
	void OnCommandCANCAL(const char * szCommand);
	void OnCommandMovel(const char * szCommand);
	void OnCommandMover(const char * szCommand);
	void SetELF(CECIvtrItem *pItem1, int nSlot);
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	bool UpdateTeach(int nPage = -1);
	void Reset();
	void SetNPCName(ACString name);
	virtual void OnTick(void);
	
	CDlgELFLearn();
	virtual ~CDlgELFLearn();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	
	AUIImagePicture *	m_pImg_ELF;
	PAUIOBJECT		m_pTxt_ELFName;
	AUIImagePicture *	m_pImg_SkillIcon[CDLGELFLEARN_PP];
	AUILabel *			m_pTxt_SkillName[CDLGELFLEARN_PP];
	AUILabel *			m_pTxt_SkillCostSP[CDLGELFLEARN_PP];
	AUILabel *			m_pTxt_Xuqiu[CDLGELFLEARN_PP];
	
	int					m_nSlot;
	
private:
	int m_nCurTeachPage;
};
