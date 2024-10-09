/*
 * FILE: EC_Ornament.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_SceneObject.h"
#include "AString.h"

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

class CECViewport;
class CECOrnamentMan;
class CECScene;
class AFile;
class CELBrushBuilding;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECOrnament
//	
///////////////////////////////////////////////////////////////////////////

class CECOrnament : public CECSceneObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CECOrnament(CECOrnamentMan* pOrnamentMan);
	virtual ~CECOrnament();

public:		//	Attributes

public:		//	Operations

	//	Load ornament data from file
	bool Load(CECScene* pScene, const char* szMapPath, float fOffX, float fOffZ);

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Set / Get absolute position
	void SetPos(const A3DVECTOR3& vPos);
	A3DVECTOR3 GetPos() ;//{ return m_vPos; }
	A3DVECTOR3 GetLoadPos() { return m_vReservedPos; }
	//	Set / Get absolute forward and up direction
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	A3DVECTOR3 GetDir() { return m_vDir; } 
	A3DVECTOR3 GetUp() { return m_vUp; }

	//	Set / Get ornament ID
	void SetOrnamentID(DWORD dwID) { m_dwOnmtID = dwID; }
	DWORD GetOrnamentID() { return m_dwOnmtID; }
	//	Get building model
	CELBrushBuilding* GetBrushBuilding() { return m_pBrushBuilding; }
	//	Get reflect flag
	bool GetReflectFlag() { return m_bReflect; }
	//	Get refract flag
	bool GetRefractFlag() { return m_bRefract; }
	//	Get auto reflect flag
	bool IsAboveWater() { return m_bAboveWater; }
	//	Get refract flag
	bool IsUnderWater() { return m_bUnderWater; }
	//	whether added to brush man
	bool IsAddedToBrushMan() const { return m_bAddedToBrushMan; }
	bool IsAddedToTriMan() const { return m_bAddedToTriMan; }
	void SetAddedToTriMan(bool b) { m_bAddedToTriMan = b; }

	void LoadInThread(bool bInLoaderThread);
	void ReleaseInThread();

	void GetOffSet(float& x,float& z)
	{
		x = m_fOffSetX;
		z = m_fOffSetZ;
	}
protected:	//	Attributes

	CECOrnamentMan*		m_pOnmtMan;		//	Ornament manager
	CELBrushBuilding *	m_pBrushBuilding; // building model using brush cd
	A3DVECTOR3			m_vPos;			//	Position
	A3DVECTOR3			m_vDir;			//	Direction and up
	A3DVECTOR3			m_vUp;

	DWORD	m_dwOnmtID;		//	Ornament ID
	bool	m_bReflect;		//	Reflect flag
	bool	m_bRefract;		//	Reflact flag

	bool	m_bAboveWater;	//	flag indicates whether some part of this ornament is above water
	bool	m_bUnderWater;	//	flag indicates whether some part of this ornament is under water

	bool	m_bAddedToBrushMan;
	bool	m_bAddedToTriMan;

	AString		m_strFileToLoad;
	A3DVECTOR3	m_vReservedPos;
	A3DVECTOR3	m_vReservedDir;
	A3DVECTOR3	m_vReservedUp;

	float m_fOffSetX;
	float m_fOffSetZ;
protected:	//	Operations

	//	Release object
	virtual void Release();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

