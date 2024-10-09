/*
 * FILE: DlgOfflineShopCreate.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"
#include "EC_OfflineShopCtrl.h"
#include <AUIImagePicture.h>
#include <AUIStillImageButton.h>
#include <AUICheckBox.h>
#include <AUILabel.h>

namespace GNET{
	class Protocol;
}

class CDlgOfflineShopCreate : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgOfflineShopCreate();
	~CDlgOfflineShopCreate();
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);	
	void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);	
	void OnCommandConfirm(const char *szCommand);
	void OnCommandCANCEL(const char *szCommand);
	void OnCommandBtn2(const char *szCommand); // 取消，或者改类型

	void SetDlgType(bool bCreate) { m_bDlgCreate = bCreate;}

	void SetShopType(int type) { m_shop_type = type;}
	
protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();
	virtual void DoDataExchange(bool bSave);

protected:
	int m_shop_type;
	PAUIIMAGEPICTURE m_pImg_Item; 
	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;
	PAUISTILLIMAGEBUTTON m_pBtn_Cancel;
	PAUILABEL m_pLbl_Title;
	bool m_bDlgCreate; // true: create shop, false: active and modify shop type 
};

//////////////////////////////////////////////////////////////////////////

class CDlgOfflineShopType : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgOfflineShopType();
	~CDlgOfflineShopType(){}

	void SetShopTypeMask(int mask) { m_typeMask = mask;}
	int GetShopTypeMask() const { return m_typeMask;}

	void OnCommandConfirm(const char *szCommand);

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();

protected:
	int m_typeMask;

	PAUICHECKBOX m_pChk_Type[COfflineShopCtrl::OST_NUM];
};

//////////////////////////////////////////////////////////////////////////
