/*
 * FILE: EL_Precinct.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_REGION_H_
#define _EL_REGION_H_

#include "vector.h"
#include <stdio.h>

#ifdef _ELEMENTCLIENT
#include "AAssist.h"
#include "A3DVector.h"
#include "AArray.h"
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define ELRGNFILE_VERSION		5

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//	Data file header
struct REGIONFILEHEADER1
{
	unsigned int	dwVersion;		//	File version
	int				iNumRegion;		//	Number of region
};

struct REGIONFILEHEADER2
{
	unsigned int	dwVersion;		//	File version
	int				iNumRegion;		//	Number of region
	int				iNumTrans;		//	Number of transport box
};

struct REGIONFILEHEADER4
{
	unsigned int	dwVersion;		//	File version
	int				iNumRegion;		//	Number of region
	int				iNumTrans;		//	Number of transport box
	unsigned int	dwTimeStamp;	//	Time stamp of this data file
};

#pragma pack()

#ifdef _ELEMENTCLIENT
class AWScriptFile;
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CELRegion
//	
///////////////////////////////////////////////////////////////////////////

class CELRegion
{
public:		//	Types

	//	Region type
	enum
	{
		REGION_PK = 0,
		REGION_SANCTUARY,
	};

	struct VECTOR3
	{
		float x, y, z;
	};

public:		//	Constructor and Destructor

	CELRegion();
	virtual ~CELRegion();

public:		//	Attributes

public:		//	Operations

	//	Is specified position in this region ?
	bool IsPointIn(float x, float z);
	//	Get region type
	int GetType() { return m_iType; }

#ifdef _ELEMENTCLIENT

	bool Load(AWScriptFile* pFile, int iVersion);

	const ACHAR* GetName() { return m_strName; }

#else

	//	Load data from file
	bool Load(FILE* fp, int iVersion);

#endif

protected:	//	Attributes

#ifdef _ELEMENTCLIENT

	ACString	m_strName;

#endif

	int		m_iType;		//	Region type
	
	float	m_fLeft;		//	Bound box of precinct
	float	m_fTop;
	float	m_fRight;
	float	m_fBottom;

	abase::vector<VECTOR3>	m_aPoints;	//	Region points

protected:	//	Operations

	bool IsCrossLine(float x, float z, int iIndex);
	//	Build precinct bound box
	void BuildBoundBox();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELTransportBox
//	
///////////////////////////////////////////////////////////////////////////

class CELTransportBox
{
public:		//	Types

	struct VECTOR3
	{
		float x, y, z;
	};

public:		//	Constructor and Destructor

	CELTransportBox();
	virtual ~CELTransportBox();

public:		//	Attributes

public:		//	Operations

	//	Is specified position in this transport box ?
	bool IsPointIn(float x, float y, float z)
	{
		if (x < m_vPos.x - m_vExts.x || x > m_vPos.x + m_vExts.x ||
			y < m_vPos.y - m_vExts.y || y > m_vPos.y + m_vExts.y ||
			z < m_vPos.z - m_vExts.z || z > m_vPos.z + m_vExts.z)
			return false;

		return true;
	}

#ifdef _ELEMENTCLIENT

	bool Load(AWScriptFile* pFile, int iVersion);

#else

	//	Load data from file
	bool Load(FILE* fp, int iVersion);

#endif

	//	Get instance id
	int GetInstanceID() { return m_idInst; }
	//	Get source instance id
	int GetSrcInstanceID() { return m_idSrcInst; }
	//	Get level limit
	int GetLevelLimit() { return m_iLevelLmt; }
	//	Get target position
	const VECTOR3& GetTargetPos() { return m_vTarget; }
	//	Get area position
	const VECTOR3& GetPos() { return m_vPos; }
	//	Get area extents
	const VECTOR3& GetExtents() { return m_vExts; }

	//	Set index in manager
	void SetIndex(int iIndex) { m_iIndex = iIndex; }
	//	Get index in manager
	int GetIndex() { return m_iIndex; }

protected:	//	Attributes

	int			m_idInst;		//	Instance id
	int			m_idSrcInst;	//	Source instance id
	int			m_iLevelLmt;	//	Level limit
	VECTOR3		m_vTarget;		//	Target position
	VECTOR3		m_vPos;			//	Area position
	VECTOR3		m_vExts;		//	Transport box area extents
	int			m_iIndex;		//	Transport box's index in manager

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELRegionSet
//	
///////////////////////////////////////////////////////////////////////////

class CELRegionSet
{
public:		//	Types

public:		//	Constructor and Destructor

	CELRegionSet();
	virtual ~CELRegionSet();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFileName);

	//	Get time stamp
	unsigned int GetTimeStamp() { return m_dwTimeStamp; }

	//	Get the region specified position is in
	CELRegion* IsPointInRegion(float x, float z);
	//	Get the PK region specified position is in
	CELRegion* IsPointInPKRegion(float x, float z);
	//	Get the peace region specified position is in
	CELRegion* IsPointInSanctuary(float x, float z);
	//	Get region number
	int GetRegionNum() { return m_aRegions.size(); }
	//	Get region by index
	CELRegion* GetRegion(int n)
	{
		if (n >= 0 && n < (int)m_aRegions.size())
			return m_aRegions[n];
		else 
			return NULL;
	} 

	//	Get the transport box specified position is in
	CELTransportBox* IsPointInTransport(float x, float y, float z, int idSrcInst);
	//	Get transport box number
	int GetTransportBoxNum() { return m_aTransBoxes.size(); }
	//	Get transport box by index
	CELTransportBox* GetTransportBox(int n)
	{
		if (n >= 0 && n < (int)m_aTransBoxes.size())
			return m_aTransBoxes[n];
		else
			return NULL;
	}

protected:	//	Attributes

	unsigned int	m_dwTimeStamp;		//	Time stamp of data

	abase::vector<CELRegion*>	m_aRegions;			//	Region array
	abase::vector<CELRegion*>	m_aPKRegions;		//	PK region array
	abase::vector<CELRegion*>	m_aPeaceRegions;	//	Peace region array

	abase::vector<CELTransportBox*>	m_aTransBoxes;	//	Transport boxes
	
protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_EL_REGION_H_
