/*
 * FILE: EC_IvtrEquip.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrItem.h"
#include "AArray.h"
#include "AAssist.h"
#include <A3DTypes.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrEquip
//	
///////////////////////////////////////////////////////////////////////////

class CECDataReader;
class CECIvtrEquip : public CECIvtrItem
{
public:		//	Types

	//	Property effect essence index
	enum
	{
		PEEI_PHYDAMAGE = 0,
		PEEI_PHYDEF,
		PEEI_MAGICDAMAGE,
		PEEI_GOLDDEF,
		PEEI_WOODDEF,
		PEEI_WATERDEF,
		PEEI_FIREDEF,
		PEEI_EARTHDEF,
		PEEI_HP,
		PEEI_MP,
		PEEI_ENDURANCE,
		PEEI_ATKDIST,
		PEEI_STRENGTHREQ,
		PEEI_AGILITYREQ,
		PEEI_ATKSPEED,
		PEEI_DODGE,
		PEEI_MAX_PHYDAMAGE,
		PEEI_MAX_MAGICDAMAGE,
		MAX_PEEINDEX,
	};
	
	//	Property effect essence mask
	enum
	{
		PEE_PHYDAMAGE		= 0x00000001,
		PEE_PHYDEF			= 0x00000002,
		PEE_MAGICDAMAGE		= 0x00000004,
		PEE_GOLDDEF			= 0x00000008,
		PEE_WOODDEF			= 0x00000010,
		PEE_WATERDEF		= 0x00000020,
		PEE_FIREDEF			= 0x00000040,
		PEE_EARTHDEF		= 0x00000080,
		PEE_HP				= 0x00000100,
		PEE_MP				= 0x00000200,
		PEE_ENDURANCE		= 0x00000400,
		PEE_ATKDIST			= 0x00000800,
		PEE_STRENGTHREQ		= 0x00001000,
		PEE_AGILITYREQ		= 0x00002000,
		PEE_ATKSPEED		= 0x00004000,
		PEE_DODGE			= 0x00008000,
		PEE_ENERGYREQ		= 0x00010000,
		PEE_VITALITYREQ		= 0x00020000,
	};

	//	Refine effect
	enum
	{
		REFINE_PHYDAMAGE = 0,
		REFINE_MAGICDAMAGE,
		REFINE_PHYDEF,
		REFINE_GOLDDEF,
		REFINE_WOODDEF,
		REFINE_WATERDEF,
		REFINE_FIREDEF,
		REFINE_EARTHDEF,
		REFINE_HP,
		REFINE_DODGE,
		MAX_REFINEINDEX,
	};

	//	Equipment type
	enum
	{
		EQUIP_UNKNOWN = -1,
		EQUIP_WEAPON = 0,
		EQUIP_ARMOR,
		EQUIP_DECORATION,
		EQUIP_FASHION,
		EQUIP_GENERALCARD,
	};

	//	生产类型
	enum ITEM_MAKE_TAG
	{
		IMT_NULL,
		IMT_CREATE,		//	GM 发放
		IMT_DROP,		//	怪物掉落
		IMT_SHOP,		//	商城或商店买得
		IMT_PRODUCE,	//	生产得
		IMT_SIGN,		//	装备签名
	};
	
	struct PROPERTY
	{
		int		iType;		//	Property type
		int		iNumParam;	//	Number of property parameters
		bool	bEmbed;		//	Embed property flag
		bool	bSuite;		//	Suite property flag
		bool	bEngraved;	//	Engraved property flag
		int		aParams[3];	//	Property parameters
		bool	bLocal;		//  Use local property in essence
	};

public:		//	Constructor and Destructor

	CECIvtrEquip(int tid, int expire_date);
	CECIvtrEquip(const CECIvtrEquip& s);
	virtual ~CECIvtrEquip();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get scaled item
	virtual int GetScaledPrice();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrEquip(*this); }
	//	Get item description for booth buying
	virtual const wchar_t* GetBoothBuyDesc();

	//	Does this equipment has random property ?
	virtual bool HasRandomProp() { return false; }
	virtual bool HasViewProp(){ return false; }
	//	Get number of material needed to refine equipment
	virtual int GetRefineMaterialNum() { return 0; } 

	//	Get equip requirement
	int GetLevelRequirement() const { return m_iLevelReq; }
	int GetStrengthRequirement() const { return m_iStrengthReq; }
	int GetAgilityRequirement() const { return m_iAgilityReq; }
	int GetProfessionRequirement() const { return m_iProfReq; }
	int GetVitalityRequirement() const { return m_iVitalityReq; }
	int GetEnergyRequirement() const { return m_iEnergyReq; }
	int GetReputationRequirement()const{return m_iReputationReq;}
	//	Get current endurance
	int GetCurEndurance() const { return m_iCurEndurance; }
	//	Get maximum endurance
	int GetMaxEndurance() const { return m_iMaxEndurance; }
	//	Add current endurance
	int AddCurEndurance(int iValue);
	//	Add deadly strike rate provided by this equipment
	//virtual int GetDeadlyStrikeRate(bool bSuiteGen);
	//	Check whether this equipment belongs to a suite
	int GetSuiteID();
	//	Get refine level
	int GetRefineLevel() const { return m_iRefineLvl; }

	//	Is this item need repairing ?
	bool NeedRepair() { if(!IsRepairable()) return false;  return m_iCurEndurance < m_iMaxEndurance ? true : false; }
	//	Repair
	void Repair() { if(!IsRepairable()) return;  m_iCurEndurance = m_iMaxEndurance; }
	//	Get raw repair cost
	virtual float GetRawRepairCost();
	//	Get repair cost
	virtual int GetRepairCost();

	//	Get equipment type
	virtual int GetEquipmentType() const { return EQUIP_UNKNOWN; }
	bool IsWeapon() const { return GetEquipmentType() == EQUIP_WEAPON; }
	bool IsArmor() const { return GetEquipmentType() == EQUIP_ARMOR; }
	bool IsDecoration() const { return GetEquipmentType() == EQUIP_DECORATION; }
	bool IsFashion() const { return GetEquipmentType() == EQUIP_FASHION; }
	bool IsDestroying() const { return (m_iProcType & PROC_DESTROYING) ? true : false; }

	//	Get hole number
	int GetHoleNum() { return m_aHoles.GetSize(); }
	//	Get empty hole number
	int GetEmptyHoleNum();
	//	Get item id in specified hole
	int GetHoleItem(int n) { return m_aHoles[n]; }
	//	Get property number
	int GetPropertyNum() { return m_aProps.GetSize(); }
	//	Get property
	const PROPERTY& GetProperty(int n) { return m_aProps[n]; }
	//	Get made from flag
	BYTE GetMadeFromFlag() const { return m_byMadeFrom; } 
	//	Get maker info
	void ReadMakerInfo(CECDataReader &dr);
	const ACString & GetMakerName() const { return m_strMaker; }
	void SetNewMark(const ACString &strMark, A3DCOLOR clr);
	//	Get stone mask
	WORD GetStoneMask() { return m_wStoneMask; }
	
	int	GetSoulPowerAdded();

protected:	//	Attributes

	int			m_iLevelReq;		//	Level requirement
	int			m_iStrengthReq;		//	Strength requirement
	int			m_iAgilityReq;		//	Agility requirement
	int			m_iProfReq;			//	Profession requirement
	int			m_iVitalityReq;		//	Vitality requirement
	int			m_iEnergyReq;		//	Energy requirement
	int			m_iReputationReq;	//	Reputation requirement
	int			m_iCurEndurance;	//	Current endurance
	int			m_iMaxEndurance;	//	Maximum endurance
	int			m_iRepairFee;		//	Unit repair fee
	BYTE		m_byMadeFrom;		//	Made from flag
	ACString	m_strMaker;			//	Maker's name
	WORD		m_wStoneMask;		//	Stone mask
	int			m_iFixProps;		//	Fixed properties
	int			m_iRefineLvl;		//	Refine level
	BYTE		m_byPropNum;		//	Add-on properties number
	BYTE		m_byEmbedNum;		//	Embed stone number

	AArray<int, int>				m_aHoles;	//	Holes on equipment
	AArray<PROPERTY, PROPERTY&>		m_aProps;	//	Properties

protected:	//	Operations

	//	Add price description
	virtual void AddPriceDesc(int col, bool bRepair);
	//	Decide equipment name color
	virtual int DecideNameCol();

	//  Set properties to local
	virtual void SetLocalProps();

	//	Convert endurance real value to displaying value
	int VisualizeEndurance(int v);
	//	Add one add-on property description
	void AddOneAddOnPropDesc(int idProp, const int* p, int* aPEEVals, int* aRefines, bool bLocal = false) { AddOneAddOnPropDesc(idProp, p[0], p[1], p[2], aPEEVals, aRefines, bLocal); }
	void AddOneAddOnPropDesc(int idProp, int p0, int p1, int p2, int* aPEEVals, int* aRefines, bool bLocal = false);
	//	Build add-ons properties description
	void BuildAddOnPropDesc(int* aPEEVals, int* aRefines);
	//	Build tessera description
	void BuildTesseraDesc();
	//	Parse properties
	void ParseProperties();
	//	Get item description text for suite information
	virtual void AddSuiteDesc();
	//  Get equip destroying description
	void AddDestroyingDesc(int id, int iNum);
	// Add Reputation Desc
	void AddReputationReqDesc();
	//	添加磨刀石增加的特殊属性
	void AddSharpenerDesc();
	//	添加镌刻属性
	void AddEngravedDesc();
	//	添加制造者或标记信息
	void AddMakerDesc();
	
	//	Does eqiupment has extend properties which effect equipment's essence ?
	DWORD PropEffectEssence();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


struct EQUIPMENT_ADDON;

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrEquipAddonDesc
//	
///////////////////////////////////////////////////////////////////////////
class CECIvtrEquipAddonDesc : private CECIvtrEquip
{
public:
	CECIvtrEquipAddonDesc(unsigned int id_equip_addon = 0)
		:CECIvtrEquip(0, 0), m_pAddon(NULL)
	{
		SetAddon(id_equip_addon);
	}

	const wchar_t *GetText()const
	{
		return m_strDesc.GetLength() ? m_strDesc : NULL;
	}

	bool SetAddon(unsigned int id_equip_addon);

private:
	const EQUIPMENT_ADDON *m_pAddon;
};
