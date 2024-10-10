// Filename	: DlgELFRefineTrans.h
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUIStillImageButton.h"
#include "AUITextArea.h"
#include "AUIEditBox.h"
#include "EC_IvtrItem.h"

class CDlgELFRefineTrans : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFRefineTrans();
	virtual ~CDlgELFRefineTrans();
	virtual bool Render(void);
	virtual void OnTick(void);
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnEventLButtonDown_Source(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Target(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	void SetSource(CECIvtrItem *pItem, int nSlot);
	void SetTarget(CECIvtrItem *pItem, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_Source;
	PAUIIMAGEPICTURE		m_pImg_Target;
	PAUIOBJECT				m_pTxt_Source;
	PAUIOBJECT				m_pTxt_Target;
	PAUIOBJECT				m_pTxt_Result;
	PAUIOBJECT				m_pTxt_Count;
	int						m_nSlotSource;
	int						m_nSlotTarget;
	int						m_nLastSlotSource;
	int						m_nLastSlotTarget;
	int						m_dwEnalbeTick;
	
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void RefreshHostDetails();
};
