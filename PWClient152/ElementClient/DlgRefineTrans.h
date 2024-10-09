// File		: DlgRefineTrans.h
// Creator	: Xiao Zhou
// Date		: 2007/8/31

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgRefineTrans : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgRefineTrans();
	virtual ~CDlgRefineTrans();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown_Source(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Target(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetSourceEquip(CECIvtrItem *pItem, int nSlot);
	void SetTargetEquip(CECIvtrItem *pItem, int nSlot);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUISTILLIMAGEBUTTON	m_pBtn_Trans;
	PAUIIMAGEPICTURE		m_pImg_Source;
	PAUIIMAGEPICTURE		m_pImg_Target;
	PAUIOBJECT				m_pTxt_Qiankun;
	PAUIOBJECT				m_pTxt_Trans;
	PAUIOBJECT				m_pTxt_Source;
	PAUIOBJECT				m_pTxt_Target;
	int						m_nSoltSource;
	int						m_nSoltTarget;
};
