// Filename	: DlgRandProp.h
// Creator	: Feng Ning
// Date		: 2011/06/15

#pragma once

#include "DlgBase.h"
#include "EC_IvtrEquip.h"
#include <map>

#define CDLGRANDPROP_MAX_MATERIALS 8

class CECIvtrItem;

class CDlgRandProp : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgRandProp();
	virtual ~CDlgRandProp();

	void OnCommand_set(const char * szCommand);
	void OnCommand_begin(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonUp_Target(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetTarget(int iSlot);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	bool OnRandPropAction(int idAction, void* pData);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	virtual void OnChangeLayoutEnd(bool bAllDone);

	int m_nCurShopSet;
	int m_nCurIcon;

	bool SaveProps(CECIvtrItem* pItem);
	bool CheckTargetValid(CECIvtrItem* pItem);
	bool SelectIcon(int page, int itemID);

	void RefreshAll();
	// return how many valid button
	int RefreshButtonName();
	void RefreshShopSet(int nSet, int nIcon);
	void RefreshContent(int page);
	// return whether have enough material
	bool RefreshMaterial(int recipeID);

	int GetCurRecipe();
	bool CanBegin();
	bool IsWorking();

	int m_nSoltMaterail[CDLGRANDPROP_MAX_MATERIALS];

	typedef std::map<int, CECIvtrEquip::PROPERTY> PropMap;
	PropMap m_OriginProps;

	int m_RegenCount;
	ACString m_RegenDesc;
	ACString m_OriginDesc;
	ACString m_PropsDesc;
};