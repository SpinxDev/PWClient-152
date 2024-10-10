/*
 * FILE: A3DTypes.h
 *
 * DESCRIPTION: Fundermental data types for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTYPES_H_
#define _A3DTYPES_H_

#include "A3DPlatform.h"

#include "ABaseDef.h"
#include "A3DVector.h"
#include "A3DMatrix.h"
#include "A3DQuaternion.h"
#include "ARect.h"

typedef long		A3DRESULT;	//	Return code data type;
typedef DWORD		A3DCOLOR;	//	Color

typedef APointI		A3DPOINT2;
typedef ARectI		A3DRECT;
typedef DWORD		HA3DFONT;

#define MAX_TEX_LAYERS		8
#define MAX_SAMPLE_LAYERS	16
#define VS_SAMPLE_LAYERS_NUM 4
#define MIN_VS_SAMPLE_LAYERS D3DVERTEXTEXTURESAMPLER0
#define MAX_VS_SAMPLE_LAYERS D3DVERTEXTEXTURESAMPLER3
#define MAX_CLIP_PLANES		6	//	D3DMAXUSERCLIPPLANES = 32, but 6 is enough for us ?

//	Color value
class A3DCOLORVALUE
{
public:		//	Constructors and Destructors

	A3DCOLORVALUE() {}
	A3DCOLORVALUE(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }
	A3DCOLORVALUE(float c) { r=c; g=c; b=c; a=c; }
	A3DCOLORVALUE(const A3DCOLORVALUE& v) { r=v.r; g=v.g; b=v.b; a=v.a; }
	A3DCOLORVALUE(A3DCOLOR Color);

public:		//	Attributes

	float r, g, b, a;

public:		//	Operations

	//	Operator *
	friend A3DCOLORVALUE operator * (const A3DCOLORVALUE& v, float s) { return A3DCOLORVALUE(v.r * s, v.g * s, v.b * s, v.a * s); }
	friend A3DCOLORVALUE operator * (float s, const A3DCOLORVALUE& v) { return A3DCOLORVALUE(v.r * s, v.g * s, v.b * s, v.a * s); }
	friend A3DCOLORVALUE operator * (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r*v2.r, v1.g*v2.g, v1.b*v2.b, v1.a*v2.a); }
	//	Operator + and -
	friend A3DCOLORVALUE operator + (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r+v2.r, v1.g+v2.g, v1.b+v2.b, v1.a+v2.a); }
	friend A3DCOLORVALUE operator - (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r-v2.r, v1.g-v2.g, v1.b-v2.b, v1.a-v2.a); }
	//	Operator != and ==
	friend bool operator != (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return (v1.r!=v2.r || v1.g!=v2.g || v1.b!=v2.b || v1.a!=v2.a); }
	friend bool operator == (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return (v1.r==v2.r && v1.g==v2.g && v1.b==v2.b && v1.a==v2.a); }
	//	Operator *=
	const A3DCOLORVALUE& operator *= (float s) { r*=s; g*=s; b*=s; a*=s; return *this; }
	const A3DCOLORVALUE& operator *= (const A3DCOLORVALUE& v) { r*=v.r; g*=v.g; b*=v.b; a*=v.a; return *this; }
	//	Operator += and -=
	const A3DCOLORVALUE& operator += (const A3DCOLORVALUE& v) { r+=v.r; g+=v.g; b+=v.b; a+=v.a; return *this; }
	const A3DCOLORVALUE& operator -= (const A3DCOLORVALUE& v) { r-=v.r; g-=v.g; b-=v.b; a-=v.a; return *this; }
	//	Operator =
	const A3DCOLORVALUE& operator = (const A3DCOLORVALUE& v) { r=v.r; g=v.g; b=v.b; a=v.a; return *this; }
	const A3DCOLORVALUE& operator = (A3DCOLOR Color);

	//	Set value
	void Set(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }

	//	Clamp values
	void ClampRoof() { if (r>1.0f) r=1.0f; if (g>1.0f) g=1.0f; if (b>1.0f) b=1.0f; if (a>1.0f) a=1.0f; }
	void ClampFloor() { if (r<0.0f) r=0.0f; if (g<0.0f) g=0.0f; if (b<0.0f) b=0.0f; if (a<0.0f) a=0.0f; }
	void Clamp() 
	{
		if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
		if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
		if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;
		if (a > 1.0f) a = 1.0f; else if (a < 0.0f) a = 0.0f;
	}

	//	Convert to A3DCOLOR
	A3DCOLOR ToRGBAColor();
};

struct A3DHSVCOLORVALUE
{
	FLOAT	h;
	FLOAT	s;
	FLOAT	v;
	FLOAT	a;
public:
	A3DHSVCOLORVALUE() {}
	inline A3DHSVCOLORVALUE(FLOAT _h, FLOAT _s, FLOAT _v, FLOAT _a) { h = _h; s = _s; v = _v; a = _a; }
	inline A3DHSVCOLORVALUE(FLOAT c) {h = c; s = c; v = c; a = c;	}
};

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */


//Device Material and Texture Surface related structures;
enum A3DFORMAT
{
	A3DFMT_UNKNOWN              =  0,

	A3DFMT_R8G8B8               = D3DFMT_R8G8B8,
	A3DFMT_A8R8G8B8             = D3DFMT_A8R8G8B8,
	A3DFMT_X8R8G8B8             = D3DFMT_X8R8G8B8,
	A3DFMT_R5G6B5               = D3DFMT_R5G6B5,
	A3DFMT_X1R5G5B5             = D3DFMT_X1R5G5B5,
	A3DFMT_A1R5G5B5             = D3DFMT_A1R5G5B5,
	A3DFMT_A4R4G4B4             = D3DFMT_A4R4G4B4,
	A3DFMT_R3G3B2               = D3DFMT_R3G3B2,
	A3DFMT_A8                   = D3DFMT_A8,
	A3DFMT_A8R3G3B2             = D3DFMT_A8R3G3B2,
	A3DFMT_X4R4G4B4             = D3DFMT_X4R4G4B4,
	A3DFMT_A2B10G10R10          = D3DFMT_A2B10G10R10,
	A3DFMT_A8B8G8R8             = D3DFMT_A8B8G8R8,
	A3DFMT_X8B8G8R8             = D3DFMT_X8B8G8R8,
	A3DFMT_G16R16               = D3DFMT_G16R16,
	A3DFMT_A2R10G10B10          = D3DFMT_A2R10G10B10,
	A3DFMT_A16B16G16R16         = D3DFMT_A16B16G16R16,

	A3DFMT_A8P8					= D3DFMT_A8P8,
	A3DFMT_P8                   = D3DFMT_P8,

	A3DFMT_L8                   = D3DFMT_L8,
	A3DFMT_A8L8                 = D3DFMT_A8L8,
	A3DFMT_A4L4                 = D3DFMT_A4L4,

	A3DFMT_V8U8                 = D3DFMT_V8U8,
	A3DFMT_L6V5U5               = D3DFMT_L6V5U5,
	A3DFMT_X8L8V8U8             = D3DFMT_X8L8V8U8,
	A3DFMT_Q8W8V8U8             = D3DFMT_Q8W8V8U8,
	A3DFMT_V16U16               = D3DFMT_V16U16,
	A3DFMT_A2W10V10U10          = D3DFMT_A2W10V10U10,

	A3DFMT_UYVY                 = D3DFMT_UYVY,
	A3DFMT_R8G8_B8G8            = D3DFMT_R8G8_B8G8,
	A3DFMT_YUY2                 = D3DFMT_YUY2,
	A3DFMT_G8R8_G8B8            = D3DFMT_G8R8_G8B8,
	A3DFMT_DXT1                 = D3DFMT_DXT1,
	A3DFMT_DXT2                 = D3DFMT_DXT2,
	A3DFMT_DXT3                 = D3DFMT_DXT3,
	A3DFMT_DXT4                 = D3DFMT_DXT4,
	A3DFMT_DXT5                 = D3DFMT_DXT5,

	A3DFMT_D16_LOCKABLE         = D3DFMT_D16_LOCKABLE,
	A3DFMT_D32                  = D3DFMT_D32,
	A3DFMT_D15S1                = D3DFMT_D15S1,
	A3DFMT_D24S8                = D3DFMT_D24S8,
	A3DFMT_D24X8                = D3DFMT_D24X8,
	A3DFMT_D24X4S4              = D3DFMT_D24X4S4,
	A3DFMT_D16                  = D3DFMT_D16,

	A3DFMT_D32F_LOCKABLE        = D3DFMT_D32F_LOCKABLE,
	A3DFMT_D24FS8               = D3DFMT_D24FS8,

	A3DFMT_D32_LOCKABLE			= D3DFMT_D32_LOCKABLE,
	A3DFMT_S8_LOCKABLE			= D3DFMT_S8_LOCKABLE,

	A3DFMT_L16                  = D3DFMT_L16,

	A3DFMT_VERTEXDATA           = D3DFMT_VERTEXDATA,
	A3DFMT_INDEX16              = D3DFMT_INDEX16,
	A3DFMT_INDEX32              = D3DFMT_INDEX32,

	A3DFMT_Q16W16V16U16         = D3DFMT_Q16W16V16U16,

	A3DFMT_MULTI2_ARGB8         = D3DFMT_MULTI2_ARGB8,

	// s10e5 formats (16-bits per channel)
	A3DFMT_R16F                 = D3DFMT_R16F,
	A3DFMT_G16R16F              = D3DFMT_G16R16F,
	A3DFMT_A16B16G16R16F        = D3DFMT_A16B16G16R16F,

	// IEEE s23e8 formats (32-bits per channel)
	A3DFMT_R32F                 = D3DFMT_R32F,
	A3DFMT_G32R32F              = D3DFMT_G32R32F,
	A3DFMT_A32B32G32R32F        = D3DFMT_A32B32G32R32F,
	A3DFMT_CxV8U8               = D3DFMT_CxV8U8,

	A3DFMT_A1					= D3DFMT_A1,
	A3DFMT_BINARYBUFFER			= D3DFMT_BINARYBUFFER,

	A3DFMT_RAWZ					= (MAKEFOURCC('R', 'A', 'W', 'Z')),
	A3DFMT_INTZ					= (MAKEFOURCC('I', 'N', 'T', 'Z')),
	A3DFMT_NULL					= (MAKEFOURCC('N', 'U', 'L', 'L')),

	A3DFMT_FORCE_DWORD          = D3DFMT_FORCE_DWORD,
};

enum A3DDEVTYPE
{
    A3DDEVTYPE_HAL         = 1,
    A3DDEVTYPE_REF         = 2,
    A3DDEVTYPE_SW          = 3,

    A3DDEVTYPE_FORCE_DWORD = 0xffffffff
};

struct A3DDEVFMT
{
	A3DDEVFMT() : bWindowed(true), nWidth(800), nHeight(600), fmtAdapter(A3DFMT_UNKNOWN), fmtTarget(A3DFMT_UNKNOWN), bVSync(false), bSndGlobal(false) {}

	bool		bWindowed;
	int			nWidth;
	int			nHeight;
	A3DFORMAT	fmtAdapter;
	A3DFORMAT	fmtTarget;
	A3DFORMAT	fmtDepth;
	bool		bVSync;
	bool		bSndGlobal;
};

enum A3DBLEND
{
    A3DBLEND_ZERO               =  1,
    A3DBLEND_ONE                =  2,
    A3DBLEND_SRCCOLOR           =  3,
    A3DBLEND_INVSRCCOLOR        =  4,
    A3DBLEND_SRCALPHA           =  5,
    A3DBLEND_INVSRCALPHA        =  6,
    A3DBLEND_DESTALPHA          =  7,
    A3DBLEND_INVDESTALPHA       =  8,
    A3DBLEND_DESTCOLOR          =  9,
    A3DBLEND_INVDESTCOLOR       = 10,
    A3DBLEND_SRCALPHASAT        = 11,
    A3DBLEND_BOTHSRCALPHA       = 12,
    A3DBLEND_BOTHINVSRCALPHA    = 13,

    A3DBLEND_FORCE_DWORD        = 0x7fffffff
};

struct A3DMATERIALPARAM
{
    A3DCOLORVALUE   Diffuse;
    A3DCOLORVALUE   Ambient;
    A3DCOLORVALUE   Specular;
    A3DCOLORVALUE   Emissive;
    FLOAT           Power;
};

enum A3DCMPFUNC
{
    A3DCMP_NEVER                = 1,
    A3DCMP_LESS                 = 2,
    A3DCMP_EQUAL                = 3,
    A3DCMP_LESSEQUAL            = 4,
    A3DCMP_GREATER              = 5,
    A3DCMP_NOTEQUAL             = 6,
    A3DCMP_GREATEREQUAL         = 7,
    A3DCMP_ALWAYS               = 8,

    A3DCMP_FORCE_DWORD          = 0x7fffffff
};

//Light Param;
enum A3DLIGHTTYPE
{
    A3DLIGHT_POINT          = 1,
    A3DLIGHT_SPOT           = 2,
    A3DLIGHT_DIRECTIONAL    = 3,

	A3DLIGHT_AMBIENT		= 100000,
    A3DLIGHT_FORCE_DWORD    = 0x7fffffff
};

struct A3DLIGHTPARAM
{
    A3DLIGHTTYPE    Type;
    A3DCOLORVALUE   Diffuse;
    A3DCOLORVALUE   Specular;
    A3DCOLORVALUE   Ambient;
    A3DVECTOR3      Position;
    A3DVECTOR3      Direction;
    FLOAT           Range;
    FLOAT           Falloff;
    FLOAT           Attenuation0;
    FLOAT           Attenuation1;
    FLOAT           Attenuation2;
    FLOAT           Theta;
    FLOAT           Phi;
};

enum A3DPRIMITIVETYPE
{
    A3DPT_POINTLIST       = 1,
    A3DPT_LINELIST        = 2,
    A3DPT_LINESTRIP       = 3,
    A3DPT_TRIANGLELIST    = 4,
    A3DPT_TRIANGLESTRIP   = 5,
    A3DPT_TRIANGLEFAN     = 6,

    A3DPT_FORCE_DWORD     = 0x7fffffff
};

//Viewport Parameters;
struct A3DVIEWPORTPARAM
{	
    DWORD	X;
    DWORD	Y;
    DWORD	Width;
    DWORD	Height;
    FLOAT	MinZ;
    FLOAT	MaxZ;
};

//Face Culling;
enum A3DCULLTYPE
{
	A3DCULL_NONE	= 1,
	A3DCULL_CW		= 2,
	A3DCULL_CCW		= 3
};

//Fill Mode;
enum A3DFILLMODE
{
    A3DFILL_POINT        = 1,
    A3DFILL_WIREFRAME    = 2,
    A3DFILL_SOLID        = 3,
};

//Filter Type;
enum A3DTEXTUREFILTERTYPE
{
    A3DTEXF_NONE            = 0,
    A3DTEXF_POINT           = 1,
    A3DTEXF_LINEAR          = 2,
    A3DTEXF_ANISOTROPIC     = 3,
    A3DTEXF_FLATCUBIC       = 4,
    A3DTEXF_GAUSSIANCUBIC   = 5,

    A3DTEXF_FORCE_DWORD     = 0x7fffffff
};

//Texture operation type
enum A3DTEXTUREOP
{
    A3DTOP_DISABLE                   =  1,
    A3DTOP_SELECTARG1                =  2,
    A3DTOP_SELECTARG2                =  3,
    A3DTOP_MODULATE                  =  4,
    A3DTOP_MODULATE2X                =  5,
    A3DTOP_MODULATE4X                =  6,
    A3DTOP_ADD                       =  7,
    A3DTOP_ADDSIGNED                 =  8,
    A3DTOP_ADDSIGNED2X               =  9,
    A3DTOP_SUBTRACT                  = 10,
    A3DTOP_ADDSMOOTH                 = 11,
    A3DTOP_BLENDDIFFUSEALPHA         = 12,
    A3DTOP_BLENDTEXTUREALPHA         = 13,
    A3DTOP_BLENDFACTORALPHA          = 14,
    A3DTOP_BLENDTEXTUREALPHAPM       = 15,
    A3DTOP_BLENDCURRENTALPHA         = 16,
    A3DTOP_PREMODULATE               = 17,
    A3DTOP_MODULATEALPHA_ADDCOLOR    = 18,
    A3DTOP_MODULATECOLOR_ADDALPHA    = 19,
    A3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
    A3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
    A3DTOP_BUMPENVMAP                = 22,
    A3DTOP_BUMPENVMAPLUMINANCE       = 23,
    A3DTOP_DOTPRODUCT3               = 24,
    A3DTOP_MULTIPLYADD               = 25,
    A3DTOP_LERP                      = 26,

    A3DTOP_FORCE_DWORD               = 0x7fffffff,
};
/* Multi-Sample buffer types */
enum A3DMULTISAMPLE_TYPE
{
	A3DMULTISAMPLE_NONE			= D3DMULTISAMPLE_NONE,
	A3DMULTISAMPLE_2_SAMPLES	= D3DMULTISAMPLE_2_SAMPLES,
	A3DMULTISAMPLE_3_SAMPLES    = D3DMULTISAMPLE_3_SAMPLES,
	A3DMULTISAMPLE_4_SAMPLES    = D3DMULTISAMPLE_4_SAMPLES,
	A3DMULTISAMPLE_5_SAMPLES    = D3DMULTISAMPLE_5_SAMPLES,
	A3DMULTISAMPLE_6_SAMPLES    = D3DMULTISAMPLE_6_SAMPLES,
	A3DMULTISAMPLE_7_SAMPLES    = D3DMULTISAMPLE_7_SAMPLES,
	A3DMULTISAMPLE_8_SAMPLES    = D3DMULTISAMPLE_8_SAMPLES,
	A3DMULTISAMPLE_9_SAMPLES    = D3DMULTISAMPLE_9_SAMPLES,
	A3DMULTISAMPLE_10_SAMPLES   = D3DMULTISAMPLE_10_SAMPLES,
	A3DMULTISAMPLE_11_SAMPLES   = D3DMULTISAMPLE_11_SAMPLES,
	A3DMULTISAMPLE_12_SAMPLES   = D3DMULTISAMPLE_12_SAMPLES,
	A3DMULTISAMPLE_13_SAMPLES   = D3DMULTISAMPLE_13_SAMPLES,
	A3DMULTISAMPLE_14_SAMPLES   = D3DMULTISAMPLE_14_SAMPLES,
	A3DMULTISAMPLE_15_SAMPLES   = D3DMULTISAMPLE_15_SAMPLES,
	A3DMULTISAMPLE_16_SAMPLES   = D3DMULTISAMPLE_16_SAMPLES,
	A3DMULTISAMPLE_FORCE_DWORD	= D3DMULTISAMPLE_FORCE_DWORD,
};

enum A3DFOGMODE
{
	A3DFOG_NONE			= 0,
	A3DFOG_EXP			= 1,
	A3DFOG_EXP2			= 2,
	A3DFOG_LINEAR		= 3,
	A3DFOG_FORCE_DWORD	= 0x7fffffff
} ; 

//	Texture addressing method
enum A3DTEXTUREADDR
{
	A3DTADDR_WRAP		= 1,	//	Wrap
	A3DTADDR_MIRROR		= 2,	//	Mirror
	A3DTADDR_CLAMP		= 3,	//	Clamp
	A3DTADDR_BORDER		= 4,	//	Border
	A3DTADDR_MIRRORONCE	= 5		//	Mirror once
};

//	Texture transform flags
enum A3DTEXTURETRANSFLAGS
{
	A3DTTFF_DISABLE		= 0,	//	Disable
	A3DTTFF_COUNT1		= 1,	//	Expect 1-D texture coordinates
	A3DTTFF_COUNT2		= 2,	//	Expect 2-D texture coordinates
	A3DTTFF_COUNT3		= 3,	//	Expect 3-D texture coordinates
	A3DTTFF_COUNT4		= 4,	//	Expect 4-D texture coordinates
	A3DTTFF_PROJECTED	= 256
};

//	Texture's color and alpha operation arguments and modifiers
//	Arguments
#define	A3DTA_SELECTMASK		0x0f	//	Mask for arg selector
#define	A3DTA_DIFFUSE			0x00	//	Select diffuse color
#define	A3DTA_CURRENT			0x01	//	Select stage destination register
#define	A3DTA_TEXTURE			0x02	//	Select texture color
#define	A3DTA_TFACTOR			0x03	//	Select RENDERSTATE_TEXTUREFACTOR
#define	A3DTA_SPECULAR			0x04	//	Select specular color
#define	A3DTA_TEMP				0x05	//	Select temporary register color
	//	Modifiers
#define	A3DTA_COMPLEMENT		0x10	//	Take 1.0 - x
#define	A3DTA_ALPHAREPLICATE	0x20	//	Replicate alpha to color components

//	Material type
enum A3DMATERIALTYPE
{
	A3DMTL_CONCRETE = 0,
	A3DMTL_WOOD,
	A3DMTL_METALSOLID,
	A3DMTL_METALBARREL,
	A3DMTL_GLASS,
	A3DMTL_RUBBER,
	A3DMTL_FLESH,
	A3DMTL_WATER,
	A3DMTL_EARTH,
};

//	This struct is out of date and should be replaced by A3DMESHPROP. 
//	It's only used for loading old version .mox file
struct A3DMESH_PROP
{
	bool			b2Sided;
	bool			bCanShootThrough;
	bool			bNoMark;
	bool			bCanPickThrough;
	A3DMATERIALTYPE	nMaterialType;
	bool			bOnGround;
	bool			bUsedForPVS;
	char			foo[30];//This is used to make some reserved room for expanding new proerpty;
};

class A3DMESHPROP
{
public:		//	Types

	//	Property flags
	enum
	{
		FG_2SIDES			= 0x0001,		//	Two sides
		FG_SHOOTTHROUGH		= 0x0002,		//	Can be shoot through
		FG_NOMARK			= 0x0004,		//	No mark
		FG_PICKTHROUGH		= 0x0008,		//	Can be picked through
		FG_ONGROUND			= 0x0010,		//	On ground flag used by BSP
		FG_BSPPVS			= 0x0020,		//	Used for BSP PVS calculation
		FG_NOTRENDER		= 0x0040,		//	Not render
		FG_WALKIGNORE		= 0x0080,		//	Ignore when character walks
		FG_NOLYFORWALK		= 0x0100,		//	Only used for character walking
	};

public:		//	Constructors and Destructors

	A3DMESHPROP() { dwProps = 0; MtlType = A3DMTL_CONCRETE; }
	A3DMESHPROP(DWORD _dwProps, A3DMATERIALTYPE _MtlType) { dwProps = _dwProps; MtlType = _MtlType; }
	A3DMESHPROP(const A3DMESHPROP& mp) { dwProps = mp.dwProps; MtlType = mp.MtlType; }

public:		//	Attributes

	DWORD			dwProps;
	A3DMATERIALTYPE	MtlType;

public:		//	Operations

	//	== operator
	friend bool operator == (const A3DMESHPROP& mp1, const A3DMESHPROP& mp2) { return mp1.dwProps == mp2.dwProps && mp1.MtlType == mp2.MtlType; }
	//	!= operator
	friend bool operator != (const A3DMESHPROP& mp1, const A3DMESHPROP& mp2) { return mp1.dwProps != mp2.dwProps || mp1.MtlType != mp2.MtlType; }

	//	= operator
	const A3DMESHPROP& operator = (const A3DMESHPROP& mp) { dwProps = mp.dwProps; MtlType = mp.MtlType; return *this; }

	void Set2SidesFlag(bool bTrue) { if (bTrue) dwProps |= FG_2SIDES; else dwProps &= ~FG_2SIDES; }
	bool Get2SidesFlag() const { return (dwProps & FG_2SIDES) ? true : false; }
	void SetShootThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_SHOOTTHROUGH; else dwProps &= ~FG_SHOOTTHROUGH; }
	bool GetShootThroughFlag() const { return (dwProps & FG_SHOOTTHROUGH) ? true : false; }
	void SetNoMarkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOMARK; else dwProps &= ~FG_NOMARK; }
	bool GetNoMarkFlag() const { return (dwProps & FG_NOMARK) ? true : false; }
	void SetPickThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_PICKTHROUGH; else dwProps &= ~FG_PICKTHROUGH; }
	bool GetPickThroughFlag() const { return (dwProps & FG_PICKTHROUGH) ? true : false; }
	void SetOnGroundFlag(bool bTrue) { if (bTrue) dwProps |= FG_ONGROUND; else dwProps &= ~FG_ONGROUND; }
	bool GetOnGroundFlag() const { return (dwProps & FG_ONGROUND) ? true : false; }
	void SetBSPPVSFlag(bool bTrue) { if (bTrue) dwProps |= FG_BSPPVS; else dwProps &= ~FG_BSPPVS; }
	bool GetBSPPVSFlag() const { return (dwProps & FG_BSPPVS) ? true : false; }
	void SetNotRenderFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOTRENDER; else dwProps &= ~FG_NOTRENDER; }
	bool GetNotRenderFlag() const { return (dwProps & FG_NOTRENDER) ? true : false; }
	void SetWalkIgnoreFlag(bool bTrue) { if (bTrue) dwProps |= FG_WALKIGNORE; else dwProps &= ~FG_WALKIGNORE; }
	bool GetWalkIgnoreFlag() const { return (dwProps & FG_WALKIGNORE) ? true : false; }
	void SetOnlyForWalkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOLYFORWALK; else dwProps &= ~FG_NOLYFORWALK; }
	bool GetOnlyForWalkFlag() const { return (dwProps & FG_NOLYFORWALK) ? true : false; }
};

enum A3DTRANSPARENT_MODE 
{
	A3DTRANSPARENT_COLORKEY = 0,
	A3DTRANSPARENT_COLORALPHA,
	A3DTRANSPARENT_SRCALPHA_DESTINVSRCALPHA,
	A3DTRANSPARENT_SRCONE_DESTINVSRCALPHA,
	A3DTRANSPARENT_SRCALPHA_DESTONE,
};

struct A3DSURFACE_PARAM
{
	//Output param;
	int		width;
	int		height;
};

struct A3DSHADER
{
	A3DBLEND	SrcBlend;
	A3DBLEND	DestBlend;
};

enum A3DCONTAINER
{
	A3DCONTAINER_NULL = -1,
	A3DCONTAINER_WORLD_OBJECTLIST = 0,
	A3DCONTAINER_WORLD_BUILDINGLIST = 1,
	A3DCONTAINER_ENGINE = 2
};

enum A3DVERTEXBLENDFLAGS
{
	A3DVBF_DISABLE		= 0,
    A3DVBF_1WEIGHTS		= 1,
    A3DVBF_2WEIGHTS		= 2,
    A3DVBF_3WEIGHTS		= 3,
    A3DVBF_TWEENING		= 255,
    A3DVBF_0WEIGHTS		= 256,
};

struct A3DIBLLIGHTPARAM
{
	A3DVECTOR3		vecLightDir;
	A3DCOLORVALUE	clLightDirect;
	A3DCOLOR		clLightAmbient;
	A3DLIGHTPARAM	dynamicLightParam;
};

enum A3DTEXCOORDINDEX
{
	A3DTCI_PASSTHRU                     = 0x00000000,
	A3DTCI_CAMERASPACENORMAL            = 0x00010000,
	A3DTCI_CAMERASPACEPOSITION          = 0x00020000,
	A3DTCI_CAMERASPACEREFLECTIONVECTOR  = 0x00030000,
};

struct A3DCOMMCONSTTABLE
{
	int cbTable;
};

#endif	//	_A3DTYPES_H_
