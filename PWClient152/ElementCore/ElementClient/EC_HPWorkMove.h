/*
 * FILE: EC_HPWorkMove.h
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
//	Class CECHPWorkMove
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkMove : public CECHPWork
{
public:		//	Types

	enum
	{
		DEST_2D = 0,
		DEST_3D,
		DEST_DIR,
		DEST_PUSH,
		DEST_STANDJUMP,
		DEST_AUTOPF,		//	智能寻路模式
	};

public:		//	Constructor and Destructor

	CECHPWorkMove(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkMove();

public:		//	Attributes

public:		//	Operations

	//	Set destination position or direction
	void SetDestination(int iDestType, const A3DVECTOR3& vMoveDest);

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

	//	Play move target effect
	void PlayMoveTargetGFX(const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal);

	//	User press cancel button
	void PressCancel() { m_bReadyCancel = true; }

	void SetUseAutoMoveDialog(bool bUseAutoMoveDialog);
	bool GetUseAutoMoveDialog()	{ return m_bUseAutoMoveDialog; }
	bool GetAutoMove();

	void SetAutoLand(bool bAutoLand)	{ m_bAutoLand = bAutoLand; }
	bool GetAutoLand()	{ return m_bAutoLand; }

	void SetAutoHeight(float fHeight) { m_fAutoHeight = fHeight; m_bAutoFly = true; m_bReachedHeight = false; }
	float GetAutoHeight() { return m_fAutoHeight; }
	bool IsAutoFly() const { return m_bAutoFly;}

	bool IsAutoPF()const{ return m_iDestType == DEST_AUTOPF; }

	//	Finish work
	void Finish();

	void SetTaskNPCInfo(int tid, int taskid);

	void SwitchToDest2D();
	
protected:	//	Attributes

	A3DVECTOR3	m_vMoveDest;	//	Move destination position or direction
	int			m_iDestType;	//	Destination type
	bool		m_bHaveDest;	//	true, have destination
	bool		m_bMeetSlide;	//	true, meet slide
	A3DVECTOR3	m_vCurDir;		//	Current move direction
	bool		m_bReadyCancel;	//	true, ready to cancel
	bool		m_bGliding;		//	glide
	float		m_fGlideTime;
	float		m_fGlideSpan;
	float		m_fGlideAng;
	float		m_fGlideVel;	//	glide angular vel
	float		m_fGlidePitch;	//	glide pitch angle
	float		m_fCurPitch;
	float		m_fPushPitch;
	float		m_fPushLean;
	
	bool		m_bUseAutoMoveDialog;	//  Auto move
	float		m_fAutoHeight;	//  Height of auto moving destination 
	bool		m_bAutoLand;	//  Auto land when arrive at destination
	bool		m_bAutoFly;		//  Auto fly
	bool		m_bReachedHeight;// Player reached specified height
	bool		m_bAutoFlyPending;		//  Mark whether a fly command had been executed

	int			m_iNPCTempleId;
	int			m_iTaskId;
	bool		m_bSwitchTo2D;

	bool		m_bResetAutoPF;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();

	//	Tick routine of walking on ground
	bool Tick_Walk(float fDeltaTime);
	//	Tick routine of flying or swimming
	bool Tick_FlySwim(float fDeltaTime);
	//	Start gliding
	void Glide(float fMoveTime, const A3DVECTOR3& vMoveDirH, float fDeltaTime, bool bFly);

	//	Calculate vertical speed when fly or swim
	float CalcFlySwimVertSpeed(float fSpeed1, float fPushDir, float fPushAccel, float fDeltaTime);

	void ResetUseAutoPF();
	void ClearResetUseAutoPF();
	void UpdateResetUseAutoPF();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

