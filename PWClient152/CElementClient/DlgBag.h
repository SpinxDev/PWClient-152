// Filename	: DlgBag.h
// Creator	: Xu Wenbin
// Date		: 2009/11/12

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

class CDlgBag : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
public:
	virtual bool Tick(void);
	
	void OnLButtonDownItem(WPARAM, LPARAM, AUIObject *pObj);
	void OnEventLButtonDblclk(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCommand_CANCEL(const char * szCommand);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	virtual bool OnInitDialog();
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual bool Render();

	void RefreshIconName(int start);
	void UpdateBag();

	abase::vector<PAUIIMAGEPICTURE> m_vecImgCell;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgBg;
};
