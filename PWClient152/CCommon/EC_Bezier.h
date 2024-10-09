/*
 * FILE: EC_Bezier.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XinQingFeng, 2005/2/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

#include "ABaseDef.h"
#include "A3DVector.h"
#include "AString.h"
#include "EC_SceneObject.h"
#include <stdio.h>

#define BEZIER_FILE_VERSION		3

class AFile;

class CECBezierPoint
{

public:
	
	CECBezierPoint(){};
	virtual ~CECBezierPoint(){};
	
public:
	
	const A3DVECTOR3& GetPos() const { return m_vPos; };
	const A3DVECTOR3& GetDir() const { return m_vDir; };
	void SetPos( const A3DVECTOR3& pos)	{ m_vPos = pos; };
	void SetDir( const A3DVECTOR3& dir) { m_vDir = dir; };

protected:	

	A3DVECTOR3			m_vPos;
	A3DVECTOR3          m_vDir;

private:

};

class CECBezierSeg
{

public:
	
	CECBezierSeg();
	virtual ~CECBezierSeg();

public:

	void        Init( CECBezierPoint *pListPt);
	
	A3DVECTOR3	Bezier( float u ,bool bForward);
	A3DVECTOR3	Vector( float u ,bool bForward);

	inline A3DVECTOR3	GetAnchorHead() const { return m_vAnchorHead; };
	inline A3DVECTOR3	GetAnchorTail() const { return m_vAnchorTail; };

	inline int			GetHeadPoint() const { return m_nHeadPoint; };
	inline int			GetTailPoint() const { return m_nTailPoint; };

	inline float		GetSegLength() const { return m_fLenght; }

	inline void		SetAnchorHead( const A3DVECTOR3 v ) { m_vAnchorHead = v; };
	inline void		SetAnchorTail( const A3DVECTOR3 v ) { m_vAnchorTail = v; };

	inline void     SetHeadPoint( const int index ) { m_nHeadPoint = index; };
	inline void		SetTailPoint( const int index ) { m_nTailPoint = index; };

	inline void		SetSegLenght( const float lenght) { m_fLenght = lenght; }
	

protected:
	
	A3DVECTOR3	m_vAnchorHead;		
	A3DVECTOR3	m_vAnchorTail;	
	int         m_nHeadPoint;
	int         m_nTailPoint;          
	float       m_fLenght;  
	
private:
	CECBezierPoint *m_pListPoint;
};

class CECBezier : public CECSceneObject
{

public:
	CECBezier();
	virtual ~CECBezier();

	//User interface
public:

	bool			Load(AFile* pFile);

	void            SetOffset(A3DVECTOR3 vOffset);
	inline int	    GetObjectID() const { return m_nObjectID; };
	void            Release();

	int				GetSegmentNum() { return m_nNumSeg; }
	CECBezierSeg*	GetSegment(int n) { ASSERT(n >= 0 && n < m_nNumSeg); return &m_pListSeg[n]; }

	//	Set / Get global ID
	void SetGlobalID(int iID) { m_iGlobalID = iID; }
	int GetGlobalID() { return m_iGlobalID; }
	//	Set / Get next route's global ID
	void SetNextGlobalID(int iID) { m_iNextGlobalID = iID; }
	int GetNextGlobalID() { return m_iNextGlobalID; }

	//Only useful for CEditerBezier's object translate to CECBezier object
public:
	void            Assign(int nNumPt,int nNumSeg);
	inline void     SetObjectID( int id){  m_nObjectID = id; };
	void            AddBezierPoint( CECBezierPoint *pt , int i);
	void            AddBezierSeg( CECBezierSeg *seg , int i);

protected:

	int             m_nObjectID;
	int				m_iGlobalID;		//	Global ID used to link bezier routes
	int				m_iNextGlobalID;	//	Next bezier route's global ID
	int				m_nNumPoint;
	int				m_nNumSeg;
	
	AString			m_strActName;		//	Action name

	CECBezierPoint*	m_pListPoint;
	CECBezierSeg*	m_pListSeg;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECBezierWalker
//	
///////////////////////////////////////////////////////////////////////////

class CECBezierWalker
{
public:		//	Types

public:		//	Constructor and Destructor

	CECBezierWalker();
	virtual ~CECBezierWalker();

public:		//	Attributes

public:		//	Operations

	//	Bind bezier route
	bool BindBezier(CECBezier* pBezier);
	//	Start walk
	bool StartWalk(bool bLoop, bool bForward);
	//	Pause walk
	void Pause(bool bPause);

	//	Tick routine
	bool Tick(int iDeltaTime);

	//	Get current position
	A3DVECTOR3 GetPos();
	//	Get current direction
	A3DVECTOR3 GetDir();

	//	Forward flag
	void SetForwardFlag(bool bForward);
	bool GetForwardFlag() { return m_bForward; }
	//	Move speed
	void SetSpeed(float fSpeed);
	float GetSpeed() { return m_fSpeed; }
	//	Get total time
	int GetTotalTime() { return m_iTotalTime; }
	//	Loop flag
	void SetLoopFlag(bool bLoop) { m_bLoop = bLoop; }
	bool GetLoopFlag() { return m_bLoop; }
	//	Get walking flag
	bool IsWalking() { return m_bWalking; }
	//	Get pause flag
	bool IsPause() { return m_bPause; }

protected:	//	Attributes

	CECBezier*	m_pBezier;		//	Bezier route data
	bool		m_bForward;		//	Forward flag
	float		m_fSpeed;		//	Moving speed
	int			m_iTotalTime;	//	Total time of whole bezier route
	int			m_iTimeCnt;		//	Time counter
	int			m_iCurSeg;		//	Current segment
	int			m_iCurSegTime;	//	Total time of current segment
	int			m_iPassSegTime;	//	Total time of passed segments
	bool		m_bLoop;		//	Loop flag
	bool		m_bWalking;		//	true, is walking
	bool		m_bPause;		//	Pause flag
	bool		m_bForwardStop;	//	Stop flag

protected:	//	Operations

};


