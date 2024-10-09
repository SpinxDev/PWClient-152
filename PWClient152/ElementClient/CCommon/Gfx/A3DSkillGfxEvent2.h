#ifndef _A3DSKILLGFXEVENT2_H_
#define _A3DSKILLGFXEVENT2_H_

#include "A3DGFXExMan.h"

enum GfxMoveMode
{
	enumLinearMove = 0,		// 直线
	enumParabolicMove,		// 抛物线
	enumMissileMove,		// 导弹
	enumMeteoricMove,		// 流星雨
	enumHelixMove,			// 螺旋线
	enumCurvedMove,			// 香蕉线
	enumAccMove,			// 加速
	enumOnTarget,			// 定点
	enumLink,				// 链接
	enumRandMove,			// 随机移动
	enumMoveModeNum
};

enum EmitShape
{
	enumBox = 0,
	enumSphere,
	enumCylinder,
	enumShapeNum
};

enum GfxHitPos
{
	enumHitCenter,
	enumHitBottom
};

struct GFX_SKILL_PARAM
{
	union
	{
		bool	bVal;
		int		nVal;
		float	fVal;
	};

	bool		m_bArea;
	EmitShape	m_Shape;
	A3DVECTOR3	m_vSize;
};

enum GfxSkillValType
{
	enumGfxSkillBool,
	enumGfxSkillInt,
	enumGfxSkillFloat,
	enumGfxSkillValNum
};

class CGfxMoveBase
{
protected:
	CGfxMoveBase(GfxMoveMode mode) : m_Mode(mode), m_HitPos(enumHitCenter) {}

public:
	virtual ~CGfxMoveBase() {}

protected:
	GfxMoveMode	m_Mode;
	GfxHitPos	m_HitPos;
	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vMoveDir;
	bool		m_bOneOfCluser;
	DWORD		m_dwMaxFlyTime;

	bool		m_bReverse;
	// rand area
	bool		m_bArea;
	EmitShape	m_Shape;
	A3DVECTOR3	m_vSize;
	A3DVECTOR3	m_vXRange;
	A3DVECTOR3	m_vYRange;
	A3DVECTOR3	m_vZRange;
	float		m_fSquare;
	float		m_fSquareH;

protected:
	void CalcRange(const A3DVECTOR3& vDir)
	{
		m_vYRange = _unit_y;
		m_vZRange.Set(vDir.x, 0, vDir.z);
		if (m_vZRange.Normalize() < .01f) m_vZRange = _unit_z;
		m_vXRange = CrossProduct(m_vYRange, m_vZRange);
		m_vXRange *= m_vSize.x;
		m_vYRange *= m_vSize.y;
		m_vZRange *= m_vSize.z;
		m_fSquare = SquareMagnitude(m_vSize);
		m_fSquareH = m_vSize.x * m_vSize.x + m_vSize.z * m_vSize.z;
	}

	A3DVECTOR3 GetRandOff() const
	{
		if (m_Shape == enumBox)
		{
			A3DVECTOR3 xOff, yOff, zOff;
			xOff = _SymmetricRandom() * m_vXRange;
			yOff = _SymmetricRandom() * m_vYRange;
			zOff = _SymmetricRandom() * m_vZRange;
			return xOff + yOff + zOff;
		}
		else if (m_Shape == enumSphere)
		{
			float x, y, z;

			while (true)
			{
				x = _SymmetricRandom();
				y = _SymmetricRandom();
				z = _SymmetricRandom();
				if (x * x + y * y + z * z <= 1.0f) break;
			}
			
			return x * m_vXRange + y * m_vYRange + z * m_vZRange;
		}
		else if (m_Shape == enumCylinder)
		{
			float x, y, z;
			y = _SymmetricRandom();

			while (true)
			{
				x = _SymmetricRandom();
				z = _SymmetricRandom();
				if (x * x + z * z <= 1.0f) break;
			}

			return x * m_vXRange + y * m_vYRange + z * m_vZRange;
		}
		else
			return _unit_zero;
	}

	bool IsPosInRange(const A3DVECTOR3& vPos) const
	{
		if (m_Shape == enumBox)
		{
			if (fabsf(vPos.x) > m_vSize.x) return false;
			if (fabsf(vPos.y) > m_vSize.y) return false;
			if (fabsf(vPos.z) > m_vSize.z) return false;
			return true;
		}
		else if (m_Shape == enumSphere)
			return SquareMagnitude(vPos) <= m_fSquare;
		else if (m_Shape == enumCylinder)
		{
			if (fabsf(vPos.y) > m_vSize.y) return false;
			if (vPos.x * vPos.x + vPos.z * vPos.z > m_fSquareH) return false;
			return true;
		}

		return true;
	}

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget) = 0;
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos) = 0;
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		m_bArea		= param->m_bArea;
		m_Shape		= param->m_Shape;
		m_vSize		= param->m_vSize;
	}
	virtual void SetIsCluster(bool bCluster) { m_bOneOfCluser = bCluster; }
	virtual void UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget) {}
	virtual void SetReverse(bool bReverse) { m_bReverse = bReverse; }

	void SetIsArea(bool bArea) { m_bArea = bArea; }
	void SetShape(EmitShape shape) { m_Shape = shape; }
	void SetRange(const A3DVECTOR3& vSize) { m_vSize = vSize; }
	GfxMoveMode GetMode() const { return m_Mode; }
	GfxHitPos GetHitPos() const { return m_HitPos; }
	const A3DVECTOR3& GetPos() const { return m_vPos; }
	const A3DVECTOR3& GetMoveDir() const { return m_vMoveDir; }
	bool IsReverse() const { return m_bReverse; }
	void SetMaxFlyTime(DWORD dwTime) { m_dwMaxFlyTime = dwTime; if(m_dwMaxFlyTime==0) m_dwMaxFlyTime = 1;}
	static CGfxMoveBase* CreateMoveMethod(GfxMoveMode mode);
};

class CGfxLinearMove : public CGfxMoveBase
{
public:
	CGfxLinearMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxLinearMove() {}

protected:
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxParabolicMove : public CGfxMoveBase
{
public:
	CGfxParabolicMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxParabolicMove() {}

protected:
	A3DVECTOR3 m_vOrgPos;
	A3DVECTOR3 m_vHorzVel;
	float m_fVertVel;
	float m_fTotalTime;
	float m_fCurTime;
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxMissileMove : public CGfxMoveBase
{
public:
	CGfxMissileMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxMissileMove() {}

protected:
	A3DVECTOR3 m_vOrgPos;
	float m_fCurVel;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxMeteoricMove : public CGfxMoveBase
{
public:
	CGfxMeteoricMove(GfxMoveMode mode) : CGfxMoveBase(mode) { m_fRadius = 2.0f; }
	~CGfxMeteoricMove() {}

protected:
	A3DVECTOR3 m_vFallVel;
	float m_fRadius;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxHelixMove : public CGfxMoveBase
{
public:
	CGfxHelixMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_fRadius(0) {}
	~CGfxHelixMove() {}

protected:
	float m_fFlyTime;
	float m_fRadius;
	float m_fShrinkRate;
	float m_fAngle;
	float m_fSpeed;

	A3DVECTOR3 m_vCenter;
	A3DVECTOR3 m_vAxisX;
	A3DVECTOR3 m_vAxisY;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxCurvedMove : public CGfxMoveBase
{
public:
	CGfxCurvedMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_bFromRight(true) {}
	~CGfxCurvedMove() {}

protected:
	bool m_bFromRight;
	float m_fLatSpd;
	float m_fTime;
	float m_fSpeed;
	A3DVECTOR3 m_vAxisX;
	A3DVECTOR3 m_vAxisZ;
	A3DVECTOR3 m_vZSpd;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_bFromRight = param->bVal;
	}
};

class CGfxAccMove : public CGfxMoveBase
{
public:
	CGfxAccMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_fAcc(1.0f) {}
	~CGfxAccMove() {}

protected:
	float m_fAcc;
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fAcc = param->fVal;
	}
};

class CGfxOnTargetMove : public CGfxMoveBase
{
public:
	CGfxOnTargetMove(GfxMoveMode mode) : CGfxMoveBase(mode) { m_HitPos = enumHitBottom; m_fRadius = 0; }
	~CGfxOnTargetMove() {}

protected:
	float		m_fRadius;
	A3DVECTOR3	m_vOffset;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxLinkMove : public CGfxMoveBase
{
public:
	CGfxLinkMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxLinkMove()
	{
		for (size_t i = 0; i < m_Params.size(); i++)
			delete m_Params[i];
	}

protected:
	GfxParamList m_Params;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
};

class CGfxRandMove : public CGfxMoveBase
{
public:
	CGfxRandMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxRandMove() {}

protected:
	float		m_fStep;
	float		m_fSpeed;
	float		m_fTimeSpan;
	float		m_fCurSpan;
	A3DVECTOR3	m_CtrlPoints[6];

	void CalcStep();
	A3DVECTOR3 GetNextStep(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vCenter) const;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fSpeed = param->fVal;
	}
};

inline CGfxMoveBase* CGfxMoveBase::CreateMoveMethod(GfxMoveMode mode)
{
	switch(mode)
	{
	case enumLinearMove:
		return new CGfxLinearMove(mode);
	case enumParabolicMove:
		return new CGfxParabolicMove(mode);
	case enumMissileMove:
		return new CGfxMissileMove(mode);
	case enumMeteoricMove:
		return new CGfxMeteoricMove(mode);
	case enumHelixMove:
		return new CGfxHelixMove(mode);
	case enumCurvedMove:
		return new CGfxCurvedMove(mode);
	case enumAccMove:
		return new CGfxAccMove(mode);
	case enumOnTarget:
		return new CGfxOnTargetMove(mode);
	case enumLink:
		return new CGfxLinkMove(mode);
	case enumRandMove:
		return new CGfxRandMove(mode);
	}

	assert(false);
	return NULL;
}

extern A3DGFXExMan* AfxGetGFXExMan();

class A3DSkillGfxMan;
class A3DSkillGfxComposer;
class A3DSkillGfxEvent
{
public:
	A3DSkillGfxEvent(GfxMoveMode mode);
	virtual ~A3DSkillGfxEvent();

protected:

	friend class A3DSkillGfxMan;

	A3DSkillGfxComposer* m_pComposer;
	CGfxMoveBase*	m_pMoveMethod;
	A3DGFXEx*		m_pFlyGfx;			// 飞行效果
	A3DGFXEx*		m_pHitGfx;			// 击中效果
	DWORD			m_dwFlyTimeSpan;	// 飞行时间
	DWORD			m_dwCurSpan;
	DWORD			m_dwDelayTime;
	bool			m_bTraceTarget;
	bool			m_bFadeOut;
	clientid_t		m_nHostID;			// 发起者ID
	clientid_t		m_nTargetID;		// 目标ID
	DWORD			m_dwModifier;		// 攻击附加效果
	bool			m_bIsGoblinSkill;	// 是否为小精灵技能

	A3DVECTOR3		m_vHostPos;
	A3DVECTOR3		m_vTargetPos;
	A3DVECTOR3		m_vTargetDir;
	A3DVECTOR3		m_vTargetUp;
	bool			m_bHostExist;
	bool			m_bTargetExist;
	bool			m_bHitGfxInfinite;
	bool			m_bTargetDirAndUpExist;

	bool			m_bGfxUseLod;
	bool			m_bGfxDisableCamShake;
	bool			m_bHostECMCreatedByGfx;
	
	enum
	{
		enumWait,
		enumFlying,
		enumHit,
		enumFinished
	} m_enumState;

protected:
	// inline funcs
	void ReleaseFlyGfx()
	{
		if (m_pFlyGfx)
		{
			if (m_bFadeOut)
				AfxGetGFXExMan()->QueueFadeOutGfx(m_pFlyGfx, 1000);
			else
			{
				m_pFlyGfx->Release();
				delete m_pFlyGfx;
			}

			m_pFlyGfx = NULL;
		}
	}
	void ReleaseHitGfx()
	{
		if (m_pHitGfx)
		{
			AfxGetGFXExMan()->CacheReleasedGfx(m_pHitGfx);
			m_pHitGfx = NULL;
		}
	}
	void ReleaseGfx()
	{
		ReleaseFlyGfx();
		ReleaseHitGfx();
	}
	
	// virtual funcs
	virtual void HitTarget(const A3DVECTOR3& vTarget);

public:
	void SetGfxUseLod(bool b) { m_bGfxUseLod = b; }
	bool GetGfxUseLod() const { return m_bGfxUseLod; }
	void SetDisableCamShake(bool b) { m_bGfxDisableCamShake = b; }
	bool GetDisableCamShake() const { return m_bGfxDisableCamShake; }
	void SetHostModelCreatedByGfx(bool b) { m_bHostECMCreatedByGfx = b; }
	bool GetHostModelCreatedByGfx() const { return m_bHostECMCreatedByGfx; }
	void SetComposer(A3DSkillGfxComposer* pComposer) { m_pComposer = pComposer; }
	// inline funcs
	CGfxMoveBase* GetMoveMethod() { return m_pMoveMethod; }
	GfxMoveMode GetMode() const { return m_pMoveMethod->GetMode(); }
	GfxHitPos GetHitPos() const { return m_pMoveMethod->GetHitPos(); }
	A3DGFXEx* GetFlyGfx() { return m_pFlyGfx; }
	A3DGFXEx* GetHitGfx() { return m_pHitGfx; }
	void SetFlyTimeSpan(DWORD dwSpan) { m_dwFlyTimeSpan = dwSpan; }
	void SetDelay(DWORD dwDelay) { m_dwDelayTime = dwDelay; }
	void SetReverse(bool bReverse) const { m_pMoveMethod->SetReverse(bReverse); }
	void SetParam(const GFX_SKILL_PARAM* param) { m_pMoveMethod->SetParam(param); }
	void SetIsCluster(bool bCluster) { m_pMoveMethod->SetIsCluster(bCluster); }
	void SetTraceTarget(bool bTrace) { m_bTraceTarget = bTrace; }
	void SetFadeOut(bool bFadeOut) { m_bFadeOut = bFadeOut; }
	bool IsFinished() const { return m_enumState == enumFinished; }
	clientid_t GetHostID() const { return m_nHostID; }
	void SetHostID(clientid_t nID) { m_nHostID = nID; }
	clientid_t GetTargetID() const { return m_nTargetID; }
	void SetTargetID(clientid_t nID) { m_nTargetID = nID; }
	void SetHostPos(const A3DVECTOR3& vPos) { m_vHostPos = vPos; }
	void SetTargetPos(const A3DVECTOR3& vPos) { m_vTargetPos = vPos; }
	void SetHostExist(bool bExist) { m_bHostExist = bExist; }
	void SetTargetExist(bool bExist) { m_bTargetExist = bExist; }
	void SetModifier(DWORD dwModifier) { m_dwModifier = dwModifier; }
	void SetGoblinSkill(bool bGoblinSkill) { m_bIsGoblinSkill = bGoblinSkill; }
	bool GetGoblinSkill() { return m_bIsGoblinSkill; }
	void Resume()
	{
		ReleaseGfx();
		m_enumState = enumWait;
		m_dwCurSpan = 0;
	}

	// virtual funcs
	virtual A3DGFXEx* LoadFlyGfx(A3DDevice* pDev, const char* szPath) { return AfxGetGFXExMan()->LoadGfx(pDev, szPath); }
	virtual A3DGFXEx* LoadHitGfx(A3DDevice* pDev, const char* szPath) { return AfxGetGFXExMan()->LoadGfx(pDev, szPath); }
	virtual void SetFlyGfx(A3DGFXEx* pFlyGfx) { m_pFlyGfx = pFlyGfx; }
	virtual void SetHitGfx(A3DGFXEx* pHitGfx) { m_pHitGfx = pHitGfx; }
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Render()
	{
		if (m_pFlyGfx) AfxGetGFXExMan()->RegisterGfx(m_pFlyGfx);
		if (m_pHitGfx) AfxGetGFXExMan()->RegisterGfx(m_pHitGfx);
	}
	virtual A3DVECTOR3 GetTargetCenter() = 0;
	virtual bool GetTargetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) { return false; }
};


struct ECMODEL_GFX_PROPERTY
{
	bool bGfxUseLod;
	bool bGfxDisableCamShake;
	bool bHostECMCreatedByGfx;
};

class A3DSkillGfxMan
{
public:
	A3DSkillGfxMan() {}
	virtual ~A3DSkillGfxMan() {}

protected:

	friend class A3DSkillGfxEvent;
	A3DDevice* m_pDevice;

protected:
	bool AddOneSkillGfxEvent(
		A3DSkillGfxComposer* pComposer,
		clientid_t nHostID,
		clientid_t nTargetID,
		const char* szFlyGfx,
		GfxMoveMode mode,
		DWORD dwDelayTime,
		DWORD dwFlyTimeSpan,
		const char* szHitGfx,
		const GFX_SKILL_PARAM* param,
		bool bTraceTarget,
		float fFlyGfxScale,
		float fHitGfxScale,
		DWORD dwModifier,
		bool bCluster,
		bool bFadeOut,
		bool bIsGoblinSkill = false,
		bool bReverse = false
		);

	virtual A3DSkillGfxEvent* GetEmptyEvent(GfxMoveMode mode) = 0;
	virtual void PushEvent(A3DSkillGfxEvent* pEvent) = 0;
	virtual bool GetPropertyById(clientid_t nId, ECMODEL_GFX_PROPERTY* pProperty) = 0;

public:
	virtual float GetTargetScale(clientid_t nTargetId) = 0;
	void Init(A3DDevice* pDev) { m_pDevice = pDev; }
	bool AddSkillGfxEvent(
		A3DSkillGfxComposer* pComposer,
		clientid_t nHostID,
		clientid_t nTargetID,
		const char* szFlyGfx,
		const char* szHitGfx,
		DWORD dwFlyTimeSpan,
		bool bTraceTarget = true,
		GfxMoveMode FlyMode = enumLinearMove,
		int nFlyGfxCount = 1,
		DWORD dwInterval = 0,
		const GFX_SKILL_PARAM* param = NULL,
		float fFlyGfxScale = 1.0f,
		float fHitGfxScale = 1.0f,
		DWORD dwModifier = 0,
		bool bOnlyOneHit = true,
		bool bFadeOut = false,
		bool bIsGoblinSkill = false,
		bool bReverse = false
		);
};

#endif
