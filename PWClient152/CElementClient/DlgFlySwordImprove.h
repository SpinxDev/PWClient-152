// Filename	: DlgFlySwordImprove.h
// Creator	: WYD
// Date		: 2013/11/29

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CECIvtrFlySword;

class CDlgFlySwordImprove : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
	
public:
	CDlgFlySwordImprove();
	virtual ~CDlgFlySwordImprove();
	
	void OnCommandCANCEL(const char * szCommand);
	void OnCommandImprove(const char * szCommand);
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void DoImprove();

protected:

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

	int GetObtainedItemCountForImprove();
	void ClearFlySword(PAUIIMAGEPICTURE pImage);
	bool CanImprove(CECIvtrItem* pItem);
	void SetFlySwordDynInfo(CECIvtrFlySword* pFlySword);

protected:
	
	PAUIIMAGEPICTURE m_pImg_FlySwordIcon;
	PAUILABEL		 m_pLbl_FlySwordName;
	PAUILABEL		 m_pLbl_FlySwordImproveLevel;
	
	PAUIIMAGEPICTURE m_pImg_ConsumeItemIcon;
	PAUILABEL		 m_pLbl_ConsuemItemName;
	PAUILABEL		 m_pLbl_ConsuemItemAmount;
	PAUILABEL		 m_pLbl_GeneralSpeed;
	PAUILABEL		 m_pLbl_AcceleratedSpeed;

	PAUISTILLIMAGEBUTTON m_pBtn_Improve;

};
