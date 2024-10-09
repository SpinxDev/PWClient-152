/*
 * FILE: ExpElementMap.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ElementMap.h"

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
//	Class CExpElementMap
//	
///////////////////////////////////////////////////////////////////////////

class CExpElementMap : public CElementMap
{
public:		//	Types

public:		//	Constructor and Destructor

	CExpElementMap();
	virtual ~CExpElementMap();

public:		//	Attributes

public:		//	Operations

	//	Create an empty map
	virtual bool CreateEmpty(const char* szMapName, const CTerrain::INITDATA& TrnInitData);
	//	Release map object
	virtual void Release();

	//	Load data from file
	bool Load(const char* szFile, int iLoadFlag);

protected:	//	Attributes

protected:	//	Operations

	//	Load data using current loading flag
	virtual bool Load(CELArchive& ar);
	//	Save data
	virtual bool Save(CELArchive& ar) { return true; }

	//	Create map objects
	virtual bool CreateMapObjects();
	//	Release map objects
	virtual void ReleaseMapObjects();
	//	Load terrain data
	virtual bool LoadTerrain(CELArchive& ar);
	//	Save terrain data
	virtual bool SaveTerrain(CELArchive& ar) { return true; }
	//	Load terrain data
	virtual bool LoadTerrainWater(CELArchive& ar);
	//	Save terrain data
	virtual bool SaveTerrainWater(CELArchive& ar) { return true; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

