/*
 * FILE: EC_HomeOrnament.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2006/6/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_SceneObject.h"
#include "A3DMatrix.h"
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
class CAutoScene;
class AFile;
class CELBuildingWithBrush;

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

class CECHomeOrnament : public CECSceneObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHomeOrnament(CECOrnamentMan* pOrnamentMan);
	virtual ~CECHomeOrnament();

public:		//	Attributes

public:		//	Operations

	//	Load ornament data from file
	bool Load(CAutoScene* pScene, const char* szMox, const char* szCHF, const A3DMATRIX4& matTM);

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Set / Get ornament ID
	void SetOrnamentID(DWORD dwID) { m_dwOnmtID = dwID; }
	DWORD GetOrnamentID() { return m_dwOnmtID; }
	//	Get building model
	CELBuildingWithBrush* GetBuildingWithBrush() { return m_pBuildingWithBrush; }
	
	void LoadInThread(bool bInLoaderThread);
	void ReleaseInThread();

protected:	//	Attributes

	CECOrnamentMan*		m_pOnmtMan;		//	Ornament manager

	CELBuildingWithBrush * m_pBuildingWithBrush; // EL_Building with brush cd dynamic created

	AString		m_strMoxFile;
	AString		m_strCHFFile;

	A3DMATRIX4	m_LoadTM;

	DWORD	m_dwOnmtID;		//	Ornament ID

protected:	//	Operations

	//	Release object
	virtual void Release();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

