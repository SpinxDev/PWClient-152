#include "StdAfx.h"
#include "A3DGFXEditorInterface.h"
#include "A3DGFXElement.h"
#include "A3DGFXKeyPoint.h"
#include "A3DParticleEmitter.h"
#include "A3DParticleSystemEx.h"
#include "RandStringContainer.h"
#include "A3DGFXAnimable.h"
#include "A3DGFXFuncs.h"

#define OBJ_COUNT(n)	(sizeof(n) / sizeof(GFX_OBJ_INFO))
#define PROP_COUNT(n)	(sizeof(n) / sizeof(GFX_PROP_INFO))
#define PARAM_COUNT(n)	(sizeof(n) / sizeof(GFX_PARAM_INFO))
#define SHADER_COUNT	(sizeof(_gfx_shader_info) / sizeof(GFX_SHADER_INFO))

extern const GFX_PROP_INFO _ctrl_noise_base[];

#define GFX_BASE_INFO	\
{ _prop_base_info[0].m_szName, _prop_base_info[0].m_OpId },\
{ _prop_base_info[1].m_szName, _prop_base_info[1].m_OpId },\
{ _prop_base_info[2].m_szName, _prop_base_info[2].m_OpId },\
{ _prop_base_info[3].m_szName, _prop_base_info[3].m_OpId },\
{ _prop_base_info[4].m_szName, _prop_base_info[4].m_OpId },\
{ _prop_base_info[5].m_szName, _prop_base_info[5].m_OpId },\
{ _prop_base_info[6].m_szName, _prop_base_info[6].m_OpId },\
{ _prop_base_info[7].m_szName, _prop_base_info[7].m_OpId },\
{ _prop_base_info[8].m_szName, _prop_base_info[8].m_OpId },\
{ _prop_base_info[9].m_szName, _prop_base_info[9].m_OpId },\
{ _prop_base_info[10].m_szName, _prop_base_info[10].m_OpId },\
{ _prop_base_info[11].m_szName, _prop_base_info[11].m_OpId },\
{ _prop_base_info[12].m_szName, _prop_base_info[12].m_OpId },\
{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },\
{ _prop_base_info[14].m_szName, _prop_base_info[14].m_OpId },\
{ _prop_base_info[15].m_szName, _prop_base_info[15].m_OpId },\
{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },\
{ _prop_base_info[17].m_szName, _prop_base_info[17].m_OpId },\
{ _prop_base_info[18].m_szName, _prop_base_info[18].m_OpId },\
{ _prop_base_info[19].m_szName, _prop_base_info[19].m_OpId },\
{ _prop_base_info[20].m_szName, _prop_base_info[20].m_OpId }

#define GFX_REPEAT_INFO	\
{ _prop_repeat_info[0].m_szName, _prop_repeat_info[0].m_OpId },\
{ _prop_repeat_info[1].m_szName, _prop_repeat_info[1].m_OpId },\
{ _prop_repeat_info[2].m_szName, _prop_repeat_info[2].m_OpId },\
{ _prop_repeat_info[3].m_szName, _prop_repeat_info[3].m_OpId },\
{ _prop_repeat_info[4].m_szName, _prop_repeat_info[4].m_OpId }

#define GFX_PIXELSHADER_INFO	\
{ _prop_pixelshader_info[0].m_szName, _prop_pixelshader_info[0].m_OpId },\
{ _prop_pixelshader_info[1].m_szName, _prop_pixelshader_info[1].m_OpId },\

#define EMITTER_BASE_INFO	\
{ _emitter_base_info[0].m_szName, _emitter_base_info[0].m_OpId },\
{ _emitter_base_info[1].m_szName, _emitter_base_info[1].m_OpId },\
{ _emitter_base_info[2].m_szName, _emitter_base_info[2].m_OpId },\
{ _emitter_base_info[3].m_szName, _emitter_base_info[3].m_OpId },\
{ _emitter_base_info[4].m_szName, _emitter_base_info[4].m_OpId },\
{ _emitter_base_info[5].m_szName, _emitter_base_info[5].m_OpId },\
{ _emitter_base_info[6].m_szName, _emitter_base_info[6].m_OpId },\
{ _emitter_base_info[7].m_szName, _emitter_base_info[7].m_OpId },\
{ _emitter_base_info[8].m_szName, _emitter_base_info[8].m_OpId },\
{ _emitter_base_info[9].m_szName, _emitter_base_info[9].m_OpId },\
{ _emitter_base_info[10].m_szName, _emitter_base_info[10].m_OpId },\
{ _emitter_base_info[11].m_szName, _emitter_base_info[11].m_OpId },\
{ _emitter_base_info[12].m_szName, _emitter_base_info[12].m_OpId },\
{ _emitter_base_info[13].m_szName, _emitter_base_info[13].m_OpId },\
{ _emitter_base_info[14].m_szName, _emitter_base_info[14].m_OpId },\
{ _emitter_base_info[15].m_szName, _emitter_base_info[15].m_OpId },\
{ _emitter_base_info[16].m_szName, _emitter_base_info[16].m_OpId },\
{ _emitter_base_info[17].m_szName, _emitter_base_info[17].m_OpId },\
{ _emitter_base_info[18].m_szName, _emitter_base_info[18].m_OpId }

#define EMITTER_BASE_INFO_NOBIND	\
{ _emitter_base_info[0].m_szName, _emitter_base_info[0].m_OpId },\
{ _emitter_base_info[1].m_szName, _emitter_base_info[1].m_OpId },\
{ _emitter_base_info[2].m_szName, _emitter_base_info[2].m_OpId },\
{ _emitter_base_info[3].m_szName, _emitter_base_info[3].m_OpId },\
{ _emitter_base_info[4].m_szName, _emitter_base_info[4].m_OpId },\
{ _emitter_base_info[5].m_szName, _emitter_base_info[5].m_OpId },\
{ _emitter_base_info[6].m_szName, _emitter_base_info[6].m_OpId },\
{ _emitter_base_info[7].m_szName, _emitter_base_info[7].m_OpId },\
{ _emitter_base_info[8].m_szName, _emitter_base_info[8].m_OpId },\
{ _emitter_base_info[9].m_szName, _emitter_base_info[9].m_OpId },\
{ _emitter_base_info[10].m_szName, _emitter_base_info[10].m_OpId },\
{ _emitter_base_info[11].m_szName, _emitter_base_info[11].m_OpId },\
{ _emitter_base_info[12].m_szName, _emitter_base_info[12].m_OpId },\
{ _emitter_base_info[13].m_szName, _emitter_base_info[13].m_OpId },\
{ _emitter_base_info[14].m_szName, _emitter_base_info[14].m_OpId },\
{ _emitter_base_info[18].m_szName, _emitter_base_info[18].m_OpId }

#define PARSYS_BASE_INFO	\
{ _prop_parsys_base_info[0].m_szName, _prop_parsys_base_info[0].m_OpId },\
{ _prop_parsys_base_info[1].m_szName, _prop_parsys_base_info[1].m_OpId },\
{ _prop_parsys_base_info[2].m_szName, _prop_parsys_base_info[2].m_OpId },\
{ _prop_parsys_base_info[3].m_szName, _prop_parsys_base_info[3].m_OpId },\
{ _prop_parsys_base_info[4].m_szName, _prop_parsys_base_info[4].m_OpId },\
{ _prop_parsys_base_info[5].m_szName, _prop_parsys_base_info[5].m_OpId },\
{ _prop_parsys_base_info[6].m_szName, _prop_parsys_base_info[6].m_OpId },\
{ _prop_parsys_base_info[7].m_szName, _prop_parsys_base_info[7].m_OpId },\
{ _prop_parsys_base_info[8].m_szName, _prop_parsys_base_info[8].m_OpId },\
{ _prop_parsys_base_info[9].m_szName, _prop_parsys_base_info[9].m_OpId },\
{ _prop_parsys_base_info[10].m_szName, _prop_parsys_base_info[10].m_OpId },\
{ _prop_parsys_base_info[11].m_szName, _prop_parsys_base_info[11].m_OpId },\
{ _prop_parsys_base_info[12].m_szName, _prop_parsys_base_info[12].m_OpId },\
{ _prop_parsys_base_info[13].m_szName, _prop_parsys_base_info[13].m_OpId },\
{ _prop_parsys_base_info[14].m_szName, _prop_parsys_base_info[14].m_OpId }

#define AREA_EMITTER_BASE_INFO	\
{ _prop_areapar_info[0].m_szName, _prop_areapar_info[0].m_OpId }

#define CTRL_BASE_INFO	\
{ _ctrl_base[0].m_szName, _ctrl_base[0].m_OpId },\
{ _ctrl_base[1].m_szName, _ctrl_base[1].m_OpId },


#ifdef _GFX_LANG_EN

// English Version
/* !!!!!增加新项时，注意所有的语言版本需要对应更新	*/

static const GFX_SHADER_INFO _gfx_shader_info[] =
{
	{ "AlphaLightMode",	ID_SHADER_NORMAL,		A3DBLEND_SRCALPHA,	A3DBLEND_INVSRCALPHA },
	{ "HighLightMode",	ID_SHADER_HILIGHT,		A3DBLEND_SRCALPHA,	A3DBLEND_ONE },
	{ "SuperLightMode",	ID_SHADER_SUPER_HILIGHT,A3DBLEND_ONE,		A3DBLEND_ONE },
	{ "ColorLightMode",	ID_SHADER_COLOR,		A3DBLEND_ONE,		A3DBLEND_INVSRCCOLOR }
};

/* !!!!!IMPORTANT new prop must be added at tail */
/* !!!!!在尾部更新因为顶部的宏是使用索引的，而其他引用该宏的属性项可能直接依赖于索引值 */
/* !!!!!增加新项时，注意顶部对应的宏需要更新	*/
static const GFX_PROP_INFO _prop_base_info[] =
{
	{ "AlphaMode",		ID_GFXOP_ALPHA_MODE },
	{ "IsCoverable",	ID_GFXOP_ZTESTENABLE },
	{ "IsUseGroundNormal",	ID_GFXOP_GROUND_NORMAL },
	{ "TexturePath",	ID_GFXOP_TEX_PATH },
	{ "IsTileMode",		ID_GFXOP_TILE_MODE },
	{ "U coord speed",	ID_GFXOP_U_SPEED },
	{ "V coord speed",	ID_GFXOP_V_SPEED },
	{ "UV interchange",	ID_GFXOP_UV_INTERCHANGE },
	{ "U coord reverse",ID_GFXOP_U_REVERSE },
	{ "V coord reverse",ID_GFXOP_V_REVERSE },
	{ "Texture rows",	ID_GFXOP_TEX_ROWS },
	{ "Texture columns",ID_GFXOP_TEX_COLS },
	{ "Texture random",	ID_GFXOP_TEX_RANDOM},
	{ "Texture interval",	ID_GFXOP_TEX_INTERVAL },
	{ "Render priority",ID_GFXOP_PRIORITY },
	{ "Is dummy",		ID_GFXOP_IS_DUMMY },
	{ "Is generate warp",	ID_GFXOP_WARP },
	{ "Render layer",		ID_GFXOP_RENDER_LAYER },
	{ "Is using ground height",	ID_GFXOP_GROUND_HEIGHT },
	{ "Is no down tex sample",	ID_GFXOP_TEX_NODOWNSAMPLE },
	{ "Reset on loop end", ID_GFXOP_RESETONLOOPEND },
	{ "TexAnim max time", ID_GFXOP_TEXANIMMAXTIME },
};

static const GFX_PROP_INFO _prop_repeat_info[] =
{
	{ "Repeat count",		ID_GFXOP_REPEATCOUNT },
	{ "Repeat interval",	ID_GFXOP_REPEATDELAY },
	{ "Can do fade out",	ID_GFXOP_CAN_DO_FADE_OUT },
    { "Is soft edge",	    ID_GFXOP_SOFT_EDGE },
	{ "Use inner light",    ID_GFXOP_USE_INNER_LIGHT },
};

static const GFX_PROP_INFO _prop_pixelshader_info[] =
{
	{ "shader path",		ID_GFXOP_PIXELSHADERPATH },
	{ "shader texture",		ID_GFXOP_SHADER_TEX }
};

static const GFX_PROP_INFO _prop_decal_info[] =
{
	{ "Width",					ID_GFXOP_DECAL_WIDTH },
	{ "Height",					ID_GFXOP_DECAL_HEIGHT },
	{ "Is face to view",		ID_GFXOP_DECAL_ROTFROMVIEW },
	{ "Only use Ground Normal",	ID_GFXOP_DECAL_GRNDNORM_ONLY },
	{ "Is match surface",		ID_GFXOP_DECAL_MATCH_SURFACE },
	{ "No width scale",			ID_GFXOP_DECAL_NO_WID_SCALE },
	{ "No height scale",		ID_GFXOP_DECAL_NO_HEI_SCALE },
	{ "Origin point width",		ID_GFXOP_DECAL_ORG_PT_WID },
	{ "Origin point height",	ID_GFXOP_DECAL_ORG_PT_HEI },
	{ "Offset on Z",			ID_GFXOP_DECAL_Z_OFFSET },
	{ "Dir On Using Ground Normal", ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM},
	{ "surface use pafent dir",		ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR },
	{ "2d screen space dimension",	ID_GFXOP_DECAL_SCREEN_DIMENSION	},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_griddecal_info[] = 
{
	{ "Width Points",	ID_GFXOP_GRIDDECAL_W_VNUM },
	{ "Height Points",	ID_GFXOP_GRIDDECAL_H_VNUM },
	{ "Grid Size",		ID_GFXOP_GRIDDECAL_GRIDSIZE},
	{ "Offset on Z",	ID_GFXOP_GRIDDECAL_Z_OFFSET},
	{ "Affect by Scale",	ID_GFXOP_GRIDDECAL_AFFBYSCALE},
	{ "Rot From View",	ID_GFXOP_GRIDDECAL_ROTFROMVIEW },
	{ "Offset Height on Ground",	ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT},
	{ "Always on Ground" ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_physemitter_info[] = 
{
	{ "Particle width",		ID_GFXOP_PHYSPAR_PE_PARWIDTH},
	{ "Particle height",	ID_GFXOP_PHYSPAR_PE_PARHEIGHT},
	{ "MinScale",			ID_GFXOP_PHYSPAR_PE_MINSCALE},
	{ "MaxScale",			ID_GFXOP_PHYSPAR_PE_MAXSCALE},
	{ "MinRotation",		ID_GFXOP_PHYSPAR_PE_MINROT},
	{ "MaxRotation",		ID_GFXOP_PHYSPAR_PE_MAXROT},
	{ "MinColor",			ID_GFXOP_PHYSPAR_PE_MINCOLOR},
	{ "MaxColor",			ID_GFXOP_PHYSPAR_PE_MAXCOLOR},
	{ "ParticleOrient",		ID_GFXOP_PHYSPAR_PE_PARORIENT},
	{ "Stop When Fade",		ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_physpointemitter_info[] = 
{
	// Base part
	{ "PointSize",			ID_GFXOP_PHYSPAR_PPE_POINTSIZE},
	{ "ScaleA",				ID_GFXOP_PHYSPAR_PPE_SCALEA},
	{ "ScaleB",				ID_GFXOP_PHYSPAR_PPE_SCALEB},
	{ "ScaleC",				ID_GFXOP_PHYSPAR_PPE_SCALEC},
	// particle prop part
	{ "IsFade",				ID_GFXOP_PHYSPAR_PPE_ISFADE},
	{ "FadeTime",			ID_GFXOP_PHYSPAR_PPE_FADETIME},
	{_prop_base_info[0].m_szName,	_prop_base_info[0].m_OpId},
	{_prop_base_info[3].m_szName,	_prop_base_info[3].m_OpId},
	{_prop_base_info[13].m_szName,	_prop_base_info[13].m_OpId},
	{_prop_base_info[16].m_szName,	_prop_base_info[16].m_OpId},
	{_prop_base_info[18].m_szName,	_prop_base_info[18].m_OpId},
	{_prop_base_info[19].m_szName,	_prop_base_info[19].m_OpId},
};

static const GFX_PROP_INFO _prop_parab_info[] =
{
	{ "Paraboloid coeff",		ID_GFXOP_PARAB_COEFF },
	{ "Decal height",			ID_GFXOP_DECAL_HEIGHT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_trail_info[] =
{
	{ "Origin pos1",	ID_GFXOP_TRAIL_ORGPOS1 },
	{ "Origin pos2",	ID_GFXOP_TRAIL_ORGPOS2 },
	{ "Segment life",	ID_GFXOP_TRAIL_SEGLIFE },
	{ "Is bind",		ID_GFXOP_TRAIL_BIND },
	{ "Is Spline Mode",	ID_GFXOP_TRAIL_SPLINEMODE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_trailex_info[] =
{
    { "Origin pos1",	ID_GFXOP_TRAIL_ORGPOS1 },
    { "Origin pos2",	ID_GFXOP_TRAIL_ORGPOS2 },
    { "Segment life",	ID_GFXOP_TRAIL_SEGLIFE },
    { "Is bind",		ID_GFXOP_TRAIL_BIND },
    { "Shrink time",    ID_GFXOP_TRAIL_SHRINK },
    GFX_BASE_INFO,
    GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ltntrail_info[] =
{
	{ "Origin pos1",	ID_GFXOP_LTNTRAIL_POS1 },
	{ "Origin pos2",	ID_GFXOP_LTNTRAIL_POS2 },
	{ "Segment life",	ID_GFXOP_LTNTRAIL_SEGLIFE },
	{ "Min Amplitude",	ID_GFXOP_LTNTRAIL_MIN_AMP },
	{ "Max Amplitude",	ID_GFXOP_LTNTRAIL_MAX_AMP },
	{ "Is bind",		ID_GFXOP_LTNTRAIL_BIND },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _emitter_base_info[] =
{
	{ "Emission rate",			ID_GFXOP_EMITTER_RATE },
	{ "Angle",					ID_GFXOP_EMITTER_ANGLE },
	{ "Emitter Speed",			ID_GFXOP_EMITTER_SPEED },
	{ "Particle acceleration",	ID_GFXOP_EMITTER_PAR_ACC },
	{ "Acceleration dir",		ID_GFXOP_EMITTER_ACC_DIR },
	{ "Emitter acceleration",	ID_GFXOP_EMITTER_ACC },
	{ "Particle direction",		ID_GFXOP_EMITTER_PARTICAL_INITDIR},
	{ "Particle TTL",			ID_GFXOP_EMITTER_TTL },
	{ "Particle color Min",		ID_GFXOP_EMITTER_COLOR_MIN },
	{ "Particle color Max",		ID_GFXOP_EMITTER_COLOR_MAX },
	{ "Particle Scale Min",		ID_GFXOP_EMITTER_SCALE_MIN },
	{ "Particle Scale Max",		ID_GFXOP_EMITTER_SCALE_MAX },
	{ "Rotation Min",		ID_GFXOP_EMITTER_ROT_MIN },
	{ "Rotation Max",		ID_GFXOP_EMITTER_ROT_MAX },
	{ "Emit on surface",	ID_GFXOP_EMITTER_SURFACE },
	{ "Particle is binded",	ID_GFXOP_EMITTER_BIND },
	{ "Particle is draged",	ID_GFXOP_EMITTER_DRAG },
	{ "Drag power",			ID_GFXOP_EMITTER_DRAG_POWER	},
	{ "Use HSV interpolation", ID_GFXOP_EMITTER_COLOR_HSVINTERP},
};

static const GFX_PROP_INFO _prop_parsys_base_info[] =
{
	{ "Particle quota",		ID_GFXOP_PARSYS_PAR_QUOTA },
	{ "Particle width",		ID_GFXOP_PARSYS_PAR_WIDTH },
	{ "Particle height",	ID_GFXOP_PARSYS_PAR_HEIGHT },
	{ "3D Particle",		ID_GFXOP_PARSYS_PAR_3D },
	{ "Facing Camera",		ID_GFXOP_PARSYS_PAR_FACING },
	{ "Z Offset",			ID_GFXOP_PARSYS_Z_OFFSET },
	{ "Scale no offset",	ID_GFXOP_PARSYS_SCALE_NO_OFFSET },
	{ "No width scale",		ID_GFXOP_PARSYS_NO_WID_SCALE },
	{ "No height scale",	ID_GFXOP_PARSYS_NO_HEI_SCALE },
	{ "Origin point width",	ID_GFXOP_PARSYS_ORG_PT_WID },
	{ "Origin point height",ID_GFXOP_PARSYS_ORG_PT_HEI },
	{ "Is Use Particle UV", ID_GFXOP_PARSYS_USE_PAR_UV },
	{ "Is Particle Init Random", ID_GFXOP_PARSYS_INIT_RANDOM },
	{ "Par starts at ground", ID_GFXOP_PARSYS_USE_GRND_HEIGHT },
	{ "Stop emit when fade", ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE },
};

static const GFX_PROP_INFO _prop_pointpar_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_areapar_info[] =
{
	{ "Area size",		ID_GFXOP_EMITTER_AREA_SIZE }
};

static const GFX_PROP_INFO _prop_boxpar_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ellipsoidpar_info[] = 
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "Average generate",	ID_GFXOP_EMITTER_ELLIPSOID_ISAVG },
	{ "Alpha segments",		ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG },
	{ "Beta segments",		ID_GFXOP_EMITTER_ELLIPSOID_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_cylinder_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "Average generate",	ID_GFXOP_EMITTER_CYLINDER_ISAVG },
	{ "Alpha segments",		ID_GFXOP_EMITTER_CYLINDER_ALPHASEG },
	{ "Beta segments",		ID_GFXOP_EMITTER_CYLINDER_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_curve_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "Curve radius",		ID_GFXOP_EMITTER_CURVE_R },
	{ "Average generate",	ID_GFXOP_EMITTER_CURVE_ISAVG},
	{ "Average segments",	ID_GFXOP_EMITTER_CURVE_AVGSEGNUM},
	{ "Average gen sequentially", ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_multiplane_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "Reference model mox",		ID_GFXOP_EMITTER_REF_MOX },
	{ "Reference model smd",		ID_GFXOP_EMITTER_REF_SMD },
	{ "smd model skin index",		ID_GFXOP_EMITTER_SMD_SKIN_INDEX},
	{ "smd model skin mesh index",	ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX},
	{ "Use normal",		ID_GFXOP_EMITTER_USE_NORMAL },
	{ "Perpendicular to normal",		ID_GFXOP_EMITTER_PERP_TO_NORMAL },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_light_info[] =
{
	{ "Light type",		ID_GFXOP_LIGHT_TYPE },
	{ "Ambient",		ID_GFXOP_LIGHT_AMBIENT },
//	{ "Diffuse",		ID_GFXOP_LIGHT_DIFFUSE },
//	{ "Specular",		ID_GFXOP_LIGHT_SPECULAR },
	{ "Position",		ID_GFXOP_LIGHT_POSITION },
//	{ "Direction",		ID_GFXOP_LIGHT_DIRECTION },
	{ "Range",			ID_GFXOP_LIGHT_RANGE },
	{ "Attenuation0",	ID_GFXOP_LIGHT_ATTENUATION0	},
	{ "Attenuation1",	ID_GFXOP_LIGHT_ATTENUATION1	},
	{ "Attenuation2",	ID_GFXOP_LIGHT_ATTENUATION2 },
	{ "Inner Use",		ID_GFXOP_INNER_USE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ring_info[] =
{
	{ "Ring radius",		ID_GFXOP_RING_RADIUS },
	{ "Ring height",		ID_GFXOP_RING_HEIGHT },
	{ "Pitch",				ID_GFXOP_RING_PITCH },
	{ "Segment Number",		ID_GFXOP_RING_SECTS },
	{ "No radius scale",	ID_GFXOP_RING_NORADSCALE },
	{ "No height scale",	ID_GFXOP_RING_NOHEISCALE },
	{ "Origin at center",	ID_GFXOP_RING_ORGATCENTER },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ln_info[] =
{
	{ "Start position",		ID_GFXOP_LN_START_POS },
	{ "End position",		ID_GFXOP_LN_END_POS },
	{ "Segment Num",		ID_GFXOP_LN_SEGS },
	{ "Lightning Num",		ID_GFXOP_LN_NUM },
	{ "Wave length",		ID_GFXOP_LN_WAVELEN },
	{ "Interval",			ID_GFXOP_LN_INTERVAL },
	{ "Start width",		ID_GFXOP_LN_WIDTH_START },
	{ "End width",			ID_GFXOP_LN_WIDTH_END },
	{ "Mid width",			ID_GFXOP_LN_WIDTH_MID },
	{ "Start Alpha",		ID_GFXOP_LN_ALPHA_START },
	{ "End Alpha",			ID_GFXOP_LN_ALPHA_END },
	{ "Mid Alpha",			ID_GFXOP_LN_ALPHA_MID },
	{ "Use normal",			ID_GFXOP_LN_USE_NORMAL },
	{ "Self normal",		ID_GFXOP_LN_NORMAL },
	{ "Amplitude",			ID_GFXOP_LN_AMPLITUDE },
	{ _ctrl_noise_base[3].m_szName, _ctrl_noise_base[3].m_OpId },
	{ "Filter type",		ID_GFXOP_LN_FILTER },
	{ "Is Wave Moving",		ID_GFXOP_LN_WAVEMOVE },
	{ "Wave Moving Speed",	ID_GFXOP_LN_WAVEMOVESPEED },
	{ "Fix Wave Length",	ID_GFXOP_LN_FIXWAVELENGTH },
	{ "Num Waves",			ID_GFXOP_LN_NUMWAVES },
	
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

// Derived from lightning
// its properties should contain all lightning's properties
static const GFX_PROP_INFO _prop_ln_ex_info[] = 
{
	{ _prop_ln_info[0].m_szName, _prop_ln_info[0].m_OpId },
	{ _prop_ln_info[1].m_szName, _prop_ln_info[1].m_OpId },
	{ _prop_ln_info[2].m_szName, _prop_ln_info[2].m_OpId },
	{ _prop_ln_info[3].m_szName, _prop_ln_info[3].m_OpId },
	{ _prop_ln_info[4].m_szName, _prop_ln_info[4].m_OpId },
	{ _prop_ln_info[5].m_szName, _prop_ln_info[5].m_OpId },
	{ _prop_ln_info[6].m_szName, _prop_ln_info[6].m_OpId },
	{ _prop_ln_info[7].m_szName, _prop_ln_info[7].m_OpId },
	{ _prop_ln_info[8].m_szName, _prop_ln_info[8].m_OpId },
	{ _prop_ln_info[9].m_szName, _prop_ln_info[9].m_OpId },
	{ _prop_ln_info[10].m_szName, _prop_ln_info[10].m_OpId },
	{ _prop_ln_info[11].m_szName, _prop_ln_info[11].m_OpId },
	{ _prop_ln_info[12].m_szName, _prop_ln_info[12].m_OpId },
	{ _prop_ln_info[13].m_szName, _prop_ln_info[13].m_OpId },
	{ _prop_ln_info[14].m_szName, _prop_ln_info[14].m_OpId },
	{ _prop_ln_info[15].m_szName, _prop_ln_info[15].m_OpId },
	{ _prop_ln_info[16].m_szName, _prop_ln_info[16].m_OpId },
	{ "Render side",		ID_GFXOP_LN_EX_RENDERSIDE	},
	{ "Is appendly",		ID_GFXOP_LN_EX_ISAPPENDLY	},
	{ "Is use verts life",	ID_GFXOP_LN_EX_ISUSEVERTSLIFE },
	{ "Verts life",			ID_GFXOP_LN_EX_VERTSLIFE	},
	{ "Tail fadeout",		ID_GFXOP_LN_EX_ISTAILFADEOUT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_bolt_info[] =
{
	{ "Deviation angle",	ID_GFXOP_BOLT_DEVIATION },
	{ "Bolt steps",			ID_GFXOP_BOLT_STEPS },
	{ "Bolt branches",		ID_GFXOP_BOLT_BRANCHES },
	{ "Min step",			ID_GFXOP_BOLT_STEP_MIN },
	{ "Max step",			ID_GFXOP_BOLT_STEP_MAX },
	{ "Start width",		ID_GFXOP_BOLT_WIDTH_START },
	{ "End width",			ID_GFXOP_BOLT_WIDTH_END },
	{ "Start Alpha",		ID_GFXOP_BOLT_ALPHA_START },
	{ "End Alpha",			ID_GFXOP_BOLT_ALPHA_END },
	{ "Bolt amplitude",		ID_GFXOP_BOLT_AMP },
	{ "Interval",			ID_GFXOP_BOLT_INTERVAL },
	{ "Bolts half circle",	ID_GFXOP_BOLT_PER_BOLTS },
	{ "Circles",			ID_GFXOP_BOLT_CIRCLES },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_model_info[] =
{
	{ "Model path",		ID_GFXOP_MODEL_PATH },
	{ "Model action",	ID_GFXOP_MODEL_ACT_NAME },
	{ "Loops",			ID_GFXOP_MODEL_LOOPS },
	{ "Alpha Compare",	ID_GFXOP_MODEL_ALPHA_CMP },
	{ "Cover others",	ID_GFXOP_MODEL_WRITE_Z },
//	{ "Use 3d camera in 2D render," ID_GFXOP_MODEL_USE_3DCAMERA },
	{ "Face Moving dir"	ID_GFXOP_MODEL_FACE_DIR },
	{ _prop_base_info[0].m_szName, _prop_base_info[0].m_OpId },
	{ _prop_base_info[2].m_szName, _prop_base_info[2].m_OpId },
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },
	{ _prop_base_info[14].m_szName, _prop_base_info[14].m_OpId },
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ecmodel_info[] =
{
	{ "ECModel path",		ID_GFXOP_ECMODEL_PATH },
	{ "ECModel action",		ID_GFXOP_ECMODEL_ACT_NAME },
	//{ "Is Use Player Skin",	ID_GFXOP_ECMODEL_USECASTERSKIN },
	{ "ECM Loader Type",	ID_GFXOP_ECMODEL_LDTYPE },
	{ "ECM User Command",	ID_GFXOP_ECMODEL_USERCMD },
	{ _prop_base_info[0].m_szName, _prop_base_info[0].m_OpId },
	{ _prop_base_info[2].m_szName, _prop_base_info[2].m_OpId },
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },
	{ _prop_base_info[14].m_szName, _prop_base_info[14].m_OpId },
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_container_info[] =
{
	{ "GFX path",		ID_GFXOP_CONTAINER_GFX_PATH },
	{ "Outer color",	ID_GFXOP_CONTAINER_OUT_COLOR },
	{ "Loop replay",	ID_GFXOP_CONTAINER_LOOP_FLAG },
	{ "GFX playspeed",	ID_GFXOP_CONTAINER_GFX_PLAYSPEED },
	{ "Dummy use GFX scale", ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_sound_info[] =
{
	{ "Force 2D",		ID_GFXOP_SOUND_FORCE2D },
	{ "Loop",			ID_GFXOP_SOUND_LOOP },
//	{ "Volume",			ID_GFXOP_SOUND_VOLUME },
	{ "Min dist",		ID_GFXOP_SOUND_MIN_DIST },
	{ "Max dist",		ID_GFXOP_SOUND_MAX_DIST },
	{ "Min volume",		ID_GFXOP_SOUND_VOLUME_MIN },
	{ "Max volume",		ID_GFXOP_SOUND_VOLUME_MAX },
	{ "Min pitch",		ID_GFXOP_SOUND_PITCH_MIN },
	{ "Max pitch",		ID_GFXOP_SOUND_PITCH_MAX },
	{ "Sound file",		ID_GFXOP_SOUND_FILE },
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO	_prop_ribbon_info[] = 
{
	{ "Org Pos1",	ID_GFXOP_RIBBON_ORGPOS1 },
	{ "Org Pos2",	ID_GFXOP_RIBBON_ORGPOS2 },
	{ "Seg life",	ID_GFXOP_RIBBON_SEGLIFE },
	{ "Bind",	ID_GFXOP_RIBBON_BIND },
	{ "Time to gravity", ID_GFXOP_RIBBON_TIME_TO_GRAVITY},
	{ "Velocity to gravity", ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY},
	{ "Gravity", ID_GFXOP_RIBBON_GRAVITY},
	{ "Vert noise amplitude", ID_GFXOP_RIBBON_VERTICAL_NOISE},
	{ "Vert noise speed", ID_GFXOP_RIBBON_VERTICAL_SPEED},
	{ "Horz noise amplitude", ID_GFXOP_RIBBON_HORZ_AMPLITUDE},
	{ "Horz noise speed", ID_GFXOP_RIBBON_HORZ_SPEED},
	{ "X noise", ID_GFXOP_RIBBON_X_NOISE},
	{ "Z noise", ID_GFXOP_RIBBON_Z_NOISE},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _ctrl_base[] =
{
	{ "Start time",		ID_KPCTRL_BASE_START_TM },
	{ "End time",		ID_KPCTRL_BASE_END_TM }
};

static const GFX_PROP_INFO _ctrl_move[] =
{
	CTRL_BASE_INFO
	{ "Move direction",		ID_KPCTRL_MOVE_DIR },
	{ "Move velocity",		ID_KPCTRL_MOVE_VEL },
	{ "Move acceleration",	ID_KPCTRL_MOVE_ACC }
};

static const GFX_PROP_INFO _ctrl_rot[] =
{
	CTRL_BASE_INFO
	{ "Angular vel",	ID_KPCTRL_ROT_VEL },
	{ "Angular accel",	ID_KPCTRL_ROT_ACC }
};

static const GFX_PROP_INFO _ctrl_rot_axis[] =
{
	CTRL_BASE_INFO
	{ "Axis pos",			ID_KPCTRL_ROTAXIS_POS },
	{ "Axis dir",			ID_KPCTRL_ROTAXIS_DIR },
	{ "Angular vel",		ID_KPCTRL_ROTAXIS_VEL },
	{ "Angular accel",		ID_KPCTRL_ROTAXIS_ACC }
};

static const GFX_PROP_INFO _ctrl_revol[] =
{
	CTRL_BASE_INFO
	{ "Axis pos",			ID_KPCTRL_REVOL_POS },
	{ "Axis dir",			ID_KPCTRL_REVOL_DIR },
	{ "Angular vel",		ID_KPCTRL_REVOL_VEL },
	{ "Angular accel",	ID_KPCTRL_REVOL_ACC }
};

static const GFX_PROP_INFO _ctrl_centri_move[] =
{
	CTRL_BASE_INFO
	{ "Center pos",					ID_KPCTRL_CENTRI_POS },
	{ "Centrifugal speed",			ID_KPCTRL_CENTRI_VEL },
	{ "Centrifugal acceleration",	ID_CPCTRL_CENTRI_ACC }
};

static const GFX_PROP_INFO _ctrl_color[] =
{
	CTRL_BASE_INFO
	{ "R change",		ID_KPCTRL_COLOR_R },
	{ "G change",		ID_KPCTRL_COLOR_G },
	{ "B change",		ID_KPCTRL_COLOR_B },
	{ "A change",		ID_KPCTRL_COLOR_A }
};

static const GFX_PROP_INFO _ctrl_cltrans[] =
{
	CTRL_BASE_INFO
	{ "Alpha Only",		ID_KPCTLR_CLTRAN_ALPHAONLY},
	{ "Org color",		ID_KPCTRL_CLTRAN_ORG },
	{ "Dest color num",	ID_KPCTRL_CLTRAN_NUM },
	{ "Dest color1",		ID_KPCTRL_CLTRAN_DEST1 },
	{ "Trans interval1",	ID_KPCTRL_CLTRAN_TIME1 },
	{ "Dest color2",		ID_KPCTRL_CLTRAN_DEST2 },
	{ "Trans interval2",	ID_KPCTRL_CLTRAN_TIME2 },
	{ "Dest color3",		ID_KPCTRL_CLTRAN_DEST3 },
	{ "Trans interval3",	ID_KPCTRL_CLTRAN_TIME3 },
	{ "Dest color4",		ID_KPCTRL_CLTRAN_DEST4 },
	{ "Trans interval4",	ID_KPCTRL_CLTRAN_TIME4 },
	{ "Dest color5",		ID_KPCTRL_CLTRAN_DEST5 },
	{ "Trans interval5",	ID_KPCTRL_CLTRAN_TIME5 },
	{ "Dest color6",		ID_KPCTRL_CLTRAN_DEST6 },
	{ "Trans interval6",	ID_KPCTRL_CLTRAN_TIME6 },
	{ "Dest color7",		ID_KPCTRL_CLTRAN_DEST7 },
	{ "Trans interval7",	ID_KPCTRL_CLTRAN_TIME7 },
	{ "Dest color8",		ID_KPCTRL_CLTRAN_DEST8 },
	{ "Trans interval8",	ID_KPCTRL_CLTRAN_TIME8 },
	{ "Dest color9",		ID_KPCTRL_CLTRAN_DEST9 },
	{ "Trans interval9",	ID_KPCTRL_CLTRAN_TIME9 },
	{ "Dest color10",		ID_KPCTRL_CLTRAN_DEST10 },
	{ "Trans interval10",	ID_KPCTRL_CLTRAN_TIME10 },
};

static const GFX_PROP_INFO _ctrl_scltrans[] =
{
	CTRL_BASE_INFO
	{ "Org scale",		ID_KPCTRL_SCLTRAN_ORG },
	{ "Dest scale num",	ID_KPCTRL_SCLTRAN_NUM },
	{ "Dest scale1",		ID_KPCTRL_SCLTRAN_DEST1 },
	{ "Trans interval1",	ID_KPCTRL_SCLTRAN_TIME1 },
	{ "Dest scale2",		ID_KPCTRL_SCLTRAN_DEST2 },
	{ "Trans interval2",	ID_KPCTRL_SCLTRAN_TIME2 },
	{ "Dest scale3",		ID_KPCTRL_SCLTRAN_DEST3 },
	{ "Trans interval3",	ID_KPCTRL_SCLTRAN_TIME3 },
	{ "Dest scale4",		ID_KPCTRL_SCLTRAN_DEST4 },
	{ "Trans interval4",	ID_KPCTRL_SCLTRAN_TIME4 },
	{ "Dest scale5",		ID_KPCTRL_SCLTRAN_DEST5 },
	{ "Trans interval5",	ID_KPCTRL_SCLTRAN_TIME5 },
};

static const GFX_PROP_INFO _ctrl_scale[] =
{
	CTRL_BASE_INFO
	{ "Scale delta",		ID_KPCTRL_SCALE_DELTA },
	{ "Min scale",		ID_KPCTRL_SCALE_MIN },
	{ "Max scale",		ID_KPCTRL_SCALE_MAX }
};

static const GFX_PROP_INFO _ctrl_noise_base[] =
{
	{ "Buffer len",		ID_KPCTRL_NOISE_BUFLEN },
	{ "Amplitude",		ID_KPCTRL_NOISE_AMP },
	{ "Wave length",	ID_KPCTRL_NOISE_WAVLEN },
	{ "Persistence",	ID_KPCTRL_NOISE_PERSIST	},
	{ "Sub wave num",	ID_KPCTRL_NOISE_OCTNUM }
};

static const GFX_PROP_INFO _ctrl_color_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId },
	{ "Base color",		ID_KPCTRL_NOISE_BASE_COLOR }
};

static const GFX_PROP_INFO _ctrl_scale_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId }
};

static const GFX_PROP_INFO _param_trail[] =
{
	{ "Origin pos1",		ID_PARAM_TRAIL_POS1 },
	{ "Origin pos2",		ID_PARAM_TRAIL_POS2	},
	{ "Transform matrix",	ID_PARAM_TRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_trailex[] =
{
    { "Origin pos1",		ID_PARAM_TRAIL_POS1 },
    { "Origin pos2",		ID_PARAM_TRAIL_POS2	},
    { "Transform matrix",	ID_PARAM_TRAIL_MATRIX }
};


static const GFX_PROP_INFO _param_ltntrail[] =
{
	{ "Origin pos1",	ID_PARAM_LTNTRAIL_POS1 },
	{ "Origin pos2",	ID_PARAM_LTNTRAIL_POS2	},
	{ "Transform matrix",		ID_PARAM_LTNTRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_ltn[] =
{
	{ "Start pos",		ID_PARAM_LTN_POS1 },
	{ "End pos",		ID_PARAM_LTN_POS2 }
};

static const GFX_PROP_INFO _param_ribbon[] =
{
	{ "Origin pos1",	ID_PARAM_RIBBON_POS1 },
	{ "Origin pos2",	ID_PARAM_RIBBON_POS2	},
	{ "Transform matrix",		ID_PARAM_RIBBON_MATRIX }
};

static const GFX_OBJ_INFO _gfx_method_info[] =
{
	{ "Rotation",			ID_KPCTRL_ROT,			_ctrl_rot,			PROP_COUNT(_ctrl_rot),			NULL,	NULL,	0 },
	{ "Rotation around axis",	ID_KPCTRL_ROTAXIS,		_ctrl_rot_axis,		PROP_COUNT(_ctrl_rot_axis),		NULL,	NULL,	0 },
	{ "Revolution around axis",	ID_KPCTRL_REVOL,		_ctrl_revol,		PROP_COUNT(_ctrl_revol),		NULL,	NULL,	0 },
	{ "Centrifugal Move",	ID_KPCTRL_CENTRI,		_ctrl_centri_move,	PROP_COUNT(_ctrl_centri_move),	NULL,	NULL,	0 },
	{ "Color change",		ID_KPCTRL_COLOR,		_ctrl_color,		PROP_COUNT(_ctrl_color),		NULL,	NULL,	0 },
	{ "Color transition",	ID_KPCTRL_CL_TRANS,		_ctrl_cltrans,		PROP_COUNT(_ctrl_cltrans),		NULL,	NULL,	0 },
	{ "Color noise",		ID_KPCTRL_CL_NOISE,		_ctrl_color_noise,	PROP_COUNT(_ctrl_color_noise),	NULL,	NULL,	0 },
	{ "Scale change",		ID_KPCTRL_SCALE,		_ctrl_scale,		PROP_COUNT(_ctrl_scale),		NULL,	NULL,	0 },
	{ "Scale transition",	ID_KPCTRL_SCALE_TRANS,	_ctrl_scltrans,		PROP_COUNT(_ctrl_scltrans),		NULL,	NULL,	0 },
	{ "Scale noise",		ID_KPCTRL_SCA_NOISE,	_ctrl_scale_noise,	PROP_COUNT(_ctrl_scale_noise),	NULL,	NULL,	0 },
	{ "Translation",		ID_KPCTRL_MOVE,			_ctrl_move,			PROP_COUNT(_ctrl_move),			NULL,	NULL,	0 }
};

static const GFX_OBJ_INFO _gfx_obj_info[] =
{
	{ "3D decal",			ID_ELE_TYPE_DECAL_3D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "2D decal",			ID_ELE_TYPE_DECAL_2D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "Cross decal",			ID_ELE_TYPE_DECAL_BILLBOARD,	_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "Paraboloid",			ID_ELE_TYPE_PARABOLOID,			_prop_parab_info,		PROP_COUNT(_prop_parab_info),		NULL,	NULL,	0 },
	{ "Trail",				ID_ELE_TYPE_TRAIL,				_prop_trail_info,		PROP_COUNT(_prop_trail_info),		NULL,	NULL,	0 },
    { "New Trail",			ID_ELE_TYPE_TRAIL_EX,			_prop_trailex_info,		PROP_COUNT(_prop_trailex_info),		NULL,	NULL,	0 },
	{ "LightningTrail",		ID_ELE_TYPE_LTNTRAIL,			_prop_ltntrail_info,	PROP_COUNT(_prop_ltntrail_info),	NULL,	NULL,	0 },
	{ "Par Point",			ID_ELE_TYPE_PARTICLE_POINT,		_prop_pointpar_info,	PROP_COUNT(_prop_pointpar_info),	"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Par Box",			ID_ELE_TYPE_PARTICLE_BOX,		_prop_boxpar_info,		PROP_COUNT(_prop_boxpar_info),		"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Par Multiplane",		ID_ELE_TYPE_PARTICLE_MULTIPLANE,_prop_multiplane_info,	PROP_COUNT(_prop_multiplane_info),	"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Par Ellipsoid",		ID_ELE_TYPE_PARTICLE_ELLIPSOID,	_prop_ellipsoidpar_info,	PROP_COUNT(_prop_ellipsoidpar_info),		"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Par Cylinder",		ID_ELE_TYPE_PARTICLE_CYLINDER,	_prop_cylinder_info,	PROP_COUNT(_prop_cylinder_info),	"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Par Curve",			ID_ELE_TYPE_PARTICLE_CURVE,		_prop_curve_info,		PROP_COUNT(_prop_curve_info),		"Particle effect affector prop",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Light",				ID_ELE_TYPE_LIGHT,				_prop_light_info,		PROP_COUNT(_prop_light_info),		NULL,	NULL,	0 },
	{ "Ring",				ID_ELE_TYPE_RING,				_prop_ring_info,		PROP_COUNT(_prop_ring_info),		NULL,	NULL,	0 },
	{ "Lightning",			ID_ELE_TYPE_LIGHTNING,			_prop_ln_info,			PROP_COUNT(_prop_ln_info),			NULL,	NULL,	0 },
	{ "Lightning bolt",		ID_ELE_TYPE_LTNBOLT,			_prop_bolt_info,		PROP_COUNT(_prop_bolt_info),		NULL,	NULL,	0 },
	{ "Model",				ID_ELE_TYPE_MODEL,				_prop_model_info,		PROP_COUNT(_prop_model_info),		NULL,	NULL,	0 },
	{ "Sound",				ID_ELE_TYPE_SOUND,				_prop_sound_info,		PROP_COUNT(_prop_sound_info),		NULL,	NULL,	0 },
	{ "SubGFX",				ID_ELE_TYPE_GFX_CONTAINER,		_prop_container_info,	PROP_COUNT(_prop_container_info),	NULL,	NULL,	0 },
	{ "Grid3D decal",		ID_ELE_TYPE_GRID_DECAL_3D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "Grid2D decal",		ID_ELE_TYPE_GRID_DECAL_2D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "PhysPar",			ID_ELE_TYPE_PHYS_EMITTER,		_prop_physemitter_info,	PROP_COUNT(_prop_physemitter_info),	"Phys Particle effect affector prop",	NULL,	0 },
	{ "PhysPar point",		ID_ELE_TYPE_PHYS_POINTEMITTER,	_prop_physpointemitter_info, PROP_COUNT(_prop_physpointemitter_info), NULL, NULL, 0},
	{ "LightningEx",		ID_ELE_TYPE_LIGHTNINGEX,		_prop_ln_ex_info,		PROP_COUNT(_prop_ln_ex_info),		NULL,	NULL,	0},
	{ "ECModel",			ID_ELE_TYPE_ECMODEL,			_prop_ecmodel_info,		PROP_COUNT(_prop_ecmodel_info),		NULL,	NULL,	0},
	{ "Ribbon",			ID_ELE_TYPE_RIBBON,				_prop_ribbon_info,	PROP_COUNT(_prop_ribbon_info),			NULL,	NULL,	0},
};

static const GFX_PARAM_INFO _gfx_param_info[] =
{
	{ ID_ELE_TYPE_TRAIL,		_param_trail,	PROP_COUNT(_param_trail) },
	{ ID_ELE_TYPE_LIGHTNING,	_param_ltn,		PROP_COUNT(_param_ltn) },
	{ ID_ELE_TYPE_LTNTRAIL,		_param_ltntrail,PROP_COUNT(_param_ltntrail) },
	{ ID_ELE_TYPE_RIBBON,		_param_ribbon, PROP_COUNT(_param_ribbon) }
    { ID_ELE_TYPE_TRAIL_EX,		_param_trailex,	PROP_COUNT(_param_trailex) },
};

static const GFX_INTERP_INFO _gfx_interp_info[] =
{
	{ "None",		A3DGFXKeyPoint::enumInterpNone },
	{ "Interp All",		A3DGFXKeyPoint::enumInterpAll },
	{ "Interp Not on dir",	A3DGFXKeyPoint::enumInterpNoDir }
};

static const GFX_PROP_INFO _prop_kp_info[] =
{
	{ "Center pos",		ID_GFXOP_KP_POS},
	{ "Normal direction",		ID_GFXOP_KP_DIR},
	{ "Color",			ID_GFXOP_KP_COLOR},
	{ "Scale",			ID_GFXOP_KP_SCALE},
	{ "2D Rotation",		ID_GFXOP_KP_RAD2D},
	{ "Infinite",		ID_GFXOP_KP_INFINITE},
	{ "InterpMode",		ID_GFXOP_KP_INTERP},
	{ "Timespan",		ID_GFXOP_KP_TIMESPAN},
};

#else

// Not English Version
// Currently Chinese
static const GFX_SHADER_INFO _gfx_shader_info[] =
{
	{ "Alpha亮度模式",	ID_SHADER_NORMAL,		A3DBLEND_SRCALPHA,	A3DBLEND_INVSRCALPHA },
	{ "高亮模式",		ID_SHADER_HILIGHT,		A3DBLEND_SRCALPHA,	A3DBLEND_ONE },
	{ "超亮模式",		ID_SHADER_SUPER_HILIGHT,A3DBLEND_ONE,		A3DBLEND_ONE },
	{ "颜色亮度模式",	ID_SHADER_COLOR,		A3DBLEND_ONE,		A3DBLEND_INVSRCCOLOR }
};

/* !!!!!IMPORTANT new prop must be added at tail */
/* !!!!!需要在尾部更新是因为其他引用该属性的属性项可能直接依赖于属性的索引值 */
/* !!!!!增加新项时，注意顶部对应的宏需要更新	*/
static const GFX_PROP_INFO _prop_base_info[] =
{
	{ "Alpha模式",		ID_GFXOP_ALPHA_MODE },				// 0
	{ "是否能被遮挡",	ID_GFXOP_ZTESTENABLE },				// 1
	{ "运用地面法向",	ID_GFXOP_GROUND_NORMAL },			// 2
	{ "贴图文件路径",	ID_GFXOP_TEX_PATH },				// 3
	{ "平铺模式",		ID_GFXOP_TILE_MODE },				// 4
	{ "U坐标平移速度",	ID_GFXOP_U_SPEED },					// 5
	{ "V坐标平移速度",	ID_GFXOP_V_SPEED },					// 6
	{ "UV坐标互换",		ID_GFXOP_UV_INTERCHANGE },			// 7
	{ "U坐标翻转",		ID_GFXOP_U_REVERSE },				// 8
	{ "V坐标翻转",		ID_GFXOP_V_REVERSE },				// 9
	{ "动画行数",		ID_GFXOP_TEX_ROWS },				// 10
	{ "动画列数",		ID_GFXOP_TEX_COLS },				// 11
	{ "动画间隔时间",	ID_GFXOP_TEX_INTERVAL },			// 12
	{ "显示优先级",		ID_GFXOP_PRIORITY },				// 13
	{ "是否傀儡",		ID_GFXOP_IS_DUMMY },				// 14
	{ "产生空间扭曲",	ID_GFXOP_WARP },					// 15
	{ "渲染层次",		ID_GFXOP_RENDER_LAYER },			// 16
	{ "运用地面高度",	ID_GFXOP_GROUND_HEIGHT },			// 17
	{ "不降低贴图精度",	ID_GFXOP_TEX_NODOWNSAMPLE },		// 18
	{ "循环结束时重置状态", ID_GFXOP_RESETONLOOPEND },		// 19
	{ "纹理动画最大时间", ID_GFXOP_TEXANIMMAXTIME },		// 20
};

static const GFX_PROP_INFO _prop_repeat_info[] =
{
	{ "重复次数",		ID_GFXOP_REPEATCOUNT },
	{ "重复间隔",		ID_GFXOP_REPEATDELAY },
	{ "能否淡出",		ID_GFXOP_CAN_DO_FADE_OUT },
    { "是否软边缘",		ID_GFXOP_SOFT_EDGE },
	{ "使用内部灯光",	ID_GFXOP_USE_INNER_LIGHT },
};

static const GFX_PROP_INFO _prop_pixelshader_info[] =
{
	{ "Shader路径",		ID_GFXOP_PIXELSHADERPATH },
	{ "Shader所用贴图",	ID_GFXOP_SHADER_TEX },
};

static const GFX_PROP_INFO _prop_decal_info[] =
{
	{ "宽度",			ID_GFXOP_DECAL_WIDTH },
	{ "高度",			ID_GFXOP_DECAL_HEIGHT },
	{ "是否随视角偏转",	ID_GFXOP_DECAL_ROTFROMVIEW },
	{ "仅随地面倾斜",	ID_GFXOP_DECAL_GRNDNORM_ONLY },
	{ "紧贴地面",		ID_GFXOP_DECAL_MATCH_SURFACE },
	{ "宽度比例不变",	ID_GFXOP_DECAL_NO_WID_SCALE },
	{ "高度比例不变",	ID_GFXOP_DECAL_NO_HEI_SCALE },
	{ "原点宽度比例",	ID_GFXOP_DECAL_ORG_PT_WID },
	{ "原点高度比例",	ID_GFXOP_DECAL_ORG_PT_HEI },
	{ "距视点偏移",		ID_GFXOP_DECAL_Z_OFFSET },
	{ "运用地面法向时方向有效", ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM},
	{ "贴地时使用Gfx方向",		ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR },
	{ "2D面片使用屏幕坐标",		ID_GFXOP_DECAL_SCREEN_DIMENSION },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_griddecal_info[] = 
{
	{ "宽控制点数目",	ID_GFXOP_GRIDDECAL_W_VNUM },
	{ "高控制点数目",	ID_GFXOP_GRIDDECAL_H_VNUM },
	{ "网格间隔大小",	ID_GFXOP_GRIDDECAL_GRIDSIZE},
	{ "距视点偏移",		ID_GFXOP_GRIDDECAL_Z_OFFSET},
	{ "受比例改变影响",	ID_GFXOP_GRIDDECAL_AFFBYSCALE},
	{ "是否随视角偏转",	ID_GFXOP_GRIDDECAL_ROTFROMVIEW },
	{ "贴地时距地面高度",	ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT},
	{ "总是贴地", ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_physemitter_info[] = 
{
	{ "粒子宽度",			ID_GFXOP_PHYSPAR_PE_PARWIDTH},
	{ "粒子高度",			ID_GFXOP_PHYSPAR_PE_PARHEIGHT},	
	{ "最小比例",			ID_GFXOP_PHYSPAR_PE_MINSCALE},
	{ "最大比例",			ID_GFXOP_PHYSPAR_PE_MAXSCALE},
	{ "最小旋转角度",		ID_GFXOP_PHYSPAR_PE_MINROT},
	{ "最大旋转角度",		ID_GFXOP_PHYSPAR_PE_MAXROT},
	{ "最小颜色值",			ID_GFXOP_PHYSPAR_PE_MINCOLOR},
	{ "最大颜色值",			ID_GFXOP_PHYSPAR_PE_MAXCOLOR},
	{ "粒子朝向",			ID_GFXOP_PHYSPAR_PE_PARORIENT},
	{ "淡出时停止发射",		ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_physpointemitter_info[] = 
{
	{ "粒子大小",			ID_GFXOP_PHYSPAR_PPE_POINTSIZE},
	{ "缩放因子A",			ID_GFXOP_PHYSPAR_PPE_SCALEA},
	{ "缩放因子B",			ID_GFXOP_PHYSPAR_PPE_SCALEB},
	{ "缩放因子C",			ID_GFXOP_PHYSPAR_PPE_SCALEC},
	{ "淡入淡出",			ID_GFXOP_PHYSPAR_PPE_ISFADE},
	{ "淡入淡出时间",		ID_GFXOP_PHYSPAR_PPE_FADETIME},
	{_prop_base_info[0].m_szName,	_prop_base_info[0].m_OpId},
	{_prop_base_info[3].m_szName,	_prop_base_info[3].m_OpId},
	{_prop_base_info[13].m_szName,	_prop_base_info[13].m_OpId},
	{_prop_base_info[16].m_szName,	_prop_base_info[16].m_OpId},
	{_prop_base_info[18].m_szName,	_prop_base_info[18].m_OpId},
	{_prop_base_info[19].m_szName,	_prop_base_info[19].m_OpId},
};

static const GFX_PROP_INFO _prop_parab_info[] =
{
	{ "抛物面系数",		ID_GFXOP_PARAB_COEFF },
	{ "高度",			ID_GFXOP_DECAL_HEIGHT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_trail_info[] =
{
	{ "初始点坐标1",	ID_GFXOP_TRAIL_ORGPOS1 },
	{ "初始点坐标2",	ID_GFXOP_TRAIL_ORGPOS2 },
	{ "面片持续时间",	ID_GFXOP_TRAIL_SEGLIFE },
	{ "是否随父移动",	ID_GFXOP_TRAIL_BIND },
	{ "是否平滑模式",	ID_GFXOP_TRAIL_SPLINEMODE },
	{ "采样间隔(平滑|单位ms)",	 ID_GFXOP_TRAIL_SAMPLEFREQ },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_trailex_info[] =
{
    { "初始点坐标1",	ID_GFXOP_TRAIL_ORGPOS1 },
    { "初始点坐标2",	ID_GFXOP_TRAIL_ORGPOS2 },
    { "面片持续时间",	ID_GFXOP_TRAIL_SEGLIFE },
    { "是否随父移动",	ID_GFXOP_TRAIL_BIND },
    { "消失收尾时间",	ID_GFXOP_TRAIL_SHRINK },
    { "采样间隔(平滑|单位ms)",	 ID_GFXOP_TRAIL_SAMPLEFREQ },
    GFX_BASE_INFO,
    GFX_REPEAT_INFO,
    GFX_PIXELSHADER_INFO
};


static const GFX_PROP_INFO _prop_ltntrail_info[] =
{
	{ "初始点坐标1",	ID_GFXOP_LTNTRAIL_POS1 },
	{ "初始点坐标2",	ID_GFXOP_LTNTRAIL_POS2 },
	{ "面片持续时间",	ID_GFXOP_LTNTRAIL_SEGLIFE },
	{ "最小振幅",		ID_GFXOP_LTNTRAIL_MIN_AMP },
	{ "最大振幅",		ID_GFXOP_LTNTRAIL_MAX_AMP },
	{ "是否随父移动",	ID_GFXOP_LTNTRAIL_BIND },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _emitter_base_info[] =
{
	{ "发射数率",		ID_GFXOP_EMITTER_RATE },
	{ "发散角度",		ID_GFXOP_EMITTER_ANGLE },
	{ "粒子速度",		ID_GFXOP_EMITTER_SPEED },
	{ "粒子自身加速度",	ID_GFXOP_EMITTER_PAR_ACC },
	{ "加速度方向",		ID_GFXOP_EMITTER_ACC_DIR },
	{ "加速度大小",		ID_GFXOP_EMITTER_ACC },
	{ "粒子朝向(绕XYZ轴)",	ID_GFXOP_EMITTER_PARTICAL_INITDIR},
	{ "粒子生命",		ID_GFXOP_EMITTER_TTL },
	{ "粒子色彩最小值",	ID_GFXOP_EMITTER_COLOR_MIN },
	{ "粒子色彩最大值",	ID_GFXOP_EMITTER_COLOR_MAX },
	{ "比例最小值",		ID_GFXOP_EMITTER_SCALE_MIN },
	{ "比例最大值",		ID_GFXOP_EMITTER_SCALE_MAX },
	{ "初始角度最小值",	ID_GFXOP_EMITTER_ROT_MIN },
	{ "初始角度最大值",	ID_GFXOP_EMITTER_ROT_MAX },
	{ "仅表面产生粒子",	ID_GFXOP_EMITTER_SURFACE },
	{ "粒子随父移动",	ID_GFXOP_EMITTER_BIND },
	{ "粒子追随移动",	ID_GFXOP_EMITTER_DRAG },
	{ "追随强度",		ID_GFXOP_EMITTER_DRAG_POWER	},
	{ "颜色使用HSV插值", ID_GFXOP_EMITTER_COLOR_HSVINTERP},
};

static const GFX_PROP_INFO _prop_parsys_base_info[] =
{
	{ "粒子配额",		ID_GFXOP_PARSYS_PAR_QUOTA },
	{ "粒子宽度",		ID_GFXOP_PARSYS_PAR_WIDTH },
	{ "粒子高度",		ID_GFXOP_PARSYS_PAR_HEIGHT },
	{ "3D粒子",			ID_GFXOP_PARSYS_PAR_3D },
	{ "面向视点",		ID_GFXOP_PARSYS_PAR_FACING },
	{ "距视点偏移",		ID_GFXOP_PARSYS_Z_OFFSET },
	{ "比例缩放无位移",	ID_GFXOP_PARSYS_SCALE_NO_OFFSET },
	{ "宽度比例不变",	ID_GFXOP_PARSYS_NO_WID_SCALE },
	{ "高度比例不变",	ID_GFXOP_PARSYS_NO_HEI_SCALE },
	{ "原点宽度比例",	ID_GFXOP_PARSYS_ORG_PT_WID },
	{ "原点高度比例",	ID_GFXOP_PARSYS_ORG_PT_HEI },
	{ "粒子独立贴图序列", ID_GFXOP_PARSYS_USE_PAR_UV },
	{ "粒子产生时随机贴图序列", ID_GFXOP_PARSYS_INIT_RANDOM },
	{ "粒子从地面产生", ID_GFXOP_PARSYS_USE_GRND_HEIGHT },
	{ "淡出时停止发射", ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE },
};

static const GFX_PROP_INFO _prop_pointpar_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_areapar_info[] =
{
	{ "区域大小",		ID_GFXOP_EMITTER_AREA_SIZE }
};

static const GFX_PROP_INFO _prop_boxpar_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_ellipsoidpar_info[] = 
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "均匀产生粒子",	ID_GFXOP_EMITTER_ELLIPSOID_ISAVG },
	{ "经度分段数",		ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG },
	{ "纬度分段数",		ID_GFXOP_EMITTER_ELLIPSOID_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_cylinder_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "均匀产生粒子",	ID_GFXOP_EMITTER_CYLINDER_ISAVG },
	{ "经度分段数",		ID_GFXOP_EMITTER_CYLINDER_ALPHASEG },
	{ "纬度分段数",		ID_GFXOP_EMITTER_CYLINDER_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_curve_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "曲线半径",		ID_GFXOP_EMITTER_CURVE_R },
	{ "是否均匀产生",	ID_GFXOP_EMITTER_CURVE_ISAVG},
	{ "均匀分段数目",	ID_GFXOP_EMITTER_CURVE_AVGSEGNUM},
	{ "均匀顺序生成粒子", ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_multiplane_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "参考模型MOX",		ID_GFXOP_EMITTER_REF_MOX },
	{ "参考模型SMD",		ID_GFXOP_EMITTER_REF_SMD},
	{ "SMD模型Skin索引",	ID_GFXOP_EMITTER_SMD_SKIN_INDEX	},
	{ "SMD模型Skin Mesh索引", ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX   },
	{ "使用法向",		ID_GFXOP_EMITTER_USE_NORMAL },
	{ "垂直于法向",		ID_GFXOP_EMITTER_PERP_TO_NORMAL },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_skeleton_par_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO_NOBIND,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
};

static const GFX_PROP_INFO _prop_light_info[] =
{
	{ "灯光类型",		ID_GFXOP_LIGHT_TYPE },
	{ "环境色彩",		ID_GFXOP_LIGHT_AMBIENT },
//	{ "散射色彩",		ID_GFXOP_LIGHT_DIFFUSE },
//	{ "反射色彩",		ID_GFXOP_LIGHT_SPECULAR },
	{ "光源位置",		ID_GFXOP_LIGHT_POSITION },
//	{ "发射方向",		ID_GFXOP_LIGHT_DIRECTION },
	{ "光照范围",		ID_GFXOP_LIGHT_RANGE },
	{ "衰减常数0",		ID_GFXOP_LIGHT_ATTENUATION0	},
	{ "衰减常数1",		ID_GFXOP_LIGHT_ATTENUATION1	},
	{ "衰减常数2",		ID_GFXOP_LIGHT_ATTENUATION2 },
	{ "仅Gfx内部生效",	ID_GFXOP_INNER_USE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ring_info[] =
{
	{ "圆环半径",		ID_GFXOP_RING_RADIUS },
	{ "圆环高度",		ID_GFXOP_RING_HEIGHT },
	{ "倾斜角度",		ID_GFXOP_RING_PITCH },
	{ "圆环分段数",		ID_GFXOP_RING_SECTS },
	{ "半径比例不变",	ID_GFXOP_RING_NORADSCALE },
	{ "高度比例不变",	ID_GFXOP_RING_NOHEISCALE },
	{ "原点居中",		ID_GFXOP_RING_ORGATCENTER },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

// Notion!!!
// 给电弧增加新属性时，要对应增加高级电弧(ln_ex)的属性。
static const GFX_PROP_INFO _prop_ln_info[] =
{
	{ "起始点坐标",		ID_GFXOP_LN_START_POS },
	{ "终点坐标",		ID_GFXOP_LN_END_POS },
	{ "闪电节数",		ID_GFXOP_LN_SEGS },
	{ "闪电根数",		ID_GFXOP_LN_NUM },
	{ "闪电波长",		ID_GFXOP_LN_WAVELEN },
	{ "间隔时间",		ID_GFXOP_LN_INTERVAL },
	{ "闪电起始宽度",	ID_GFXOP_LN_WIDTH_START },
	{ "闪电终止宽度",	ID_GFXOP_LN_WIDTH_END },
	{ "闪电中间宽度",	ID_GFXOP_LN_WIDTH_MID },
	{ "闪电起始Alpha",	ID_GFXOP_LN_ALPHA_START },
	{ "闪电终止Alpha",	ID_GFXOP_LN_ALPHA_END },
	{ "闪电中间Alpha",	ID_GFXOP_LN_ALPHA_MID },
	{ "运用自身法向",	ID_GFXOP_LN_USE_NORMAL },
	{ "自身法向",		ID_GFXOP_LN_NORMAL },
	{ "可变振幅",			ID_GFXOP_LN_AMPLITUDE },
	{ _ctrl_noise_base[3].m_szName, _ctrl_noise_base[3].m_OpId },
	{ "过滤器",			ID_GFXOP_LN_FILTER },
	{ "是否波动",		ID_GFXOP_LN_WAVEMOVE },
	{ "波动速度",		ID_GFXOP_LN_WAVEMOVESPEED },
	{ "是否波动固定波长",	ID_GFXOP_LN_FIXWAVELENGTH },
	{ "波动段数",			ID_GFXOP_LN_NUMWAVES },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

// Derived from lightning
// its properties should contain all lightning's properties
static const GFX_PROP_INFO _prop_ln_ex_info[] = 
{
	{ _prop_ln_info[0].m_szName, _prop_ln_info[0].m_OpId },
	{ _prop_ln_info[1].m_szName, _prop_ln_info[1].m_OpId },
	{ _prop_ln_info[2].m_szName, _prop_ln_info[2].m_OpId },
	{ _prop_ln_info[3].m_szName, _prop_ln_info[3].m_OpId },
	{ _prop_ln_info[4].m_szName, _prop_ln_info[4].m_OpId },
	{ _prop_ln_info[5].m_szName, _prop_ln_info[5].m_OpId },
	{ _prop_ln_info[6].m_szName, _prop_ln_info[6].m_OpId },
	{ _prop_ln_info[7].m_szName, _prop_ln_info[7].m_OpId },
	{ _prop_ln_info[8].m_szName, _prop_ln_info[8].m_OpId },
	{ _prop_ln_info[9].m_szName, _prop_ln_info[9].m_OpId },
	{ _prop_ln_info[10].m_szName, _prop_ln_info[10].m_OpId },
	{ _prop_ln_info[11].m_szName, _prop_ln_info[11].m_OpId },
	{ _prop_ln_info[12].m_szName, _prop_ln_info[12].m_OpId },
	{ _prop_ln_info[13].m_szName, _prop_ln_info[13].m_OpId },
	{ _prop_ln_info[14].m_szName, _prop_ln_info[14].m_OpId },
	{ _prop_ln_info[15].m_szName, _prop_ln_info[15].m_OpId },
	{ _prop_ln_info[16].m_szName, _prop_ln_info[16].m_OpId },
	{ "渲染边侧",		ID_GFXOP_LN_EX_RENDERSIDE },
	{ "是否渐进生成",	ID_GFXOP_LN_EX_ISAPPENDLY },
	{ "是否使用顶点生命",	ID_GFXOP_LN_EX_ISUSEVERTSLIFE },
	{ "顶点生命",			ID_GFXOP_LN_EX_VERTSLIFE	},
	{ "尾部淡出",		ID_GFXOP_LN_EX_ISTAILFADEOUT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_bolt_info[] =
{
	{ "分支偏移角度",	ID_GFXOP_BOLT_DEVIATION },
	{ "闪电步数",		ID_GFXOP_BOLT_STEPS },
	{ "闪电分支数",		ID_GFXOP_BOLT_BRANCHES },
	{ "最小步长",		ID_GFXOP_BOLT_STEP_MIN },
	{ "最大步长",		ID_GFXOP_BOLT_STEP_MAX },
	{ "闪电起始宽度",	ID_GFXOP_BOLT_WIDTH_START },
	{ "闪电终止宽度",	ID_GFXOP_BOLT_WIDTH_END },
	{ "起始Alpha",		ID_GFXOP_BOLT_ALPHA_START },
	{ "终止Alpha",		ID_GFXOP_BOLT_ALPHA_END },
	{ "闪电振幅",		ID_GFXOP_BOLT_AMP },
	{ "时间间隔",		ID_GFXOP_BOLT_INTERVAL },
	{ "半圈闪电数",		ID_GFXOP_BOLT_PER_BOLTS },
	{ "圈数",			ID_GFXOP_BOLT_CIRCLES },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_model_info[] =
{
	{ "模型路径",		ID_GFXOP_MODEL_PATH },
	{ "模型动作",		ID_GFXOP_MODEL_ACT_NAME },
	{ "循环次数",		ID_GFXOP_MODEL_LOOPS },
	{ "Alpha比较",		ID_GFXOP_MODEL_ALPHA_CMP },
	{ "遮挡其它特效",	ID_GFXOP_MODEL_WRITE_Z },
//	{ "2d渲染中强制使用3D摄像机", ID_GFXOP_MODEL_USE_3DCAMERA },
	{ "模型朝向运动方向",	ID_GFXOP_MODEL_FACE_DIR },
	{ _prop_base_info[0].m_szName, _prop_base_info[0].m_OpId },
	{ _prop_base_info[1].m_szName, _prop_base_info[1].m_OpId },
	{ _prop_base_info[2].m_szName, _prop_base_info[2].m_OpId },
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },
	{ _prop_base_info[14].m_szName, _prop_base_info[14].m_OpId },
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },
	{ _prop_base_info[17].m_szName, _prop_base_info[17].m_OpId },
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_ecmodel_info[] =
{
	{ "模型路径",		ID_GFXOP_ECMODEL_PATH },
	{ "模型动作",		ID_GFXOP_ECMODEL_ACT_NAME },
	//{ "使用所属模型皮肤",	ID_GFXOP_ECMODEL_USECASTERSKIN },
	{ "模型加载类型",	ID_GFXOP_ECMODEL_LDTYPE },
	{ "用户指令",		ID_GFXOP_ECMODEL_USERCMD },
	{ _prop_base_info[0].m_szName, _prop_base_info[0].m_OpId },
	{ _prop_base_info[2].m_szName, _prop_base_info[2].m_OpId },
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },
	{ _prop_base_info[14].m_szName, _prop_base_info[14].m_OpId },
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },
	{ _prop_base_info[17].m_szName, _prop_base_info[17].m_OpId },
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_model_proxy_info[] = 
{
	{ "Alpha模式",			ID_GFXOP_ALPHA_MODE },
	{ "是否隐藏客户端模型", ID_GFXOP_MODELPROXY_HIDECLIENTMODEL},
	{ "是否渲染模型静止帧",	ID_GFXOP_MODELPROXY_USESTATICFRAME },
	{ "是否影响子模型", ID_GFXOP_MODELPROXY_INCLUDECHILDMODELS},
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },	// 优先级
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },	// 渲染层次
	GFX_REPEAT_INFO,
};

static const GFX_PROP_INFO _prop_container_info[] =
{
	{ "效果路径",		ID_GFXOP_CONTAINER_GFX_PATH },
	{ "使用外部色彩",	ID_GFXOP_CONTAINER_OUT_COLOR },
	{ "循环重新播放",	ID_GFXOP_CONTAINER_LOOP_FLAG },
	{ "GFX播放速度",	ID_GFXOP_CONTAINER_GFX_PLAYSPEED },
	{ "做傀儡时使用GFX比例", ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_sound_info[] =
{
	{ "强制2D",			ID_GFXOP_SOUND_FORCE2D },
	{ "是否循环",		ID_GFXOP_SOUND_LOOP },
	//{ "音量大小",		ID_GFXOP_SOUND_VOLUME },
	{ "最近距离",		ID_GFXOP_SOUND_MIN_DIST },
	{ "最远距离",		ID_GFXOP_SOUND_MAX_DIST },
	{ "最小音量",		ID_GFXOP_SOUND_VOLUME_MIN },
	{ "最大音量",		ID_GFXOP_SOUND_VOLUME_MAX },
	{ "最小音调[-12, 12]",	ID_GFXOP_SOUND_PITCH_MIN },
	{ "最大音调[-12, 12]",	ID_GFXOP_SOUND_PITCH_MAX },
	{ "声音文件",		ID_GFXOP_SOUND_FILE },
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO	_prop_ribbon_info[] = 
{
	{ "初始点坐标1",	ID_GFXOP_RIBBON_ORGPOS1 },
	{ "初始点坐标2",	ID_GFXOP_RIBBON_ORGPOS2 },
	{ "面片持续时间",	ID_GFXOP_RIBBON_SEGLIFE },
	{ "是否随父移动",	ID_GFXOP_RIBBON_BIND },
	{ "进入重力模式延迟时间(ms)", ID_GFXOP_RIBBON_TIME_TO_GRAVITY},
	{ "进入重力模式临界速度", ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY},
	{ "重力", ID_GFXOP_RIBBON_GRAVITY},
	{ "垂直扰动比例", ID_GFXOP_RIBBON_VERTICAL_NOISE},
	{ "垂直扰动速度", ID_GFXOP_RIBBON_VERTICAL_SPEED},
	{ "水平扰动振幅", ID_GFXOP_RIBBON_HORZ_AMPLITUDE},
	{ "水平扰动速度", ID_GFXOP_RIBBON_HORZ_SPEED},
	{ "是否扰动X轴", ID_GFXOP_RIBBON_X_NOISE},
	{ "是否扰动Z轴", ID_GFXOP_RIBBON_Z_NOISE},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_audioevent_info[] =
{
	{ "AudioEvent路径", ID_GFXOP_SOUND_FILE },
	{ "使用自定义音频属性",	ID_GFXOP_SOUND_USECUSTOM },
	{ "最小距离",		ID_GFXOP_SOUND_MIN_DIST },
	{ "最大距离",		ID_GFXOP_SOUND_MAX_DIST },
};

static const GFX_PROP_INFO _ctrl_base[] =
{
	{ "起始时间",		ID_KPCTRL_BASE_START_TM },
	{ "终止时间",		ID_KPCTRL_BASE_END_TM }
};

static const GFX_PROP_INFO _ctrl_move[] =
{
	CTRL_BASE_INFO
	{ "移动方向",		ID_KPCTRL_MOVE_DIR },
	{ "移动速度",		ID_KPCTRL_MOVE_VEL },
	{ "移动加速度",		ID_KPCTRL_MOVE_ACC }
};

static const GFX_PROP_INFO _ctrl_rot[] =
{
	CTRL_BASE_INFO
	{ "自转角速度",		ID_KPCTRL_ROT_VEL },
	{ "自转角加速度",	ID_KPCTRL_ROT_ACC }
};

static const GFX_PROP_INFO _ctrl_rot_axis[] =
{
	CTRL_BASE_INFO
	{ "轴位置",			ID_KPCTRL_ROTAXIS_POS },
	{ "轴方向",			ID_KPCTRL_ROTAXIS_DIR },
	{ "转动角速度",		ID_KPCTRL_ROTAXIS_VEL },
	{ "转动角加速度",	ID_KPCTRL_ROTAXIS_ACC }
};

static const GFX_PROP_INFO _ctrl_revol[] =
{
	CTRL_BASE_INFO
	{ "轴位置",			ID_KPCTRL_REVOL_POS },
	{ "轴方向",			ID_KPCTRL_REVOL_DIR },
	{ "转动角速度",		ID_KPCTRL_REVOL_VEL },
	{ "转动角加速度",	ID_KPCTRL_REVOL_ACC }
};

static const GFX_PROP_INFO _ctrl_centri_move[] =
{
	CTRL_BASE_INFO
	{ "中心点位置",		ID_KPCTRL_CENTRI_POS },
	{ "离心速度",		ID_KPCTRL_CENTRI_VEL },
	{ "离心加速度",		ID_CPCTRL_CENTRI_ACC }
};

static const GFX_PROP_INFO _ctrl_color[] =
{
	CTRL_BASE_INFO
	{ "R改变值",		ID_KPCTRL_COLOR_R },
	{ "G改变值",		ID_KPCTRL_COLOR_G },
	{ "B改变值",		ID_KPCTRL_COLOR_B },
	{ "A改变值",		ID_KPCTRL_COLOR_A }
};

static const GFX_PROP_INFO _ctrl_cltrans[] =
{
	CTRL_BASE_INFO
	{ "只影响透明度",	ID_KPCTLR_CLTRAN_ALPHAONLY},
	{ "初始色彩",		ID_KPCTRL_CLTRAN_ORG },
	{ "目标色彩数目",	ID_KPCTRL_CLTRAN_NUM },
	{ "目标色彩1",		ID_KPCTRL_CLTRAN_DEST1 },
	{ "过渡间隔1",		ID_KPCTRL_CLTRAN_TIME1 },
	{ "目标色彩2",		ID_KPCTRL_CLTRAN_DEST2 },
	{ "过渡间隔2",		ID_KPCTRL_CLTRAN_TIME2 },
	{ "目标色彩3",		ID_KPCTRL_CLTRAN_DEST3 },
	{ "过渡间隔3",		ID_KPCTRL_CLTRAN_TIME3 },
	{ "目标色彩4",		ID_KPCTRL_CLTRAN_DEST4 },
	{ "过渡间隔4",		ID_KPCTRL_CLTRAN_TIME4 },
	{ "目标色彩5",		ID_KPCTRL_CLTRAN_DEST5 },
	{ "过渡间隔5",		ID_KPCTRL_CLTRAN_TIME5 },
	{ "目标色彩6",		ID_KPCTRL_CLTRAN_DEST6 },
	{ "过渡间隔6",		ID_KPCTRL_CLTRAN_TIME6 },
	{ "目标色彩7",		ID_KPCTRL_CLTRAN_DEST7 },
	{ "过渡间隔7",		ID_KPCTRL_CLTRAN_TIME7 },
	{ "目标色彩8",		ID_KPCTRL_CLTRAN_DEST8 },
	{ "过渡间隔8",		ID_KPCTRL_CLTRAN_TIME8 },
	{ "目标色彩9",		ID_KPCTRL_CLTRAN_DEST9 },
	{ "过渡间隔9",		ID_KPCTRL_CLTRAN_TIME9 },
	{ "目标色彩10",		ID_KPCTRL_CLTRAN_DEST10 },
	{ "过渡间隔10",		ID_KPCTRL_CLTRAN_TIME10 },
};

static const GFX_PROP_INFO _ctrl_scltrans[] =
{
	CTRL_BASE_INFO
	{ "初始比例",		ID_KPCTRL_SCLTRAN_ORG },
	{ "目标比例数目",	ID_KPCTRL_SCLTRAN_NUM },
	{ "目标比例1",		ID_KPCTRL_SCLTRAN_DEST1 },
	{ "过渡间隔1",		ID_KPCTRL_SCLTRAN_TIME1 },
	{ "目标比例2",		ID_KPCTRL_SCLTRAN_DEST2 },
	{ "过渡间隔2",		ID_KPCTRL_SCLTRAN_TIME2 },
	{ "目标比例3",		ID_KPCTRL_SCLTRAN_DEST3 },
	{ "过渡间隔3",		ID_KPCTRL_SCLTRAN_TIME3 },
	{ "目标比例4",		ID_KPCTRL_SCLTRAN_DEST4 },
	{ "过渡间隔4",		ID_KPCTRL_SCLTRAN_TIME4 },
	{ "目标比例5",		ID_KPCTRL_SCLTRAN_DEST5 },
	{ "过渡间隔5",		ID_KPCTRL_SCLTRAN_TIME5 },
};

static const GFX_PROP_INFO _ctrl_scale[] =
{
	CTRL_BASE_INFO
	{ "比例改变值",		ID_KPCTRL_SCALE_DELTA },
	{ "最小比例",		ID_KPCTRL_SCALE_MIN },
	{ "最大比例",		ID_KPCTRL_SCALE_MAX }
};

static const GFX_PROP_INFO _ctrl_noise_base[] =
{
	{ "缓冲大小",		ID_KPCTRL_NOISE_BUFLEN },
	{ "振幅",			ID_KPCTRL_NOISE_AMP },
	{ "波长",			ID_KPCTRL_NOISE_WAVLEN },
	{ "Persistence",	ID_KPCTRL_NOISE_PERSIST	},
	{ "子波数目",		ID_KPCTRL_NOISE_OCTNUM }
};

static const GFX_PROP_INFO _ctrl_color_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId },
	{ "基本色彩",		ID_KPCTRL_NOISE_BASE_COLOR }
};

static const GFX_PROP_INFO _ctrl_scale_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId }
};

static const GFX_PROP_INFO _param_trail[] =
{
	{ "初始点坐标1",	ID_PARAM_TRAIL_POS1 },
	{ "初始点坐标2",	ID_PARAM_TRAIL_POS2	},
	{ "变换矩阵",		ID_PARAM_TRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_trailex[] =
{
    { "初始点坐标1",	ID_PARAM_TRAIL_POS1 },
    { "初始点坐标2",	ID_PARAM_TRAIL_POS2	},
    { "变换矩阵",		ID_PARAM_TRAIL_MATRIX }
};


static const GFX_PROP_INFO _param_ltntrail[] =
{
	{ "初始点坐标1",	ID_PARAM_LTNTRAIL_POS1 },
	{ "初始点坐标2",	ID_PARAM_LTNTRAIL_POS2	},
	{ "变换矩阵",		ID_PARAM_LTNTRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_ltn[] =
{
	{ "起始点坐标",		ID_PARAM_LTN_POS1 },
	{ "终点坐标",		ID_PARAM_LTN_POS2 }
};

static const GFX_PROP_INFO _param_ribbon[] =
{
	{ "初始点坐标1",	ID_PARAM_RIBBON_POS1 },
	{ "初始点坐标2",	ID_PARAM_RIBBON_POS2	},
	{ "变换矩阵",		ID_PARAM_RIBBON_MATRIX }
};

static const GFX_OBJ_INFO _gfx_method_info[] =
{
	{ "自转",		ID_KPCTRL_ROT,			_ctrl_rot,			PROP_COUNT(_ctrl_rot),			NULL,	NULL,	0 },
	{ "绕轴转动",	ID_KPCTRL_ROTAXIS,		_ctrl_rot_axis,		PROP_COUNT(_ctrl_rot_axis),		NULL,	NULL,	0 },
	{ "绕轴公转",	ID_KPCTRL_REVOL,		_ctrl_revol,		PROP_COUNT(_ctrl_revol),		NULL,	NULL,	0 },
	{ "离心运动",	ID_KPCTRL_CENTRI,		_ctrl_centri_move,	PROP_COUNT(_ctrl_centri_move),	NULL,	NULL,	0 },
	{ "色彩改变",	ID_KPCTRL_COLOR,		_ctrl_color,		PROP_COUNT(_ctrl_color),		NULL,	NULL,	0 },
	{ "色彩过渡",	ID_KPCTRL_CL_TRANS,		_ctrl_cltrans,		PROP_COUNT(_ctrl_cltrans),		NULL,	NULL,	0 },
	{ "噪音色彩",	ID_KPCTRL_CL_NOISE,		_ctrl_color_noise,	PROP_COUNT(_ctrl_color_noise),	NULL,	NULL,	0 },
	{ "比例改变",	ID_KPCTRL_SCALE,		_ctrl_scale,		PROP_COUNT(_ctrl_scale),		NULL,	NULL,	0 },
	{ "比例过渡",	ID_KPCTRL_SCALE_TRANS,	_ctrl_scltrans,		PROP_COUNT(_ctrl_scltrans),		NULL,	NULL,	0 },
	{ "噪音比例",	ID_KPCTRL_SCA_NOISE,	_ctrl_scale_noise,	PROP_COUNT(_ctrl_scale_noise),	NULL,	NULL,	0 },
	{ "平移",		ID_KPCTRL_MOVE,			_ctrl_move,			PROP_COUNT(_ctrl_move),			NULL,	NULL,	0 }
};

static const GFX_OBJ_INFO _phys_particle_method_info[] =
{
	{ "色彩改变",	ID_KPCTRL_COLOR,		_ctrl_color,		PROP_COUNT(_ctrl_color),		NULL,	NULL,	0 },
	{ "色彩过渡",	ID_KPCTRL_CL_TRANS,		_ctrl_cltrans,		PROP_COUNT(_ctrl_cltrans),		NULL,	NULL,	0 },
	{ "噪音色彩",	ID_KPCTRL_CL_NOISE,		_ctrl_color_noise,	PROP_COUNT(_ctrl_color_noise),	NULL,	NULL,	0 },
	{ "比例改变",	ID_KPCTRL_SCALE,		_ctrl_scale,		PROP_COUNT(_ctrl_scale),		NULL,	NULL,	0 },
	{ "比例过渡",	ID_KPCTRL_SCALE_TRANS,	_ctrl_scltrans,		PROP_COUNT(_ctrl_scltrans),		NULL,	NULL,	0 },
	{ "噪音比例",	ID_KPCTRL_SCA_NOISE,	_ctrl_scale_noise,	PROP_COUNT(_ctrl_scale_noise),	NULL,	NULL,	0 },
};


static const GFX_OBJ_INFO _gfx_obj_info[] =
{
	{ "3D面片",			ID_ELE_TYPE_DECAL_3D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "2D面片",			ID_ELE_TYPE_DECAL_2D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
    { "十字面片",		ID_ELE_TYPE_DECAL_BILLBOARD,	_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "抛物面",			ID_ELE_TYPE_PARABOLOID,			_prop_parab_info,		PROP_COUNT(_prop_parab_info),		NULL,	NULL,	0 },
	{ "轨迹",			ID_ELE_TYPE_TRAIL,				_prop_trail_info,		PROP_COUNT(_prop_trail_info),		NULL,	NULL,	0 },
#ifdef _ANGELICA21
    { "新轨迹",			ID_ELE_TYPE_TRAIL_EX,			_prop_trailex_info,		PROP_COUNT(_prop_trailex_info),		NULL,	NULL,	0 },
#endif
	{ "闪电轨迹",		ID_ELE_TYPE_LTNTRAIL,			_prop_ltntrail_info,	PROP_COUNT(_prop_ltntrail_info),	NULL,	NULL,	0 },
	{ "点型粒子",		ID_ELE_TYPE_PARTICLE_POINT,		_prop_pointpar_info,	PROP_COUNT(_prop_pointpar_info),	"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "盒型粒子",		ID_ELE_TYPE_PARTICLE_BOX,		_prop_boxpar_info,		PROP_COUNT(_prop_boxpar_info),		"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "多面粒子",		ID_ELE_TYPE_PARTICLE_MULTIPLANE,_prop_multiplane_info,	PROP_COUNT(_prop_multiplane_info),	"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "椭球粒子",		ID_ELE_TYPE_PARTICLE_ELLIPSOID,	_prop_ellipsoidpar_info,	PROP_COUNT(_prop_ellipsoidpar_info),		"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "圆柱粒子",		ID_ELE_TYPE_PARTICLE_CYLINDER,	_prop_cylinder_info,	PROP_COUNT(_prop_cylinder_info),	"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "曲线粒子",		ID_ELE_TYPE_PARTICLE_CURVE,		_prop_curve_info,		PROP_COUNT(_prop_curve_info),		"粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "骨骼粒子",		ID_ELE_TYPE_PARTICLE_SKELETON,	_prop_skeleton_par_info,PROP_COUNT(_prop_skeleton_par_info), "粒子效果影响器属性",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "灯光效果",		ID_ELE_TYPE_LIGHT,				_prop_light_info,		PROP_COUNT(_prop_light_info),		NULL,	NULL,	0 },
	{ "圆环",			ID_ELE_TYPE_RING,				_prop_ring_info,		PROP_COUNT(_prop_ring_info),		NULL,	NULL,	0 },
	{ "电弧",			ID_ELE_TYPE_LIGHTNING,			_prop_ln_info,			PROP_COUNT(_prop_ln_info),			NULL,	NULL,	0 },
	{ "闪电",			ID_ELE_TYPE_LTNBOLT,			_prop_bolt_info,		PROP_COUNT(_prop_bolt_info),		NULL,	NULL,	0 },
	{ "模型",			ID_ELE_TYPE_MODEL,				_prop_model_info,		PROP_COUNT(_prop_model_info),		NULL,	NULL,	0 },
	{ "声音",			ID_ELE_TYPE_SOUND,				_prop_sound_info,		PROP_COUNT(_prop_sound_info),		NULL,	NULL,	0 },
	{ "子效果",			ID_ELE_TYPE_GFX_CONTAINER,		_prop_container_info,	PROP_COUNT(_prop_container_info),	NULL,	NULL,	0 },
	{ "Grid3D面片",		ID_ELE_TYPE_GRID_DECAL_3D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "Grid2D面片",		ID_ELE_TYPE_GRID_DECAL_2D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "物理粒子",		ID_ELE_TYPE_PHYS_EMITTER,		_prop_physemitter_info,	PROP_COUNT(_prop_physemitter_info),	"物理粒子效果影响器属性",	_phys_particle_method_info,	OBJ_COUNT(_phys_particle_method_info) },
	{ "物理点粒子",		ID_ELE_TYPE_PHYS_POINTEMITTER,	_prop_physpointemitter_info, PROP_COUNT(_prop_physpointemitter_info), NULL, NULL, 0},
	{ "高级电弧",		ID_ELE_TYPE_LIGHTNINGEX,		_prop_ln_ex_info,		PROP_COUNT(_prop_ln_ex_info),		NULL,	NULL,	0},
	{ "ECModel",		ID_ELE_TYPE_ECMODEL,			_prop_ecmodel_info,		PROP_COUNT(_prop_ecmodel_info),		NULL,	NULL,	0},
	{ "模型代理",		ID_ELE_TYPE_MODEL_PROXY,		_prop_model_proxy_info,	PROP_COUNT(_prop_model_proxy_info),	NULL,	NULL,	0},
	{ "飘带",			ID_ELE_TYPE_RIBBON,				_prop_ribbon_info,	PROP_COUNT(_prop_ribbon_info),			NULL,	NULL,	0},
};

#ifdef _ANGELICA22
static const GFX_OBJ_INFO _gfx_audio_info_[] =
{
	{ "声音",			ID_ELE_TYPE_SOUND,				_prop_audioevent_info,		PROP_COUNT(_prop_audioevent_info),		NULL,	NULL,	0 },
};
#endif

static const GFX_PARAM_INFO _gfx_param_info[] =
{
	{ ID_ELE_TYPE_TRAIL,		_param_trail,	PROP_COUNT(_param_trail) },
	{ ID_ELE_TYPE_LIGHTNING,	_param_ltn,		PROP_COUNT(_param_ltn) },
	{ ID_ELE_TYPE_LTNTRAIL,		_param_ltntrail,PROP_COUNT(_param_ltntrail) },
	{ ID_ELE_TYPE_RIBBON,		_param_ribbon, PROP_COUNT(_param_ribbon) },
#ifdef _ANGELICA21
    { ID_ELE_TYPE_TRAIL_EX,		_param_trailex,	PROP_COUNT(_param_trailex) },
#endif
};

static const GFX_INTERP_INFO _gfx_interp_info[] =
{
	{ "不运用插值",		A3DGFXKeyPoint::enumInterpNone },
	{ "运用插值",		A3DGFXKeyPoint::enumInterpAll },
	{ "仅不对方向插值",	A3DGFXKeyPoint::enumInterpNoDir }
};

static const GFX_PROP_INFO _prop_kp_info[] =
{
	{ "中心点位置",		ID_GFXOP_KP_POS},
	{ "法线方向",		ID_GFXOP_KP_DIR},
	{ "颜色",			ID_GFXOP_KP_COLOR},
	{ "比例",			ID_GFXOP_KP_SCALE},
	{ "2D旋转角度",		ID_GFXOP_KP_RAD2D},
	{ "无限时间",		ID_GFXOP_KP_INFINITE},
	{ "插值方式",		ID_GFXOP_KP_INTERP},
	{ "持续时间",		ID_GFXOP_KP_TIMESPAN},
};

#endif

bool gfx_is_based_on_physx(int nTypeId)
{
	switch (nTypeId)
	{
	case ID_ELE_TYPE_PHYS_EMITTER:
	case ID_ELE_TYPE_PHYS_POINTEMITTER:
		return true;
	}
	return false;
}

int gfx_get_obj_count()
{
	return OBJ_COUNT(_gfx_obj_info);
}


const GFX_OBJ_INFO* gfx_obj_check(const GFX_OBJ_INFO* pInfo)
{

#ifdef _ANGELICA22
	if (pInfo->m_nTypeId == ID_ELE_TYPE_SOUND)
	{
		if (GFX_IsUseAudioEvent())
			return &_gfx_audio_info_[0];
	}
#endif

	return pInfo;
}


const GFX_OBJ_INFO* gfx_obj_info(int nIndex)
{
	return gfx_obj_check(&_gfx_obj_info[nIndex]);
}

const GFX_OBJ_INFO* gfx_obj_info_by_type(int nTypeId)
{
	for (int i = 0; i < OBJ_COUNT(_gfx_obj_info); i++)
	{
		if (_gfx_obj_info[i].m_nTypeId == nTypeId)
			return gfx_obj_check(&_gfx_obj_info[i]);
	}
	return NULL;
}

int gfx_ctrl_method_count()
{
	return OBJ_COUNT(_gfx_method_info);
}

const GFX_OBJ_INFO* gfx_method_info(int nIndex)
{
	return &_gfx_method_info[nIndex];
}

const GFX_PARAM_INFO* gfx_param_info(int nTypeId)
{
	for (int i = 0; i < PARAM_COUNT(_gfx_param_info); i++)
		if (_gfx_param_info[i].m_nTypeId == nTypeId)
			return &_gfx_param_info[i];
	return NULL;
}

const GFX_PROP_INFO* gfx_param_sub_info(int nTypeId, int nParamId)
{
	const GFX_PARAM_INFO* pParam = gfx_param_info(nTypeId);
	if (pParam == NULL) return NULL;
	for (int i = 0; i < pParam->m_nPropCount; i++)
		if (pParam->m_pPropInfo[i].m_OpId == nParamId)
			return &pParam->m_pPropInfo[i];
	return NULL;
}

int gfx_interp_type_count()
{
	return sizeof(_gfx_interp_info) / sizeof(GFX_INTERP_INFO);
}

const GFX_INTERP_INFO* gfx_interp_info(int nIndex)
{
	return &_gfx_interp_info[nIndex];
}

int gfx_shader_count()
{
	return SHADER_COUNT;
}

const GFX_SHADER_INFO* gfx_shader_info(int nIndex)
{
	return &_gfx_shader_info[nIndex];
}

const A3DSHADER& gfx_shader_from_mode(int nMode)
{
	for (int i = 0; i < SHADER_COUNT; i++)
		if (nMode == _gfx_shader_info[i].m_nMode)
			return _gfx_shader_info[i].m_Shader;
	return _gfx_shader_info[0].m_Shader;
}

int gfx_mode_from_shader(const A3DSHADER& sh)
{
	for (int i = 0; i < SHADER_COUNT; i++)
		if (sh.SrcBlend == _gfx_shader_info[i].m_Shader.SrcBlend
		 && sh.DestBlend == _gfx_shader_info[i].m_Shader.DestBlend)
		 return _gfx_shader_info[i].m_nMode;
	return _gfx_shader_info[0].m_nMode;
}

const GFX_PROP_INFO * gfx_prop_info(const GFX_PROP_INFO *pPropInfoSet, int nIndex)
{
	return &pPropInfoSet[nIndex];
}

const GFX_PROP_INFO * gfx_prop_info_by_name(const GFX_PROP_INFO *pPropInfoSet, int nCount, const AString &name)
{
	for (int i = 0; i < nCount; i++)
		if (name == pPropInfoSet[i].m_szName)
			return &pPropInfoSet[i];
	return NULL;
}

const GFX_OBJ_INFO* gfx_method_info_by_type(int nTypeID)
{
	for (int i = 0; i < gfx_ctrl_method_count(); i++)
		if (nTypeID == _gfx_method_info[i].m_nTypeId)
			return &_gfx_method_info[i];
	return NULL;
}

const GFX_OBJ_INFO* gfx_method_info_by_name(const AString & name)
{
	for (int i = 0; i < gfx_ctrl_method_count(); i++)
		if (name == _gfx_method_info[i].m_szName)
			return &_gfx_method_info[i];
	return NULL;
}

const GFX_PROP_INFO * gfx_kp_prop_info_set()
{
	return _prop_kp_info;
}

int gfx_size_of_kp_prop_set()
{
	return sizeof(_prop_kp_info) / sizeof(_prop_kp_info[0]);
}



#pragma warning(disable : 4715)

bool operator == (const GFX_PROPERTY& gp1, const GFX_PROPERTY& gp2)
{
	if (gp1.m_Type != gp2.m_Type) return false;

	switch (gp1.m_Type)
	{
	case GFX_VALUE_INT:
	case GFX_VALUE_SHADER_MODE:
	case GFX_VALUE_COLOR:
	case GFX_VALUE_RENDER_LAYER:
	case GFX_VALUE_RENDER_SIDE:
	case GFX_VALUE_LIGHTNING_FILTER:
	case GFX_VALUE_LIGHT_TYPE:
	case GFX_VALUE_ECM_LDTYPE:
	case GFX_VALUE_PHYSPAR_ORIENT:
	case GFX_VALUE_PHYSPAR_SHAPE:
	case GFX_VALUE_TRAIL_TYPE:
		return gp1.m_Data.i == gp2.m_Data.i;
	case GFX_VALUE_BOOL:
		return gp1.m_Data.b == gp2.m_Data.b;
	case GFX_VALUE_FLOAT:
		return gp1.m_Data.f == gp2.m_Data.f;
	case GFX_VALUE_VECTOR3:
		return (gp1.m_Data.vec.x == gp2.m_Data.vec.x
			 && gp1.m_Data.vec.y == gp2.m_Data.vec.y
			 && gp1.m_Data.vec.z == gp2.m_Data.vec.z);
	case GFX_VALUE_STRING:
	case GFX_VALUE_PATH_TEX:
	case GFX_VALUE_PATH_MOD:
	case GFX_VALUE_PATH_SOUND:
	case GFX_VALUE_PATH_GFX:
	case GFX_VALUE_PATH_MOX:
	case GFX_VALUE_SHADER_PATH:
	case GFX_VALUE_PATH_AUDIOEVENT:
		return gp1.m_string == gp2.m_string;
	case GFX_VALUE_MATRIX4:
		return (memcmp(gp1.m_Data.mat, gp2.m_Data.mat, sizeof(A3DMATRIX4)) == 0);
	case GFX_VALUE_QUATERNION:
		return (memcmp(gp1.m_Data.quad, gp2.m_Data.quad, sizeof(A3DQUATERNION)) == 0);
	case GFX_VALUE_RAND_STRING:
	case GFX_VALUE_RAND_PATH_SOUND:
		return *(gp1.m_Data.pRSC) == *(gp2.m_Data.pRSC);
	case GFX_VALUE_ANIMABLE:
		return *(gp1.m_Data.pAnimable) == *(gp2.m_Data.pAnimable);
	}

	return false;
}

bool operator != (const GFX_PROPERTY& gp1, const GFX_PROPERTY& gp2)
{
	return !(gp1 == gp2);
}

GFX_PROPERTY operator + (const GFX_PROPERTY &left, const GFX_PROPERTY &right)
{
	switch(left.GetType()) {
	case GFX_VALUE_INT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(int(left) + int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(int(left) + float(right));
		}
		break;
	case GFX_VALUE_FLOAT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(float(left) + int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(float(left) + float(right));
		}
		break;
	}
	return GFX_PROPERTY();
}

GFX_PROPERTY operator - (const GFX_PROPERTY &left, const GFX_PROPERTY &right)
{
	switch(left.GetType()) {
	case GFX_VALUE_INT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(int(left) - int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(int(left) - float(right));
		}
		break;
	case GFX_VALUE_FLOAT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(float(left) - int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(float(left) - float(right));
		}
		break;
	}
	ASSERT(false);
}

GFX_PROPERTY operator * (const GFX_PROPERTY &left, const GFX_PROPERTY &right)
{
	switch(left.GetType()) {
	case GFX_VALUE_INT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(int(left) * int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(int(left) * float(right));
		}
		break;
	case GFX_VALUE_FLOAT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(float(left) * int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(float(left) * float(right));
		}
		break;
	}
	return GFX_PROPERTY();
}

GFX_PROPERTY operator / (const GFX_PROPERTY &left, const GFX_PROPERTY &right)
{
	switch(left.GetType()) {
	case GFX_VALUE_INT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(int(left) / int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(int(left) / float(right));
		}
		break;
	case GFX_VALUE_FLOAT:
		switch(right.GetType()) 
		{
		case GFX_VALUE_INT:
			return GFX_PROPERTY(float(left) / int(right));
		case GFX_VALUE_FLOAT:
			return GFX_PROPERTY(float(left) / float(right));
		}
		break;
	}
	return GFX_PROPERTY();
}

