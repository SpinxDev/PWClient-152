/*
 * FILE: EC_HPWorkFall.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_HPWork.h"

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
//	Class CECHPWorkFall
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkFall : public CECHPWork
{
public:		//	Types

	//	Fall type
	enum
	{
		TYPE_FREEFALL = 0,
		TYPE_FLYFALL,
	};

public:		//	Constructor and Destructor

	CECHPWorkFall(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkFall();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Work is cancel
	virtual void Cancel();

	//	This work is do player moving ?
	virtual bool IsMoving() { return true; }
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

	//	Set fall type
	void SetFallType(int iType) { m_iFallType = iType; }
	int GetFallType() { return m_iFallType; }
	
protected:	//	Attributes
	
	A3DVECTOR3	m_vDirH;
	float		m_fSpeedH;
	bool		m_bEnterWater;
	int			m_iFallMode;
	int			m_nCurStage;
	int			m_iFallType;
	bool		m_fForceDown;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();

	//	Fly fall in air
	bool Fall_Air(float fDeltaTime);
	//	Fly fall in water
	bool Fall_Water(float fDeltaTime);
	//	Free fall in air
	bool FreeFall_Air(float fDeltaTime);
	//	Free fall in water
	bool FreeFall_Water(float fDeltaTime);
	//	Finish
	void Finish();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

