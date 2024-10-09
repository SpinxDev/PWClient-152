/*
 * FILE: LightMapMerger.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "A3DTerrain2File.h"

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

class CELBitmap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CLightMapMerger
//	
///////////////////////////////////////////////////////////////////////////

class CLightMapMerger
{
public:		//	Types

	//	Initialize data
	struct INITDATA
	{
		const char*	szName;		//	Exporting data name
		const char*	szDestDir;	//	Destination exporting data directory

		int		iNumRow;		//	Number of terrain row
		int		iNumCol;		//	Number of terrain column
		int		iProjMaskCol;	//	Column number of mask area in a project
		int		iProjMaskRow;	//	Row number of mask area in a project
		int		iSubTrnGrid;	
	};

	friend class CLightMapMergerA;
	friend class CLightMapMergerB;

public:		//	Constructor and Destructor

	CLightMapMerger();
	virtual ~CLightMapMerger();

public:		//	Attributes

public:		//	Operations

	//	Calculate lightmap size
	static int CalcLightmapSize(int iMaskNum, int iMaskGrid);
	//	Read lightmap data from .t2lm file
	static bool ReadLightmapData(CELBitmap* pBmp, FILE* fp, int iMaskNum, int iMaskLMSize);

	//	Initialize object
	bool Init(const INITDATA& InitData);
	//	Release object
	void Release();

	//	Add project name
	void AddProject(const char* szName);
	//	Do export
	bool DoExport();

protected:	//	Attributes

	CString		m_strName;			//	Exporting data name
	CString		m_strDestDir;		//	Destination exporting data directory
	INITDATA	m_InitData;			//	Initial data
	int			m_iNumProject;		//	Number of project in whole world
	int			m_iNumProjMask;		//	Number of mask area in a project
	int			m_iNumMask;			//	Total mumber of mask area in whole world

	APtrArray<CString*>		m_aProjNames;		//	Project name array

protected:	//	Operations

	//	Fill lightmap with white color
	void FillWhiteLightmap(CELBitmap* pBmp);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


