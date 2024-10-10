 /*
 * FILE: A3DGFXKeyPoint.h
 *
 * DESCRIPTION: GFX Key Point Data
 *
 * CREATED BY: ZhangYu, 2004/7/9
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXKEYPOINT_H_
#define _A3DGFXKEYPOINT_H_

#include "vector.h"
#include "A3DTypes.h"
#include "A3DObject.h"
#include "A3DFuncs.h"
#include "A3DGFXEditorInterface.h"
#include "AFile.h"
#include "APerlinNoise1D.h"

// keypoint properties
#define		ID_GFXOP_KP_POS					100
#define		ID_GFXOP_KP_DIR					101
#define		ID_GFXOP_KP_COLOR				102
#define		ID_GFXOP_KP_SCALE				103
#define		ID_GFXOP_KP_RAD2D				104
#define		ID_GFXOP_KP_INFINITE			105
#define		ID_GFXOP_KP_INTERP				106
#define		ID_GFXOP_KP_TIMESPAN			107

// control operation types
#define		ID_KPCTRL_NONE				-1
#define		ID_KPCTRL_MOVE				100
#define		ID_KPCTRL_ROT				101
#define		ID_KPCTRL_ROTAXIS			102
#define		ID_KPCTRL_REVOL				103
#define		ID_KPCTRL_CENTRI			104
#define		ID_KPCTRL_COLOR				105
#define		ID_KPCTRL_SCALE				106
#define		ID_KPCTRL_CL_NOISE			107
#define		ID_KPCTRL_CL_TRANS			108
#define		ID_KPCTRL_SCA_NOISE			109
#define		ID_KPCTRL_CURVE_MOVE		110
#define		ID_KPCTRL_SCALE_TRANS		111
#define		ID_KPCTRL_NOISE_BASE		112

// control operation parameters

#define		ID_KPCTRL_BASE_START_TM		20000
#define		ID_KPCTRL_BASE_END_TM		20001

#define		ID_KPCTRL_MOVE_DIR			10000
#define		ID_KPCTRL_MOVE_VEL			10001
#define		ID_KPCTRL_MOVE_ACC			10002

#define		ID_KPCTRL_ROT_VEL			10000
#define		ID_KPCTRL_ROT_ACC			10001

#define		ID_KPCTRL_ROTAXIS_POS		10000
#define		ID_KPCTRL_ROTAXIS_DIR		10001
#define		ID_KPCTRL_ROTAXIS_VEL		10002
#define		ID_KPCTRL_ROTAXIS_ACC		10003

#define		ID_KPCTRL_REVOL_POS			10000
#define		ID_KPCTRL_REVOL_DIR			10001
#define		ID_KPCTRL_REVOL_VEL			10002
#define		ID_KPCTRL_REVOL_ACC			10003

#define		ID_KPCTRL_CENTRI_POS		10000
#define		ID_KPCTRL_CENTRI_VEL		10001
#define		ID_CPCTRL_CENTRI_ACC		10002

#define		ID_KPCTRL_COLOR_R			10000
#define		ID_KPCTRL_COLOR_G			10001
#define		ID_KPCTRL_COLOR_B			10002
#define		ID_KPCTRL_COLOR_A			10003

#define		ID_KPCTRL_CLTRAN_ORG		10000
#define		ID_KPCTRL_CLTRAN_NUM		10001
#define		ID_KPCTRL_CLTRAN_DEST1		10002
#define		ID_KPCTRL_CLTRAN_DEST2		10003
#define		ID_KPCTRL_CLTRAN_DEST3		10004
#define		ID_KPCTRL_CLTRAN_DEST4		10005
#define		ID_KPCTRL_CLTRAN_DEST5		10006
#define		ID_KPCTRL_CLTRAN_TIME1		10007
#define		ID_KPCTRL_CLTRAN_TIME2		10008
#define		ID_KPCTRL_CLTRAN_TIME3		10009
#define		ID_KPCTRL_CLTRAN_TIME4		10010
#define		ID_KPCTRL_CLTRAN_TIME5		10011
#define		ID_KPCTRL_CLTRAN_DEST6		10012
#define		ID_KPCTRL_CLTRAN_DEST7		10013
#define		ID_KPCTRL_CLTRAN_DEST8		10014
#define		ID_KPCTRL_CLTRAN_DEST9		10015
#define		ID_KPCTRL_CLTRAN_DEST10		10016
#define		ID_KPCTRL_CLTRAN_TIME6		10017
#define		ID_KPCTRL_CLTRAN_TIME7		10018
#define		ID_KPCTRL_CLTRAN_TIME8		10019
#define		ID_KPCTRL_CLTRAN_TIME9		10020
#define		ID_KPCTRL_CLTRAN_TIME10		10021
#define		ID_KPCTLR_CLTRAN_ALPHAONLY	10022

#define		ID_KPCTRL_SCLTRAN_ORG		10000
#define		ID_KPCTRL_SCLTRAN_NUM		10001
#define		ID_KPCTRL_SCLTRAN_DEST1		10002
#define		ID_KPCTRL_SCLTRAN_DEST2		10003
#define		ID_KPCTRL_SCLTRAN_DEST3		10004
#define		ID_KPCTRL_SCLTRAN_DEST4		10005
#define		ID_KPCTRL_SCLTRAN_DEST5		10006
#define		ID_KPCTRL_SCLTRAN_TIME1		10007
#define		ID_KPCTRL_SCLTRAN_TIME2		10008
#define		ID_KPCTRL_SCLTRAN_TIME3		10009
#define		ID_KPCTRL_SCLTRAN_TIME4		10010
#define		ID_KPCTRL_SCLTRAN_TIME5		10011

#define		ID_KPCTRL_SCALE_DELTA		10000
#define		ID_KPCTRL_SCALE_MIN			10001
#define		ID_KPCTRL_SCALE_MAX			10002

#define		ID_KPCTRL_NOISE_BUFLEN		10000
#define		ID_KPCTRL_NOISE_AMP			10001
#define		ID_KPCTRL_NOISE_WAVLEN		10002
#define		ID_KPCTRL_NOISE_PERSIST		10003
#define		ID_KPCTRL_NOISE_OCTNUM		10004
#define		ID_KPCTRL_NOISE_BASE_COLOR	10005

// outer parameter
#define		ID_PARAM_TRAIL_POS1			100
#define		ID_PARAM_TRAIL_POS2			101
#define		ID_PARAM_TRAIL_MATRIX		102
#define		ID_PARAM_TRAIL_ORGMAT		103
#define		ID_PARAM_TRAIL_DELTATM		104
#define		ID_PARAM_TRAIL_TIMESPAN		105
#define		ID_PARAM_TRAIL_PARENTTM		106
#define		ID_PARAM_TRAIL_ADD_DATA		107
#define		ID_PARAM_TRAIL_LIFE 		108

#define		ID_PARAM_LTN_POS1			200
#define		ID_PARAM_LTN_POS2			201

#define		ID_PARAM_LTNTRAIL_POS1		300
#define		ID_PARAM_LTNTRAIL_POS2		301
#define		ID_PARAM_LTNTRAIL_MATRIX	302

#define		ID_PARAM_RIBBON_POS1		400
#define		ID_PARAM_RIBBON_POS2		401
#define		ID_PARAM_RIBBON_MATRIX		402

#define		VECTOR_XYZ(n) n.x,n.y,n.z
#define		VECTORADDR_XYZ(n) &n.x,&n.y,&n.z
#define		QUAT_XYZW(n) n.x,n.y,n.z,n.w
#define		QUATADDR_XYZW(n) &n.x,&n.y,&n.z,&n.w

#define		GFX_ALPHA_MASK	A3DCOLORRGBA(255, 255, 255, 0)
#define		SET_ALPHA(c, a) (((c) & GFX_ALPHA_MASK) + A3DCOLORRGBA(0, 0, 0, (a)))

template<class T>
inline void gfx_swap(T& v1, T& v2)
{
	T tmp = v1;
	v1 = v2;
	v2 = tmp;
}

inline void gfx_u_reverse(float fU[4])
{
	gfx_swap(fU[0], fU[2]);
	gfx_swap(fU[1], fU[3]);
}

inline void gfx_v_reverse(float fV[4])
{
	gfx_swap(fV[0], fV[1]);
	gfx_swap(fV[2], fV[3]);
}

inline void gfx_uv_interchange(float fU[4], float fV[4])
{
	gfx_swap(fU[0], fV[0]);
	gfx_swap(fU[1], fV[1]);
	gfx_swap(fU[2], fV[2]);
	gfx_swap(fU[3], fV[3]);
}

extern const A3DMATRIX4 _identity_mat4;
extern const A3DVECTOR3 _unit_x;
extern const A3DVECTOR3 _unit_y;
extern const A3DVECTOR3 _unit_z;
extern const A3DVECTOR3 _unit_zero;
extern const A3DQUATERNION _unit_dir;
extern const A3DCOLOR _white_diffuse;
extern const A3DCOLOR _white_spec;

const float _grnd_delta = (float)cos(DEG2RAD(5.0f));

inline bool CalcGrndNormQuat(const A3DVECTOR3& vNorm, A3DQUATERNION& qRot)
{
	float fAngle = DotProduct(vNorm, _unit_y);

	if (fabs(fAngle) > _grnd_delta)
		return false;

	A3DVECTOR3 vAxis = CrossProduct(_unit_y, vNorm);
	vAxis.Normalize();
	qRot.ConvertFromAxisAngle(vAxis, (float)acos(fAngle));
	return true;
}

struct KEY_POINT
{
	A3DVECTOR3		m_vPos;		// 位置
	A3DCOLOR		m_color;	// 颜色
	float			m_fScale;	// 比例
	// 3D
	A3DQUATERNION	m_vDir;
	// 2D
	float			m_fRad2D;	// 2D平面上的旋转角度

	inline KEY_POINT& Init();
	inline A3DCOLOR MultiplyAlpha(float fAlpha) const;
};

struct PROC_DATA
{
	KEY_POINT	m_KPProcess;
	A3DVECTOR3	m_vAxisOff;
	float		m_fTimeSpan;
	float		m_fScaleNoise;

	PROC_DATA() :
	m_vAxisOff(0),
	m_fTimeSpan(0),
	m_fScaleNoise(0) { m_KPProcess.Init(); }
};

inline bool operator == (const KEY_POINT& k1, const KEY_POINT& k2)
{
	return (k1.m_vPos == k2.m_vPos
		 && k1.m_color == k2.m_color
		 && k1.m_fScale == k2.m_fScale
		 && k1.m_vDir == k2.m_vDir
		 && k1.m_fRad2D == k2.m_fRad2D);
}

inline bool operator != (const KEY_POINT& k1, const KEY_POINT& k2)
{
	return !(k1 == k2);
}

#define	SET_VERT(a, b) { (a).x = (b).x; (a).y = (b).y; (a).z = (b).z; }

inline A3DMATRIX4 GetTranMatrix(const KEY_POINT& kp, const A3DVECTOR3& vAxis = _unit_z)
{
	A3DMATRIX4 matTran;
	if (kp.m_fRad2D) (kp.m_vDir * A3DQUATERNION(vAxis, kp.m_fRad2D)).ConvertToMatrix(matTran);
	else kp.m_vDir.ConvertToMatrix(matTran);
	matTran.SetRow(3, kp.m_vPos);
	return matTran;
}

inline void GetMatchGrndQuat(A3DQUATERNION& qWorld, const A3DVECTOR3& vNorm)
{
	A3DQUATERNION q;

	if (CalcGrndNormQuat(vNorm, q))
		qWorld = q * qWorld;
}

inline A3DQUATERNION QuatFromAxes(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	return A3DQUATERNION(a3d_TransformMatrix(vDir, vUp, _unit_zero));
}

inline A3DVECTOR3 RotateVec(const A3DQUATERNION& q, const A3DVECTOR3& v)
{
	A3DVECTOR3 uv, uuv;
	A3DVECTOR3 qvec(q.x, q.y, q.z);

	uv = CrossProduct(qvec, v); 
	uuv = CrossProduct(qvec, uv);
	uv *= (2.0f * q.w);
	uuv *= 2.0f;
		
	return v + uv + uuv;
}

inline A3DQUATERNION RotateQuat(const A3DQUATERNION& q, const A3DVECTOR3& v, float fRad)
{
	return A3DQUATERNION(RotateVec(q, v), fRad) * q;
}

inline void ToRotationMatrix(const A3DQUATERNION& q, A3DMATRIX4& rot)
{
    float fTx  = 2.0f*q.x;
    float fTy  = 2.0f*q.y;
    float fTz  = 2.0f*q.z;
    float fTwx = fTx*q.w;
    float fTwy = fTy*q.w;
    float fTwz = fTz*q.w;
    float fTxx = fTx*q.x;
    float fTxy = fTy*q.x;
    float fTxz = fTz*q.x;
    float fTyy = fTy*q.y;
    float fTyz = fTz*q.y;
    float fTzz = fTz*q.z;

    rot.m[0][0] = 1.0f-(fTyy+fTzz);
    rot.m[1][0] = fTxy-fTwz;
    rot.m[2][0] = fTxz+fTwy;
    rot.m[3][0] = 0.f;
    
	rot.m[0][1] = fTxy+fTwz;
    rot.m[1][1] = 1.0f-(fTxx+fTzz);
    rot.m[2][1] = fTyz-fTwx;
    rot.m[3][1] = 0.f;
    
	rot.m[0][2] = fTxz-fTwy;
    rot.m[1][2] = fTyz+fTwx;
    rot.m[2][2] = 1.0f-(fTxx+fTyy);
    rot.m[3][2] = 0.f;

    rot.m[0][3] = 0.f;
    rot.m[1][3] = 0.f;
    rot.m[2][3] = 0.f;
    rot.m[3][3] = 1.0f;
}

inline void FromRotationMatrix(A3DQUATERNION& q, const A3DMATRIX4& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float fTrace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];
    float fRoot;

    if (fTrace > 0.0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = (float)sqrt(fTrace + 1.0f);  // 2w
        q.w = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;  // 1/(4w)
        q.x = (rot.m[1][2] - rot.m[2][1]) * fRoot;
        q.y = (rot.m[2][0] - rot.m[0][2]) * fRoot;
        q.z = (rot.m[0][1] - rot.m[1][0]) * fRoot;
    }
    else
    {
        // |w| <= 1/2
        static size_t s_iNext[3] = { 1, 2, 0 };
        size_t i = 0;
        if ( rot.m[1][1] > rot.m[0][0] )
            i = 1;
        if ( rot.m[2][2] > rot.m[i][i] )
            i = 2;
 
		size_t j = s_iNext[i];
        size_t k = s_iNext[j];

        fRoot = (float)sqrt(rot.m[i][i] - rot.m[j][j] - rot.m[k][k] + 1.0f);
        float* apkQuat[3] = { &q.x, &q.y, &q.z };
        *apkQuat[i] = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;
        q.w = (rot.m[j][k] - rot.m[k][j]) * fRoot;
        *apkQuat[j] = (rot.m[i][j] + rot.m[j][i]) * fRoot;
        *apkQuat[k] = (rot.m[i][k] + rot.m[k][i]) * fRoot;
    }
}

inline A3DVECTOR3 CalcVertVec(const A3DVECTOR3& vDir)
{
	A3DVECTOR3 v = fabs(vDir.y) > .999f ? CrossProduct(vDir, _unit_x) : CrossProduct(_unit_y, vDir);
	v.Normalize();
	return v;
}

inline A3DVECTOR3 RandomDeviation(const A3DVECTOR3& vDir, float fAngle)
{
	A3DVECTOR3 vUp = CalcVertVec(vDir);
	A3DQUATERNION q(vDir, _UnitRandom() * A3D_2PI);
	vUp = RotateVec(q, vUp);
	q.ConvertFromAxisAngle(vUp, _UnitRandom() * fAngle);
	return RotateVec(q, vDir);
}

inline void Fill_TranMatrix(
	A3DMATRIX4& matTran,
	const A3DVECTOR3& vX,
	const A3DVECTOR3& vY,
	const A3DVECTOR3& vZ,
	const A3DVECTOR3& vPos)
{
	matTran.SetRow(0, vX);
	matTran.SetRow(1, vY);
	matTran.SetRow(2, vZ);
	matTran.SetRow(3, vPos);
	matTran.SetCol(3, _unit_zero);
	matTran._44 = 1.0f;
}

inline A3DMATRIX4 _build_matrix(const A3DVECTOR3& vDir, const A3DVECTOR3& vPos)
{
	A3DVECTOR3 vX, vY;

	if (fabs(vDir.y) > .999f)
	{
		vY = CrossProduct(vDir, _unit_x);
		vY.Normalize();
		vX = CrossProduct(vY, vDir);
	}
	else
	{
		vX = CrossProduct(_unit_y, vDir);
		vX.Normalize();
		vY = CrossProduct(vDir, vX);
	}

	A3DMATRIX4 matTran;
	Fill_TranMatrix(matTran, vX, vY, vDir, vPos);
	return matTran;
}

inline A3DCOLOR Interp_Color(const A3DCOLOR& r1, const A3DCOLOR& r2, float fPortion)
{
	float f = 1.0f - fPortion;

	return A3DCOLORRGBA(
		(int)(A3DCOLOR_GETRED(r1) * f + (A3DCOLOR_GETRED(r2) * fPortion)),
		(int)(A3DCOLOR_GETGREEN(r1) * f + A3DCOLOR_GETGREEN(r2) * fPortion),
		(int)(A3DCOLOR_GETBLUE(r1) * f + A3DCOLOR_GETBLUE(r2) * fPortion),
		(int)(A3DCOLOR_GETALPHA(r1) * f + A3DCOLOR_GETALPHA(r2) * fPortion)
		);
}

inline A3DVECTOR3 Interp_Position(const A3DVECTOR3& v1, const A3DVECTOR3& v2, float fPortion)
{
	float f = 1.0f - fPortion;
	return A3DVECTOR3(v1 * f + v2 * fPortion);
}

inline A3DVECTOR3 Bezier4(const A3DVECTOR3& p0, const A3DVECTOR3& p1, const A3DVECTOR3& p2, const A3DVECTOR3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	float rt = 1.f - t;
	float rt2 = rt * rt;
	float rt3 = rt2 * rt;

	return rt3 * p0 + (3.f * rt2 * t) * p1 + (3.f * rt * t2) * p2 + t3 * p3;
}

inline KEY_POINT& KEY_POINT::Init()
{
	m_vPos.Clear();
	m_vDir		=	_unit_dir;
	m_color		=	A3DCOLORRGBA(255, 255, 255, 255);
	m_fScale	=	1.0f;
	m_fRad2D	=	0.0f;
	return *this;
}

inline A3DCOLOR KEY_POINT::MultiplyAlpha(float fAlpha) const
{
	return (m_color & A3DCOLORRGBA(255, 255, 255, 0))
		 | A3DCOLORRGBA(0, 0, 0, DWORD(A3DCOLOR_GETALPHA(m_color) * fAlpha));
}

class A3DGFXKeyPoint;
class A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXKeyPointCtrlBase()
	{
		m_nType			= ID_KPCTRL_NONE;
		m_fStartTime	= 0;
		m_fEndTime		= -1;
		m_bHasAxis		= false;
	}
	virtual ~A3DGFXKeyPointCtrlBase() {}

protected:
	int		m_nType;
	float	m_fStartTime;
	float	m_fEndTime;
	bool	m_bHasAxis;

protected:
	bool SetBaseProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_BASE_START_TM:
			m_fStartTime = int(prop) / 1000.f;
			break;
		case ID_KPCTRL_BASE_END_TM:
			m_fEndTime = int(prop) / 1000.f;
			if (m_fEndTime < 0) m_fEndTime = -1.f;
			break;
		default:
			return false;
		}
		return true;
	}
	GFX_PROPERTY GetBaseProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_BASE_START_TM:
			return GFX_PROPERTY(int(m_fStartTime * 1000.f));
		case ID_KPCTRL_BASE_END_TM:
			if (m_fEndTime < 0) return GFX_PROPERTY(-1);
			else return GFX_PROPERTY(int(m_fEndTime * 1000.f));
		}
		return GFX_PROPERTY();
	}

public:
	friend class A3DGFXKeyPoint;
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan) = 0;
	virtual void Resume() = 0;
	virtual bool Save(AFile* pFile) = 0;
	virtual bool Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual void Update(A3DGFXKeyPoint* pKeyPoint) {}

public:
	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop) = 0;
	virtual GFX_PROPERTY GetProperty(int nOp) const = 0;
	float GetStartTm() const { return m_fStartTime; }
	float GetEndTm() const { return m_fEndTime; }
	bool Tick(float fTime) { return AfterStart(fTime) && BeforeEnd(fTime); }
	bool AfterStart(float fTime) const { return fTime >= m_fStartTime; };
	bool BeforeEnd(float fTime) const { return (m_fEndTime < 0 || fTime <= m_fEndTime); }

public:
	int GetType() const { return m_nType; }
	virtual bool SaveToFile(AFile* pFile);
	static A3DGFXKeyPointCtrlBase* CreateKPCtrl(int nType);
	static A3DGFXKeyPointCtrlBase* LoadFromFile(AFile* pFile, DWORD dwVersion);
	virtual A3DGFXKeyPointCtrlBase* Clone() const = 0;
	void CloneBase(const A3DGFXKeyPointCtrlBase* pSrc)
	{
		m_fStartTime	= pSrc->m_fStartTime;
		m_fEndTime		= pSrc->m_fEndTime;
	}
	bool HasAxis() const { return m_bHasAxis; }
	virtual const A3DVECTOR3& GetAxisCenter() const { return _unit_zero; }
	virtual void SetAxisOffset(const A3DVECTOR3& vOff) {}
};

typedef abase::vector<A3DGFXKeyPointCtrlBase*> CtrlList;

class A3DGFXKeyPoint : A3DObject
{
public:

	A3DGFXKeyPoint() : m_dwTimeSpan(0), m_enumMode(enumInterpNone)
	{
		m_KPOrg.Init();
	}

	virtual ~A3DGFXKeyPoint() { Release(); }

public:
	enum INTERPOLATE_MODE
	{
		enumInterpNone,
		enumInterpAll,
		enumInterpNoDir
	};

protected:
	KEY_POINT		m_KPOrg;
	DWORD			m_dwTimeSpan;
	INTERPOLATE_MODE m_enumMode;
	friend class A3DGFXKeyPointCtrlBase;
	friend class A3DGFXKeyPointSet;
	CtrlList m_CtrlLst;

protected:
	virtual void Resume() 
	{ 
		for (size_t i = 0; i < m_CtrlLst.size(); i++)
			m_CtrlLst[i]->Resume();
	}

public:
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch(nOp) 
		{
		case ID_GFXOP_KP_POS:
			return GFX_PROPERTY(GetKPOrg().m_vPos);
		case ID_GFXOP_KP_COLOR:
			return GFX_PROPERTY(GetKPOrg().m_color);
		case ID_GFXOP_KP_SCALE:
			return GFX_PROPERTY(GetKPOrg().m_fScale);
		case ID_GFXOP_KP_DIR:
			return GFX_PROPERTY(GetKPOrg().m_vDir);
		case ID_GFXOP_KP_RAD2D:
			return GFX_PROPERTY(GetKPOrg().m_fRad2D);
		case ID_GFXOP_KP_INFINITE:
			return GFX_PROPERTY(IsInfinite());
		case ID_GFXOP_KP_INTERP:
			return GFX_PROPERTY((int)GetInterpMode());
		case ID_GFXOP_KP_TIMESPAN:
			return GFX_PROPERTY(GetTimeSpan());
		}
		return GFX_PROPERTY();
	}
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		KEY_POINT kp = GetKPOrg();
		switch(nOp) 
		{
		case ID_GFXOP_KP_POS:
			kp.m_vPos = prop;
			break;
		case ID_GFXOP_KP_COLOR:
			kp.m_color = prop;
			break;
		case ID_GFXOP_KP_SCALE:
			kp.m_fScale = prop;
			break;
		case ID_GFXOP_KP_DIR:
			kp.m_vDir = prop;
			break;
		case ID_GFXOP_KP_RAD2D:
			kp.m_fRad2D = prop;
			break;
		case ID_GFXOP_KP_INFINITE:
			SetInfinite(prop);
			break;
		case ID_GFXOP_KP_INTERP:
			SetInterpMode((INTERPOLATE_MODE)(int)prop);
			break;
		case ID_GFXOP_KP_TIMESPAN:
			SetTimeSpan(prop);
			break;
		default:
			return false;
		}
		SetKPOrg(kp);
		return true;
	}


	virtual void Process(PROC_DATA& ProcData, int nTimeSpan);
	void SetData(const KEY_POINT& kp, DWORD dwTimeSpan = 2000) { m_KPOrg = kp; m_dwTimeSpan = dwTimeSpan; }
	void AddCtrlMethod(A3DGFXKeyPointCtrlBase* pCtrl) { m_CtrlLst.push_back(pCtrl);	}
	A3DGFXKeyPointCtrlBase* AddCtrlMethodByType(int nType);
	void RemoveCtrlMethod(A3DGFXKeyPointCtrlBase* pCtrl) 
	{ 
		for (size_t i = 0; i < m_CtrlLst.size(); i++)
		{
			if (m_CtrlLst[i] == pCtrl)
			{
				delete m_CtrlLst[i];
				m_CtrlLst.erase(m_CtrlLst.begin()+i);
				break;
			}
		}
	}
	int GetCtrlMethodCount() const { return static_cast<int>(m_CtrlLst.size()); }
	A3DGFXKeyPointCtrlBase* GetCtrlMethod(int nIndex) { return m_CtrlLst[nIndex]; }
	void GetCtrlMethodByType(int nType, CtrlList& Lst)
	{
		for (size_t i = 0; i < m_CtrlLst.size(); i++)
			if (nType == m_CtrlLst[i]->m_nType)
				Lst.push_back(m_CtrlLst[i]);
	}
	A3DGFXKeyPointCtrlBase* GetCtrlMethodByType(int nType)
	{
		for (size_t i = 0; i < m_CtrlLst.size(); i++)
			if (nType == m_CtrlLst[i]->m_nType)
				return m_CtrlLst[i];
		return NULL;
	}
	void SetInterpMode(INTERPOLATE_MODE mode) { m_enumMode = mode; }
	INTERPOLATE_MODE GetInterpMode() const { return m_enumMode;	}
	DWORD GetTimeSpan() const { return m_dwTimeSpan; }
	void SetTimeSpan(DWORD dwSpan) { m_dwTimeSpan = dwSpan; }
	void SetKPOrg(const KEY_POINT& kp) { m_KPOrg = kp; }
	const KEY_POINT& GetKPOrg() const { return m_KPOrg;	}
	bool IsInfinite() const { return m_dwTimeSpan == 0xffffffff; }
	void SetInfinite(bool bInfinite)
	{
		if (bInfinite) m_dwTimeSpan = 0xffffffff;
		else m_dwTimeSpan = 2000;
	}
	void Release()
	{
		for (size_t i = 0; i < m_CtrlLst.size(); i++)
			delete m_CtrlLst[i];
		m_CtrlLst.clear();
	}
	A3DGFXKeyPoint& operator= (const A3DGFXKeyPoint& src)
	{
		m_KPOrg			= src.m_KPOrg;
		m_dwTimeSpan	= src.m_dwTimeSpan;
		m_enumMode		= src.m_enumMode;

		for (size_t i = 0; i < src.m_CtrlLst.size(); i++)
			m_CtrlLst.push_back(src.m_CtrlLst[i]->Clone());

		Resume();
		return *this;
	}
	A3DGFXKeyPoint* Clone() const
	{
		A3DGFXKeyPoint* p = new A3DGFXKeyPoint();
		return &(*p = *this);
	}

public:
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
};

class A3DGFXCtrlMove : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlMove() : m_vMoveDir(0.0f)
	{
		m_nType		= ID_KPCTRL_MOVE;
		m_fVelOrg	= 0.0f;
		m_fAcc		= 0.0f;
	}
	virtual ~A3DGFXCtrlMove() {}

protected:
	A3DVECTOR3	m_vMoveDir;		// 中心点移动方向
	float		m_fVelOrg;		// 中心点移动速度
	float		m_fAcc;			// 中心点移动加速度

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_MOVE_DIR:
			m_vMoveDir = prop;
			m_vMoveDir.Normalize();
			break;
		case ID_KPCTRL_MOVE_VEL:
			m_fVelOrg = prop;
			break;
		case ID_KPCTRL_MOVE_ACC:
			m_fAcc = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_MOVE_DIR:
			return GFX_PROPERTY(m_vMoveDir);
		case ID_KPCTRL_MOVE_VEL:
			return GFX_PROPERTY(m_fVelOrg);
		case ID_KPCTRL_MOVE_ACC:
			return GFX_PROPERTY(m_fAcc);
		}
		return GetBaseProperty(nOp);
	}

	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlMove* p = new A3DGFXCtrlMove;
		return &(*p = *this);
	}

	A3DGFXCtrlMove& operator = (const A3DGFXCtrlMove& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_vMoveDir, src.m_fVelOrg, src.m_fAcc);
		return *this;
	}

public:
	void SetData(const A3DVECTOR3& dir, float fVel, float fAcc)
	{
		m_vMoveDir	= dir;
		m_fVelOrg	= fVel;
		m_fAcc		= fAcc;
	}
};

class A3DGFXCtrlRot : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlRot()	
	{ 
		m_nType = ID_KPCTRL_ROT; 
		m_fAngVelRot = 0.0f;
		m_fAngAccRot = 0.0f;
	}
	virtual ~A3DGFXCtrlRot() {}

protected:
	float		m_fAngVelRot;	// 自转角速度
	float		m_fAngAccRot;	// 自转角加速度

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_ROT_VEL:
			m_fAngVelRot = DEG2RAD((float)prop);
			break;
		case ID_KPCTRL_ROT_ACC:
			m_fAngAccRot = DEG2RAD((float)prop);
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_ROT_VEL:
			return GFX_PROPERTY(RAD2DEG(m_fAngVelRot));
		case ID_KPCTRL_ROT_ACC:
			return GFX_PROPERTY(RAD2DEG(m_fAngAccRot));
		default:
			break;
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlRot* p = new A3DGFXCtrlRot;
		return &(*p = *this);
	}

	A3DGFXCtrlRot& operator = (const A3DGFXCtrlRot& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_fAngVelRot, src.m_fAngAccRot);
		return *this;
	}

public:
	void SetData(float fVel, float fAcc)
	{
		m_fAngVelRot = fVel;
		m_fAngAccRot = fAcc;
	}
};

class A3DGFXCtrlRotAxis : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlRotAxis() : m_vPos(0.0f), m_vAxis(0.0f, 1.0f, 0.0f)
	{ 
		m_nType		= ID_KPCTRL_ROTAXIS;
		m_fVel		= 0.0f;
		m_fAcc		= 0.0f;
		m_bHasAxis	= true;
	}
	virtual ~A3DGFXCtrlRotAxis() {}

protected:
	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vAxis;
	float		m_fVel;
	float		m_fAcc;

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_ROTAXIS_POS:
			m_vPos = prop;
			break;
		case ID_KPCTRL_ROTAXIS_DIR:
			m_vAxis = prop;
			m_vAxis.Normalize();
			break;
		case ID_KPCTRL_ROTAXIS_VEL:
			m_fVel = DEG2RAD((float)prop);
			break;
		case ID_KPCTRL_ROTAXIS_ACC:
			m_fAcc = DEG2RAD((float)prop);
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_ROTAXIS_POS:
			return GFX_PROPERTY(m_vPos);
		case ID_KPCTRL_ROTAXIS_DIR:
			return GFX_PROPERTY(m_vAxis);
		case ID_KPCTRL_ROTAXIS_VEL:
			return GFX_PROPERTY(RAD2DEG(m_fVel));
		case ID_KPCTRL_ROTAXIS_ACC:
			return GFX_PROPERTY(RAD2DEG(m_fAcc));
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlRotAxis* p = new A3DGFXCtrlRotAxis;
		return &(*p = *this);
	}

	A3DGFXCtrlRotAxis& operator = (const A3DGFXCtrlRotAxis& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_vPos, src.m_vAxis, src.m_fVel, src.m_fAcc);
		return *this;
	}

public:
	void SetData(const A3DVECTOR3& vPos, const A3DVECTOR3& vAxis, float fVel, float fAcc)
	{
		m_vPos	= vPos;
		m_vAxis = vAxis;
		m_fVel	= fVel;
		m_fAcc	= fAcc;
	}
	const A3DVECTOR3& GetAxisCenter() const { return m_vPos; }
	void SetAxisOffset(const A3DVECTOR3& vOff) { m_vPos += vOff; }
};

class A3DGFXCtrlRevol : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlRevol() : m_vAxiRevol(0.0f, 1.0f, 0.0f), m_vPosRevol(0.0f)
	{ 
		m_nType			= ID_KPCTRL_REVOL;
		m_fAngVelRevol	= 0.0f;
		m_fAngAccRevol	= 0.0f;
		m_bHasAxis		= true;
	}
	virtual ~A3DGFXCtrlRevol() {}

protected:
	A3DVECTOR3	m_vAxiRevol;	// 公转轴
	A3DVECTOR3	m_vPosRevol;	// 公转轴原点
	float		m_fAngVelRevol;	// 公转速度
	float		m_fAngAccRevol;	// 公转角加速度

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_REVOL_POS:
			m_vPosRevol = prop;
			break;
		case ID_KPCTRL_REVOL_DIR:
			m_vAxiRevol = prop;
			m_vAxiRevol.Normalize();
			break;
		case ID_KPCTRL_REVOL_VEL:
			m_fAngVelRevol = DEG2RAD((float)prop);
			break;
		case ID_KPCTRL_REVOL_ACC:
			m_fAngAccRevol = DEG2RAD((float)prop);
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_REVOL_POS:
			return GFX_PROPERTY(m_vPosRevol);
		case ID_KPCTRL_REVOL_DIR:
			return GFX_PROPERTY(m_vAxiRevol);
		case ID_KPCTRL_REVOL_VEL:
			return (RAD2DEG(m_fAngVelRevol));
		case ID_KPCTRL_REVOL_ACC:
			return GFX_PROPERTY(RAD2DEG(m_fAngAccRevol));
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlRevol* p = new A3DGFXCtrlRevol;
		return &(*p = *this);
	}

	A3DGFXCtrlRevol& operator = (const A3DGFXCtrlRevol& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_vAxiRevol, src.m_vPosRevol, src.m_fAngVelRevol, src.m_fAngAccRevol);
		return *this;
	}

public:
	void SetData(const A3DVECTOR3& vAxi, const A3DVECTOR3& vPos, float fVel, float fAcc)
	{
		m_vAxiRevol		= vAxi;
		m_vPosRevol		= vPos;
		m_fAngVelRevol	= fVel;
		m_fAngAccRevol	= fAcc;
	}
	const A3DVECTOR3& GetAxisCenter() const { return m_vPosRevol; }
	void SetAxisOffset(const A3DVECTOR3& vOff) { m_vPosRevol += vOff; }
};

class A3DGFXCtrlCentriMove : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlCentriMove() : m_vCentPos(0.0f)
	{ 
		m_nType		= ID_KPCTRL_CENTRI;
		m_fVel		= 0.0f;
		m_fAcc		= 0.0f;
		m_bHasAxis	= true;
	}
	virtual ~A3DGFXCtrlCentriMove() {}

protected:
	A3DVECTOR3	m_vCentPos;
	float		m_fVel;
	float		m_fAcc;

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_CENTRI_POS:
			m_vCentPos = prop;
			break;
		case ID_KPCTRL_CENTRI_VEL:
			m_fVel = prop;
			break;
		case ID_CPCTRL_CENTRI_ACC:
			m_fAcc = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_CENTRI_POS:
			return GFX_PROPERTY(m_vCentPos);
		case ID_KPCTRL_CENTRI_VEL:
			return GFX_PROPERTY(m_fVel);
		case ID_CPCTRL_CENTRI_ACC:
			return GFX_PROPERTY(m_fAcc);
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlCentriMove* p = new A3DGFXCtrlCentriMove;
		return &(*p = *this);
	}

	A3DGFXCtrlCentriMove& operator = (const A3DGFXCtrlCentriMove& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_vCentPos, src.m_fVel, src.m_fAcc);
		return *this;
	}

public:
	void SetData(const A3DVECTOR3& vCentPos, float fVel, float fAcc)
	{
		m_vCentPos	= vCentPos;
		m_fVel		= fVel;
		m_fAcc		= fAcc;
	}
	const A3DVECTOR3& GetAxisCenter() const { return m_vCentPos; }
	void SetAxisOffset(const A3DVECTOR3& vOff) { m_vCentPos += vOff; }
};

class A3DGFXCtrlColorChange : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlColorChange() 
	{ 
		m_nType		= ID_KPCTRL_COLOR;
		for (int i = 0; i < 4; i++)
		{
			m_colorDelta[i] = 0;
			m_clRemain[i] = 0;
		}
	}
	virtual ~A3DGFXCtrlColorChange() {}

protected:
	int 		m_colorDelta[4];// 颜色改变量
	float		m_clRemain[4];

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() 
	{ 
		for (int i = 0; i < 4; i++)
			m_clRemain[i] = 0;
	}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_COLOR_R:
			m_colorDelta[0] = prop;
			break;
		case ID_KPCTRL_COLOR_G:
			m_colorDelta[1] = prop;
			break;
		case ID_KPCTRL_COLOR_B:
			m_colorDelta[2] = prop;
			break;
		case ID_KPCTRL_COLOR_A:
			m_colorDelta[3] = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_COLOR_R:
			return GFX_PROPERTY(m_colorDelta[0]);
		case ID_KPCTRL_COLOR_G:
			return GFX_PROPERTY(m_colorDelta[1]);
		case ID_KPCTRL_COLOR_B:
			return GFX_PROPERTY(m_colorDelta[2]);
		case ID_KPCTRL_COLOR_A:
			return GFX_PROPERTY(m_colorDelta[3]);
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlColorChange* p = new A3DGFXCtrlColorChange;
		return &(*p = *this);
	}

	A3DGFXCtrlColorChange& operator = (const A3DGFXCtrlColorChange& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_colorDelta[0], src.m_colorDelta[1], src.m_colorDelta[2], src.m_colorDelta[3]);
		return *this;
	}

public:
	void SetData(int R, int G, int B, int A)
	{
		m_colorDelta[0] = R;
		m_colorDelta[1] = G;
		m_colorDelta[2] = B;
		m_colorDelta[3] = A;
	}
};

const int _cl_lookup_tbl_len = 128;
const int _cl_tbl_max_num = 10;

class A3DGFXCtrlClTrans : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlClTrans() 
		: m_clOrg(A3DCOLORRGBA(255, 255, 255, 255))
		, m_bAlphaOnly(false)
	{
		m_nType = ID_KPCTRL_CL_TRANS;
		m_nDestNum = 1;

		for (int i = 0; i < _cl_tbl_max_num; i++)
		{
			m_clDest[i] = A3DCOLORRGBA(255, 255, 255, 255);
			m_fTransTime[i] = 100.0f;
		}

		m_fTotalSpan = m_fTransTime[0];
	}
	virtual ~A3DGFXCtrlClTrans() {}

protected:
	A3DCOLOR	m_clOrg;
	int			m_nDestNum;
	A3DCOLOR	m_clDest[_cl_tbl_max_num];
	float		m_fTransTime[_cl_tbl_max_num];
	float		m_fTotalSpan;
	bool		m_bAlphaOnly;

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Resume() {}

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_CLTRAN_ORG:
			m_clOrg = prop;
			break;
		case ID_KPCTRL_CLTRAN_NUM:
			m_nDestNum = prop;
			if (m_nDestNum > _cl_tbl_max_num)
				m_nDestNum = _cl_tbl_max_num;
			break;
		case ID_KPCTRL_CLTRAN_DEST1:
			m_clDest[0] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST2:
			m_clDest[1] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST3:
			m_clDest[2] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST4:
			m_clDest[3] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST5:
			m_clDest[4] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST6:
			m_clDest[5] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST7:
			m_clDest[6] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST8:
			m_clDest[7] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST9:
			m_clDest[8] = prop;
			break;
		case ID_KPCTRL_CLTRAN_DEST10:
			m_clDest[9] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME1:
			m_fTransTime[0] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME2:
			m_fTransTime[1] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME3:
			m_fTransTime[2] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME4:
			m_fTransTime[3] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME5:
			m_fTransTime[4] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME6:
			m_fTransTime[5] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME7:
			m_fTransTime[6] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME8:
			m_fTransTime[7] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME9:
			m_fTransTime[8] = prop;
			break;
		case ID_KPCTRL_CLTRAN_TIME10:
			m_fTransTime[9] = prop;
			break;
		case ID_KPCTLR_CLTRAN_ALPHAONLY:
			m_bAlphaOnly = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}

		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_CLTRAN_ORG:
			return GFX_PROPERTY(m_clOrg);
		case ID_KPCTRL_CLTRAN_NUM:
			return GFX_PROPERTY(m_nDestNum);
		case ID_KPCTRL_CLTRAN_DEST1:
			return GFX_PROPERTY(m_clDest[0]);
		case ID_KPCTRL_CLTRAN_DEST2:
			return GFX_PROPERTY(m_clDest[1]);
		case ID_KPCTRL_CLTRAN_DEST3:
			return GFX_PROPERTY(m_clDest[2]);
		case ID_KPCTRL_CLTRAN_DEST4:
			return GFX_PROPERTY(m_clDest[3]);
		case ID_KPCTRL_CLTRAN_DEST5:
			return GFX_PROPERTY(m_clDest[4]);
		case ID_KPCTRL_CLTRAN_DEST6:
			return GFX_PROPERTY(m_clDest[5]);
		case ID_KPCTRL_CLTRAN_DEST7:
			return GFX_PROPERTY(m_clDest[6]);
		case ID_KPCTRL_CLTRAN_DEST8:
			return GFX_PROPERTY(m_clDest[7]);
		case ID_KPCTRL_CLTRAN_DEST9:
			return GFX_PROPERTY(m_clDest[8]);
		case ID_KPCTRL_CLTRAN_DEST10:
			return GFX_PROPERTY(m_clDest[9]);
		case ID_KPCTRL_CLTRAN_TIME1:
			return GFX_PROPERTY(m_fTransTime[0]);
		case ID_KPCTRL_CLTRAN_TIME2:
			return GFX_PROPERTY(m_fTransTime[1]);
		case ID_KPCTRL_CLTRAN_TIME3:
			return GFX_PROPERTY(m_fTransTime[2]);
		case ID_KPCTRL_CLTRAN_TIME4:
			return GFX_PROPERTY(m_fTransTime[3]);
		case ID_KPCTRL_CLTRAN_TIME5:
			return GFX_PROPERTY(m_fTransTime[4]);
		case ID_KPCTRL_CLTRAN_TIME6:
			return GFX_PROPERTY(m_fTransTime[5]);
		case ID_KPCTRL_CLTRAN_TIME7:
			return GFX_PROPERTY(m_fTransTime[6]);
		case ID_KPCTRL_CLTRAN_TIME8:
			return GFX_PROPERTY(m_fTransTime[7]);
		case ID_KPCTRL_CLTRAN_TIME9:
			return GFX_PROPERTY(m_fTransTime[8]);
		case ID_KPCTRL_CLTRAN_TIME10:
			return GFX_PROPERTY(m_fTransTime[9]);
		case ID_KPCTLR_CLTRAN_ALPHAONLY:
			return GFX_PROPERTY(m_bAlphaOnly);
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlClTrans* p = new A3DGFXCtrlClTrans;
		return &(*p = *this);
	}

	A3DGFXCtrlClTrans& operator = (const A3DGFXCtrlClTrans& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		m_clOrg			= src.m_clOrg;
		m_nDestNum		= src.m_nDestNum;
		m_fTotalSpan	= src.m_fTotalSpan;
		m_bAlphaOnly	= src.m_bAlphaOnly;

		for (int i = 0; i < m_nDestNum; i++)
		{
			m_clDest[i] = src.m_clDest[i];
			m_fTransTime[i] = src.m_fTransTime[i];
		}

		return *this;
	}
};

const int _scl_dest_num = 5;

class A3DGFXCtrlScaleTrans : public A3DGFXKeyPointCtrlBase
{
public:
	
	A3DGFXCtrlScaleTrans();
	virtual ~A3DGFXCtrlScaleTrans() {}

protected:
	int		m_nDestNum;
	float	m_DestScales[_scl_dest_num + 1];
	float	m_fTransTime[_scl_dest_num];

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Resume() {}
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_SCLTRAN_ORG:
			m_DestScales[0] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_NUM:
			m_nDestNum = prop;
			if (m_nDestNum > _scl_dest_num) m_nDestNum = _scl_dest_num;
			break;
		case ID_KPCTRL_SCLTRAN_DEST1:
			m_DestScales[1] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_DEST2:
			m_DestScales[2] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_DEST3:
			m_DestScales[3] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_DEST4:
			m_DestScales[4] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_DEST5:
			m_DestScales[5] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_TIME1:
			m_fTransTime[0] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_TIME2:
			m_fTransTime[1] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_TIME3:
			m_fTransTime[2] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_TIME4:
			m_fTransTime[3] = prop;
			break;
		case ID_KPCTRL_SCLTRAN_TIME5:
			m_fTransTime[4] = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}

		return true;
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_SCLTRAN_ORG:
			return GFX_PROPERTY(m_DestScales[0]);
		case ID_KPCTRL_SCLTRAN_NUM:
			return GFX_PROPERTY(m_nDestNum);
		case ID_KPCTRL_SCLTRAN_DEST1:
			return GFX_PROPERTY(m_DestScales[1]);
		case ID_KPCTRL_SCLTRAN_DEST2:
			return GFX_PROPERTY(m_DestScales[2]);
		case ID_KPCTRL_SCLTRAN_DEST3:
			return GFX_PROPERTY(m_DestScales[3]);
		case ID_KPCTRL_SCLTRAN_DEST4:
			return GFX_PROPERTY(m_DestScales[4]);
		case ID_KPCTRL_SCLTRAN_DEST5:
			return GFX_PROPERTY(m_DestScales[5]);
		case ID_KPCTRL_SCLTRAN_TIME1:
			return GFX_PROPERTY(m_fTransTime[0]);
		case ID_KPCTRL_SCLTRAN_TIME2:
			return GFX_PROPERTY(m_fTransTime[1]);
		case ID_KPCTRL_SCLTRAN_TIME3:
			return GFX_PROPERTY(m_fTransTime[2]);
		case ID_KPCTRL_SCLTRAN_TIME4:
			return GFX_PROPERTY(m_fTransTime[3]);
		case ID_KPCTRL_SCLTRAN_TIME5:
			return GFX_PROPERTY(m_fTransTime[4]);
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlScaleTrans* p = new A3DGFXCtrlScaleTrans;
		return &(*p = *this);
	}

	A3DGFXCtrlScaleTrans& operator = (const A3DGFXCtrlScaleTrans& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		m_nDestNum = src.m_nDestNum;

		for (int i = 0; i < m_nDestNum; i++)
		{
			m_DestScales[i] = src.m_DestScales[i];
			m_fTransTime[i] = src.m_fTransTime[i];
		}

		m_DestScales[m_nDestNum] = src.m_DestScales[m_nDestNum];
		return *this;
	}
};

class A3DGFXCtrlScaleChange : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlScaleChange() 
	{
		m_nType			= ID_KPCTRL_SCALE;
		m_fScaleDelta	= 0.0f;
		m_fMinScale		= .01f;
		m_fMaxScale		= 9999.f;
	}
	virtual ~A3DGFXCtrlScaleChange() {}

protected:
	float	m_fScaleDelta;	// 比例改变量
	float	m_fMinScale;
	float	m_fMaxScale;

protected:
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Resume() {}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_SCALE_DELTA:
			m_fScaleDelta = prop;
			break;
		case ID_KPCTRL_SCALE_MIN:
			m_fMinScale = prop;
			break;
		case ID_KPCTRL_SCALE_MAX:
			m_fMaxScale = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}
		return true;
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_SCALE_DELTA:
			return GFX_PROPERTY(m_fScaleDelta);
		case ID_KPCTRL_SCALE_MIN:
			return GFX_PROPERTY(m_fMinScale);
		case ID_KPCTRL_SCALE_MAX:
			return GFX_PROPERTY(m_fMaxScale);
		}
		return GetBaseProperty(nOp);
	}
	
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlScaleChange* p = new A3DGFXCtrlScaleChange;
		return &(*p = *this);
	}

	A3DGFXCtrlScaleChange& operator = (const A3DGFXCtrlScaleChange& src)
	{
		if (&src == this)
			return *this;

		CloneBase(&src);
		SetData(src.m_fScaleDelta, src.m_fMinScale, src.m_fMaxScale);
		return *this;
	}

public:
	void SetData(float fScale, float fMin = .01f, float fMax = 9999.0f)
	{ 
		m_fScaleDelta	= fScale;
		m_fMinScale		= fMin;
		m_fMaxScale		= fMax;
	}
};

class A3DGFXCtrlNoiseBase : public A3DGFXKeyPointCtrlBase
{
public:
	A3DGFXCtrlNoiseBase()
	{
		m_nType			= ID_KPCTRL_NOISE_BASE;
		m_nBufLen		= 256;
		m_fAmplitude	= 2.0f;
		m_nWaveLen		= 1;
		m_fPersistence	= .25;
		m_nOctaveNum	= 5;
	}
	~A3DGFXCtrlNoiseBase() { m_PerlinNoise.Release(); }

protected:
	APerlinNoise1D	m_PerlinNoise;
	int				m_nBufLen;
	float			m_fAmplitude;
	int				m_nWaveLen;
	float			m_fPersistence;
	int				m_nOctaveNum;

public:
	virtual void Resume()
	{
	#ifndef _ANGELICA22
		m_PerlinNoise.Release();
	#endif
		m_PerlinNoise.Init(m_nBufLen, m_fAmplitude, m_nWaveLen, m_fPersistence, m_nOctaveNum, GetTickCount());
	}
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan) {}
	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlNoiseBase* p = new A3DGFXCtrlNoiseBase;
		p->_CloneBase(this);
		return p;
	}
	void _CloneBase(const A3DGFXCtrlNoiseBase* pSrc)
	{
		CloneBase(pSrc);
		m_nBufLen		= pSrc->m_nBufLen;
		m_fAmplitude	= pSrc->m_fAmplitude;
		m_nWaveLen		= pSrc->m_nWaveLen;
		m_fPersistence	= pSrc->m_fPersistence;
		m_nOctaveNum	= pSrc->m_nOctaveNum;
		Resume();
	}
	float GetNoiseVal(DWORD dwTotalSpan)
	{
		float fNoise = 0;
		m_PerlinNoise.GetValue((float)dwTotalSpan, &fNoise, 1);
		return fNoise;
	}

	// interfaces used for GFXEditor
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_NOISE_BUFLEN:
			m_nBufLen = prop;
			break;
		case ID_KPCTRL_NOISE_AMP:
			m_fAmplitude = prop;
			break;
		case ID_KPCTRL_NOISE_WAVLEN:
			m_nWaveLen = prop;
			break;
		case ID_KPCTRL_NOISE_PERSIST:
			m_fPersistence = prop;
			break;
		case ID_KPCTRL_NOISE_OCTNUM:
			m_nOctaveNum = prop;
			break;
		default:
			return SetBaseProperty(nOp, prop);
		}

		Resume();
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_NOISE_BUFLEN:
			return GFX_PROPERTY(m_nBufLen);
		case ID_KPCTRL_NOISE_AMP:
			return GFX_PROPERTY(m_fAmplitude);
		case ID_KPCTRL_NOISE_WAVLEN:
			return GFX_PROPERTY(m_nWaveLen);
		case ID_KPCTRL_NOISE_PERSIST:
			return GFX_PROPERTY(m_fPersistence);
		case ID_KPCTRL_NOISE_OCTNUM:
			return GFX_PROPERTY(m_nOctaveNum);
		}
		return GetBaseProperty(nOp);
	}
	
public:
	int GetBufLen() const { return m_nBufLen; }
	float GetAmplitude() const { return m_fAmplitude; }
	int GetWaveLen() const { return m_nWaveLen; }
	float GetPersistence() const { return m_fPersistence; }
	int GetOctaveNum() const { return m_nOctaveNum; }
	APerlinNoise1D& GetNoise() { return m_PerlinNoise; }
	void SetData(int nBufLen, float fAmp, int nWaveLen, float fPers, int nOctNum)
	{
		m_nBufLen		= nBufLen;
		m_fAmplitude	= fAmp;
		m_nWaveLen		= nWaveLen;
		m_fPersistence	= fPers;
		m_nOctaveNum	= nOctNum;
		Resume();
	}
};

class A3DGFXCtrlColorNoise : public A3DGFXCtrlNoiseBase
{
public:
	A3DGFXCtrlColorNoise() 
	{ 
		m_nType		= ID_KPCTRL_CL_NOISE; 
		m_clBase	= A3DCOLORRGBA(255, 255, 255, 255);
	}
	virtual ~A3DGFXCtrlColorNoise() {}

protected:
	A3DCOLOR	m_clBase;

protected:
	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);

	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlColorNoise* p = new A3DGFXCtrlColorNoise;
		return &(*p = *this);
	}

	A3DGFXCtrlColorNoise& operator = (const A3DGFXCtrlColorNoise& src)
	{
		if (&src == this)
			return *this;

		_CloneBase(&src);
		m_clBase = src.m_clBase;
		return *this;
	}

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_KPCTRL_NOISE_BASE_COLOR:
			m_clBase = prop;
			break;
		default:
			return A3DGFXCtrlNoiseBase::SetProperty(nOp, prop);
		}
		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_KPCTRL_NOISE_BASE_COLOR:
			return GFX_PROPERTY(m_clBase);
		}
		return A3DGFXCtrlNoiseBase::GetProperty(nOp);
	}
};

class A3DGFXCtrlScaleNoise : public A3DGFXCtrlNoiseBase
{
public:
	A3DGFXCtrlScaleNoise() { m_nType = ID_KPCTRL_SCA_NOISE; }
	virtual ~A3DGFXCtrlScaleNoise() {}

protected:
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);

	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlScaleNoise* p = new A3DGFXCtrlScaleNoise;
		return &(*p = *this);
	}

	A3DGFXCtrlScaleNoise& operator = (const A3DGFXCtrlScaleNoise& src)
	{
		if (&src == this)
			return *this;

		_CloneBase(&src);
		return *this;
	}
};

class A3DGFXCtrlCurveMove : public A3DGFXKeyPointCtrlBase
{
public:

	A3DGFXCtrlCurveMove()
	{
		m_nType		= ID_KPCTRL_CURVE_MOVE;
		m_pBezier	= NULL;
		m_pDir		= NULL;
		m_ulCount	= 0;
		m_fTimeSpan	= 0;
		m_ulCurPoint= 0;
		m_fTotalLen	= 0;
		m_bCalcDir	= false;
	}

	virtual ~A3DGFXCtrlCurveMove()
	{
		if (m_pBezier)
			delete[] m_pBezier;

		if (m_pDir)
			delete[] m_pDir;
	}

	struct BezierPt
	{
		A3DVECTOR3	m_vec;
		float		m_fRatio;
	};

protected:

	typedef abase::vector<A3DVECTOR3> VertArray;
	VertArray		m_VertArray;
	BezierPt*		m_pBezier;
	A3DQUATERNION*	m_pDir;
	size_t			m_ulCount;
	float			m_fTimeSpan;
	size_t			m_ulCurPoint;
	float			m_fTotalLen;
	bool			m_bCalcDir;

protected:

	virtual bool Save(AFile* pFile);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual void Process(PROC_DATA& ProcData, float fTimeSpan);
	virtual void Update(A3DGFXKeyPoint* pKeyPoint) { m_fTimeSpan = pKeyPoint->GetTimeSpan() / 1000.f; }

	virtual A3DGFXKeyPointCtrlBase* Clone() const
	{
		A3DGFXCtrlCurveMove* p = new A3DGFXCtrlCurveMove;
		return &(*p = *this);
	}

	A3DGFXCtrlCurveMove& operator = (const A3DGFXCtrlCurveMove& src)
	{
		if (&src == this)
			return *this;

		size_t i;
		for ( i = 0; i < src.m_VertArray.size(); i++)
			m_VertArray.push_back(src.m_VertArray[i]);

		if (src.m_pBezier)
		{
			m_pBezier = new BezierPt[src.m_ulCount];

			for (i = 0; i < src.m_ulCount; i++)
			{
				m_pBezier[i].m_vec = src.m_pBezier[i].m_vec;
				m_pBezier[i].m_fRatio = src.m_pBezier[i].m_fRatio;
			}
		}

		if (src.m_pDir)
		{
			m_pDir = new A3DQUATERNION[src.m_ulCount];
			memcpy(m_pDir, src.m_pDir, src.m_ulCount * sizeof(A3DQUATERNION));
		}

		m_ulCount = src.m_ulCount;
		m_fTotalLen = src.m_fTotalLen;	
		m_bCalcDir = src.m_bCalcDir;

		return *this;
	}

	virtual void Resume() { m_ulCurPoint = 0; }
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)	{ return true; }
	virtual GFX_PROPERTY GetProperty(int nOp) const	{ return GFX_PROPERTY(); }

public:

	void RemoveAllVerts() { m_VertArray.clear(); }
	void RemoveLastVert() { m_VertArray.pop_back(); }
	void RemoveVertAt(int nIndex) { m_VertArray.erase(m_VertArray.begin() + nIndex); }
	void AddVert(const A3DVECTOR3& vec) { m_VertArray.push_back(vec); }
	void AddVertAt(const A3DVECTOR3& vec, int nIndex) { m_VertArray.insert(m_VertArray.begin() + nIndex, vec); }
	size_t GetVertCount() const { return m_VertArray.size(); }
	A3DVECTOR3& GetVert(size_t uIndex) { return m_VertArray[uIndex]; }
	size_t GetBezierPtCount() const { return m_ulCount; }
	const BezierPt* GetBezierPt() const { return m_pBezier; }
	void GenPath();

	bool GetInterpPoint(float fRatio, A3DVECTOR3& vInterp, A3DQUATERNION& qIntDir, size_t& ulCurPoint)
	{
		if (m_ulCount < 2)
			return false;

		while (ulCurPoint + 1 < m_ulCount)
		{
			if (fRatio <= m_pBezier[ulCurPoint+1].m_fRatio)
				break;

			ulCurPoint++;
		}

		if (ulCurPoint + 1 >= m_ulCount)
		{
			vInterp = m_pBezier[m_ulCount-1].m_vec;

			if (m_pDir)
				qIntDir = m_pDir[m_ulCount-1];

			return true;
		}

		fRatio -= m_pBezier[ulCurPoint].m_fRatio;
		fRatio /= m_pBezier[ulCurPoint+1].m_fRatio - m_pBezier[ulCurPoint].m_fRatio;

		vInterp = m_pBezier[ulCurPoint].m_vec * (1.f - fRatio)
				+ m_pBezier[ulCurPoint+1].m_vec * fRatio;

		if (m_pDir)
			qIntDir = SLERPQuad(m_pDir[ulCurPoint], m_pDir[ulCurPoint+1], fRatio);

		return true;
	}

	float GetTotalLen() const { return m_fTotalLen; }
	bool GetCalcDir() const { return m_bCalcDir; }
	void SetCalcDir(bool b) { m_bCalcDir = b; }
};

class A3DGFXElement;
class A3DGFXKeyPointSet
{
public:
	A3DGFXKeyPointSet() :
	m_pEle(NULL),
	m_dwTimeStart(0),
	m_dwTimeSpan(0),
	m_nRepeatCount(1),
	m_nCurRepeat(0),
	m_dwRepeatDelay(0),
	m_dwCurDelay(0),
	m_nActKeyPoint(-1),
	m_bInDelay(false) {}

	virtual ~A3DGFXKeyPointSet() { Release();}

protected:
	A3DGFXElement* m_pEle;
	abase::vector<A3DGFXKeyPoint*> m_KeyPointArray;
	DWORD		m_dwTimeStart;
	DWORD		m_dwTimeSpan;
	int			m_nRepeatCount;
	int			m_nCurRepeat;
	DWORD		m_dwRepeatDelay;
	DWORD		m_dwCurDelay;
	int			m_nActKeyPoint;
	bool		m_bInDelay;
	PROC_DATA	m_ProcData;

protected:
	void Interpolate(float fPortion);

public:
	inline DWORD GetTimeElapsed() const { return m_dwTimeSpan < m_dwTimeStart ? 0 : m_dwTimeSpan - m_dwTimeStart; }
	void TickAnimation(DWORD dwTickTime);
	bool GetCurKeyPoint(KEY_POINT* pKeyPt)
	{
		if (m_nActKeyPoint < 0) return false;
		*pKeyPt = m_ProcData.m_KPProcess;
		pKeyPt->m_fScale += m_ProcData.m_fScaleNoise;
		return true;
	}

	void AddKeyPoint(A3DGFXKeyPoint* pKeyPoint) { m_KeyPointArray.push_back(pKeyPoint);	}
	void InsertKeyPoint(int index, A3DGFXKeyPoint* pKeyPoint) { m_KeyPointArray.insert(m_KeyPointArray.begin() + index, pKeyPoint); }

	void Release()
	{
		for (size_t i = 0; i < m_KeyPointArray.size(); i++)
			delete m_KeyPointArray[i];
		m_KeyPointArray.clear();
	}

	void RemoveKeyPoint(int nIndex)
	{
		delete m_KeyPointArray[nIndex];
		m_KeyPointArray.erase(m_KeyPointArray.begin()+nIndex);
	}

	void RemoveKeyPoint(A3DGFXKeyPoint* p)
	{
		for (size_t i = 0; i < m_KeyPointArray.size(); i++)
		{
			if (m_KeyPointArray[i] == p)
			{
				m_KeyPointArray.erase(m_KeyPointArray.begin()+i);
				break;
			}
		}
		delete p;
	}

	void Resume(bool bDelay = false)
	{
		m_nActKeyPoint	= -1;
		m_dwCurDelay	= 0;

		if (!bDelay)
		{
			m_bInDelay		= false;
			m_nCurRepeat	= 0;
			m_dwTimeSpan	= 0;
		}
		else
		{
			m_bInDelay		= true;
			if (m_nRepeatCount != -1) m_nCurRepeat++;
			m_dwTimeSpan	= m_dwTimeStart;
		}

		if (m_KeyPointArray.size())
			m_ProcData.m_KPProcess = m_KeyPointArray[0]->m_KPOrg;
		else
			m_ProcData.m_KPProcess.Init();
		
		m_ProcData.m_vAxisOff.Clear();
		m_ProcData.m_fTimeSpan = 0;
		m_ProcData.m_fScaleNoise = 0;

		for (size_t i = 0; i < m_KeyPointArray.size(); i++)
			m_KeyPointArray[i]->Resume();
	}

	void SetEle(A3DGFXElement* pEle) { m_pEle = pEle; }
	int GetKeyPointCount() const { return (int)m_KeyPointArray.size(); }
	A3DGFXKeyPoint* GetKeyPoint(int nIndex) { return m_KeyPointArray[nIndex]; }
	DWORD GetTimeStart() const { return m_dwTimeStart; }
	void SetTimeStart(DWORD dwStart) { m_dwTimeStart = dwStart; }
	int GetRepeatCount() const { return m_nRepeatCount;	}
	void SetRepeatCount(int nCount) { m_nRepeatCount = nCount; }
	int GetRepeatDelay() const { return m_dwRepeatDelay; }
	void SetRepeatDelay(DWORD dwDelay) { m_dwRepeatDelay = dwDelay;	}
	bool IsFinished() const { return m_nRepeatCount != -1 && m_nCurRepeat == m_nRepeatCount; }
	bool IsActive() const { return m_nActKeyPoint != -1; }
	int GetActKeyPoint() const { return m_nActKeyPoint; }
	A3DGFXKeyPointSet& operator = (const A3DGFXKeyPointSet& src)
	{
		if (&src == this)
			return *this;

		Release();
		for (int i = 0; i < src.GetKeyPointCount(); i++)
			AddKeyPoint(src.m_KeyPointArray[i]->Clone());

		m_dwTimeStart	= src.m_dwTimeStart;
		m_dwTimeSpan	= src.m_dwTimeSpan;
		m_nRepeatCount	= src.m_nRepeatCount;
		m_dwRepeatDelay	= src.m_dwRepeatDelay;
		return *this;
	}
	bool IsInfinite() const
	{
		if (m_nRepeatCount == -1)
			return true;

		for (size_t i = 0; i < m_KeyPointArray.size(); i++)
			if (m_KeyPointArray[i]->IsInfinite())
				return true;

		return false;
	}

public:
	bool Save(AFile* pFile);
	bool Load(AFile* pFile, DWORD dwVersion);
};

#endif
