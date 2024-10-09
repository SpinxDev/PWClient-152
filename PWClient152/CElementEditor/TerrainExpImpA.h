/*
 * FILE: TerrainExpImpA.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

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

class CExpTerrain;
class CTerrainExportor;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainExpImpA
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainExpImpA
{
public:		//	Types

public:		//	Constructor and Destructor

	CTerrainExpImpA();
	virtual ~CTerrainExpImpA();

public:		//	Attributes

public:		//	Operations

	//	Do export
	bool DoExport(CTerrainExportor* pExporter, TRN2FILEHEADER* pTrnInfo);

	//	Get Height Map size in every project
	int GetProjHMSize() { return m_iProjHMSize; }
	//	Get column number of mask area in a project
	int GetProjMaskCol() { return m_iProjMaskCol; }
	//	Get row number of mask area in a project
	int GetProjMaskRow() { return m_iProjMaskRow; }

protected:	//	Attributes

	int			m_iProjMaskArea;	//	Number of mask area in a project
	int			m_iProjBlock;		//	Number of block in a project
	int			m_iProjectCnt;		//	Export project counter
	int			m_iProjRowCnt;		//	Project row counter
	int			m_iProjColCnt;		//	Project column counter
	float		m_fProjTrnSize;		//	Terrain size in every project
	int			m_iProjHMSize;		//	Height Map size in every project
	int			m_iProjMaskCol;		//	Column number of mask area in a project
	int			m_iProjMaskRow;		//	Row number of mask area in a project

	FILE*		m_fpTerrain;		//	Terrain file pointer
	FILE*		m_fpMaskMap;		//	Mask map file pointer

	CTerrainExportor*		m_pe;
	TRN2FILEHEADER			m_T2FileHeader;		//	Terrain File header
	A3DTRN2LOADERA::T2MKFILEHEADER		m_MKFileHeader;		//	Mask file header
	A3DTRN2LOADERA::T2MKFILEMASKAREA*	m_aMaskAreas;		//	Mask area data

	TRN2FILESUBTRN*			m_aSubTerrains;		//	Sub-terrains
	DWORD*					m_aBlockOffs;		//	Block data offsets in file
	DWORD*					m_aMaskAreaOffs;	//	Mask area data offsets in file
	AArray<int, int>		m_aTexIndices;		//	Texture index lump
	AArray<int, int>		m_aSpecuMaps;		//	Specular map index lump
	AArray<int, int>		m_aMaskSizes;		//	Mask map image size lump
	AArray<DWORD, DWORD>	m_aMaskMapOffs;		//	Mask map image offset lump
	AArray<BYTE, BYTE>		m_aProjAxises;		//	Porjection axis lump
	AArray<WORD, WORD>		m_aLayerMaps;		//	Layer maps
	AArray<BYTE, BYTE>		m_aLayerWeights;	//	Layer weight

	AArray<A3DTRN2LOADERA::T2MKFILEUVSCALE, A3DTRN2LOADERA::T2MKFILEUVSCALE&>	m_aUVScales;	//	uv-scale lump
	APtrArray<CString*>		m_aTextureFile;		//	Texture file name array

protected:	//	Operations

	//	Export all terrain data
	bool ExportTerrainData();
	//	Export terrain data of a project
	bool ExportProject(int iProject);
	//	Export all mask map areas in a project terrain
	bool ExportProjectMaskAreas(CExpTerrain* pTerrain);
	//	Export one mask map area in a project terrain
	bool ExportProjectMaskArea(CExpTerrain* pTerrain, int iMaskArea, int iRow, int iCol);
	//	Export all blocks in a project terrain
	bool ExportProjectBlocks(CExpTerrain* pTerrain);
	//	Export one block in a project terrain
	bool ExportProjectBlock(CExpTerrain* pTerrain, int iTrnRow, int iTrnCol, int iRow, int iCol);
	//	Export lumps in mask map file
	bool ExportMaskMapLumps();
	//	End export mask map file
	bool EndExportMaskMapFile();
	//	End export terrain file
	bool EndExportTerrainFile();
	//	Allocate exporting terrain resources
	bool AllocTerrainExportRes();
	//	Release resources used during exporting terrain
	void ReleaseTerrainExportRes();

	//	Calculate block's LOD scale
	float CalcBlockLODScale(float* aHeis, int iWid, int iHei);
	//	Add a texture file name
	int AddTextureFile(const char* szFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

