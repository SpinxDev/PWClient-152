// Filename	: DlgDragDrop.h
// Creator	: Tom Zhou
// Date		: October 20, 2005

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CECIvtrItem;
class CECSCSysModule;

class CDlgDragDrop : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgDragDrop();
	virtual ~CDlgDragDrop();

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	virtual void SetCapture(bool bCapture, int x = -1, int y = -1);
	
	POINT GetDragPos() const { return m_ptDragPos; }
	PAUIIMAGEPICTURE GetImgGood() { return m_pImgGoods; }

protected:
	POINT m_ptDragPos;
	PAUIIMAGEPICTURE m_pImgGoods;

	virtual bool OnInitDialog();

public:
	void OnDragClick(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver);
	void OnGeneralScene(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnTradeScene(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);

	//----------------------------------------------------------------
	// Handle drag-drop operation from these dialog
	//----------------------------------------------------------------
	void OnInventoryTradeDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnInventoryDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);

	void OnStorageDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnStorageDragDropToNormalPack(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnStorageDragDropInSameStorage(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);

	void OnTradeDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	
	void OnActionDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnSkillEditDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnSkillDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnSysModDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);

	void OnElfDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnQuickBarDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnShopDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnSysModQuickBarDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);

	void OnChangeSkirtDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnPShop2DragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnPetListDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	void OnEPEquipDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc);
	//----------------------------------------------------------------
};
