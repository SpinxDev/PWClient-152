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

#ifndef _AUTOSCENECONFIG_H_
#define _AUTOSCENECONFIG_H_

#include <A3DTypes.h>
#include <AString.h>

struct AUTOTREETYPELIST
{
	DWORD		dwID;
	AString		strName;
	AString		strSptFile;
	AString		strDdsFile;
	float		fInEarth;
	DWORD		dwPrice;
};

struct AUTOGRASSTYPELIST
{
	DWORD		dwID;
	AString		strName;
	AString		strMoxFile;
	DWORD		dwPrice;
};

struct AUTOTEXTURETYPELIST
{
	DWORD	dwID;
	AString strName;
	AString strTexture;
	DWORD	dwPrice;
};

struct AUTOMODELTYPELIST
{
	DWORD		dwID;
	AString		strName;
	AString		strMoxFile;
	AString		strChfFile;
	DWORD		dwAttribute;
	DWORD		dwPrice;
};

struct AUTOSKYTYPELIST
{
	DWORD		dwID;
	AString		strName;
	AString		strCap;
	AString		strSurroundF;
	AString		strSurroundB;
};

enum MODELATTRIBUTE
{
	MA_HOUSE			= 0,		// 房屋
	MA_OUTDOOR			= 1,		// 室外摆设
	MA_FURNITURE		= 2,		// 家具
	MA_PLANT			= 3,		// 植物
	MA_TOY				= 4,		// 玩具
	MA_VIRTU			= 5,		// 古董
	MA_OTHER			= 0xff,		// 其他或错误
};

enum TEXTURETYPE
{
	TT_MAIN				= 0,		// Main terrain
	TT_MAIN_PATCH,
	TT_BRIDGE,						// Hill top
	TT_BRIDGE_PATCH,
	TT_PART,						// Special texture
	TT_PART_PATCH,
	TT_ROADBED,						// Road
	TT_ROAD,
	TT_RIVERBED,					// River
	TT_RIVERSAND,
	TT_COUNT,
};

class CAutoSceneConfig  
{
public:
	CAutoSceneConfig();
	virtual ~CAutoSceneConfig();

	bool Init(const char* szFile);
	void Release();

	int GetAutoTreeTypeListByID(DWORD dwID, AUTOTREETYPELIST* pType = NULL);
	int GetAutoLandGrassTypeListByID(DWORD dwID, AUTOGRASSTYPELIST* pType = NULL);
	int GetAutoWaterGrassTypeListByID(DWORD dwID, AUTOGRASSTYPELIST* pType = NULL);
	int GetAutoTextureTypeListByID(DWORD dwID, AUTOTEXTURETYPELIST* pType = NULL);
	int GetAutoModelTypeListByID(DWORD dwID, AUTOMODELTYPELIST* pType = NULL);
	int GetAutoSkyTypeListByID(DWORD dwID, AUTOSKYTYPELIST* pType = NULL);

	inline const AUTOTREETYPELIST&	GetAutoTreeTypeListByIndex(int nIndex)			{ return m_pAutoTreeTypeList[nIndex]; }
	inline const AUTOGRASSTYPELIST&	GetAutoLandGrassTypeListByIndex(int nIndex)		{ return m_pAutoLandGrassTypeList[nIndex]; }
	inline const AUTOGRASSTYPELIST&	GetAutoWaterGrassTypeListByIndex(int nIndex)	{ return m_pAutoWaterGrassTypeList[nIndex]; }
	inline const AUTOTEXTURETYPELIST&	GetAutoTextureTypeListByIndex(int nIndex)	{ return m_pAutoTextureTypeList[nIndex]; }
	inline const AUTOMODELTYPELIST&	GetAutoModelTypeListByIndex(int nIndex)			{ return m_pAutoModelTypeList[nIndex]; }
	inline const AUTOSKYTYPELIST&	GetAutoSkyTypeListByIndex(int nIndex)			{ return m_pAutoSkyTypeList[nIndex]; }
	inline const AUTOTREETYPELIST*	GetAutoTreeTypeList()			{ return m_pAutoTreeTypeList; }
	inline const AUTOGRASSTYPELIST*	GetAutoLandGrassTypeList()		{ return m_pAutoLandGrassTypeList; }
	inline const AUTOGRASSTYPELIST*	GetAutoWaterGrassTypeList()		{ return m_pAutoWaterGrassTypeList; }
	inline const AUTOTEXTURETYPELIST*	GetAutoTextureTypeList()	{ return m_pAutoTextureTypeList; }
	inline const AUTOMODELTYPELIST*	GetAutoModelTypeList()			{ return m_pAutoModelTypeList; }
	inline const AUTOSKYTYPELIST*	GetAutoSkyTypeList()			{ return m_pAutoSkyTypeList; }

	inline int	GetNumAutoTreeTypeList()		{ return m_nNumAutoTreeTypeList; }
	inline int	GetNumAutoLandGrassTypeList()	{ return m_nNumAutoLandGrassTypeList; }
	inline int	GetNumAutoWaterGrassTypeList()	{ return m_nNumAutoWaterGrassTypeList; }
	inline int	GetNumAutoTextureTypeList()		{ return m_nNumAutoTextureTypeList; }
	inline int	GetNumAutoModelTypeList()		{ return m_nNumAutoModelTypeList; }
	inline int	GetNumAutoSkyTypeList()			{ return m_nNumAutoSkyTypeList; }
	inline DWORD	GetDefaultTextureID()		{ return m_dwDefaultTextureID; }
	inline DWORD	GetTextureID(int nIndex)	{ return m_aTextureID[nIndex]; }
	inline int		GetNumBlockrow()			{ return m_nNumBlockRow; }
	inline int		GetNumBlockCol()			{ return m_nNumBlockCol; }
	inline float	GetWorldWid()				{ return m_fWorldWid; }
	inline float	GetWorldLen()				{ return m_fWorldLen; }
	inline float	GetBlockSize()				{ return m_fBlockSize; }
	inline int		GetNumMaxLayer()			{ return TT_COUNT; }
	inline A3DVECTOR3&	GetSunDir()				{ return m_vSunDir; }
	inline DWORD	GetSunColorDay()			{ return m_dwSunColorDay; }
	inline void		SetSunColorDay(DWORD dwColor)	{ m_dwSunColorDay = dwColor; }
	inline DWORD	GetAmbientDay()				{ return m_dwAmbientDay; }
	inline void		SetAmbientDay(DWORD dwColor)	{ m_dwAmbientDay = dwColor; }
	inline DWORD	GetSunColorNight()			{ return m_dwSunColorNight; }
	inline void		SetSunColorNight(DWORD dwColor)	{ m_dwSunColorNight = dwColor; }
	inline DWORD	GetAmbientNight()			{ return m_dwAmbientNight; }
	inline void		SetAmbientNight(DWORD dwColor)	{ m_dwAmbientNight = dwColor; }
	inline DWORD	GetSkyIDDay()				{ return m_dwSkyIDDay; }
	inline void		SetSkyIDDay(DWORD dwID)		{ m_dwSkyIDDay = dwID; }
	inline DWORD	GetSkyIDNight()				{ return m_dwSkyIDNight; }
	inline void		SetSkyIDNight(DWORD dwID)	{ m_dwSkyIDNight = dwID; }
	inline bool		IsFogEnable()				{ return m_bFogEnable; }
	inline void		SetFogEnable(bool bEnable)	{ m_bFogEnable = bEnable; }
	inline float	GetFogStartDay()			{ return m_fFogStartDay; }
	inline void		SetFogStartDay(float fStart)	{ m_fFogStartDay = fStart; }
	inline float	GetFogEndDay()				{ return m_fFogEndDay; }
	inline void		SetFogEndDay(float fEnd)		{ m_fFogEndDay = fEnd; }
	inline DWORD	GetFogColorDay()			{ return m_dwFogColorDay; }
	inline void		SetFogColorDay(DWORD dwColor)	{ m_dwFogColorDay = dwColor; }
	inline float	GetFogStartNight()			{ return m_fFogStartNight; }
	inline void		SetFogStartNight(float fStart)	{ m_fFogStartNight = fStart; }
	inline float	GetFogEndNight()			{ return m_fFogEndNight; }
	inline void		SetFogEndNight(float fEnd)		{ m_fFogEndNight = fEnd; }
	inline DWORD	GetFogColorNight()			{ return m_dwFogColorNight; }
	inline void		SetFogColorNight(DWORD dwColor)	{ m_dwFogColorNight = dwColor; }
	inline DWORD	GetFogColorWater()			{ return m_dwFogColorWater; }
	inline void		SetFogColorWater(DWORD dwColor)	{ m_dwFogColorWater = dwColor; }
	
	inline const AString& GetScenePath()		{ return m_strScenePath; }
	inline const AString& GetBorderFile()		{ return m_strBorderFile; }

protected:
	// Resource
	AUTOTREETYPELIST*				m_pAutoTreeTypeList;
	AUTOGRASSTYPELIST*				m_pAutoLandGrassTypeList;
	AUTOGRASSTYPELIST*				m_pAutoWaterGrassTypeList;
	AUTOTEXTURETYPELIST*			m_pAutoTextureTypeList;
	AUTOMODELTYPELIST*				m_pAutoModelTypeList;
	AUTOSKYTYPELIST*				m_pAutoSkyTypeList;
	int								m_nNumAutoTreeTypeList;
	int								m_nNumAutoLandGrassTypeList;
	int								m_nNumAutoWaterGrassTypeList;
	int								m_nNumAutoTextureTypeList;
	int								m_nNumAutoModelTypeList;
	int								m_nNumAutoSkyTypeList;
	AString							m_strTreeListFileName;
	AString							m_strLandGrassListFileName;
	AString							m_strWaterGrassListFileName;
	AString							m_strTextureListFilename;
	AString							m_strModelListFileName;
	AString							m_strSkyListFileName;

	// Texture IDs
	DWORD							m_dwDefaultTextureID;
	DWORD							m_aTextureID[TT_COUNT];

	// World information
	int								m_nNumBlockRow;
	int								m_nNumBlockCol;
	float							m_fWorldWid;
	float							m_fWorldLen;
	float							m_fBlockSize;

	// Sun
	A3DVECTOR3						m_vSunDir;
	DWORD							m_dwSunColorDay;
	DWORD							m_dwAmbientDay;
	DWORD							m_dwSunColorNight;
	DWORD							m_dwAmbientNight;
	
	// Fog
	bool							m_bFogEnable;
	float							m_fFogStartDay;
	float							m_fFogEndDay;
	DWORD							m_dwFogColorDay;
	float							m_fFogStartNight;
	float							m_fFogEndNight;
	DWORD							m_dwFogColorNight;
	DWORD							m_dwFogColorWater;

	// Sky
	DWORD							m_dwSkyIDDay;
	DWORD							m_dwSkyIDNight;

	// Home files path
	AString							m_strScenePath;
	// Border file name
	AString							m_strBorderFile;

protected:
	// Load tree type list
	bool LoadTreeTypeList(const char* szFile);
	// Load grass type list
	bool LoadGrassTypeList(const char* szLandGrassFile, const char* szWaterGrassFile);
	// Load texture type list
	bool LoadTextureTypeList(const char* szFile);
	// Load all model type list
	bool LoadModelTypeList(const char* szFile);
	// Load sky type list
	bool LoadSkyTypeList(const char* szFile);
	// Load scene config
	bool LoadSceneConfig(const char* szFile);
};

#endif // #ifndef _AUTOSCENECONFIG_H_

