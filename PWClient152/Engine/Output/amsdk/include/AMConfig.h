/*
 * FILE: AMConfig.h
 *
 * DESCRIPTION: configuration class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMCONFIG_H_
#define _AMCONFIG_H_

#include "AMTypes.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

enum AMRUNENV
{
	AMRUNENV_UNKNOWN		= -1,
	AMRUNENV_GAME			= 0,
	AMRUNENV_MODEDITOR		= 1,
	AMRUNENV_GFXEDITOR		= 2,
	AMRUNENV_SCENEEDITOR	= 3,
	AMRUNENV_PURESERVER		= 4,
	AMRUNENV_CONSOLETOOL	= 5
};
		  
enum AMSOUND_QUALITY
{
	AMSOUND_QUALITY_NULL	= -1,	// Not set yet
	AMSOUND_QUALITY_HIGH	= 0,	// origin sample rate
	AMSOUND_QUALITY_MEDIUM	= 1,	// half of origin sample rate, but keep above 11k
	AMSOUND_QUALITY_LOW		= 2,	// quarter of origian sample rate, but keep above 11k
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class AMConfig
//
///////////////////////////////////////////////////////////////////////////

class AMConfig
{
private:

	static AMRUNENV		m_nRunEnv;					//	0 -- Game; 1 -- ModEditor; 2 -- GfxEditor; 3 -- SceneEditor;

	bool				m_bFlagForceFeedback;		//	Flag indicates whether to use force feed back effects;
	AMSOUND_QUALITY		m_SoundQuality;				//	Quality type of sound;
	DWORD				m_dw3DSndUpdateInterval;	//	3D sound update interval (ms)

	AArray<int, int>	m_a3DSndGroups;				//	3D sound group
	int					m_iNum2DSoundBuf;			//	Number of 2D sound
	int					m_iMaxCachedFileSize;		//	Maximum size (byte) of every cached file
	int					m_iMaxFileCacheSize;		//	Maximum size (byte) of file cache

protected:

public:

	AMConfig();
	virtual ~AMConfig();

	bool LoadConfig(const char* szFile);

	static AMRUNENV GetRunEnv() { return m_nRunEnv; }
	static void SetRunEnv(AMRUNENV env) { m_nRunEnv = env; }

	inline bool GetFlagForceFeedback() { return m_bFlagForceFeedback; }
	void SetFlagForceFeedback(bool bFlag) { m_bFlagForceFeedback = bFlag; }

	inline AMSOUND_QUALITY GetSoundQuality() { return m_SoundQuality; }
	inline void SetSoundQuality(AMSOUND_QUALITY quality) { m_SoundQuality = quality; }

	inline DWORD Get3DSoundUpdateInterval() { return m_dw3DSndUpdateInterval; }
	inline void Set3DSoundUpdateInterval(DWORD dwInterval) { m_dw3DSndUpdateInterval = dwInterval; }

	inline int Get3DSndGroupNum() { return m_a3DSndGroups.GetSize(); }
	inline int Get3DSndGroupSize(int n) { return m_a3DSndGroups[n]; }
	inline int* GetAll3DSndGroupSize() { return m_a3DSndGroups.GetData(); }
	inline int GetMax2DSoundBuf() { return m_iNum2DSoundBuf; }
	inline int GetMaxCachedFileSize() { return m_iMaxCachedFileSize; }
	inline int GetMaxFileCacheSize() { return m_iMaxFileCacheSize; }
};


#endif	//	_AMCONFIG_H_
