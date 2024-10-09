/*
 * FILE: AIGenExportor.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

#include "AArray.h"
#include "A3DVector.h"
#include "NPCGenData.h"
#include "EL_Precinct.h"
#include "EL_Region.h"
#include "SevBezier.h"
#include "hashmap.h"
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

class CExpElementMap;
class CEditerBezier;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CAIGenExportor
//	
///////////////////////////////////////////////////////////////////////////

class CAIGenExportor
{
public:		//	Types
	//	Initialize data
	struct INITDATA
	{
		const char*	szExpDir;		//	Exporting directory
		const char* szWorkingDir;
		int		iNumRow;			//	Number of terrain row
		int		iNumCol;			//	Number of terrain column
	};

	//	NPC area
	struct NPCAREA
	{
		NPCGENFILEAREA7	Area;
		int				iFirstGen;
	};

	//	Resource area
	struct RESAREA
	{
		NPCGENFILERESAREA7	Area;
		int					iFirstRes;
	};

	//	Bezier route
	struct BEZIERRT
	{
		CEditerBezier*	pBezier;
		A3DVECTOR3		vOffset;
	};

	// Account mounster number
	struct MOUNSTER_STATUS
	{
		abase::vector<char*> maplist;
		DWORD num;
	};
	
	struct NAVIGATEBEZIERFILEHEADER
	{
		int		iVersion;		//	Version
		int		iNumBezier;		//	Number of bezier route
	};

public:		//	Constructor and Destructor

	CAIGenExportor();
	virtual ~CAIGenExportor();

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

protected:	//	Attributes

	CString		m_strExpDir;		//	Exporting directory
	INITDATA	m_InitData;			//	Initial data

	int			m_iNumProject;		//	Number of project in whole world
	int			m_iProjectCnt;		//	Export project counter
	int			m_iProjRowCnt;		//	Project row counter
	int			m_iProjColCnt;		//	Project column counter
	float		m_fProjTrnSize;		//	Terrain size in every project
	float		m_fWorldWid;		//	World size in meters
	float		m_fWorldLen;
	
	APtrArray<CString*>	m_aProjNames;	//	Project name array
	NPCGENFILEHEADER7	m_FileHeader;	//	AI generator file header
	FILE*				m_fpAIGen;		//	AI generator file
	SEVBEZIERFILEHEADER	m_BFHeader;		//	Bezier data file header
	FILE*				m_fpBezier;		//	Bezier data file
	DWORD				m_dwExportID;	//	Export ID

	NAVIGATEBEZIERFILEHEADER	m_BFHeaderNavigate;		//	Bezier data file header for force navigate
	FILE*				m_fpBezierNavigate;		//	Bezier data file for force navigate

	AArray<BEZIERRT, BEZIERRT&>						m_aExpBeziers;
	AArray<NPCAREA, NPCAREA&>						m_aExpNPCAreas;
	AArray<NPCGENFILEAIGEN, NPCGENFILEAIGEN&>		m_aExpNPCGens;
	AArray<RESAREA, RESAREA&>						m_aExpResAreas;
	AArray<NPCGENFILERES, NPCGENFILERES&>			m_aExpRes;
	AArray<NPCGENFILEDYNOBJ10, NPCGENFILEDYNOBJ10&>	m_aExpDynObjs;
	AArray<NPCGENFILECTRL8, NPCGENFILECTRL8&>		m_aExpCtrls;

	AArray<BEZIERRT, BEZIERRT>						m_aExpBeziersForceNavigate; // just for client

	PRECINCTFILEHEADER5	m_PCTFHeader;	//	Precinct file header
	FILE*				m_fpPCTF_s;		//	Precinct file object (server version)
	FILE*				m_fpPCTF_c;		//	Precinct file object (client version)
	REGIONFILEHEADER4	m_RGNFHeader;	//	Region file header
	FILE*				m_fpRgn_s;		//	Region file object (server version)
	FILE*				m_fpRgn_c;		//	Region file object (client version)
	
	abase::hash_map<int , MOUNSTER_STATUS*> mounster_s;
protected:	//	Operations

	bool BeginExportNpcGen(int iProj);

	//	Release resources used during exporting terrain
	void ReleaseExportRes();
	//	Generate export ID
	void GenerateExportID(CExpElementMap* pMap, int iProj);
	//	Add exp bezier object
	void AddExpBezier(CEditerBezier* pBeizer, const A3DVECTOR3& vOff);

	//	Export all AI generator data
	bool ExportAIGenerator();
	//	Export scene data of a project
	bool ExportProject(int iProject);
	//	Export AI generators of this project
	bool ExportProjectAIGens(CExpElementMap* pMap);
	//	Export precinct data of this project
	bool ExportProjectPrecincts(CExpElementMap* pMap);
	//	Export region data of this project
	bool ExportProjectRegions(CExpElementMap* pMap);
	//	Export transport box data of this project
	bool ExportTransportBoxes(CExpElementMap* pMap);
	//	Export bezier data of this project
	bool ExportProjectBezier(CExpElementMap* pMap);
	//	Export npc controller data table
	bool ExportControllers();
	//	End export world file
	bool EndExportAIGenFile(bool bExpCtrl);
	//	End export precinct file
	bool EndExportPrecinctFile();
	//	End export region file
	bool EndExportRegionFile();
	//	End export bezier route
	bool EndExportBezierFile();
	//  Exp space path map route
	bool ExportSpaccePathMap(CExpElementMap* pMap, int iProj);
	//   Account mounster number
    void AddMounsterToMap(int id, int num, const char *szMap, const char *szObj);
	//   Save mounster status to file
	void DumpMounsterStatus();
	//   Account mounster number in map
    void AccountMounster(CExpElementMap* pMap);
	//   Is current export copy?
	bool IsExpCopy(int nCopy);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

