/*
 * FILE: TerrainExportor.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
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
class CSceneObjectManager;
class CExpTerrain;
class CLightMapGen;
class CELBitmap;
class CExpElementMap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrainExportor
//	
///////////////////////////////////////////////////////////////////////////

struct MAPCACHE_ITEM
{
	CExpElementMap* m_pMap;
	CString         strProjName;
};

class CElMapCache
{
public:
	
	CElMapCache();
	~CElMapCache();

public:
	CExpElementMap * GetMap(const CString& strProjName);
	void             AddProject(const CString *strProjName);
	CExpElementMap*  AddStartMap(const CString& strProjName);
	void             Release();
	void			 ReleaseOthers(const CString& strProject);

protected:
	MAPCACHE_ITEM m_pMapList[9];
};


class CTerrainExportor
{
public:		//	Types

	//	Initialize data
	struct INITDATA
	{
		const char*		szName;		//	Exporting data name
		const char*		szDestDir;	//	Destination exporting data directory
		CLightMapGen*	pLMGen;		//	Light map generator
		CLightMapGen*   pLMNightGen;//  Night light map generator

		int		iNumRow;		//	Number of terrain row
		int		iNumCol;		//	Number of terrain column
		int		iMaskTile;		//	Mask clip tile number
		int		iBlockGrid;		//	Block grid
		bool	bGameExp;		//	true, export data for game
	};

	friend class CTerrainExpImpA;
	friend class CTerrainExpImpB;

public:		//	Constructor and Destructor

	CTerrainExportor();
	virtual ~CTerrainExportor();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const INITDATA& InitData);
	//	Release object
	void Release();

	//	Add project name
	void AddProject(const char* szName);
	//	Do export
	bool DoExport();

	//	Get column number of mask area in a project
	int GetProjMaskCol() { return m_iProjMaskCol; }
	//	Get row number of mask area in a project
	int GetProjMaskRow() { return m_iProjMaskRow; }
	//	Get sub-terrain grid
	int GetSubTrnGrid() { return m_TrnInfo.iSubTrnGrid; }
	
	//  Get quadrant
	int GetQuadrant(bool bIsNight);

	CString                 m_strExtProj[9];    //  Added by XQF
	bool                    m_bLightNeighbor;// Added by XQF, 
	bool                    m_bLightMapTans; // 灯光图过渡开关
	CElMapCache*            m_pCache;
protected:	//	Attributes

	CString		m_strName;			//	Exporting data name
	CString		m_strDestDir;		//	Destination exporting data directory
	INITDATA	m_InitData;			//	Initial data

	FILE*		m_fpLightmap;		//	Light map file pointer
	DWORD*		m_aLMOffs;			//	Mask area light map offsets in file

	int			m_iProjNum;			//	Project number
	int			m_iProjectCnt;		//	Export project counter
	int			m_iProjRowCnt;		//	Project row counter
	int			m_iProjColCnt;		//	Project column counter
	int			m_iProjMaskArea;	//	Mask area number in every project
	int			m_iProjBlock;		//	Block number in every project
	float		m_fProjTrnSize;		//	Terrain size in every project
	int			m_iProjHMSize;		//	Height Map size in every project
	int			m_iProjMaskCol;		//	Column number of mask area in a project
	int			m_iProjMaskRow;		//	Row number of mask area in a project

	APtrArray<CString*>		m_aProjNames;	//	Project name array

	TRN2FILEHEADER		m_TrnInfo;			//	Terrain File header
	T2LMFILEHEADER2		m_LMFileHeader;		//	Light map file header

protected:	//	Operations

	//	Export light maps
	bool ExportLightMaps(bool bIsNight);
	//	Export a project's light map
	bool ExportProjectLightMap(int iProject,bool bIsNight);
	//	Export a project mask area's light map
	bool ExportProjectMaskLightMap(int iMaskArea, int iRow, int iCol,bool bIsNight);
	//	Set project used to calculate light map
	bool SetLightMapProject(int iPos, int r, int c,bool bIsNight);
	//	Allocate exporting light map resources
	bool AllocLightmapExportRes();
	//	Release resources used during exporting light map
	void ReleaseLightmapExportRes();
	//对5张图做过渡处理，处理结果放到0号图中
	void ProcessELBitmap(CELBitmap* listLightMap[]);

	void TansUpEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansLeftEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansRightEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansRightEdge2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansBottomEdge(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansBottomEdge2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansRightBottomCorner(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	void TansRightBottomCorner2(CELBitmap *pDestBitmap, CELBitmap *pSrcBitmap, int iTansPixel);
	CELBitmap* CopyELBitmap(CELBitmap *pSrcBitmap);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

