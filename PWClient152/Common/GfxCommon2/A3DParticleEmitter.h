/*
 * FILE: A3DParticleEmitter.h
 *
 * DESCRIPTION: Abstract interface for Particle Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/3
 *
 * HISTORY:
 *
 */

#ifndef _A3DPARTICLEEMITTER_H_
#define _A3DPARTICLEEMITTER_H_

#include "A3DTypes.h"
#include "A3DParticle.h"
#include "A3DGFXEditorInterface.h"
#include "AFile.h"
#include "A3DFuncs.h"

#define		EMITTER_TYPE_NONE				-1
#define		EMITTER_TYPE_POINT				100
#define		EMITTER_TYPE_BOX				101
#define		EMITTER_TYPE_MULTIPLANE			102
#define		EMITTER_TYPE_ELLIPSOID			103
#define		EMITTER_TYPE_CYLINDER			104
#define		EMITTER_TYPE_CURVE				105
#define		EMITTER_TYPE_SKELETON			106

// #define		ID_GFXOP_EMITTER_POS		200
#define		ID_GFXOP_EMITTER_RATE			201
// #define		ID_GFXOP_EMITTER_DIR		202
#define		ID_GFXOP_EMITTER_ANGLE			203
#define		ID_GFXOP_EMITTER_SPEED			204
#define		ID_GFXOP_EMITTER_ACC_DIR		205
#define		ID_GFXOP_EMITTER_ACC			206
#define		ID_GFXOP_EMITTER_TTL			207
#define		ID_GFXOP_EMITTER_COLOR_MIN		208
#define		ID_GFXOP_EMITTER_COLOR_MAX		209
#define		ID_GFXOP_EMITTER_SCALE_MIN		210
#define		ID_GFXOP_EMITTER_SCALE_MAX		211
#define		ID_GFXOP_EMITTER_ROT_MIN		212
#define		ID_GFXOP_EMITTER_ROT_MAX		213
#define		ID_GFXOP_EMITTER_AREA_SIZE		214
#define		ID_GFXOP_EMITTER_SURFACE		215
#define		ID_GFXOP_EMITTER_BIND			216
#define		ID_GFXOP_EMITTER_DRAG			217
#define		ID_GFXOP_EMITTER_PAR_ACC		218
#define		ID_GFXOP_EMITTER_CURVE_R		219
#define		ID_GFXOP_EMITTER_DRAG_POWER		220
#define		ID_GFXOP_EMITTER_REF_MOX		221
#define		ID_GFXOP_EMITTER_USE_NORMAL		222
#define		ID_GFXOP_EMITTER_PERP_TO_NORMAL	223
#define		ID_GFXOP_EMITTER_PARTICAL_INITDIR		224
#define		ID_GFXOP_EMITTER_CURVE_ISAVG			225
#define		ID_GFXOP_EMITTER_CURVE_AVGSEGNUM		226
#define		ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY 227
#define		ID_GFXOP_EMITTER_COLOR_HSVINTERP		228
#define		ID_GFXOP_EMITTER_ELLIPSOID_ISAVG		229
#define		ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG		230
#define		ID_GFXOP_EMITTER_ELLIPSOID_BETASEG		231
#define		ID_GFXOP_EMITTER_CYLINDER_ISAVG			232
#define		ID_GFXOP_EMITTER_CYLINDER_ALPHASEG		233
#define		ID_GFXOP_EMITTER_CYLINDER_BETASEG		234
#define		ID_GFXOP_EMITTER_REF_SMD		235
#define		ID_GFXOP_EMITTER_SMD_SKIN_INDEX		236
#define		ID_GFXOP_EMITTER_SMD_SKINMESH_INDEX		237

class A3DParticleSystemEx;

class A3DParticleEmitter
{
public:
	A3DParticleEmitter();
	virtual ~A3DParticleEmitter();

protected:
	// 发射器类型
	int				m_nType;
	// 粒子喷射数率
	float			m_fEmissionRate;
	float			m_fRemainder;
	// 沿方向上偏转的范围
	float			m_fAngle;
	// 粒子速度
	float			m_fSpeed;
	// 粒子自身加速度
	float			m_fParAcc;
	// 加速度方向
	A3DVECTOR3		m_vAccDir;
	// 加速度
	float			m_fAcc;
	// 粒子生命期, Time to live
	float			m_fTTL;
	// 粒子色彩
	A3DCOLOR		m_colorMin;
	A3DCOLOR		m_colorMax;
	// HSL色彩: 对应于A3DCOLOR的colorMin colorMax，仅为运行时数据，不做存储
	A3DHSVCOLORVALUE	m_hsvMin;
	A3DHSVCOLORVALUE	m_hsvMax;
	// 粒子比例
	float			m_fScaleMin;
	float			m_fScaleMax;
	// 粒子初始旋转角度
	float			m_fRotMin;
	float			m_fRotMax;
	// Parent位置
	A3DVECTOR3		m_vParentPos;
	// Parent方向
	A3DQUATERNION	m_qParentDir;
	// 是否仅在表面产生粒子
	bool			m_bSurface;
	bool			m_bBind;
	bool			m_bDrag;
	float			m_fDragIntensity;
	// 整体alpha
	float			m_fEmitterAlpha;
	// 发射器本身的关键帧
	A3DGFXKeyPoint* m_pKeyPoint;
	
	// 粒子自身朝向 
	typedef	 A3DVECTOR3	EULERANGLE3;
	EULERANGLE3		m_vParticalInitDir;
	A3DQUATERNION	m_qParticleDir;

	// 是否使用HSV插值 
	// true : Use HSV interpolation
	// false : Use RGB interpolation
	bool		m_bUseHSVInterpolation;

	// Particle system
	A3DParticleSystemEx* m_pParticleSystem;

public:
	// 产生方向
	virtual void GenDirection(A3DVECTOR3& vDir);
	// 产生粒子生命期
	virtual float GenTTL();
	// 产生粒子色彩
	virtual A3DCOLOR GenColor();
	// 粒子产生数目
	virtual int GenEmissionCount(float fTimeSpan);
	// 产生粒子比例
	virtual float GenScale();
	// 粒子初始角度
	virtual float GenRotAngle();

public:
	static A3DParticleEmitter* CreateEmitter(int nType);
	virtual int GetEmissionCount(float fTimeSpan) = 0;
	virtual void InitParticle(A3DParticle* pParticle)
	{
		pParticle->m_fVelAlongAcc	= 0;
		pParticle->m_fScaleNoise	= 0;
		pParticle->m_color			= GenColor();
		pParticle->m_fScale			= GenScale();
		pParticle->m_fRotAngle		= GenRotAngle();
		pParticle->m_fSelfVel		= m_fSpeed;
		pParticle->m_vDir			= _unit_dir;
		pParticle->m_vAxisOff.Clear();
	}
	virtual bool IsParticlePosAbs() const { return !IsBind(); }
	void SetScale(float fMin, float fMax) { m_fScaleMin = fMin; m_fScaleMax = fMax; }
	float GetScaleMin() const { return m_fScaleMin; }
	float GetScaleMax() const { return m_fScaleMax; }
	void SetParentDirAndPos(const A3DVECTOR3& vPos, const A3DQUATERNION& qDir)
	{
		m_vParentPos = vPos;
		m_qParentDir = qDir;
		calcParticleDir();
	}
	virtual void UpdateEmitterScale(float fScale) {}
	virtual int CalcPoolSize() const { return (int)(m_fTTL * m_fEmissionRate) + 1; }
	virtual void CloneFrom(const A3DParticleEmitter* p);
	virtual void OnStart(A3DParticleSystemEx* pParticleSystem);
	virtual void OnTick(A3DParticleSystemEx* pParticleSystem) {}
	float GetSpeed() const { return m_fSpeed; }
	float GetParAcc() const { return m_fParAcc; }
	A3DVECTOR3 GetAccDir() const { return m_vAccDir; }
	float GetAcc() const { return m_fAcc; }
	bool IsBind() const { return m_bBind; }
	bool IsDrag() const { return m_bDrag; }
	float GetDragIntensity() const { return m_fDragIntensity; }
	void UpdateEmitterAlpha(float fAlpha) { m_fEmitterAlpha = fAlpha; }
	void SetEmissionRate(float fRate) { m_fEmissionRate = fRate; }
	void SetEmissionAng(float fAngle) { m_fAngle = fAngle; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetTTL(float fTTL) { m_fTTL = fTTL; }
	A3DGFXKeyPoint* GetKeyPoint() { return m_pKeyPoint; }

public:
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_RATE:
			m_fEmissionRate = prop;
			break;
		case ID_GFXOP_EMITTER_ANGLE:
			m_fAngle = DEG2RAD((float)prop);
			break;
		case ID_GFXOP_EMITTER_SPEED:
			m_fSpeed = prop;
			break;
		case ID_GFXOP_EMITTER_PAR_ACC:
			m_fParAcc = prop;
			break;
		case ID_GFXOP_EMITTER_ACC_DIR:
			m_vAccDir = prop;
			m_vAccDir.Normalize();
			break;
		case ID_GFXOP_EMITTER_ACC:
			m_fAcc = prop;
			break;
		case ID_GFXOP_EMITTER_TTL:
			m_fTTL = prop;
			m_fTTL /= 1000.f;
			break;
		case ID_GFXOP_EMITTER_COLOR_MIN:
			m_colorMin = prop;
			a3d_RGBToHSV(m_colorMin, &m_hsvMin);
			break;
		case ID_GFXOP_EMITTER_COLOR_MAX:
			m_colorMax = prop;
			a3d_RGBToHSV(m_colorMax, &m_hsvMax);
			break;
		case ID_GFXOP_EMITTER_SCALE_MIN:
			m_fScaleMin = prop;
			break;
		case ID_GFXOP_EMITTER_SCALE_MAX:
			m_fScaleMax = prop;
			break;
		case ID_GFXOP_EMITTER_ROT_MIN:
			m_fRotMin = DEG2RAD((float)prop);
			break;
		case ID_GFXOP_EMITTER_ROT_MAX:
			m_fRotMax = DEG2RAD((float)prop);
			break;
		case ID_GFXOP_EMITTER_SURFACE:
			m_bSurface = prop;
			break;
		case ID_GFXOP_EMITTER_BIND:
			m_bBind = prop;
			break;
		case ID_GFXOP_EMITTER_DRAG:
			m_bDrag = prop;
			break;
		case ID_GFXOP_EMITTER_DRAG_POWER:
			m_fDragIntensity = prop;
			break;
		case ID_GFXOP_EMITTER_PARTICAL_INITDIR:
			{
				A3DVECTOR3 vTmp = (A3DVECTOR3)prop;
				m_vParticalInitDir.Set(DEG2RAD(vTmp.x), DEG2RAD(vTmp.y), DEG2RAD(vTmp.z));
				calcParticleDir();
			}
			break;
		case ID_GFXOP_EMITTER_COLOR_HSVINTERP:
			m_bUseHSVInterpolation = prop;
			break;
		}
		return true;
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_RATE:
			return GFX_PROPERTY(m_fEmissionRate);
		case ID_GFXOP_EMITTER_ANGLE:
			return GFX_PROPERTY(RAD2DEG(m_fAngle));
		case ID_GFXOP_EMITTER_SPEED:
			return GFX_PROPERTY(m_fSpeed);
		case ID_GFXOP_EMITTER_PAR_ACC:
			return GFX_PROPERTY(m_fParAcc);
		case ID_GFXOP_EMITTER_ACC_DIR:
			return GFX_PROPERTY(m_vAccDir);
		case ID_GFXOP_EMITTER_ACC:
			return GFX_PROPERTY(m_fAcc);
		case ID_GFXOP_EMITTER_TTL:
			return GFX_PROPERTY(m_fTTL * 1000.f);
		case ID_GFXOP_EMITTER_COLOR_MIN:
			return GFX_PROPERTY(m_colorMin);
		case ID_GFXOP_EMITTER_COLOR_MAX:
			return GFX_PROPERTY(m_colorMax);
		case ID_GFXOP_EMITTER_SCALE_MIN:
			return GFX_PROPERTY(m_fScaleMin);
		case ID_GFXOP_EMITTER_SCALE_MAX:
			return GFX_PROPERTY(m_fScaleMax);
		case ID_GFXOP_EMITTER_ROT_MIN:
			return GFX_PROPERTY(RAD2DEG(m_fRotMin));
		case ID_GFXOP_EMITTER_ROT_MAX:
			return GFX_PROPERTY(RAD2DEG(m_fRotMax));
		case ID_GFXOP_EMITTER_SURFACE:
			return GFX_PROPERTY(m_bSurface);
		case ID_GFXOP_EMITTER_BIND:
			return GFX_PROPERTY(m_bBind);
		case ID_GFXOP_EMITTER_DRAG:
			return GFX_PROPERTY(m_bDrag);
		case ID_GFXOP_EMITTER_DRAG_POWER:
			return GFX_PROPERTY(m_fDragIntensity);
		case ID_GFXOP_EMITTER_PARTICAL_INITDIR:
			return GFX_PROPERTY(A3DVECTOR3(RAD2DEG(m_vParticalInitDir.x), RAD2DEG(m_vParticalInitDir.y), RAD2DEG(m_vParticalInitDir.z)));
		case ID_GFXOP_EMITTER_COLOR_HSVINTERP:
			return GFX_PROPERTY(m_bUseHSVInterpolation);
		}
		return GFX_PROPERTY();
	}

	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void Resume()
	{
		m_fRemainder = 0.0f;
	}

protected:
	inline void calcParticleDir()
	{ m_qParticleDir = m_qParentDir * A3DQUATERNION(m_vParticalInitDir.x, m_vParticalInitDir.y, m_vParticalInitDir.z); }
	inline void initParticlePosDirByBindAndDragProperty(A3DParticle* pParticle)
	{
		if (!m_bBind)
		{
			if (m_bDrag)
				pParticle->m_vOrgOffset = m_vParentPos;

			pParticle->m_vPos += m_vParentPos;
			pParticle->m_vDir = m_qParticleDir;
		}
	}
};

#endif
