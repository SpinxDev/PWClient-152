#ifndef		_A3DGFXEDITORINTERFACE_H_
#define		_A3DGFXEDITORINTERFACE_H_

#include "A3DVector.h"
#include "AString.h"
#include "A3DTypes.h"
#include "A3DQuaternion.h"

#pragma warning(error: 4018)

inline float _UnitRandom() { return rand() / (float)RAND_MAX; }
inline float _SymmetricRandom() { return _UnitRandom() * 2.0f - 1.0f; }

#define		PROP_INTERFACE(type)	operator type() const { return *((type*)&m_Data); }

enum GfxValueType
{
	GFX_VALUE_UNKNOWN,
	GFX_VALUE_INT,
	GFX_VALUE_COLOR,
	GFX_VALUE_BOOL,
	GFX_VALUE_FLOAT,
	GFX_VALUE_VECTOR3,
	GFX_VALUE_STRING,
	GFX_VALUE_PATH_TEX,
	GFX_VALUE_PATH_MOD,
	GFX_VALUE_PATH_SOUND,
	GFX_VALUE_SHADER_MODE,
	GFX_VALUE_MATRIX4,
	GFX_VALUE_QUATERNION,
	GFX_VALUE_PATH_GFX,
	GFX_VALUE_RENDER_LAYER,
	GFX_VALUE_PATH_MOX,
	GFX_VALUE_RENDER_SIDE,
	GFX_VALUE_LIGHTNING_FILTER,
	GFX_VALUE_LIGHT_TYPE,
	GFX_VALUE_PATH_FILE,
	GFX_VALUE_ECM_LDTYPE,
	GFX_VALUE_PHYSPAR_ORIENT,
	GFX_VALUE_PHYSPAR_SHAPE,
	GFX_VALUE_RAND_STRING,
	GFX_VALUE_RAND_PATH_SOUND,
	GFX_VALUE_SHADER_PATH,
	GFX_VALUE_TRAIL_TYPE,
	GFX_VALUE_PATH_AUDIOEVENT,
	GFX_VALUE_ANIMABLE,
};

class RandStringContainer;
class A3DGFXAnimable;

struct GFX_PROPERTY
{
	GfxValueType	m_Type;
	AString			m_string;

	union
	{
		int				i;
		bool			b;
		float			f;
		unsigned long	c;
		RandStringContainer* pRSC;
		struct {
			float x;
			float y;
			float z;
		} vec;
		char			mat[sizeof(A3DMATRIX4)];
		char			quad[sizeof(A3DQUATERNION)];
		A3DGFXAnimable*		pAnimable;
	} m_Data;

	GFX_PROPERTY()					{ m_Type = GFX_VALUE_UNKNOWN; }
	GFX_PROPERTY(int _i)			{ m_Type = GFX_VALUE_INT; m_Data.i = _i; }
	GFX_PROPERTY(bool _b)			{ m_Type = GFX_VALUE_BOOL; m_Data.b = _b; }
	GFX_PROPERTY(float _f)			{ m_Type = GFX_VALUE_FLOAT; m_Data.f = _f; }
	GFX_PROPERTY(unsigned long _c)	{ m_Type = GFX_VALUE_COLOR; m_Data.c = _c; }
	GFX_PROPERTY(RandStringContainer* _pRSC) { m_Type = GFX_VALUE_RAND_STRING; m_Data.pRSC = _pRSC; }
	GFX_PROPERTY(const A3DVECTOR3& _vec)
	{ 
		m_Type = GFX_VALUE_VECTOR3;
		m_Data.vec.x = _vec.x;
		m_Data.vec.y = _vec.y;
		m_Data.vec.z = _vec.z;
	}
	GFX_PROPERTY(const AString& str, GfxValueType type = GFX_VALUE_STRING)
	{ 
		m_Type = type;
		m_string = str;
	}
	GFX_PROPERTY(const A3DMATRIX4& mat)
	{
		m_Type = GFX_VALUE_MATRIX4;
		memcpy(m_Data.mat, &mat, sizeof(mat));
	}
	GFX_PROPERTY(const A3DQUATERNION& quad)
	{
		m_Type = GFX_VALUE_QUATERNION;
		memcpy(m_Data.quad, &quad, sizeof(quad));
	}
	explicit GFX_PROPERTY(A3DGFXAnimable* pAnimable) { m_Type = GFX_VALUE_ANIMABLE; m_Data.pAnimable = pAnimable; }
	// {{ begin modify

	// original

//	PROP_INTERFACE(int)
//	PROP_INTERFACE(float)
//	PROP_INTERFACE(unsigned long)

	// modified
	// handle the assignment between int and float type
	operator int () const
	{
		if (GetType() == GFX_VALUE_FLOAT)
			return int(m_Data.f);
		else if (GetType() == GFX_VALUE_COLOR)
			return int(m_Data.c);
		else // assume if (GetType() == GFX_VALUE_INT)
			return m_Data.i;
	}

	operator float () const
	{
		if (GetType() == GFX_VALUE_INT)
			return float(m_Data.i);
		else if (GetType() == GFX_VALUE_COLOR)
			return float(m_Data.c);
		else // assume if (GetType() == GFX_VALUE_FLOAT)
			return m_Data.f;
	}

	operator unsigned long () const
	{
		if (GetType() == GFX_VALUE_INT)
			return unsigned long(m_Data.i);
		else if (GetType() == GFX_VALUE_FLOAT)
			return unsigned long(m_Data.f);
		else // assume if (GetType() == GFX_VALUE_COLOR)
			return m_Data.c;
	}

	// }} end modify

	PROP_INTERFACE(bool)
	PROP_INTERFACE(A3DMATRIX4)
	PROP_INTERFACE(A3DQUATERNION)

	operator A3DVECTOR3() const { return A3DVECTOR3(m_Data.vec.x, m_Data.vec.y, m_Data.vec.z); }
	operator AString() const { return m_string;	}
	operator RandStringContainer*() const { return m_Data.pRSC; }

	GFX_PROPERTY& operator = (const GFX_PROPERTY& gp)
	{
		m_Type		=	gp.m_Type;
		m_string	=	gp.m_string;
		memcpy(&m_Data, &gp.m_Data, sizeof(m_Data));
		return *this;
	}
	GFX_PROPERTY& SetType(GfxValueType type) { m_Type = type; return *this;	}
	GfxValueType GetType() const { return m_Type; }

	// only valid when type is INT or FLOAT
	friend GFX_PROPERTY operator + (const GFX_PROPERTY &left, const GFX_PROPERTY &right);
	friend GFX_PROPERTY operator - (const GFX_PROPERTY &left, const GFX_PROPERTY &right);
	friend GFX_PROPERTY operator * (const GFX_PROPERTY &left, const GFX_PROPERTY &right);
	friend GFX_PROPERTY operator / (const GFX_PROPERTY &left, const GFX_PROPERTY &right);
};

bool operator == (const GFX_PROPERTY& gp1, const GFX_PROPERTY& gp2);
bool operator != (const GFX_PROPERTY& gp1, const GFX_PROPERTY& gp2);

struct GFX_PROP_INFO
{
	const char*			m_szName;
	int					m_OpId;
};

struct GFX_OBJ_INFO
{
	const char*			m_szName;
	int					m_nTypeId;
	const GFX_PROP_INFO*m_pPropInfo;
	int					m_nPropCount;
	const char*			m_szSubName;
	const GFX_OBJ_INFO*	m_pSubObjInfo;
	int					m_nSubCount;
};

struct GFX_PARAM_INFO
{
	int					m_nTypeId;
	const GFX_PROP_INFO*m_pPropInfo;
	int					m_nPropCount;
};

struct GFX_INTERP_INFO
{
	const char*			m_szName;
	int					m_nType;
};

struct GFX_SHADER_INFO
{
	const char*			m_szName;
	int					m_nMode;
	A3DSHADER			m_Shader;
};

int gfx_get_obj_count();
const GFX_OBJ_INFO* gfx_obj_info(int nIndex);
const GFX_OBJ_INFO* gfx_obj_info_by_type(int nTypeId);
const GFX_OBJ_INFO* gfx_obj_check(const GFX_OBJ_INFO* pInfo);

const GFX_PROP_INFO * gfx_prop_info(const GFX_PROP_INFO *pPropInfoSet, int nIndex);
const GFX_PROP_INFO * gfx_prop_info_by_name(const GFX_PROP_INFO *pPropInfoSet, int nCount, const AString &name);

const GFX_PARAM_INFO* gfx_param_info(int nTypeId);
const GFX_PROP_INFO* gfx_param_sub_info(int nTypeId, int nParamId);

int gfx_ctrl_method_count();
const GFX_OBJ_INFO* gfx_method_info(int nIndex);
const GFX_OBJ_INFO* gfx_method_info_by_type(int nTypeID);
const GFX_OBJ_INFO* gfx_method_info_by_name(const AString & name);

int gfx_interp_type_count();
const GFX_INTERP_INFO* gfx_interp_info(int nIndex);

int gfx_shader_count();
const GFX_SHADER_INFO* gfx_shader_info(int nIndex);

const A3DSHADER& gfx_shader_from_mode(int nMode);
int gfx_mode_from_shader(const A3DSHADER& sh);

const GFX_PROP_INFO * gfx_kp_prop_info_set();
int gfx_size_of_kp_prop_set();

bool gfx_is_based_on_physx(int nTypeId);

#endif
