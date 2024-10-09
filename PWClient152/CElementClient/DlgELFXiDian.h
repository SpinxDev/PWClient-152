// Filename	: DlgELFXiDian.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUIStillImageButton.h"
#include "EC_IvtrItem.h"
class CDlgELFXiDian : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFXiDian();
	virtual ~CDlgELFXiDian();
	virtual bool Render(void);
	virtual void OnTick(void);
	virtual void DoDataExchange(bool bSave);
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnEventLButtonDownAdd(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDownMinus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	void SetELF(CECIvtrItem *pItem, int nSlot);
	void ResetPoint(void);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	
	PAUIIMAGEPICTURE		m_pImg_ELF;
	PAUILABEL				m_pTxt_TotleCount;
	PAUILABEL				m_pTxt_CostCount;
	PAUIOBJECT				m_pTxt_ELFName;
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	int						m_iSetting[4];
	int						m_iCost;
	int						m_iMaxPro[4];
	int						m_nSlot;
	int						m_nLastSlot;

	void RefreshHostDetails();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};