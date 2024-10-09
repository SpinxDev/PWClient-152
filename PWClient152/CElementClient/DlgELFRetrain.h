// File		: DlgELFRetrain.h
// Creator	: Chen ZHixin
// Date		: 2008/11/18

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIEditBox.h"
#include "EC_Skill.h"

class CECIvtrItem;

class CDlgELFRetrain : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgELFRetrain();
	virtual ~CDlgELFRetrain();
	virtual void DoDataExchange(bool bSave);
	
	void SetELF(CECIvtrItem *pItem, int nSlot);

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDownSkill(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventInput(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void RefreshHostDetails();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Render(void);
	void SetImage(AUIImagePicture *pImage, int id);
	void Reset(void);
	
	PAUILABEL			m_pTxt_ELFName;
	PAUILABEL			m_pTxt_ELFLevel;
	PAUILABEL			m_pTxt_Desc;
	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;
	PAUILABEL			m_pTxt_SkillName;
	PAUIIMAGEPICTURE	m_pImg_Item;
	PAUIIMAGEPICTURE	m_pImg_PIC;
	PAUIIMAGEPICTURE	m_pImg_Skill[8];
	PAUILABEL			m_pTxt_Skill[8];
	PAUIIMAGEPICTURE	m_pImg_Genius[5];
	int					m_nSkillID;
	int					m_nRetrainLevel;
	int					m_nMaxLevel;
	int					m_nSlot;
	int					m_nLastSolt;
	int					m_nSelect;
	CECSkill*			m_pSkill[8];
};
