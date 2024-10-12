/*
 * FILE: EC_IvtrConsume.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrEquip.h"
#include "../CElementClient/EC_IvtrTypes.h"

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

struct ARMORRUNE_SUB_TYPE;
struct ARMORRUNE_ESSENCE;
struct DAMAGERUNE_ESSENCE;
struct DAMAGERUNE_SUB_TYPE;
struct MEDICINE_MAJOR_TYPE;
struct MEDICINE_SUB_TYPE;
struct MEDICINE_ESSENCE;
struct ELEMENT_ESSENCE;
struct TOSSMATTER_ESSENCE;
struct FIREWORKS_ESSENCE;
struct SKILLMATTER_ESSENCE;
struct INC_SKILL_ABILITY_ESSENCE;
struct DOUBLE_EXP_ESSENCE;
struct TRANSMITSCROLL_ESSENCE;
struct DYE_TICKET_ESSENCE;
struct TARGET_ITEM_ESSENCE;
struct LOOK_INFO_ESSENCE;
struct WEDDING_BOOKCARD_ESSENCE;
struct WEDDING_INVITECARD_ESSENCE;
struct SHARPENER_ESSENCE;
struct FACTION_MATERIAL_ESSENCE;
struct CONGREGATE_ESSENCE;
struct MONSTER_SPIRIT_ESSENCE;
struct POKER_DICE_ESSENCE;
struct UNIVERSAL_TOKEN_ESSENCE;
class CECSkill;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrArmorRune
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrArmorRune : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrArmorRune(int tid, int expire_date);
	CECIvtrArmorRune(const CECIvtrArmorRune& s);
	virtual ~CECIvtrArmorRune();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrArmorRune(*this); }
	//	Get item cool time
	//virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const ARMORRUNE_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	const ARMORRUNE_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }

protected:	//	Attributes

	//	Data in database
	ARMORRUNE_ESSENCE*		m_pDBEssence;
	ARMORRUNE_SUB_TYPE*		m_pDBSubType;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrDmgRune
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrDmgRune : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDmgRune(int tid, int expire_date);
	CECIvtrDmgRune(const CECIvtrDmgRune& s);
	virtual ~CECIvtrDmgRune();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrDmgRune(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const DAMAGERUNE_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	const DAMAGERUNE_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }

protected:	//	Attributes

	//	Data in database
	DAMAGERUNE_ESSENCE*		m_pDBEssence;
	DAMAGERUNE_SUB_TYPE*	m_pDBSubType;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrMedicine
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrMedicine : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrMedicine(int tid, int expire_date);
	CECIvtrMedicine(const CECIvtrMedicine& s);
	virtual ~CECIvtrMedicine();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrMedicine(*this); }
	//	Get item cool time
	//virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const MEDICINE_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const MEDICINE_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const MEDICINE_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	int GetLevelRequirement() const { return m_iLevelReq; }

protected:	//	Operations

	//	Data in database
	MEDICINE_MAJOR_TYPE*	m_pDBMajorType;
	MEDICINE_SUB_TYPE*		m_pDBSubType;
	MEDICINE_ESSENCE*		m_pDBEssence;

	int						m_iLevelReq;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);

	//	Build medicine effect description
	void BuildEffectDesc();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrElement
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrElement : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrElement(int tid, int expire_date);
	CECIvtrElement(const CECIvtrElement& s);
	virtual ~CECIvtrElement();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrElement(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const ELEMENT_ESSENCE* GetDBEssence() const { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	ELEMENT_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTossMat
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTossMat : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTossMat(int tid, int expire_date);
	CECIvtrTossMat(const CECIvtrTossMat& s);
	virtual ~CECIvtrTossMat();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTossMat(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const TOSSMATTER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TOSSMATTER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFirework
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFirework : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFirework(int tid, int expire_date);
	CECIvtrFirework(const CECIvtrFirework& s);
	virtual ~CECIvtrFirework();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrFirework(*this); }
	//	Get item cool time
	//virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const FIREWORKS_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	FIREWORKS_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrSkillMat
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrSkillMat : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrSkillMat(int tid, int expire_date);
	CECIvtrSkillMat(const CECIvtrSkillMat& s);
	virtual ~CECIvtrSkillMat();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrSkillMat(*this); }
	//	Get item cool time
	//virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const SKILLMATTER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	SKILLMATTER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrSkillMat
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrIncSkillAbility : public CECIvtrItem
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrIncSkillAbility(int tid, int expire_date);
	CECIvtrIncSkillAbility(const CECIvtrIncSkillAbility& s);
	virtual ~CECIvtrIncSkillAbility();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrIncSkillAbility(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	
	const INC_SKILL_ABILITY_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	
protected:	//	Attributes
	
	//	Data in database
	INC_SKILL_ABILITY_ESSENCE*	m_pDBEssence;
	
protected:	//	Operations
	
	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrDoubleExp
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrDoubleExp : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDoubleExp(int tid, int expire_date);
	CECIvtrDoubleExp(const CECIvtrDoubleExp& s);
	virtual ~CECIvtrDoubleExp();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrDoubleExp(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const DOUBLE_EXP_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	DOUBLE_EXP_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrDyeTicket
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrDyeTicket : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDyeTicket(int tid, int expire_date);
	CECIvtrDyeTicket(const CECIvtrDyeTicket& s);
	virtual ~CECIvtrDyeTicket();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrDyeTicket(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const DYE_TICKET_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	DYE_TICKET_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTransmitScroll
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTransmitScroll : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTransmitScroll(int tid, int expire_date);
	CECIvtrTransmitScroll(const CECIvtrTransmitScroll& s);
	virtual ~CECIvtrTransmitScroll();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTransmitScroll(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();

	const TRANSMITSCROLL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TRANSMITSCROLL_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTargetItem
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTargetItem : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTargetItem(int tid, int expire_date);
	CECIvtrTargetItem(const CECIvtrTargetItem& s);
	virtual ~CECIvtrTargetItem();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTargetItem(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();

	const TARGET_ITEM_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	CECSkill* GetTargetSkill() { return m_pTargetSkill; }

protected:	//	Attributes

	//	Data in database
	TARGET_ITEM_ESSENCE*		m_pDBEssence;
	
	CECSkill* m_pTargetSkill;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrLookInfoItem
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrLookInfoItem : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrLookInfoItem(int tid, int expire_date);
	CECIvtrLookInfoItem(const CECIvtrLookInfoItem& s);
	virtual ~CECIvtrLookInfoItem();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrLookInfoItem(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);

	const LOOK_INFO_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	LOOK_INFO_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrWeddingBookCard
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrWeddingBookCard : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrWeddingBookCard(int tid, int expire_date);
	CECIvtrWeddingBookCard(const CECIvtrWeddingBookCard& s);
	virtual ~CECIvtrWeddingBookCard();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrWeddingBookCard(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const WEDDING_BOOKCARD_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	const IVTR_ESSENCE_WEDDING_BOOKCARD & GetEssence(){ return m_Essence; }

protected:	//	Attributes
	IVTR_ESSENCE_WEDDING_BOOKCARD	m_Essence;

	//	Data in database
	WEDDING_BOOKCARD_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrWeddingInviteCard
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrWeddingInviteCard : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrWeddingInviteCard(int tid, int expire_date);
	CECIvtrWeddingInviteCard(const CECIvtrWeddingInviteCard& s);
	virtual ~CECIvtrWeddingInviteCard();

public:		//	Attributes

	enum {WEDDING_CONFIG_ID = 801};

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrWeddingInviteCard(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	请柬当前是否可使用
	//virtual bool CheckUseCondition();

	const WEDDING_INVITECARD_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	const IVTR_ESSENCE_WEDDING_INVITECARD & GetEssence() { return m_Essence; }
protected:	//	Attributes
	IVTR_ESSENCE_WEDDING_INVITECARD	m_Essence;

	//	Data in database
	WEDDING_INVITECARD_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrShapener
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrShapener : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrShapener(int tid, int expire_date);
	CECIvtrShapener(const CECIvtrShapener& s);
	virtual ~CECIvtrShapener();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrShapener(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	
	bool CanSharpenSlot(int iSlot) const;
	
	const SHARPENER_ESSENCE* GetDBEssence() const { return m_pDBEssence; }
	
	int GetLevel()const;
	
	virtual int GetItemLevel()const;
protected:	//	Attributes

	//	Data in database
	SHARPENER_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);

	void AddSharpenerDesc();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFactionMaterial
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFactionMaterial : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFactionMaterial(int tid, int expire_date);
	CECIvtrFactionMaterial(const CECIvtrFactionMaterial& s);
	virtual ~CECIvtrFactionMaterial();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrFactionMaterial(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
		
	const FACTION_MATERIAL_ESSENCE* GetDBEssence() { return m_pDBEssence; }
		
protected:	//	Attributes

	//	Data in database
	FACTION_MATERIAL_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrCongregate
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrCongregate : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrCongregate(int tid, int expire_date);
	CECIvtrCongregate(const CECIvtrCongregate& s);
	virtual ~CECIvtrCongregate();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get item cool time
	//virtual int GetCoolTime(int* piMax=NULL);
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrCongregate(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Check item use condition
	//virtual bool CheckUseCondition();

	const CONGREGATE_ESSENCE* GetDBEssence() { return m_pDBEssence; }	
	
protected:	//	Attributes

	//	Data in database
	CONGREGATE_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrMonsterSpirit
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrMonsterSpirit : public CECIvtrItem
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrMonsterSpirit(int tid, int expire_date);
	CECIvtrMonsterSpirit(const CECIvtrMonsterSpirit& s);
	virtual ~CECIvtrMonsterSpirit();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrMonsterSpirit(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const IVTR_ESSENCE_MONSTERSPIRIT& GetEssence() { return m_Essence; }
	
	const MONSTER_SPIRIT_ESSENCE* GetDBEssence() { return m_pDBEssence; }	
	
protected:	//	Attributes
	
	IVTR_ESSENCE_MONSTERSPIRIT m_Essence;
	//	Data in database
	MONSTER_SPIRIT_ESSENCE*		m_pDBEssence;
	
protected:	//	Operations
	
	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGeneralCardDice
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrGeneralCardDice : public CECIvtrItem
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrGeneralCardDice(int tid, int expire_date);
	CECIvtrGeneralCardDice(const CECIvtrGeneralCardDice& s);
	virtual ~CECIvtrGeneralCardDice();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGeneralCardDice(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	
	const POKER_DICE_ESSENCE* GetDBEssence() { return m_pDBEssence; }	
	
protected:	//	Attributes
	
	//	Data in database
	POKER_DICE_ESSENCE*		m_pDBEssence;
	
protected:	//	Operations
	
	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrUniversalToken
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrUniversalToken : public CECIvtrItem
{
public:	
	CECIvtrUniversalToken(int tid, int expire_date);
	CECIvtrUniversalToken(const CECIvtrUniversalToken& s);
	virtual ~CECIvtrUniversalToken();

public:
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	virtual const char* GetIconFile();
	virtual const wchar_t* GetName();
	virtual CECIvtrItem* Clone() { return new CECIvtrUniversalToken(*this); }
	virtual const char * GetDropModel();	
	const UNIVERSAL_TOKEN_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	bool HasAnyUsage()const;
	bool HasUsage(unsigned int usageMask)const;
	int	 ConfigIDFor(int usageIndex)const;
	int	 UsageCount()const;
	int	 UsageIndexAt(int index)const;
	static int MaskToUsageIndex(unsigned int usageMask);
	static unsigned int UsageIndexToMask(int usageIndex);
	
protected:
	UNIVERSAL_TOKEN_ESSENCE*	m_pDBEssence;
	
protected:
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

