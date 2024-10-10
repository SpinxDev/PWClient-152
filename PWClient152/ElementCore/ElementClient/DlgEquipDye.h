// File		: DlgEquipDye.h
// Creator	: Xiao Zhou
// Date		: 2007/4/9

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CECIvtrEquip;
class CECIvtrDyeTicket;

class CDlgEquipDye : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgEquipDye();
	virtual ~CDlgEquipDye();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	void OnCommandShowAll(const char *szCommand);
	void OnCommandShowNormal(const char *szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetEquip(CECIvtrItem *pItem, int nSlot, int nImageIndex = 0);
	void SetDye(CECIvtrItem *pItem, int nSlot);
	void OnEquipChange(CECIvtrItem *pItem, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	enum {MaxImgCount = 4};

	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUISTILLIMAGEBUTTON    m_pBtn_ShowNormal;
	PAUISTILLIMAGEBUTTON    m_pBtn_ShowAll;
	PAUIIMAGEPICTURE		m_pImg_Item[MaxImgCount];
	PAUIIMAGEPICTURE		m_pImg_Dye;
	PAUIOBJECT				m_pTxt_Num;
	PAUIOBJECT				m_pTxt_Dye;
	PAUIOBJECT				m_pTxt_Name;
	PAUILABEL				m_pTxt_Color[MaxImgCount];
	int						m_nSolt[MaxImgCount];
	int						m_nSoltDye;
	int						m_nRequireNum;

private:
	bool IsDlgEquipDye();
	bool IsDlgAllEquipDye();

	void ShowDlgEquipDye();
	void ShowDlgAllEquipDye();
	void SwitchDialog(CDlgEquipDye *pDlgToShow, CDlgEquipDye *pDlgToHide);

	CECIvtrItem * GetItemFromPack(int nSlot);
	CECIvtrDyeTicket * GetDyeItem();
	CECIvtrEquip * GetEquipItem(int index);

	int	GetDyeItemID();
	void SetDyeItemID(int tid);
	int GetEquipItemID(int index);
	void SetEquipItemID(int index, int tid);

	void ClearDialog();
	bool GetRequireDyeCount(CECIvtrItem *pItem, int &count);
	int  GetDyeCount();
	void UpdateRequireNum();
	void UpdateNumDisplay();
};
