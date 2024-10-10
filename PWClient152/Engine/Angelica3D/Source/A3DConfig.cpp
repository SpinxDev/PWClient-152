/*
 * FILE: A3DConfig.cpp
 *
 * DESCRIPTION: configuration class for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DConfig.h"
#include "A3DPI.h"
#include "AMemory.h"
#include "AIniFile.h"

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
	A3DCK_RUNENV = 0,			//	m_nRunEnv
	A3DCK_DETAILSCENE,			//	m_bFlagDetailScene;			
	A3DCK_DETAILMODEL,			//	m_bFlagDetailModel;			
	A3DCK_DETAILTERRAIN,		//	m_bFlagDetailTerrain;		
	A3DCK_LIGHTMAPSCENE,		//	m_bFlagLightMapScene;		
	A3DCK_DYNAMICLIGHT,			//	m_bFlagDynamicLight;		
	A3DCK_HQSKY,				//	m_bFlagHQSky;				
	A3DCK_HQTEXTURE,			//	m_bFlagHQTexture;			
	A3DCK_TRILINEAR,			//	m_bFlagTrilinear;			
	A3DCK_NOTEXTURES,			//	m_bFlagNoTextures;			
	A3DCK_NUMVERTBLENDMAT,		//	m_iNumVertBlendMatrix;		
	A3DCK_TEXTUREQUALITY,		//	m_TextureQuality
	A3DCK_MOTIONBLUR,			//	m_bFlagMotionBlur
	A3DCK_TERRAIN2BLKGRID,		//	m_iNumTerrain2BlkGrid
	A3DCK_NEWBONESCALE,			//	m_bFlagNewBoneScale
	A3DCK_FAUDIOENABLE,			//	m_bFlagFAudioEnable
	A3DCK_HLSLEFFECT,			//	m_bFlagHLSLEffect
	A3DCK_PERFHUD,				//	m_bFlagPerfHUD
	A3DCK_DONOTREPLACEBYSDRX,	//  m_bFlagDoNotReplaceBySdrx
    A3DCK_OCCTYPE,              //  m_occType
	A3DCK_FORCESHADERVER2,		//	m_bFlagForceShaderVer20
    A3DCK_SKINTANGET,           //  m_bSkinCheckTangent
};

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

A3DConfig* g_pA3DConfig;

//	A3D config keys in cfg file
static char* l_aA3DCfgKeys[] = 
{
	"run_env",				//	A3DCK_RUNENV,				m_nRunEnv
	"detail_scene",			//	A3DCK_DETAILSCENE,			m_bFlagDetailScene;			
	"detail_model",			//	A3DCK_DETAILMODEL,			m_bFlagDetailModel;			
	"detail_terrain",		//	A3DCK_DETAILTERRAIN,		m_bFlagDetailTerrain;		
	"lightmap_scene",		//	A3DCK_LIGHTMAPSCENE,		m_bFlagLightMapScene;		
	"dynamic_light",		//	A3DCK_DYNAMICLIGHT,			m_bFlagDynamicLight;		
	"high_quality_sky",		//	A3DCK_HQSKY,				m_bFlagHQSky;				
	"high_quality_tex",		//	A3DCK_HQTEXTURE,			m_bFlagHQTexture;			
	"trilinear",			//	A3DCK_TRILINEAR,			m_bFlagTrilinear;			
	"no_textures",			//	A3DCK_NOTEXTURES,			m_bFlagNoTextures;			
	"num_vert_blend_mat",	//	A3DCK_NUMVERTBLENDMAT,		m_iNumVertBlendMatrix;		
	"texture_quality",		//	A3DCK_TEXTUREQUALITY,		m_TextureQuality
	"motion_blur",			//	A3DCK_MOTIONBLUR,			m_bFlagMotionBlur
	"num_t2_block_grid",	//	A3DCK_TERRAIN2BLKGRID,		m_iNumTerrain2BlkGrid
	"new_bone_scale",		//	A3DCK_NEWBONESCALE,			m_bFlagNewBoneScale
	"faudio_enable",		//	A3DCK_FAUDIOENABLE,			m_bFlagFAudioEnable
	"hlsl_effect",			//	A3DCK_HLSLEFFECT,			m_bFlagHLSLEffect
	"PerfHUD",				//	A3DCK_PERFHUD,				m_bFlagPerfHUD
	"DoNotReplaceBySdrx",	//  A3DCK_DONOTREPLACEBYSDRX,	m_bFlagDoNotReplaceBySdrx
    "occ_type",				//	A3DCK_OCCTYPE,				m_occType
	"ForceShaderVer20",		//	A3DCK_FORCESHADERVER2,		m_bFlagForceShaderVer20
    "skin_check_tangent",	//	A3DCK_SKINTANGENT,			m_bSkinCheckTangent
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

A3DRUNENV A3DConfig::m_nRunEnv = A3DRUNENV_UNKNOWN;

A3DConfig::A3DConfig()
{
	//	Default Values;
	if (m_nRunEnv == A3DRUNENV_UNKNOWN)
	{
		//	Not set running environment yet, we use default value;
		m_nRunEnv = A3DRUNENV_GAME;
	}

	m_bFlagDetailScene		= true;
	m_bFlagDetailModel		= true;
	m_bFlagDetailTerrain	= true;
	m_bFlagLightMapScene	= true;
	m_bFlagDynamicLight		= true;
	m_bFlagHQSky			= true;
	m_bFlagHQTexture		= true;
	m_TextureQuality		= A3DTEXTURE_QUALITY_NULL;
	m_bFlagTrilinear		= true;
	m_bFlagNoTextures		= false;
	m_iNumVertBlendMatrix	= 4;
	m_bFlagMotionBlur		= false;
	m_iNumTerrain2BlkGrid	= 64;
	m_bFlagLinearSprite		= false;
	m_bFlagNewBoneScale		= true;
	m_bFlagFAudioEnable		= false;
	m_bFlagHLSLEffect		= true;
	m_bFlagPerfHUD			= false;
	m_bFlagNoReplaceBySdrx	= false;
    m_occType               = A3DOCCTYPE_NONE;
	m_bFlagForceShaderVer20	= false;
  
	//	Try to load config from file: Configs\angelica.cfg
	A3DConfig::LoadConfig("Configs\\angelica.cfg");

	//	Set default runtime flags
	m_RTFlags.bShowBoundBox		= false;
	m_RTFlags.bShowPerformance	= false;
	m_RTFlags.bShowPhyShapes	= false;
	m_RTFlags.bShowFPS			= true;
	m_RTFlags.bBuildAutoOBB		= false;
    m_RTFlags.bRecordPerformance = false;
}

A3DConfig::~A3DConfig()
{
}

//	Load config from file
bool A3DConfig::LoadConfig(const char* szFile)
{
	AIniFile IniFile;

	if (!IniFile.Open(szFile))
		return false;

	char szSect[] = "a3d";

	m_nRunEnv = (A3DRUNENV)IniFile.GetValueAsInt(szSect, l_aA3DCfgKeys[A3DCK_RUNENV], (int)m_nRunEnv);

	m_bFlagDetailScene		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_DETAILSCENE], m_bFlagDetailScene);
	m_bFlagDetailModel		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_DETAILMODEL], m_bFlagDetailModel);
	m_bFlagDetailTerrain	= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_DETAILTERRAIN], m_bFlagDetailTerrain);		
	m_bFlagLightMapScene	= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_LIGHTMAPSCENE], m_bFlagLightMapScene);
	m_bFlagDynamicLight		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_DYNAMICLIGHT], m_bFlagDynamicLight);
	m_bFlagHQSky			= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_HQSKY], m_bFlagHQSky);
	m_bFlagHQTexture		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_HQTEXTURE], m_bFlagHQTexture);
	m_bFlagTrilinear		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_TRILINEAR], m_bFlagTrilinear);
	m_bFlagNoTextures		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_NOTEXTURES], m_bFlagNoTextures);
	m_bFlagMotionBlur		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_MOTIONBLUR], m_bFlagMotionBlur);
	m_bFlagNewBoneScale		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_NEWBONESCALE], m_bFlagNewBoneScale);
	m_bFlagFAudioEnable		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_FAUDIOENABLE], m_bFlagFAudioEnable);
	m_bFlagHLSLEffect		= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_HLSLEFFECT], m_bFlagHLSLEffect);
	m_bFlagPerfHUD			= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_PERFHUD], m_bFlagPerfHUD);
	m_bFlagNoReplaceBySdrx	= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_DONOTREPLACEBYSDRX], m_bFlagNoReplaceBySdrx);
	m_bFlagForceShaderVer20	= IniFile.GetValueAsBoolean(szSect, l_aA3DCfgKeys[A3DCK_FORCESHADERVER2], m_bFlagForceShaderVer20);
    m_occType   			= (A3DOCCLUSION_TYPE)IniFile.GetValueAsInt(szSect, l_aA3DCfgKeys[A3DCK_OCCTYPE], (int)m_occType);

	m_iNumVertBlendMatrix	= IniFile.GetValueAsInt(szSect, l_aA3DCfgKeys[A3DCK_NUMVERTBLENDMAT], m_iNumVertBlendMatrix);
	m_iNumTerrain2BlkGrid	= IniFile.GetValueAsInt(szSect, l_aA3DCfgKeys[A3DCK_TERRAIN2BLKGRID], m_iNumTerrain2BlkGrid);
	m_TextureQuality		= (A3DTEXTURE_QUALITY)IniFile.GetValueAsInt(szSect, l_aA3DCfgKeys[A3DCK_TEXTUREQUALITY], (int)m_TextureQuality);
	IniFile.Close();

	return true;
}



