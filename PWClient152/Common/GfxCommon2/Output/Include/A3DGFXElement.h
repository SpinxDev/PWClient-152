/*
 * FILE: A3DGFXElement.h
 *
 * DESCRIPTION: Base Class of GFX classes
 *
 * CREATED BY: ZhangYu, 2004/7/9
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXELEMENT_H_
#define _A3DGFXELEMENT_H_

#include "A3DObject.h"
#include "A3DGFXKeyPoint.h"
#include "A3DAnimationTrack.h"
#include "A3DDevice.h"
#include "A3DGFXEditorInterface.h"
#include "AFile.h"
#include "A3DTypes.h"
#include "A3DGFXStreamMan.h"
#include "A3DGFXTexMan.h"
#include "A3DSkinModel.h"
#include "GfxCommonTypes.h"
#include "A3DHLPixelShader.h"

#define		ID_ELE_TYPE_NONE				-1
#define		ID_ELE_TYPE_DECAL_3D			100
#define		ID_ELE_TYPE_DECAL_2D			101
#define		ID_ELE_TYPE_DECAL_BILLBOARD		102
#define		ID_ELE_TYPE_TRAIL				110
#define		ID_ELE_TYPE_TRAIL_EX			111
#define		ID_ELE_TYPE_PARTICLE_POINT		120
#define		ID_ELE_TYPE_PARTICLE_BOX		121
#define		ID_ELE_TYPE_PARTICLE_MULTIPLANE	122
#define		ID_ELE_TYPE_PARTICLE_ELLIPSOID	123
#define		ID_ELE_TYPE_PARTICLE_CYLINDER	124
#define		ID_ELE_TYPE_PARTICLE_CURVE		125
#define		ID_ELE_TYPE_PARTICLE_SKELETON	126
#define		ID_ELE_TYPE_LIGHT				130
#define		ID_ELE_TYPE_RING				140
#define		ID_ELE_TYPE_LIGHTNING			150
#define		ID_ELE_TYPE_LTNBOLT				151
#define		ID_ELE_TYPE_LIGHTNINGEX			152
#define		ID_ELE_TYPE_MODEL				160
#define		ID_ELE_TYPE_SOUND				170
#define		ID_ELE_TYPE_LTNTRAIL			180
#define		ID_ELE_TYPE_PARABOLOID			190
#define		ID_ELE_TYPE_GFX_CONTAINER		200
#define		ID_ELE_TYPE_GRID_DECAL_3D		210
#define		ID_ELE_TYPE_GRID_DECAL_2D		211
#define		ID_ELE_TYPE_PHYS_EMITTER		220
#define		ID_ELE_TYPE_PHYS_POINTEMITTER	221
#define		ID_ELE_TYPE_ECMODEL				230
#define		ID_ELE_TYPE_RIBBON				240
#define		ID_ELE_TYPE_MODEL_PROXY			250

#define		ID_SHADER_NORMAL				100
#define		ID_SHADER_HILIGHT				101
#define		ID_SHADER_SUPER_HILIGHT			102
#define		ID_SHADER_COLOR					103

#define		ID_GFXOP_ALPHA_MODE				1000
#define		ID_GFXOP_TEX_PATH				1001
#define		ID_GFXOP_REPEATCOUNT			1002
#define		ID_GFXOP_REPEATDELAY			1003
#define		ID_GFXOP_ELE_NAME				1004
#define		ID_GFXOP_BIND					1005
#define		ID_GFXOP_ZTESTENABLE			1006
#define		ID_GFXOP_TEX_ROWS				1007
#define		ID_GFXOP_TEX_COLS				1008
#define		ID_GFXOP_TEX_INTERVAL			1009
#define		ID_GFXOP_GROUND_NORMAL			1010
#define		ID_GFXOP_PRIORITY				1011
#define		ID_GFXOP_IS_DUMMY				1012
#define		ID_GFXOP_DUMMY_ELE				1013
#define		ID_GFXOP_WARP					1014
#define		ID_GFXOP_TILE_MODE				1015
#define		ID_GFXOP_U_SPEED				1016
#define		ID_GFXOP_V_SPEED				1017
#define		ID_GFXOP_UV_INTERCHANGE			1018
#define		ID_GFXOP_U_REVERSE				1019
#define		ID_GFXOP_V_REVERSE				1020
#define		ID_GFXOP_RENDER_LAYER			1021
#define		ID_GFXOP_GROUND_HEIGHT			1022
#define		ID_GFXOP_TEX_NODOWNSAMPLE		1023
#define		ID_GFXOP_RESETONLOOPEND			1024
#define		ID_GFXOP_TEXANIMMAXTIME			1025
#define		ID_GFXOP_PIXELSHADERPATH		1026
#define		ID_GFXOP_SHADER_TEX				1027
#define		ID_GFXOP_CAN_DO_FADE_OUT		1028
#define		ID_GFXOP_SOFT_EDGE      		1030
#define		ID_GFXOP_USE_INNER_LIGHT   		1031

#define		ID_GFXOP_TRAIL_ORGPOS1			100
#define		ID_GFXOP_TRAIL_ORGPOS2			101
#define		ID_GFXOP_TRAIL_SEGLIFE			102
#define		ID_GFXOP_TRAIL_BIND				103
#define		ID_GFXOP_TRAIL_SPLINEMODE		104
#define		ID_GFXOP_TRAIL_SAMPLEFREQ		105
#define		ID_GFXOP_TRAIL_SHRINK	        106

#define		ID_GFXOP_LTNTRAIL_POS1			100
#define		ID_GFXOP_LTNTRAIL_POS2			101
#define		ID_GFXOP_LTNTRAIL_SEGLIFE		102
#define		ID_GFXOP_LTNTRAIL_MIN_AMP		103
#define		ID_GFXOP_LTNTRAIL_MAX_AMP		104
#define		ID_GFXOP_LTNTRAIL_BIND			105

#define		ID_GFXOP_DECAL_WIDTH			100
#define		ID_GFXOP_DECAL_HEIGHT			101
#define		ID_GFXOP_DECAL_ROTFROMVIEW		102
#define		ID_GFXOP_DECAL_GRNDNORM_ONLY	103
#define		ID_GFXOP_DECAL_NO_WID_SCALE		104
#define		ID_GFXOP_DECAL_NO_HEI_SCALE		105
#define		ID_GFXOP_DECAL_ORG_PT_WID		106
#define		ID_GFXOP_DECAL_ORG_PT_HEI		107
#define		ID_GFXOP_DECAL_Z_OFFSET			108
#define		ID_GFXOP_DECAL_MATCH_SURFACE	109
#define		ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM		110
#define		ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR	111
#define		ID_GFXOP_DECAL_SCREEN_DIMENSION	112

#define		ID_GFXOP_GRIDDECAL_W_VNUM		100
#define		ID_GFXOP_GRIDDECAL_H_VNUM		101
#define		ID_GFXOP_GRIDDECAL_GRIDSIZE		102
#define		ID_GFXOP_GRIDDECAL_Z_OFFSET		103
#define		ID_GFXOP_GRIDDECAL_AFFBYSCALE	104
#define		ID_GFXOP_GRIDDECAL_ROTFROMVIEW	105			//是否随视角偏转
#define		ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT 106		//贴地时距地面高度
#define		ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND	107		//总是贴地

// Property for PhysPointEmitter
#define		ID_GFXOP_PHYSPAR_PPE_POINTSIZE	100
#define		ID_GFXOP_PHYSPAR_PPE_SCALEA		101
#define		ID_GFXOP_PHYSPAR_PPE_SCALEB		102
#define		ID_GFXOP_PHYSPAR_PPE_SCALEC		103
#define		ID_GFXOP_PHYSPAR_PPE_ISFADE		104
#define		ID_GFXOP_PHYSPAR_PPE_FADETIME	105

// Property for PhysEmitter
#define		ID_GFXOP_PHYSPAR_PE_PARORIENT	100
#define		ID_GFXOP_PHYSPAR_PE_MAXSCALE	101
#define		ID_GFXOP_PHYSPAR_PE_MINSCALE	102
#define		ID_GFXOP_PHYSPAR_PE_MAXROT		103
#define		ID_GFXOP_PHYSPAR_PE_MINROT		104
#define		ID_GFXOP_PHYSPAR_PE_MAXCOLOR	105
#define		ID_GFXOP_PHYSPAR_PE_MINCOLOR	106
#define		ID_GFXOP_PHYSPAR_PE_PARWIDTH	107
#define		ID_GFXOP_PHYSPAR_PE_PARHEIGHT	108
#define		ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE	109

#define		ID_GFXOP_PARSYS_PAR_QUOTA		100
#define		ID_GFXOP_PARSYS_PAR_WIDTH		101
#define		ID_GFXOP_PARSYS_PAR_HEIGHT		102
#define		ID_GFXOP_PARSYS_PAR_3D			103
#define		ID_GFXOP_PARSYS_PAR_FACING		104
#define		ID_GFXOP_PARSYS_SCALE_NO_OFFSET	105
#define		ID_GFXOP_PARSYS_NO_WID_SCALE	106
#define		ID_GFXOP_PARSYS_NO_HEI_SCALE	107
#define		ID_GFXOP_PARSYS_ORG_PT_WID		108
#define		ID_GFXOP_PARSYS_ORG_PT_HEI		109
#define		ID_GFXOP_PARSYS_USE_PAR_UV		110		// Use Particle's UV to render (for a serial of frames in one texture)
#define		ID_GFXOP_PARSYS_USE_GRND_HEIGHT	111		// Use ground height for init the particle
#define		ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE		112
#define		ID_GFXOP_PARSYS_INIT_RANDOM		113
#define		ID_GFXOP_PARSYS_Z_OFFSET		114

#define		ID_GFXOP_LIGHT_TYPE				100
#define		ID_GFXOP_LIGHT_DIFFUSE			101
#define		ID_GFXOP_LIGHT_SPECULAR			102
#define		ID_GFXOP_LIGHT_AMBIENT			103
#define		ID_GFXOP_LIGHT_POSITION			104
#define		ID_GFXOP_LIGHT_DIRECTION		105
#define		ID_GFXOP_LIGHT_RANGE			106
#define		ID_GFXOP_LIGHT_FALLOFF			107
#define		ID_GFXOP_LIGHT_ATTENUATION0		108
#define		ID_GFXOP_LIGHT_ATTENUATION1		109
#define		ID_GFXOP_LIGHT_ATTENUATION2		110
#define		ID_GFXOP_LIGHT_THETA			111
#define		ID_GFXOP_LIGHT_PHI				112
#define		ID_GFXOP_INNER_USE				113

#define		ID_GFXOP_RING_RADIUS			100
#define		ID_GFXOP_RING_HEIGHT			101
#define		ID_GFXOP_RING_PITCH				102
#define		ID_GFXOP_RING_SECTS				103
#define		ID_GFXOP_RING_NORADSCALE		104
#define		ID_GFXOP_RING_NOHEISCALE		105
#define		ID_GFXOP_RING_ORGATCENTER		106

#define		ID_GFXOP_LN_START_POS			100
#define		ID_GFXOP_LN_END_POS				101
#define		ID_GFXOP_LN_SEGS				102
#define		ID_GFXOP_LN_NUM					103
#define		ID_GFXOP_LN_WAVELEN				104
#define		ID_GFXOP_LN_INTERVAL			105
#define		ID_GFXOP_LN_WIDTH_START			106
#define		ID_GFXOP_LN_WIDTH_END			107
#define		ID_GFXOP_LN_WIDTH_MID			108
#define		ID_GFXOP_LN_ALPHA_START			109
#define		ID_GFXOP_LN_ALPHA_END			110
#define		ID_GFXOP_LN_ALPHA_MID			111
#define		ID_GFXOP_LN_AMPLITUDE			112
#define		ID_GFXOP_LN_USE_NORMAL			113
#define		ID_GFXOP_LN_NORMAL				114
#define		ID_GFXOP_LN_FILTER				115
#define		ID_GFXOP_LN_WAVEMOVE			116
#define		ID_GFXOP_LN_WAVEMOVESPEED		117
#define		ID_GFXOP_LN_FIXWAVELENGTH		118
#define		ID_GFXOP_LN_NUMWAVES			119

#define		ID_GFXOP_LN_EX_RENDERSIDE		200
#define		ID_GFXOP_LN_EX_ISAPPENDLY		201
#define		ID_GFXOP_LN_EX_ISUSEVERTSLIFE	202
#define		ID_GFXOP_LN_EX_VERTSLIFE		203
#define		ID_GFXOP_LN_EX_ISTAILFADEOUT	204

#define		ID_GFXOP_BOLT_DEVIATION			100
#define		ID_GFXOP_BOLT_STEP_MAX			101
#define		ID_GFXOP_BOLT_STEP_MIN			102
#define		ID_GFXOP_BOLT_WIDTH_START		103
#define		ID_GFXOP_BOLT_WIDTH_END			104
#define		ID_GFXOP_BOLT_ALPHA_START		105
#define		ID_GFXOP_BOLT_ALPHA_END			106
#define		ID_GFXOP_BOLT_AMP				107
#define		ID_GFXOP_BOLT_STEPS				108
#define		ID_GFXOP_BOLT_BRANCHES			109
#define		ID_GFXOP_BOLT_INTERVAL			110
#define		ID_GFXOP_BOLT_PER_BOLTS			111
#define		ID_GFXOP_BOLT_CIRCLES			112

#define		ID_GFXOP_MODEL_PATH				100
#define		ID_GFXOP_MODEL_ACT_NAME			101
#define		ID_GFXOP_MODEL_LOOPS			102
#define		ID_GFXOP_MODEL_ALPHA_CMP		103
#define		ID_GFXOP_MODEL_WRITE_Z			104
#define		ID_GFXOP_MODEL_USE_3DCAMERA		105
#define		ID_GFXOP_MODEL_FACE_DIR			106

#define		ID_GFXOP_ECMODEL_PATH			100
#define		ID_GFXOP_ECMODEL_USECASTERSKIN	101
#define		ID_GFXOP_ECMODEL_ACT_NAME		102
#define		ID_GFXOP_ECMODEL_LDTYPE			103
#define		ID_GFXOP_ECMODEL_USERCMD		104

#define		ID_GFXOP_MODELPROXY_HIDECLIENTMODEL 100
#define		ID_GFXOP_MODELPROXY_USESTATICFRAME 101
#define		ID_GFXOP_MODELPROXY_INCLUDECHILDMODELS	102

#define		ID_GFXOP_SOUND_FORCE2D			100
#define		ID_GFXOP_SOUND_LOOP				101
//#define		ID_GFXOP_SOUND_VOLUME			102
#define		ID_GFXOP_SOUND_MIN_DIST			103
#define		ID_GFXOP_SOUND_MAX_DIST			104
#define		ID_GFXOP_SOUND_FILE				105
#define		ID_GFXOP_SOUND_VOLUME_MIN		106
#define		ID_GFXOP_SOUND_VOLUME_MAX		107
#define		ID_GFXOP_SOUND_PITCH_MIN		108
#define		ID_GFXOP_SOUND_PITCH_MAX		109
#define		ID_GFXOP_SOUND_USECUSTOM		110

#define		ID_GFXOP_PARAB_COEFF			100
#define		ID_GFXOP_PARAB_HEIGHT			101

#define		ID_GFXOP_CONTAINER_GFX_PATH		100
#define		ID_GFXOP_CONTAINER_OUT_COLOR	101
#define		ID_GFXOP_CONTAINER_LOOP_FLAG	102
#define		ID_GFXOP_CONTAINER_GFX_PLAYSPEED 103
#define		ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY 104

#define		ID_GFXOP_RIBBON_ORGPOS1			100
#define		ID_GFXOP_RIBBON_ORGPOS2			101
#define		ID_GFXOP_RIBBON_SEGLIFE			102
#define		ID_GFXOP_RIBBON_BIND			103
#define		ID_GFXOP_RIBBON_TIME_TO_GRAVITY		104
#define		ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY	105
#define		ID_GFXOP_RIBBON_GRAVITY			106
#define		ID_GFXOP_RIBBON_VERTICAL_NOISE	107
#define		ID_GFXOP_RIBBON_VERTICAL_SPEED	108
#define		ID_GFXOP_RIBBON_HORZ_AMPLITUDE	109
#define		ID_GFXOP_RIBBON_HORZ_SPEED		110
#define		ID_GFXOP_RIBBON_X_NOISE	111
#define		ID_GFXOP_RIBBON_Z_NOISE	112

#ifdef _ANGELICA21
#define GFX_PS_CONST_INDEX_TEX_ADDR_TRANS		8
#define GFX_PS_CONST_INDEX_FOG_COLOR			9
#else
#define GFX_PS_CONST_INDEX_TEX_ADDR_TRANS		7
#endif

inline A3DVECTOR4 TransformToScreen(const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
	A3DVECTOR4 ret(v);
	ret = mat * ret;
	float rhw = 1.0f / ret.w;
	ret.x *= rhw;
	ret.y *= rhw;
	ret.z *= rhw;
	ret.w = rhw;
	return ret;
}

inline bool EleIsParticleSystem(int id)
{
	switch (id)
	{
	case ID_ELE_TYPE_PARTICLE_POINT:
	case ID_ELE_TYPE_PARTICLE_BOX:
	case ID_ELE_TYPE_PARTICLE_MULTIPLANE:
	case ID_ELE_TYPE_PARTICLE_ELLIPSOID:
	case ID_ELE_TYPE_PARTICLE_CYLINDER:
	case ID_ELE_TYPE_PARTICLE_CURVE:
		return true;
	}

	return false;
}

enum GFX_STATE
{
	ST_STOP		= 0,
	ST_PLAY		= 1,
	ST_PAUSE	= 2,
	ST_EDITMODE	= 3
};

enum
{
	GFX_RENDER_LAYER_NORMAL	= 0,
	GFX_RENDER_LAYER_BASE1,
	GFX_RENDER_LAYER_FRONT1,
	GFX_RENDER_LAYER_BASE2,
	GFX_RENDER_LAYER_FRONT2,
	GFX_RENDER_LAYER_BASE3,
	GFX_RENDER_LAYER_FRONT3,
	GFX_RENDER_LAYER_BASE4,
	GFX_RENDER_LAYER_FRONT4,
	GFX_RENDER_LAYER_COUNT,
};

class EVENT_INFO;
class GfxSoundParamInfo
{
public:
	GfxSoundParamInfo() :
	m_bForce2D(false),
	m_bLoop(false),
	m_dwVolumeMax(100),
	m_dwVolumeMin(100),
	m_bAbsoluteVolume(false),
	m_fPitchMin(0),
	m_fPitchMax(0),
	m_fMinDist(12),
	m_fMaxDist(50),
	m_bFixSpeed(true),
	m_iSilentHeader(0),
	m_fPertentStart(1.0f),
	m_nGroup(0)
	{
	}
	virtual ~GfxSoundParamInfo() {}

protected:
	bool	m_bForce2D;
	bool	m_bLoop;
	DWORD	m_dwVolumeMax, m_dwVolumeMin;
	bool	m_bAbsoluteVolume;		// whether m_dwVolumeMax, m_dwVolumeMin are absolute values
	float	m_fPitchMax, m_fPitchMin;
	float	m_fMinDist;
	float	m_fMaxDist;
	bool	m_bFixSpeed;
	int		m_iSilentHeader;	// 开始处无声部分的长度(单位ms)，可用于修正事件开始时间
	float	m_fPertentStart;	//播放的几率
	int		m_nGroup;			//数据套数

public:
	bool GetForce2D() const { return m_bForce2D; }
	void SetForce2D(bool bForce2D) { m_bForce2D = bForce2D; }
	bool GetLoop() const { return m_bLoop; }
	void SetLoop(bool bLoop) { m_bLoop = bLoop; }
	DWORD GetRandVolume() const;
	DWORD GetVolumeMax() const { return m_dwVolumeMax; }
	DWORD GetVolumeMin() const { return m_dwVolumeMin; }
	bool GetIsAbsoluteVolume() const { return m_bAbsoluteVolume; }
	void SetIsAbsoluteVolume(bool bAbsolute) { m_bAbsoluteVolume = bAbsolute; }
	float GetRandPitch() const;
	void SetVolume(DWORD dwVolume) { m_dwVolumeMax = m_dwVolumeMin = dwVolume; }
	void SetVolume(DWORD dwVolumeMin, DWORD dwVolumeMax);
	float GetPitchMin() const { return m_fPitchMin; }
	float GetPitchMax() const { return m_fPitchMax; }
	void SetSoundPitch(float fPitchMin, float fPitchMax);
	float GetMinDist() const { return m_fMinDist; }
	void SetMinDist(float fMinDist) { m_fMinDist = fMinDist; }
	float GetMaxDist() const { return m_fMaxDist; }
	void SetMaxDist(float fMaxDist) { m_fMaxDist = fMaxDist; }
	bool GetFixSpeed() const { return m_bFixSpeed; }
	void SetFixSpeed(bool bFixSpeed) { m_bFixSpeed = bFixSpeed; }
	int GetSilentHeader() const { return m_iSilentHeader; }
	void SetSilentHeader(int iSilentHeader) { m_iSilentHeader = iSilentHeader; }
	void SetPercentStart(float fPercent) { m_fPertentStart = fPercent; }
	float GetPercentStart() const { return m_fPertentStart; }
	int GetGroup() const { return m_nGroup; }
	void SetGroup(int n) { m_nGroup = n; }
	bool LoadSoundParamInfo(AFile* pFile);
	bool SaveSoundParamInfo(AFile* pFile);
	void Clone(const GfxSoundParamInfo* pSrc);
	GfxSoundParamInfo& operator = (const GfxSoundParamInfo& src);
};

class A3DGFXEx;
struct A3DTLWARPVERTEX;


struct PointSpriteInfo
{
	/*
	Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex. 
	This value is not used when the vertex contains point size. 
	This value is in screen space units if D3DRS_POINTSCALEENABLE is FALSE; otherwise this value is in world space units. 
	The default value is 1.0f. The range for this value is greater than or equal to 0.0f. 
	Because the IDirect3DDevice8::SetRenderState method accepts DWORD values, your application must cast a variable that contains the value, as shown in the following code example. 
	pd3dDevice8->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&PointSize));
	*/
	float fPointSize;

	/*
	Float value that controls for distance-based size attenuation for point primitives. 
	Active only when D3DRS_POINTSCALEENABLE is TRUE. 
	The default value is 1.0f. The range for this value is greater than or equal to 0.0f. 
	Because the IDirect3DDevice8::SetRenderState method accepts DWORD values, your application must cast a variable that contains the value, as shown in the following code example. 
	pd3dDevice8->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&PointScaleA));
	pd3dDevice8->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&PointScaleB));
	pd3dDevice8->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&PointScaleC));

	D3DRS_POINTSCALE_A, D3DRS_POINTSCALE_B, D3DRS_POINTSCALE_C Controls how the size of the PointSprites changes when the distance changes
	The distance means the space between a PointSprite and the Camera
	Direct3D use the Function below to calculate the final size of a PointSprite
	FinalSize = ViewportHeight * Size * sqrt( 1 / (A + B(D) + C(D^2)) )
	*/
	float fScaleA;
	float fScaleB;
	float fScaleC;
};

// pixel shader .hlsl (with annotation)
class GFXEleHLSLInfo
{
public:
	GFXEleHLSLInfo() : m_pHLPixelShader(a3d_CreateHLPixelShader())
	{
		ASSERT( m_pHLPixelShader );
	}
	GFXEleHLSLInfo(const GFXEleHLSLInfo& rhs)
		: m_pHLPixelShader(rhs.m_pHLPixelShader->Clone())
		, m_animationTrack(rhs.m_animationTrack)
	{
		ASSERT( m_pHLPixelShader );
	}
	
	GFXEleHLSLInfo& operator = (const GFXEleHLSLInfo& obj)
	{
		if (this == &obj)
			return *this;

		Swap(GFXEleHLSLInfo(obj));
		return *this;
	}

	~GFXEleHLSLInfo()
	{
		Release();
		delete m_pHLPixelShader;
	}

	void Swap(GFXEleHLSLInfo& rhs)
	{
		std::swap(m_pHLPixelShader, rhs.m_pHLPixelShader);
		m_animationTrack.Swap(rhs.m_animationTrack);
	}

	void Release()
	{
		m_pHLPixelShader->ReleaseShader();
	}

	A3DHLPixelShader* PixelShader() const { return m_pHLPixelShader; }
	bool HasPixelShader() const { ASSERT( m_pHLPixelShader ); return m_pHLPixelShader && m_pHLPixelShader->HasShaderFile(); }
	A3DAnimationTrack& AnimationTrack() { return m_animationTrack; }
	bool Save(AFile *file) const;
	bool Load(AFile *file);

private:
	A3DAnimationTrack m_animationTrack;
	A3DHLPixelShader* m_pHLPixelShader;
};

struct GFXEleAnimtableProperty		// A3DAnimatable 的编辑信息
{
	AString dispName;	// 显示名
	AString desc;		// 描述
	enum
	{
		UIWAY_DEFAULT,
		UIWAY_SLIDER,
	} uiWay;
	float minValue, maxValue, step;		// 目前仅用于单Float + Slider控件

	GFXEleAnimtableProperty() : uiWay(UIWAY_DEFAULT), minValue(-1e6f), maxValue(1e6f), step(0.01f)
		{}
};

class A3DShader;

class A3DGFXElement : public A3DObject
{
public:
	A3DGFXElement(A3DGFXEx* pGfx);
	virtual ~A3DGFXElement();

protected:
	int					m_nEleType;
	A3DGFXEx*			m_pGfx;
	A3DDevice*			m_pDevice;
	A3DSHADER			m_Shader;
	A3DGFXKeyPointSet	m_KeyPointSet;
	AString				m_strTexture;
	A3DTexture*			m_pTexture;

	//	pixel shader .sdr file
	//	Below members are kept for compatible with old files only and will not be used anymore
	/*********************************************/
	AString				m_strPixelShader;
	AString				m_strPixelShaderPrefix;
	AString				m_strShaderTex;
	AString				m_strPixelShaderEditInfo;
	/*********************************************/

	// hlsl shader .hlslp (with annotation)
	GFXEleHLSLInfo		m_HLSLInfo;
	A3DAnimationTrack   m_AnimatedProperty;

	GfxPSConstVec		m_vecPSConsts;
	AString				m_strBind;
	A3DGFXElement*		m_pBind;
	AString				m_strDummy;
	A3DGFXElement*		m_pDummy;
	DWORD				m_dwTexTimeRemain;
	DWORD				m_dwTexInterval;
	int					m_nTexRow;
	int					m_nTexCol;
	int					m_nCurTexRow;
	int					m_nCurTexCol;
	float				m_fUOffsetSpeed;
	float				m_fVOffsetSpeed;
	float				m_fTexU;
	float				m_fTexV;
	float				m_fTexWid;
	float				m_fTexHei;
	int					m_nPriority;
	A3DMATRIX4*			m_pDummyMat;
	A3DCOLOR			m_clDummy;
	float				m_fDummyScale;
	DWORD				m_dwRenderSlot;
	int					m_nRenderLayer;
	A3DAABB				m_AABB;
	DWORD				m_dwTexTimeTotal;	//	Texture only tick for this time totally
	DWORD				m_dwTexTickCount;
	DWORD				m_dwEleTickTime;

	bool				m_bTileMode;
	bool				m_bDummy;
	bool				m_bVisible;
	bool				m_bTotalBind;
	bool				m_bZEnable;
	bool				m_bGroundNormal;
	bool				m_bGroundHeight;
	bool				m_bNeedCalcTex;
	bool				m_bTexNoDownSample;
	bool				m_bUVInterchange;
	bool				m_bUReverse;
	bool				m_bVReverse;
	bool				m_bWarp;
	bool				m_bTLVert;
	bool				m_bAddedToWarpQueue;
	bool				m_bAddedToPostQueue;
	bool				m_bInit;
	bool				m_bIsPhysEle;
	bool				m_bResetWhenResumeLoop;
	bool				m_bCanDoFadeOut;
    bool                m_bSoftEdge;
	bool				m_bAbsTexPath;
	bool				m_bUseInnerLight;

public:
	static int			m_nUnnamedCount;

protected:
	void _CloneBase(const A3DGFXElement* pSrc)
	{
		m_strName		= pSrc->m_strName;
		m_strTexture	= pSrc->m_strTexture;
		m_strPixelShader= pSrc->m_strPixelShader;
		m_strShaderTex	= pSrc->m_strShaderTex;
		m_Shader		= pSrc->m_Shader;
		m_HLSLInfo		= pSrc->m_HLSLInfo;
		m_KeyPointSet	= pSrc->m_KeyPointSet;
		m_strBind		= pSrc->m_strBind;
		m_strDummy		= pSrc->m_strDummy;
		m_bTotalBind	= pSrc->m_bTotalBind;
		m_bZEnable		= pSrc->m_bZEnable;
		m_bGroundNormal	= pSrc->m_bGroundNormal;
		m_bGroundHeight	= pSrc->m_bGroundHeight;
		m_bTileMode		= pSrc->m_bTileMode;
		m_fUOffsetSpeed	= pSrc->m_fUOffsetSpeed;
		m_fVOffsetSpeed	= pSrc->m_fVOffsetSpeed;
		m_nTexRow		= pSrc->m_nTexRow;
		m_nTexCol		= pSrc->m_nTexCol;
		m_bUVInterchange= pSrc->m_bUVInterchange;
		m_bUReverse		= pSrc->m_bUReverse;
		m_bVReverse		= pSrc->m_bVReverse;
		m_dwTexInterval	= pSrc->m_dwTexInterval;
		m_bTexNoDownSample	= pSrc->m_bTexNoDownSample;
		m_nPriority		= pSrc->m_nPriority;
		m_bDummy		= pSrc->m_bDummy;
		if (m_bDummy) m_pDummyMat = new A3DMATRIX4;
		m_bWarp			= pSrc->m_bWarp;
		m_bTLVert		= pSrc->m_bTLVert;
		m_nRenderLayer	= pSrc->m_nRenderLayer;
		m_bIsPhysEle	= pSrc->m_bIsPhysEle;
		m_bResetWhenResumeLoop = pSrc->m_bResetWhenResumeLoop;
		m_bCanDoFadeOut	= pSrc->m_bCanDoFadeOut;
        m_bSoftEdge 	= pSrc->m_bSoftEdge;
		m_bUseInnerLight= pSrc->m_bUseInnerLight;
		m_dwTexTimeTotal = pSrc->m_dwTexTimeTotal;
		m_AnimatedProperty = pSrc->m_AnimatedProperty;
		CalcTexAnimation();

		const size_t sz = pSrc->m_vecPSConsts.size();
		m_vecPSConsts.reserve(sz);

		for (size_t i = 0; i < sz; i++)
			m_vecPSConsts.push_back(pSrc->m_vecPSConsts[i]);
	}

	void CalcTexAnimation()
	{
		if (m_bTileMode)
		{
			m_bNeedCalcTex = true;
			m_fTexWid = static_cast<float>(m_nTexCol);
			m_fTexHei = static_cast<float>(m_nTexRow);
		}
		else if (m_nTexRow == 1 && m_nTexCol == 1)
		{
			m_bNeedCalcTex = false;
			m_fTexWid = static_cast<float>(m_nTexCol);
			m_fTexHei = static_cast<float>(m_nTexRow);
		}
		else
		{
			m_bNeedCalcTex = true;
			m_fTexWid	= 1.f / m_nTexCol;
			m_fTexHei	= 1.f / m_nTexRow;
		}
	}

	void TexAnimation(DWORD dwTickTime)
	{
		if (m_bTileMode)
		{
			float fTickTime = dwTickTime * .001f;
			m_fTexU += m_fUOffsetSpeed * fTickTime;
			m_fTexV += m_fVOffsetSpeed * fTickTime;
		}
		else
		{
			m_dwTexTimeRemain += dwTickTime;
			if (m_dwTexTimeRemain < m_dwTexInterval) return;
			m_dwTexTimeRemain -= m_dwTexInterval;
	
			if (++m_nCurTexCol >= m_nTexCol)				
			{
				if (++m_nCurTexRow >= m_nTexRow)
				{
					m_nCurTexRow = 0;
					m_nCurTexCol = 0;
				}
				else
					m_nCurTexCol = 0;
			}
			
			m_fTexU = m_fTexWid * m_nCurTexCol;
			m_fTexV = m_fTexHei * m_nCurTexRow;
		}
	}
	void InitBaseData();
	void GetTexTrans(float vTexTrans[4]) const;
	virtual void ApplyPSConstants(A3DViewport* pView);
	//	ApplyPixelShaderWithFrameBuffer 
	//	TODO: 这是个临时的解决方案，对于只有一个新参数的情况
	//	我们增加一个函数来处理，因为这样实现起来比较简单，但是如果将来还要更多参数要传递，就需要重新想个办法了
	void ApplyPSConstantsWithFrameBuffer(A3DViewport* pView, A3DRenderTarget* pFrameBuffer);
	virtual void SkinModelRenderAtOnce(A3DSkinModel* pSkinModel, A3DViewport* pView, DWORD dwFlags, A3DReplaceHLSL* pRepHLSL);

public:
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion) = 0;
	virtual bool Save(AFile* pFile) = 0;
	void ReleaseTex()
	{
		if (m_pTexture)
		{
			m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
			m_pTexture = NULL;
		}
	}
	virtual void Release();
	virtual bool Play();
	virtual bool Pause() { return true; }
	virtual bool Stop();
	virtual bool StopParticleEmit() { return true; }
	virtual void DoFadeOut() {}
	virtual bool Reload();		// used by editor

	virtual bool Render(A3DViewport*) = 0;
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const = 0;
	virtual int GetVertsCount() { return 0; };
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView) { return 0; }
	virtual bool Init(A3DDevice* pDevice);
	virtual bool ChangeTexture();
	virtual A3DSkinModel* GetSkinModel() { return NULL; }
	virtual void PrepareRenderSkinModel() {}
	virtual void RenderSkinModel(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DRenderTarget* pFrameBuffer);
	virtual void RestoreRenderSkinModel() {}
	virtual void ResumeLoop();
	virtual void DummyTick(DWORD dwTick) {}
	bool IsInit() const { return m_bInit; }
	void SetInit(bool bInit) { m_bInit = bInit; }
	virtual A3DVECTOR3 GetAABBCenter();
	const A3DAABB& GetAABB() const { return m_AABB; }

	virtual bool IsParamEnable(int nParamId) const { return false; }
	virtual void EnableParam(int nParamId, bool bEnable) {}
	virtual void BeginUpdateParam() {}
	virtual bool NeedUpdateParam(int nParamID) const { return true; }
	virtual void UpdateParam(int nParamId, const GFX_PROPERTY& prop) {}
	virtual void EndUpdateParam() {}
	virtual GFX_PROPERTY GetParam(int nParamId) const { return GFX_PROPERTY(); }
	virtual GfxValueType GetParamType(int nParamId) const { return GFX_VALUE_UNKNOWN; }

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;

	//	interface used by GFXEditor
	virtual size_t GetSubProtertyCount(int nSubId) { return 0; }
	virtual bool AddSubProterty(int nSubId) { return false;	}
	virtual bool RemoveSubProterty(int nSubId, size_t nSubIdx) { return false; }
	virtual bool SetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId, const GFX_PROPERTY& prop) { return false; }
	virtual GFX_PROPERTY GetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId) const { return GFX_PROPERTY(); }
	
	virtual void GetPointSpriteInfo(PointSpriteInfo* pInfo) const {}
	// This function returns the property that is force use camera in 2d-viewport render
	// false for default
	virtual bool IsForceUse3DCamera() const { return false; }

public:
	A3DGFXEx* GetGfx() { return m_pGfx; };
	const A3DGFXEx* GetGfx() const { return m_pGfx; };
	A3DGFXKeyPointSet& GetKeyPointSet() { return m_KeyPointSet; }
	const A3DGFXKeyPointSet& GetKeyPointSet() const { return m_KeyPointSet; }
	void AddKeyPoint(A3DGFXKeyPoint* pKeyPoint) { m_KeyPointSet.AddKeyPoint(pKeyPoint);	}
	void InsertKeyPoint(int index, A3DGFXKeyPoint* pKeyPoint) { m_KeyPointSet.InsertKeyPoint(index, pKeyPoint); }
	int GetKeyPointCount() const { return m_KeyPointSet.GetKeyPointCount();	}
	void RemoveKeyPoint(int nIndex) { m_KeyPointSet.RemoveKeyPoint(nIndex);	}
	void RemoveKeyPoint(A3DGFXKeyPoint* p) { m_KeyPointSet.RemoveKeyPoint(p); }
	A3DGFXKeyPoint* GetKeyPoint(int nIndex) { return m_KeyPointSet.GetKeyPoint(nIndex); }
	A3DMATRIX4 GetKeyPointMat(int nIndex)
	{
		const KEY_POINT& kp = GetKeyPoint(nIndex)->GetKPOrg();
		A3DMATRIX4 mat;
		kp.m_vDir.ConvertToMatrix(mat);
		mat.SetRow(3, kp.m_vPos);
		return mat;
	}
	DWORD GetTimeStart() const { return m_KeyPointSet.GetTimeStart(); }
	void SetTimeStart(DWORD dwStart) { m_KeyPointSet.SetTimeStart(dwStart);	}
	virtual bool IsFinished() const { return m_KeyPointSet.IsFinished(); }
	bool IsActive() const { return m_KeyPointSet.IsActive(); }

	virtual bool CanRender() const { return IsInit() && (m_bDummy || IsActive()); }

public:
	static A3DGFXElement* CreateEmptyElement(A3DDevice* pDev, A3DGFXEx* pGfx, int nEleId);
	static A3DGFXElement* LoadElementFromFile(A3DDevice* pDev, A3DGFXEx* pGfx, AFile* pFile, DWORD dwVersion);
	bool SaveElementToFile(AFile* pFile);
	int GetEleTypeId() const { return m_nEleType; }
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool IsVisible() const { return m_bVisible;	}
	void SetToEditMode() {}
	void SetTotalBind(bool bTotal) { m_bTotalBind = bTotal; }
	A3DGFXElement* GetBindEle() { return m_pBind; }
	const AString& GetBindEleName() { return m_strBind; }
	void SetBindEle(A3DGFXElement* pBind);
	const AString& GetBindEleName() const { return m_strBind; }
	bool IsDummyEle() const { return m_bDummy; }
	A3DGFXElement* GetDummyEle() { return m_pDummy; }
	const AString& GetDummyName() const { return m_strDummy; }
	void SetDummyEle(A3DGFXElement* pDummy);
	bool GetSelfKeyPoint(KEY_POINT* pKeyPt) { return m_KeyPointSet.GetCurKeyPoint(pKeyPt); }
	bool GetCurKeyPoint(KEY_POINT* pKeyPt)
	{
		if (!m_KeyPointSet.GetCurKeyPoint(pKeyPt)) return false;
		if (m_pBind)
		{
			KEY_POINT kpBind;
			if (m_pBind->GetCurKeyPoint(&kpBind))
			{
				if (m_bTotalBind) *pKeyPt = kpBind;
				else
				{
					pKeyPt->m_vPos = kpBind.m_vPos;
					pKeyPt->m_vDir = kpBind.m_vDir;
				}
			}
			else return false;
		}
		return true;
	}
	A3DSHADER GetShader() const { return m_Shader; }
	void SetShader(const A3DSHADER& shader) { m_Shader = shader; }
	void CloneNaked(const A3DGFXElement* pSrc)
	{
		m_Shader		= pSrc->m_Shader;
		m_HLSLInfo		= pSrc->m_HLSLInfo;
		m_KeyPointSet	= pSrc->m_KeyPointSet;
		m_strBind		= pSrc->m_strBind;
		m_bTotalBind	= pSrc->m_bTotalBind;
		m_bZEnable		= pSrc->m_bZEnable;
	}
	void UpdateTex();
	void ApplyPixelShader(A3DViewport* pView);
	void RestorePixelShader();
	const AString& GetTexPath() const { return m_strTexture; }
	void SetTexPath(const char* szFile, bool bAbsTexPath = false)
	{
		m_strTexture = szFile;
		m_bAbsTexPath = bAbsTexPath;
	}

	const AString& GetPixelShaderPath() const { return m_strPixelShader; }
	void SetPixelShaderPath(const char* szPath) { m_strPixelShader = szPath; }
	const char* GetPixelShaderPrefix() const { return m_strPixelShaderPrefix; }
	void SetPixelShaderPrefix(const char* prefix) { m_strPixelShaderPrefix = prefix; }
	const char* GetPixelShaderEditInfo() const { return m_strPixelShaderEditInfo; }
	void SetPixelShaderEditInfo(const char* editInfo) { m_strPixelShaderEditInfo = editInfo; }
	const AString& GetShaderTexture() const { return m_strShaderTex; }
	void SetShaderTexture(const char* szPath) { m_strShaderTex = szPath; }
	GfxPSConstVec& GetPSConstVec() { return m_vecPSConsts; }
	DWORD GetPSTickTime() const { return m_dwEleTickTime; }
	A3DTexture* GetTexture() const { return m_pTexture; }

	GFXEleHLSLInfo& GetHLSLInfo() { return m_HLSLInfo; }
	const GFXEleHLSLInfo& GetHLSLInfo() const { return m_HLSLInfo; }
	A3DAnimationTrack& GetAnimationTrack() { return m_AnimatedProperty; }
	const A3DAnimationTrack& GetAnimationTrack() const { return m_AnimatedProperty; }
	//@para name animatable name in m_AnimatedProperty
	//@return false if does not has such property
	virtual bool GetAnimatableProperty(const char* name, GFXEleAnimtableProperty* pProperty/*out*/) { return false; };
	bool HasPixelShader() const { return m_HLSLInfo.PixelShader()->HasShaderFile(); }

	bool UseGroundNormal() const { return m_bGroundNormal; }
	bool UseGroundHeight() const { return m_bGroundHeight; }
	bool IsInfinite() const { return m_KeyPointSet.IsInfinite(); }
	void SetPriority(int nPriority) { m_nPriority = nPriority; }
	int GetPriority() const { return m_nPriority; }
	const A3DMATRIX4& GetParentTM() const;
	const A3DQUATERNION& GetParentDir() const;
	void SetDummyMatrix(const A3DMATRIX4& mat) { *m_pDummyMat = mat; }
	const A3DMATRIX4* GetDummyMatrix() const { return m_pDummyMat; }
	void SetDummyColor(A3DCOLOR cl) { m_clDummy = cl; }
	void SetDummyScale(float fScale) { m_fDummyScale = fScale; }
	void SetAddedToWarpQueue(bool b) { m_bAddedToWarpQueue = b; }
	bool GetAddedToWarpQueue() const { return m_bAddedToWarpQueue; }
	void SetAddedToPostQueue(bool b) { m_bAddedToPostQueue = b; }
	bool GetAddedToPostQueue() const { return m_bAddedToPostQueue; }
	bool CanDoFadeOut() const { return m_bCanDoFadeOut; }
	void SetCanDoFadeOut(bool b) { m_bCanDoFadeOut = b; }
    bool GetSoftEdge() const { return m_bSoftEdge; }
    void SetSoftEdge(bool b) { m_bSoftEdge = b; }
	bool GetUseInnerLight() const { return m_bUseInnerLight; }
	void SetUseInnerLight(bool b) { m_bUseInnerLight = b; }

	bool IsPhysEle() const { return m_bIsPhysEle; }
	bool IsTLVert() const { return m_bTLVert; }
	virtual int DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh) { return 0; }
	virtual int DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts) { return 0; }

protected:
	bool IsEleShouldBeDelayRendered() const;

};

// Global Functions
void g_GfxApplyPixelShaderConsts(A3DDevice* pA3DDevice, const GfxPSConstVec& ConstsVec, DWORD dwPSTickTime, int nIndexOffset);
void g_GfxLoadPixelShaderConsts(AFile* pFile, AString& strPixelShader, AString& strShaderTex, GfxPSConstVec& vecPSConsts);
void g_GfxSavePixelShaderConsts(AFile* pFile, const AString& strPixelShader, const AString& strShaderTex, const GfxPSConstVec& vecPSConsts);


#endif
