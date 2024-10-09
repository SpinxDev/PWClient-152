// File		: DlgStoneChange.h 
// Creator	: Wang And Shi
// Date		: 2013/01/14

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h "
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "EC_Counter.h"
#include <vector.h>

class CECIvtrEquip;
struct STONE_CHANGE_RECIPE;

class CDlgStoneChange : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum
	{
		STONE_NUM = 4,		
	};

public:
	enum
	{
		EM_TRANSFER = 0x01,
		EM_REPLACE	= 0x02,
		EM_ALL		= 0xff,
	};

	typedef abase::vector<STONE_CHANGE_RECIPE*> RECIPES;

public:
	CDlgStoneChange();
	virtual ~CDlgStoneChange();

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandTransfer(const char * szCommand);
	void OnCommandReplace(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void SetType(int t) { m_Type = t;}
	int GetType() const { return m_Type;}
	bool IsWeapon ();
	int GetEquipSlot() const { return m_iEquipSlot;}
	int GetHoleIndex() const { return m_iSelHoleIndex;}
	int GetOldStoneTid() ;

	void OnStoneChangeEnd(int type,int equip_idx,int hole_idx);
	const RECIPES& GetRecipe(int stone_id);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

	void ClearEquip(PAUIIMAGEPICTURE pImage);
	void SetEquipStone(CECIvtrEquip* pSrcEquip);
	void ShowStoneControls(int stone_num,CECIvtrEquip* pEquip);

	void ClearGfx();
	void EnumStoneRecipe();
	bool CanTransfer(int stone_id);
	
protected:
	
	PAUIIMAGEPICTURE m_pItem;

	PAUIIMAGEPICTURE m_pStoneImg[STONE_NUM];
	PAUISTILLIMAGEBUTTON m_pTransferBtn[STONE_NUM];
	PAUISTILLIMAGEBUTTON m_pReplaceBtn[STONE_NUM];
	PAUILABEL	m_pStoneName[STONE_NUM];
	PAUIIMAGEPICTURE m_pStoneBackGround[STONE_NUM];
	PAUILABEL	m_pEquipName;

	int m_Type;

	int m_iEquipSlot;
	int m_iSelHoleIndex;

	int m_nOriginalHeight;

	CECCounter m_gfxCounter;

	abase::hash_map<int, RECIPES> m_stone_recipes;
};
