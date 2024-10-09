/*
 * FILE: AutoParameters.h
 *
 * DESCRIPTION: Parameters accession
 *
 * CREATED BY: Jiang Dalong, 2005/12/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOPARAMETERS_H_
#define _AUTOPARAMETERS_H_

#include <A3DTypes.h>
#include <AString.h>

const int HILL_HEIGHT_CURVE_NUMBER		= 7;
const int HILL_LINE_CURVE_NUMBER		= 10;

struct MAINTERRAINHEIGHT
{
	float	fMaxHeight;
	int		nMainSmooth;
	float	fPartSmooth;
	DWORD	dwSeed;
	float	fTextureRatio;
	float	fMainTexture;
	float	fPatchTexture;
	float	fPatchDivision;
};
struct TEXTUREINFO
{
	float	fMainTextureU;
	float	fMainTextureV;
	float	fPatchTextureU;
	float	fPatchTextureV;
	DWORD	dwMainTextureID;
	DWORD	dwPatchTextureID;
};
struct HILLINFO
{
	float	fMaxHeightHill;
	float	fMainSmoothHill;
	float	fPartSmoothHill;
	float	fBridgeScope;
	float	fBridgeDepth;
	float	fBorderErode;
	float	fLineHillWidth;
	bool	bHillUp;
	DWORD	dwHillSeed;
	int		nNumHeightCurve;
	float	pHeightCurve[HILL_HEIGHT_CURVE_NUMBER];
	int		nNumLineCurve;
	float	pLineCurve[HILL_LINE_CURVE_NUMBER];
	float	fTextureRatio;
	float	fMainTexture;
	float	fPatchTexture;
	float	fPatchDivision;
};
struct ROADINFO
{
	float	fMaxRoadHeight;
	float	fMainSmoothRoad;
	float	fPartSmoothRoad;
	float	fRoadWidth;
	bool	bFlatRoad;
	DWORD	dwSeedRoad;
};
struct PARTTEXTUREINFO
{
	float	fBorderScope;
	int		nTextureType;
	DWORD	dwSeed;
	float	fTextureRatio;
	float	fMainTexture;
	float	fPatchTexture;
	float	fPatchDivision;
};
struct PRESETINFO
{
	DWORD	dwID;
	AString	strName;
	AString	strPath;
	AString	strIcon;
};

class CAutoParameters  
{
public:
	CAutoParameters();
	virtual ~CAutoParameters();

public:
	// Load main terrain height parameters
	bool LoadMainTerrainHeight(const char* szFile, MAINTERRAINHEIGHT* pParam);
	// Save main terrain height parameters
	bool SaveMainTerrainHeight(const char* szFile, const MAINTERRAINHEIGHT& param);
	// Load texture parameters
	bool LoadTextureInfo(const char* szFile, TEXTUREINFO* pParam);
	// Save texture paramters
	bool SaveTextureInfo(const char* szFile, const TEXTUREINFO& param);
	// Load hill parameters
	bool LoadHillInfo(const char* szFile, HILLINFO* pParam);
	// Save hill paramters
	bool SaveHillInfo(const char* szFile, const HILLINFO& param);
	// Load road parameters
	bool LoadRoadInfo(const char* szFile, ROADINFO* pParam);
	// Save road paramters
	bool SaveRoadInfo(const char* szFile, const ROADINFO& param);
	// Load part texture parameters
	bool LoadPartTextureInfo(const char* szFile, PARTTEXTUREINFO* pParam);
	// Save part texture paramters
	bool SavePartTextureInfo(const char* szFile, const PARTTEXTUREINFO& param);

	// Release
	void Release();
	// Initialize
	bool Init();
	// Load a prepared set
	bool LoadSet(PRESETINFO** pSet, int* pNumber, const char* szFile);

	// Get main height set info by ID
	bool GetMainHeightSetInfoByID(PRESETINFO* info, DWORD dwID);
	// Get texture set info by ID
	bool GetTextureSetInfoByID(PRESETINFO* info, DWORD dwID);
	// Get hill poly info by ID
	bool GetHillPolySetInfoByID(PRESETINFO* info, DWORD dwID);
	// Get hill line info by ID
	bool GetHillLineSetInfoByID(PRESETINFO* info, DWORD dwID);
	// Get road set info by ID
	bool GetRoadSetInfoByID(PRESETINFO* info, DWORD dwID);
	// Get part texture set info by ID
	bool GetPartTextureSetInfoByID(PRESETINFO* info, DWORD dwID);

	inline PRESETINFO* GetMainHeightSet() { return m_pMainHeightSet; }
	inline PRESETINFO* GetTextureSet() { return m_pTextureSet; }
	inline PRESETINFO* GetHillPolySet() { return m_pHillPolySet; }
	inline PRESETINFO* GetHillLineSet() { return m_pHillLineSet; }
	inline PRESETINFO* GetRoadSet() { return m_pRoadSet; }
	inline PRESETINFO* GetPartTextureSet() { return m_pPartTextureSet; }
	inline int GetNumMainHeightSet() { return m_nNumMainHeightSet; }
	inline int GetNumTextureSet() { return m_nNumTextureSet; }
	inline int GetNumHillPolySet() { return m_nNumHillPolySet; }
	inline int GetNumHillLineSet() { return m_nNumHillLineSet; }
	inline int GetNumRoadSet() { return m_nNumRoadSet; }
	inline int GetNumPartTextureSet() { return m_nNumPartTextureSet; }

protected:
	PRESETINFO*			m_pMainHeightSet;	// Main height set
	PRESETINFO*			m_pTextureSet;		// Texture set
	PRESETINFO*			m_pHillPolySet;		// Hill poly set
	PRESETINFO*			m_pHillLineSet;		// Hill line set
	PRESETINFO*			m_pRoadSet;			// Road set
	PRESETINFO*			m_pPartTextureSet;	// Part texture set
	int					m_nNumMainHeightSet;
	int					m_nNumTextureSet;
	int					m_nNumHillPolySet;
	int					m_nNumHillLineSet;
	int					m_nNumRoadSet;
	int					m_nNumPartTextureSet;
};

#endif // #ifndef _AUTOPARAMETERS_H_

