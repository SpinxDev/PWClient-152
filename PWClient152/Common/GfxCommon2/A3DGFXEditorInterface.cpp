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
/* !!!!!��������ʱ��ע�����е����԰汾��Ҫ��Ӧ����	*/

static const GFX_SHADER_INFO _gfx_shader_info[] =
{
	{ "AlphaLightMode",	ID_SHADER_NORMAL,		A3DBLEND_SRCALPHA,	A3DBLEND_INVSRCALPHA },
	{ "HighLightMode",	ID_SHADER_HILIGHT,		A3DBLEND_SRCALPHA,	A3DBLEND_ONE },
	{ "SuperLightMode",	ID_SHADER_SUPER_HILIGHT,A3DBLEND_ONE,		A3DBLEND_ONE },
	{ "ColorLightMode",	ID_SHADER_COLOR,		A3DBLEND_ONE,		A3DBLEND_INVSRCCOLOR }
};

/* !!!!!IMPORTANT new prop must be added at tail */
/* !!!!!��β��������Ϊ�����ĺ���ʹ�������ģ����������øú�����������ֱ������������ֵ */
/* !!!!!��������ʱ��ע�ⶥ����Ӧ�ĺ���Ҫ����	*/
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
	{ "Alpha����ģʽ",	ID_SHADER_NORMAL,		A3DBLEND_SRCALPHA,	A3DBLEND_INVSRCALPHA },
	{ "����ģʽ",		ID_SHADER_HILIGHT,		A3DBLEND_SRCALPHA,	A3DBLEND_ONE },
	{ "����ģʽ",		ID_SHADER_SUPER_HILIGHT,A3DBLEND_ONE,		A3DBLEND_ONE },
	{ "��ɫ����ģʽ",	ID_SHADER_COLOR,		A3DBLEND_ONE,		A3DBLEND_INVSRCCOLOR }
};

/* !!!!!IMPORTANT new prop must be added at tail */
/* !!!!!��Ҫ��β����������Ϊ�������ø����Ե����������ֱ�����������Ե�����ֵ */
/* !!!!!��������ʱ��ע�ⶥ����Ӧ�ĺ���Ҫ����	*/
static const GFX_PROP_INFO _prop_base_info[] =
{
	{ "Alphaģʽ",		ID_GFXOP_ALPHA_MODE },				// 0
	{ "�Ƿ��ܱ��ڵ�",	ID_GFXOP_ZTESTENABLE },				// 1
	{ "���õ��淨��",	ID_GFXOP_GROUND_NORMAL },			// 2
	{ "��ͼ�ļ�·��",	ID_GFXOP_TEX_PATH },				// 3
	{ "ƽ��ģʽ",		ID_GFXOP_TILE_MODE },				// 4
	{ "U����ƽ���ٶ�",	ID_GFXOP_U_SPEED },					// 5
	{ "V����ƽ���ٶ�",	ID_GFXOP_V_SPEED },					// 6
	{ "UV���껥��",		ID_GFXOP_UV_INTERCHANGE },			// 7
	{ "U���귭ת",		ID_GFXOP_U_REVERSE },				// 8
	{ "V���귭ת",		ID_GFXOP_V_REVERSE },				// 9
	{ "��������",		ID_GFXOP_TEX_ROWS },				// 10
	{ "��������",		ID_GFXOP_TEX_COLS },				// 11
	{ "�������ʱ��",	ID_GFXOP_TEX_INTERVAL },			// 12
	{ "��ʾ���ȼ�",		ID_GFXOP_PRIORITY },				// 13
	{ "�Ƿ����",		ID_GFXOP_IS_DUMMY },				// 14
	{ "�����ռ�Ť��",	ID_GFXOP_WARP },					// 15
	{ "��Ⱦ���",		ID_GFXOP_RENDER_LAYER },			// 16
	{ "���õ���߶�",	ID_GFXOP_GROUND_HEIGHT },			// 17
	{ "��������ͼ����",	ID_GFXOP_TEX_NODOWNSAMPLE },		// 18
	{ "ѭ������ʱ����״̬", ID_GFXOP_RESETONLOOPEND },		// 19
	{ "���������ʱ��", ID_GFXOP_TEXANIMMAXTIME },		// 20
};

static const GFX_PROP_INFO _prop_repeat_info[] =
{
	{ "�ظ�����",		ID_GFXOP_REPEATCOUNT },
	{ "�ظ����",		ID_GFXOP_REPEATDELAY },
	{ "�ܷ񵭳�",		ID_GFXOP_CAN_DO_FADE_OUT },
    { "�Ƿ����Ե",		ID_GFXOP_SOFT_EDGE },
	{ "ʹ���ڲ��ƹ�",	ID_GFXOP_USE_INNER_LIGHT },
};

static const GFX_PROP_INFO _prop_pixelshader_info[] =
{
	{ "Shader·��",		ID_GFXOP_PIXELSHADERPATH },
	{ "Shader������ͼ",	ID_GFXOP_SHADER_TEX },
};

static const GFX_PROP_INFO _prop_decal_info[] =
{
	{ "���",			ID_GFXOP_DECAL_WIDTH },
	{ "�߶�",			ID_GFXOP_DECAL_HEIGHT },
	{ "�Ƿ����ӽ�ƫת",	ID_GFXOP_DECAL_ROTFROMVIEW },
	{ "���������б",	ID_GFXOP_DECAL_GRNDNORM_ONLY },
	{ "��������",		ID_GFXOP_DECAL_MATCH_SURFACE },
	{ "��ȱ�������",	ID_GFXOP_DECAL_NO_WID_SCALE },
	{ "�߶ȱ�������",	ID_GFXOP_DECAL_NO_HEI_SCALE },
	{ "ԭ���ȱ���",	ID_GFXOP_DECAL_ORG_PT_WID },
	{ "ԭ��߶ȱ���",	ID_GFXOP_DECAL_ORG_PT_HEI },
	{ "���ӵ�ƫ��",		ID_GFXOP_DECAL_Z_OFFSET },
	{ "���õ��淨��ʱ������Ч", ID_GFXOP_DECAL_YAWEFFECT_GRNDNORM},
	{ "����ʱʹ��Gfx����",		ID_GFXOP_DECAL_SURFACE_USE_PARENT_DIR },
	{ "2D��Ƭʹ����Ļ����",		ID_GFXOP_DECAL_SCREEN_DIMENSION },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_griddecal_info[] = 
{
	{ "����Ƶ���Ŀ",	ID_GFXOP_GRIDDECAL_W_VNUM },
	{ "�߿��Ƶ���Ŀ",	ID_GFXOP_GRIDDECAL_H_VNUM },
	{ "��������С",	ID_GFXOP_GRIDDECAL_GRIDSIZE},
	{ "���ӵ�ƫ��",		ID_GFXOP_GRIDDECAL_Z_OFFSET},
	{ "�ܱ����ı�Ӱ��",	ID_GFXOP_GRIDDECAL_AFFBYSCALE},
	{ "�Ƿ����ӽ�ƫת",	ID_GFXOP_GRIDDECAL_ROTFROMVIEW },
	{ "����ʱ�����߶�",	ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT},
	{ "��������", ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_physemitter_info[] = 
{
	{ "���ӿ��",			ID_GFXOP_PHYSPAR_PE_PARWIDTH},
	{ "���Ӹ߶�",			ID_GFXOP_PHYSPAR_PE_PARHEIGHT},	
	{ "��С����",			ID_GFXOP_PHYSPAR_PE_MINSCALE},
	{ "������",			ID_GFXOP_PHYSPAR_PE_MAXSCALE},
	{ "��С��ת�Ƕ�",		ID_GFXOP_PHYSPAR_PE_MINROT},
	{ "�����ת�Ƕ�",		ID_GFXOP_PHYSPAR_PE_MAXROT},
	{ "��С��ɫֵ",			ID_GFXOP_PHYSPAR_PE_MINCOLOR},
	{ "�����ɫֵ",			ID_GFXOP_PHYSPAR_PE_MAXCOLOR},
	{ "���ӳ���",			ID_GFXOP_PHYSPAR_PE_PARORIENT},
	{ "����ʱֹͣ����",		ID_GFXOP_PHYSPAR_PE_STOP_EMIT_WHEN_FADE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_physpointemitter_info[] = 
{
	{ "���Ӵ�С",			ID_GFXOP_PHYSPAR_PPE_POINTSIZE},
	{ "��������A",			ID_GFXOP_PHYSPAR_PPE_SCALEA},
	{ "��������B",			ID_GFXOP_PHYSPAR_PPE_SCALEB},
	{ "��������C",			ID_GFXOP_PHYSPAR_PPE_SCALEC},
	{ "���뵭��",			ID_GFXOP_PHYSPAR_PPE_ISFADE},
	{ "���뵭��ʱ��",		ID_GFXOP_PHYSPAR_PPE_FADETIME},
	{_prop_base_info[0].m_szName,	_prop_base_info[0].m_OpId},
	{_prop_base_info[3].m_szName,	_prop_base_info[3].m_OpId},
	{_prop_base_info[13].m_szName,	_prop_base_info[13].m_OpId},
	{_prop_base_info[16].m_szName,	_prop_base_info[16].m_OpId},
	{_prop_base_info[18].m_szName,	_prop_base_info[18].m_OpId},
	{_prop_base_info[19].m_szName,	_prop_base_info[19].m_OpId},
};

static const GFX_PROP_INFO _prop_parab_info[] =
{
	{ "������ϵ��",		ID_GFXOP_PARAB_COEFF },
	{ "�߶�",			ID_GFXOP_DECAL_HEIGHT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_trail_info[] =
{
	{ "��ʼ������1",	ID_GFXOP_TRAIL_ORGPOS1 },
	{ "��ʼ������2",	ID_GFXOP_TRAIL_ORGPOS2 },
	{ "��Ƭ����ʱ��",	ID_GFXOP_TRAIL_SEGLIFE },
	{ "�Ƿ��游�ƶ�",	ID_GFXOP_TRAIL_BIND },
	{ "�Ƿ�ƽ��ģʽ",	ID_GFXOP_TRAIL_SPLINEMODE },
	{ "�������(ƽ��|��λms)",	 ID_GFXOP_TRAIL_SAMPLEFREQ },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_trailex_info[] =
{
    { "��ʼ������1",	ID_GFXOP_TRAIL_ORGPOS1 },
    { "��ʼ������2",	ID_GFXOP_TRAIL_ORGPOS2 },
    { "��Ƭ����ʱ��",	ID_GFXOP_TRAIL_SEGLIFE },
    { "�Ƿ��游�ƶ�",	ID_GFXOP_TRAIL_BIND },
    { "��ʧ��βʱ��",	ID_GFXOP_TRAIL_SHRINK },
    { "�������(ƽ��|��λms)",	 ID_GFXOP_TRAIL_SAMPLEFREQ },
    GFX_BASE_INFO,
    GFX_REPEAT_INFO,
    GFX_PIXELSHADER_INFO
};


static const GFX_PROP_INFO _prop_ltntrail_info[] =
{
	{ "��ʼ������1",	ID_GFXOP_LTNTRAIL_POS1 },
	{ "��ʼ������2",	ID_GFXOP_LTNTRAIL_POS2 },
	{ "��Ƭ����ʱ��",	ID_GFXOP_LTNTRAIL_SEGLIFE },
	{ "��С���",		ID_GFXOP_LTNTRAIL_MIN_AMP },
	{ "������",		ID_GFXOP_LTNTRAIL_MAX_AMP },
	{ "�Ƿ��游�ƶ�",	ID_GFXOP_LTNTRAIL_BIND },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _emitter_base_info[] =
{
	{ "��������",		ID_GFXOP_EMITTER_RATE },
	{ "��ɢ�Ƕ�",		ID_GFXOP_EMITTER_ANGLE },
	{ "�����ٶ�",		ID_GFXOP_EMITTER_SPEED },
	{ "����������ٶ�",	ID_GFXOP_EMITTER_PAR_ACC },
	{ "���ٶȷ���",		ID_GFXOP_EMITTER_ACC_DIR },
	{ "���ٶȴ�С",		ID_GFXOP_EMITTER_ACC },
	{ "���ӳ���(��XYZ��)",	ID_GFXOP_EMITTER_PARTICAL_INITDIR},
	{ "��������",		ID_GFXOP_EMITTER_TTL },
	{ "����ɫ����Сֵ",	ID_GFXOP_EMITTER_COLOR_MIN },
	{ "����ɫ�����ֵ",	ID_GFXOP_EMITTER_COLOR_MAX },
	{ "������Сֵ",		ID_GFXOP_EMITTER_SCALE_MIN },
	{ "�������ֵ",		ID_GFXOP_EMITTER_SCALE_MAX },
	{ "��ʼ�Ƕ���Сֵ",	ID_GFXOP_EMITTER_ROT_MIN },
	{ "��ʼ�Ƕ����ֵ",	ID_GFXOP_EMITTER_ROT_MAX },
	{ "�������������",	ID_GFXOP_EMITTER_SURFACE },
	{ "�����游�ƶ�",	ID_GFXOP_EMITTER_BIND },
	{ "����׷���ƶ�",	ID_GFXOP_EMITTER_DRAG },
	{ "׷��ǿ��",		ID_GFXOP_EMITTER_DRAG_POWER	},
	{ "��ɫʹ��HSV��ֵ", ID_GFXOP_EMITTER_COLOR_HSVINTERP},
};

static const GFX_PROP_INFO _prop_parsys_base_info[] =
{
	{ "�������",		ID_GFXOP_PARSYS_PAR_QUOTA },
	{ "���ӿ��",		ID_GFXOP_PARSYS_PAR_WIDTH },
	{ "���Ӹ߶�",		ID_GFXOP_PARSYS_PAR_HEIGHT },
	{ "3D����",			ID_GFXOP_PARSYS_PAR_3D },
	{ "�����ӵ�",		ID_GFXOP_PARSYS_PAR_FACING },
	{ "���ӵ�ƫ��",		ID_GFXOP_PARSYS_Z_OFFSET },
	{ "����������λ��",	ID_GFXOP_PARSYS_SCALE_NO_OFFSET },
	{ "��ȱ�������",	ID_GFXOP_PARSYS_NO_WID_SCALE },
	{ "�߶ȱ�������",	ID_GFXOP_PARSYS_NO_HEI_SCALE },
	{ "ԭ���ȱ���",	ID_GFXOP_PARSYS_ORG_PT_WID },
	{ "ԭ��߶ȱ���",	ID_GFXOP_PARSYS_ORG_PT_HEI },
	{ "���Ӷ�����ͼ����", ID_GFXOP_PARSYS_USE_PAR_UV },
	{ "���Ӳ���ʱ�����ͼ����", ID_GFXOP_PARSYS_INIT_RANDOM },
	{ "���Ӵӵ������", ID_GFXOP_PARSYS_USE_GRND_HEIGHT },
	{ "����ʱֹͣ����", ID_GFXOP_PARSYS_STOP_EMIT_WHEN_FADE },
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
	{ "�����С",		ID_GFXOP_EMITTER_AREA_SIZE }
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
	{ "���Ȳ�������",	ID_GFXOP_EMITTER_ELLIPSOID_ISAVG },
	{ "���ȷֶ���",		ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG },
	{ "γ�ȷֶ���",		ID_GFXOP_EMITTER_ELLIPSOID_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_cylinder_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "���Ȳ�������",	ID_GFXOP_EMITTER_CYLINDER_ISAVG },
	{ "���ȷֶ���",		ID_GFXOP_EMITTER_CYLINDER_ALPHASEG },
	{ "γ�ȷֶ���",		ID_GFXOP_EMITTER_CYLINDER_BETASEG },
	AREA_EMITTER_BASE_INFO,
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_curve_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "���߰뾶",		ID_GFXOP_EMITTER_CURVE_R },
	{ "�Ƿ���Ȳ���",	ID_GFXOP_EMITTER_CURVE_ISAVG},
	{ "���ȷֶ���Ŀ",	ID_GFXOP_EMITTER_CURVE_AVGSEGNUM},
	{ "����˳����������", ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_multiplane_info[] =
{
	PARSYS_BASE_INFO,
	EMITTER_BASE_INFO,
	{ "�ο�ģ��MOX",		ID_GFXOP_EMITTER_REF_MOX },
	{ "�ο�ģ��SMD",		ID_GFXOP_EMITTER_REF_SMD},
	{ "SMDģ��Skin����",	ID_GFXOP_EMITTER_SMD_SKIN_INDEX	},
	{ "SMDģ��Skin Mesh����", ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX   },
	{ "ʹ�÷���",		ID_GFXOP_EMITTER_USE_NORMAL },
	{ "��ֱ�ڷ���",		ID_GFXOP_EMITTER_PERP_TO_NORMAL },
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
	{ "�ƹ�����",		ID_GFXOP_LIGHT_TYPE },
	{ "����ɫ��",		ID_GFXOP_LIGHT_AMBIENT },
//	{ "ɢ��ɫ��",		ID_GFXOP_LIGHT_DIFFUSE },
//	{ "����ɫ��",		ID_GFXOP_LIGHT_SPECULAR },
	{ "��Դλ��",		ID_GFXOP_LIGHT_POSITION },
//	{ "���䷽��",		ID_GFXOP_LIGHT_DIRECTION },
	{ "���շ�Χ",		ID_GFXOP_LIGHT_RANGE },
	{ "˥������0",		ID_GFXOP_LIGHT_ATTENUATION0	},
	{ "˥������1",		ID_GFXOP_LIGHT_ATTENUATION1	},
	{ "˥������2",		ID_GFXOP_LIGHT_ATTENUATION2 },
	{ "��Gfx�ڲ���Ч",	ID_GFXOP_INNER_USE },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_ring_info[] =
{
	{ "Բ���뾶",		ID_GFXOP_RING_RADIUS },
	{ "Բ���߶�",		ID_GFXOP_RING_HEIGHT },
	{ "��б�Ƕ�",		ID_GFXOP_RING_PITCH },
	{ "Բ���ֶ���",		ID_GFXOP_RING_SECTS },
	{ "�뾶��������",	ID_GFXOP_RING_NORADSCALE },
	{ "�߶ȱ�������",	ID_GFXOP_RING_NOHEISCALE },
	{ "ԭ�����",		ID_GFXOP_RING_ORGATCENTER },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

// Notion!!!
// ���绡����������ʱ��Ҫ��Ӧ���Ӹ߼��绡(ln_ex)�����ԡ�
static const GFX_PROP_INFO _prop_ln_info[] =
{
	{ "��ʼ������",		ID_GFXOP_LN_START_POS },
	{ "�յ�����",		ID_GFXOP_LN_END_POS },
	{ "�������",		ID_GFXOP_LN_SEGS },
	{ "�������",		ID_GFXOP_LN_NUM },
	{ "���粨��",		ID_GFXOP_LN_WAVELEN },
	{ "���ʱ��",		ID_GFXOP_LN_INTERVAL },
	{ "������ʼ���",	ID_GFXOP_LN_WIDTH_START },
	{ "������ֹ���",	ID_GFXOP_LN_WIDTH_END },
	{ "�����м���",	ID_GFXOP_LN_WIDTH_MID },
	{ "������ʼAlpha",	ID_GFXOP_LN_ALPHA_START },
	{ "������ֹAlpha",	ID_GFXOP_LN_ALPHA_END },
	{ "�����м�Alpha",	ID_GFXOP_LN_ALPHA_MID },
	{ "����������",	ID_GFXOP_LN_USE_NORMAL },
	{ "������",		ID_GFXOP_LN_NORMAL },
	{ "�ɱ����",			ID_GFXOP_LN_AMPLITUDE },
	{ _ctrl_noise_base[3].m_szName, _ctrl_noise_base[3].m_OpId },
	{ "������",			ID_GFXOP_LN_FILTER },
	{ "�Ƿ񲨶�",		ID_GFXOP_LN_WAVEMOVE },
	{ "�����ٶ�",		ID_GFXOP_LN_WAVEMOVESPEED },
	{ "�Ƿ񲨶��̶�����",	ID_GFXOP_LN_FIXWAVELENGTH },
	{ "��������",			ID_GFXOP_LN_NUMWAVES },
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
	{ "��Ⱦ�߲�",		ID_GFXOP_LN_EX_RENDERSIDE },
	{ "�Ƿ񽥽�����",	ID_GFXOP_LN_EX_ISAPPENDLY },
	{ "�Ƿ�ʹ�ö�������",	ID_GFXOP_LN_EX_ISUSEVERTSLIFE },
	{ "��������",			ID_GFXOP_LN_EX_VERTSLIFE	},
	{ "β������",		ID_GFXOP_LN_EX_ISTAILFADEOUT },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_bolt_info[] =
{
	{ "��֧ƫ�ƽǶ�",	ID_GFXOP_BOLT_DEVIATION },
	{ "���粽��",		ID_GFXOP_BOLT_STEPS },
	{ "�����֧��",		ID_GFXOP_BOLT_BRANCHES },
	{ "��С����",		ID_GFXOP_BOLT_STEP_MIN },
	{ "��󲽳�",		ID_GFXOP_BOLT_STEP_MAX },
	{ "������ʼ���",	ID_GFXOP_BOLT_WIDTH_START },
	{ "������ֹ���",	ID_GFXOP_BOLT_WIDTH_END },
	{ "��ʼAlpha",		ID_GFXOP_BOLT_ALPHA_START },
	{ "��ֹAlpha",		ID_GFXOP_BOLT_ALPHA_END },
	{ "�������",		ID_GFXOP_BOLT_AMP },
	{ "ʱ����",		ID_GFXOP_BOLT_INTERVAL },
	{ "��Ȧ������",		ID_GFXOP_BOLT_PER_BOLTS },
	{ "Ȧ��",			ID_GFXOP_BOLT_CIRCLES },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_model_info[] =
{
	{ "ģ��·��",		ID_GFXOP_MODEL_PATH },
	{ "ģ�Ͷ���",		ID_GFXOP_MODEL_ACT_NAME },
	{ "ѭ������",		ID_GFXOP_MODEL_LOOPS },
	{ "Alpha�Ƚ�",		ID_GFXOP_MODEL_ALPHA_CMP },
	{ "�ڵ�������Ч",	ID_GFXOP_MODEL_WRITE_Z },
//	{ "2d��Ⱦ��ǿ��ʹ��3D�����", ID_GFXOP_MODEL_USE_3DCAMERA },
	{ "ģ�ͳ����˶�����",	ID_GFXOP_MODEL_FACE_DIR },
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
	{ "ģ��·��",		ID_GFXOP_ECMODEL_PATH },
	{ "ģ�Ͷ���",		ID_GFXOP_ECMODEL_ACT_NAME },
	//{ "ʹ������ģ��Ƥ��",	ID_GFXOP_ECMODEL_USECASTERSKIN },
	{ "ģ�ͼ�������",	ID_GFXOP_ECMODEL_LDTYPE },
	{ "�û�ָ��",		ID_GFXOP_ECMODEL_USERCMD },
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
	{ "Alphaģʽ",			ID_GFXOP_ALPHA_MODE },
	{ "�Ƿ����ؿͻ���ģ��", ID_GFXOP_MODELPROXY_HIDECLIENTMODEL},
	{ "�Ƿ���Ⱦģ�;�ֹ֡",	ID_GFXOP_MODELPROXY_USESTATICFRAME },
	{ "�Ƿ�Ӱ����ģ��", ID_GFXOP_MODELPROXY_INCLUDECHILDMODELS},
	{ _prop_base_info[13].m_szName, _prop_base_info[13].m_OpId },	// ���ȼ�
	{ _prop_base_info[16].m_szName, _prop_base_info[16].m_OpId },	// ��Ⱦ���
	GFX_REPEAT_INFO,
};

static const GFX_PROP_INFO _prop_container_info[] =
{
	{ "Ч��·��",		ID_GFXOP_CONTAINER_GFX_PATH },
	{ "ʹ���ⲿɫ��",	ID_GFXOP_CONTAINER_OUT_COLOR },
	{ "ѭ�����²���",	ID_GFXOP_CONTAINER_LOOP_FLAG },
	{ "GFX�����ٶ�",	ID_GFXOP_CONTAINER_GFX_PLAYSPEED },
	{ "������ʱʹ��GFX����", ID_GFXOP_CONTAINER_USE_GFX_SCALE_WHENDUMMY },
	GFX_BASE_INFO,
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO _prop_sound_info[] =
{
	{ "ǿ��2D",			ID_GFXOP_SOUND_FORCE2D },
	{ "�Ƿ�ѭ��",		ID_GFXOP_SOUND_LOOP },
	//{ "������С",		ID_GFXOP_SOUND_VOLUME },
	{ "�������",		ID_GFXOP_SOUND_MIN_DIST },
	{ "��Զ����",		ID_GFXOP_SOUND_MAX_DIST },
	{ "��С����",		ID_GFXOP_SOUND_VOLUME_MIN },
	{ "�������",		ID_GFXOP_SOUND_VOLUME_MAX },
	{ "��С����[-12, 12]",	ID_GFXOP_SOUND_PITCH_MIN },
	{ "�������[-12, 12]",	ID_GFXOP_SOUND_PITCH_MAX },
	{ "�����ļ�",		ID_GFXOP_SOUND_FILE },
	GFX_REPEAT_INFO
};

static const GFX_PROP_INFO	_prop_ribbon_info[] = 
{
	{ "��ʼ������1",	ID_GFXOP_RIBBON_ORGPOS1 },
	{ "��ʼ������2",	ID_GFXOP_RIBBON_ORGPOS2 },
	{ "��Ƭ����ʱ��",	ID_GFXOP_RIBBON_SEGLIFE },
	{ "�Ƿ��游�ƶ�",	ID_GFXOP_RIBBON_BIND },
	{ "��������ģʽ�ӳ�ʱ��(ms)", ID_GFXOP_RIBBON_TIME_TO_GRAVITY},
	{ "��������ģʽ�ٽ��ٶ�", ID_GFXOP_RIBBON_VELOCITY_TO_GRAVITY},
	{ "����", ID_GFXOP_RIBBON_GRAVITY},
	{ "��ֱ�Ŷ�����", ID_GFXOP_RIBBON_VERTICAL_NOISE},
	{ "��ֱ�Ŷ��ٶ�", ID_GFXOP_RIBBON_VERTICAL_SPEED},
	{ "ˮƽ�Ŷ����", ID_GFXOP_RIBBON_HORZ_AMPLITUDE},
	{ "ˮƽ�Ŷ��ٶ�", ID_GFXOP_RIBBON_HORZ_SPEED},
	{ "�Ƿ��Ŷ�X��", ID_GFXOP_RIBBON_X_NOISE},
	{ "�Ƿ��Ŷ�Z��", ID_GFXOP_RIBBON_Z_NOISE},
	GFX_BASE_INFO,
	GFX_REPEAT_INFO,
	GFX_PIXELSHADER_INFO
};

static const GFX_PROP_INFO _prop_audioevent_info[] =
{
	{ "AudioEvent·��", ID_GFXOP_SOUND_FILE },
	{ "ʹ���Զ�����Ƶ����",	ID_GFXOP_SOUND_USECUSTOM },
	{ "��С����",		ID_GFXOP_SOUND_MIN_DIST },
	{ "������",		ID_GFXOP_SOUND_MAX_DIST },
};

static const GFX_PROP_INFO _ctrl_base[] =
{
	{ "��ʼʱ��",		ID_KPCTRL_BASE_START_TM },
	{ "��ֹʱ��",		ID_KPCTRL_BASE_END_TM }
};

static const GFX_PROP_INFO _ctrl_move[] =
{
	CTRL_BASE_INFO
	{ "�ƶ�����",		ID_KPCTRL_MOVE_DIR },
	{ "�ƶ��ٶ�",		ID_KPCTRL_MOVE_VEL },
	{ "�ƶ����ٶ�",		ID_KPCTRL_MOVE_ACC }
};

static const GFX_PROP_INFO _ctrl_rot[] =
{
	CTRL_BASE_INFO
	{ "��ת���ٶ�",		ID_KPCTRL_ROT_VEL },
	{ "��ת�Ǽ��ٶ�",	ID_KPCTRL_ROT_ACC }
};

static const GFX_PROP_INFO _ctrl_rot_axis[] =
{
	CTRL_BASE_INFO
	{ "��λ��",			ID_KPCTRL_ROTAXIS_POS },
	{ "�᷽��",			ID_KPCTRL_ROTAXIS_DIR },
	{ "ת�����ٶ�",		ID_KPCTRL_ROTAXIS_VEL },
	{ "ת���Ǽ��ٶ�",	ID_KPCTRL_ROTAXIS_ACC }
};

static const GFX_PROP_INFO _ctrl_revol[] =
{
	CTRL_BASE_INFO
	{ "��λ��",			ID_KPCTRL_REVOL_POS },
	{ "�᷽��",			ID_KPCTRL_REVOL_DIR },
	{ "ת�����ٶ�",		ID_KPCTRL_REVOL_VEL },
	{ "ת���Ǽ��ٶ�",	ID_KPCTRL_REVOL_ACC }
};

static const GFX_PROP_INFO _ctrl_centri_move[] =
{
	CTRL_BASE_INFO
	{ "���ĵ�λ��",		ID_KPCTRL_CENTRI_POS },
	{ "�����ٶ�",		ID_KPCTRL_CENTRI_VEL },
	{ "���ļ��ٶ�",		ID_CPCTRL_CENTRI_ACC }
};

static const GFX_PROP_INFO _ctrl_color[] =
{
	CTRL_BASE_INFO
	{ "R�ı�ֵ",		ID_KPCTRL_COLOR_R },
	{ "G�ı�ֵ",		ID_KPCTRL_COLOR_G },
	{ "B�ı�ֵ",		ID_KPCTRL_COLOR_B },
	{ "A�ı�ֵ",		ID_KPCTRL_COLOR_A }
};

static const GFX_PROP_INFO _ctrl_cltrans[] =
{
	CTRL_BASE_INFO
	{ "ֻӰ��͸����",	ID_KPCTLR_CLTRAN_ALPHAONLY},
	{ "��ʼɫ��",		ID_KPCTRL_CLTRAN_ORG },
	{ "Ŀ��ɫ����Ŀ",	ID_KPCTRL_CLTRAN_NUM },
	{ "Ŀ��ɫ��1",		ID_KPCTRL_CLTRAN_DEST1 },
	{ "���ɼ��1",		ID_KPCTRL_CLTRAN_TIME1 },
	{ "Ŀ��ɫ��2",		ID_KPCTRL_CLTRAN_DEST2 },
	{ "���ɼ��2",		ID_KPCTRL_CLTRAN_TIME2 },
	{ "Ŀ��ɫ��3",		ID_KPCTRL_CLTRAN_DEST3 },
	{ "���ɼ��3",		ID_KPCTRL_CLTRAN_TIME3 },
	{ "Ŀ��ɫ��4",		ID_KPCTRL_CLTRAN_DEST4 },
	{ "���ɼ��4",		ID_KPCTRL_CLTRAN_TIME4 },
	{ "Ŀ��ɫ��5",		ID_KPCTRL_CLTRAN_DEST5 },
	{ "���ɼ��5",		ID_KPCTRL_CLTRAN_TIME5 },
	{ "Ŀ��ɫ��6",		ID_KPCTRL_CLTRAN_DEST6 },
	{ "���ɼ��6",		ID_KPCTRL_CLTRAN_TIME6 },
	{ "Ŀ��ɫ��7",		ID_KPCTRL_CLTRAN_DEST7 },
	{ "���ɼ��7",		ID_KPCTRL_CLTRAN_TIME7 },
	{ "Ŀ��ɫ��8",		ID_KPCTRL_CLTRAN_DEST8 },
	{ "���ɼ��8",		ID_KPCTRL_CLTRAN_TIME8 },
	{ "Ŀ��ɫ��9",		ID_KPCTRL_CLTRAN_DEST9 },
	{ "���ɼ��9",		ID_KPCTRL_CLTRAN_TIME9 },
	{ "Ŀ��ɫ��10",		ID_KPCTRL_CLTRAN_DEST10 },
	{ "���ɼ��10",		ID_KPCTRL_CLTRAN_TIME10 },
};

static const GFX_PROP_INFO _ctrl_scltrans[] =
{
	CTRL_BASE_INFO
	{ "��ʼ����",		ID_KPCTRL_SCLTRAN_ORG },
	{ "Ŀ�������Ŀ",	ID_KPCTRL_SCLTRAN_NUM },
	{ "Ŀ�����1",		ID_KPCTRL_SCLTRAN_DEST1 },
	{ "���ɼ��1",		ID_KPCTRL_SCLTRAN_TIME1 },
	{ "Ŀ�����2",		ID_KPCTRL_SCLTRAN_DEST2 },
	{ "���ɼ��2",		ID_KPCTRL_SCLTRAN_TIME2 },
	{ "Ŀ�����3",		ID_KPCTRL_SCLTRAN_DEST3 },
	{ "���ɼ��3",		ID_KPCTRL_SCLTRAN_TIME3 },
	{ "Ŀ�����4",		ID_KPCTRL_SCLTRAN_DEST4 },
	{ "���ɼ��4",		ID_KPCTRL_SCLTRAN_TIME4 },
	{ "Ŀ�����5",		ID_KPCTRL_SCLTRAN_DEST5 },
	{ "���ɼ��5",		ID_KPCTRL_SCLTRAN_TIME5 },
};

static const GFX_PROP_INFO _ctrl_scale[] =
{
	CTRL_BASE_INFO
	{ "�����ı�ֵ",		ID_KPCTRL_SCALE_DELTA },
	{ "��С����",		ID_KPCTRL_SCALE_MIN },
	{ "������",		ID_KPCTRL_SCALE_MAX }
};

static const GFX_PROP_INFO _ctrl_noise_base[] =
{
	{ "�����С",		ID_KPCTRL_NOISE_BUFLEN },
	{ "���",			ID_KPCTRL_NOISE_AMP },
	{ "����",			ID_KPCTRL_NOISE_WAVLEN },
	{ "Persistence",	ID_KPCTRL_NOISE_PERSIST	},
	{ "�Ӳ���Ŀ",		ID_KPCTRL_NOISE_OCTNUM }
};

static const GFX_PROP_INFO _ctrl_color_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId },
	{ "����ɫ��",		ID_KPCTRL_NOISE_BASE_COLOR }
};

static const GFX_PROP_INFO _ctrl_scale_noise[] =
{
	CTRL_BASE_INFO
	{ _ctrl_noise_base[1].m_szName, _ctrl_noise_base[1].m_OpId },
	{ _ctrl_noise_base[2].m_szName, _ctrl_noise_base[2].m_OpId }
};

static const GFX_PROP_INFO _param_trail[] =
{
	{ "��ʼ������1",	ID_PARAM_TRAIL_POS1 },
	{ "��ʼ������2",	ID_PARAM_TRAIL_POS2	},
	{ "�任����",		ID_PARAM_TRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_trailex[] =
{
    { "��ʼ������1",	ID_PARAM_TRAIL_POS1 },
    { "��ʼ������2",	ID_PARAM_TRAIL_POS2	},
    { "�任����",		ID_PARAM_TRAIL_MATRIX }
};


static const GFX_PROP_INFO _param_ltntrail[] =
{
	{ "��ʼ������1",	ID_PARAM_LTNTRAIL_POS1 },
	{ "��ʼ������2",	ID_PARAM_LTNTRAIL_POS2	},
	{ "�任����",		ID_PARAM_LTNTRAIL_MATRIX }
};

static const GFX_PROP_INFO _param_ltn[] =
{
	{ "��ʼ������",		ID_PARAM_LTN_POS1 },
	{ "�յ�����",		ID_PARAM_LTN_POS2 }
};

static const GFX_PROP_INFO _param_ribbon[] =
{
	{ "��ʼ������1",	ID_PARAM_RIBBON_POS1 },
	{ "��ʼ������2",	ID_PARAM_RIBBON_POS2	},
	{ "�任����",		ID_PARAM_RIBBON_MATRIX }
};

static const GFX_OBJ_INFO _gfx_method_info[] =
{
	{ "��ת",		ID_KPCTRL_ROT,			_ctrl_rot,			PROP_COUNT(_ctrl_rot),			NULL,	NULL,	0 },
	{ "����ת��",	ID_KPCTRL_ROTAXIS,		_ctrl_rot_axis,		PROP_COUNT(_ctrl_rot_axis),		NULL,	NULL,	0 },
	{ "���ṫת",	ID_KPCTRL_REVOL,		_ctrl_revol,		PROP_COUNT(_ctrl_revol),		NULL,	NULL,	0 },
	{ "�����˶�",	ID_KPCTRL_CENTRI,		_ctrl_centri_move,	PROP_COUNT(_ctrl_centri_move),	NULL,	NULL,	0 },
	{ "ɫ�ʸı�",	ID_KPCTRL_COLOR,		_ctrl_color,		PROP_COUNT(_ctrl_color),		NULL,	NULL,	0 },
	{ "ɫ�ʹ���",	ID_KPCTRL_CL_TRANS,		_ctrl_cltrans,		PROP_COUNT(_ctrl_cltrans),		NULL,	NULL,	0 },
	{ "����ɫ��",	ID_KPCTRL_CL_NOISE,		_ctrl_color_noise,	PROP_COUNT(_ctrl_color_noise),	NULL,	NULL,	0 },
	{ "�����ı�",	ID_KPCTRL_SCALE,		_ctrl_scale,		PROP_COUNT(_ctrl_scale),		NULL,	NULL,	0 },
	{ "��������",	ID_KPCTRL_SCALE_TRANS,	_ctrl_scltrans,		PROP_COUNT(_ctrl_scltrans),		NULL,	NULL,	0 },
	{ "��������",	ID_KPCTRL_SCA_NOISE,	_ctrl_scale_noise,	PROP_COUNT(_ctrl_scale_noise),	NULL,	NULL,	0 },
	{ "ƽ��",		ID_KPCTRL_MOVE,			_ctrl_move,			PROP_COUNT(_ctrl_move),			NULL,	NULL,	0 }
};

static const GFX_OBJ_INFO _phys_particle_method_info[] =
{
	{ "ɫ�ʸı�",	ID_KPCTRL_COLOR,		_ctrl_color,		PROP_COUNT(_ctrl_color),		NULL,	NULL,	0 },
	{ "ɫ�ʹ���",	ID_KPCTRL_CL_TRANS,		_ctrl_cltrans,		PROP_COUNT(_ctrl_cltrans),		NULL,	NULL,	0 },
	{ "����ɫ��",	ID_KPCTRL_CL_NOISE,		_ctrl_color_noise,	PROP_COUNT(_ctrl_color_noise),	NULL,	NULL,	0 },
	{ "�����ı�",	ID_KPCTRL_SCALE,		_ctrl_scale,		PROP_COUNT(_ctrl_scale),		NULL,	NULL,	0 },
	{ "��������",	ID_KPCTRL_SCALE_TRANS,	_ctrl_scltrans,		PROP_COUNT(_ctrl_scltrans),		NULL,	NULL,	0 },
	{ "��������",	ID_KPCTRL_SCA_NOISE,	_ctrl_scale_noise,	PROP_COUNT(_ctrl_scale_noise),	NULL,	NULL,	0 },
};


static const GFX_OBJ_INFO _gfx_obj_info[] =
{
	{ "3D��Ƭ",			ID_ELE_TYPE_DECAL_3D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "2D��Ƭ",			ID_ELE_TYPE_DECAL_2D,			_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
    { "ʮ����Ƭ",		ID_ELE_TYPE_DECAL_BILLBOARD,	_prop_decal_info,		PROP_COUNT(_prop_decal_info),		NULL,	NULL,	0 },
	{ "������",			ID_ELE_TYPE_PARABOLOID,			_prop_parab_info,		PROP_COUNT(_prop_parab_info),		NULL,	NULL,	0 },
	{ "�켣",			ID_ELE_TYPE_TRAIL,				_prop_trail_info,		PROP_COUNT(_prop_trail_info),		NULL,	NULL,	0 },
#ifdef _ANGELICA21
    { "�¹켣",			ID_ELE_TYPE_TRAIL_EX,			_prop_trailex_info,		PROP_COUNT(_prop_trailex_info),		NULL,	NULL,	0 },
#endif
	{ "����켣",		ID_ELE_TYPE_LTNTRAIL,			_prop_ltntrail_info,	PROP_COUNT(_prop_ltntrail_info),	NULL,	NULL,	0 },
	{ "��������",		ID_ELE_TYPE_PARTICLE_POINT,		_prop_pointpar_info,	PROP_COUNT(_prop_pointpar_info),	"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "��������",		ID_ELE_TYPE_PARTICLE_BOX,		_prop_boxpar_info,		PROP_COUNT(_prop_boxpar_info),		"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "��������",		ID_ELE_TYPE_PARTICLE_MULTIPLANE,_prop_multiplane_info,	PROP_COUNT(_prop_multiplane_info),	"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "��������",		ID_ELE_TYPE_PARTICLE_ELLIPSOID,	_prop_ellipsoidpar_info,	PROP_COUNT(_prop_ellipsoidpar_info),		"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "Բ������",		ID_ELE_TYPE_PARTICLE_CYLINDER,	_prop_cylinder_info,	PROP_COUNT(_prop_cylinder_info),	"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "��������",		ID_ELE_TYPE_PARTICLE_CURVE,		_prop_curve_info,		PROP_COUNT(_prop_curve_info),		"����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "��������",		ID_ELE_TYPE_PARTICLE_SKELETON,	_prop_skeleton_par_info,PROP_COUNT(_prop_skeleton_par_info), "����Ч��Ӱ��������",	_gfx_method_info,	OBJ_COUNT(_gfx_method_info) },
	{ "�ƹ�Ч��",		ID_ELE_TYPE_LIGHT,				_prop_light_info,		PROP_COUNT(_prop_light_info),		NULL,	NULL,	0 },
	{ "Բ��",			ID_ELE_TYPE_RING,				_prop_ring_info,		PROP_COUNT(_prop_ring_info),		NULL,	NULL,	0 },
	{ "�绡",			ID_ELE_TYPE_LIGHTNING,			_prop_ln_info,			PROP_COUNT(_prop_ln_info),			NULL,	NULL,	0 },
	{ "����",			ID_ELE_TYPE_LTNBOLT,			_prop_bolt_info,		PROP_COUNT(_prop_bolt_info),		NULL,	NULL,	0 },
	{ "ģ��",			ID_ELE_TYPE_MODEL,				_prop_model_info,		PROP_COUNT(_prop_model_info),		NULL,	NULL,	0 },
	{ "����",			ID_ELE_TYPE_SOUND,				_prop_sound_info,		PROP_COUNT(_prop_sound_info),		NULL,	NULL,	0 },
	{ "��Ч��",			ID_ELE_TYPE_GFX_CONTAINER,		_prop_container_info,	PROP_COUNT(_prop_container_info),	NULL,	NULL,	0 },
	{ "Grid3D��Ƭ",		ID_ELE_TYPE_GRID_DECAL_3D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "Grid2D��Ƭ",		ID_ELE_TYPE_GRID_DECAL_2D,		_prop_griddecal_info,	PROP_COUNT(_prop_griddecal_info),	NULL,	NULL,	0 },
	{ "��������",		ID_ELE_TYPE_PHYS_EMITTER,		_prop_physemitter_info,	PROP_COUNT(_prop_physemitter_info),	"��������Ч��Ӱ��������",	_phys_particle_method_info,	OBJ_COUNT(_phys_particle_method_info) },
	{ "���������",		ID_ELE_TYPE_PHYS_POINTEMITTER,	_prop_physpointemitter_info, PROP_COUNT(_prop_physpointemitter_info), NULL, NULL, 0},
	{ "�߼��绡",		ID_ELE_TYPE_LIGHTNINGEX,		_prop_ln_ex_info,		PROP_COUNT(_prop_ln_ex_info),		NULL,	NULL,	0},
	{ "ECModel",		ID_ELE_TYPE_ECMODEL,			_prop_ecmodel_info,		PROP_COUNT(_prop_ecmodel_info),		NULL,	NULL,	0},
	{ "ģ�ʹ���",		ID_ELE_TYPE_MODEL_PROXY,		_prop_model_proxy_info,	PROP_COUNT(_prop_model_proxy_info),	NULL,	NULL,	0},
	{ "Ʈ��",			ID_ELE_TYPE_RIBBON,				_prop_ribbon_info,	PROP_COUNT(_prop_ribbon_info),			NULL,	NULL,	0},
};

#ifdef _ANGELICA22
static const GFX_OBJ_INFO _gfx_audio_info_[] =
{
	{ "����",			ID_ELE_TYPE_SOUND,				_prop_audioevent_info,		PROP_COUNT(_prop_audioevent_info),		NULL,	NULL,	0 },
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
	{ "�����ò�ֵ",		A3DGFXKeyPoint::enumInterpNone },
	{ "���ò�ֵ",		A3DGFXKeyPoint::enumInterpAll },
	{ "�����Է����ֵ",	A3DGFXKeyPoint::enumInterpNoDir }
};

static const GFX_PROP_INFO _prop_kp_info[] =
{
	{ "���ĵ�λ��",		ID_GFXOP_KP_POS},
	{ "���߷���",		ID_GFXOP_KP_DIR},
	{ "��ɫ",			ID_GFXOP_KP_COLOR},
	{ "����",			ID_GFXOP_KP_SCALE},
	{ "2D��ת�Ƕ�",		ID_GFXOP_KP_RAD2D},
	{ "����ʱ��",		ID_GFXOP_KP_INFINITE},
	{ "��ֵ��ʽ",		ID_GFXOP_KP_INTERP},
	{ "����ʱ��",		ID_GFXOP_KP_TIMESPAN},
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

