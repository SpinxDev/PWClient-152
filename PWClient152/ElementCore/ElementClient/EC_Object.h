/*
 * FILE: EC_Object.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DCoordinate.h"

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
//	Class CECObject
//	
///////////////////////////////////////////////////////////////////////////

class CECObject : public A3DCoordinate
{
public:		//	Types

	//	Class ID
	enum
	{
		OCID_OBJECT = 0,
		OCID_PLAYER,
		OCID_HOSTPLAYER,
		OCID_ELSEPLAYER,
		OCID_LOGINPLAYER,
		OCID_NPC,
		OCID_MONSTER,
		OCID_SERVER,
		OCID_MATTER,
		OCID_PET,
		OCID_CLONED_PLAYER,		//	复制角色模型
		OCID_HOST_NAVIGATER,	//	纯客户端player类,用于强制移动
		OCID_NPCCLONED_MASTER,	//	NPC 复制的角色模型
	};
	
	//	Render flag
	enum
	{
		RD_NORMAL = 0,
		RD_REFRACT,
		RD_REFLECT,
	};

	//	Move environment
	enum
	{
		MOVEENV_GROUND = 0,	//	Move on ground
		MOVEENV_WATER,
		MOVEENV_AIR,
	};


public:		//	Constructor and Destructor

	CECObject();
	virtual ~CECObject();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set destination orientation of model
	void SetDestDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, DWORD dwTime);

	//	Calculate distance between this object and specified position
	float CalcDist(const A3DVECTOR3& vPos, bool b3D)
	{
		A3DVECTOR3 vDelta = GetPos() - vPos;
		return b3D ? vDelta.Magnitude() : vDelta.MagnitudeH();
	}
	//	Turn around object and face to specified object
	virtual void TurnFaceTo(int idTarget, DWORD dwTime=200);

	//	Set / Get visible flag
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool GetVisible() { return m_bVisible; }

	//	Get object class ID
	int GetClassID()const { return m_iCID; }
	bool IsPlayer()const { return m_iCID == OCID_HOSTPLAYER || m_iCID == OCID_ELSEPLAYER || m_iCID == OCID_LOGINPLAYER; }
	bool IsHostPlayer()const { return m_iCID == OCID_HOSTPLAYER; }
	bool IsElsePlayer()const { return m_iCID == OCID_ELSEPLAYER; }
	bool IsLoginPlayer() const{ return m_iCID == OCID_LOGINPLAYER; }
	bool IsNPC()const { return m_iCID == OCID_MONSTER || m_iCID == OCID_SERVER; }
	bool IsMonsterNPC()const { return m_iCID == OCID_MONSTER; }
	bool IsServerNPC()const { return m_iCID == OCID_SERVER; }
	bool IsMatter()const { return m_iCID == OCID_MATTER; }
	bool IsSelectable()const { return m_bSelectable; }
	//	Set / Get object born stamp
	DWORD GetBornStamp()const { return m_dwBornStamp; }
	void SetBornStamp(DWORD dwStamp) { m_dwBornStamp = dwStamp; }
	//	Set / Get born in sight flag
	bool GetBornInSight()const { return m_bBornInSight; }
	void SetBornInSight(bool bFlag) { m_bBornInSight = bFlag; }
	
	//	Get object id
	static int GetObjectID(CECObject* pObject);

protected:	//	Attributes

	int				m_iCID;				//	Class ID
	bool			m_bAdjustOrient;	//	Is adjusting orientation
	A3DQUATERNION	m_quOrientStart;	//	Orientation start
	A3DQUATERNION	m_quOrientEnd;		//	Orientation end
	DWORD			m_dwOrientTime;		//	Orientation adjusting time counter
	DWORD			m_dwOrientTimeCnt;	//	Orientation adjusting time counter

	bool			m_bBornInSight;		//	Born in my sight
	DWORD			m_dwBornStamp;		//	Born time stamp
	bool			m_bVisible;			//	Render flag

	bool			m_bUseGroundNormal;	//	flag indicates whether we use a ground normal as up
	A3DVECTOR3		m_vecGroundNormal;	//	current ground normal
	A3DVECTOR3		m_vecGroundNormalSet;	//	ground normal set

	bool			m_bSelectable;		// whether the object can be selected

public:
	inline const A3DVECTOR3& GetGroundNormal()const			{ return m_vecGroundNormal; }
	inline bool GetUseGroundNormal()const					{ return m_bUseGroundNormal; }

	void SetUseGroundNormal(bool bFlag);
	void SetGroundNormal(const A3DVECTOR3& vecNormal);
	void SetSelectable(bool bSelectable) { m_bSelectable = bSelectable; }

protected:	//	Operations

	//	Adjust orientation
	void AdjustOrientation(DWORD dwDeltaTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

