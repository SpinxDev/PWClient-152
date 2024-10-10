/*
 * FILE: AMConfig.cpp
 *
 * DESCRIPTION: configuration class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AIniFile.h"
#include "AMConfig.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

//	Index of config keys
enum
{
	AMCK_RUNENV = 0,			//	m_nRunEnv	
	AMCK_FORCEFEEDBACK,			//	m_bFlagForceFeedback
	AMCK_SOUNDQUALITY,			//	m_SoundQuality
	AMCK_SNDUPDATEINTERVAL,		//	m_dw3DSndUpdateInterval
	AMCK_NUM3DSNDGROUP,			//	Number of 3D sound group
	AMCK_3DSNDGROUPS,			//	size of 3D sound groups
	AMCK_NUM2DSNDBUFFER,		//	Number of 2D sound buffer
	AMCK_MAXCACHEDFILESIZE,		//	Max size of each cached file
	AMCK_MAXFILECACHESIZE,		//	Max size of file cache
};

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	AM config keys in cfg file
static char* l_aAMCfgKeys[] = 
{
	"run_env",					//	AMCK_RUNENV,			m_nRunEnv	
	"force_feed_back",			//	AMCK_FORCEFEEDBACK,		m_bFlagForceFeedback
	"sound_quality",			//	AMCK_SOUNDQUALITY,		m_SoundQuality
	"sound_update_interval",	//	AMCK_SNDUPDATEINTERVAL,	m_dw3DSndUpdateInterval
	"3d_sound_group_num",		//	AMCK_NUM3DSNDGROUP,		number of 3D sound group
	"3d_sound_group_size",		//	AMCK_3DSNDGROUPS,		size of 3D sound groups
	"2d_sound_buffer_num",		//	AMCK_NUM2DSNDBUFFER,	m_iNum2DSoundBuf
	"max_cached_file_size",		//	AMCK_MAXCACHEDFILESIZE,	Max size of each cached file
	"max_file_cache_size",		//	AMCK_MAXFILECACHESIZE,	Max size of file cache
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkinMan
//
///////////////////////////////////////////////////////////////////////////

AMRUNENV AMConfig::m_nRunEnv = AMRUNENV_UNKNOWN;

AMConfig::AMConfig()
{
	if (m_nRunEnv == AMRUNENV_UNKNOWN)
		m_nRunEnv = AMRUNENV_GAME;
	
	m_bFlagForceFeedback	= true;
	m_SoundQuality			= AMSOUND_QUALITY_HIGH;
	m_dw3DSndUpdateInterval = 100;	//	ms
	m_iNum2DSoundBuf		= 64;
	m_iMaxCachedFileSize	= 250 * 1024;	//	byte
	m_iMaxFileCacheSize		= 10 * 1024 * 1024;	//	byte

	//	Try to load config from file: Configs\angelica.cfg
	AMConfig::LoadConfig("Configs\\angelica.cfg");
}

AMConfig::~AMConfig()
{
}

bool AMConfig::LoadConfig(const char* szFile)
{
	AIniFile IniFile;

	if (!IniFile.Open(szFile))
		return false;

	char szSect[] = "amedia";

	m_nRunEnv = (AMRUNENV)IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_RUNENV], (int)m_nRunEnv);

	m_bFlagForceFeedback	= IniFile.GetValueAsBoolean(szSect, l_aAMCfgKeys[AMCK_FORCEFEEDBACK], m_bFlagForceFeedback);
	m_SoundQuality			= (AMSOUND_QUALITY)IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_SOUNDQUALITY], (int)m_SoundQuality);
	m_dw3DSndUpdateInterval	= (DWORD)IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_SNDUPDATEINTERVAL], (int)m_dw3DSndUpdateInterval);
	m_iNum2DSoundBuf		= IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_NUM2DSNDBUFFER], m_iNum2DSoundBuf);
	m_iMaxCachedFileSize	= IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_MAXCACHEDFILESIZE], m_iMaxCachedFileSize);
	m_iMaxFileCacheSize		= IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_MAXFILECACHESIZE], m_iMaxFileCacheSize);

	int iNum3DSndGroup = IniFile.GetValueAsInt(szSect, l_aAMCfgKeys[AMCK_NUM3DSNDGROUP], 1);
	m_a3DSndGroups.SetSize(iNum3DSndGroup, 16);

	//	Set default value
	for (int i=0; i < iNum3DSndGroup; i++)
		m_a3DSndGroups[i] = 64;

	IniFile.GetValueAsIntArray(szSect, l_aAMCfgKeys[AMCK_3DSNDGROUPS], iNum3DSndGroup, m_a3DSndGroups.GetData());

	IniFile.Close();

	return true;
}

