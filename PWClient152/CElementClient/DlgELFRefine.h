// Filename	: DlgELFRefine.h
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

#define  ELF_REFINE_TICKET0 23547
#define  ELF_REFINE_TICKET1 23548
#define  ELF_REFINE_TICKET2 23549
#define  ELF_REFINE_TICKET3 23550

class CDlgELFRefine : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFRefine();
	virtual ~CDlgELFRefine();
	virtual bool Render(void);
	virtual void OnTick(void);
	virtual void DoDataExchange(bool bSave);

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_FillMax(const char * szCommand);
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Minus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	
	void SetELF(CECIvtrItem *pItem, int nSlot);
	void SetItem(CECIvtrItem *pItem, int nSlot);
	void RefineResult(int result);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIIMAGEPICTURE		m_pImg_ELF;
	PAUIOBJECT				m_pTxt_Item;
	PAUIOBJECT				m_pTxt_Name;
	PAUIOBJECT				m_pTxt_DescNorm;
	PAUIOBJECT				m_pTxt_DescRefined;
	PAUITEXTAREA			m_pTxt_RefineList;
	PAUILABEL				m_pTxt_Prob;
	PAUILABEL				m_pTxt_FailResult;
	int						m_nSolt;
	int						m_nSoltItem;
	int						m_nLastSolt;
	int						m_nLastSoltItem;
	int						m_dwEnalbeTick;
	int						m_nCount;
	int						m_nMaxCount;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void RefreshHostDetails();
};
