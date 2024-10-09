/*
 * FILE: EC_ShortcutSet.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECShortcut;
class CECIvtrItem;
class CECSkill;
struct SkillArrayWrapper;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECShortcutSet
//	
///////////////////////////////////////////////////////////////////////////

class CECShortcutSet
{
public:		//	Types

public:		//	Constructor and Destructor

	CECShortcutSet();
	virtual ~CECShortcutSet();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	bool Init(int iSize);
	//	Release object
	void Release();

	//	Save shortcut configs to specified buffer
	bool SaveConfigData(void* pDataBuf, int* piSize);
	//	Load shortcut configs from specified buffer
	bool LoadConfigData(const void* pDataBuf, int* piSize, DWORD dwVer);

	//	Create a item shortcut at specified position
	bool CreateItemShortcut(int iSlot, int iIvtr, int iIvtrSlot, CECIvtrItem* pItem);
	//	Create a skill shortcut at specified position
	bool CreateSkillShortcut(int iSlot, CECSkill* pSkill);
	//	Create a skill group shortcut at specified position
	bool CreateSkillGroupShortcut(int iSlot, int iGroupIdx);
	//	Create a pet shortcut at specified position
	bool CreatePetShortcut(int iSlot, int iPetIndex);
	//	Create a auto fashion shortcut at specified position
	bool CreateAutoFashionShortcut(int iSlot, int iFashionIdx);
	//	Duplicate a shortcut to specified position
	bool CreateClonedShortcut(int iSlot, CECShortcut* pShortcut);
	//	Create a system module shortcut at specified position
	bool CreateSystemModuleShortcut(int iSlot, int iSys);
	
	//	Put a shortcut into set
	CECShortcut* PutShortcut(int iSlot, CECShortcut* pShortcut);
	//	Get a shortcut from set
	CECShortcut* GetShortcut(int iSlot, bool bRemove=false);
	//	Set a shortcut into set and release old shortcut at this position automatically
	void SetShortcut(int iSlot, CECShortcut* pShortcut);
	//	Exchange shortcut in inventory
	void ExchangeShortcut(int iSlot1, int iSlot2);
	//	Remove all shortcuts
	void RemoveAllShortcuts();
	//	Remove skill and skill group shortcuts
	void RemoveSkillShortcuts();

	//	Update item shortcut when item position changed
	void UpdateMovedItemSC(int tidItem, int iSrcIvtr, int iSrcSlot, int iDstIvtr, int iDstSlot);
	//	Update item shortcut when item removed
	void UpdateRemovedItemSC(int tidItem, int iIvtr, int iSlot, int iSameItem);
	//	Update item shortcut when two items exchanged
	void UpdateExchangedItemSC(int tidItem1, int iIvtr1, int iSlot1, int tidItem2, int iIvtr2, int iSlot2);
	//	Update pet shortcut when pet freeed
	void UpdateFreedPetSC(int iPetIndex);

	//	Replace skill id in skill shortcuts
	void ReplaceSkillID(int idOld, CECSkill* pNewSkill);
	void ReplaceSkillID(const SkillArrayWrapper &skillArray, CECSkill* pNewSkill);

	//	删除引用技能的所有快捷方式
	bool	RemoveSkillShortcut(int idSkill);

	//	Get shortcut number
	int GetShortcutNum() { return m_aShortcuts.GetSize(); }

protected:	//	Attributes

	APtrArray<CECShortcut*>	m_aShortcuts;	//	Shortcut array

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



