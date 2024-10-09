/*
 * FILE: EC_HostMove.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DVector.h"
#include "EC_Counter.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Negative accelerate in water and air
#define EC_NACCE_AIR		-5.0f
#define EC_NACCE_WATER		-5.0f

//	Push accelerate
#define EC_PUSH_ACCE		8.0f	//	Normal push accelerate in air and water
#define EC_FLYOFF_ACCE		10.0f	//	Fly off accelerate

#define EC_SLOPE_Y			0.5f
#define EC_GRAVITY			9.8f

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECHostPlayer;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostMove
//	
///////////////////////////////////////////////////////////////////////////

class CECHostMove
{
public:		//	Types

	//	Delayed stop move data
	struct STOPMOVE
	{
		bool		bValid;		//	Valid flag
		A3DVECTOR3	vPos;
		float		fSpeed;
		int			iMoveMode;
		BYTE		byDir;
		DWORD		dwTimeStamp;
		float		fTime;
	};

public:		//	Constructor and Destructor

	CECHostMove(CECHostPlayer* pHost);
	virtual ~CECHostMove();

public:		//	Attributes

	A3DVECTOR3	m_vFlashTPNormal;	//	TP normal after last flash move

public:		//	Operations

	//	Tick routine
	void Tick(DWORD dwDeltaTime);

	//	Forcase moving and send move command
	void SendMoveCmd(const A3DVECTOR3& vCurPos, int iDestType, const A3DVECTOR3& vDest, const A3DVECTOR3& vMoveDir, float fSpeed, int iMoveMode, bool bForceSend = false);
	//	Forcase moving and send move command
	void SendMoveCmd(const A3DVECTOR3& vCurPos, int iDestType, const A3DVECTOR3& vDest, const A3DVECTOR3& vVel, int iMoveMode, bool bForceSend = false)
	{
		A3DVECTOR3 vMoveDir(vVel);
		float fSpeed = vMoveDir.Normalize();
		SendMoveCmd(vCurPos, iDestType, vDest, vMoveDir, fSpeed, iMoveMode, bForceSend);
	}
	//	Send stop move command
	void SendStopMoveCmd(const A3DVECTOR3& vPos, float fSpeed, int iMoveMode);
	//	Send stop move command using current position and default speed and move mode
	void SendStopMoveCmd();

	//	Check whether host meet a slope
	bool MeetSlope(const A3DVECTOR3& vMoveDirH, float& fMaxSpeedV);
	//	Is stoping ?
	bool IsStop() { return m_bStop; }

	//	Ground move
	A3DVECTOR3 GroundMove(const A3DVECTOR3& vDirH, float fSpeedH, float fTime, float fSpeedV=0.0f, float fGravity=EC_GRAVITY);
	//	Air/Water move
	A3DVECTOR3 AirWaterMove(const A3DVECTOR3& vSpeed, float fTime, bool bInAir, bool bTrace=false)
	{
		A3DVECTOR3 vDir = vSpeed;
		float fSpeed = vDir.Normalize();
		return AirWaterMove(vDir, fSpeed, fTime, bInAir, bTrace);
	}
	A3DVECTOR3 AirWaterMove(const A3DVECTOR3& vDir, float fSpeed, float fTime, bool bInAir, bool bTrace=false);
	//	Flash move
	A3DVECTOR3 FlashMove(const A3DVECTOR3& vDir, float fSpeed, float fDist);

	//	Get move blocked counter
	int MoveBlocked() { return m_iBlockedCnt; }
	//	Get host average horizonal moving speed (m/s)
	float GetAverHoriSpeed() { return m_fAverSpeedH; }
	//	Force to set host's last position
	void SetHostLastPos(const A3DVECTOR3& vPos) { m_vLastPos = vPos; }
	//	Set host's last position sent to server
	void SetLastSevPos(const A3DVECTOR3& vPos) { m_vLastSevPos = vPos; }
	//	Get host's last position sent to server
	const A3DVECTOR3& GetLastSevPos() { return m_vLastSevPos; }
	//	Set / Get move stamp
	void SetMoveStamp(WORD wStamp) { m_wMoveStamp = wStamp; }
	WORD GetMoveStamp() { return m_wMoveStamp; }
	//	Set / Get slide block flag
	void SetSlideLock(bool bLock) { m_bSlideLock = bLock; }
	bool GetSlideLock() { return m_bSlideLock; }
	//	Set / Get local move flag
	void SetLocalMoveFlag(bool bTrue) { m_bLocalMove = bTrue; }
	bool GetLocalMoveFlag() { return m_bLocalMove; }

	//	Reset move control to stop state
	void Reset();

protected:	//	Attributes

	CECHostPlayer*		m_pHost;	//	Host player

	CECCounter	m_CmdTimeCnt;		//	Move command sending time counter
	CECCounter	m_StopCmdCnt;
	DWORD		m_dwMoveTimeCnt;	//	Move command sending time counter
	DWORD		m_dwMoveCmdInter;	//	Interval of sending move command to server
	bool		m_bStop;			//	Stop flag
	int			m_iBlockedCnt;		//	Move blocked counter
	bool		m_bSlideLock;		//	Locked when slide
	float		m_fMoveTime;		//	Move time counter
	float		m_fBlockTime;		//	Block time counter
	float		m_fBlockMove;		//	Block move counter
	A3DVECTOR3	m_vBlockMove;
	bool		m_bLocalMove;		//	true, Moving info isn't sent to server

	float		m_fAverSpeedH;		//	Host average horizonal moving speed (m/s)
	A3DVECTOR3	m_vLastPos;			//	Host's position in last tick
	DWORD		m_dwLastTime;		//	Last tick time
	WORD		m_wMoveStamp;		//	Move stamp
	A3DVECTOR3	m_vLastSevPos;		//	Host's last position sent to server

	STOPMOVE	m_DelayedStop;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



