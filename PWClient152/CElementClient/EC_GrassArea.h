/*
 * FILE: EC_GrassArea.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_SceneObject.h"
#include "A3DVector.h"

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

class AFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGrassArea
//	
///////////////////////////////////////////////////////////////////////////

class CECGrassArea : public CECSceneObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CECGrassArea();
	virtual ~CECGrassArea() {}

public:		//	Attributes

public:		//	Operations

	//	Get / Set grass area ID
	void SetGrassID(DWORD dwID) { m_dwGrassID = dwID; }
	DWORD GetGrassID() { return m_dwGrassID; }
	//	Get / Set grass type ID
	void SetGrassType(DWORD dwType) { m_dwGrassType = dwType; }
	DWORD GetGrassType() { return m_dwGrassType; }
	//	Set / Get sub-terrain index
	void SetSubTerrain(int iIndex) { m_iSubTerrain = iIndex; }
	int GetSubTerrain() { return m_iSubTerrain; }

protected:	//	Attributes

	DWORD	m_dwGrassID;	//	Grass area ID
	DWORD	m_dwGrassType;	//	Grass type ID
	int		m_iSubTerrain;	//	Index of sub-terrain this water area belongs to

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECWaterArea
//	
///////////////////////////////////////////////////////////////////////////

class CECWaterArea : public CECSceneObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CECWaterArea();
	virtual ~CECWaterArea() {}

public:		//	Attributes

public:		//	Operations

	//	Set / Get sub-terrain index
	void SetSubTerrain(int iIndex) { m_iSubTerrain = iIndex; }
	int GetSubTerrain() { return m_iSubTerrain; }

protected:	//	Attributes

	int		m_iSubTerrain;	//	Index of sub-terrain this water area belongs to

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

