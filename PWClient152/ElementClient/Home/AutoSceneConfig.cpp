/*
 * FILE: AutoSceneConfig.h
 *
 * DESCRIPTION: Class for scene config
 *
 * CREATED BY: Jiang Dalong, 2006/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoSceneConfig.h"
#include "AutoSceneFunc.h"
#include <AFileImage.h>
#include <AIniFile.h>
#include <A3DMacros.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoSceneConfig::CAutoSceneConfig()
{
	m_pAutoTreeTypeList			= NULL;
	m_pAutoLandGrassTypeList	= NULL;
	m_pAutoWaterGrassTypeList	= NULL;
	m_pAutoTextureTypeList		= NULL;
	m_pAutoModelTypeList		= NULL;
	m_pAutoSkyTypeList			= NULL;
}

CAutoSceneConfig::~CAutoSceneConfig()
{
	Release();
}

void CAutoSceneConfig::Release()
{
	SAFERELEASE(m_pAutoTreeTypeList);
	SAFERELEASE(m_pAutoLandGrassTypeList);
	SAFERELEASE(m_pAutoWaterGrassTypeList);
	SAFERELEASE(m_pAutoTextureTypeList);
	SAFERELEASE(m_pAutoModelTypeList);
}

// Load tree type list
bool CAutoSceneConfig::LoadTreeTypeList(const char* szFile)
{
	AFileImage fileToLoad;
	if (!fileToLoad.Open(szFile, AFILE_OPENEXIST))
		return false;
	
	char	szLine[256];
	DWORD	dwReadLen;
	int i;
	
	// Read number of tree types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoTreeTypeList);

	SAFERELEASE(m_pAutoTreeTypeList);
	m_pAutoTreeTypeList = new AUTOTREETYPELIST[m_nNumAutoTreeTypeList];

	// Read tree infos
	for (i=0; i<m_nNumAutoTreeTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoTreeTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoTreeTypeList[i].strName = szLine;
		// Read spt file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoTreeTypeList[i].strSptFile = szLine;
		// Read dds file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoTreeTypeList[i].strDdsFile = szLine;
		// Read in earth
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%f", &m_pAutoTreeTypeList[i].fInEarth);
		// Read price
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoTreeTypeList[i].dwPrice);
	}

	fileToLoad.Close();
	return true;

READFAIL:
	fileToLoad.Close();
	return false;
}

// Load grass type list
bool CAutoSceneConfig::LoadGrassTypeList(const char* szLandGrassFile, const char* szWaterGrassFile)
{
	AFileImage fileToLoad;
	char	szLine[256];
	DWORD	dwReadLen;
	int i;
	
	if (!fileToLoad.Open(szLandGrassFile, AFILE_OPENEXIST))
		return false;
	
	// Read number of grass types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoLandGrassTypeList);

	SAFERELEASE(m_pAutoLandGrassTypeList);
	m_pAutoLandGrassTypeList = new AUTOGRASSTYPELIST[m_nNumAutoLandGrassTypeList];

	// Read grass infos
	for (i=0; i<m_nNumAutoLandGrassTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoLandGrassTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoLandGrassTypeList[i].strName = szLine;
		// Read mox file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoLandGrassTypeList[i].strMoxFile = szLine;
		// Read price
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoLandGrassTypeList[i].dwPrice);
	}

	fileToLoad.Close();

	if (!fileToLoad.Open(szWaterGrassFile, AFILE_OPENEXIST))
		return false;
	
	// Read number of grass types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoWaterGrassTypeList);

	SAFERELEASE(m_pAutoWaterGrassTypeList);
	m_pAutoWaterGrassTypeList = new AUTOGRASSTYPELIST[m_nNumAutoWaterGrassTypeList];

	// Read grass infos
	for (i=0; i<m_nNumAutoWaterGrassTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoWaterGrassTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoWaterGrassTypeList[i].strName = szLine;
		// Read mox file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoWaterGrassTypeList[i].strMoxFile = szLine;
		// Read price
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoWaterGrassTypeList[i].dwPrice);
	}
	fileToLoad.Close();

	return true;

READFAIL:
	fileToLoad.Close();
	return false;
}

// Load texture type list
bool CAutoSceneConfig::LoadTextureTypeList(const char* szFile)
{
	AFileImage fileToLoad;
	if (!fileToLoad.Open(szFile, AFILE_OPENEXIST))
		return false;
	
	char	szLine[256];
	DWORD	dwReadLen;
	int i;
	
	// Read number of texture types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoTextureTypeList);

	SAFERELEASE(m_pAutoTextureTypeList);
	m_pAutoTextureTypeList = new AUTOTEXTURETYPELIST[m_nNumAutoTextureTypeList];

	// Read texture infos
	for (i=0; i<m_nNumAutoTextureTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoTextureTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoTextureTypeList[i].strName = szLine;
		// Read texture file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoTextureTypeList[i].strTexture = szLine;
		// Read price
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoTextureTypeList[i].dwPrice);
	}
	
	fileToLoad.Close();
	return true;

READFAIL:
	fileToLoad.Close();
	return false;
}

// Load all model type list
bool CAutoSceneConfig::LoadModelTypeList(const char* szFile)
{
	AFileImage fileToLoad;
	if (!fileToLoad.Open(szFile, AFILE_OPENEXIST))
		return false;
	
	char	szLine[256];
	AString strLine;
	DWORD	dwReadLen;
	int i;
	
	// Read number of model types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoModelTypeList);

	SAFERELEASE(m_pAutoModelTypeList);
	m_pAutoModelTypeList = new AUTOMODELTYPELIST[m_nNumAutoModelTypeList];

	// Read model infos
	for (i=0; i<m_nNumAutoModelTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoModelTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoModelTypeList[i].strName = szLine;
		// Read mox file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoModelTypeList[i].strMoxFile = szLine;
		// Read chf file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoModelTypeList[i].strChfFile = szLine;
		// Read attribute
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		strLine = szLine;
		strLine.TrimLeft();
		strLine.TrimRight();
		if (strLine.CompareNoCase("房屋") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_HOUSE;
		else if (strLine.CompareNoCase("室外摆设") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_OUTDOOR;
		else if (strLine.CompareNoCase("家具") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_FURNITURE;
		else if (strLine.CompareNoCase("植物") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_PLANT;
		else if (strLine.CompareNoCase("玩具") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_TOY;
		else if (strLine.CompareNoCase("古董") == 0)
			m_pAutoModelTypeList[i].dwAttribute = MA_VIRTU;
		else
			m_pAutoModelTypeList[i].dwAttribute =  MA_OTHER;
		// Read price
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoModelTypeList[i].dwPrice);
	}

	fileToLoad.Close();
	return true;

READFAIL:
	fileToLoad.Close();
	return false;	
}

// Load sky type list
bool CAutoSceneConfig::LoadSkyTypeList(const char* szFile)
{
	AFileImage fileToLoad;
	if (!fileToLoad.Open(szFile, AFILE_OPENEXIST))
		return false;
	
	char	szLine[256];
	DWORD	dwReadLen;
	int i;
	
	// Read number of sky types
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", &m_nNumAutoSkyTypeList);

	SAFERELEASE(m_pAutoSkyTypeList);
	m_pAutoSkyTypeList = new AUTOSKYTYPELIST[m_nNumAutoSkyTypeList];

	// Read sky infos
	for (i=0; i<m_nNumAutoSkyTypeList; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &m_pAutoSkyTypeList[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoSkyTypeList[i].strName = szLine;
		// Read cap file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoSkyTypeList[i].strCap = szLine;
		// Read surroundF file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoSkyTypeList[i].strSurroundF = szLine;
		// Read surroundB file
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		m_pAutoSkyTypeList[i].strSurroundB = szLine;
		// Read price(no use)
		fileToLoad.ReadLine(szLine, 256, &dwReadLen);
	}
	
	fileToLoad.Close();
	return true;

READFAIL:
	fileToLoad.Close();
	return false;
}

// Initialize
bool CAutoSceneConfig::Init(const char* szFile)
{
	// Load scene config
	if (!LoadSceneConfig(szFile))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load scene config.");
		return false;
	}

	// Load tree type list
	if (!LoadTreeTypeList(m_strTreeListFileName))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load tree type list.");
		return false;
	}

	// Load grass type list
	if (!LoadGrassTypeList(m_strLandGrassListFileName, m_strWaterGrassListFileName))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load grass type list.");
		return false;
	}

	// Load texture type list
	if (!LoadTextureTypeList(m_strTextureListFilename))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load texture list.");
		return false;
	}

	// Load model type list
	if (!LoadModelTypeList(m_strModelListFileName))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load model type list.");
		return false;
	}

	// Load sky type list
	if (!LoadSkyTypeList(m_strSkyListFileName))
	{
		a_LogOutput(1, "CAutoSceneConfig::Init, Failed to load sky type list.");
		return false;
	}

	return true;
}

// Load scene config
bool CAutoSceneConfig::LoadSceneConfig(const char* szFile)
{
	AIniFile f;
	if (!f.Open(szFile))
		return false;
	
	// Load world information
	m_nNumBlockRow				= f.GetValueAsInt("World", "num_block_row", 0);
	m_nNumBlockCol				= f.GetValueAsInt("World", "num_block_col", 0);
	m_fWorldWid					= f.GetValueAsFloat("World", "world_width", 0.0f);
	m_fWorldLen					= f.GetValueAsFloat("World", "world_length", 0.0f);
	m_fBlockSize				= f.GetValueAsFloat("World", "block_size", 0.0f);

	// Load sun dir
	float fDir[3];
	if (!f.GetValueAsFloatArray("Sun", "sun_dir", 3, fDir))
	{
		fDir[0] = 0.152431f;
		fDir[1] = -0.619779f;
		fDir[2] = 0.769830f;
	}
	m_vSunDir = A3DVECTOR3(fDir[0], fDir[1], fDir[2]);

	// Load sun colors
	int nColor[3];
	if (!f.GetValueAsIntArray("Sun", "sun_color_day", 3, nColor))
	{
		nColor[0] = 255;
		nColor[1] = 255;
		nColor[2] = 255;
	}
	m_dwSunColorDay = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	if (!f.GetValueAsIntArray("Sun", "ambient_day", 3, nColor))
	{
		nColor[0] = 220;
		nColor[1] = 220;
		nColor[2] = 220;
	}
	m_dwAmbientDay = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	if (!f.GetValueAsIntArray("Sun", "sun_color_night", 3, nColor))
	{
		nColor[0] = 128;
		nColor[1] = 128;
		nColor[2] = 128;
	}
	m_dwSunColorNight = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	if (!f.GetValueAsIntArray("Sun", "ambient_night", 3, nColor))
	{
		nColor[0] = 128;
		nColor[1] = 128;
		nColor[2] = 128;
	}
	m_dwAmbientNight = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	
	// Load fog
	m_bFogEnable	= f.GetValueAsBoolean("Fog", "fog_enable", true);
	m_fFogStartDay	= f.GetValueAsFloat("Fog", "fog_start_day", 20.0f);
	m_fFogEndDay	= f.GetValueAsFloat("Fog", "fog_end_day", 500.0f);
	if (!f.GetValueAsIntArray("Fog", "fog_color_day", 3, nColor))
	{
		nColor[0] = 192;
		nColor[1] = 192;
		nColor[2] = 192;
	}
	m_dwFogColorDay = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	m_fFogStartNight	= f.GetValueAsFloat("Fog", "fog_start_night", 20.0f);
	m_fFogEndNight		= f.GetValueAsFloat("Fog", "fog_end_night", 500.0f);
	if (!f.GetValueAsIntArray("Fog", "fog_color_night", 3, nColor))
	{
		nColor[0] = 192;
		nColor[1] = 192;
		nColor[2] = 192;
	}
	m_dwFogColorNight = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);
	if (!f.GetValueAsIntArray("Fog", "fog_color_water", 3, nColor))
	{
		nColor[0] = 255;
		nColor[1] = 255;
		nColor[2] = 255;
	}
	m_dwFogColorWater = A3DCOLORRGBA(nColor[0], nColor[1], nColor[2], 255);

	// Load sky ID
	m_dwSkyIDDay				= f.GetValueAsInt("Sky", "sky_id_day", 0);
	m_dwSkyIDNight				= f.GetValueAsInt("Sky", "sky_id_night", 1);

	// Load texture ID
	m_dwDefaultTextureID		= f.GetValueAsInt("Texture", "default", 0);
	m_aTextureID[TT_MAIN]		= f.GetValueAsInt("Texture", "main_terrain_main", 0);
	m_aTextureID[TT_MAIN_PATCH] = f.GetValueAsInt("Texture", "main_terrain_patch", 0);
	m_aTextureID[TT_BRIDGE]		= f.GetValueAsInt("Texture", "hill_main", 0);
	m_aTextureID[TT_BRIDGE_PATCH] = f.GetValueAsInt("Texture", "hill_patch", 0);
	m_aTextureID[TT_PART]		= f.GetValueAsInt("Texture", "part_main", 0);
	m_aTextureID[TT_PART_PATCH] = f.GetValueAsInt("Texture", "part_patch", 0);
	m_aTextureID[TT_ROADBED]	= f.GetValueAsInt("Texture", "road_main", 0);
	m_aTextureID[TT_ROAD]		= f.GetValueAsInt("Texture", "road_patch", 0);
	m_aTextureID[TT_RIVERBED]	= f.GetValueAsInt("Texture", "water_main", 0);
	m_aTextureID[TT_RIVERSAND]	= f.GetValueAsInt("Texture", "water_patch", 0);

	// Load resource file name
	m_strTreeListFileName		= f.GetValueAsString("Resource", "tree_list", NULL);
	m_strLandGrassListFileName	= f.GetValueAsString("Resource", "land_grass_list", NULL);
	m_strWaterGrassListFileName	= f.GetValueAsString("Resource", "water_grass_list", NULL);
	m_strTextureListFilename	= f.GetValueAsString("Resource", "texture_list", NULL);
	m_strModelListFileName		= f.GetValueAsString("Resource", "model_list", NULL);
	m_strSkyListFileName		= f.GetValueAsString("Resource", "sky_list", NULL);
		
	// Load scene path
	m_strScenePath				= f.GetValueAsString("Resource", "scene_path", NULL);
	
	// Load border file
	m_strBorderFile				= f.GetValueAsString("Resource", "border_file", NULL);
	
	f.Close();
	return true;
}

int CAutoSceneConfig::GetAutoTreeTypeListByID(DWORD dwID, AUTOTREETYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoTreeTypeList; i++)
	{
		if (m_pAutoTreeTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoTreeTypeList[i].dwID;
				pType->strName		= m_pAutoTreeTypeList[i].strName;
				pType->strSptFile	= m_pAutoTreeTypeList[i].strSptFile;
				pType->strDdsFile	= m_pAutoTreeTypeList[i].strDdsFile;
				pType->fInEarth		= m_pAutoTreeTypeList[i].fInEarth;
				pType->dwPrice		= m_pAutoTreeTypeList[i].dwPrice;
			}
			return i;
		}
	}

	return -1;
}

int CAutoSceneConfig::GetAutoLandGrassTypeListByID(DWORD dwID, AUTOGRASSTYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoLandGrassTypeList; i++)
	{
		if (m_pAutoLandGrassTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoLandGrassTypeList[i].dwID;
				pType->strName		= m_pAutoLandGrassTypeList[i].strName;
				pType->strMoxFile	= m_pAutoLandGrassTypeList[i].strMoxFile;
				pType->dwPrice		= m_pAutoLandGrassTypeList[i].dwPrice;
			}
			return i;
		}
	}

	return -1;
}

int CAutoSceneConfig::GetAutoWaterGrassTypeListByID(DWORD dwID, AUTOGRASSTYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoWaterGrassTypeList; i++)
	{
		if (m_pAutoWaterGrassTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoWaterGrassTypeList[i].dwID;
				pType->strName		= m_pAutoWaterGrassTypeList[i].strName;
				pType->strMoxFile	= m_pAutoWaterGrassTypeList[i].strMoxFile;
				pType->dwPrice		= m_pAutoWaterGrassTypeList[i].dwPrice;
			}
			return i;
		}
	}

	return NULL;
}

int CAutoSceneConfig::GetAutoTextureTypeListByID(DWORD dwID, AUTOTEXTURETYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoTextureTypeList; i++)
	{
		if (m_pAutoTextureTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoTextureTypeList[i].dwID;
				pType->strName		= m_pAutoTextureTypeList[i].strName;
				pType->strTexture	= m_pAutoTextureTypeList[i].strTexture;
				pType->dwPrice		= m_pAutoTextureTypeList[i].dwPrice;
			}
			return i;
		}
	}

	return -1;
}

int CAutoSceneConfig::GetAutoModelTypeListByID(DWORD dwID, AUTOMODELTYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoModelTypeList; i++)
	{
		if (m_pAutoModelTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoModelTypeList[i].dwID;
				pType->strName		= m_pAutoModelTypeList[i].strName;
				pType->strMoxFile	= m_pAutoModelTypeList[i].strMoxFile;
				pType->strChfFile	= m_pAutoModelTypeList[i].strChfFile;
				pType->dwAttribute	= m_pAutoModelTypeList[i].dwAttribute;
				pType->dwPrice		= m_pAutoModelTypeList[i].dwPrice;
			}
			return i;
		}
	}

	return -1;
}

int CAutoSceneConfig::GetAutoSkyTypeListByID(DWORD dwID, AUTOSKYTYPELIST* pType)
{
	int i;
	for (i=0; i<m_nNumAutoSkyTypeList; i++)
	{
		if (m_pAutoSkyTypeList[i].dwID == dwID)
		{
			if (NULL != pType)
			{
				pType->dwID			= m_pAutoSkyTypeList[i].dwID;
				pType->strName		= m_pAutoSkyTypeList[i].strName;
				pType->strCap		= m_pAutoSkyTypeList[i].strCap;
				pType->strSurroundF	= m_pAutoSkyTypeList[i].strSurroundF;
				pType->strSurroundB	= m_pAutoSkyTypeList[i].strSurroundB;
			}
			return i;
		}
	}

	return -1;
}
