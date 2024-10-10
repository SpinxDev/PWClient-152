/*
 * FILE: EC_HPWorkFly.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/23
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
//	Class CECHPWorkFly
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkFly : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkFly(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkFly();

public:		//	Attributes
	
	DWORD m_dwLaunchTime;
	bool m_bContinueFly;

public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//	This work is do player moving ?
	virtual bool IsMoving() { return true; }
	//	Work is cancel
	virtual void Cancel();

protected:	//	Attributes

	A3DVECTOR3 	m_vDestPos;		//	Destination stop position
	bool		m_bAddSpeed;	//	Add a vertical speed

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkFMove
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkFMove : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkFMove(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkFMove();

public:		//	Attributes
	
public:		//	Operations

	//	Prepare to move
	void PrepareMove(const A3DVECTOR3& vDestPos, float fMoveTime, int iFMoveSkillID = 0);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//	This work is do player moving ?
	virtual bool IsMoving() { return true; }

protected:	//	Attributes

	A3DVECTOR3 	m_vDestPos;		//	Destination position
	A3DVECTOR3	m_vMoveDir;		//	Move direction
	float		m_fSpeed;		//	Move speed
	float		m_fDist;		//	Distance
	float		m_fDistCnt;		//	Distance counter
	int			m_iFMoveSkillID;//  Flash move skill id

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
	
	//	Finish work
	void Finish();
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkPassiveMove
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkPassiveMove : public CECHPWork
{
public:		//	Types

	// Passive move type
	enum
	{
		PASSIVE_DIRECT,		// cmd_player_teleport mode == 0
		PASSIVE_PULL,		// cmd_player_teleport mode == 1
		PASSIVE_KNOCKBACK,	// cmd_player_knockback
	};

public:		//	Constructor and Destructor

	CECHPWorkPassiveMove(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkPassiveMove();

public:		//	Attributes
	
public:		//	Operations

	//	Prepare to move
	void PrepareMove(const A3DVECTOR3& vDestPos, int nMoveTime);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//	This work is do player moving ?
	virtual bool IsMoving() { return true; }

protected:	//	Attributes

	A3DVECTOR3	m_vStartPos;	//	起始位置（被击退时开始位置）
	A3DVECTOR3 	m_vDestPos;		//	目标位置（服务器传来）
	A3DVECTOR3	m_vMoveDir;		//	移动方向
	float		m_fSpeed;		//	移动速度
	int			m_nMoveTime;	//	移动的总时间(ms)
	int			m_nMoveTimeCnt;	//	累计的移动时间(ms)
	int			m_nWaitTime;	//	移动完成后与服务器同步等待的时间(ms)
	int			m_nWaitTimeCnt;	//	累计的同步等待时间(ms)

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
	
	//	Finish work
	void Finish();
};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



