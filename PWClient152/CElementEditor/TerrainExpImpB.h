/*
 * FILE: TerrainExpImpB.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/22
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
class CELBitmap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainExpImpB
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainExpImpB
{
public:		//	Types

public:		//	Constructor and Destructor

	CTerrainExpImpB();
	virtual ~CTerrainExpImpB();

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
	int			m_iMaskMapSize;		//	Mask map size

	FILE*		m_fpTerrain;		//	Terrain file pointer
	FILE*		m_fpBlock;			//	Block data flie pointer
	FILE*		m_fpMaskMap;		//	Mask map file pointer

	CTerrainExportor*					m_pe;
	TRN2FILEHEADER						m_T2FileHeader;		//	Terrain File header
	A3DTRN2LOADERB::T2BKFILEHEADER5		m_BKFileHeader;		//	Block file header
	A3DTRN2LOADERB::T2MKFILEHEADER2		m_MKFileHeader;		//	Mask file header
	A3DTRN2LOADERB::T2MKFILEMASKAREA*	m_aMaskAreas;		//	Mask area data
	A3DTRN2LOADERB::T2MKFILELAYER*		m_aProjLayers;		//	Layers of a project
	AArray<int, int>					m_aLayerMap;		//	Mask layer map
	int*								m_aFirstIdx;		//	First indices

	TRN2FILESUBTRN*			m_aSubTerrains;		//	Sub-terrains
	DWORD*					m_aBlockOffs;		//	Block data offsets in file
	DWORD*					m_aMaskAreaOffs;	//	Mask area data offsets in file
	APtrArray<CString*>		m_aTextureFile;		//	Texture file name array
	CELBitmap*				m_pLMBmp;			//	Current project's lightmap for day
	CELBitmap*				m_pLMBmp1;			//	Current project's lightmap for night

	BYTE*		m_pCompHei;		//	Buffer used to store compressed block's height data
	BYTE*		m_pCompNormal;	//	Buffer used to store compressed block's normal data
	BYTE*		m_pCompDiff;	//	Buffer used to store compressed block's diffuse data for day
	BYTE*		m_pCompDiff1;	//	Buffer used to store compressed block's diffuse data for night

protected:	//	Operations

	//	Check project name collision
	bool CheckProjNameCollision();

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
	bool ExportProjectBlock(CExpTerrain* pTerrain, int iBlock, int iTrnRow, int iTrnCol, int iRow, int iCol);
	//	End export block data file
	bool EndBlockDataFile();
	//	End export mask map file
	bool EndExportMaskMapFile();
	//	End export terrain file
	bool EndExportTerrainFile();
	//	Allocate exporting terrain resources
	bool AllocTerrainExportRes();
	//	Release resources used during exporting terrain
	void ReleaseTerrainExportRes();
	//	Allocate exporting project resources
	bool AllocProjectExportRes(int iProject, CExpTerrain* pTerrain);
	//	Release resources used during exporting a project
	void ReleaseProjectExportRes();
	//	Create project lightmap
	bool CreateLightmap(const char* szFile, bool bDay);
	//	Get vertex color
	DWORD GetVertexColor(float fr, float fc, bool bDay);

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

