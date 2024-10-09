/*
 * FILE: EC_HPWorkFollow.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DVector.h"
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
//	Class CECHPWorkFollow
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkFollow : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkFollow(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkFollow();

public:		//	Attributes

public:		//	Operations

	//	Change trace target
	void ChangeTarget(int idTarget);

	//	This work is do player moving ?
	virtual bool IsMoving() { return !m_bStopMove; }

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	virtual void Cancel();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

protected:	//	Attributes

	int			m_idTarget;		//	Target object's id
	A3DVECTOR3	m_vDestPos;		//	Destination position
	A3DVECTOR3	m_vOffset;		//	Offset to target
	A3DVECTOR3	m_vCurDirH;		//	Current move direction
	bool		m_bStopMove;	//	Stop move flag
	bool		m_bMeetSlide;	//	true, meet slide

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();

	//	Follow on ground
	bool Follow_Walk(float fDeltaTime);
	//	Follow in air and water
	bool Follow_FlySwim(float fDeltaTime);

	//	Stop move
	void StopMove(const A3DVECTOR3& vPos, float fSpeed, int iMoveMode);
	//	Calculate vertical speed when fly or swim
	float CalcFlySwimVertSpeed(float fSpeed1, float fPushDir, float fPushAccel, float fDeltaTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

