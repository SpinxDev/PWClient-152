/*
 * FILE: EC_HPWorkStand.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/11
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
//	Class CECHPWorkStand
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkStand : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkStand(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkStand();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	
	//	On work shift to
	virtual void OnWorkShift();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//	This work is do player moving ?
	virtual bool IsMoving() { return m_bMoving; }
	//	Work is cancel
	virtual void Cancel();

	//	Set / Get pose action
	void SetPoseAction(int iAction, bool bSession);
	int GetPoseAction() { return m_iPoseAction; }
	//	Doing session pose ?
	bool DoingSessionPose();
	//	Get stop sliding flag
	bool GetStopSlideFlag() { return m_bStopSlide; }

protected:	//	Attributes

	bool	m_bLastPush;	//	Last push flag
	bool	m_bMeetSlide;	//	true, meet slope
	int		m_iPoseAction;	//	Pose action
	bool	m_bSession;		//	true, this pose action is a session action
	bool	m_bWaterStop;	//	Stopped in water flag
	bool	m_bMoving;		//	Moving flag
	bool	m_bStopSlide;	//	Stop sliding flag
	int		m_iCurAction;	//	Action of this tick

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();

	//	Should player start move ?
	int ShouldMove();

	//	Tick routine of walking on ground
	bool Tick_Walk(float fDeltaTime);
	//	Tick routine of flying or swimming
	bool Tick_FlySwim(float fDeltaTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


