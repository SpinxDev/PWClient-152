// File		: DlgStoneTransfer.h 
// Creator	: Wang and Shi
// Date		: 2013/01/14

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include <vector.h>

class CECIvtrStone;
struct STONE_CHANGE_RECIPE;

class CDlgStoneTransfer : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum
	{
		RECIPE_NUM = 4,
	};

	enum
	{
		MATERIAL_NUM = 8,
	};

public:
	CDlgStoneTransfer();
	virtual ~CDlgStoneTransfer();

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandTransfer(const char * szCommand);
	void OnEventLButtonDown_Stone(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Material(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void DoTransfer();

protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	void EnumRecipe(unsigned int oldStone_tid);
	void SelectRecipe(int index);
	void SetMaterial(int iSrc,int iDst);

	void ClearMaterial();
	void UpdateMaterial();

protected:
	PAUIIMAGEPICTURE m_pStoneImg[RECIPE_NUM];
	PAUIIMAGEPICTURE m_pMaterialImg[MATERIAL_NUM];
	PAUILABEL m_pStoneName;
	PAUILABEL m_pStoneAttribute;
	PAUIEDITBOX m_pMoney;
	PAUISTILLIMAGEBUTTON m_pTranferBtn;

	STONE_CHANGE_RECIPE* m_recipes[RECIPE_NUM];
	CECIvtrStone* m_pNewStones[RECIPE_NUM];

	int m_iNewStoneIndex;

	int m_iMaterailSlot[MATERIAL_NUM];
};
