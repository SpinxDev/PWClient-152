/*
 * FILE: A3DShader.h
 *
 * DESCRIPTION: Routines for shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSHADER_H_
#define _A3DSHADER_H_

#include "A3DTexture.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	General Properties Mask Flags
#define SGMF_ZFUNC					0x00000001		//	dwZFunc
#define SGMF_ZENABLE				0x00000002		//	bZEnable
#define SGMF_ZWENABLE				0x00000004		//	bWriteEnable
#define SGMF_ALPHACOMP				0x00000008		//	dwAlphaFunc and fAlphaRef
#define SGMF_CULLMODE				0x00000010		//	dwCullMode
#define SGMF_BLEND					0x00000020		//	dwSrcBlend and dwDstBlend
#define SGMF_COLORGEN				0x00000040		//	rgbGen
#define SGMF_ALPHAGEN				0x00000080		//	AlphaGen
#define SGMF_TFACTOR				0x00000100		//	Texture factor; ps's c0 if ps is enabled
#define SGMF_TTEMP					0x00000200		//	Texture temp color, ps's c1 if ps is enabled
#define SGMF_PSC2					0x00000400		//	ps's c2 register
#define SGMF_PSC3					0x00000800		//	ps's c3 register
#define SGMF_PSC4					0x00001000		//	ps's c2 register
#define SGMF_PSC5					0x00002000		//	ps's c3 register
#define SGMF_PSC6					0x00004000		//	ps's c2 register
#define SGMF_PSC7					0x00008000		//	ps's c3 register
#define SGMF_TRANSPARENTTEXCHANNEL	0x00010000		//	iTransTextureIndex and iTransChannel. Added by PanYupin, for alphatest in z-prepass

//	Shader Stage Mask Flags
#define SSMF_FILTER			0x00000001		//	dwFilter
#define SSMF_ADDRESS		0x00000002		//	dwAddress
#define SSMF_COLOROP		0x00000004		//	dwColorOp, dwColorArg1 and dwColorArg2
#define SSMF_ALPHAOP		0x00000008		//	dwAlphaOp, dwAlphaArg1 and dwAlphaArg2
#define SSMF_COLORGEN		0x00000010		//	r, g, b
#define SSMF_ALPHAGEN		0x00000020		//	a
#define SSMF_ALPHACOMP		0x00000040		//	dwAlphaFunc and fAlphaRef
#define SSMF_BUMPMAT		0x00000080		//	fBumpEnvMat00, 01, 10, 11
#define SSMF_BUMPSCALE		0x00000100		//	fBumpEnvScale
#define SSMF_BUMPOFFSET		0x00000200		//	fBumpEnvOffset
#define SSMF_RESULTARG		0x00000400		//	dwResultArg
#define SSMF_TEXTRANS		0x00000800		//	dwTexCoord and matTexTrans
#define SSMF_TEXCOORD		0x00001000		//	dwTexCoord

//	Objects wave applied to (Shader Wave Object)
enum
{
	SWO_NONE = 0,		//	None
	SWO_COLORGEN,		//	Color generate
	SWO_ALPHAGEN,		//	Alpha generate
	SWO_TCSCALE,		//	Texture Coordinates scale
	SWO_TCSCROLL,		//	Texture Coordinates scroll
	SWO_TCROTATE,		//	Texture Coordinates rotate
	SWO_TCSTRETCH,		//	Texture Coordinates stretch
	SWO_TCTURB,			//	Texture Coordinates turbulence
};

//	Shader Wave Function Type
enum
{
	SWFT_NONE = 0,		//	None
	SWFT_SIN,			//	Sin wave
	SWFT_TRIANGLE,		//	Triangle wave
	SWFT_SQUARE,		//	Square wave
	SWFT_SAWTOOTH,		//	Sawtooth
	SWFT_INVSAWTOOTH,	//	Inverse sawtooth
};

//	Texture type
enum A3DSHADERTEXTYPE
{
	A3DSDTEX_NONE = 0,	//	None
	A3DSDTEX_NORMAL,	//	Normal map texture
	A3DSDTEX_LIGHTMAP,	//	Lightmap
	A3DSDTEX_BUMPMAP,	//	Bumpmap
	A3DSDTEX_PROCESS,	//	Process texture
	A3DSDTEX_DETAIL,	//	Detail texture
	A3DSDTEX_ANIMATION,	//	Animation texture
	A3DSDTEX_D3DTEX,	//	Raw D3D texture
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class AScriptFile;
class A3DPixelShader;
class A3DEngine;
class A3DTextureProxy;

typedef void (*LPFNUVGENERATE)(float* pu, float* pv);	//	U, V generate or transform function
typedef void (*LPFNPROCESSTEXTURE)();		//	Process texture function

//	Wave function used by shader
struct SHADERWAVE
{
	int			iType;			//	Wave applied for what ?
	int			iFunc;			//	Wave function
	float		fBase;			//	Wave base
	float		fAmp;			//	Amplitude
	float		fPhase;			//	Initial phase
	int			iPeriod;		//	Period (ticks)
};

//	General properties
struct SHADERGENERAL
{
	DWORD		dwMask;			//	Mask flags
	
	DWORD		dwZFunc;		//	Depth compare function
	bool		bZEnable;		//	Depth compare enable
	bool		bZWriteEnable;	//	Depth write enable
	DWORD		dwAlphaFunc;	//	Alpha compare function
	int			iAlphaRef;		//	Alpha reference value
	DWORD		dwCullMode;		//	Cull mode
	DWORD		dwSrcBlend;		//	Source blend factor
	DWORD		dwDstBlend;		//	Destination blend factor

	A3DObject*	pPShader;		//	Pixel shader

	SHADERWAVE	rgbGen;			//	Color generator
	SHADERWAVE	AlphaGen;		//	Alpha generator

	DWORD		dwTFactor;		//	Texture factor; ps's constant c0 if ps is enabled
	DWORD		dwTTemp;		//	Texture Temp Color; ps's constant c1 if ps is enabled

	A3DCOLORVALUE	aPSConsts[6];	//	ps's constant registers c2-c6

	int			iTransTextureIndex;	//	indicate the transparent texture for alpha test in z-pre pass, Added by PanYupin

	int			iTransChannel;
public:

	SHADERGENERAL();
	SHADERGENERAL(const SHADERGENERAL& s);

	const SHADERGENERAL& operator = (const SHADERGENERAL& s);

protected:

	void Duplicate(const SHADERGENERAL& s);
};

//	Stage properties
struct SHADERSTAGE
{
	DWORD		dwMask;			//	Mask flags
	DWORD		dwStates;		//	States
	
	//	Texture properties
	A3DSHADERTEXTYPE	iTexType;		//	Texture type
	A3DFORMAT			TexFormat;		//	Texture format
	APtrArray<void*>	aTextures;		//	Texture pointers

	DWORD		dwFrameTime;	//	Frame time (ms)
	DWORD		dwFilter;		//	Magnification and minification filter
	DWORD		dwAddress;		//	Texture-addressing method for U and V
	
	//	Color and alpha
	DWORD		dwColorOp;		//	Color operation
	DWORD		dwColorArg1;	//	Color argument 1
	DWORD		dwColorArg2;	//	Color argument 2
	DWORD		dwAlphaOp;		//	Alpha operation
	DWORD		dwAlphaArg1;	//	Alpha argument 1
	DWORD		dwAlphaArg2;	//	Alpha argument 2
	float		r, g, b, a;		//	Color and alpha generator
	DWORD		dwAlphaFunc;	//	Alpha compare function
	int			iAlphaRef;		//	Alpha reference value
	DWORD		dwResultArg;	//	Result argument
	DWORD		dwTexCoord;		//	Texture coordinate index
	DWORD		dwTTFF;			//	Texture transform flags
	float		matTexTrans[16];//	Texture transform matrix

	//	Bump-mapping
	float		fBumpEnvMat00;	//	Bump-mapping matrix
	float		fBumpEnvMat01;
	float		fBumpEnvMat10;
	float		fBumpEnvMat11;
	float		fBumpEnvScale;	//	Scale value for bump-mapping
	float		fBumpEnvOffset;	//	Offset value for bump-mapping
	
	//	Process functions
	int			iUVGenFunc;		//	U, V generate or transform function's index, -1 means null
	int			iProTexFunc;	//	Process texture function's index, -1 means null

	int			iNumtcMod;		//	Number of TC modifier
	SHADERWAVE	atcMods[4];		//	Texture Coordinate modifier

	DWORD		dwColorOPOld;	//	Color operation before this shader apply
	DWORD		dwAlphaOPOld;	//	Alpha operation before this shader apply

    A3DVECTOR4  vColorFactor;   // for HLSL
    A3DVECTOR4  vAlphaFactor;

public:

	SHADERSTAGE();
	SHADERSTAGE(const SHADERSTAGE& s);
	const SHADERSTAGE& operator = (const SHADERSTAGE& s);
    bool ComputeHLSLParams();

protected:

	void Duplicate(const SHADERSTAGE& s);
};

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DShader
//
///////////////////////////////////////////////////////////////////////////

class A3DShader : public A3DTexture
{
public:		//	Types

	enum
	{
		//	Version changes:
		//	7: add A3DSDTEX_D3DTEX support
		VERSION				= 7,	//	Current version

		MAXNUM_STAGE		= 8,	//	Maximum number of stage
		MAXNUM_TCMOD		= 5,	//	Maximum number of TC modifiers
	};

	//	Dynamic function values
	enum
	{
		VAL_GENCOLORGEN		= 0,	//	General color generate
		VAL_GENALPHAGEN		= 1,	//	General alpha generate
		VAL_TCMODSTRETCH0	= 2,	//	tcmod stretch for stage 0 -- 8
		VAL_TCMODUSCROLL0	= 10,	//	tcmod u scroll for stage 0 -- 8
		VAL_TCMODVSCROLL0	= 18,	//	tcmod v scroll for stage 0 -- 8
		VAL_TCMODROTATE0	= 26,	//	tcmod rotate for stage 0 -- 8
		VAL_TCMODOUTURB0	= 34,	//	tcmod turbulence u offset for stage 0 -- 8
		VAL_TCMODOVTURB0	= 42,	//	tcmod turbulence v offset for stage 0 -- 8
		VAL_TCMODSUTURB0	= 50,
		VAL_TCMODSVTURB0	= 58,

		VAL_NUMBER			= 66	//	Number of value
	};

	//	State flags of stage
	enum
	{
		STGF_HIDE		= 0x0001	//	Stage is hidden
	};

	//	Animation frame
	struct ANIMFRAME
	{
		DWORD	dwTimeCnt;		//	Time counter
		int		iCurFrame;		//	Current frame
		int		iCurStartFrame;	//	Current start frame
		int		iCurEndFrame;	//	Current end frame
		bool	bLoop;			//  Loop or not flag
	};

public:		//	Constructors and Destructors

	A3DShader();
	virtual ~A3DShader();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual bool Release();

	//	Apply shader
	virtual bool Appear(int iLayer=0);

    //	Abolish shader
	virtual bool Disappear(int iLayer=0);
	//	Tick animation
	virtual bool TickAnimation();

	//	Load shader from file
	bool Load(AFile* pFile);
	//	Load shader from file
	bool Load(const char* szFile);

	//	Get general properties
	SHADERGENERAL& GetGeneralProps() { return m_General; }
	//	Set general properties
	void SetGeneralProps(SHADERGENERAL& Props);
	//	Get general colorgen value
	bool GetCurGenColorGenerate(float* pfVal);
	//	Get general alphagen value
	bool GetCurGenAlphaGenerate(float* pfVal);
	//	Change pixel shader
	bool ChangePixelShader(const char* szFileName);

	//	Get stage number
	int	GetStageNum() { return m_iNumStage;	}
	//	Get stage properties
	SHADERSTAGE& GetStage(int iStage) { return m_aStages[iStage]; }
	//	Set a stage properties
	bool SetStage(int iStage, SHADERSTAGE& Stage);
	//	Insert a stage
	bool InsertStage(int iIndex, SHADERSTAGE& Stage);
	//	Remove a stage
	void RemoveStage(int iIndex);
	//	Change a stage's texture
	bool ChangeStageTexture(int iStage, A3DSHADERTEXTYPE iType, DWORD dwTex, A3DFORMAT Fmt=A3DFMT_UNKNOWN, int iNumAnimTex=1);
	bool ChangeStageTexture_D3DTex(int iStage, A3DTextureProxy* pTexProxy);
	//	Get current frame of stage's animation texture
	int GetCurAnimTexFrame(int iStage);
	//	Set current frame of stage's animation texture
	void SetCurAnimTexFrame(int iStage, int iFrame);
	//	Set current animation info of stage's animation texture
	void SetCurAnimInfo(int iStage, int iStartFrame, int iEndFrame, bool bLoop, int iFrameTime);
	//	Step animation texture of specified texture
	int StepAnimTexFrame(int iStage, int iStep=1);

	//	Check whether a stage is enable
	bool StageIsVisible(int iStage)	{ return (m_aStages[iStage].dwStates & STGF_HIDE) ? false : true; }
	//	Enable / Disable a stage
	void ShowStage(int iStage, bool bShow)
	{
		if (!bShow)
			m_aStages[iStage].dwStates |= STGF_HIDE;
		else
			m_aStages[iStage].dwStates &= ~STGF_HIDE;
	}

	//	only affect active stages, leave other stages not affected
	bool GetOnlyActiveStagesFlag() { return m_bOnlyActiveStages; }
	void SetOnlyActiveStagesFlag(bool bFlag) { m_bOnlyActiveStages = bFlag; }
	//	Set alpha texture flag
	void SetAlphaTextureFlag(bool bAlpha) { m_bAlphaTexture = bAlpha; }

	A3DTexture*	GetBaseTexture() const { return m_pBaseTexture; }
	
	A3DTexture* GetTransparentMaskTexture() const { return m_pTransMaskTexture; } //Added by PanYupin, for alphatest in z-prepass
	int GetTransparentMaskTextureIndex() const { return m_General.iTransTextureIndex; }
	int	GetTransparentChannel() const { return m_General.iTransChannel; }

	int GetVersion() const { return m_iVersion; }

    A3DMATRIX4 GetTextureMatrix(int iIndex);

protected:	//	Attributes

	static const SHADERGENERAL	m_DefGeneral;	//	Default general properties
	static const SHADERSTAGE	m_DefStage;		//	Default stage properties

	bool			m_bOnlyActiveStages;		//  flags indicates the shader only need to set active stages, don't touch other stages.
	A3DEngine*		m_pA3DEngine;				//	A3D engine object
	A3DDevice*		m_pA3DDevice;				//	A3D device object

	int				m_iVersion;					//	Shader version
	SHADERGENERAL	m_General;					//	General properties
	SHADERSTAGE		m_aStages[MAXNUM_STAGE];	//	Stages
	int				m_iNumStage;				//	Number of stage
	int				m_iMaxNumStage;				//	Maximum number of stage

	DWORD			m_dwStartTime;				//	Start time
	float			m_aFuncVal[VAL_NUMBER];		//	Dynamic function value
	ANIMFRAME		m_aCurFrame[MAXNUM_STAGE];	//	Current frame of every stage
	A3DTexture*		m_pBaseTexture;
	A3DTexture*		m_pTransMaskTexture;
protected:	//	Operations

	//	Load a stage
	bool LoadStage(AScriptFile* pFile);
	bool LoadStageProperty(AScriptFile* pFile);	//	Load a stage property
	bool LoadGeneralProperty(AScriptFile* pFile);	//	Load a general property

	bool LoadTCMod(AScriptFile* pFile, int iType, SHADERSTAGE* pStage);	//	Load a tc modifier property
	bool LoadWave(AScriptFile* pFile, SHADERWAVE* pWave);	//	Load a wave function

	//	Apply general properties
	void ApplyGeneral();
	//	Apply default general properties
	void ApplyDefGeneral();
	//	Apply one stage
	void ApplyStage(int iStage, int iIndex);
	//	Restore specified stage to default value
	void RestoreStage(int iStage, int iIndex);

	//	Calculate wave function value
	float CaluWaveValue(SHADERWAVE& Wave, DWORD dwTime);
	//	Calculate turbulence value
	void CaluTurbulence(SHADERWAVE& Wave, DWORD dwTime, int iStage);

	//	Release a stage's texture
	void ReleaseStageTexture(int iStage);
	//	Release pixel shader
	void ReleasePixelShader();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSHADER_H_


