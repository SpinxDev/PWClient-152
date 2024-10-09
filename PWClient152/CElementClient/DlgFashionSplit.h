// File		: DlgFashionSplit.h
// Creator	: Xu Wenbin
// Date		: 2013/4/15

#pragma once

#include "DlgBase.h"
#include "EC_IvtrFashion.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgFashionSplit : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgFashionSplit();
	virtual ~CDlgFashionSplit();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool SetFashion(CECIvtrFashion *pFashion, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	CECIvtrFashion * GetCurFashion();
	bool	CanConfirm();

	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIOBJECT				m_pTxt_Name;
	PAUIOBJECT				m_pTxt_Color;
	PAUIOBJECT				m_pTxt_Num;
	int						m_nSolt;
	int						m_nItemID;
};
