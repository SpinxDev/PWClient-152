/*
 * FILE: A3DConfig.h
 *
 * DESCRIPTION: configuration class for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCONFIG_H_
#define _A3DCONFIG_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

enum A3DRUNENV
{
	A3DRUNENV_UNKNOWN		= -1,
	A3DRUNENV_GAME			= 0,
	A3DRUNENV_MODEDITOR		= 1,
	A3DRUNENV_GFXEDITOR		= 2,
	A3DRUNENV_SCENEEDITOR	= 3,
	A3DRUNENV_PURESERVER	= 4,
	A3DRUNENV_CONSOLETOOL	= 5
};

enum A3DTEXTURE_QUALITY
{
	A3DTEXTURE_QUALITY_NULL		= -1,	// Not set yet, use m_bFlagHQTexture flag
	A3DTEXTURE_QUALITY_HIGH		= 0,	// origin size 
	A3DTEXTURE_QUALITY_MEDIUM	= 1,	// half of origin size in width and height
	A3DTEXTURE_QUALITY_LOW		= 2,	// quarter of origin size in width and height
};

enum A3DOCCLUSION_TYPE
{
    A3DOCCTYPE_NONE     = 0,            // Do not use occlusion culling
    A3DOCCTYPE_D3DQUERY = 1,            // Hardware occlusion query system
    A3DOCCTYPE_SOFTWARE = 2,            // Software coverage buffer system
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
//	Class A3DConfig
//
///////////////////////////////////////////////////////////////////////////

class A3DConfig
{
public:		//	Types

	//	Runtime flags
	struct s_RUNTIMEFLAG
	{
		bool	bShowBoundBox;		    //	true, show bounding box
		bool	bShowFPS;			    //	true, show FPS
		bool	bShowPhyShapes;		    //	true, show physique shapes
		bool	bShowPerformance;	    //	true, show performance
		bool	bBuildAutoOBB;		    //	true, build frame's auto obb
        bool    bUseOcclusionCulling;   //  true, enable occlusion culling
        bool    bRecordPerformance;     //  true, record performance
	};

public:

	A3DConfig();
	~A3DConfig();

	//	Load config from file
	bool LoadConfig(const char* szFile);

	static A3DRUNENV GetRunEnv() { return m_nRunEnv; }
	static void SetRunEnv(A3DRUNENV env) { m_nRunEnv = env; }

	bool GetFlagDetailScene() { return m_bFlagDetailScene; }
	void SetFlagDetailScene(bool bFlag) { m_bFlagDetailScene = bFlag; }

	bool GetFlagDetailModel() { return m_bFlagDetailModel; }
	void SetFlagDetailModel(bool bFlag) { m_bFlagDetailModel = bFlag; }

	bool GetFlagDetailTerrain() { return m_bFlagDetailTerrain; }
	void SetFlagDetailTerrain(bool bFlag) { m_bFlagDetailTerrain = bFlag; }

	bool GetFlagLightMapScene() { return m_bFlagLightMapScene; }
	void SetFlagLightMapScene(bool bFlag) { m_bFlagLightMapScene = bFlag; }

	bool GetFlagDynamicLight() { return m_bFlagDynamicLight; }
	void SetFlagDynamicLight(bool bFlag) { m_bFlagDynamicLight = bFlag; }

	bool GetFlagHQSky() { return m_bFlagHQSky; }
	void SetFlagHQSky(bool bFlag) { m_bFlagHQSky = bFlag; }

	bool GetFlagHQTexture() { return m_bFlagHQTexture; }
	void SetFlagHQTexture(bool bFlag) { m_bFlagHQTexture = bFlag; }

	A3DTEXTURE_QUALITY GetTextureQuality() { return m_TextureQuality; }
	void SetTextureQuality(A3DTEXTURE_QUALITY quality) { m_TextureQuality = quality; }

	bool GetFlagTrilinear() { return m_bFlagTrilinear; }
	void SetFlagTrilinear(bool bFlag) { m_bFlagTrilinear = bFlag; }

	bool GetFlagNoTextures() { return m_bFlagNoTextures; }
	void SetFlagNoTextures(bool bFlag) { m_bFlagNoTextures = bFlag; }

	void SetNumVertBlendMatrix(int iNum) { m_iNumVertBlendMatrix = iNum; }
	int GetNumVertBlendMatrix() { return m_iNumVertBlendMatrix; }

	void SetTerrain2BlkGrid(int iNum) { m_iNumTerrain2BlkGrid = iNum; }
	int GetTerrain2BlkGrid() { return m_iNumTerrain2BlkGrid; }

	void SetFlagMotionBlur(bool bFlag) { m_bFlagMotionBlur = bFlag; }
	bool GetFlagMotionBlur() { return m_bFlagMotionBlur; }

	void SetFlagLinearSprite(bool bFlag) { m_bFlagLinearSprite = bFlag; }
	bool GetFlagLinearSprite() { return m_bFlagLinearSprite; }

	void SetFlagNewBoneScale(bool bFlag) { m_bFlagNewBoneScale = bFlag; }
	bool GetFlagNewBoneScale() { return m_bFlagNewBoneScale; }

	void SetFlagFAudioEnable(bool bFlag) { m_bFlagFAudioEnable = bFlag; }
	bool GetFlagFAudioEnable() const { return m_bFlagFAudioEnable; }

	bool GetFlagHLSLEffectEnable()
	{
		return m_bFlagHLSLEffect;
	}

	bool GetFlagDoNotReplaceBySdrx()
	{
		return m_bFlagNoReplaceBySdrx;
	}

	void SetFlagDoNotReplaceBySdrx(bool bVal)
	{
		m_bFlagNoReplaceBySdrx = bVal;
	}

	inline bool GetFlagForceShaderVer20();

	void SetPerfHUD(bool bFlagPerfHUD) { m_bFlagPerfHUD = bFlagPerfHUD; }
	bool GetPerfHUD() { return m_bFlagPerfHUD; }

    void SetOcclusionType(A3DOCCLUSION_TYPE occType) { m_occType = occType; }
    A3DOCCLUSION_TYPE GetOcclusionType() const { return m_occType; }

    //	Runtime flags ...
	void RT_SetShowBoundBoxFlag(bool bTrue) { m_RTFlags.bShowBoundBox = bTrue; }
	bool RT_GetShowBoundBoxFlag() { return m_RTFlags.bShowBoundBox; }

	void RT_SetShowFPSFlag(bool bTrue) { m_RTFlags.bShowFPS = bTrue; }
	bool RT_GetShowFPSFlag() { return m_RTFlags.bShowFPS; } 

	void RT_SetShowPhyShapesFlag(bool bTrue) { m_RTFlags.bShowPhyShapes = bTrue; }
	bool RT_GetShowPhyShapesFlag() { return m_RTFlags.bShowPhyShapes; }

	void RT_SetShowPerformanceFlag(bool bTrue) { m_RTFlags.bShowPerformance = bTrue; }
	bool RT_GetShowPerformanceFlag() { return m_RTFlags.bShowPerformance; }

	void RT_SetBuildAutoOBBFlag(bool bTrue) { m_RTFlags.bBuildAutoOBB = bTrue; }
	bool RT_GetBuildAutoOBBFlag() { return m_RTFlags.bBuildAutoOBB; }

    void RT_SetUseOcclusionCullingFlag(bool bTrue) { m_RTFlags.bUseOcclusionCulling = bTrue; }
    bool RT_GetUseOcclusionCullingFlag() { return m_RTFlags.bUseOcclusionCulling; }

    void RT_SetRecordPerformanceFlag(bool bTrue) { m_RTFlags.bRecordPerformance = bTrue; }
    bool RT_GetRecordPerformanceFlag() { return m_RTFlags.bRecordPerformance; }

protected:

	static A3DRUNENV	m_nRunEnv;		//	0 -- Game; 1 -- ModEditor; 2 -- GfxEditor; 3 -- SceneEditor;

	bool	m_bFlagDetailScene;			//	true, use detail texture on scene buildings;
	bool	m_bFlagDetailModel;			//	true, use detail texture on model;
	bool	m_bFlagDetailTerrain;		//	true, use detail texture on terrain;
	bool	m_bFlagLightMapScene;		//	true, use light maps to shade the scene;
	bool	m_bFlagDynamicLight;		//	true, use dynamic lights;
	bool	m_bFlagHQSky;				//	true, use high quality sky
	bool	m_bFlagHQTexture;			//	true, use large texture
	bool	m_bFlagTrilinear;			//	true, we will use trilinear to blend textures;
	bool	m_bFlagNoTextures;			//	true, load textures;
	bool	m_bFlagMotionBlur;			//	true, enable motion blur
	bool	m_bFlagLinearSprite;		//	true, all 2D sprite defaults to use linear filter
	bool	m_bFlagNewBoneScale;		//	true, use new bone scale interface
	bool	m_bFlagFAudioEnable;		//	true, audio engine based on fmod is enabled
	bool	m_bFlagHLSLEffect;			//	ture, render skin model with HLSL shader that has tangent
	bool	m_bFlagPerfHUD;				//	false
	bool	m_bFlagNoReplaceBySdrx;		//	false
	bool	m_bFlagForceShaderVer20;	//	false

	int		m_iNumTerrain2BlkGrid;		//	Grid number of A3DTerrain2 block
	int		m_iNumVertBlendMatrix;		//	Number of vertex blend matrix

	A3DTEXTURE_QUALITY m_TextureQuality;	//	Texture quality

	s_RUNTIMEFLAG	m_RTFlags;			//	Runtime flags

    A3DOCCLUSION_TYPE m_occType;         //  A3DOCCTYPE_NONE, Type of occlusion system

protected:

};

extern A3DConfig* g_pA3DConfig;

inline bool A3DConfig::GetFlagForceShaderVer20()
{
	return m_bFlagForceShaderVer20;
}

#endif	//	_A3DCONFIG_H_