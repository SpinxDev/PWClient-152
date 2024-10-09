// Filename	: DlgELFGeniusReset.h
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

class CDlgELFGeniusReset : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFGeniusReset();
	virtual ~CDlgELFGeniusReset();
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	void SetELF(CECIvtrItem *pItem, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
		
protected:
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_ELF;
	PAUILABEL				m_pTxt_Item;
	PAUILABEL				m_pTxt_Item2;
	int						m_nSolt;
	int						m_nLastSolt;
	int						m_dwEnalbeTick;
	
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void RefreshHostDetails();
};
