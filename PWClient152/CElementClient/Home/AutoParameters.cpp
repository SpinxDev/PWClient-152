/*
 * FILE: AutoParameters.cpp
 *
 * DESCRIPTION: Parameters accession
 *
 * CREATED BY: Jiang Dalong, 2005/12/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoParameters.h"
#include "AutoSceneFunc.h"
#include <AFileImage.h>
#include <AIniFile.h>
#include <AFI.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoParameters::CAutoParameters()
{
	m_pMainHeightSet	= NULL;
	m_pTextureSet		= NULL;
	m_pHillPolySet		= NULL;
	m_pHillLineSet		= NULL;
	m_pRoadSet			= NULL;
	m_pPartTextureSet	= NULL;
	m_nNumMainHeightSet	= 0;
	m_nNumTextureSet	= 0;
	m_nNumHillPolySet	= 0;
	m_nNumHillLineSet	= 0;
	m_nNumRoadSet		= 0;
	m_nNumPartTextureSet= 0;
}

CAutoParameters::~CAutoParameters()
{
	Release();
}

// Release
void CAutoParameters::Release()
{
	SAFERELEASE(m_pMainHeightSet);
	SAFERELEASE(m_pTextureSet);
	SAFERELEASE(m_pHillPolySet);
	SAFERELEASE(m_pHillLineSet);
	SAFERELEASE(m_pRoadSet);
	SAFERELEASE(m_pPartTextureSet);
	
}

// Initialize
bool CAutoParameters::Init()
{
	AString strSetPath;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\整体高度.lst";
	if (!LoadSet(&m_pMainHeightSet, &m_nNumMainHeightSet, strSetPath))
		return false;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\纹理.lst";
	if (!LoadSet(&m_pTextureSet, &m_nNumTextureSet, strSetPath))
		return false;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\多边形山.lst";
	if (!LoadSet(&m_pHillPolySet, &m_nNumHillPolySet, strSetPath))
		return false;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\线形山.lst";
	if (!LoadSet(&m_pHillLineSet, &m_nNumHillLineSet, strSetPath))
		return false;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\路.lst";
	if (!LoadSet(&m_pRoadSet, &m_nNumRoadSet, strSetPath))
		return false;

	strSetPath = "Configs\\AutoFamilyConfigs\\Parameters\\预设\\局部纹理.lst";
	if (!LoadSet(&m_pPartTextureSet, &m_nNumPartTextureSet, strSetPath))
		return false;

	return true;
}

// Load main terrain height parameters
bool CAutoParameters::LoadMainTerrainHeight(const char* szFile, MAINTERRAINHEIGHT* pParam)
{
	AIniFile file;
	if( !file.Open(szFile) )
		return false;

	const char* szSection = "main_Terrain_Height";

	pParam->fMaxHeight = file.GetValueAsFloat(szSection, "max_height", 100.0f);
	pParam->nMainSmooth = file.GetValueAsInt(szSection, "main_smooth", 4);
	pParam->fPartSmooth = file.GetValueAsFloat(szSection, "part_smooth", 0.3f);
	pParam->dwSeed = file.GetValueAsInt(szSection, "seed", 4);
	pParam->fTextureRatio = file.GetValueAsFloat(szSection, "texture_ratio", 0.2f);
	pParam->fMainTexture = file.GetValueAsFloat(szSection, "main_texture", 0.4f);
	pParam->fPatchTexture = file.GetValueAsFloat(szSection, "patch_texture", 1.2f);
	pParam->fPatchDivision = file.GetValueAsFloat(szSection, "patch_division", 30.0f);

	file.Close();
	return true;
}

// Save main terrain height parameters
bool CAutoParameters::SaveMainTerrainHeight(const char* szFile, const MAINTERRAINHEIGHT& param)
{
	AIniFile f;
	const char* szSection = "main_Terrain_Height";

	if (!f.WriteFloatValue(szSection, "max_height", param.fMaxHeight))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "main_smooth", param.nMainSmooth))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "part_smooth", param.fPartSmooth))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "seed", param.dwSeed))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "texture_ratio", param.fTextureRatio))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_texture", param.fMainTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_texture", param.fPatchTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_division", param.fPatchDivision))
		goto WRITEFAIL;

	if (!f.Save(szFile))
		goto WRITEFAIL;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return false;
}

// Load texture parameters
bool CAutoParameters::LoadTextureInfo(const char* szFile, TEXTUREINFO* pParam)
{
	AIniFile file;
	if( !file.Open(szFile) )
		return false;

	const char* szSection = "texture";

	pParam->fMainTextureU = file.GetValueAsFloat(szSection, "main_texture_u", 10.0f);
	pParam->fMainTextureV = file.GetValueAsFloat(szSection, "main_texture_v", 10.0f);
	pParam->fPatchTextureU = file.GetValueAsFloat(szSection, "patch_texture_u", 10.0f);
	pParam->fPatchTextureV = file.GetValueAsFloat(szSection, "patch_texture_v", 10.0f);
	pParam->dwMainTextureID = file.GetValueAsInt(szSection, "main_texture_id", 0);
	pParam->dwPatchTextureID = file.GetValueAsInt(szSection, "patch_texture_id", 1);

	file.Close();
	return true;
}

// Save texture paramters
bool CAutoParameters::SaveTextureInfo(const char* szFile, const TEXTUREINFO& param)
{
	AIniFile f;
	const char* szSection = "texture";

	if (!f.WriteFloatValue(szSection, "main_texture_u", param.fMainTextureU))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_texture_v", param.fMainTextureV))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_texture_u", param.fPatchTextureU))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_texture_v", param.fPatchTextureV))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "main_texture_id", param.dwMainTextureID))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "patch_texture_id", param.dwPatchTextureID))
		goto WRITEFAIL;

	if (!f.Save(szFile))
		goto WRITEFAIL;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return false;
}

// Load hill parameters
bool CAutoParameters::LoadHillInfo(const char* szFile, HILLINFO* pParam)
{
	AIniFile file;
	if( !file.Open(szFile) )
		return false;

	const char* szSection = "hill";

	pParam->fMaxHeightHill = file.GetValueAsFloat(szSection, "max_height", 150.0f);
	pParam->fMainSmoothHill = file.GetValueAsFloat(szSection, "main_smooth", 3.0f);
	pParam->fPartSmoothHill = file.GetValueAsFloat(szSection, "part_smooth", 0.4f);
	pParam->fBridgeScope = file.GetValueAsFloat(szSection, "bridge_scope", 0.6f);
	pParam->fBridgeDepth = file.GetValueAsFloat(szSection, "bridge_depth", 0.2f);
	pParam->fBorderErode = file.GetValueAsFloat(szSection, "border_erode", 0.4f);
	pParam->fLineHillWidth = file.GetValueAsFloat(szSection, "line_width", 100.0f);
	pParam->bHillUp = file.GetValueAsBoolean(szSection, "hill_up", true);
	pParam->dwHillSeed = file.GetValueAsInt(szSection, "hill_seed", 1);
	pParam->fTextureRatio = file.GetValueAsFloat(szSection, "texture_ratio", 0.2f);
	pParam->fMainTexture = file.GetValueAsFloat(szSection, "main_texture", 0.4f);
	pParam->fPatchTexture = file.GetValueAsFloat(szSection, "patch_texture", 1.2f);
	pParam->fPatchDivision = file.GetValueAsFloat(szSection, "patch_division", 30.0f);
	
	int i;
	AString strKey;
	// Load height curve
	pParam->nNumHeightCurve = file.GetValueAsInt(szSection, "height_curve_number", 0);
	if (pParam->nNumHeightCurve > 0)
	{
		if (HILL_HEIGHT_CURVE_NUMBER != pParam->nNumHeightCurve)
			return false;
		for (i=0; i<pParam->nNumHeightCurve; i++)
		{
			strKey.Format("height_curve_%d", i);
			pParam->pHeightCurve[i] = file.GetValueAsFloat(szSection, strKey, 0.0f);
		}
	}

	// Load line curve
	pParam->nNumLineCurve = file.GetValueAsInt(szSection, "line_curve_number", 0);
	if (pParam->nNumLineCurve > 0)
	{
		if (HILL_LINE_CURVE_NUMBER != pParam->nNumLineCurve)
			return false;
		for (i=0; i<pParam->nNumLineCurve; i++)
		{
			strKey.Format("line_curve_%d", i);
			pParam->pLineCurve[i] = file.GetValueAsFloat(szSection, strKey, 0.0f);
		}
	}

	file.Close();
	return true;
}

// Save hill paramters
bool CAutoParameters::SaveHillInfo(const char* szFile, const HILLINFO& param)
{
	AIniFile f;
	const char* szSection = "hill";
	int i;
	AString strKey;

	if (!f.WriteFloatValue(szSection, "max_height", param.fMaxHeightHill))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_smooth", param.fMainSmoothHill))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "part_smooth", param.fPartSmoothHill))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "bridge_scope", param.fBridgeScope))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "bridge_depth", param.fBridgeDepth))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "border_erode", param.fBorderErode))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "line_width", param.fLineHillWidth))
		goto WRITEFAIL;
	int nHillUp;
	param.bHillUp ? nHillUp = 1 : nHillUp = 0;
	if (!f.WriteIntValue(szSection, "hill_up", nHillUp))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "hill_seed", param.dwHillSeed))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "texture_ratio", param.fTextureRatio))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_texture", param.fMainTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_texture", param.fPatchTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_division", param.fPatchDivision))
		goto WRITEFAIL;

	// Save height curve
	if (!f.WriteIntValue(szSection, "height_curve_number", param.nNumHeightCurve))
		goto WRITEFAIL;
	if (param.nNumHeightCurve > 0)
	{
		for (i=0; i<param.nNumHeightCurve; i++)
		{
			strKey.Format("height_curve_%d", i);
			if (!f.WriteFloatValue(szSection, strKey, param.pHeightCurve[i]))
				goto WRITEFAIL;
		}
	}

	// Save line curve
	if (!f.WriteIntValue(szSection, "line_curve_number", param.nNumLineCurve))
		goto WRITEFAIL;
	if (param.nNumLineCurve > 0)
	{
		for (i=0; i<param.nNumLineCurve; i++)
		{
			strKey.Format("line_curve_%d", i);
			if (!f.WriteFloatValue(szSection, strKey, param.pLineCurve[i]))
				goto WRITEFAIL;
		}
	}

	if (!f.Save(szFile))
		goto WRITEFAIL;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return false;
}

// Load road parameters
bool CAutoParameters::LoadRoadInfo(const char* szFile, ROADINFO* pParam)
{
	AIniFile file;
	if( !file.Open(szFile) )
		return false;

	const char* szSection = "road";

	pParam->fMaxRoadHeight = file.GetValueAsFloat(szSection, "max_height", 10.0f);
	pParam->fMainSmoothRoad = file.GetValueAsFloat(szSection, "main_smooth", 50.0f);
	pParam->fPartSmoothRoad = file.GetValueAsFloat(szSection, "part_smooth", 0.2f);
	pParam->fRoadWidth = file.GetValueAsFloat(szSection, "road_width", 20.0f);
	pParam->bFlatRoad = file.GetValueAsBoolean(szSection, "flat_road", false);
	pParam->dwSeedRoad = file.GetValueAsInt(szSection, "road_seed", 1);


	file.Close();
	return true;
}

// Save road paramters
bool CAutoParameters::SaveRoadInfo(const char* szFile, const ROADINFO& param)
{
	AIniFile f;
	const char* szSection = "road";

	if (!f.WriteFloatValue(szSection, "max_height", param.fMaxRoadHeight))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_smooth", param.fMainSmoothRoad))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "part_smooth", param.fPartSmoothRoad))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "road_width", param.fRoadWidth))
		goto WRITEFAIL;
	int nRoadUp;
	param.bFlatRoad ? nRoadUp = 1 : nRoadUp = 0;
	if (!f.WriteIntValue(szSection, "flat_road", nRoadUp))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "road_seed", param.dwSeedRoad))
		goto WRITEFAIL;

	if (!f.Save(szFile))
		goto WRITEFAIL;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return false;
}


// Load road parameters
bool CAutoParameters::LoadPartTextureInfo(const char* szFile, PARTTEXTUREINFO* pParam)
{
	AIniFile file;
	if( !file.Open(szFile) )
		return false;

	const char* szSection = "part_texture";

	pParam->fBorderScope = file.GetValueAsFloat(szSection, "border_scope", 0.5f);
	pParam->nTextureType = file.GetValueAsInt(szSection, "texture_type", 2);
	pParam->dwSeed = file.GetValueAsInt(szSection, "part_texture_seed", 0);
	pParam->fTextureRatio = file.GetValueAsFloat(szSection, "texture_ratio", 0.2f);
	pParam->fMainTexture = file.GetValueAsFloat(szSection, "main_texture", 0.4f);
	pParam->fPatchTexture = file.GetValueAsFloat(szSection, "patch_texture", 1.2f);
	pParam->fPatchDivision = file.GetValueAsFloat(szSection, "patch_division", 30.0f);

	file.Close();
	return true;
}

// Save road paramters
bool CAutoParameters::SavePartTextureInfo(const char* szFile, const PARTTEXTUREINFO& param)
{
	AIniFile f;
	const char* szSection = "part_texture";

	if (!f.WriteFloatValue(szSection, "border_scope", param.fBorderScope))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "texture_type", param.nTextureType))
		goto WRITEFAIL;
	if (!f.WriteIntValue(szSection, "part_texture_seed", param.dwSeed))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "texture_ratio", param.fTextureRatio))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "main_texture", param.fMainTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_texture", param.fPatchTexture))
		goto WRITEFAIL;
	if (!f.WriteFloatValue(szSection, "patch_division", param.fPatchDivision))
		goto WRITEFAIL;

	if (!f.Save(szFile))
		goto WRITEFAIL;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return false;
}

// Load a prepared set
bool CAutoParameters::LoadSet(PRESETINFO** pSet, int* pNumber, const char* szFile)
{
	AFileImage fileToLoad;
	if (!fileToLoad.Open(szFile, AFILE_OPENEXIST))
		return false;
	
	char	szLine[256];
	DWORD	dwReadLen;
	int i;
	
	// Read number of set
	if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
		goto READFAIL;
	sscanf(szLine, "%d", pNumber);
	if (*pNumber <= 0)
		goto READFAIL;

	// malloc set memory
	SAFERELEASE(*pSet);
	*pSet = new PRESETINFO[*pNumber];

	// Read list
	for (i=0; i<*pNumber; i++)
	{
		// Read ID
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		sscanf(szLine, "%d", &(*pSet)[i].dwID);
		// Read name
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		(*pSet)[i].strName = szLine;
		// Read path
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		(*pSet)[i].strPath = szLine;
		// Read icon
		if (!fileToLoad.ReadLine(szLine, 256, &dwReadLen))
			goto READFAIL;
		(*pSet)[i].strIcon = szLine;
	}

	fileToLoad.Close();
	return true;

READFAIL:
	fileToLoad.Close();
	return false;

}

// Get main height set info by ID
bool CAutoParameters::GetMainHeightSetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumMainHeightSet; i++)
	{
		if (m_pMainHeightSet[i].dwID == dwID)
		{
			info->dwID = m_pMainHeightSet[i].dwID;
			info->strName = m_pMainHeightSet[i].strName;
			info->strPath = m_pMainHeightSet[i].strPath;
			info->strIcon = m_pMainHeightSet[i].strIcon;
			return true;
		}
	}
	
	return false;
}

// Get texture set info by ID
bool CAutoParameters::GetTextureSetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumTextureSet; i++)
	{
		if (m_pTextureSet[i].dwID == dwID)
		{
			info->dwID = m_pTextureSet[i].dwID;
			info->strName = m_pTextureSet[i].strName;
			info->strPath = m_pTextureSet[i].strPath;
			info->strIcon = m_pTextureSet[i].strIcon;
			return true;
		}
	}
	
	return false;
}

// Get hill poly info by ID
bool CAutoParameters::GetHillPolySetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumHillPolySet; i++)
	{
		if (m_pHillPolySet[i].dwID == dwID)
		{
			info->dwID = m_pHillPolySet[i].dwID;
			info->strName = m_pHillPolySet[i].strName;
			info->strPath = m_pHillPolySet[i].strPath;
			info->strIcon = m_pHillPolySet[i].strIcon;
			return true;
		}
	}
	
	return false;
}

// Get hill line info by ID
bool CAutoParameters::GetHillLineSetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumHillLineSet; i++)
	{
		if (m_pHillLineSet[i].dwID == dwID)
		{
			info->dwID = m_pHillLineSet[i].dwID;
			info->strName = m_pHillLineSet[i].strName;
			info->strPath = m_pHillLineSet[i].strPath;
			info->strIcon = m_pHillLineSet[i].strIcon;
			return true;
		}
	}
	
	return false;
}

// Get road set info by ID
bool CAutoParameters::GetRoadSetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumRoadSet; i++)
	{
		if (m_pRoadSet[i].dwID == dwID)
		{
			info->dwID = m_pRoadSet[i].dwID;
			info->strName = m_pRoadSet[i].strName;
			info->strPath = m_pRoadSet[i].strPath;
			info->strIcon = m_pRoadSet[i].strIcon;
			return true;
		}
	}
	
	return false;
}

// Get part texture set info by ID
bool CAutoParameters::GetPartTextureSetInfoByID(PRESETINFO* info, DWORD dwID)
{
	for (int i=0; i<m_nNumPartTextureSet; i++)
	{
		if (m_pPartTextureSet[i].dwID == dwID)
		{
			info->dwID = m_pPartTextureSet[i].dwID;
			info->strName = m_pPartTextureSet[i].strName;
			info->strPath = m_pPartTextureSet[i].strPath;
			info->strIcon = m_pPartTextureSet[i].strIcon;
			return true;
		}
	}
	
	return false;
}
