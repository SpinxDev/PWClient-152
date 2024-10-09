/*
 * FILE: EC_Shortcut.h
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

#include "AString.h"
#include "AWString.h"

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

class CECIvtrItem;
class CECSkill;
struct PET_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECShortcut
//	
///////////////////////////////////////////////////////////////////////////

class CECShortcut
{
public:		//	Types

	//	Shortcut type
	enum
	{
		SCT_UNKNOWN = -1,
		SCT_SKILL = 0,
		SCT_ITEM,
		SCT_COMMAND,
		SCT_SKILLGRP,
		SCT_PET,
		SCT_AUTOFASHION,
		SCT_SYSMODULE,
	};

public:		//	Constructor and Destructor

	CECShortcut() { m_iSCType = SCT_UNKNOWN; }
	CECShortcut(const CECShortcut& src);
	virtual ~CECShortcut() {}

public:		//	Attributes

public:		//	Operations

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute() { return true; }
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc() { return NULL; }
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL) { return 0; }

	//	Get shortcut type
	int GetType() { return m_iSCType; }

protected:	//	Attributes

	int		m_iSCType;	//	Shortcut type

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCSkill
//	
///////////////////////////////////////////////////////////////////////////

class CECSCSkill : public CECShortcut
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSCSkill();
	CECSCSkill(const CECSCSkill& src);
	virtual ~CECSCSkill();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(CECSkill* pSkill);

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);

	//	Get skill object
	CECSkill* GetSkill() { return m_pSkill; }
	//	Replace skill object
	void SetSkill(CECSkill* pSkill) { m_pSkill = pSkill; }

protected:	//	Attributes

	CECSkill*	m_pSkill;	//	Skill object

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCItem
//	
///////////////////////////////////////////////////////////////////////////

class CECSCItem : public CECShortcut
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSCItem();
	CECSCItem(const CECSCItem& src);
	virtual ~CECSCItem();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iIvtr, int iSlot, CECIvtrItem* pItem);

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile() { return m_strIconFile; }
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);

	//	Get inventory index
	int GetInventory() { return m_iIvtr; }
	//	Get slot index
	int GetIvtrSlot() { return m_iSlot; }
	//	Get item's template ID
	int GetItemTID() { return m_tidItem; }
	//	Set new position of item referenced by this shortcut
	void MoveItem(int iIvtr, int iSlot);
	//	Get auto find flag
	bool GetAutoFindFlag() { return m_bAutoFind; }

protected:	//	Attributes

	int			m_iIvtr;		//	Inventory index
	int			m_iSlot;		//	Slot in inventory
	int			m_tidItem;		//	Item's template ID
	AString		m_strIconFile;	//	Icon file name
	bool		m_bAutoFind;	//	Auto find flag

protected:	//	Operations

	//	Get item object
	CECIvtrItem* GetItem();
	//	Update item associated data after m_iIvtr or m_iSlot changed
	void UpdateItemData();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCCommand
//	
///////////////////////////////////////////////////////////////////////////

class CECSCCommand : public CECShortcut
{
public:		//	Types

	//	Command ID
	enum
	{
		CMD_UNKNOWN = -1,
		CMD_SITDOWN = 0,
		CMD_WALKRUN,
		CMD_NORMALATTACK,
		CMD_FINDTARGET,
		CMD_ASSISTATTACK,
		CMD_INVITETOTEAM,
		CMD_LEAVETEAM,	
		CMD_KICKTEAMMEM,
		CMD_FINDTEAM,
		CMD_STARTTRADE,
		CMD_SELLBOOTH,
		CMD_BUYBOOTH,
		CMD_PLAYPOSE,
		CMD_INVITETOFACTION,
		CMD_FLY,
		CMD_PICKUP,
		CMD_GATHER,
		CMD_RUSHFLY,
		CMD_BINDBUDDY,
	};

public:		//	Constructor and Destructor

	CECSCCommand(int iCommand);
	CECSCCommand(const CECSCCommand& src);
	virtual ~CECSCCommand();

public:		//	Attributes

public:		//	Operations

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);

	//	Set / Get command parameter
	void SetParam(DWORD dwParam) { m_dwParam = dwParam; }
	DWORD GetParam() { return m_dwParam; }
	//	Get command ID
	int	GetCommandID() { return m_iCommand; }

protected:	//	Attributes

	int		m_iCommand;		//	Command ID
	DWORD	m_dwParam;		//	Parameter

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCSkillGrp
//	
///////////////////////////////////////////////////////////////////////////

class CECSCSkillGrp : public CECShortcut
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSCSkillGrp();
	CECSCSkillGrp(const CECSCSkillGrp& src);
	virtual ~CECSCSkillGrp() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iGroupIdx);

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);

	//	Get group index
	int GetGroupIndex() { return m_iGroupIdx; }

protected:	//	Attributes

	int			m_iGroupIdx;	//	Group index
	AWString	m_strDesc;		//	Description

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCPet
//	
///////////////////////////////////////////////////////////////////////////

class CECSCPet : public CECShortcut
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSCPet();
	CECSCPet(const CECSCPet& src);
	virtual ~CECSCPet() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iPetIndex);

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();

	//	This pet is the active one ?
	bool IsActivePet();

	//	Get pet index in pet corral
	int GetPetIndex() { return m_iPetIndex; }
	//	Get pet essence
	const PET_ESSENCE* GetPetEssence() { return m_pPetEssence; }

protected:	//	Attributes

	int				m_iPetIndex;	//	Pet index
	PET_ESSENCE*	m_pPetEssence;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCAutoFashion
//	
///////////////////////////////////////////////////////////////////////////

class CECSCAutoFashion : public CECShortcut
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSCAutoFashion();
	CECSCAutoFashion(const CECSCAutoFashion& src);
	virtual ~CECSCAutoFashion() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iFashionIdx);

	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	Get icon file
	virtual const char* GetIconFile();
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	
	//	Get auto fashion index in auto fashion package
	int GetAutoFashionIndex() { return m_iFashionIdx; }

protected:	//	Attributes
	int		m_iFashionIdx;
	char	m_IconFile[256];

protected:	//	Operations

};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSCAutoFashion
//	
///////////////////////////////////////////////////////////////////////////

class CECSCSysModule : public CECShortcut
{
public:		//	Types
	enum
	{
		FM_NONE = -1,
		FM_GT,
		FM_TOUCH,
		FM_ROBOT, // 修炼系统
		FM_WIKI,
		FM_OFFLINESHOP,// 寄卖店铺
		FM_BORADCAST, // 完美服务
		FM_MATCH,// 情缘系统
		FM_ADDEXP, // 聚灵系统
		FM_AUTOHPMP, // 辅助功能，自动喝药

		FM_NUM,
	};

public:		//	Constructor and Destructor
	
	CECSCSysModule();
	CECSCSysModule(const CECSCSysModule& src);
	virtual ~CECSCSysModule() {}
	
public:		//	Attributes
	static const int g_SysIndexMap[FM_NUM];
public:		//	Operations
	
	//	Initialize object
	bool Init(int iSys);
	
	//	Clone shortcut
	virtual CECShortcut* Clone();
	//	Execute shortcut
	virtual bool Execute();
	//	
	int GetSysModID() const { return m_iSystem; }
	//	Get shortcut description text
	virtual const wchar_t* GetDesc();
	
protected:	//	Attributes
	ACString m_strDesc;
	char m_IconFile[256];

	int		m_iSystem;		//	system ID

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



