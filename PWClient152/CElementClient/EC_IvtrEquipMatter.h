/*
 * FILE: EC_IvtrEquipMatter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2006/8/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"
#include <vector.h>

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

struct BIBLE_ESSENCE;
struct SPEAKER_ESSENCE;
struct AUTOHP_ESSENCE;
struct AUTOMP_ESSENCE;
struct SELL_CERTIFICATE_ESSENCE;
struct FORCE_TOKEN_ESSENCE;
struct DYNSKILLEQUIP_ESSENCE;
struct POKER_ESSENCE;
struct POKER_SUB_TYPE;

class CECSkill;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrBible
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrBible : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrBible(int tid, int expire_date);
	CECIvtrBible(const CECIvtrBible& s);
	virtual ~CECIvtrBible();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrBible(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Add deadly strike rate provided by this equipment 
	//  By Sunxuewei 2008/9/3
	virtual int GetDeadlyStrikeRate(bool bSuiteGen);

	//	Get database data
	const BIBLE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	BIBLE_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrSpeaker
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrSpeaker : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrSpeaker(int tid, int expire_date);
	CECIvtrSpeaker(const CECIvtrSpeaker& s);
	virtual ~CECIvtrSpeaker();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrSpeaker(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get database data
	const SPEAKER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	SPEAKER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrAutoHP
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrAutoHP : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrAutoHP(int tid, int expire_date);
	CECIvtrAutoHP(const CECIvtrAutoHP& s);
	virtual ~CECIvtrAutoHP();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrAutoHP(*this); }
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get essence data
	const IVTR_ESSENCE_AUTOHP& GetEssence() { return m_Essence; }
	
	//	Get database data
	const AUTOHP_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes
	IVTR_ESSENCE_AUTOHP		m_Essence;

	//	Data in database
	AUTOHP_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrAutoMP
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrAutoMP : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrAutoMP(int tid, int expire_date);
	CECIvtrAutoMP(const CECIvtrAutoMP& s);
	virtual ~CECIvtrAutoMP();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrAutoMP(*this); }
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get essence data
	const IVTR_ESSENCE_AUTOMP& GetEssence() { return m_Essence; }
	
	//	Get database data
	const AUTOMP_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes
	IVTR_ESSENCE_AUTOMP		m_Essence;

	//	Data in database
	AUTOMP_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrCertificate
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrCertificate : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrCertificate(int tid, int expire_date);
	CECIvtrCertificate(const CECIvtrCertificate& s);
	virtual ~CECIvtrCertificate();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrCertificate(*this); }
	//  Get booth model id
	const char * GetBoothModelID();
	//	»ñÈ¡°ÚÌ¯ÕÐÅÆÍ¼Æ¬
	const char* GetBoothBarFile();

	int GetSellColNum()			{ return m_iSellColNum; }
	int GetBuyColNum()			{ return m_iBuyColNum; }
	int GetBoothNameLength()	{ return m_iBoothNameLength; }

	//	Get database data
	const SELL_CERTIFICATE_ESSENCE* GetDBEssence() { return m_pDBEssence; }
protected:	//	Attributes
	//	Data in database
	SELL_CERTIFICATE_ESSENCE*		m_pDBEssence;

	int		m_iSellColNum;			// Sell column number
	int		m_iBuyColNum;			// Buy column number
	int		m_iBoothNameLength;		// Booth name length

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrForceToken
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrForceToken : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrForceToken(int tid, int expire_date);
	CECIvtrForceToken(const CECIvtrForceToken& s);
	virtual ~CECIvtrForceToken();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrForceToken(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get essence data
	const IVTR_ESSENCE_FORCE_TOKEN& GetEssence() { return m_Essence; }

	//	Get database data
	const FORCE_TOKEN_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes
	IVTR_ESSENCE_FORCE_TOKEN		m_Essence;

	//	Data in database
	FORCE_TOKEN_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrDynSkillEquip
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrDynSkillEquip : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDynSkillEquip(int tid, int expire_date);
	CECIvtrDynSkillEquip(const CECIvtrDynSkillEquip& s);
	virtual ~CECIvtrDynSkillEquip();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrDynSkillEquip(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const DYNSKILLEQUIP_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	int GetSkillCount()const{ return (int)m_pSkills.size(); }
	CECSkill * GetSkill(int index)const{ return m_pSkills[index]; }

protected:	//	Attributes

	//	Data in database
	DYNSKILLEQUIP_ESSENCE*	m_pDBEssence;
	abase::vector<CECSkill*>	m_pSkills;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
	void ClearSkills();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGeneralCard
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrGeneralCard : public CECIvtrEquip
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrGeneralCard(int tid, int expire_date);
	CECIvtrGeneralCard(const CECIvtrGeneralCard& s);
	virtual ~CECIvtrGeneralCard();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGeneralCard(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	virtual int GetEquipmentType() const { return EQUIP_GENERALCARD; }

	const IVTR_ESSENCE_GENERALCARD& GetEssence() { return m_Essence; }
	
	//	Get database data
	const POKER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	int	CalcGeneralCardProp(int prop_base, int prop_grow);
	float GetProfessionRatio();
	float GetSuiteRatio();
	float GetRebirthRatio();
	float GetRankRatio();

	int GetSwallowExp();
	int GetLevelUpExp(int level);
	
protected:	//	Attributes

	IVTR_ESSENCE_GENERALCARD m_Essence;
	
	//	Data in database
	POKER_ESSENCE*	m_pDBEssence;
	POKER_SUB_TYPE*	m_pDBSubEssence;
	
protected:	//	Operations
	
	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
	//  Ìí¼Ó¿¨ÅÆÌ××°ÃèÊö
	virtual void AddSuiteDesc();
	virtual void SetLocalProps() {};
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

