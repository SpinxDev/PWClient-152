/*
 * FILE: EC_IvtrSkillTome.h
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

#include "EC_IvtrItem.h"
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

struct SKILLTOME_ESSENCE;
struct SKILLTOME_SUB_TYPE;
struct TOWNSCROLL_ESSENCE;
struct REVIVESCROLL_ESSENCE;
struct UNIONSCROLL_ESSENCE;
struct FACETICKET_MAJOR_TYPE;
struct FACETICKET_SUB_TYPE;
struct FACETICKET_ESSENCE;
struct FACEPILL_MAJOR_TYPE;
struct FACEPILL_SUB_TYPE;
struct FACEPILL_ESSENCE;
struct GM_GENERATOR_TYPE;
struct GM_GENERATOR_ESSENCE;
struct WAR_TANKCALLIN_ESSENCE;
struct SHOP_TOKEN_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrSkillTome
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrSkillTome : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrSkillTome(int tid, int expire_date);
	CECIvtrSkillTome(const CECIvtrSkillTome& s);
	virtual ~CECIvtrSkillTome();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrSkillTome(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const SKILLTOME_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	const SKILLTOME_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }

protected:	//	Attributes

	//	Data in database
	SKILLTOME_ESSENCE*		m_pDBEssence;
	SKILLTOME_SUB_TYPE*		m_pDBSubType;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTownScroll
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTownScroll : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTownScroll(int tid, int expire_date);
	CECIvtrTownScroll(const CECIvtrTownScroll& s);
	virtual ~CECIvtrTownScroll();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTownScroll(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const TOWNSCROLL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TOWNSCROLL_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrRevScroll
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrRevScroll : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrRevScroll(int tid, int expire_date);
	CECIvtrRevScroll(const CECIvtrRevScroll& s);
	virtual ~CECIvtrRevScroll();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrRevScroll(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const REVIVESCROLL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	REVIVESCROLL_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrUnionScroll
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrUnionScroll : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrUnionScroll(int tid, int expire_date);
	CECIvtrUnionScroll(const CECIvtrUnionScroll& s);
	virtual ~CECIvtrUnionScroll();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrUnionScroll(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const UNIONSCROLL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	UNIONSCROLL_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFaceTicket
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFaceTicket : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFaceTicket(int tid, int expire_date);
	CECIvtrFaceTicket(const CECIvtrFaceTicket& s);
	virtual ~CECIvtrFaceTicket();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrFaceTicket(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const FACETICKET_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const FACETICKET_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const FACETICKET_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	int GetLevelRequirement() const { return m_iLevelReq; }

protected:	//	Attributes

	//	Data in database
	FACETICKET_MAJOR_TYPE*	m_pDBMajorType;
	FACETICKET_SUB_TYPE*	m_pDBSubType;
	FACETICKET_ESSENCE*		m_pDBEssence;

	int						m_iLevelReq;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFacePill
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFacePill : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFacePill(int tid, int expire_date);
	CECIvtrFacePill(const CECIvtrFacePill& s);
	virtual ~CECIvtrFacePill();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrFacePill(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get pill file name
	const char* GetPillFile(int iProfession, int iGender);

	//	Get database data
	const FACEPILL_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const FACEPILL_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const FACEPILL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	FACEPILL_MAJOR_TYPE*	m_pDBMajorType;
	FACEPILL_SUB_TYPE*		m_pDBSubType;
	FACEPILL_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGMGenerator
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrGMGenerator : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrGMGenerator(int tid, int expire_date);
	CECIvtrGMGenerator(const CECIvtrGMGenerator& s);
	virtual ~CECIvtrGMGenerator();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGMGenerator(*this); }
	//	Get item cool time
// 	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const GM_GENERATOR_TYPE* GetDBMajorType() { return m_pDBType; }
	const GM_GENERATOR_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	GM_GENERATOR_TYPE*		m_pDBType;
	GM_GENERATOR_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTankCallin
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTankCallin : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTankCallin(int tid, int expire_date);
	CECIvtrTankCallin(const CECIvtrTankCallin& s);
	virtual ~CECIvtrTankCallin();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTankCallin(*this); }
	//	Check item use condition
// 	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const WAR_TANKCALLIN_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Operations

	//	Data in database
	WAR_TANKCALLIN_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrShopToken
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrShopToken : public CECIvtrItem
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrShopToken(int tid, int expire_date);
	CECIvtrShopToken(const CECIvtrShopToken& s);
	virtual ~CECIvtrShopToken();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrShopToken(*this); }
	//	Check item use condition
	//virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	
	//	Get database data
	const SHOP_TOKEN_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	
protected:	//	Operations
	
	//	Data in database
	SHOP_TOKEN_ESSENCE*	m_pDBEssence;
	
protected:	//	Operations
	
	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


