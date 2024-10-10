/*
 * FILE: EC_IvtrFlySword.h
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
#include "EC_IvtrTypes.h"

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

struct FLYSWORD_ESSENCE;
struct WINGMANWING_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFlySword
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFlySword : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFlySword(int tid, int expire_date);
	CECIvtrFlySword(const CECIvtrFlySword& s);
	virtual ~CECIvtrFlySword();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrFlySword(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Whether a rare item
	virtual bool IsRare() const;

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get current time
	int GetCurTime() { return m_Essence.cur_time; }
	//	Set current time
	void SetCurTime(int iTime) { m_Essence.cur_time = iTime; m_iCurTime = iTime * 1000; }
	//	Time pass
	int TimePass(int iDeltaTime)
	{
		m_iCurTime -= iDeltaTime;
		m_Essence.cur_time = (int)(m_iCurTime * 0.001f);
		if (m_Essence.cur_time < 0)
			m_Essence.cur_time = 0;
		return m_Essence.cur_time;
	}
	//	Get max time
	int GetMaxTime();
	//	Get max element number
	int GetMaxElement();
	//	Get time each element equal to
	int GetElementTime();
	//	Get number of element if time is filled to full. This is just the number
	//	of element which has been used
	int GetUsedElementNum();
	//	Get profession requirement
	int GetProfRequirement() { return m_Essence.profession; } 

	//	Get essence data
	const IVTR_ESSENCE_FLYSWORD& GetEssence() { return m_Essence; }

	//	Get database data
	const FLYSWORD_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	bool IsImprovable(); // 是否可强化
	bool CanBeImproved(); // 是否能还能继续强化
	int GetMaxImproveLevel();

protected:	//	Attributes

	int		m_iCurTime;		//	Current time counter in ms

	IVTR_ESSENCE_FLYSWORD	m_Essence;

	//	Data in database
	FLYSWORD_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrWing
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrWing : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrWing(int tid, int expire_date);
	CECIvtrWing(const CECIvtrWing& s);
	virtual ~CECIvtrWing();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrWing(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	//	Get the MP consumed every seconds
	int GetMPPerSecond();
	//	Get the launch MP cost 
	int GetLaunchMP();

	//	Get essence data
	const IVTR_ESSENCE_WING& GetEssence() { return m_Essence; }

	//	Get database data
	const WINGMANWING_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	IVTR_ESSENCE_WING		m_Essence;

	//	Data in database
	WINGMANWING_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

