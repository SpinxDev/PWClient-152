/*
 * FILE: LightMapMergerA.h
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
class CLightMapMerger;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CLightMapMergerA
//	
///////////////////////////////////////////////////////////////////////////

class CLightMapMergerA
{
public:		//	Types

public:		//	Constructor and Destructor

	CLightMapMergerA();
	virtual ~CLightMapMergerA();

public:		//	Attributes

public:		//	Operations

	//	Do export
	bool DoExport(CLightMapMerger* pMerger, bool bDay);

protected:	//	Attributes

	CLightMapMerger*	m_pm;

	CELBitmap*	m_pLMBmp;			//	Light map bitmap
	int			m_iLMSize;			//	Sub-terrain Light map width and height in pixels
	BYTE*		m_pLMBuf;			//	Light map buffer
	int			m_iMaskLMSize;		//	Width and height of mask area lightmap

	int			m_iProjectCnt;		//	Export project counter
	int			m_iProjRowCnt;		//	Project row counter
	int			m_iProjColCnt;		//	Project column counter
	FILE*		m_fpLM;				//	Lightmap file pointer
	DWORD*		m_aLMOffsets;		//	Offset of mask area lightmaps

	T2LMFILEHEADER2	m_LMFileHeader;		//	Lightmap file header

protected:	//	Operations

	//	Allocate exporting resources
	bool AllocExportRes();
	//	Release resources used during exporting terrain
	void ReleaseExportRes();

	//	Export lightmap
	bool ExportLightmaps();
	//	Export lightmap data of a project
	bool ExportProject(int iProject);
	//	Generate project lightmap
	bool GenProjectLightmap(int iProject);
	//	End exporting lightmap file
	bool EndExportLightmapFile();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


