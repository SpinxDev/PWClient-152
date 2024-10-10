/*
 * FILE: EC_HPWorkTrace.h
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
#include "EC_Counter.h"

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

class CECObject;
class CECSkill;
class CECHPWorkTrace;


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkTrace
//	
///////////////////////////////////////////////////////////////////////////

enum TraceObjectType{
	TRACE_NONE = 0,
	TRACE_PLAYER,
	TRACE_MATTER,
	TRACE_NPC,
	TRACE_TASKNPC
};

class CECTracedObject
{
	//friend CECHPWorkTrace;
	
protected:
	int m_iTraceType;
	int m_iObjectId;
	int m_iReason;
	bool m_bMoreClose;
	CECHostPlayer* m_pHost;
	
public:
	CECTracedObject(int type, int id, CECHostPlayer* pHost, int ireason);
	CECTracedObject(const CECTracedObject& rhs);
	virtual ~CECTracedObject(){	}
	
	virtual bool OnTargetMissing() = 0;	
	virtual A3DVECTOR3 GetTargetPos()  = 0;
	virtual bool OnTouched()  = 0;
	virtual CECTracedObject* Clone() const = 0;

	virtual bool IsTargetMissing();
	virtual A3DVECTOR3 GetMovePos();

	int GetObjectID();
	bool CanTouchMoreClose();
	void SetMoveCloseFlag(bool bMoveClode);
	int GetTraceReason();
	bool CanTouchFrom(const A3DVECTOR3 &vHostPos);
	int GetTraceType();
	CECObject* GetTargetObject();	
};

class CECTracedNPC : public CECTracedObject
{
protected:
	bool m_bForceAttack;
	
public:
	CECTracedNPC(int type, int id, CECHostPlayer* pHost, int ireason, bool bForceAttack = false);
	CECTracedNPC(const CECTracedNPC& rhs);
	virtual ~CECTracedNPC(){ };
	virtual	bool IsTargetMissing();
	virtual bool OnTargetMissing();
	virtual A3DVECTOR3 GetTargetPos();
	virtual bool OnTouched();
	virtual CECTracedObject* Clone() const;	
};

class CECTracedPlayer : public CECTracedObject
{
protected:
	bool m_bForceAttack;
	
public:
	CECTracedPlayer(int type, int id, CECHostPlayer* pHost, int ireason, bool bForceAttack = false);
	CECTracedPlayer(const CECTracedPlayer& rhs);
	virtual ~CECTracedPlayer(){ };
	virtual bool IsTargetMissing();
	virtual bool OnTargetMissing();
	virtual A3DVECTOR3 GetTargetPos();
	virtual bool OnTouched();
	virtual CECTracedObject* Clone() const;	
};

class CECTracedMatter : public CECTracedObject
{
protected:
	
public:
	CECTracedMatter(int type, int id, CECHostPlayer* pHost, int ireason);
	CECTracedMatter(const CECTracedMatter& rhs);
	virtual ~CECTracedMatter(){ };
	virtual bool OnTargetMissing();
	virtual A3DVECTOR3 GetTargetPos();
	virtual bool OnTouched();
	virtual CECTracedObject* Clone() const;	
};

class CECTracedTaskNPC : public CECTracedNPC
{
protected:
	int m_iTaskId;
public:
	CECTracedTaskNPC(int type, int id, CECHostPlayer* pHost, int taskid);
	CECTracedTaskNPC(const CECTracedTaskNPC& rhs);
	virtual ~CECTracedTaskNPC(){ };

	virtual bool OnTouched();
	virtual CECTracedObject* Clone() const;	
};

class CECHPWorkTrace : public CECHPWork
{
public:		//	Types
	
	//	Trace reason
	enum
	{
		TRACE_NONE = -1,
		TRACE_ATTACK = 0,	//	Normal attack
		TRACE_PICKUP,		//	Pickup object
		TRACE_TALK,			//	Go to talk
		TRACE_SPELL,		//	Cast magic
		TRACE_GATHER,		//	Gather object
	};

public:		//	Constructor and Destructor

	CECHPWorkTrace(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkTrace();

public:		//	Attributes

public:		//	Operations

	//	Change trace target
	//void ChangeTarget(int idTarget, int iReason, bool bUseAutoPF=false);
	//	设定traceobject
	void SetTraceTarget(CECTracedObject* pTraceObj, bool bUseAutoPF = false);

	CECTracedObject* CreatTraceTarget(int iTraceObjId, int iReason, bool bForceAttack = false);

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

	//	User press cancel button
	void PressCancel();
	//	Set move close flag
	void SetMoveCloseFlag(bool bMoveClose) { m_pTraceObject->SetMoveCloseFlag( bMoveClose ); }
	
	//	Set / Get force attack flag
	void SetForceAttack(bool bTrue) { m_bForceAttack = bTrue; }
	bool GetForceAttack() { return m_bForceAttack; }
	//	Set / Get prepared skill
	void SetPrepSkill(CECSkill* pSkill) { m_pPrepSkill = pSkill; }
	CECSkill* GetPrepSkill() { return m_pPrepSkill; }
	//	Get target ID
	int GetTarget() { return m_pTraceObject->GetObjectID(); }
	//	Get trace reason
	int GetTraceReason() { return m_pTraceObject->GetTraceReason(); }
	//	AutoPF
	void SetUseAutoPF(bool bUse);
	bool GetUseAutoPF()const;
	bool IsAutoPF()const;

	void SetActionDone(bool bActionDone) { m_bActionDone = bActionDone; }

	void OnTargetMissing();

	void OnTouchTarget();
	bool CanTouch();

protected:	//	Attributes

	bool			m_bHaveMoved;	//	Have moved flag
	bool			m_bMeetSlide;	//	true, meet slide
	bool			m_bCheckTouch;	//	Check whether touch target in this frame
	A3DVECTOR3		m_vCurDirH;		//	Current move direction
	bool			m_bReadyCancel;	//	true, ready to cancel
	bool			m_bMoreClose;	//	Move close flag
	bool			m_bForceAttack;	//	Force attack flag
	CECSkill*		m_pPrepSkill;	//	Skill prepared to be casted
	bool			m_bActionDone;	//  目标行为成功发出
	bool			m_bShouldResetUseAutoPF;
	bool			m_bUseAutoPFResetValue;
	bool			m_bUseAutoPF;	//	Use CECIntelligentRoute::Search
	DWORD			m_dwAutoPFNextCheckTime;	//	下次检查时间

	CECTracedObject*	m_pTraceObject; //	定义trace目标对象

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();

	//	Trace on ground
	bool Trace_Walk(float fDeltaTime);
	//	Trace in air and water
	bool Trace_FlySwim(float fDeltaTime);

	//	Stop move when touch target
	void StopMove(bool bFinish);
	//	Handle the case that target died when host is tracing it
	bool OnTargetDied(CECObject* pTarget);
	//	Is valid time to touch target ?
	bool IsGoodTimeToTouch();
	//	Check prepare skill
	void CheckPrepSkill();
	bool GetTargetCurPos(A3DVECTOR3 &pos);
	A3DVECTOR3 GetCurMovingDest();
	void UpdateUseAutoPF();

	void ReplaceTarget(CECTracedObject* ptraceobj);

	void ResetUseAutoPF(bool bUseAutoPF);
	void UpdateResetUseAutoPF();
	void ClearResetUseAutoPF();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

