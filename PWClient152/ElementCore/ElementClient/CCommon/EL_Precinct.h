/*
 * FILE: EL_Precinct.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_PRECINCT_H_
#define _EL_PRECINCT_H_

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

#define ELPCTFILE_VERSION		7

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//	Data file header
struct PRECINCTFILEHEADER
{
	unsigned int	dwVersion;		//	File version
	int				iNumPrecinct;	//	Number of NPC generator
};

struct PRECINCTFILEHEADER5
{
	unsigned int	dwVersion;		//	File version
	int				iNumPrecinct;	//	Number of NPC generator
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
//	Class CELPrecinct
//	
///////////////////////////////////////////////////////////////////////////

class CELPrecinct
{
public:		//	Types

#ifdef _ELEMENTCLIENT

	//	Music loop type
	enum
	{
		LOOP_NONE = 0,
		LOOP_WHOLE,
		LOOP_SKIPFIRST,
	};

	struct MARK
	{
		ACString	strName;
		A3DVECTOR3	vPos;
	};

#endif

	struct VECTOR3
	{
		float x, y, z;
	};

public:		//	Constructor and Destructor

	CELPrecinct();
	virtual ~CELPrecinct();

public:		//	Attributes

public:		//	Operations

	//	Is specified position in this precinct ?
	bool IsPointIn(float x, float z);

	//	Get precinct priority
	int GetPriority() { return m_iPriority; }
	//	Get ID of instance m_vCityPos belongs to
	int GetDstInstanceID() { return m_idDstInst; }
	//	Get ID of source instance
	int GetSrcInstanceID() { return m_idSrcInst; }
	//	Get city position
	const VECTOR3& GetCityPos() { return m_vCityPos; }
	//	Get domain id
	int GetDomainID() { return m_idDomain; }
	//  是否是PK保护区
	bool IsPKProtect(){ return m_bPKProtect; }

#ifdef _ELEMENTCLIENT

	bool Load(AWScriptFile* pFile, int iVersion);

	const ACHAR* GetName() { return m_strName; }
	DWORD GetID() { return m_dwID; }

	bool GetNightSFX() { return m_bNightSFX; }
	void SetNightSFX(bool bFlag) { m_bNightSFX = bFlag; }
	const char* GetSoundFile() { return m_strSound; }
	const char* GetSoundFile_n() { return m_strSound_n; }
	int GetMusicFileNum() { return m_aMusicFiles.GetSize(); }
	const char* GetMusicFile(int n) { return *m_aMusicFiles[n]; }
	int GetMusicInterval() { return m_iMusicInter; }
	int GetMusicLoopType() { return m_iMusicLoop; }
	
	int GetMarkNum() { return m_aMarks.GetSize(); }
	MARK* GetMark(int n) { return m_aMarks[n]; }

#else

	//	Load data from file
	bool Load(FILE* fp, int iVersion);

#endif

protected:	//	Attributes

#ifdef _ELEMENTCLIENT

	ACString			m_strName;
	APtrArray<MARK*>	m_aMarks;
	AString				m_strSound;
	AString				m_strSound_n;
	APtrArray<AString*>	m_aMusicFiles;
	DWORD 				m_dwID;
	int					m_iMusicInter;		//	Music interval
	int					m_iMusicLoop;		//	Music loop

	bool				m_bNightSFX;		//	flag indicates current night sfx is activated

#endif

	int		m_idDstInst;	//	ID of instance m_vCityPos belongs to
	int		m_idSrcInst;	//	ID of source instance
	int		m_iPriority;	//	Precinct priority
	VECTOR3	m_vCityPos;		//	City position
	int		m_idDomain;		//	ID of domain
	bool	m_bPKProtect;   //  是否是PK保护区
	
	float	m_fLeft;		//	Bound box of precinct
	float	m_fTop;
	float	m_fRight;
	float	m_fBottom;

	abase::vector<VECTOR3>	m_aPoints;	//	Precinct points

protected:	//	Operations

	bool IsCrossLine(float x, float z, int iIndex);
	//	Build precinct bound box
	void BuildBoundBox();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELPrecinctSet
//	
///////////////////////////////////////////////////////////////////////////

class CELPrecinctSet
{
public:		//	Types

public:		//	Constructor and Destructor

	CELPrecinctSet();
	virtual ~CELPrecinctSet();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFileName);

	//	Get the precinct specified position is in
	CELPrecinct* IsPointIn(float x, float z, int idSrcInst);

	//	Get time stamp
	unsigned int GetTimeStamp() { return m_dwTimeStamp; }
	//	Get precinct number
	int GetPrecinctNum() { return m_aPrecincts.size(); }
	//	Get precinct
	CELPrecinct* GetPrecinct(int n) { return m_aPrecincts[n]; } 

protected:	//	Attributes

	unsigned int	m_dwTimeStamp;		//	Time stamp of precinct data

	abase::vector<CELPrecinct*>	m_aPrecincts;	//	Precinct array

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_EL_PRECINCT_H_
